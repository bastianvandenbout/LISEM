#pragma once


#include "error.h"

#include <utility>

#include <QtCore/QString>

#include "pybind11/pybind11.h"
#include "pybind11/operators.h"

#include "linear/lsm_vector.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"

namespace py = pybind11;



static void init_pylisem_math(py::module &m)
{



    py::class_<LSMVector2>(m,"vec2")
            .def(py::init<float, float>())
            .def(py::self + py::self)
            .def(float() * py::self)
            .def(py::self * float())
            .def(-py::self)
            .def_property("x",&LSMVector2::getX,&LSMVector2::setX)
            .def_property("y",&LSMVector2::getY,&LSMVector2::setY);

    py::class_<LSMVector3>(m,"vec3")
            .def(py::init<float, float,float>())
            .def(py::self + py::self)
            .def(float() * py::self)
            .def(py::self * float())
            .def(-py::self)
            .def_property("x",&LSMVector3::getX,&LSMVector3::setX)
            .def_property("y",&LSMVector3::getY,&LSMVector3::setY)
            .def_property("z",&LSMVector3::getZ,&LSMVector3::setZ);


    m.def("cross",&LSMVector3::CrossProduct);
    m.def("dot",&LSMVector3::VDot);
    m.def("normalize",&LSMVector3::VNormalize);
    m.def("length",&LSMVector3::VLength);
    m.def("distance",&LSMVector3::VDistance);
    m.def("reflect",&LSMVector3::VReflect);
    m.def("refract",&LSMVector3::VRefract);


    py::class_<LSMVector4>(m,"vec4");

    py::class_<LSMMatrix4x4>(m,"mat4x4");


}
