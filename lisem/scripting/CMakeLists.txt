file(
  GLOB_RECURSE
  lisem_scripting_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_scripting_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_scripting_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_scripting STATIC ${lisem_scripting_header} ${lisem_scripting_cpp})
target_compile_features(lisem_scripting INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_scripting PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_scripting PROPERTY FOLDER lisem)

target_link_libraries(lisem_scripting
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
    lisem_algorithm

)
target_include_directories(lisem_scripting
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_scripting DESTINATION bin EXPORT LISEM-targets)


