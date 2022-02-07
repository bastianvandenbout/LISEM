#ifndef PYLISEM_ALGORITHM_H
#define PYLISEM_ALGORITHM_H
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
#include "raster/rastersupervisedclassify.h"
#include "raster/rastersegmentize.h"
#include "geo/rastercontour.h"
#include "pybind11/pybind11.h"
#include <pybind11/numpy.h>
#include "geo/rasterize.h"
#include "geo/vectorize.h"
#include "geo/vectorwarp.h"
#include "geo/vectorvonoroi.h"
#include "geo/vectorrastersample.h"
#include "raster/rasterlinesegmentdetect.h"
#include "geo/gridding.h"
#include "geo/simplify.h"
#include "geo/segmentize.h"
#include "raster/rasterradar.h"
#include "raster/rasternoisereduce.h"
#include "raster/rasterclassified.h"
#include "raster/rastersoil.h"
#include "raster/rasterseismic.h"
#include "extensions/scriptoptimization.h"

namespace py = pybind11;

inline void init_pylisem_algorithm(py::module &m)
{


    m.def("Cover",py::overload_cast<cTMap*,cTMap*>(&AS_Cover));
    m.def("Cover",py::overload_cast<cTMap*,float>(&AS_Cover));
    m.def("Cover",py::overload_cast<float,cTMap*>(&AS_Cover));
    m.def("Cover",py::overload_cast<float,float>(&AS_Cover));
    m.def("SetMV",py::overload_cast<cTMap*,cTMap*>(&AS_SetMV));
    m.def("SetMV",py::overload_cast<cTMap*,float>(&AS_SetMV));
    m.def("GetMVVal",&AS_GetMVVal);
    m.def("IsMVVal",&AS_IsValMV);


    m.def("MapTotal",&AS_MapTotal);
    m.def("MapTotalRed",&AS_MapTotalRed);
    m.def("MapAverage",&AS_MapAverage);
    m.def("MapAverageRed",&AS_MapAverageRed);
    m.def("MapMinimum",&AS_MapMinimum);
    m.def("MapMinimumRed",&AS_MapMinimumRed);
    m.def("MapMaximum",&AS_MapMaximum);
    m.def("MapMaximumRed",&AS_MapMaximumRed);

    m.def("MapIf",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<cTMap*,cTMap*,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<cTMap*,float,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<cTMap*,float,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<float,cTMap*,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<float,cTMap*,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<float,float,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));
    m.def("MapIf",py::overload_cast<float,float,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"),py::arg("iffalse"));

    m.def("MapIf",py::overload_cast<cTMap*,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"));
    m.def("MapIf",py::overload_cast<cTMap*,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"));
    m.def("MapIf",py::overload_cast<float,cTMap*>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"));
    m.def("MapIf",py::overload_cast<float,float>(&AS_MapIf), py::arg("Condition"),py::arg("iftrue"));


    m.def("Spread",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_Spread), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("Spread",py::overload_cast<cTMap*,float,cTMap*>(&AS_Spread), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("Spread",py::overload_cast<cTMap*,cTMap*,float>(&AS_Spread), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("Spread",py::overload_cast<cTMap*,float,float>(&AS_Spread), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadZone",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_SpreadZone), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadZone",py::overload_cast<cTMap*,float,cTMap*>(&AS_SpreadZone), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadZone",py::overload_cast<cTMap*,cTMap*,float>(&AS_SpreadZone), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadZone",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_SpreadZone), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));

    m.def("SlopeLength",py::overload_cast<cTMap*, cTMap*, cTMap *,bool>(&AS_SlopeLength), py::arg("LDD"),py::arg("Initial"),py::arg("Friction"),py::arg("TwoWay") = false);
    m.def("SlopeLength",py::overload_cast<cTMap*, float, cTMap *,bool>(&AS_SlopeLength), py::arg("LDD"),py::arg("Initial"),py::arg("Friction"),py::arg("TwoWay") = false);
    m.def("SlopeLength",py::overload_cast<cTMap*, cTMap*, float,bool>(&AS_SlopeLength), py::arg("LDD"),py::arg("Initial"),py::arg("Friction"),py::arg("TwoWay") = false);
    m.def("SlopeLength",py::overload_cast<cTMap*, float,float,bool>(&AS_SlopeLength), py::arg("LDD"),py::arg("Initial"),py::arg("Friction"),py::arg("TwoWay") = false);

    m.def("SpreadNetwork",py::overload_cast<cTMap*,cTMap *,cTMap*,cTMap*>(&AS_SpreadLDD), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetwork",py::overload_cast<cTMap*,cTMap *,float,cTMap*>(&AS_SpreadLDD), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetwork",py::overload_cast<cTMap*,cTMap *,cTMap*,float>(&AS_SpreadLDD), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetwork",py::overload_cast<cTMap*,cTMap *,float,float>(&AS_SpreadLDD), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetworkZone",py::overload_cast<cTMap*,cTMap *,cTMap*,cTMap*>(&AS_SpreadLDDZone),py::arg("LDD"), py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetworkZone",py::overload_cast<cTMap*,cTMap *,float,cTMap*>(&AS_SpreadLDDZone), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetworkZone",py::overload_cast<cTMap*,cTMap *,cTMap*,float>(&AS_SpreadLDDZone), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));
    m.def("SpreadNetworkZone",py::overload_cast<cTMap*,cTMap *,cTMap*,cTMap*>(&AS_SpreadLDDZone), py::arg("LDD"),py::arg("Source"),py::arg("Initial"),py::arg("Friction"));

    m.def("ViewShed",&AS_ViewShed,py::arg("DEM"),py::arg("x"),py::arg("y"), py::arg("height") = 0.0f);
    m.def("ViewAngle",&AS_ViewAngle,py::arg("DEM"),py::arg("x"),py::arg("y"), py::arg("height") = 0.0f);
    m.def("ViewCriticalAngle",&AS_ViewCriticalAngle,py::arg("DEM"),py::arg("x"),py::arg("y"), py::arg("height") = 0.0f);

    m.def("Slope",&AS_Slope);
    m.def("Aspect",&AS_Aspect);
    m.def("GradientX",&AS_SlopeX);
    m.def("GradientXX",&AS_SlopeDx2);
    m.def("GradientY",&AS_SlopeY);
    m.def("GradientYY",&AS_SlopeDy2);
    m.def("CurvPlanar",&AS_PlanarCurvature);
    m.def("CurvProfile",&AS_ProfileCurvature);

    m.def("DrainageNetwork",py::overload_cast<cTMap*>(&AS_DrainageNetwork),py::arg("Elevation"));

    m.def("DrainageNetwork",py::overload_cast<cTMap*>(&AS_DrainageNetwork),py::arg("Elevation"));
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,cTMap*,cTMap*,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,cTMap*,cTMap*,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,cTMap*,float,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,float,cTMap*,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,cTMap*,cTMap*,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,cTMap*,cTMap*,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,cTMap*,float,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,float,float,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,float,cTMap*,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,cTMap*,float,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,float,cTMap*,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,cTMap*,float,float,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,float,cTMap*,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,float,float,cTMap*, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetwork",py::overload_cast<cTMap*,float,float,cTMap*,float, bool>(&AS_DrainageNetwork),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);

    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*>(&AS_DrainageNetworkDEM),py::arg("Elevation"));
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,cTMap*,cTMap*,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,cTMap*,cTMap*,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,cTMap*,float,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,float,cTMap*,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,cTMap*,cTMap*,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,cTMap*,cTMap*,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,cTMap*,float,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,float,float,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,float,cTMap*,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,cTMap*,float,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,float,cTMap*,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,cTMap*,float,float,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,float,cTMap*,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,float,float,cTMap*, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);
    m.def("DrainageNetworkDEM",py::overload_cast<cTMap*,float,float,cTMap*,float, bool>(&AS_DrainageNetworkDEM),py::arg("Elevation"),py::arg("Outflowdepth"),py::arg("CoreVolume"),py::arg("CoreArea"),py::arg("Precipitation"), py::arg("lddin") = true);


    m.def("Accuflux",py::overload_cast<cTMap*>(&AS_Accuflux),py::arg("LDD"));
    m.def("Accuflux",py::overload_cast<cTMap*,float>(&AS_Accuflux),py::arg("LDD"),py::arg("Source"));
    m.def("Accuflux",py::overload_cast<cTMap*,cTMap*>(&AS_Accuflux),py::arg("LDD"),py::arg("Source"));

    m.def("AccufluxLim",py::overload_cast<cTMap*,cTMap*>(&AS_AccufluxThreshold),py::arg("LDD"),py::arg("Limit"));
    m.def("AccufluxLim",py::overload_cast<cTMap*,float,cTMap*>(&AS_AccufluxThreshold),py::arg("LDD"),py::arg("Limit"),py::arg("Source"));
    m.def("AccufluxLim",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_AccufluxThreshold),py::arg("LDD"),py::arg("Limit"),py::arg("Source"));

    m.def("DownStream",&AS_DownStream,py::arg("LDD"),py::arg("Value"));
    m.def("UpStream",&AS_UpStream,py::arg("LDD"),py::arg("Value"));

    m.def("DrainageMonotonicDownstream",&AS_DrainageMonotonicDownstream,py::arg("LDD"),py::arg("Value"));

    m.def("Outlets",&AS_Outlets,py::arg("LDD"));
    m.def("Catchments",py::overload_cast<cTMap*>(&AS_Catchments),py::arg("LDD"));
    m.def("Catchments",py::overload_cast<cTMap*,cTMap*>(&AS_Catchments),py::arg("LDD"),py::arg("Outlets"));

    m.def("StreamOrder",&AS_StreamOrder);

    m.def("AreaArea",&AS_MapAreaArea,py::arg("Class"));
    m.def("AreaAverage",&AS_MapAreaAverage,py::arg("Class"),py::arg("Value"));
    m.def("AreaTotal",&AS_MapAreaTotal,py::arg("Class"),py::arg("Value"));
    m.def("AreaMinimum",&AS_MapAreaMinimum,py::arg("Class"),py::arg("Value"));
    m.def("AreaMaximum",&AS_MapAreaMaximum,py::arg("Class"),py::arg("Value"));

    m.def("InverseDistance", py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_InverseDistance),py::arg("Points"),py::arg("Values"),py::arg("Power"));
    m.def("InverseDistance", py::overload_cast<cTMap*,cTMap*,float>(&AS_InverseDistance),py::arg("Points"),py::arg("Values"),py::arg("Power"));

    m.def("SpreadOne",&AS_SpreadOne);
    m.def("ShiftMV",&AS_ShiftMV);
    m.def("Shift",py::overload_cast<cTMap*,int,int,float>(&AS_Shift), py::arg("Value"),py::arg("x"),py::arg("y"),py::arg("fill"));
    m.def("Shift",py::overload_cast<cTMap*,int,int>(&AS_Shift), py::arg("Value"),py::arg("x"),py::arg("y"));
    m.def("XIndex",&AS_XIndex);
    m.def("YIndex",&AS_YIndex);
    m.def("XCoord",&AS_XCoord);
    m.def("YCoord",&AS_YCoord);
    m.def("Clone",&AS_Clone);

    m.def("RasterDeconvolve",&AS_RasterDeconvolve,py::arg("Value"),py::arg("Iterations"),py::arg("Sigma"));
    m.def("GaussianDistribute",py::overload_cast<cTMap*,cTMap*,cTMap*>(&AS_GaussianInfluence),py::arg("Values"),py::arg("Range"),py::arg("Power"));
    m.def("GaussianDistribute",py::overload_cast<cTMap*,float,cTMap*>(&AS_GaussianInfluence),py::arg("Values"),py::arg("Range"),py::arg("Power"));
    m.def("GaussianDistribute",py::overload_cast<cTMap*,cTMap*,float>(&AS_GaussianInfluence),py::arg("Values"),py::arg("Range"),py::arg("Power"));
    m.def("GaussianDistribute",py::overload_cast<cTMap*,float,float>(&AS_GaussianInfluence),py::arg("Values"),py::arg("Range"),py::arg("Power"));

    m.def("Clump",&AS_Clump,py::arg("Values"),py::arg("Threshold"),py::arg("Adaptive"));
    m.def("UniqueCellID",&AS_UniqueID,py::arg("Values"));

    m.def("WindowAverage",py::overload_cast<cTMap*,cTMap*>(&AS_WindowAverage),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowAverage",py::overload_cast<cTMap*,float>(&AS_WindowAverage),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowVariance",py::overload_cast<cTMap*,cTMap*>(&AS_WindowVariance),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowVariance",py::overload_cast<cTMap*,float>(&AS_WindowVariance),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMajority",py::overload_cast<cTMap*,cTMap*>(&AS_WindowMajority),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMajority",py::overload_cast<cTMap*,float>(&AS_WindowMajority),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowDiversity",py::overload_cast<cTMap*,cTMap*>(&AS_WindowDiversity),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowDiversity",py::overload_cast<cTMap*,float>(&AS_WindowDiversity),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowTotal",py::overload_cast<cTMap*,cTMap*>(&AS_WindowTotal),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowTotal",py::overload_cast<cTMap*,float>(&AS_WindowTotal),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMaximum",py::overload_cast<cTMap*,cTMap*>(&AS_WindowMaximum),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMaximum",py::overload_cast<cTMap*,float>(&AS_WindowMaximum),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMinimum",py::overload_cast<cTMap*,cTMap*>(&AS_WindowMinimum),py::arg("Value"),py::arg("WindowSize"));
    m.def("WindowMinimum",py::overload_cast<cTMap*,float>(&AS_WindowMinimum),py::arg("Value"),py::arg("WindowSize"));

    m.def("Erode",&AS_Erode,py::arg("Value"),py::arg("Erosion"));
    m.def("Dilute",&AS_Dilute,py::arg("Value"),py::arg("Dilution"));

    m.def("WindowPointShift",&AS_RasterShiftToOther,py::arg("Mask"),py::arg("Points"),py::arg("window"));
    m.def("FlowDiffusiveMax",&AS_DiffusiveMaxWave,py::arg("Elevation"),py::arg("Height"),py::arg("Iterations"),py::arg("Courant"));
    m.def("FlowDiffusiveMaxCG",&AS_DiffusiveMaxWaveCG,py::arg("Elevation"),py::arg("Height"),py::arg("CGSloep"),py::arg("slopefactor"),py::arg("Iterations") = 10,py::arg("Courant") = 0.1);
    m.def("FlowDiffusiveMax",&AS_DiffusiveMaxWave,py::arg("Elevation"),py::arg("Height"),py::arg("Iterations"),py::arg("Courant"));

    m.def("FlowDynamic",&AS_DynamicWave,py::arg("Elevation"),py::arg("Manning"),py::arg("Height"),py::arg("VX"),py::arg("VY"),py::arg("dt"), py::arg("Courant") = 0.1);
    m.def("Diffusion",&AS_Diffusion,py::arg("Values"),py::arg("Coefficient"),py::arg("dt"),py::arg("iter") = 1);


    m.def("Random",&RandomFloat,py::arg("min"),py::arg("max"));
    m.def("RandomNormal",&RandomNormalFloat,py::arg("mean"),py::arg("sigma"));
    m.def("RandomMap", &AS_MapRandom,py::arg("Mask"),py::arg("min") = 0.0,py::arg("max") = 1.0);
    m.def("RandomNormalMap", &AS_MapRandomNormal,py::arg("Mask"),py::arg("mean") = 0.0 ,py::arg("sigma")=1.0);
    m.def("RandomAreaMap", &AS_MapAreaRandom,py::arg("Mask"),py::arg("min") = 0.0,py::arg("max") = 1.0);
    m.def("RandomNormalAreaMap", &AS_MapAreaRandomNormal,py::arg("Mask"),py::arg("mean") = 0.0 ,py::arg("sigma")=1.0);

    m.def("PerlinNoise",&AS_PerlinNoise,py::arg("Mask"),py::arg("Wavelength"));
    m.def("GaborNoise",&AS_GaborNoise,py::arg("Mask"),py::arg("dirx"),py::arg("diry"),py::arg("scalex"),py::arg("scaley"));
    m.def("Voronoise",&AS_Voronoise,py::arg("Mask"),py::arg("scalex"),py::arg("scaley"), py::arg("n_points") = 1);


    m.def("SuperResolutionSISR",&AS_SuperResolutionSISR,py::arg("Values"),py::arg("upscale") = 2.0,py::arg("iterations")=1, py::arg("tilesize") = 512, py::arg("patchsize") = 4, py::arg("PatchOverlap") = 3, py::arg("Lambda") = 1e-12, py::arg("Neighborhoodsize") = 200, py::arg("Neighborhoodweight") = 1.0,py::arg("Wavelet") = false);
    m.def("DistanceTransform",&AS_DistanceTransform,py::arg("Mask"));
    m.def("BilateralFilter",&AS_BilateralFilter,py::arg("Values"),py::arg("Size"), py::arg("sigma_space"),py::arg("sigma_intensity"));

    m.def("Hillshade",&AS_HillShade,py::arg("Elevation"),py::arg("Scale"));
    m.def("RasterFourier",&AS_RasterFourier);
    m.def("RasterInverseFourier",&AS_RasterInverseFourier,py::arg("Real"),py::arg("Complex"));

    m.def("RasterCohensKappa",&AS_RasterCohensKappa,py::arg("Obsered"),py::arg("Model"));
    m.def("RasterContinuousCohensKappa",&AS_RasterContinuousCohensKappa,py::arg("Obsered"),py::arg("Model"),py::arg("Threshold"));


    m.def("RasterWarp",py::overload_cast<cTMap*,std::vector<cTMap*>,QString>(&AS_RasterWarp),py::arg("Target"),py::arg("Sources"),py::arg("Interpolation") =QString("bilinear"));
    m.def("RasterWarp",py::overload_cast<cTMap*,cTMap*,QString>(&AS_RasterWarp),py::arg("Target"),py::arg("Source"),py::arg("Interpolation") =QString("bilinear"));

    m.def("SupervisedClassify",&AS_SupervisedClassify, py::arg("Input"), py::arg("Training"));
    m.def("UnSupervisedClassify",&AS_UnSupervisedClassify, py::arg("Input"), py::arg("n_Classes"), py::arg("Mask"));

    m.def("SegmentizeMS",py::overload_cast<cTMap*,float,float,float,float,float>(&AS_RasterSegmentize),py::arg("Values"),py::arg("SpatialRadius"),py::arg("RangeRadius"),py::arg("MinRegionSize"), py::arg("MaxIterations"),py::arg("Threshold"));
    m.def("SegmentizeMS",py::overload_cast<std::vector<cTMap*>,float,float,float,float,float>(&AS_RasterSegmentize),py::arg("Values"),py::arg("SpatialRadius"),py::arg("RangeRadius"),py::arg("MinRegionSize"), py::arg("MaxIterations"),py::arg("Threshold"));
    m.def("SegmentizeWS",py::overload_cast<cTMap*,float,float>(&AS_RasterSegmentizeWS),py::arg("Values"),py::arg("Threshold") = 0.01f,py::arg("Level") = 0.1f);

    m.def("SlopeMagnitude",&AS_RasterGradientMagnitude,py::arg("Values"));

    m.def("AnisotropicFilter", py::overload_cast<cTMap*,int, float, float>(&AS_RasterAnisotropicDiffusion),py::arg("Values"),py::arg("Iterations") = 100, py::arg("conductance") = 0.2f, py::arg("Timestep") = 0.125f);
    m.def("AnisotropicFilter", py::overload_cast<std::vector<cTMap*>,int, float, float>(&AS_RasterAnisotropicDiffusion),py::arg("Values"),py::arg("Iterations") = 100, py::arg("conductance") = 0.2f, py::arg("Timestep") = 0.125f);

    m.def("ClassToRGB",&AS_ClassToRGB,py::arg("Class"));
    m.def("ClassToRGB",& AS_RasterClassRegularize,py::arg("Class"), py::arg("Radius"));

    m.def("Rasterize",&AS_Rasterize,py::arg("Target"),py::arg("ShapeFile"),py::arg("Attribute"),py::arg("do_burn"),py::arg("burn_value") = 1.0, py::arg("invert") = false, py::arg("all_touched") = false, py::arg("additive") = false);
    m.def("RasterContour", py::overload_cast<cTMap*,float,float,QString>(&AS_RasterContour), py::arg("Values"),py::arg("Interval"),py::arg("Start") = 0.0f,py::arg("AttributeName"));

    m.def("Vectorize",&AS_Vectorize,py::arg("Raster"),py::arg("AttributeName")= QString("value"),py::arg("do_diagonal") = false);
    m.def("VectorizePoints",&AS_VectorizePoints, py::arg("Raster"),py::arg("AttributeName") = QString("id"));

    m.def("VectorWarp",&AS_VectorWarp,py::arg("Shapes"),py::arg("CRS"));
    m.def("Segmentize",&AS_Segmentize,py::arg("Shapes"),py::arg("DistanceMax"));
    m.def("Simplify",&AS_Simplify,py::arg("Shapes"),py::arg("DistanceTreshold"));

    m.def("Voronoi",&AS_Vonoroi,py::arg("Shapes"),py::arg("Centroids"));
    m.def("VectorToPoints",&AS_ShapeToPoints,py::arg("Shapes"),py::arg("Centroids"), py::arg("extentscale") = 1.0f);

    m.def("LineSegmentDetect",&AS_LineSegmentDetect,py::arg("Raster"));
    m.def("CircleDetect",&AS_RasterCircleDetect,py::arg("Raster"),py::arg("minradius") = 0.0,py::arg("maxradius") = 1e30,py::arg("threshold") = 100,py::arg("subdivide") = 20);

    m.def("Kriging",&AS_Kriging,py::arg("Raster"),py::arg("MaxDistance"));
    m.def("KrigingGaussian",&AS_KrigingGaussianCovariance,py::arg("Raster"),py::arg("MaxDistance"),py::arg("C0"),py::arg("C1"));

    m.def("RasterProject", py::overload_cast<std::vector<cTMap*>,GeoProjection*,QString>(&AS_RasterProject),py::arg("Raster"),py::arg("CRS"),py::arg("Interpolation") = QString("bilinear"));
    m.def("RasterProject", py::overload_cast<cTMap*,GeoProjection*,QString>(&AS_RasterProject),py::arg("Raster"),py::arg("CRS"),py::arg("Interpolation") = QString("bilinear"));

    m.def("NoiseFilterFrost",&AS_NoiseFilterFrost, py::arg("Raster"),py::arg("Radius"),py::arg("Deramp") = 0.1);
    m.def("NoiseFilterLee",&AS_NoiseFilterLee, py::arg("Raster"),py::arg("Radius"),py::arg("nlooks") = 1.0);
    m.def("NoiseFilterGamma",&AS_NoiseFilterGamma, py::arg("Raster"),py::arg("Radius"),py::arg("nlooks") = 1.0);
    m.def("NoiseFilterKuan",&AS_NoiseFilterKuan, py::arg("Raster"),py::arg("Radius"),py::arg("nlooks") = 1.0);

    m.def("NLMeans", &AS_FastNLMeansFilter, py::arg("Raster"),py::arg("WindowSize") = 7, py::arg("cutoff") = 1.0);
    m.def("RegionMerge", py::overload_cast<cTMap*, float>(&AS_ClassMerge), py::arg("Classes"), py::arg("MinArea"));
    m.def("RegionSimilarityMerge",py::overload_cast<cTMap*,std::vector<cTMap*>, float>(&AS_ClassMerge), py::arg("Classes"), py::arg("Similarities"), py::arg("MinArea"));

    m.def("ResampleLinear", &AS_ReSampleLinear,py::arg("Raster"),py::arg("Scale"));
    m.def("ResampleCubic", &AS_ReSampleCubic,py::arg("Raster"),py::arg("Scale"));
    m.def("ResampleNearest", &AS_ReSampleNearest,py::arg("Raster"),py::arg("Scale"));
    m.def("ResampleSparse", &AS_ReSampleSparse,py::arg("Raster"),py::arg("Scale"));

    m.def("LinearCorrect", &AS_ValueCorrectLinear, py::arg("Reference"),py::arg("Values"), py::arg("PixelDist"));


    m.def("KuwaharaFilter",py::overload_cast<cTMap*, cTMap*>(&AS_KuwaharaFilter),py::arg("Raster"),py::arg("WindowSize"));
    m.def("KuwaharaFilter",py::overload_cast<cTMap*, float>(&AS_KuwaharaFilter),py::arg("Raster"),py::arg("WindowSize"));

    m.def("InpaintRFSR",&AS_ResampleRFSR, py::arg("Raster"),py::arg("Iterations")= 0,py::arg("FFTSize") = 0,py::arg("intrinsicsamplingsize") = 0, py::arg("correct_dist") = 0,py::arg("rho") = 1.0, py::arg("blocksize") = 16,py::arg("blendscale") = 1.0);
    m.def("InpaintNS",&AS_ResampleNS, py::arg("Raster"), py::arg("radius") = 10.0);
    m.def("InpaintTELEA",&AS_ResampleTELEA, py::arg("Raster"),py::arg("radius") = 10.0);

    m.def("SetCellSize",&AS_SetRasterCellSize, py::arg("Raster"),py::arg("dx"), py::arg("dy") = 0.0);
    m.def("SetULC",&AS_SetRasterULC, py::arg("Raster"),py::arg("x"), py::arg("y"));
    m.def("CellSize",&AS_GetRasterCellSize);
    m.def("ULC",&AS_GetRasterULC);


    m.def("GetCRS",py::overload_cast<ShapeFile*>(&AS_GetCRS),py::arg("ShapeFile"));
    m.def("SetCRS",py::overload_cast<ShapeFile*, GeoProjection*>(&AS_SetCRS),py::arg("ShapeFile"),py::arg("CRS"));
    m.def("GetRegion",py::overload_cast<ShapeFile*>(&AS_GetRegion),py::arg("ShapeFile"));
    m.def("GetCRS",py::overload_cast<cTMap*>(&AS_GetCRS),py::arg("Map"));
    m.def("SetCRS",py::overload_cast<cTMap*, GeoProjection*>(&AS_SetCRS),py::arg("Map"),py::arg("CRS"));
    m.def("GetRegion",py::overload_cast<cTMap*>(&AS_GetRegion),py::arg("Map"));

    m.def("ClassExtent", &AS_GetClassExtent,py::arg("Classes"), py::arg("classval"));
    m.def("ClassCellExtent", &AS_GetClassPixelExtent,py::arg("Classes"), py::arg("classval"));

    m.def("SaxtonKSat", &AS_SaxtonKSat,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("densityfactor") = 1.0);
    m.def("SaxtonPorosity", &AS_SaxtonPorosity,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("densityfactor") = 1.0);
    m.def("SaxtonFieldCapacity", &AS_SaxtonFieldCapacity,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("densityfactor") = 1.0);
    m.def("SaxtonSuction", &AS_SaxtonSuction,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("Saturation"),py::arg("densityfactor") = 1.0);
    m.def("SaxtonA", &AS_SaxtonA,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("densityfactor") = 1.0);
    m.def("SaxtonB", &AS_SaxtonB,py::arg("Sand"),py::arg("Clay"),py::arg("Organic"),py::arg("Gravel"),py::arg("densityfactor") = 1.0);
    m.def("MedianGrainSize", &AS_MedianGrainSize,py::arg("Sand"),py::arg("Clay"));
    m.def("TextureClass", &AS_TextureClass,py::arg("Sand"),py::arg("Clay"));

    m.def("RasterPaintShapes",&AS_PaintShapeFile,py::arg("Target"),py::arg("Shapefile"),py::arg("Val1"),py::arg("Val2"),py::arg("Operation"));
    m.def("RasterPaintShape",&AS_PaintShape,py::arg("Target"),py::arg("Shape"),py::arg("Val1"),py::arg("Val2"),py::arg("Operation"));
    m.def("RasterPaintPolygon",&AS_PaintPolygon,py::arg("Target"),py::arg("Points"),py::arg("Val1"),py::arg("Val2"),py::arg("Operation"));
    m.def("RasterPaintLines",&AS_PaintLines,py::arg("Target"),py::arg("Points"),py::arg("Size"), py::arg("Val1"),py::arg("Val2"),py::arg("Feather"),py::arg("Operation"));
    m.def("RasterPaintEllips",&AS_PaintEllipsoid,py::arg("Target"),py::arg("x"),py::arg("y"),py::arg("rx"),py::arg("ry"),py::arg("Val1"),py::arg("Val2"),py::arg("Feather"),py::arg("Operation"));
    m.def("RasterPaintRectangle",&AS_PaintRectangle,py::arg("Target"),py::arg("x"),py::arg("y"),py::arg("rx"),py::arg("ry"),py::arg("Val1"),py::arg("Val2"),py::arg("Feather"),py::arg("Operation"));

    m.def("WaveEquation",&AS_WaveEquation,py::arg("Displacement"),py::arg("PreviousDisplacement"),py::arg("c"),py::arg("dt"));

    m.def("RasterFromTableValues",&AS_RasterFromTableHeader,py::arg("Table"),py::arg("RowValue"),py::arg("ColumnValue"));
    m.def("RasterFromTable",&AS_RasterFromTableHeader,py::arg("Table"),py::arg("RowIndex"),py::arg("ColumnIndex"));
    m.def("RasterClasses",&AS_RasterTableClasses, py::arg("Classes"));
    m.def("RasterAverage",&AS_RasterTableAverage, py::arg("Classes"),py::arg("Values"));
    m.def("RasterMinimum",&AS_RasterTableMinimum, py::arg("Classes"),py::arg("Values"));
    m.def("RasterMaximum",&AS_RasterTableMaximum, py::arg("Classes"),py::arg("Values"));
    m.def("RasterTotal",&AS_RasterTableTotal, py::arg("Classes"),py::arg("Values"));

    m.def("FlowBoussinesq",&AS_BoussinesqWave,py::arg("Elevation"),py::arg("Manning"),py::arg("Height"),py::arg("VX"),py::arg("VY"),py::arg("dt"),py::arg("courant")=0.1);
    m.def("FlowSteady",&AS_SteadyStateWave,py::arg("Elevation"),py::arg("Manning"),py::arg("Source"),py::arg("Sink"),py::arg("iterations")=100,py::arg("Courant") = 0.1);
    m.def("FlowTsunami",&AS_TsunamiFlow,py::arg("Elevation"),py::arg("Height"),py::arg("VX"), py::arg("VY"),py::arg("Manning"),py::arg("Timestep") = 0.1);
    m.def("FlowNavierStokes",&AS_NavierStokesWave,py::arg("Density"),py::arg("VX"),py::arg("VY"),py::arg("visc"),py::arg("diff"),py::arg("Timestep")=0.1,py::arg("Courant")=0.25);
    m.def("FlowKinematic",&AS_FlowKinematic,py::arg("LDD"),py::arg("Slope"),py::arg("Width"),py::arg("Manning"),py::arg("Height"),py::arg("Timestep"));

    m.def("OptimizeCustom",py::overload_cast<std::vector<double>,std::function<double(std::vector<double>)>,double,double,int>(&OptimizeCustom2),py::arg("Initial"),py::arg("Function"),py::arg("step"),py::arg("gradientstep"),py::arg("maxiterations"));
    m.def("OptimizeCustom",py::overload_cast<std::vector<double>,std::vector<bool>,std::function<double(std::vector<double>)>,double,double,int>(&OptimizeCustom2),py::arg("Initial"),py::arg("positive"),py::arg("Function"),py::arg("step"),py::arg("gradientstep"),py::arg("maxiterations"));

    m.def("SteadyStateSoil",&AS_SteadyStateSoil,py::arg("DEM"),py::arg("Source"),py::arg("QS"), py::arg("iter"));
    //m.def("Accuflux2D",&AS_AccuFluxDiffusive,py::arg("DEM"),py::arg("Source"),py::arg("FlowSource"),py::arg("Iterations"),py::arg("Courant"),py::arg("Scale"));


    /*sm->RegisterGlobalFunction("Table @IDFCurves(array<double> & in precipitation, double timestep, bool is_intensities, int n_durations, array<double> & in return_periods)",asFUNCTIONPR(CreateIDF,(CScriptArray*, double , bool, int, CScriptArray* ),MatrixTable*),asCALL_CDECL);  assert( r >= 0 );
    sm->RegisterGlobalFunction("Table @ABHyetograph(Table & in idf)",asFUNCTIONPR(CreateAlternatingBlockHyetograph,(MatrixTable*),MatrixTable*),asCALL_CDECL);  assert( r >= 0 );

    sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y, double guess_c1 , double guess_c2)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*,double,double),CScriptArray*),asCALL_CDECL);

    sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*),CScriptArray*),asCALL_CDECL);



    r = engine->RegisterGlobalFunction("Map @FlowTransient(const Map &in DEM, const Map &in HSoil, const Map &in KSAT, const Map &in Porosity, const Map &in gwh, float dt, bool inflowlimit = true)", asFUNCTIONPR(    AS_TransientFlow,(cTMap *,cTMap *,cTMap*,cTMap *, cTMap*,float,bool),cTMap *),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("array<Map> @FlowIncompressible(const Map &in M, const Map &in U, const Map &in U, const Map &in P, const Map &in LS, const Map &in Block,const Map &in BlockU,const Map &in BlockV, float dt, float visc,float courant = 0.2, float beta0 = 1.7)", asFUNCTIONPR(    AS_IncompressibleWave,(cTMap *,cTMap *,cTMap*,cTMap *,cTMap *,cTMap *, cTMap *,cTMap *,float,float, float,float),std::vector<cTMap*>),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("Map @MVMap(const Map &in M)", asFUNCTIONPR(    AS_MVMap,(cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @VectorRasterSample(const Map &in raster, Shapes &in shapes)", asFUNCTIONPR( AS_VectorRasterSample,(cTMap *,ShapeFile *),MatrixTable*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @DrainageNetworkToShapes(const Map &in ldd)", asFUNCTIONPR( AS_DrainageNetworkToShapes,(cTMap *),ShapeFile*),  asCALL_CDECL);
    */





    /*
             *

   r = engine->RegisterGlobalFunction("array<Map> @VoxelMaskDem(array<Map> &in state, float zmin, float zmax, const Map &in elevation)", asFUNCTIONPR(    AS_VoxelMaskDem,(std::vector<cTMap*>,float,float,cTMap*),std::vector<cTMap*>),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("array<Map> @FlowNavierStokes3D(array<Map> &in state, float zmin, float zmax, float visc, float diff, float timestep = 0.1, float courant = 0.25)", asFUNCTIONPR(    AS_NavierStokesWave3D,(std::vector<cTMap*>,float,float,float,float,float,float),std::vector<cTMap*>),  asCALL_CDECL); assert( r >= 0 );;


    r = engine->RegisterGlobalFunction("array<Map> @FlowIncompressible3D(array<Map> &in state, array<Map> &in Block,array<Map> &in BlockU,array<Map> &in BlockV,array<Map> &in BlockW, float csz, float dt, float visc,float courant = 0.2, float beta0 = 1.7)", asFUNCTIONPR(    AS_IncompressibleFlow3D,(std::vector<cTMap*>,std::vector<cTMap*>,std::vector<cTMap*>,std::vector<cTMap*>,std::vector<cTMap*>,float,float,float, float,float),std::vector<cTMap*>),  asCALL_CDECL); assert( r >= 0 );;



    r = engine->RegisterGlobalFunction("MathExpression SemiVariogram(const Map &in data, float distance_max, int maxpoints = 0)", asFUNCTIONPR( AS_SemiVarioGram,(cTMap *,float,int),MathFunction),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @PointCloudToDEM(PointCloud &in value, float resolution)",asFUNCTIONPR(PointCloudToDEM,(PointCloud*,float),cTMap *),asCALL_CDECL);

     r = engine->RegisterGlobalFunction("array<Map> @RasterOrthoRectify(array<Map> &in source, const Map &in DEM)", asFUNCTIONPR( AS_RasterOrthoRectify,(std::vector<cTMap*>,cTMap*),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("array<Map> @RasterRegister(array<Map> &in source, array<vec2> &in pixpos, array<vec2> &in worldpos)", asFUNCTIONPR( AS_RasterRegister,(std::vector<cTMap*>,std::vector<LSMVector2>,std::vector<LSMVector2>),std::vector<cTMap*>),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("array<Map> @RasterSpectralUnmix(array<Map> &in source, array<array<float>> &in spectra, string method = \"UCLS\")", asFUNCTIONPR( AS_SpectralUnmix,(std::vector<cTMap *>, std::vector<std::vector<double>>, QString ),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("int RasterEstimateEndmemberCount(array<Map> &in source, string method = \"HFC-VD\")", asFUNCTIONPR( AS_SpectralEndmemberEstimate,(std::vector<cTMap*>,QString),int),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("array<array<float>> @RasterVCA(array<Map> &in source, int count)", asFUNCTIONPR( AS_SpectralVCA,(std::vector<cTMap*>,int),std::vector<std::vector<float>>),  asCALL_CDECL);


    //Photogrammetry code

    r = engine->RegisterGlobalFunction("void PhotogrammetryListPrepare(array<string> &in files,string listfile =\"photogrammetry_list.txt\",double focal_pixels = -1.0,bool use_pos = false)", asFUNCTIONPR( AS_Photogrammetry_PrepareList,(std::vector<QString>,QString,double,bool),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryFeatureCompute(string listfile =\"photogrammetry_list.txt\", string outputdir = \"temp_phtgrm\",string method = \"SIFT\", float threshold = 1.0, string order = \"HIGH\")", asFUNCTIONPR( AS_Photogrammetry_FeatureCompute,(QString,QString,QString,float,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryFeatureMatch(string listfile, string outputdir = \"temp_phtgrm\", string geometrymodel = \"f\")", asFUNCTIONPR( AS_Photogrammetry_FeatureMatch,(QString,QString,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryStructureCompute(string listfile, string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_Photogrammetry_IncrementalSFM,(QString,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryRegister(array<vec3> &in gcp, array<string> &in image1, array<vec2> &in image1_pos, array<string> &in image2, array<vec2> &in image2_pos, string listfile =\"photogrammetry_list.txt\",string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_Photogrammetry_Register,(std::vector<LSMVector3>,std::vector<QString>,std::vector<LSMVector2>,std::vector<QString>,std::vector<LSMVector2>,QString, QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryDensify(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryDensifyPointCloud,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMesh(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryToMesh,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMeshRefine(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryRefineMesh,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMeshTexture(string outputdir =\"temp_phtgrm\", int ortho_resolution= 2000)",asFUNCTIONPR( AS_PhotogrammetryTextureMesh,(QString, int),void),  asCALL_CDECL);


    //PanSharpen
    r = engine->RegisterGlobalFunction("array<Map> @PanSharpenLMVM(const Map &in pan, array<Map> &in bands)",asFUNCTIONPR( AS_PanSharpenLMVM,(cTMap * ,std::vector<cTMap*>),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("array<Map> @PanSharpenBayesian(const Map &in pan, array<Map> &in bands, float Lambda = 0.9999, float S = 1.0)",asFUNCTIONPR( AS_PanSharpenBayesian,(cTMap * ,std::vector<cTMap*>,float,float),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("array<Map> @PanSharpenRCS(const Map &in pan, array<Map> &in bands)",asFUNCTIONPR( AS_PanSharpenRCS,(cTMap * ,std::vector<cTMap*>),std::vector<cTMap*>),  asCALL_CDECL);



    //disparity
    r = engine->RegisterGlobalFunction("array<Map> @OpticalFlow(const Map &in first, const Map &in second, float windowsize = 15.0)",asFUNCTIONPR( AS_OpticalFlow,(cTMap * ,cTMap *,float),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @Disparity(const Map &in first, const Map &in second, float windowsize = 5, float max_disp = 100)",asFUNCTIONPR( AS_RasterDisparity,(cTMap * ,cTMap *,float,float),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @DisparityFine(const Map &in first, const Map &in second, string method = \"SSDivMean\",float mindisp=-100, float maxdisp= 100)",asFUNCTIONPR( AS_RasterDisparityDetailed,(cTMap * ,cTMap *,QString,float,float),cTMap*),  asCALL_CDECL);



    //metadata
    //r = engine->RegisterGlobalFunction("array<string> @MapMetaData(string file, int band = 0)",asFUNCTIONPR( AS_GetRasterMetaData,(QString, int),std::vector<cTMap*>),  asCALL_CDECL);
    //r = engine->RegisterGlobalFunction("array<string> @MapMetaDataAbsPath(string file, int band = 0)",asFUNCTIONPR( AS_GetRasterMetaDataAbsPath,(QString, int),std::vector<cTMap*>),  asCALL_CDECL);

    //sar
    r = engine->RegisterGlobalFunction("Map &SARCalibrate(array<Map> &in radar, bool lut = true, bool noise = false)",asFUNCTIONPR( AS_SARCalibrate,(std::vector<cTMap*>,bool,bool),cTMap *),  asCALL_CDECL);



    r = engine->RegisterGlobalFunction("Map @XSlice(array<Map> &in val, int row, double zmin, double zmax, int levelinterval = 1, int leveloffset = 0)",asFUNCTIONPR( AS_GetXSlice,(std::vector<cTMap*>, int , double , double,int,int),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @YSlice(array<Map> &in val, int row, double zmin, double zmax, int levelinterval = 1, int leveloffset = 0)",asFUNCTIONPR( AS_GetYSlice,(std::vector<cTMap*>, int , double , double,int,int),cTMap *),  asCALL_CDECL);

    engine->RegisterGlobalFunction("array<Map> @SlopeStabilityIS(const Map & in elevation, const Map &in SoilDepth, const Map &in Cohesion, const Map&in InternalFrictionAngle, const Map &in Density, const Map&in WaterHeight, float sfmax)",asFUNCTIONPR( AS_SlopeStabilityIS,(cTMap*,cTMap*,cTMap*,cTMap*,cTMap*,cTMap*,float),std::vector<cTMap*>),  asCALL_CDECL);
    engine->RegisterGlobalFunction("array<Map> @SlopeStabilityRES(const Map & in elevation, const array<Map> &in SoilDepth, const array<Map> &in Cohesion, const array<Map> &in InternalFrictionAngle, const array<Map> &in Density, const array<Map> &in Saturation, const Map&in WaterHeight, float sample_density, float h_min, float h_max, float size_min, float size_max, float size_lat_min, float size_lat_max, float size_vert_min, float size_vert_max, float rot_min, float rot_max, float rot_lat_min, float rot_lat_max, float vol_min)",asFUNCTION( AS_SlopeStabilityRES),  asCALL_CDECL);

    engine->RegisterGlobalFunction("Map @TerrainErode(const Map &in DEM, float p_erode, float p_deposit, float p_gravity, float p_tc, float p_evapo, float p_intertia, float p_minslope, float radius)", asFUNCTION(AS_LandscapeErode),asCALL_CDECL);



    */


}

#endif // PYLISEM_ALGORITHM_H
