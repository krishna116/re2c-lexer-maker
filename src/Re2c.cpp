#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <csignal>

#include <uv.h>

#include "Re2c.h"

namespace impl{

struct EventLoopContext {
  uv_loop_t loop;
  uv_process_t proc;
  uv_pipe_t inPipe;   // child process stdin(in child process view).
  uv_pipe_t outPipe;  // child process stdout(in child process view).
  uv_pipe_t errPipe;  // child process stderr(in child process view).
  uv_write_t writeReq;

  std::string *out;
  std::string *err;

  const char *inData;
  size_t inSize;

  char rbuf[1024];

  int64_t exitStatus;
  int termSignal;
  bool stdinClosed;
};

static void allocCb(uv_handle_t *handle, size_t, uv_buf_t *buf) {
  auto *c = static_cast<EventLoopContext *>(handle->data);
  buf->base = c->rbuf;
  buf->len = static_cast<unsigned int>(sizeof(c->rbuf));
}

static void onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  auto *c = static_cast<EventLoopContext *>(stream->data);
  if (nread > 0) {
    std::string *sink = (stream == (uv_stream_t *)&c->outPipe) ? c->out : c->err;
    if (sink) sink->append(buf->base, static_cast<size_t>(nread));
    return;
  }
  if (nread == 0) return;
  uv_close(reinterpret_cast<uv_handle_t *>(stream), nullptr);   // UV_EOF
}

static void onWrite(uv_write_t *req, int status) {
  auto *c = static_cast<EventLoopContext *>(req->data);
  if (c->stdinClosed) return;
  (void)status;
  c->stdinClosed = true;
  uv_close(reinterpret_cast<uv_handle_t *>(&c->inPipe), nullptr);
}

static void onExit(uv_process_t *proc, int64_t exit_status, int term_signal) {
  auto *c = static_cast<EventLoopContext *>(proc->data);
  c->exitStatus = exit_status;
  c->termSignal = term_signal;
  uv_close(reinterpret_cast<uv_handle_t *>(proc), nullptr);
}

static void cleanup(EventLoopContext &c) {
  uv_close(reinterpret_cast<uv_handle_t *>(&c.inPipe), nullptr);
  uv_close(reinterpret_cast<uv_handle_t *>(&c.outPipe), nullptr);
  uv_close(reinterpret_cast<uv_handle_t *>(&c.errPipe), nullptr);
  uv_run(&c.loop, UV_RUN_DEFAULT);
  uv_loop_close(&c.loop);
}

static int runRe2c(const std::string &in, std::string &out, std::string *err) {
  if (in.empty()) return -1;

  out.clear();
  if (err) err->clear();

  EventLoopContext c;
  std::memset(&c, 0, sizeof(c));
  c.out = &out;
  c.err = err;
  c.inData = in.data();
  c.inSize = in.size();
  c.exitStatus = -1;
  c.termSignal = 0;
  c.stdinClosed = false;

  if (uv_loop_init(&c.loop) != 0) return -1;

  c.inPipe.data = &c;
  c.outPipe.data = &c;
  c.errPipe.data = &c;
  c.proc.data = &c;
  c.writeReq.data = &c;

  uv_pipe_init(&c.loop, &c.inPipe, 0);
  uv_pipe_init(&c.loop, &c.outPipe, 0);
  uv_pipe_init(&c.loop, &c.errPipe, 0);

  uv_stdio_container_t stdio[3];
  stdio[0].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_READABLE_PIPE);
  stdio[0].data.stream = reinterpret_cast<uv_stream_t *>(&c.inPipe);
  stdio[1].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  stdio[1].data.stream = reinterpret_cast<uv_stream_t *>(&c.outPipe);
  stdio[2].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
  stdio[2].data.stream = reinterpret_cast<uv_stream_t *>(&c.errPipe);

  std::vector<std::string> cmd{"re2c", "-"};
  char* args[] = {cmd[0].data(), cmd[1].data(), nullptr };

  uv_process_options_t opt;
  std::memset(&opt, 0, sizeof(opt));
  opt.exit_cb = onExit;
  opt.file = args[0];
  opt.args = args;
  opt.stdio_count = 3;
  opt.stdio = stdio;

  if (uv_spawn(&c.loop, &c.proc, &opt) != 0) {
    cleanup(c);
    return -1;
  }

  uv_read_start(reinterpret_cast<uv_stream_t *>(&c.outPipe), allocCb, onRead);
  uv_read_start(reinterpret_cast<uv_stream_t *>(&c.errPipe), allocCb, onRead);

  if (c.inSize > 0) {
    uv_buf_t buf = uv_buf_init(const_cast<char *>(c.inData), static_cast<unsigned int>(c.inSize));
    if (uv_write(&c.writeReq, reinterpret_cast<uv_stream_t *>(&c.inPipe), &buf, 1, onWrite) != 0) {
      c.stdinClosed = true;
      uv_close(reinterpret_cast<uv_handle_t *>(&c.inPipe), nullptr);
    }
  } else {
    c.stdinClosed = true;
    uv_close(reinterpret_cast<uv_handle_t *>(&c.inPipe), nullptr);
  }

  uv_run(&c.loop, UV_RUN_DEFAULT);
  uv_loop_close(&c.loop);

  if (c.termSignal != 0) return 128 + c.termSignal;
  return static_cast<int>(c.exitStatus);
}

}

int Re2c::run(const std::string& lexCode, std::string& cppCode){
  return impl::runRe2c(lexCode, cppCode, nullptr);
}