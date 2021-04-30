if (NOT CMAKE_CXX_COMPILER_LOADED)
    message(FATAL_ERROR "CheckCXX14Features modules only works if language CXX is enabled")
endif ()

# Determines whether or not the compiler supports C++11
macro(check_for_cxx14_compiler _VAR)
  message(STATUS "Checking for C++14 compiler")
  set(${_VAR})
  if((MSVC AND NOT MSVC_VERSION VERSION_LESS 1800) OR # checking the compiler is at least Visual Studio 2013 - MSVC++ 12
     (CMAKE_COMPILER_IS_GNUCXX AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.6) OR
     (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.1))
    set(${_VAR} 1)
    message(STATUS "Checking for C++14 compiler - available")
  else()
    message(STATUS "Checking for C++14 compiler - unavailable")
  endif()
endmacro()

