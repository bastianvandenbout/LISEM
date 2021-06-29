
file(
  GLOB_RECURSE
  lisem_python_header
  *.h
)
file(
  GLOB_RECURSE
  lisem_python_cpp
  *.cpp
)
file(
  GLOB_RECURSE
  lisem_python_py
  *.py
)



file(GLOB_RECURSE REMOVEFILESUNITTEST *_test.cpp)
#Remove the future main files

if(NOT ${REMOVEFILESUNITTEST})
list(REMOVE_ITEM lisem_python_cpp ${REMOVEFILESUNITTEST})
endif()


SET(PYBIND11_FINDPYTHON ON)

SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
find_package(pybind11 REQUIRED)
pybind11_add_module(lisem_python
        ${lisem_python_cpp}
        ${lisem_python_header}
        ${lisem_python_py}
)

target_link_libraries(lisem_python
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

install(TARGETS lisem_python COMPONENT pythonbindings DESTINATION python EXPORT LISEM-bindings)


SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
