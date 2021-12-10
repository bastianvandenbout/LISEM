# Install script for directory: C:/Apps/openMVS-master/libs/IO

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
   "C:/Program Files (x86)/OpenMVS/lib/OpenMVS/libIO.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/lib/OpenMVS" TYPE STATIC_LIBRARY FILES "C:/Apps/openMVS-master/lib/libIO.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/Common.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/Image.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageBMP.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageDDS.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageJPG.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImagePNG.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageSCI.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageTGA.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/ImageTIFF.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/OBJ.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO/PLY.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/IO" TYPE FILE FILES
    "C:/Apps/openMVS-master/libs/IO/Common.h"
    "C:/Apps/openMVS-master/libs/IO/Image.h"
    "C:/Apps/openMVS-master/libs/IO/ImageBMP.h"
    "C:/Apps/openMVS-master/libs/IO/ImageDDS.h"
    "C:/Apps/openMVS-master/libs/IO/ImageJPG.h"
    "C:/Apps/openMVS-master/libs/IO/ImagePNG.h"
    "C:/Apps/openMVS-master/libs/IO/ImageSCI.h"
    "C:/Apps/openMVS-master/libs/IO/ImageTGA.h"
    "C:/Apps/openMVS-master/libs/IO/ImageTIFF.h"
    "C:/Apps/openMVS-master/libs/IO/OBJ.h"
    "C:/Apps/openMVS-master/libs/IO/PLY.h"
    )
endif()

