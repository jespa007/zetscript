# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /opt/cmake/bin/cmake

# The command to remove a file.
RM = /opt/cmake/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jespada/zetdev/zetscript

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jespada/zetdev/zetscript/scripts/build/rpi

# Include any dependencies generated for this target.
include src/CMakeFiles/test_call_native_functions.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/test_call_native_functions.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/test_call_native_functions.dir/flags.make

src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o: src/CMakeFiles/test_call_native_functions.dir/flags.make
src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o: ../../../test/test_call_native_functions.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jespada/zetdev/zetscript/scripts/build/rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o"
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi/src && /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-g++ --sysroot=/home/jespada/rpi/rootfs  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o -c /home/jespada/zetdev/zetscript/test/test_call_native_functions.cpp

src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.i"
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi/src && /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-g++ --sysroot=/home/jespada/rpi/rootfs $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jespada/zetdev/zetscript/test/test_call_native_functions.cpp > CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.i

src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.s"
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi/src && /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-g++ --sysroot=/home/jespada/rpi/rootfs $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jespada/zetdev/zetscript/test/test_call_native_functions.cpp -o CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.s

# Object files for target test_call_native_functions
test_call_native_functions_OBJECTS = \
"CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o"

# External object files for target test_call_native_functions
test_call_native_functions_EXTERNAL_OBJECTS =

release/test_call_native_functions: src/CMakeFiles/test_call_native_functions.dir/__/test/test_call_native_functions.cpp.o
release/test_call_native_functions: src/CMakeFiles/test_call_native_functions.dir/build.make
release/test_call_native_functions: release/libzetscript-2-0-0.a
release/test_call_native_functions: src/CMakeFiles/test_call_native_functions.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jespada/zetdev/zetscript/scripts/build/rpi/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../release/test_call_native_functions"
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_call_native_functions.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/test_call_native_functions.dir/build: release/test_call_native_functions

.PHONY : src/CMakeFiles/test_call_native_functions.dir/build

src/CMakeFiles/test_call_native_functions.dir/clean:
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi/src && $(CMAKE_COMMAND) -P CMakeFiles/test_call_native_functions.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/test_call_native_functions.dir/clean

src/CMakeFiles/test_call_native_functions.dir/depend:
	cd /home/jespada/zetdev/zetscript/scripts/build/rpi && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jespada/zetdev/zetscript /home/jespada/zetdev/zetscript/src /home/jespada/zetdev/zetscript/scripts/build/rpi /home/jespada/zetdev/zetscript/scripts/build/rpi/src /home/jespada/zetdev/zetscript/scripts/build/rpi/src/CMakeFiles/test_call_native_functions.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/test_call_native_functions.dir/depend

