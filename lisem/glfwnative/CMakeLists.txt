file(
  GLOB_RECURSE
  lisem_glfwnative_header
  *.h
  *.hpp
)
file(
  GLOB_RECURSE
  lisem_glfwnative_cpp
  *.cpp
)
file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_glfwnative_cpp ${REMOVEFILESUNITTEST})
endif()


add_library(lisem_glfwnative STATIC ${lisem_glfwnative_header} ${lisem_glfwnative_cpp})
target_compile_features(lisem_glfwnative INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_glfwnative PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_glfwnative PROPERTY FOLDER lisem)

target_link_libraries(lisem_glfwnative
#  PUBLIC
#
  PRIVATE
    lisem_core
  PUBLIC
    glad
    ${GLFW_LIBRARIES}
    

)
target_include_directories(lisem_glfwnative
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

  PUBLIC
     ${GLFW_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>
     


)

install(TARGETS lisem_glfwnative COMPONENT libs DESTINATION bin EXPORT LISEM-targets)


