# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/hgfs/myserver/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/hgfs/myserver/build/debug-cpp11

# Include any dependencies generated for this target.
include examples/test1/CMakeFiles/Demo.dir/depend.make

# Include the progress variables for this target.
include examples/test1/CMakeFiles/Demo.dir/progress.make

# Include the compile flags for this target's objects.
include examples/test1/CMakeFiles/Demo.dir/flags.make

examples/test1/CMakeFiles/Demo.dir/main.cc.o: examples/test1/CMakeFiles/Demo.dir/flags.make
examples/test1/CMakeFiles/Demo.dir/main.cc.o: /mnt/hgfs/myserver/server/examples/test1/main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/hgfs/myserver/build/debug-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/test1/CMakeFiles/Demo.dir/main.cc.o"
	cd /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Demo.dir/main.cc.o -c /mnt/hgfs/myserver/server/examples/test1/main.cc

examples/test1/CMakeFiles/Demo.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Demo.dir/main.cc.i"
	cd /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/hgfs/myserver/server/examples/test1/main.cc > CMakeFiles/Demo.dir/main.cc.i

examples/test1/CMakeFiles/Demo.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Demo.dir/main.cc.s"
	cd /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/hgfs/myserver/server/examples/test1/main.cc -o CMakeFiles/Demo.dir/main.cc.s

# Object files for target Demo
Demo_OBJECTS = \
"CMakeFiles/Demo.dir/main.cc.o"

# External object files for target Demo
Demo_EXTERNAL_OBJECTS =

bin/Demo: examples/test1/CMakeFiles/Demo.dir/main.cc.o
bin/Demo: examples/test1/CMakeFiles/Demo.dir/build.make
bin/Demo: examples/test1/CMakeFiles/Demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/hgfs/myserver/build/debug-cpp11/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/Demo"
	cd /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/test1/CMakeFiles/Demo.dir/build: bin/Demo

.PHONY : examples/test1/CMakeFiles/Demo.dir/build

examples/test1/CMakeFiles/Demo.dir/clean:
	cd /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 && $(CMAKE_COMMAND) -P CMakeFiles/Demo.dir/cmake_clean.cmake
.PHONY : examples/test1/CMakeFiles/Demo.dir/clean

examples/test1/CMakeFiles/Demo.dir/depend:
	cd /mnt/hgfs/myserver/build/debug-cpp11 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/hgfs/myserver/server /mnt/hgfs/myserver/server/examples/test1 /mnt/hgfs/myserver/build/debug-cpp11 /mnt/hgfs/myserver/build/debug-cpp11/examples/test1 /mnt/hgfs/myserver/build/debug-cpp11/examples/test1/CMakeFiles/Demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/test1/CMakeFiles/Demo.dir/depend

