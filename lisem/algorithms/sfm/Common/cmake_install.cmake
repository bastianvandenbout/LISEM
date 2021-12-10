# Install script for directory: C:/Apps/openMVS-master/libs/Common

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/OpenMVS")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/mingw64/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/lib/OpenMVS/libCommon.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/lib/OpenMVS" TYPE STATIC_LIBRARY FILES "C:/Apps/openMVS-master/lib/libCommon.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/AABB.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/AABB.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/AutoEstimator.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/AutoPtr.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/CUDA.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Common.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Config.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/ConfigTable.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/CriticalSection.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/EventQueue.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/FastDelegate.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/FastDelegateBind.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/FastDelegateCPP11.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/File.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Filters.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/HTMLDoc.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/HalfFloat.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Hash.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/LinkLib.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/List.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Log.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/MemFile.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/OBB.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/OBB.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Octree.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Octree.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Plane.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Plane.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Queue.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Random.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Ray.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Ray.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Rotation.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Rotation.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/SML.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Sampler.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Semaphore.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/SharedPtr.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Sphere.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Sphere.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Streams.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Strings.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Thread.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Timer.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Types.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Types.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Util.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common/Util.inl")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Common" TYPE FILE FILES
    "C:/Apps/openMVS-master/libs/Common/AABB.h"
    "C:/Apps/openMVS-master/libs/Common/AABB.inl"
    "C:/Apps/openMVS-master/libs/Common/AutoEstimator.h"
    "C:/Apps/openMVS-master/libs/Common/AutoPtr.h"
    "C:/Apps/openMVS-master/libs/Common/CUDA.h"
    "C:/Apps/openMVS-master/libs/Common/Common.h"
    "C:/Apps/openMVS-master/libs/Common/Config.h"
    "C:/Apps/openMVS-master/libs/Common/ConfigTable.h"
    "C:/Apps/openMVS-master/libs/Common/CriticalSection.h"
    "C:/Apps/openMVS-master/libs/Common/EventQueue.h"
    "C:/Apps/openMVS-master/libs/Common/FastDelegate.h"
    "C:/Apps/openMVS-master/libs/Common/FastDelegateBind.h"
    "C:/Apps/openMVS-master/libs/Common/FastDelegateCPP11.h"
    "C:/Apps/openMVS-master/libs/Common/File.h"
    "C:/Apps/openMVS-master/libs/Common/Filters.h"
    "C:/Apps/openMVS-master/libs/Common/HTMLDoc.h"
    "C:/Apps/openMVS-master/libs/Common/HalfFloat.h"
    "C:/Apps/openMVS-master/libs/Common/Hash.h"
    "C:/Apps/openMVS-master/libs/Common/LinkLib.h"
    "C:/Apps/openMVS-master/libs/Common/List.h"
    "C:/Apps/openMVS-master/libs/Common/Log.h"
    "C:/Apps/openMVS-master/libs/Common/MemFile.h"
    "C:/Apps/openMVS-master/libs/Common/OBB.h"
    "C:/Apps/openMVS-master/libs/Common/OBB.inl"
    "C:/Apps/openMVS-master/libs/Common/Octree.h"
    "C:/Apps/openMVS-master/libs/Common/Octree.inl"
    "C:/Apps/openMVS-master/libs/Common/Plane.h"
    "C:/Apps/openMVS-master/libs/Common/Plane.inl"
    "C:/Apps/openMVS-master/libs/Common/Queue.h"
    "C:/Apps/openMVS-master/libs/Common/Random.h"
    "C:/Apps/openMVS-master/libs/Common/Ray.h"
    "C:/Apps/openMVS-master/libs/Common/Ray.inl"
    "C:/Apps/openMVS-master/libs/Common/Rotation.h"
    "C:/Apps/openMVS-master/libs/Common/Rotation.inl"
    "C:/Apps/openMVS-master/libs/Common/SML.h"
    "C:/Apps/openMVS-master/libs/Common/Sampler.inl"
    "C:/Apps/openMVS-master/libs/Common/Semaphore.h"
    "C:/Apps/openMVS-master/libs/Common/SharedPtr.h"
    "C:/Apps/openMVS-master/libs/Common/Sphere.h"
    "C:/Apps/openMVS-master/libs/Common/Sphere.inl"
    "C:/Apps/openMVS-master/libs/Common/Streams.h"
    "C:/Apps/openMVS-master/libs/Common/Strings.h"
    "C:/Apps/openMVS-master/libs/Common/Thread.h"
    "C:/Apps/openMVS-master/libs/Common/Timer.h"
    "C:/Apps/openMVS-master/libs/Common/Types.h"
    "C:/Apps/openMVS-master/libs/Common/Types.inl"
    "C:/Apps/openMVS-master/libs/Common/Util.h"
    "C:/Apps/openMVS-master/libs/Common/Util.inl"
    )
endif()

