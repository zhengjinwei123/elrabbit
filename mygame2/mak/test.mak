TARGET = bin/test
SRCS = \
src/test/main.cc \

LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = \
build/libyut.a \
build/libcommon.a \


LIB = \
-Lbuild -lcommon -lyut\
-L/usr/lib64/mysql -lmysqlclient -lpthread -lrt \

include mak/main.mak