cmake_minimum_required(VERSION 2.8)

################################
# Simple Demo Model
################################
add_subdirectory(g3m)

set(CMAKE_MODULE_PATH "${GW_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
set(BOOST_ROOT=/usr/local/lib)
set(Boost_NO_SYSTEM_PATHS=TRUE)

find_package(Boost 1.56.0 COMPONENTS system filesystem serialization log REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK")

FIND_PACKAGE(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp")

find_package(Eigen3 REQUIRED)

if (GMP_INCLUDE_DIR AND GMP_LIBRARIES)
    # Already in cache, be silent
    set(GMP_FIND_QUIETLY TRUE)
endif (GMP_INCLUDE_DIR AND GMP_LIBRARIES)

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARIES NAMES gmp libgmp)
find_library(GMPXX_LIBRARIES NAMES gmpxx libgmpxx)
MESSAGE(STATUS "GMP libs: " ${GMP_LIBRARIES} " " ${GMPXX_LIBRARIES})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARIES)

add_library(g3m SHARED
    ${SOURCE}
    ${HEADERS}
)
     
target_link_libraries(g3m pthread ${Boost_LIBRARIES} ${GMP_LIBRARIES})
target_include_directories(g3m PUBLIC ${EIGEN3_INCLUDE_DIR})
set_target_properties(g3m PROPERTIES VERSION "1.0")
install(TARGETS g3m COMPONENT libs DESTINATION bin EXPORT LISEM-targets)



