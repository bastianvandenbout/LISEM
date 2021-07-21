file(
  GLOB_RECURSE
  lisem_app_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_app_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_app_cpp ${REMOVEFILESUNITTEST})
endif()
add_executable(lisem_app WIN32 ${lisem_app_header} ${lisem_app_cpp}  ${WIN_RESOURCE_FILE})
target_compile_features(lisem_app INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_app PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_app PROPERTY FOLDER lisem)

target_link_libraries(lisem_app
  PRIVATE
    lisem_core
    lisem_math
    lisem_object
    lisem_spatial
    lisem_io
    lisem_algorithm
    lisem_model
    lisem_scripting
    lisem_glcl
    lisem_ui
    
    ${LISEM_EXTERNAL_LIBS}


)
target_include_directories(lisem_app
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_app COMPONENT app DESTINATION bin EXPORT LISEM-targets)

target_compile_definitions(lisem_app PUBLIC EIGEN_MAX_ALIGN_BYTES=0)

get_target_property(APPDEFINES lisem_app INTERFACE_COMPILE_DEFINITIONS)
get_target_property(APPDEFINES2 lisem_app COMPILE_DEFINITIONS)

message(${APPDEFINES})
message(${APPDEFINES2})
