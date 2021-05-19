file(
  GLOB_RECURSE
  lisem_core_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_core_cpp
  *.cpp
)
message(${lisem_core_cpp})

file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files
if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_core_cpp ${REMOVEFILESUNITTEST})
endif()
add_library(lisem_core STATIC ${lisem_core_header} ${lisem_core_cpp})
target_compile_features(lisem_core INTERFACE ${LISEM_CXX_FEATURES})
set_target_properties(lisem_core PROPERTIES SOVERSION ${LISEM_VERSION_MAJOR} VERSION "${LISEM_VERSION_MAJOR}.${LISEM_VERSION_MINOR}")
set_property(TARGET lisem_core PROPERTY FOLDER lisem)

target_link_libraries(lisem_core
#  PUBLIC
#    
  PRIVATE
    ${LISEM_EXTERNAL_LIBS}
)
target_include_directories(lisem_core
  PUBLIC
     ${LISEM_EXTERNAL_INCLUDE_DIRS}
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
     $<INSTALL_INTERFACE:include>
     $<INSTALL_INTERFACE:include/LISEM>

)

target_compile_features(lisem_core PUBLIC ${LISEM_CXX_FEATURES})

install(TARGETS lisem_core COMPONENT libs DESTINATION lib EXPORT LISEM-targets)


