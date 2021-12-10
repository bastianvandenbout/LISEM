# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Apps\openMVS-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Apps\openMVS-master

# Include any dependencies generated for this target.
include libs/MVS/CMakeFiles/MVS.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include libs/MVS/CMakeFiles/MVS.dir/compiler_depend.make

# Include the progress variables for this target.
include libs/MVS/CMakeFiles/MVS.dir/progress.make

# Include the compile flags for this target's objects.
include libs/MVS/CMakeFiles/MVS.dir/flags.make

libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj: libs/MVS/Camera.cpp
libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj -MF CMakeFiles\MVS.dir\Camera.cpp.obj.d -o CMakeFiles\MVS.dir\Camera.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Camera.cpp

libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Camera.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Camera.cpp > CMakeFiles\MVS.dir\Camera.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Camera.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Camera.cpp -o CMakeFiles\MVS.dir\Camera.cpp.s

libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj: libs/MVS/Common.cpp
libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj -MF CMakeFiles\MVS.dir\Common.cpp.obj.d -o CMakeFiles\MVS.dir\Common.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Common.cpp

libs/MVS/CMakeFiles/MVS.dir/Common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Common.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Common.cpp > CMakeFiles\MVS.dir\Common.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Common.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Common.cpp -o CMakeFiles\MVS.dir\Common.cpp.s

libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj: libs/MVS/DepthMap.cpp
libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj -MF CMakeFiles\MVS.dir\DepthMap.cpp.obj.d -o CMakeFiles\MVS.dir\DepthMap.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\DepthMap.cpp

libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/DepthMap.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\DepthMap.cpp > CMakeFiles\MVS.dir\DepthMap.cpp.i

libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/DepthMap.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\DepthMap.cpp -o CMakeFiles\MVS.dir\DepthMap.cpp.s

libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj: libs/MVS/Image.cpp
libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj -MF CMakeFiles\MVS.dir\Image.cpp.obj.d -o CMakeFiles\MVS.dir\Image.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Image.cpp

libs/MVS/CMakeFiles/MVS.dir/Image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Image.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Image.cpp > CMakeFiles\MVS.dir\Image.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Image.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Image.cpp -o CMakeFiles\MVS.dir\Image.cpp.s

libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj: libs/MVS/Mesh.cpp
libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj -MF CMakeFiles\MVS.dir\Mesh.cpp.obj.d -o CMakeFiles\MVS.dir\Mesh.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Mesh.cpp

libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Mesh.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Mesh.cpp > CMakeFiles\MVS.dir\Mesh.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Mesh.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Mesh.cpp -o CMakeFiles\MVS.dir\Mesh.cpp.s

libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj: libs/MVS/Platform.cpp
libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj -MF CMakeFiles\MVS.dir\Platform.cpp.obj.d -o CMakeFiles\MVS.dir\Platform.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Platform.cpp

libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Platform.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Platform.cpp > CMakeFiles\MVS.dir\Platform.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Platform.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Platform.cpp -o CMakeFiles\MVS.dir\Platform.cpp.s

libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj: libs/MVS/PointCloud.cpp
libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj -MF CMakeFiles\MVS.dir\PointCloud.cpp.obj.d -o CMakeFiles\MVS.dir\PointCloud.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\PointCloud.cpp

libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/PointCloud.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\PointCloud.cpp > CMakeFiles\MVS.dir\PointCloud.cpp.i

libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/PointCloud.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\PointCloud.cpp -o CMakeFiles\MVS.dir\PointCloud.cpp.s

libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj: libs/MVS/RectsBinPack.cpp
libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj -MF CMakeFiles\MVS.dir\RectsBinPack.cpp.obj.d -o CMakeFiles\MVS.dir\RectsBinPack.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\RectsBinPack.cpp

libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/RectsBinPack.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\RectsBinPack.cpp > CMakeFiles\MVS.dir\RectsBinPack.cpp.i

libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/RectsBinPack.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\RectsBinPack.cpp -o CMakeFiles\MVS.dir\RectsBinPack.cpp.s

libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj: libs/MVS/Scene.cpp
libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj -MF CMakeFiles\MVS.dir\Scene.cpp.obj.d -o CMakeFiles\MVS.dir\Scene.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\Scene.cpp

libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/Scene.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\Scene.cpp > CMakeFiles\MVS.dir\Scene.cpp.i

libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/Scene.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\Scene.cpp -o CMakeFiles\MVS.dir\Scene.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj: libs/MVS/SceneDensify.cpp
libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj -MF CMakeFiles\MVS.dir\SceneDensify.cpp.obj.d -o CMakeFiles\MVS.dir\SceneDensify.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SceneDensify.cpp

libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SceneDensify.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SceneDensify.cpp > CMakeFiles\MVS.dir\SceneDensify.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SceneDensify.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SceneDensify.cpp -o CMakeFiles\MVS.dir\SceneDensify.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj: libs/MVS/SceneReconstruct.cpp
libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj -MF CMakeFiles\MVS.dir\SceneReconstruct.cpp.obj.d -o CMakeFiles\MVS.dir\SceneReconstruct.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SceneReconstruct.cpp

libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SceneReconstruct.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SceneReconstruct.cpp > CMakeFiles\MVS.dir\SceneReconstruct.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SceneReconstruct.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SceneReconstruct.cpp -o CMakeFiles\MVS.dir\SceneReconstruct.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj: libs/MVS/SceneRefine.cpp
libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj -MF CMakeFiles\MVS.dir\SceneRefine.cpp.obj.d -o CMakeFiles\MVS.dir\SceneRefine.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SceneRefine.cpp

libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SceneRefine.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SceneRefine.cpp > CMakeFiles\MVS.dir\SceneRefine.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SceneRefine.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SceneRefine.cpp -o CMakeFiles\MVS.dir\SceneRefine.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj: libs/MVS/SceneRefineCUDA.cpp
libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj -MF CMakeFiles\MVS.dir\SceneRefineCUDA.cpp.obj.d -o CMakeFiles\MVS.dir\SceneRefineCUDA.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SceneRefineCUDA.cpp

libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SceneRefineCUDA.cpp > CMakeFiles\MVS.dir\SceneRefineCUDA.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SceneRefineCUDA.cpp -o CMakeFiles\MVS.dir\SceneRefineCUDA.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj: libs/MVS/SceneTexture.cpp
libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj -MF CMakeFiles\MVS.dir\SceneTexture.cpp.obj.d -o CMakeFiles\MVS.dir\SceneTexture.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SceneTexture.cpp

libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SceneTexture.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SceneTexture.cpp > CMakeFiles\MVS.dir\SceneTexture.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SceneTexture.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SceneTexture.cpp -o CMakeFiles\MVS.dir\SceneTexture.cpp.s

libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/flags.make
libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/includes_CXX.rsp
libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj: libs/MVS/SemiGlobalMatcher.cpp
libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj: libs/MVS/CMakeFiles/MVS.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj -MF CMakeFiles\MVS.dir\SemiGlobalMatcher.cpp.obj.d -o CMakeFiles\MVS.dir\SemiGlobalMatcher.cpp.obj -c C:\Apps\openMVS-master\libs\MVS\SemiGlobalMatcher.cpp

libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.i"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Apps\openMVS-master\libs\MVS\SemiGlobalMatcher.cpp > CMakeFiles\MVS.dir\SemiGlobalMatcher.cpp.i

libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.s"
	cd /d C:\Apps\openMVS-master\libs\MVS && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Apps\openMVS-master\libs\MVS\SemiGlobalMatcher.cpp -o CMakeFiles\MVS.dir\SemiGlobalMatcher.cpp.s

