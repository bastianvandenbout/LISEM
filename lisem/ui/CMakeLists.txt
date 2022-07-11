file(
  GLOB_RECURSE
  lisem_ui_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_ui_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_ui_cpp ${REMOVEFILESUNITTEST})
endif()

add_library(lisem_ui STATIC ${lisem_ui_header} ${lisem_ui_cpp})
target_compile_features(lisem_ui INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_ui PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_ui PROPERTY FOLDER lisem)

target_link_libraries(lisem_ui
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_core
    lisem_math
    lisem_object
    lisem_spatial
    lisem_io
    lisem_audio
    lisem_glcl
    lisem_model
    lisem_algorithm
    lisem_scripting
)



target_include_directories(lisem_ui
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_ui COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


