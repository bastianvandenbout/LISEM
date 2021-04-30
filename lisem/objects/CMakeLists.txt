file(
  GLOB_RECURSE
  lisem_object_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_object_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_object_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_object STATIC ${lisem_object_header} ${lisem_object_cpp})
target_compile_features(lisem_object INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_object PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_object PROPERTY FOLDER lisem)

target_link_libraries(lisem_object
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_core
    lisem_math
)
target_include_directories(lisem_object
  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>

)
target_compile_features(lisem_object PUBLIC ${LISEM_CXX_FEATURES})

install(TARGETS lisem_object COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


