# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\JetBrains\CLion 2021.1.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\CLion 2021.1.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\MCworkspace\odrParser

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\MCworkspace\odrParser\cmake-build-debug

# Include any dependencies generated for this target.
include test/CMakeFiles/pure_odrInfo.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/pure_odrInfo.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/pure_odrInfo.dir/flags.make

test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/flags.make
test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/includes_CXX.rsp
test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj: ../test/getPureOdrInfo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MCworkspace\odrParser\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\pure_odrInfo.dir\getPureOdrInfo.cpp.obj -c D:\MCworkspace\odrParser\test\getPureOdrInfo.cpp

test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.i"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MCworkspace\odrParser\test\getPureOdrInfo.cpp > CMakeFiles\pure_odrInfo.dir\getPureOdrInfo.cpp.i

test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.s"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MCworkspace\odrParser\test\getPureOdrInfo.cpp -o CMakeFiles\pure_odrInfo.dir\getPureOdrInfo.cpp.s

test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/flags.make
test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/includes_CXX.rsp
test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj: ../src/pureOdrParse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MCworkspace\odrParser\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\pure_odrInfo.dir\__\src\pureOdrParse.cpp.obj -c D:\MCworkspace\odrParser\src\pureOdrParse.cpp

test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.i"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MCworkspace\odrParser\src\pureOdrParse.cpp > CMakeFiles\pure_odrInfo.dir\__\src\pureOdrParse.cpp.i

test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.s"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MCworkspace\odrParser\src\pureOdrParse.cpp -o CMakeFiles\pure_odrInfo.dir\__\src\pureOdrParse.cpp.s

test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/flags.make
test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj: test/CMakeFiles/pure_odrInfo.dir/includes_CXX.rsp
test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj: ../src/tinyxml2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\MCworkspace\odrParser\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\pure_odrInfo.dir\__\src\tinyxml2.cpp.obj -c D:\MCworkspace\odrParser\src\tinyxml2.cpp

test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.i"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\MCworkspace\odrParser\src\tinyxml2.cpp > CMakeFiles\pure_odrInfo.dir\__\src\tinyxml2.cpp.i

test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.s"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && "D:\Program Files\mingw6481\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\MCworkspace\odrParser\src\tinyxml2.cpp -o CMakeFiles\pure_odrInfo.dir\__\src\tinyxml2.cpp.s

# Object files for target pure_odrInfo
pure_odrInfo_OBJECTS = \
"CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj" \
"CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj" \
"CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj"

# External object files for target pure_odrInfo
pure_odrInfo_EXTERNAL_OBJECTS =

../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/getPureOdrInfo.cpp.obj
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/__/src/pureOdrParse.cpp.obj
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/__/src/tinyxml2.cpp.obj
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/build.make
../bin/pure_odrInfo.exe: ../lib/lib81/libODrive.1.5.5.a
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/linklibs.rsp
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/objects1.rsp
../bin/pure_odrInfo.exe: test/CMakeFiles/pure_odrInfo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\MCworkspace\odrParser\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ..\..\bin\pure_odrInfo.exe"
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\pure_odrInfo.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/pure_odrInfo.dir/build: ../bin/pure_odrInfo.exe

.PHONY : test/CMakeFiles/pure_odrInfo.dir/build

test/CMakeFiles/pure_odrInfo.dir/clean:
	cd /d D:\MCworkspace\odrParser\cmake-build-debug\test && $(CMAKE_COMMAND) -P CMakeFiles\pure_odrInfo.dir\cmake_clean.cmake
.PHONY : test/CMakeFiles/pure_odrInfo.dir/clean

test/CMakeFiles/pure_odrInfo.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\MCworkspace\odrParser D:\MCworkspace\odrParser\test D:\MCworkspace\odrParser\cmake-build-debug D:\MCworkspace\odrParser\cmake-build-debug\test D:\MCworkspace\odrParser\cmake-build-debug\test\CMakeFiles\pure_odrInfo.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/pure_odrInfo.dir/depend

