TARGET = build/libcommon

SRCS = \
src/common/log.cc \

LINK_TYPE = static
INCLUDE = -Isrc
CPP_FLAG = -DYUT_BUILD_ENABLE_BASE_LOG
BUILD_DIR = build

include mak/main.mak