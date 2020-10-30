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
src/common/log.cc \
src/lib/ele/net/socket_addr.cc \
src/lib/ele/base/timer_heap.cc \
src/lib/ele/net/io_device.cc \
src/lib/ele/net/io_service.cc \
src/lib/ele/net/tcp_socket.cc \


LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = 

LIB = \
-lpthread -lrt \

include mak/main.mak