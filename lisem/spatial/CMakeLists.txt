file(
  GLOB_RECURSE
  lisem_spatial_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_spatial_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_spatial_cpp ${REMOVEFILESUNITTEST})
endif()
add_library(lisem_spatial STATIC ${lisem_spatial_header} ${lisem_spatial_cpp})
target_compile_features(lisem_spatial INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_spatial PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_spatial PROPERTY FOLDER lisem)

target_link_libraries(lisem_spatial
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_core
    lisem_math
    lisem_object
)
target_include_directories(lisem_spatial
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_spatial COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


