#ifndef PYLISEM_SPATIAL_H
#define PYLISEM_SPATIAL_H

#include "geo/raster/map.h"
#include "raster/rastermath.h"
#include "raster/rasterconditional.h"
#include "raster/rasterspread.h"
#include "raster/rasterreduce.h"
#include "raster/rastertrigonometric.h"
#include "raster/rasterlimit.h"
#include "raster/rasternetwork.h"
#include "iogdal.h"
#include "raster/rasterderivative.h"
#include "raster/rasterclassified.h"
#include "raster/rasterinterpolation.h"
#include "raster/rasteralgorithms.h"
#include "raster/rasterconvolve.h"
#include "geo/raster/rasteralgebra.h"
#include "raster/rasterconstructors.h"
#include "matrixtable.h"
#include "raster/rastertable.h"
#include "raster/rasterflow.h"
#include "raster/rastershading.h"
#include "raster/rasterartificialflow.h"
#include "raster/rasterstatistics.h"
#include "raster/rasterrandom.h"
#include "raster/resterboussinesq.h"
#include "raster/rasterfourier.h"
#include "raster/rasterkinematic.h"
#include "raster/rasterdeconvolve.h"
#include "raster/rasterwarp.h"
#include "raster/rasterpainting.h"
#include "raster/rasterincompressibleflow.h"
#include "geo/raster/map.h"
#include "raster/rasterconstructors.h"
#include "raster/rasteralgorithms.h"
#include "extensions/gpuscripting.h"
#include "extensions/vectorscripting.h"

#include "linear/lsm_vector2.h"
#include "boundingbox.h"
#include "models/3dmodel.h"
#include "models/lsmmesh.h"
#include "geo/pointcloud/pointcloud.h"

#include "pybind11/pybind11.h"
#include <pybind11/numpy.h>


namespace py = pybind11;

