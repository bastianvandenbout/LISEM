file(
  GLOB_RECURSE
  lisem_app_calc_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_app_calc_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_app_calc_cpp ${REMOVEFILESUNITTEST})
# WIN32
add_executable(lisem_app_calc ${lisem_app_calc_header} ${lisem_app_calc_cpp}  ${WIN_RESOURCE_FILE})
target_compile_features(lisem_app INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_app_calc PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_app_calc PROPERTY FOLDER lisem)

target_link_libraries(lisem_app_calc
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
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


)
target_include_directories(lisem_app_calc
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_app_calc COMPONENT app DESTINATION bin EXPORT LISEM-targets)


