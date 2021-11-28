cmake_minimum_required(VERSION 3.9.0)

#We make our project and set the languages we use
#Lisem Integrated Spatial Earth Modeller
project(LISEM C CXX Fortran)
ENABLE_LANGUAGE(Fortran)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

SET(REMOVEFILESUNITTEST "")

# ==============================================================================
# get the compilation settings
# ==============================================================================
option(LISEM_BUILD_SHARED "Build LISEM shared libs" ON)
option(LISEM_BUILD_TESTS "Build LISEM tests" OFF)
option(LISEM_BUILD_DOC "Build LISEM documentation" ON)
option(LISEM_BUILD_APP "Build LISEM application." ON)
option(LISEM_BUILD_PYTHON "Build LISEM pyhton bindings" ON)
option(LISEM_BUILD_CONDA "Build LISEM CONDA PACKAGE" ON)
option(LISEM_BUILD_INSTALLER "Build LISEM installer" ON)
option(LISEM_USE_EXTERNALDIR "try to find all external libraries in a specified external dir" ON)
option(LISEM_BUILD_DEPLOY "Use a python script and Visual studio build tools to auto-generate a python binding proxy-dll" ON)

set(CMAKE_BUILD_TYPE "Release")
add_compile_definitions("LISEM_BUILD")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmakeFindModules)

# Set shared or static mode
if (LISEM_BUILD_SHARED)
  set(BUILD_SHARED_LIBS ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else(LISEM_BUILD_SHARED)
  set(BUILD_SHARED_LIBS OFF)
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")


# ==============================================================================
# Check C++11 support
# ==============================================================================
include(CXX14)

check_for_cxx14_compiler(CXX11_COMPILER)
# If a C++14 compiler is available, then set the appropriate flags
if (CXX11_COMPILER)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
else (CXX11_COMPILER)
  message(FATAL_ERROR "A CXX11 compiler is required.")
endif (CXX11_COMPILER)



set(LISEM_CXX_FEATURES
  cxx_std_14
  cxx_auto_type
  cxx_constexpr
  cxx_lambdas
  cxx_nullptr
  cxx_override
  cxx_range_for
  cxx_strong_enums
)


set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
add_definitions("-DASSETS_DIR=\"${ASSETS_DIR}\"")

if (WIN32)
    add_definitions("-DOS_WIN")
endif(WIN32)
if(UNIX)
    add_definitions("-DOS_LNX")
endif(UNIX)

if(UNIX)
    add_definitions(-Wall -fvisibility=hidden)
endif()

message("Co
ler info: ")
message("C -Compiler: " ${CMAKE_CXX_COMPILER})
message("C++ -Compiler: " ${CMAKE_C_COMPILER})


# ==============================================================================
# Automoc
# ==============================================================================
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC OFF)


# ==============================================================================
# LISEM version
# ==============================================================================
file(STRINGS "lisem/core/version.h" _LISEM_VERSION_H_CONTENTS REGEX "#define LISEM_VERSION_")

foreach(v MAJOR MINOR PATCH)
  if("${_LISEM_VERSION_H_CONTENTS}" MATCHES "#define LISEM_VERSION_${v} ([0-9]+)")
    set(LISEM_VERSION_${v} "${CMAKE_MATCH_1}")
 else()
    message(FATAL_ERROR "Failed to retrieve the LISEM version from the source code. Missing LISEM_VERSION_${v}.")
  endif()
endforeach()
set(LISEM_VERSION ${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}.${LISEM_VERSION_PATCH})

message("Detected LISEM version: " ${LISEM_VERSION})

# ==============================================================================
# get the external libraries
# ==============================================================================
add_subdirectory(third_party)

set(WIN_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lisem.rc")

SET(EIGEN_DONT_PARALLELIZE ON)

include(externals)


set(CMAKE_CXX_STANDARD 17)
if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -std=c++14 -Wl,--default-image-base-low -Werror=return-type -lopengl32 -fopenmp -lws2_32 -lwsock32 -lWs2_32 -fext-numeric-literals -mavx -msse4.1 -mavx2") # generate debug information#
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -Wl,--default-image-base-low")
else()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DEIGEN_DONT_VECTORIZE -DEIGEN_MAX_ALIGN_BYTES=0 -DEIGEN_DONT_PARALLELIZE") # generate debug information#
set(CMAKE_CXX_FLAGS "-pthread -DEIGEN_DONT_VECTORIZE -DEIGEN_MAX_ALIGN_BYTES=0 -DEIGEN_DONT_PARALLELIZE -fopenmp -w -std=c++17 -Werror=return-type -fopenmp -Wall") # generate debug information#
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
endif()

message(${CMAKE_C_FLAGS})
message(${CMAKE_CXX_FLAGS})

# ==============================================================================
# we need to compile several modules as part of lisem itself
# ==============================================================================
add_subdirectory(lisem)


# ==============================================================================
# INSTALL RULES
# ==============================================================================

install(EXPORT LISEM-targets DESTINATION bin)
#install(EXPORT LISEM-bindings DESTINATION python)
install(EXPORT LISEM-targets
  FILE LISEMTargets.cmake
  NAMESPACE LISEM::
  DESTINATION share/LISEM/cmake
)
install(
  DIRECTORY .
  DESTINATION include/lisem
  COMPONENT headers
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)


# ==============================================================================
# Resource copying so we can actually run the model (shaders, libs, assets etc..)
# ==============================================================================

IF(EXISTS ${EXTERNAL_DIR})
install(DIRECTORY "${EXTERNAL_DIR}/runtime/" DESTINATION "bin/")
ENDIF()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/lisem/model/kernels/" DESTINATION "bin/kernels")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/lisem/ui/shaders/" DESTINATION "bin/shaders")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/dist/" DESTINATION "bin/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/resources/" DESTINATION "bin/assets")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/lisem/model/kernels/" DESTINATION "bin/shaders")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/lisem/ui/shaders/" DESTINATION "bin/kernels")

