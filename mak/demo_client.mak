TARGET = bin/demo_client1
SRCS = \
src/demo/server/client1.cc \

LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = 

LIB = \
-lpthread -lrt \

include mak/main.mak