#include <iostream>
#include <msgpack_rpc.h>
#include <windows_stream_transport.h>

auto STDINPIPE = L"\\\\.\\pipe\\nvim_stdin";
auto STDOUTPIPE = L"\\\\.\\pipe\\nvim_stdout";
auto BUFFERSIZE = 8192;
auto PIPE_TIMEOUT = 5000;

class NamedPipe {
  HANDLE _pipe = nullptr;
  std::vector<uint8_t> _outbuffer;
  std::vector<uint8_t> _inbuffer;

public:
  NamedPipe(const wchar_t *name, size_t size)
      : _outbuffer(size), _inbuffer(size) {
    SECURITY_ATTRIBUTES sec_attribs = {0};
    sec_attribs.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec_attribs.bInheritHandle = true;
    _pipe =
        CreateNamedPipeW(name,
                         PIPE_ACCESS_DUPLEX |        // read/write access
                             FILE_FLAG_OVERLAPPED,   // overlapped mode
                         PIPE_TYPE_MESSAGE |         // message-type pipe
                             PIPE_READMODE_MESSAGE | // message-read mode
                             PIPE_WAIT               // blocking mode
                         ,
                         2, (DWORD)_outbuffer.size(), (DWORD)_inbuffer.size(),
                         PIPE_TIMEOUT, &sec_attribs);
    assert(_pipe != INVALID_HANDLE_VALUE);
  }
  // ~NamedPipe() { CloseHandle(this->_pipe); }
  HANDLE Pipe() const { return _pipe; }
};

class Nvim {
  NamedPipe _stdin_write;
  NamedPipe _stdin_read;
  NamedPipe _stdout_write;
  NamedPipe _stdout_read;
  PROCESS_INFORMATION _process_info = {0};

  Nvim()
      : _stdin_write(STDINPIPE, BUFFERSIZE), _stdin_read(STDINPIPE, BUFFERSIZE),
        _stdout_write(STDOUTPIPE, BUFFERSIZE),
        _stdout_read(STDOUTPIPE, BUFFERSIZE) {}

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

  HANDLE ReadHandle() const { return _stdout_read.Pipe(); }
  HANDLE WriteHandle() const { return _stdin_write.Pipe(); }

  static std::shared_ptr<Nvim> Launch(std::wstring command) {

    auto nvim = std::shared_ptr<Nvim>(new Nvim());

    STARTUPINFOW startup_info = {0};
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = nvim->_stdin_read.Pipe();
    startup_info.hStdOutput = nvim->_stdout_write.Pipe();
    startup_info.hStdError = nvim->_stdout_write.Pipe();

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

  msgpack_rpc::rpc_base<msgpack_rpc::WindowsStreamTransport> rpc;
  rpc.attach(msgpack_rpc::WindowsStreamTransport(std::move(reader),
                                                 std::move(writer)));

  auto result = rpc.call("");

  context.run();

  return 0;
}
