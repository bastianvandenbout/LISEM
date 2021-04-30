# -*- coding: utf-8 -*-


from ._version import version_info, __version__
import matplotlib.pyplot as pp
from IPython.display import Image
import PIL


# -*- coding: utf-8 -*-
import sys
import sysconfig
import os

print("=========================================")
print("Importing LISEM")

try:  # to use Python 3.8's DLL handling
	os.add_dll_directory(os.path.dirname(os.path.realpath(__file__))+ "\\")
	print("added dir " + os.path.dirname(os.path.realpath(__file__))+ "\\")
except AttributeError:  # <3.8, not supported
	print("Python version < 3.8, using path to find dll automatically")
	sys.path.insert(0, os.path.abspath(os.path.dirname(os.path.realpath(__file__)) +  os.path.sep))
	os.environ['PATH'] += os.pathsep + os.path.dirname(os.path.realpath(__file__)) +  os.path.sep
	print("adding to environment PATH variable:  " + os.path.dirname(os.path.realpath(__file__)) +  os.path.sep)
	

import lisem.lisem
import lisem.lisem_python as lsm

print("Succesfully imported LISEM")
print("=========================================")

Accuflux = lsm.Accuflux
AccufluxLim = lsm.AccufluxLim
AnisotropicFilter = lsm.AnisotropicFilter
AreaArea = lsm.AreaArea
AreaAverage = lsm.AreaAverage
AreaMaximum = lsm.AreaMaximum
AreaMinimum = lsm.AreaMinimum
AreaTotal = lsm.AreaTotal
Aspect = lsm.Aspect
BilateralFilter = lsm.BilateralFilter
CPUToGPU = lsm.CPUToGPU
Catchments = lsm.Catchments
CellSize = lsm.CellSize
CircleDetect = lsm.CircleDetect
ClassCellExtent = lsm.ClassCellExtent
ClassExtent = lsm.ClassExtent
ClassToRGB = lsm.ClassToRGB
Clone = lsm.Clone
Clump = lsm.Clump
CopyFile = lsm.CopyFile
Cover = lsm.Cover
CurvPlanar = lsm.CurvPlanar
CurvProfile = lsm.CurvProfile
Diffusion = lsm.Diffusion
Dilute = lsm.Dilute
DistanceTransform = lsm.DistanceTransform
DownStream = lsm.DownStream
DrainageMonotonicDownstream = lsm.DrainageMonotonicDownstream
DrainageNetwork = lsm.DrainageNetwork
DrainageNetworkDEM = lsm.DrainageNetworkDEM
DropColumn = lsm.DropColumn
DropRow = lsm.DropRow
Erode = lsm.Erode
ExtractFile = lsm.ExtractFile
Feature = lsm.Feature
FlowBoussinesq = lsm.FlowBoussinesq
FlowDiffusiveMax = lsm.FlowDiffusiveMax
FlowDiffusiveMaxCG = lsm.FlowDiffusiveMaxCG
FlowDynamic = lsm.FlowDynamic
FlowKinematic = lsm.FlowKinematic
FlowNavierStokes = lsm.FlowNavierStokes
FlowSteady = lsm.FlowSteady
FlowTsunami = lsm.FlowTsunami
GPUMap = lsm.GPUMap
GPUToCPU = lsm.GPUToCPU
GaborNoise = lsm.GaborNoise
GaussianDistribute = lsm.GaussianDistribute
GeoPlot = lsm.GeoPlot
GeoProjection = lsm.GeoProjection
GeoProjectionFromEPSG = lsm.GeoProjectionFromEPSG
GeoProjectionFromWGS84UTM = lsm.GeoProjectionFromWGS84UTM
GeoProjectionFromWKT = lsm.GeoProjectionFromWKT
GetCRS = lsm.GetCRS
GetColumn = lsm.GetColumn
GetFileDir = lsm.GetFileDir
GetFileExt = lsm.GetFileExt
GetFileName = lsm.GetFileName
GetFilesInDir = lsm.GetFilesInDir
GetLastError = lsm.GetLastError
GetMVVal = lsm.GetMVVal
GetMapListName = lsm.GetMapListName
GetMapListNames = lsm.GetMapListNames
GetRegion = lsm.GetRegion
GetWorkDir = lsm.GetWorkDir
GradientX = lsm.GradientX
GradientXX = lsm.GradientXX
GradientY = lsm.GradientY
GradientYY = lsm.GradientYY
Hillshade = lsm.Hillshade
InpaintNS = lsm.InpaintNS
InpaintRFSR = lsm.InpaintRFSR
InpaintTELEA = lsm.InpaintTELEA
InverseDistance = lsm.InverseDistance
IsMVVal = lsm.IsMVVal
Kriging = lsm.Kriging
KrigingGaussian = lsm.KrigingGaussian
KuwaharaFilter = lsm.KuwaharaFilter
LineSegmentDetect = lsm.LineSegmentDetect
LinearCorrect = lsm.LinearCorrect
LisemException = lsm.LisemException
LoadMap = lsm.LoadMap
LoadMapAbsPath = lsm.LoadMapAbsPath
LoadMapBandList = lsm.LoadMapBandList
LoadObjectCloud = lsm.LoadObjectCloud
LoadObjectCloudAbsPath = lsm.LoadObjectCloudAbsPath
LoadObjecttCloudAbsPath = lsm.LoadObjecttCloudAbsPath
LoadPointCloud = lsm.LoadPointCloud
LoadPointCloudAbsPath = lsm.LoadPointCloudAbsPath
LoadShapes = lsm.LoadShapes
LoadShapesAbsPath = lsm.LoadShapesAbsPath
LoadTable = lsm.LoadTable
LoadTableAbsPath = lsm.LoadTableAbsPath
Map = lsm.Map
MapAverage = lsm.MapAverage
MapAverageRed = lsm.MapAverageRed
MapIf = lsm.MapIf
MapMaximum = lsm.MapMaximum
MapMaximumRed = lsm.MapMaximumRed
MapMinimum = lsm.MapMinimum
MapMinimumRed = lsm.MapMinimumRed
MapTotal = lsm.MapTotal
MapTotalRed = lsm.MapTotalRed
MedianGrainSize = lsm.MedianGrainSize
Mesh = lsm.Mesh
ModelResult = lsm.ModelResult
NLMeans = lsm.NLMeans
NoiseFilterFrost = lsm.NoiseFilterFrost
NoiseFilterGamma = lsm.NoiseFilterGamma
NoiseFilterKuan = lsm.NoiseFilterKuan
NoiseFilterLee = lsm.NoiseFilterLee
Object = lsm.Object
Outlets = lsm.Outlets
PearsonCorrelation = lsm.PearsonCorrelation
PerlinNoise = lsm.PerlinNoise
PointCloud = lsm.PointCloud
Random = lsm.Random
RandomAreaMap = lsm.RandomAreaMap
RandomMap = lsm.RandomMap
RandomNormal = lsm.RandomNormal
RandomNormalAreaMap = lsm.RandomNormalAreaMap
RandomNormalMap = lsm.RandomNormalMap
RasterAverage = lsm.RasterAverage
RasterClasses = lsm.RasterClasses
RasterCohensKappa = lsm.RasterCohensKappa
RasterContinuousCohensKappa = lsm.RasterContinuousCohensKappa
RasterContour = lsm.RasterContour
RasterDeconvolve = lsm.RasterDeconvolve
RasterFourier = lsm.RasterFourier
RasterFromTable = lsm.RasterFromTable
RasterFromTableValues = lsm.RasterFromTableValues
RasterInverseFourier = lsm.RasterInverseFourier
RasterMaximum = lsm.RasterMaximum
RasterMinimum = lsm.RasterMinimum
RasterPaintEllips = lsm.RasterPaintEllips
RasterPaintLines = lsm.RasterPaintLines
RasterPaintPolygon = lsm.RasterPaintPolygon
RasterPaintRectangle = lsm.RasterPaintRectangle
RasterPaintShape = lsm.RasterPaintShape
RasterPaintShapes = lsm.RasterPaintShapes
RasterProject = lsm.RasterProject
RasterTotal = lsm.RasterTotal
RasterWarp = lsm.RasterWarp
Rasterize = lsm.Rasterize
Region = lsm.Region
RegionMerge = lsm.RegionMerge
RegionSimilarityMerge = lsm.RegionSimilarityMerge
ResampleCubic = lsm.ResampleCubic
ResampleLinear = lsm.ResampleLinear
ResampleNearest = lsm.ResampleNearest
ResampleSparse = lsm.ResampleSparse
RunModel = lsm.RunModel
SaveMap = lsm.SaveMap
SaveMapAbsPath = lsm.SaveMapAbsPath
SaveShapes = lsm.SaveShapes
SaveShapesAbsPath = lsm.SaveShapesAbsPath
SaveTable = lsm.SaveTable
SaveTableAbsPath = lsm.SaveTableAbsPath
SaxtonA = lsm.SaxtonA
SaxtonB = lsm.SaxtonB
SaxtonFieldCapacity = lsm.SaxtonFieldCapacity
SaxtonKSat = lsm.SaxtonKSat
SaxtonPorosity = lsm.SaxtonPorosity
SaxtonSuction = lsm.SaxtonSuction
Segmentize = lsm.Segmentize
SegmentizeMS = lsm.SegmentizeMS
SegmentizeWS = lsm.SegmentizeWS
SetCRS = lsm.SetCRS
SetCellSize = lsm.SetCellSize
SetMV = lsm.SetMV
SetULC = lsm.SetULC
SetWorkDir = lsm.SetWorkDir
Shape = lsm.Shape
ShapeFile = lsm.ShapeFile
ShapeLayer = lsm.ShapeLayer
Shift = lsm.Shift
ShiftMV = lsm.ShiftMV
Simplify = lsm.Simplify
Slope = lsm.Slope
SlopeLength = lsm.SlopeLength
SlopeMagnitude = lsm.SlopeMagnitude
SortByColumn = lsm.SortByColumn
SortColumn = lsm.SortColumn
SpearmanCorrelation = lsm.SpearmanCorrelation
Spread = lsm.Spread
SpreadNetwork = lsm.SpreadNetwork
SpreadNetworkZone = lsm.SpreadNetworkZone
SpreadOne = lsm.SpreadOne
SpreadZone = lsm.SpreadZone
StreamOrder = lsm.StreamOrder
SuperResolutionSISR = lsm.SuperResolutionSISR
SupervisedClassify = lsm.SupervisedClassify
Table = lsm.Table
TableTypeToDouble = lsm.TableTypeToDouble
TableTypeToInt = lsm.TableTypeToInt
TableTypeToString = lsm.TableTypeToString
TextureClass = lsm.TextureClass
ToNumpy = lsm.ToNumpy
ULC = lsm.ULC
UnSupervisedClassify = lsm.UnSupervisedClassify
UniqueCellID = lsm.UniqueCellID
UpStream = lsm.UpStream
VectorToPoints = lsm.VectorToPoints
VectorWarp = lsm.VectorWarp
Vectorize = lsm.Vectorize
VectorizePoints = lsm.VectorizePoints
ViewAngle = lsm.ViewAngle
ViewCriticalAngle = lsm.ViewCriticalAngle
ViewShed = lsm.ViewShed
Voronoi = lsm.Voronoi
Voronoise = lsm.Voronoise
WaveEquation = lsm.WaveEquation
WindowAverage = lsm.WindowAverage
WindowDiversity = lsm.WindowDiversity
WindowMajority = lsm.WindowMajority
WindowMaximum = lsm.WindowMaximum
WindowMinimum = lsm.WindowMinimum
WindowPointShift = lsm.WindowPointShift
WindowTotal = lsm.WindowTotal
WindowVariance = lsm.WindowVariance
XCoord = lsm.XCoord
XIndex = lsm.XIndex
YCoord = lsm.YCoord
YIndex = lsm.YIndex
__doc__ = lsm.__doc__
abs = lsm.abs
acos = lsm.acos
acosh = lsm.acosh
asin = lsm.asin
asinh = lsm.asinh
atan = lsm.atan
atan2 = lsm.atan2
atanh = lsm.atanh
cbrt = lsm.cbrt
ceil = lsm.ceil
cos = lsm.cos
cosh = lsm.cosh
cross = lsm.cross
distance = lsm.distance
dot = lsm.dot
exp = lsm.exp
fdim = lsm.fdim
floor = lsm.floor
fraction = lsm.fraction
hypot = lsm.hypot
length = lsm.length
log = lsm.log
log10 = lsm.log10
logn = lsm.logn
mat4x4 = lsm.mat4x4
max = lsm.max
min = lsm.min
normalize = lsm.normalize
pow = lsm.pow
rand = lsm.rand
reflect = lsm.reflect
refract = lsm.refract
round = lsm.round
sign = lsm.sign
sin = lsm.sin
sinh = lsm.sinh
smoothstep = lsm.smoothstep
sqrt = lsm.sqrt
tan = lsm.tan
tanh = lsm.tanh
vec2 = lsm.vec2
vec3 = lsm.vec3
vec4 = lsm.vec4



def PlotMapSimple(m, *args, **kwargs):
    datai = lisem.lisem_python.ToNumpy(m)
    height = datai.shape[0]
    width = datai.shape[1]
    has_color = False
    is_simple = False
    fig = pp.figure()
    for key, value in kwargs.items():
        if key == 'cmap':
            has_color = True
        if key == 'is_simple':
            is_simple = value
    if has_color:
        i = pp.imshow(datai, *args, **kwargs)
        fig.colorbar(i)
    else :
        i = pp.imshow(datai, cmap = 'gray', *args, **kwargs)
        fig.colorbar(i)
    if is_simple :
        pp.axis('off')
    #custom range without altering data view (use CRS coordinates from actual map)
    locsx, labelsx = pp.xticks()
    locsy, labelsy = pp.yticks()
    labelsx = [m.West() + float(item)*m.CellSizeX() for item in locsx]
    labelsy = [m.North() + float(item)*m.CellSizeY() for item in locsy]
    pp.xticks(locsx, labelsx)
    pp.yticks(locsy, labelsy)
    pp.xlim([0, width])
    pp.ylim([0, height])
    fig.autofmt_xdate()