# Object files for target MVS
MVS_OBJECTS = \
"CMakeFiles/MVS.dir/Camera.cpp.obj" \
"CMakeFiles/MVS.dir/Common.cpp.obj" \
"CMakeFiles/MVS.dir/DepthMap.cpp.obj" \
"CMakeFiles/MVS.dir/Image.cpp.obj" \
"CMakeFiles/MVS.dir/Mesh.cpp.obj" \
"CMakeFiles/MVS.dir/Platform.cpp.obj" \
"CMakeFiles/MVS.dir/PointCloud.cpp.obj" \
"CMakeFiles/MVS.dir/RectsBinPack.cpp.obj" \
"CMakeFiles/MVS.dir/Scene.cpp.obj" \
"CMakeFiles/MVS.dir/SceneDensify.cpp.obj" \
"CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj" \
"CMakeFiles/MVS.dir/SceneRefine.cpp.obj" \
"CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj" \
"CMakeFiles/MVS.dir/SceneTexture.cpp.obj" \
"CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj"

# External object files for target MVS
MVS_EXTERNAL_OBJECTS =

bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Camera.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Common.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/DepthMap.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Image.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Mesh.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Platform.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/PointCloud.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/RectsBinPack.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/Scene.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SceneDensify.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SceneReconstruct.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SceneRefine.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SceneRefineCUDA.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SceneTexture.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/SemiGlobalMatcher.cpp.obj
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/build.make
bin/libMVS.dll: lib/libCommon.a
bin/libMVS.dll: lib/libMath.a
bin/libMVS.dll: lib/libIO.a
bin/libMVS.dll: c:/msys64/mingw64/lib/libgmp.a
bin/libMVS.dll: c:/msys64//mingw64/lib/libgmpxx.a
bin/libMVS.dll: lib/libCommon.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libboost_iostreams-mt.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libboost_program_options-mt.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libboost_system-mt.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libboost_serialization-mt.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libboost_regex-mt.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_gapi.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_stitching.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_alphamat.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_aruco.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_bgsegm.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_ccalib.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_dnn_objdetect.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_dnn_superres.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_dpm.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_face.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_fuzzy.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_hdf.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_hfs.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_img_hash.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_intensity_transform.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_line_descriptor.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_mcc.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_ovis.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_quality.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_rapid.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_reg.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_rgbd.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_saliency.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_sfm.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_stereo.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_structured_light.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_phase_unwrapping.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_superres.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_optflow.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_surface_matching.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_tracking.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_highgui.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_datasets.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_plot.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_text.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_videostab.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_videoio.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_xfeatures2d.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_ml.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_shape.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_ximgproc.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_video.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_dnn.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_xobjdetect.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_imgcodecs.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_objdetect.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_calib3d.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_features2d.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_flann.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_xphoto.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_photo.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_imgproc.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libopencv_core.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libpng.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libz.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libjpeg.dll.a
bin/libMVS.dll: C:/msys64/mingw64/lib/libtiff.dll.a
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/linklibs.rsp
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/objects1.rsp
bin/libMVS.dll: libs/MVS/CMakeFiles/MVS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Apps\openMVS-master\CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking CXX shared library ..\..\bin\libMVS.dll"
	cd /d C:\Apps\openMVS-master\libs\MVS && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\MVS.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/MVS/CMakeFiles/MVS.dir/build: bin/libMVS.dll
.PHONY : libs/MVS/CMakeFiles/MVS.dir/build

libs/MVS/CMakeFiles/MVS.dir/clean:
	cd /d C:\Apps\openMVS-master\libs\MVS && $(CMAKE_COMMAND) -P CMakeFiles\MVS.dir\cmake_clean.cmake
.PHONY : libs/MVS/CMakeFiles/MVS.dir/clean

libs/MVS/CMakeFiles/MVS.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Apps\openMVS-master C:\Apps\openMVS-master\libs\MVS C:\Apps\openMVS-master C:\Apps\openMVS-master\libs\MVS C:\Apps\openMVS-master\libs\MVS\CMakeFiles\MVS.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : libs/MVS/CMakeFiles/MVS.dir/depend

