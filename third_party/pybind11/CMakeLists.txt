add_library(pybind INTERFACE)

file(
  GLOB_RECURSE
  pybind_header
  *.h
  *.hpp
)

target_include_directories(pybind
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
install(TARGETS pybind DESTINATION lib EXPORT LISEM-targets)