file(GLOB kernel_src "lisem/model/kernels/*.*")
file(GLOB shader_src "lisem/ui/shaders/*.*")
file(GLOB resources "resources/*.*")
if(WIN32)
file(GLOB dllresources "third_party/dllwrap/*")
endif()
#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedKernels "")
 foreach(QmlFile ${kernel_src})
   get_filename_component(nam ${QmlFile} NAME)
   configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/kernels/${nam} COPYONLY)
   list(APPEND copiedKernels ${CMAKE_CURRENT_BINARY_DIR}/bin/kernels/${nam})
 endforeach()


 set(copiedShaders "")
 foreach(QmlFile ${shader_src})
   get_filename_component(nam ${QmlFile} NAME)
   configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${nam} COPYONLY)
   list(APPEND copiedShaders ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${nam})
 endforeach()


#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedResources "")
 foreach(QmlFile ${resources})
    get_filename_component(nam ${QmlFile} NAME)
        configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/assets/${nam} COPYONLY)

   list(APPEND copiedResources ${CMAKE_CURRENT_BINARY_DIR}/bin/assets/${nam})
 endforeach()

 #Add additional command dependend on extra compiled libary
 #this copies all the changed kernel files to the release/assets folder
  set(copiedDLLResources "")
  foreach(QmlFile ${dllresources})
     get_filename_component(nam ${QmlFile} NAME)
         configure_file(${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/python/${nam} COPYONLY)

    list(APPEND copiedDLLResources ${CMAKE_CURRENT_BINARY_DIR}/python/${nam})
  endforeach()


 #Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedKernels "")
 foreach(QmlFile ${kernel_src})
   get_filename_component(nam ${QmlFile} NAME)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bin/kernels/${nam}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/kernels/
     DEPENDS ${QmlFile}
     COMMENT "Copying ${QmlFile}"
     VERBATIM
   )
   list(APPEND copiedKernels ${CMAKE_CURRENT_BINARY_DIR}/bin/kernels/${nam})
 endforeach()

add_library(Kernels SHARED ${kernel_src} ${copiedKernels})



set(copiedShaders "")
foreach(QmlFile ${shader_src})
  get_filename_component(nam ${QmlFile} NAME)

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${nam}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/
    DEPENDS ${QmlFile}
    COMMENT "Copying ${QmlFile}"
    VERBATIM
  )
  list(APPEND copiedKernels ${CMAKE_CURRENT_BINARY_DIR}/bin/shaders/${nam})
endforeach()

add_library(Shaders SHARED ${shader_src} ${copiedShaders})



#Add additional command dependend on extra compiled libary
#this copies all the changed kernel files to the release/assets folder
 set(copiedResources "")
 foreach(QmlFile ${resources})
    get_filename_component(nam ${QmlFile} NAME)

   add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/bin/assets/${nam}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/bin/assets/
     DEPENDS ${QmlFile}
     COMMENT "Copying ${QmlFile}"
     VERBATIM
   )
   list(APPEND copiedResources ${CMAKE_CURRENT_BINARY_DIR}/bin/assets/${nam})
 endforeach()

add_library(Resources SHARED ${resources} ${copiedResources})


set(copiedDLLResources "")
foreach(QmlFile ${dllresources})
   get_filename_component(nam ${QmlFile} NAME)

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/python/${nam}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QmlFile} ${CMAKE_CURRENT_BINARY_DIR}/python/
    DEPENDS ${QmlFile}
    COMMENT "Copying ${QmlFile}"
    VERBATIM
  )
  list(APPEND copiedDLLResources ${CMAKE_CURRENT_BINARY_DIR}/python/${nam})
endforeach()

add_library(DLLResources SHARED ${dllresources} ${copiedDLLResources})

# ==============================================================================
# INSTALLER CREATION
# ==============================================================================

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "LISEM")
set(CPACK_PACKAGE_VENDOR "Bastian van den Bout")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lisem Integrated Spatial Earth Modeller")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "lisem")
#set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_EXECUTABLES lisem_app "LISEM")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/licence.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${LISEM_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${LISEM_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${LISEM_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "UTwente")
set(CPACK_PACKAGE_VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}.${LISEM_VERSION_PATCH}")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources\\\\lisem_128.bmp")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/lisem.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/lisem.ico")
#Set the programs, directories and files, displayed under the Start menu shortcut.
set(CPACK_NSIS_MENU_LINKS "programs/lisem" "lisem_app")
set(CPACK_NSIS_WELCOME_TITLE "Welcome to LISEM!")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN lisem_app "LISEM")
set(CPACK_NSIS_MODIFY_PATH ON)
include(CPack)



# ==============================================================================
# Python wrapper creation
# ==============================================================================

if(WIN32)

if(LISEM_BUILD_PYTHON)

add_custom_command(OUTPUT deploylisemi
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
                     COMMAND cmd
                     ARGS /c ${CMAKE_CURRENT_BINARY_DIR}/python/wrap.bat
                     DEPENDS lisem_python
                     COMMENT bd1)

add_custom_target(deploylisem DEPENDS deploylisemi)

endif()


endif()



message("${CMAKE_CXX_FLAGS}")
