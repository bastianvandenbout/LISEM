file(
  GLOB_RECURSE
  lisem_model_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_model_cpp
  *.cpp
  *.cl
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_model_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_model STATIC ${lisem_model_header} ${lisem_model_cpp})
target_compile_features(lisem_model INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_model PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_model PROPERTY FOLDER lisem)

target_link_libraries(lisem_model
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

)
target_include_directories(lisem_model
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_model COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


