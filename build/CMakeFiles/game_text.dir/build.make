# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_SOURCE_DIR = /autofs/unityaccount/cremi/awniang/net-b11

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /autofs/unityaccount/cremi/awniang/net-b11/build

# Include any dependencies generated for this target.
include CMakeFiles/game_text.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/game_text.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/game_text.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/game_text.dir/flags.make

CMakeFiles/game_text.dir/codegen:
.PHONY : CMakeFiles/game_text.dir/codegen

CMakeFiles/game_text.dir/game_text.c.o: CMakeFiles/game_text.dir/flags.make
CMakeFiles/game_text.dir/game_text.c.o: /autofs/unityaccount/cremi/awniang/net-b11/game_text.c
CMakeFiles/game_text.dir/game_text.c.o: CMakeFiles/game_text.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/autofs/unityaccount/cremi/awniang/net-b11/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/game_text.dir/game_text.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/game_text.dir/game_text.c.o -MF CMakeFiles/game_text.dir/game_text.c.o.d -o CMakeFiles/game_text.dir/game_text.c.o -c /autofs/unityaccount/cremi/awniang/net-b11/game_text.c

CMakeFiles/game_text.dir/game_text.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/game_text.dir/game_text.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /autofs/unityaccount/cremi/awniang/net-b11/game_text.c > CMakeFiles/game_text.dir/game_text.c.i

CMakeFiles/game_text.dir/game_text.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/game_text.dir/game_text.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /autofs/unityaccount/cremi/awniang/net-b11/game_text.c -o CMakeFiles/game_text.dir/game_text.c.s

# Object files for target game_text
game_text_OBJECTS = \
"CMakeFiles/game_text.dir/game_text.c.o"

# External object files for target game_text
game_text_EXTERNAL_OBJECTS =

game_text: CMakeFiles/game_text.dir/game_text.c.o
game_text: CMakeFiles/game_text.dir/build.make
game_text: libgame.a
game_text: CMakeFiles/game_text.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/autofs/unityaccount/cremi/awniang/net-b11/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable game_text"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/game_text.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/game_text.dir/build: game_text
.PHONY : CMakeFiles/game_text.dir/build

CMakeFiles/game_text.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/game_text.dir/cmake_clean.cmake
.PHONY : CMakeFiles/game_text.dir/clean

CMakeFiles/game_text.dir/depend:
	cd /autofs/unityaccount/cremi/awniang/net-b11/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /autofs/unityaccount/cremi/awniang/net-b11 /autofs/unityaccount/cremi/awniang/net-b11 /autofs/unityaccount/cremi/awniang/net-b11/build /autofs/unityaccount/cremi/awniang/net-b11/build /autofs/unityaccount/cremi/awniang/net-b11/build/CMakeFiles/game_text.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/game_text.dir/depend

