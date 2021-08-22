#include <iostream>
#include <msgpack_rpc.h>
#include <windows_stream_transport.h>

auto STDINPIPE = L"\\\\.\\pipe\\nvim_stdin";
auto STDOUTPIPE = L"\\\\.\\pipe\\nvim_stdout";
auto BUFFERSIZE = 1024;

class Nvim {
  HANDLE _stdin_read = nullptr;
  HANDLE _stdin_write = nullptr;
  HANDLE _stdout_read = nullptr;
  HANDLE _stdout_write = nullptr;
  PROCESS_INFORMATION _process_info = {0};

  std::wstring m_command;

  Nvim(const wchar_t *command) : m_command(command) {
    SECURITY_ATTRIBUTES sec_attribs{.nLength = sizeof(SECURITY_ATTRIBUTES),
                                    .bInheritHandle = true};
    CreatePipe(&this->_stdin_read, &this->_stdin_write, &sec_attribs, 0);
    CreatePipe(&this->_stdout_read, &this->_stdout_write, &sec_attribs, 0);
  }

public:
  ~Nvim() {

    DWORD exit_code;
    GetExitCodeProcess(_process_info.hProcess, &exit_code);
    if (exit_code == STILL_ACTIVE) {
      CloseHandle(ReadHandle());
      CloseHandle(WriteHandle());
      TerminateProcess(this->_process_info.hProcess, 0);
      CloseHandle(this->_process_info.hProcess);
    }
  }

  wchar_t *Command() { return m_command.data(); }
  HANDLE ReadHandle() const { return _stdout_read; }
  HANDLE WriteHandle() const { return _stdin_write; }

  static std::shared_ptr<Nvim> Launch(const wchar_t *command) {

    auto nvim = std::shared_ptr<Nvim>(new Nvim(command));

    STARTUPINFOW startup_info = {0};
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = nvim->_stdin_read;
    startup_info.hStdOutput = nvim->_stdout_write;
    startup_info.hStdError = nvim->_stdout_write;

    if (!CreateProcessW(nullptr, nvim->Command(), nullptr, nullptr, true,
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
  asio::io_context::work work(context);

  msgpack_rpc::rpc_base<msgpack_rpc::WindowsStreamTransport> rpc;
  rpc.attach(msgpack_rpc::WindowsStreamTransport(nvim->ReadHandle(),
                                                 nvim->WriteHandle()));

  std::thread context_thead([&context]() { context.run(); });

  auto result = rpc.call("nvim_get_api_info").get();
  std::cout << msgpackpp::parser(result) << std::endl;

  context.stop();
  context_thead.join();

  return 0;
}