inline void init_pylisem_spatial(py::module &m)
{

     py::class_<cTMap>(m, "Map")
            .def(py::init(py::overload_cast<>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int,float>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int,float,float>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int,float,float,float,float>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int,float,float,float,float,float>(&MapFactory)))
            .def(py::init(py::overload_cast<int,int,float,float,float,float,float,QString>(&MapFactory)))
            .def(py::init(py::overload_cast<int>(&MapFactory)))
            .def(py::init(py::overload_cast<float>(&MapFactory)))
            .def("Rows",&cTMap::Rows)
            .def("Cols",&cTMap::Cols)
            .def("North",&cTMap::North)
            .def("West",&cTMap::West)
            .def("CellSizeX",&cTMap::cellSizeX)
            .def("CellSizeY",&cTMap::cellSizeY)
            .def("__add__",[](cTMap*A,cTMap *B){return A->OpAdd(B);},py::is_operator())
            .def("__sub__",[](cTMap*A,cTMap *B){return A->OpSub(B);},py::is_operator())
            .def("__mul__",[](cTMap*A,cTMap *B){return A->OpMul(B);},py::is_operator())
            .def("__pow__",[](cTMap*A,cTMap *B){return A->OpPow(B);},py::is_operator())
            .def("__truediv__",[](cTMap*A,cTMap *B){return A->OpDiv(B);},py::is_operator())
            //.def("__floordiv__",[](cTMap*A,cTMap *B){return A->OpAdd(B);},py::is_operator())
            .def("__mod__",[](cTMap*A,cTMap *B){return A->OpMod(B);},py::is_operator())
            .def("__and__",[](cTMap*A,cTMap *B){return A->And(B);},py::is_operator())
            .def("__or__",[](cTMap*A,cTMap *B){return A->Or(B);},py::is_operator())
            .def("__xor__",[](cTMap*A,cTMap *B){return A->Xor(B);},py::is_operator())
            .def("__invert__",[](cTMap*A){return A->Negate();},py::is_operator())
            .def("__lt__",[](cTMap*A,cTMap *B){return A->SmallerThen(B);},py::is_operator())
            .def("__le__",[](cTMap*A,cTMap *B){return A->SmallerEqualThen(B);},py::is_operator())
            .def("__eq__",[](cTMap*A,cTMap *B){return A->EqualTo(B);},py::is_operator())
            .def("__ne__",[](cTMap*A,cTMap *B){return A->NotEqualTo(B);},py::is_operator())
            .def("__gt__",[](cTMap*A,cTMap *B){return A->LargerThen(B);},py::is_operator())
            .def("__ge__",[](cTMap*A,cTMap *B){return A->LargerEqualThen(B);},py::is_operator())

            .def("__add__",[](cTMap*A,float B){return A->OpAdd(B);},py::is_operator())
            .def("__sub__",[](cTMap*A,float B){return A->OpSub(B);},py::is_operator())
            .def("__mul__",[](cTMap*A,float B){return A->OpMul(B);},py::is_operator())
            .def("__pow__",[](cTMap*A,float B){return A->OpPow(B);},py::is_operator())
            .def("__truediv__",[](cTMap*A,float B){return A->OpDiv(B);},py::is_operator())
            //.def("__floordiv__",[](cTMap*A,float B){return A->OpAdd(B);},py::is_operator())
            .def("__mod__",[](cTMap*A,float B){return A->OpMod(B);},py::is_operator())
            .def("__and__",[](cTMap*A,float B){return A->And(B);},py::is_operator())
            .def("__or__",[](cTMap*A,float B){return A->Or(B);},py::is_operator())
            .def("__xor__",[](cTMap*A,float B){return A->Xor(B);},py::is_operator())
            .def("__lt__",[](cTMap*A,float B){return A->SmallerThen(B);},py::is_operator())
            .def("__le__",[](cTMap*A,float B){return A->SmallerEqualThen(B);},py::is_operator())
            .def("__eq__",[](cTMap*A,float B){return A->EqualTo(B);},py::is_operator())
            .def("__ne__",[](cTMap*A,float B){return A->NotEqualTo(B);},py::is_operator())
            .def("__gt__",[](cTMap*A,float B){return A->LargerThen(B);},py::is_operator())
            .def("__ge__",[](cTMap*A,float B){return A->LargerEqualThen(B);},py::is_operator())

             .def("__add__",[](float B,cTMap* A){return A->OpAdd_r(B);},py::is_operator())
             .def("__sub__",[](float B,cTMap* A){return A->OpSub_r(B);},py::is_operator())
             .def("__mul__",[](float B,cTMap* A){return A->OpMul_r(B);},py::is_operator())
             .def("__pow__",[](float B,cTMap* A){return A->OpPow_r(B);},py::is_operator())
             .def("__truediv__",[](float B,cTMap* A){return A->OpDiv_r(B);},py::is_operator())
             //.def("__floordiv__",[](float B,cTMap* A){return A->OpAdd(B);},py::is_operator())
             .def("__mod__",[](float B,cTMap* A){return A->OpMod_r(B);},py::is_operator())
             .def("__and__",[](float B,cTMap* A){return A->And_r(B);},py::is_operator())
             .def("__or__",[](float B,cTMap* A){return A->Or_r(B);},py::is_operator())
             .def("__xor__",[](float B,cTMap* A){return A->Xor_r(B);},py::is_operator())
             .def("__lt__",[](float B,cTMap* A){return A->SmallerThen_r(B);},py::is_operator())
             .def("__le__",[](float B,cTMap* A){return A->SmallerEqualThen_r(B);},py::is_operator())
             .def("__eq__",[](float B,cTMap* A){return A->EqualTo_r(B);},py::is_operator())
             .def("__ne__",[](float B,cTMap* A){return A->NotEqualTo_r(B);},py::is_operator())
             .def("__gt__",[](float B,cTMap* A){return A->LargerThen_r(B);},py::is_operator())
             .def("__ge__",[](float B,cTMap* A){return A->LargerEqualThen_r(B);},py::is_operator())
             .def("__copy__",[](const cTMap * A){return A->GetCopy();});

      m.def("ToNumpy",[](cTMap *m){
         // Allocate and initialize some data; make this big so
         // we can see the impact on the process memory use:
         size_t size = m->nrCols()* m->nrRows();
         double *foo = new double[size];
         for(int r =0; r < m->nrRows(); r++ )
         {
             for(int c = 0; c< m->nrCols(); c++ )
             {

                 foo[(m->nrRows() - 1-r)*m->nrCols() + c ] = m->data[r][c];
             }
         }

         // Create a Python object that will free the allocated
         // memory when destroyed:
         py::capsule free_when_done(foo, [](void *f) {
             double *foo = reinterpret_cast<double *>(f);
             delete[] foo;
         });
         int width = m->nrCols();
         int height = m->nrRows();

         return py::array_t<double>(py::buffer_info(
                                        foo,
                                    sizeof(double), //itemsize
                                    py::format_descriptor<double>::format(),
                                    2, // ndim
                                    std::vector<size_t> {height, width }, // shape
                                    std::vector<size_t> {width * sizeof(double), sizeof(double)})); // strides)
         /*return py::array_t<double>(
             {m->nrCols(),m->nrRows()}, // shape
             {8*m->nrRows(), 8}, // C-style contiguous strides for double
             foo, // the data pointer
             free_when_done); // numpy array references this parent*/
     ;});

     m.def("sin",&AS_Mapsin);
     m.def("cos",&AS_Mapcos);
     m.def("tan",&AS_Maptan);
     m.def("sinh",&AS_Mapsinh);
     m.def("cosh",&AS_Mapcosh);
     m.def("tanh",&AS_Maptanh);
     m.def("asin",&AS_Mapasin);
     m.def("acos",&AS_Mapacos);
     m.def("atan",&AS_Mapatan);
     m.def("atan2",&AS_Mapatan2);
     m.def("abs",&AS_Mapabs);
     m.def("max",&AS_Mapmax);
     m.def("min",&AS_Mapmin);
     m.def("exp",&AS_Mapexp);
     m.def("pow",&AS_Mappow);
     m.def("sqrt",&AS_Mapsqrt);
     m.def("log",&AS_Maplog);
     m.def("log10",&AS_Maplog10);
     m.def("ceil",&AS_Mapceil);
     m.def("floor",&AS_Mapfloor);
     m.def("fraction",&AS_Mapfraction);
     m.def("fdim",&GPUfFdim);
     m.def("fdim",&GPUfrFdim);
     m.def("hypot",&GPUfHypoth);
     m.def("hypot",&GPUfrHypoth);
     m.def("logn",&GPUfLogN);
     m.def("logn",&GPUfrLogN);
     m.def("pow",&GPUfPow);
     m.def("pow",&GPUfrPow);


    py::class_<AS_GPUMap>(m, "GPUMap")
    .def("__assign__",py::overload_cast<AS_GPUMap*>(&AS_GPUMap::AS_Assign))
    .def("__assign__",py::overload_cast<cTMap*>(&AS_GPUMap::AS_Assign))
    .def("__add__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUAdd(m1,m2);})
    .def("__mod__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUMod(m1,m2);})
    .def("__sub__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUSub(m1,m2);})
    .def("__mul__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUMul(m1,m2);})
    .def("__truediv__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUDiv(m1,m2);})
    .def("__pow__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUPow(m1,m2);})
    .def("__eq__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUEq(m1,m2);})
    .def("__lt__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUSmaller(m1,m2);})
    .def("__gt__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUGreater(m1,m2);})
    .def("__and__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUAnd(m1,m2);})
    .def("__or__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUOr(m1,m2);})
    .def("__xor__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUXOr(m1,m2);})
    .def("__invert__",[](AS_GPUMap*m1, AS_GPUMap*m2){return GPUNot(m1,m2);})

    .def("__add__",[](AS_GPUMap*m1, float m2){return GPUfAdd(m1,m2);})
    .def("__mod__",[](AS_GPUMap*m1, float m2){return GPUfMod(m1,m2);})
    .def("__sub__",[](AS_GPUMap*m1, float m2){return GPUfSub(m1,m2);})
    .def("__mul__",[](AS_GPUMap*m1, float m2){return GPUfMul(m1,m2);})
    .def("__truediv__",[](AS_GPUMap*m1, float m2){return GPUfDiv(m1,m2);})
    .def("__pow__",[](AS_GPUMap*m1, float m2){return GPUfPow(m1,m2);})
    .def("__eq__",[](AS_GPUMap*m1, float m2){return GPUfEq(m1,m2);})
    .def("__lt__",[](AS_GPUMap*m1, float m2){return GPUfSmaller(m1,m2);})
    .def("__gt__",[](AS_GPUMap*m1, float m2){return GPUfGreater(m1,m2);})
    .def("__and__",[](AS_GPUMap*m1, float m2){return GPUfAnd(m1,m2);})
    .def("__or__",[](AS_GPUMap*m1, float m2){return GPUfOr(m1,m2);})
    .def("__xor__",[](AS_GPUMap*m1, float m2){return GPUfXOr(m1,m2);})

    .def("__add__",[](float m2,AS_GPUMap*m1){return GPUfrAdd(m1,m2);})
    .def("__mod__",[](float m2,AS_GPUMap*m1){return GPUfrMod(m1,m2);})
    .def("__sub__",[](float m2,AS_GPUMap*m1){return GPUfrSub(m1,m2);})
    .def("__mul__",[](float m2,AS_GPUMap*m1){return GPUfrMul(m1,m2);})
    .def("__truediv__",[](float m2,AS_GPUMap*m1){return GPUfrDiv(m1,m2);})
    .def("__pow__",[](float m2,AS_GPUMap*m1){return GPUfrPow(m2,m1);})
    .def("__eq__",[](float m2,AS_GPUMap*m1){return GPUfrEq(m1,m2);})
    .def("__lt__",[](float m2,AS_GPUMap*m1){return GPUfrSmaller(m1,m2);})
    .def("__gt__",[](float m2,AS_GPUMap*m1){return GPUfrGreater(m1,m2);})
    .def("__and__",[](float m2,AS_GPUMap*m1){return GPUfrAnd(m1,m2);})
    .def("__or__",[](float m2,AS_GPUMap*m1){return GPUfrOr(m1,m2);})
    .def("__xor__",[](float m2,AS_GPUMap*m1){return GPUfrXOr(m1,m2);});


    m.def("CPUToGPU",&CPUToGPU);
    m.def("GPUToCPU",&GPUToCPU);

    m.def("sqrt",&GPUSqrt);
    m.def("exp",&GPUExp);
    m.def("log",&GPULog);
    m.def("sin",&GPUSin);
    m.def("asin",&GPUAsin);
    m.def("cos",&GPUCos);
    m.def("acos",&GPUAcos);
    m.def("tan",&GPUTan);
    m.def("atan",&GPUATan);
    m.def("sinh",&GPUSinh);
    m.def("asinh",&GPUAsinh);
    m.def("cosh",&GPUCosh);
    m.def("acosh",&GPUAcosh);
    m.def("tanh",&GPUTanh);
    m.def("atanh",&GPUATanh);
    m.def("abs",&GPUAbs);
    m.def("floor",&GPUFloor);
    m.def("fraction",&GPUDecimal);
    m.def("smoothstep",&GPUSmoothStep);
    m.def("cbrt",&GPUCbrt);
    m.def("rand",&GPURand);
    m.def("round",&GPURound);

    m.def("min", &GPUMin);
    m.def("max", &GPUMax);
    m.def("sign",&GPUSign);
    m.def("fdim",&GPUFdim);
    m.def("fdim",&GPUfFdim);
    m.def("fdim",&GPUfrFdim);
    m.def("hypot",&GPUHypoth);
    m.def("hypot",&GPUfHypoth);
    m.def("hypot",&GPUfrHypoth);
    m.def("logn",&GPULogN);
    m.def("logn",&GPUfLogN);
    m.def("logn",&GPUfrLogN);
    m.def("pow",&GPUPow);
    m.def("pow",&GPUfPow);
    m.def("pow",&GPUfrPow);



    //GeoProjection Class

    py::class_<GeoProjection>(m, "GeoProjection")
            .def(py::init<>())
            .def(py::init(&GeoProjectionFactory));
    m.def("GeoProjectionFromEPSG",&GetGeoProjectionFromEPSG);
    m.def("GeoProjectionFromWKT",&GetGeoProjectionFromWKT);
    m.def("GeoProjectionFromWGS84UTM",&GetGeoProjectionFromWGS84UTM,py::arg("Zone"),py::arg("is_north"));

    //Shape Class
    py::class_<AS_LSMShape>(m,"Shape")
            .def(py::init<>())
            .def("SetAsPoint",&AS_LSMShape::AS_SetAsPoint)
            .def("SetAsMultiPoint",&AS_LSMShape::AS_SetAsMultiPoint)
            .def("SetAsLine",py::overload_cast<std::vector<LSMVector2>>(&AS_LSMShape::AS_SetAsLine))
            .def("SetAsLine",py::overload_cast<LSMVector2,LSMVector2>(&AS_LSMShape::AS_SetAsLine))
            .def("SetAsMultiLine",py::overload_cast<std::vector<std::vector<LSMVector2>>>(&AS_LSMShape::AS_SetAsMultiLine))
            .def("SetAsPolygon",py::overload_cast<std::vector<LSMVector2>>(&AS_LSMShape::AS_SetAsPolygon))
            .def("SetAsPolygon",py::overload_cast<std::vector<LSMVector2>,std::vector<LSMVector2>>(&AS_LSMShape::AS_SetAsPolygon))
            .def("SetAsMultiPolygon",py::overload_cast<std::vector<std::vector<LSMVector2>>>(&AS_LSMShape::AS_SetAsMultiPolygon))
            .def("SetAsMultiPolygon",py::overload_cast<std::vector<std::vector<LSMVector2>>, std::vector<std::vector<LSMVector2>>>(&AS_LSMShape::AS_SetAsMultiPolygon))
            .def("GetShapeType",&AS_LSMShape::AS_GetShapeType)
            .def("IsMulti",&AS_LSMShape::AS_IsMulti)
            .def("GetSubShapeCount",&AS_LSMShape::GetSubItemCount)
            .def("GetSubShape",&AS_LSMShape::GetSubItem)
            .def("GetRadius",&AS_LSMShape::AS_GetRadius)
            .def("GetVertices",&AS_LSMShape::AS_GetVertices)
            .def("GetInnerRing",&AS_LSMShape::AS_GetInnerRing)
            .def("GetOuterRing",&AS_LSMShape::AS_GetOuterRing)
            .def("GetCentroid",&AS_LSMShape::AS_GetCentroid)
            .def("GetArea",&AS_LSMShape::AS_GetArea)
            .def("GetRegion",&AS_LSMShape::AS_GetBoundingBox)
            .def("Move",&AS_LSMShape::AS_Move)
            .def("Contains",&AS_LSMShape::AS_Contains)
            .def("GetClosestVertex",&AS_LSMShape::AS_GetClosestVertex)
            .def("GetClosestEdgePoint",&AS_LSMShape::AS_GetClosestEdgePoint);


    //Feature Class
    py::class_<Feature>(m,"Feature")
            .def(py::init<>())
            .def("GetShapeCount",&Feature::AS_GetShapeCount)
            .def("GetShape",&Feature::GetShape)
            .def("RemoveShape",&Feature::AS_RemoveShape)
            .def("AddShape",&Feature::AddShape)
            .def("GetCentroid",&Feature::AS_GetCentroid)
            .def("GetArea",&Feature::AS_GetArea)
            .def("GetRegion",&Feature::AS_GetBoundingBox)
            .def("Move",&Feature::AS_Move)
            .def("Contains",&Feature::AS_Contains)
            .def("GetClosestVertex",&Feature::AS_GetClosestVertex)
            .def("GetClosestEdgePoint",&Feature::AS_GetClosestEdgePoint);


    //ShapeLayer Class
    py::class_<ShapeLayer>(m,"ShapeLayer")
            .def(py::init<>())
            .def("GetFeatureCount",&ShapeLayer::AS_GetFeatureCount)
            .def("GetFeature",&ShapeLayer::AS_GetFeature)
            .def("RemoveFeature",&ShapeLayer::AS_RemoveFeature)
            .def("AddFeature",&ShapeLayer::AS_AddFeature)
            .def("GetCentroid",&ShapeLayer::AS_GetCentroid)
            .def("GetArea",&ShapeLayer::AS_GetArea)
            .def("GetRegion",&ShapeLayer::AS_GetBoundingBox)
            .def("Move",&ShapeLayer::AS_Move)
            .def("Contains",&ShapeLayer::AS_Contains)
            .def("GetClosestVertex",&ShapeLayer::AS_GetClosestVertex)
            .def("GetClosestEdgePoint",&ShapeLayer::AS_GetClosestEdgePoint)
            .def("GetAttributeTable",&ShapeLayer::AS_GetAttributes)
            .def("GetAttributeCount",&ShapeLayer::AS_GetAtrributeCount)
            .def("GetAttributeName",&ShapeLayer::AS_GetAttributeName)
            .def("IsAttributeNumber",&ShapeLayer::AS_IsAttributeNumber)
            .def("IsAttributeText",&ShapeLayer::AS_IsAttributeText)
            .def("SetAttributeAsNumber",&ShapeLayer::AS_SetAttributeAsNumber)
            .def("SetAttributeAsText",&ShapeLayer::AS_SetAttributeAsText)
            .def("SetFeatureAttribute",py::overload_cast<int, QString, double>(&ShapeLayer::AS_SetFeatureAttribute),py::arg("Index"),py::arg("Name"),py::arg("Value"))
            .def("SetFeatureAttribute",py::overload_cast<int, QString, QString>(&ShapeLayer::AS_SetFeatureAttribute),py::arg("Index"),py::arg("Name"),py::arg("Value"))
            .def("AddAttributeNumber",&ShapeLayer::AS_AddAttributeNumber)
            .def("AddAttributeText",&ShapeLayer::AS_AddAttributeString);


    //ShapeFile Class
    py::class_<ShapeFile>(m,"ShapeFile")
            .def(py::init<>())
            .def("GetLayerCount", &ShapeFile::AS_GetLayerCount)
            .def("GetLayer", &ShapeFile::AS_GetLayer)
            .def("SetAttribute", &ShapeFile::AS_SetAttributeValue)
            .def("GetTable", py::overload_cast<>(&ShapeFile::AS_GetTable))
            .def("GetAttributeTable", py::overload_cast<QString>(&ShapeFile::AS_GetTable))
            .def("SetTable", &ShapeFile::AS_SetTable)
            .def("__and__", [](ShapeFile *A, ShapeFile * B){ return AS_And(A,B) ;})
            .def("__or__", [](ShapeFile *A, ShapeFile * B){ return AS_Or(A,B) ;})
            .def("__xor__", [](ShapeFile *A, ShapeFile * B){ return AS_Xor(A,B) ;})
            .def("__sub__", [](ShapeFile *A, ShapeFile * B){ return AS_Sub(A,B) ;})
            .def("__add__", [](ShapeFile *A, ShapeFile * B){ return AS_Add(A,B) ;});

    //region class
    py::class_<BoundingBox>(m,"Region")
            .def(py::init<>())
            .def(py::init<float,float,float,float>())
            .def("GetMinX",&BoundingBox::GetMinX)
            .def("GetMinY",&BoundingBox::GetMinY)
            .def("GetMaxX",&BoundingBox::GetMaxX)
            .def("GetMaxY",&BoundingBox::GetMaxY);


    py::class_<PointCloud>(m,"PointCloud")
            .def(py::init<>());


    py::class_<ModelGeometry>(m,"Object")
            .def(py::init<>());


    py::class_<LSMMesh>(m,"Mesh")
            .def(py::init<>());


}

#endif // PYLISEM_SPATIAL_H
