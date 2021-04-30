file(
  GLOB_RECURSE
  lisem_io_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_io_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_io_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_io ${lisem_io_header} ${lisem_io_cpp})
target_compile_features(lisem_io INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_io PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_io PROPERTY FOLDER lisem)

target_link_libraries(lisem_io
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_math
    lisem_core
    lisem_object
    lisem_spatial

)
target_include_directories(lisem_io
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_io COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


