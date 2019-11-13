# Install script for directory: /mnt/hgfs/myserver/server/elrabbit/common

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/mnt/hgfs/myserver/build/debug-install-cpp11")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/hgfs/myserver/build/debug-cpp11/lib/libelrabbit_common.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/elrabbit/common" TYPE FILE FILES
    "/mnt/hgfs/myserver/server/elrabbit/common/atomic.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/class_util.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/condition.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/countdown_latch.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/current_thread.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/exception.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/file_util.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/log_file.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/log_stream.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/logging.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/mutex_lock.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/process_info.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/string_piece.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/thread.h"
    "/mnt/hgfs/myserver/server/elrabbit/common/timestamp.h"
    )
endif()

