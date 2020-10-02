TARGET = bin/session_server
SRCS = \
src/utils/math.cc \
src/session_server/main.cc \

LINK_TYPE = exec
INCLUDE = -Isrc
BUILD_DIR = build

DEPFILE = \
build/libtinyxml2.a \

LIB = \
-Lbuild -ltinyxml2 \
-Llib-bin -lprotobuf \
-L/usr/lib64/mysql -lmysqlclient -lpthread -lrt \

include mak/main.mak
