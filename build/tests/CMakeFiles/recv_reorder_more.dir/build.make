# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/sf_minnow

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/sf_minnow/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/recv_reorder_more.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/recv_reorder_more.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/recv_reorder_more.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/recv_reorder_more.dir/flags.make

tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o: tests/CMakeFiles/recv_reorder_more.dir/flags.make
tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o: /media/sf_minnow/tests/recv_reorder_more.cc
tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o: tests/CMakeFiles/recv_reorder_more.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_minnow/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o -MF CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o.d -o CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o -c /media/sf_minnow/tests/recv_reorder_more.cc

tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.i"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/sf_minnow/tests/recv_reorder_more.cc > CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.i

tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.s"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/sf_minnow/tests/recv_reorder_more.cc -o CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.s

# Object files for target recv_reorder_more
recv_reorder_more_OBJECTS = \
"CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o"

# External object files for target recv_reorder_more
recv_reorder_more_EXTERNAL_OBJECTS =

tests/recv_reorder_more: tests/CMakeFiles/recv_reorder_more.dir/recv_reorder_more.cc.o
tests/recv_reorder_more: tests/CMakeFiles/recv_reorder_more.dir/build.make
tests/recv_reorder_more: tests/libminnow_testing_debug.a
tests/recv_reorder_more: src/libminnow_debug.a
tests/recv_reorder_more: util/libutil_debug.a
tests/recv_reorder_more: tests/CMakeFiles/recv_reorder_more.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_minnow/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable recv_reorder_more"
	cd /media/sf_minnow/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/recv_reorder_more.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/recv_reorder_more.dir/build: tests/recv_reorder_more
.PHONY : tests/CMakeFiles/recv_reorder_more.dir/build

tests/CMakeFiles/recv_reorder_more.dir/clean:
	cd /media/sf_minnow/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/recv_reorder_more.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/recv_reorder_more.dir/clean

tests/CMakeFiles/recv_reorder_more.dir/depend:
	cd /media/sf_minnow/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_minnow /media/sf_minnow/tests /media/sf_minnow/build /media/sf_minnow/build/tests /media/sf_minnow/build/tests/CMakeFiles/recv_reorder_more.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/recv_reorder_more.dir/depend

