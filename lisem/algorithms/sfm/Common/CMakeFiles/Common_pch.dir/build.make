# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Apps\msys64\mingw64\bin\cmake.exe

# The command to remove a file.
RM = C:\Apps\msys64\mingw64\bin\cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Apps\openMVS-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Apps\openMVS-master

# Utility rule file for Common_pch.

# Include the progress variables for this target.
include libs/Common/CMakeFiles/Common_pch.dir/progress.make

libs/Common/CMakeFiles/Common_pch: libs/Common/cotire/Common_CXX_prefix.hxx.gch


libs/Common/cotire/Common_CXX_prefix.hxx.gch: libs/Common/cotire/Common_CXX_prefix.hxx
libs/Common/cotire/Common_CXX_prefix.hxx.gch: C:/Apps/msys64/mingw64/bin/g++.exe
libs/Common/cotire/Common_CXX_prefix.hxx.gch: libs/Common/cotire/Common_CXX_prefix.hxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX precompiled header libs/Common/cotire/Common_CXX_prefix.hxx.gch"
	cd /d C:\Apps\openMVS-master\libs\Common && C:\Apps\msys64\mingw64\bin\cmake.exe -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P C:/Apps/openMVS-master/build/Cotire.cmake precompile C:/Apps/openMVS-master/libs/Common/Common_CXX_Cotire.cmake C:/Apps/openMVS-master/libs/Common/cotire/Common_CXX_prefix.hxx C:/Apps/openMVS-master/libs/Common/cotire/Common_CXX_prefix.hxx.gch C:/Apps/openMVS-master/libs/Common/Common.cpp

libs/Common/cotire/Common_CXX_prefix.hxx: libs/Common/cotire/Common_CXX_prefix.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating CXX prefix header libs/Common/cotire/Common_CXX_prefix.hxx"
	C:\Apps\msys64\mingw64\bin\cmake.exe -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P C:/Apps/openMVS-master/build/Cotire.cmake combine C:/Apps/openMVS-master/libs/Common/Common_CXX_Cotire.cmake C:/Apps/openMVS-master/libs/Common/cotire/Common_CXX_prefix.hxx C:/Apps/openMVS-master/libs/Common/cotire/Common_CXX_prefix.cxx

libs/Common/cotire/Common_CXX_prefix.cxx: libs/Common/Common.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating CXX prefix source libs/Common/cotire/Common_CXX_prefix.cxx"
	C:\Apps\msys64\mingw64\bin\cmake.exe -DCOTIRE_BUILD_TYPE:STRING= -DCOTIRE_VERBOSE:BOOL=$(VERBOSE) -P C:/Apps/openMVS-master/build/Cotire.cmake combine C:/Apps/openMVS-master/libs/Common/Common_CXX_Cotire.cmake C:/Apps/openMVS-master/libs/Common/cotire/Common_CXX_prefix.cxx C:/Apps/openMVS-master/libs/Common/Common.h

Common_pch: libs/Common/CMakeFiles/Common_pch
Common_pch: libs/Common/cotire/Common_CXX_prefix.hxx.gch
Common_pch: libs/Common/cotire/Common_CXX_prefix.hxx
Common_pch: libs/Common/cotire/Common_CXX_prefix.cxx
Common_pch: libs/Common/CMakeFiles/Common_pch.dir/build.make

.PHONY : Common_pch

# Rule to build all files generated by this target.
libs/Common/CMakeFiles/Common_pch.dir/build: Common_pch

.PHONY : libs/Common/CMakeFiles/Common_pch.dir/build

libs/Common/CMakeFiles/Common_pch.dir/clean:
	cd /d C:\Apps\openMVS-master\libs\Common && $(CMAKE_COMMAND) -P CMakeFiles\Common_pch.dir\cmake_clean.cmake
.PHONY : libs/Common/CMakeFiles/Common_pch.dir/clean

libs/Common/CMakeFiles/Common_pch.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Apps\openMVS-master C:\Apps\openMVS-master\libs\Common C:\Apps\openMVS-master C:\Apps\openMVS-master\libs\Common C:\Apps\openMVS-master\libs\Common\CMakeFiles\Common_pch.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : libs/Common/CMakeFiles/Common_pch.dir/depend

