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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/katherine/Projects/kiter

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/katherine/Projects/kiter

# Include any dependencies generated for this target.
include build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/depend.make

# Include the progress variables for this target.
include build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/progress.make

# Include the compile flags for this target's objects.
include build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/flags.make

build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o: build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/flags.make
build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o: tests/PeriodicSchedulingTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o"
	cd /home/katherine/Projects/kiter/build_results/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o -c /home/katherine/Projects/kiter/tests/PeriodicSchedulingTest.cpp

build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.i"
	cd /home/katherine/Projects/kiter/build_results/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/katherine/Projects/kiter/tests/PeriodicSchedulingTest.cpp > CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.i

build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.s"
	cd /home/katherine/Projects/kiter/build_results/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/katherine/Projects/kiter/tests/PeriodicSchedulingTest.cpp -o CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.s

# Object files for target PeriodicSchedulingTest
PeriodicSchedulingTest_OBJECTS = \
"CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o"

# External object files for target PeriodicSchedulingTest
PeriodicSchedulingTest_EXTERNAL_OBJECTS =

bin/PeriodicSchedulingTest: build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/PeriodicSchedulingTest.cpp.o
bin/PeriodicSchedulingTest: build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/build.make
bin/PeriodicSchedulingTest: lib/liblibkiter.so
bin/PeriodicSchedulingTest: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.so.1.71.0
bin/PeriodicSchedulingTest: lib/libkiter_generators.so
bin/PeriodicSchedulingTest: lib/libkiter_algorithms.so
bin/PeriodicSchedulingTest: lib/libkiter_lp.so
bin/PeriodicSchedulingTest: /usr/lib/x86_64-linux-gnu/libglpk.so
bin/PeriodicSchedulingTest: lib/libkiter_printers.so
bin/PeriodicSchedulingTest: lib/libkiter_models.so
bin/PeriodicSchedulingTest: lib/libkiter_commons.so
bin/PeriodicSchedulingTest: /usr/lib/x86_64-linux-gnu/libxml2.so
bin/PeriodicSchedulingTest: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
bin/PeriodicSchedulingTest: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.71.0
bin/PeriodicSchedulingTest: build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/PeriodicSchedulingTest"
	cd /home/katherine/Projects/kiter/build_results/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PeriodicSchedulingTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/build: bin/PeriodicSchedulingTest

.PHONY : build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/build

build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/clean:
	cd /home/katherine/Projects/kiter/build_results/tests && $(CMAKE_COMMAND) -P CMakeFiles/PeriodicSchedulingTest.dir/cmake_clean.cmake
.PHONY : build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/clean

build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/depend:
	cd /home/katherine/Projects/kiter && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/katherine/Projects/kiter /home/katherine/Projects/kiter/tests /home/katherine/Projects/kiter /home/katherine/Projects/kiter/build_results/tests /home/katherine/Projects/kiter/build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build_results/tests/CMakeFiles/PeriodicSchedulingTest.dir/depend

