file(
  GLOB_RECURSE
  lisem_math_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_math_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
list(REMOVE_ITEM lisem_math_cpp ${REMOVEFILESUNITTEST})

add_library(lisem_math STATIC ${lisem_math_header} ${lisem_math_cpp})
target_compile_features(lisem_math INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_math PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_math PROPERTY FOLDER lisem)

target_link_libraries(lisem_math
#  PUBLIC
#
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
    lisem_core
)
target_include_directories(lisem_math
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>


)

install(TARGETS lisem_math COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


