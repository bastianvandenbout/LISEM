file(
  GLOB_RECURSE
  lisem_algorithm_header
  *.h
  *.hpp
)
file(
  GLOB_RECURSE
  lisem_algorithm_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_algorithm_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_algorithm STATIC ${lisem_algorithm_header} ${lisem_algorithm_cpp})
target_compile_features(lisem_algorithm INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_algorithm PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_algorithm PROPERTY FOLDER lisem)

target_link_libraries(lisem_algorithm
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_core
    lisem_math
    lisem_object
    lisem_spatial
    lisem_io
    lisem_glcl
    lisem_model

)
target_include_directories(lisem_algorithm
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_algorithm COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


