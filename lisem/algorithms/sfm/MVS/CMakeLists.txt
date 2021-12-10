# Find required packages

set(CGAL_ROOT "c:/apps/msys64/mingw64/")
set(CGAL_FOUND TRUE)
include_directories(${CGAL_INCLUDE_DIRS})
link_directories(${CGAL_LIBRARY_DIRS})
#FIND_PACKAGE(CGAL ${SYSTEM_PACKAGE_REQUIRED})
#if(CGAL_FOUND)
#	include_directories(${CGAL_INCLUDE_DIRS})
#	add_definitions(${CGAL_DEFINITIONS})
#	link_directories(${CGAL_LIBRARY_DIRS})
#endif()

set(VCG_ROOT "c:/apps/vcglib")
set(VCG_FOUND TRUE)
include_directories("C:/Apps/vcglib/")

#FIND_PACKAGE(VCG ${SYSTEM_PACKAGE_REQUIRED})
#if(VCG_FOUND)
#	include_directories(${VCG_INCLUDE_DIRS})
#	add_definitions(${VCG_DEFINITIONS})
#endif()

set(CERES_LIBS "")
if(OpenMVS_USE_CERES)
	FIND_PACKAGE(CERES)
	if(CERES_FOUND)
		include_directories(${CERES_INCLUDE_DIRS})
		add_definitions(${CERES_DEFINITIONS})
	else()
		set(OpenMVS_USE_CERES OFF)
	endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld") # generate debug information#-fuse-ld=lld
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld") # generate debug information#-fuse-ld=lld

# List sources files
FILE(GLOB PCH_C "Common.cpp")

FILE(GLOB LIBRARY_FILES_C "*.cpp")
FILE(GLOB LIBRARY_FILES_H "*.h" "*.inl")

# Place Common.cpp as the first file in the list
# needed by cotire when setting PCH manually
#LIST(REMOVE_ITEM LIBRARY_FILES_C ${PCH_C})
#SET(LIBRARY_FILES_C "${PCH_C};${LIBRARY_FILES_C}")

#cxx_library_with_type_no_pch(MVS "Libs" "" "${cxx_default}"
#	${LIBRARY_FILES_C} ${LIBRARY_FILES_H}
#)

add_library(MVS SHARED ${LIBRARY_FILES_C} ${LIBRARY_FILES_H})

# Manually set Common.h as the precompiled header
#set_target_pch(MVS Common.h)



# Link its dependencies
TARGET_LINK_LIBRARIES(MVS Common Math IO ${CERES_LIBS} ${CGAL_LIBS} "c:/msys64/mingw64/lib/libgmp.a" "c:/msys64//mingw64/lib/libgmpxx.a")

# Install
SET_TARGET_PROPERTIES(MVS PROPERTIES
	PUBLIC_HEADER "${LIBRARY_FILES_H}")
#INSTALL(TARGETS MVS
#	EXPORT OpenMVSTargets
#	RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
#	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
 #       PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/MVS" COMPONENT dev)
