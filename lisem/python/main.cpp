#include <pybind11/pybind11.h>
#include "string"
#include "pylisem.h"
#include "pylisem_objects.h"
#include "pylisem_spatial.h"
#include "pylisem_io.h"
#include "pylisem_algorithm.h"
#include "pylisem_ui.h"
#include "pylisem_math.h"
#include "pylisem_model.h"

namespace py = pybind11;

//this function provides a lot if functionality
//the macro expands to dll initialization, and python interaction
PYBIND11_MODULE(lisem_python, m) {

    init_pylisem_global(m);
    init_pylisem_math(m);
    init_pylisem_objects(m);
    init_pylisem_spatial(m);
    init_pylisem_io(m);
    init_pylisem_model(m);
    init_pylisem_algorithm(m);
    init_pylisem_ui(m);

}


