file(
  GLOB_RECURSE
  lisem_glcl_header
  *.h
  *.hpp
)
file(
  GLOB_RECURSE
  lisem_glcl_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_glcl_cpp ${REMOVEFILESUNITTEST})
endif()


add_library(lisem_glcl STATIC ${lisem_glcl_header} ${lisem_glcl_cpp})
target_compile_features(lisem_glcl INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_glcl PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_glcl PROPERTY FOLDER lisem)

target_link_libraries(lisem_glcl
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}

    lisem_core
    lisem_glfwnative
    lisem_math
    lisem_io
    lisem_object
    lisem_spatial

  PUBLIC
    glad

)
target_include_directories(lisem_glcl
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_glcl COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


