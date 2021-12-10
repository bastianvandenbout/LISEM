# Install script for directory: C:/Apps/openMVS-master/libs/Math

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/Common.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/LBP.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math" TYPE FILE FILES
    "C:/Apps/openMVS-master/libs/Math/Common.h"
    "C:/Apps/openMVS-master/libs/Math/LBP.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/IBFS/IBFS.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/IBFS" TYPE FILE FILES "C:/Apps/openMVS-master/libs/Math/IBFS/IBFS.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/LMFit/lmmin.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/LMFit" TYPE FILE FILES "C:/Apps/openMVS-master/libs/Math/LMFit/lmmin.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/MRFEnergy.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/MRFEnergy.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/instances.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/minimize.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/ordering.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/treeProbabilities.inl;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeBinary.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeBinaryFast.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeGeneral.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typePotts.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeTruncatedLinear.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeTruncatedLinear2D.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeTruncatedQuadratic.h;C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS/typeTruncatedQuadratic2D.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/include/OpenMVS/Math/TRWS" TYPE FILE FILES
    "C:/Apps/openMVS-master/libs/Math/TRWS/MRFEnergy.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/MRFEnergy.inl"
    "C:/Apps/openMVS-master/libs/Math/TRWS/instances.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/minimize.inl"
    "C:/Apps/openMVS-master/libs/Math/TRWS/ordering.inl"
    "C:/Apps/openMVS-master/libs/Math/TRWS/treeProbabilities.inl"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeBinary.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeBinaryFast.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeGeneral.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typePotts.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeTruncatedLinear.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeTruncatedLinear2D.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeTruncatedQuadratic.h"
    "C:/Apps/openMVS-master/libs/Math/TRWS/typeTruncatedQuadratic2D.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/OpenMVS/lib/OpenMVS/libMath.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Program Files (x86)/OpenMVS/lib/OpenMVS" TYPE STATIC_LIBRARY FILES "C:/Apps/openMVS-master/lib/libMath.a")
endif()

