#include <asio/windows/stream_handle.hpp>
#include <iostream>
#include <msgpack_rpc.h>

class Nvim {
  HANDLE _stdin_read = nullptr;
  HANDLE _stdin_write = nullptr;
  HANDLE _stdout_read = nullptr;
  HANDLE _stdout_write = nullptr;
  PROCESS_INFORMATION _process_info = {0};

  Nvim() {
    SECURITY_ATTRIBUTES sec_attribs = {0};
    sec_attribs.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec_attribs.bInheritHandle = true;

    CreatePipe(&_stdin_read, &_stdin_write, &sec_attribs, 0);
    CreatePipe(&_stdout_read, &_stdout_write, &sec_attribs, 0);
  }

public:
  ~Nvim() {
    DWORD exit_code;
    GetExitCodeProcess(_process_info.hProcess, &exit_code);

    if (exit_code == STILL_ACTIVE) {
      CloseHandle(this->_stdin_read);
      CloseHandle(this->_stdin_write);
      CloseHandle(this->_stdout_read);
      CloseHandle(this->_stdout_write);
      CloseHandle(this->_process_info.hThread);
      TerminateProcess(this->_process_info.hProcess, 0);
      CloseHandle(this->_process_info.hProcess);
    }
  }

  HANDLE WriteHandle() const { return _stdin_write; }
  HANDLE ReadHandle() const { return _stdout_read; }

  static std::shared_ptr<Nvim> Launch(std::wstring command) {

    auto nvim = std::shared_ptr<Nvim>(new Nvim());

    STARTUPINFOW startup_info = {0};
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = nvim->_stdin_read;
    startup_info.hStdOutput = nvim->_stdout_write;
    startup_info.hStdError = nvim->_stdout_write;

    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, true,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup_info,
                        &nvim->_process_info)) {
      return nullptr;
    }

    HANDLE job_object = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = {0};
    job_info.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(job_object, JobObjectExtendedLimitInformation,
                            &job_info, sizeof(job_info));
    AssignProcessToJobObject(job_object, nvim->_process_info.hProcess);

    return nvim;
  }
};

int main(int argc, char **argv) {

  auto nvim = Nvim::Launch(L"nvim --embed");
  if (!nvim) {
    return 1;
  }

  asio::io_context context;
  auto reader = asio::windows::stream_handle(context, nvim->ReadHandle());
  auto writer = asio::windows::stream_handle(context, nvim->WriteHandle());

  msgpack_rpc::rpc rpc;
  // rpc.attach(reader, writer);

  return 0;
}
