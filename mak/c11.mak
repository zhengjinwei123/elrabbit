TARGET = bin/c11
SRCS = \
src/demo/server/c11.cc \
src/lib/ele/base/string_util.cc \
src/lib/ele/base/system.cc \
src/lib/ele/base/thread.cc \
src/lib/ele/base/thread_pool.cc \
src/lib/ele/base/exception.cc \
src/lib/ele/base/timestamp.cc \
src/lib/ele/base/dynamic_buffer.cc \
src/lib/ele/base/base_logger.cc \
src/lib/ele/base/log_async_sink.cc \
src/lib/ele/base/log_core.cc \
src/lib/ele/base/log_file_sink.cc \
src/lib/ele/base/log_stderr_sink.cc \
src/lib/ele/common/log.cc \
src/lib/ele/net/socket_addr.cc \
src/lib/ele/base/timer_heap.cc \
src/lib/ele/net/io_device.cc \
src/lib/ele/net/io_service.cc \
src/lib/ele/net/tcp_socket.cc \
src/lib/ele/net/tcp_service.cc \
src/lib/ele/net/self_pipe.cc \
src/lib/ele/net/signal_queue.cc \
src/lib/ele/exchange/base_struct.cc \
src/lib/ele/common/async_task.cc \
src/lib/ele/common/net_command.cc \
src/lib/ele/common/net_id.cc \
src/lib/ele/common/net_protocol.cc \
src/lib/ele/common/net_service.cc \
src/session_server/client_net_manager.cc \
src/session_server/server_app.cc \




LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = 

LIB = \
-lpthread -lrt \

include mak/main.mak