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
include build_results/src/libkiter/CMakeFiles/kiter_lp.dir/depend.make

# Include the progress variables for this target.
include build_results/src/libkiter/CMakeFiles/kiter_lp.dir/progress.make

# Include the compile flags for this target's objects.
include build_results/src/libkiter/CMakeFiles/kiter_lp.dir/flags.make

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/flags.make
build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o: src/libkiter/lp/glpsol.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o -c /home/katherine/Projects/kiter/src/libkiter/lp/glpsol.cpp

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.i"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/katherine/Projects/kiter/src/libkiter/lp/glpsol.cpp > CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.i

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.s"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/katherine/Projects/kiter/src/libkiter/lp/glpsol.cpp -o CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.s

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/flags.make
build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o: src/libkiter/lp/glpsol_coin.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o -c /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_coin.cpp

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.i"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_coin.cpp > CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.i

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.s"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_coin.cpp -o CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.s

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/flags.make
build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o: src/libkiter/lp/glpsol_cplex.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o -c /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_cplex.cpp

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.i"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_cplex.cpp > CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.i

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.s"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_cplex.cpp -o CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.s

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/flags.make
build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o: src/libkiter/lp/glpsol_gurobi.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o -c /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_gurobi.cpp

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.i"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_gurobi.cpp > CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.i

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.s"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/katherine/Projects/kiter/src/libkiter/lp/glpsol_gurobi.cpp -o CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.s

# Object files for target kiter_lp
kiter_lp_OBJECTS = \
"CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o" \
"CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o" \
"CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o" \
"CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o"

# External object files for target kiter_lp
kiter_lp_EXTERNAL_OBJECTS =

lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol.cpp.o
lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_coin.cpp.o
lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_cplex.cpp.o
lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/lp/glpsol_gurobi.cpp.o
lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/build.make
lib/libkiter_lp.so: lib/libkiter_commons.so
lib/libkiter_lp.so: /usr/lib/x86_64-linux-gnu/libglpk.so
lib/libkiter_lp.so: /usr/lib/x86_64-linux-gnu/libxml2.so
lib/libkiter_lp.so: build_results/src/libkiter/CMakeFiles/kiter_lp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/katherine/Projects/kiter/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared library ../../../lib/libkiter_lp.so"
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kiter_lp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
build_results/src/libkiter/CMakeFiles/kiter_lp.dir/build: lib/libkiter_lp.so

.PHONY : build_results/src/libkiter/CMakeFiles/kiter_lp.dir/build

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/clean:
	cd /home/katherine/Projects/kiter/build_results/src/libkiter && $(CMAKE_COMMAND) -P CMakeFiles/kiter_lp.dir/cmake_clean.cmake
.PHONY : build_results/src/libkiter/CMakeFiles/kiter_lp.dir/clean

build_results/src/libkiter/CMakeFiles/kiter_lp.dir/depend:
	cd /home/katherine/Projects/kiter && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/katherine/Projects/kiter /home/katherine/Projects/kiter/src/libkiter /home/katherine/Projects/kiter /home/katherine/Projects/kiter/build_results/src/libkiter /home/katherine/Projects/kiter/build_results/src/libkiter/CMakeFiles/kiter_lp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : build_results/src/libkiter/CMakeFiles/kiter_lp.dir/depend

