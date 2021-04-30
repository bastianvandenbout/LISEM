add_library(glad STATIC)

target_sources(glad
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c>
    )

target_include_directories(glad
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
install(TARGETS glad DESTINATION lib EXPORT LISEM-targets)
