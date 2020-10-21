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

LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = 

LIB = \
-lpthread -lrt \

include mak/main.mak