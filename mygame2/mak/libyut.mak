TARGET = build/libyut

SRCS = \
src/yut/base/base_logger.cc \
src/yut/base/dynamic_buffer.cc \
src/yut/base/log_async_sink.cc \
src/yut/base/log_file_sink.cc \
src/yut/base/log_stderr_sink.cc \
src/yut/base/timer_heap.cc \
src/yut/base/timestamp.cc \
src/yut/base/log_core.cc \

LINK_TYPE = static
INCLUDE = -Isrc
CPP_FLAG = -DYUT_BUILD_ENABLE_BASE_LOG
BUILD_DIR = build

include mak/main.mak
