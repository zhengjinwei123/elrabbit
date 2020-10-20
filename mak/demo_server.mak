TARGET = bin/demo_server1
SRCS = \
src/demo/server/server1.cc \

LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = 

LIB = \
-lpthread -lrt \

include mak/main.mak