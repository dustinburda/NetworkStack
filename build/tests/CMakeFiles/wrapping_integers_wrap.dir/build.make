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
include tests/CMakeFiles/wrapping_integers_wrap.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/wrapping_integers_wrap.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/wrapping_integers_wrap.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/wrapping_integers_wrap.dir/flags.make

tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o: tests/CMakeFiles/wrapping_integers_wrap.dir/flags.make
tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o: /media/sf_minnow/tests/wrapping_integers_wrap.cc
tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o: tests/CMakeFiles/wrapping_integers_wrap.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_minnow/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o -MF CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o.d -o CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o -c /media/sf_minnow/tests/wrapping_integers_wrap.cc

tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.i"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/sf_minnow/tests/wrapping_integers_wrap.cc > CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.i

tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.s"
	cd /media/sf_minnow/build/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/sf_minnow/tests/wrapping_integers_wrap.cc -o CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.s

# Object files for target wrapping_integers_wrap
wrapping_integers_wrap_OBJECTS = \
"CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o"

# External object files for target wrapping_integers_wrap
wrapping_integers_wrap_EXTERNAL_OBJECTS =

tests/wrapping_integers_wrap: tests/CMakeFiles/wrapping_integers_wrap.dir/wrapping_integers_wrap.cc.o
tests/wrapping_integers_wrap: tests/CMakeFiles/wrapping_integers_wrap.dir/build.make
tests/wrapping_integers_wrap: tests/libminnow_testing_debug.a
tests/wrapping_integers_wrap: src/libminnow_debug.a
tests/wrapping_integers_wrap: util/libutil_debug.a
tests/wrapping_integers_wrap: tests/CMakeFiles/wrapping_integers_wrap.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_minnow/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable wrapping_integers_wrap"
	cd /media/sf_minnow/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wrapping_integers_wrap.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/wrapping_integers_wrap.dir/build: tests/wrapping_integers_wrap
.PHONY : tests/CMakeFiles/wrapping_integers_wrap.dir/build

tests/CMakeFiles/wrapping_integers_wrap.dir/clean:
	cd /media/sf_minnow/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/wrapping_integers_wrap.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/wrapping_integers_wrap.dir/clean

tests/CMakeFiles/wrapping_integers_wrap.dir/depend:
	cd /media/sf_minnow/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_minnow /media/sf_minnow/tests /media/sf_minnow/build /media/sf_minnow/build/tests /media/sf_minnow/build/tests/CMakeFiles/wrapping_integers_wrap.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/wrapping_integers_wrap.dir/depend

