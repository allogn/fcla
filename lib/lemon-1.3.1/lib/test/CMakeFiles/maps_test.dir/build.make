# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_SOURCE_DIR = /home/alogins/fcla/lib/lemon-1.3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alogins/fcla/lib/lemon-1.3.1/lib

# Include any dependencies generated for this target.
include test/CMakeFiles/maps_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/maps_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/maps_test.dir/flags.make

test/CMakeFiles/maps_test.dir/maps_test.cc.o: test/CMakeFiles/maps_test.dir/flags.make
test/CMakeFiles/maps_test.dir/maps_test.cc.o: ../test/maps_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alogins/fcla/lib/lemon-1.3.1/lib/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/maps_test.dir/maps_test.cc.o"
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/maps_test.dir/maps_test.cc.o -c /home/alogins/fcla/lib/lemon-1.3.1/test/maps_test.cc

test/CMakeFiles/maps_test.dir/maps_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/maps_test.dir/maps_test.cc.i"
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alogins/fcla/lib/lemon-1.3.1/test/maps_test.cc > CMakeFiles/maps_test.dir/maps_test.cc.i

test/CMakeFiles/maps_test.dir/maps_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/maps_test.dir/maps_test.cc.s"
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alogins/fcla/lib/lemon-1.3.1/test/maps_test.cc -o CMakeFiles/maps_test.dir/maps_test.cc.s

test/CMakeFiles/maps_test.dir/maps_test.cc.o.requires:

.PHONY : test/CMakeFiles/maps_test.dir/maps_test.cc.o.requires

test/CMakeFiles/maps_test.dir/maps_test.cc.o.provides: test/CMakeFiles/maps_test.dir/maps_test.cc.o.requires
	$(MAKE) -f test/CMakeFiles/maps_test.dir/build.make test/CMakeFiles/maps_test.dir/maps_test.cc.o.provides.build
.PHONY : test/CMakeFiles/maps_test.dir/maps_test.cc.o.provides

test/CMakeFiles/maps_test.dir/maps_test.cc.o.provides.build: test/CMakeFiles/maps_test.dir/maps_test.cc.o


# Object files for target maps_test
maps_test_OBJECTS = \
"CMakeFiles/maps_test.dir/maps_test.cc.o"

# External object files for target maps_test
maps_test_EXTERNAL_OBJECTS =

test/maps_test: test/CMakeFiles/maps_test.dir/maps_test.cc.o
test/maps_test: test/CMakeFiles/maps_test.dir/build.make
test/maps_test: lemon/libemon.a
test/maps_test: test/CMakeFiles/maps_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alogins/fcla/lib/lemon-1.3.1/lib/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable maps_test"
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/maps_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/maps_test.dir/build: test/maps_test

.PHONY : test/CMakeFiles/maps_test.dir/build

test/CMakeFiles/maps_test.dir/requires: test/CMakeFiles/maps_test.dir/maps_test.cc.o.requires

.PHONY : test/CMakeFiles/maps_test.dir/requires

test/CMakeFiles/maps_test.dir/clean:
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib/test && $(CMAKE_COMMAND) -P CMakeFiles/maps_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/maps_test.dir/clean

test/CMakeFiles/maps_test.dir/depend:
	cd /home/alogins/fcla/lib/lemon-1.3.1/lib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alogins/fcla/lib/lemon-1.3.1 /home/alogins/fcla/lib/lemon-1.3.1/test /home/alogins/fcla/lib/lemon-1.3.1/lib /home/alogins/fcla/lib/lemon-1.3.1/lib/test /home/alogins/fcla/lib/lemon-1.3.1/lib/test/CMakeFiles/maps_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/maps_test.dir/depend

