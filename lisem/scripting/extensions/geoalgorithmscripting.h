#ifndef GEOALGORITHMSCRIPTING_H
#define GEOALGORITHMSCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "geo/shapes/shapefile.h"
#include "geo/rasterize.h"
#include "geo/vectorize.h"
#include "geo/rastercontour.h"
#include "raster/rasterwarp.h"
#include "geo/simplify.h"
#include "geo/segmentize.h"
#include "raster/rastersegmentize.h"
#include "geo/vectorrastersample.h"
#include "raster/rastersupervisedclassify.h"
#include "raster/rasterlinesegmentdetect.h"
#include "pointcloud/pointcloudtosurface.h"
#include "geo/gridding.h"
#include "sfm/photogrammetry.h"
#include "sfm/photogrammetrymesh.h"
#include "raster/rasterradar.h"
#include "raster/rasterpansharpen.h"
#include "raster/rasterclassified.h"
#include "raster/rasterspectralunmixing.h"
#include "raster/rasterpca.h"
#include "raster/rastermetadata.h"
#include "raster/rastersar.h"
#include "raster/rasterdisparity.h"
#include "raster/rasternoisereduce.h"
#include "raster/rasterstereo.h"
#include "raster/rasterinterpolation.h"
#include "geo/voronoi.h"
#include "raster/rasterpainting.h"
#include "raster/rastercube.h"
#include "raster/rasterstability.h"
#include "raster/rasterterrain.h"
#include "raster/rastergroundwater.h"
#include "raster/rastersoil.h"
#include "raster/rasterseismic.h"
#include "geo/marchingcubes.h"
#include "scriptarrayhelpers.h"


inline static void RegisterGeoAlgorithmsToScriptEngine(LSMScriptEngine *engine)
{
    //this is a non-executed test piece of code to force linking to the relevant dll's
    //this allows debugging the setup of all dependencies
    if(engine == nullptr)
    {



    }


    //register conversion functions
    int r = engine->RegisterGlobalFunction("Map @Rasterize(const Map &in s, Shapes &in s,string attribute = "", bool burn = false, float burn_value = 1.0, bool invert = false, bool all_touched = false, bool add = false)", asFUNCTION( AS_Rasterize),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @Vectorize(const Map &in s,string attributename = \"value\", bool diagonal = false)", asFUNCTION( AS_Vectorize),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @VectorizePoints(const Map &in s,string attributename = \"id\")", asFUNCTION( AS_VectorizePoints),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @RasterContour(const Map &in s,float interval)", asFUNCTIONPR( AS_RasterContour,(cTMap *,float),ShapeFile*),  asCALL_CDECL);


    r = engine->RegisterGlobalSTDFunction("Map @RasterWarp(const Map &in target,array<Map> &in sources, string interpolation = \"linear\")", GetFuncConvert(static_cast<cTMap* (*)(cTMap*,std::vector<cTMap*>,QString)>(&AS_RasterWarp)));
    r = engine->RegisterGlobalFunction("Map @RasterWarp(const Map &in target,const Map &in sources, string interpolation = \"linear\")", asFUNCTIONPR( AS_RasterWarp,(cTMap *,cTMap*,QString),cTMap *),  asCALL_CDECL);


    r = engine->RegisterGlobalFunction("Shapes @VectorWarp(Shapes &in source,GeoProjection &in CRS)", asFUNCTIONPR( AS_VectorWarp,(ShapeFile*,GeoProjection *),ShapeFile *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @Segmentize(Shapes &in source,float distancemax)", asFUNCTIONPR( AS_Segmentize,(ShapeFile *,float),ShapeFile *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @Simplify(Shapes &in source,float distancethreshold)", asFUNCTIONPR( AS_Simplify,(ShapeFile *,float),ShapeFile *),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Shapes @Voronoi(Shapes &in source,bool centroids = true)", asFUNCTIONPR( AS_Vonoroi,(ShapeFile *,bool),ShapeFile *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @VectorToPoints(Shapes &in source,bool centroids = true, float extentscale = 1.0)", asFUNCTIONPR( AS_ShapeToPoints,(ShapeFile *,bool,float),ShapeFile *),  asCALL_CDECL);


    //otb based segmentation
    r = engine->RegisterGlobalSTDFunction("Map @SegmentizeMS(const Map &in source,float spatial_radius, float range_radius, float min_region_size, float max_iter, float in_thres)", asFUNCTIONPRCONV( AS_RasterSegmentize,(cTMap *,float,float,float,float,float),cTMap *));
    r = engine->RegisterGlobalSTDFunction("Map @SegmentizeMS(array<Map> &in multi_band_source,float spatial_radius, float range_radius, float min_region_size, float max_iter, float in_thres)", asFUNCTIONPRCONV( AS_RasterSegmentize,(std::vector<cTMap*>,float,float,float,float,float),cTMap *));
    r = engine->RegisterGlobalFunction("Map @SegmentizeWS(const Map &in source,float threshold = 0.01, float level = 0.1)", asFUNCTIONPR( AS_RasterSegmentizeWS,(cTMap *,float,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("Map @SlopeMagnitude(array<Map> &in source)", GetFuncConvert( AS_RasterGradientMagnitude));

    r = engine->RegisterGlobalFunction("Map @AnisotropicFilter(const Map &in source, int iter = 100, float conductance = 0.2, float timestep = 0.125)", asFUNCTIONPR( AS_RasterAnisotropicDiffusion,(cTMap *,int,float,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<Map> @AnisotropicFilter(array<Map> &in source, int iter = 100, float conductance = 0.2, float timestep = 0.125)",  asFUNCTIONPRCONV( AS_RasterAnisotropicDiffusion,(std::vector<cTMap*>,int,float,float),std::vector<cTMap*>),  asCALL_CDECL);


    //otb based classification

    r = engine->RegisterGlobalSTDFunction("Map @SupervisedClassify(array<Map> &in source, const Map &in train)", GetFuncConvert(  AS_SupervisedClassify));
    r = engine->RegisterGlobalSTDFunction("Map @UnSupervisedClassify(array<Map> &in source, int classes, const Map &in mask)", GetFuncConvert(  AS_UnSupervisedClassify));

    //other geo-algorithms

    r = engine->RegisterGlobalSTDFunction("array<Map> @ClassToRGB(const Map &in clas)", GetFuncConvert( AS_ClassToRGB));
    r = engine->RegisterGlobalFunction("Map @ClassRegularize(const Map &in clas, float radius)", asFUNCTIONPR( AS_RasterClassRegularize,(cTMap *,float),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @LineSegmentDetect(const Map &in value)", asFUNCTIONPR( AS_LineSegmentDetect,(cTMap *),ShapeFile*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @CircleDetect(const Map &in value, float radius_min = 0.0, float radius_max = 1e30, float threshold = 100,int subdivide = 20)", asFUNCTIONPR( AS_RasterCircleDetect,(cTMap *,float,float,float,int),ShapeFile*),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @Kriging(const Map &in data, float distance_max)", asFUNCTIONPR( AS_Kriging,(cTMap *,float),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @KrigingGaussian(const Map &in data, float distance_max, double c0,double c0)", asFUNCTIONPR( AS_KrigingGaussianCovariance,(cTMap *,float,double,double),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("MathExpression SemiVariogram(const Map &in data, float distance_max, int maxpoints = 0)", asFUNCTIONPR( AS_SemiVarioGram,(cTMap *,float,int),MathFunction),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @PointCloudToDEM(PointCloud &in value, float resolution)",asFUNCTIONPR(PointCloudToDEM,(PointCloud*,float),cTMap *),asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Table @VectorRasterSample(const Map &in raster, Shapes &in shapes)", asFUNCTIONPR( AS_VectorRasterSample,(cTMap *,ShapeFile *),MatrixTable*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Shapes @DrainageNetworkToShapes(const Map &in ldd)", asFUNCTIONPR( AS_DrainageNetworkToShapes,(cTMap *),ShapeFile*),  asCALL_CDECL);

    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterProject(array<Map> &in source,GeoProjection &in CRS,string interpolation = \"bilinear\")", asFUNCTIONPRCONV( AS_RasterProject,(std::vector<cTMap*>,GeoProjection*,QString),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterProject(const Map &in source,GeoProjection &in CRS,string interpolation = \"bilinear\")", asFUNCTIONPR( AS_RasterProject,(cTMap*,GeoProjection*,QString),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterOrthoRectify(array<Map> &in source, const Map &in DEM)", asFUNCTIONPRCONV( AS_RasterOrthoRectify,(std::vector<cTMap*>,cTMap*),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterRegister(array<Map> &in source, array<vec2> &in pixpos, array<vec2> &in worldpos)", asFUNCTIONPRCONV( AS_RasterRegister,(std::vector<cTMap*>,std::vector<LSMVector2>,std::vector<LSMVector2>),std::vector<cTMap*>),  asCALL_CDECL);

    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterSpectralUnmix(array<Map> &in source, array<array<float>> &in spectra, string method = \"UCLS\")", asFUNCTIONPRCONV( AS_SpectralUnmix,(std::vector<cTMap *>, std::vector<std::vector<double>>, QString ),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("int RasterEstimateEndmemberCount(array<Map> &in source, string method = \"HFC-VD\")", asFUNCTIONPRCONV( AS_SpectralEndmemberEstimate,(std::vector<cTMap*>,QString),int),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<array<float>> @RasterVCA(array<Map> &in source, int count)", asFUNCTIONPRCONV( AS_SpectralVCA,(std::vector<cTMap*>,int),std::vector<std::vector<float>>),  asCALL_CDECL);


    //Photogrammetry code

    r = engine->RegisterGlobalSTDFunction("void PhotogrammetryListPrepare(array<string> &in files,string listfile =\"photogrammetry_list.txt\",double focal_pixels = -1.0,bool use_pos = false)", asFUNCTIONPRCONV( AS_Photogrammetry_PrepareList,(std::vector<QString>,QString,double,bool),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryFeatureCompute(string listfile =\"photogrammetry_list.txt\", string outputdir = \"temp_phtgrm\",string method = \"SIFT\", float threshold = 1.0, string order = \"HIGH\")", asFUNCTIONPR( AS_Photogrammetry_FeatureCompute,(QString,QString,QString,float,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryFeatureMatch(string listfile, string outputdir = \"temp_phtgrm\", string geometrymodel = \"f\")", asFUNCTIONPR( AS_Photogrammetry_FeatureMatch,(QString,QString,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryStructureCompute(string listfile, string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_Photogrammetry_IncrementalSFM,(QString,QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("void PhotogrammetryRegister(array<vec3> &in gcp, array<string> &in image1, array<vec2> &in image1_pos, array<string> &in image2, array<vec2> &in image2_pos, string listfile =\"photogrammetry_list.txt\",string outputdir =\"temp_phtgrm\")",asFUNCTIONPRCONV( AS_Photogrammetry_Register,(std::vector<LSMVector3>,std::vector<QString>,std::vector<LSMVector2>,std::vector<QString>,std::vector<LSMVector2>,QString, QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryDensify(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryDensifyPointCloud,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMesh(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryToMesh,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMeshRefine(string outputdir =\"temp_phtgrm\")",asFUNCTIONPR( AS_PhotogrammetryRefineMesh,(QString),void),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("void PhotogrammetryMeshTexture(string outputdir =\"temp_phtgrm\", int ortho_resolution= 2000)",asFUNCTIONPR( AS_PhotogrammetryTextureMesh,(QString, int),void),  asCALL_CDECL);

    //noise filters
    r = engine->RegisterGlobalFunction("Map @NoiseFilterFrost(const Map &in m, float radius, float deramp = 0.1)",asFUNCTIONPR( AS_NoiseFilterFrost,(cTMap * ,float,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @NoiseFilterLee(const Map &in m, float radius, float nlooks = 1.0)",asFUNCTIONPR( AS_NoiseFilterLee,(cTMap * ,float,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @NoiseFilterGamma(const Map &in m, float radius, float nlooks = 1.0)",asFUNCTIONPR( AS_NoiseFilterGamma,(cTMap * ,float,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @NoiseFilterKuan(const Map &in m, float radius, float nlooks = 1.0)",asFUNCTIONPR( AS_NoiseFilterKuan,(cTMap * ,float,float),cTMap *),  asCALL_CDECL);

    //PanSharpen
    r = engine->RegisterGlobalSTDFunction("array<Map> @PanSharpenLMVM(const Map &in pan, array<Map> &in bands)",asFUNCTIONPRCONV( AS_PanSharpenLMVM,(cTMap * ,std::vector<cTMap*>),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<Map> @PanSharpenBayesian(const Map &in pan, array<Map> &in bands, float Lambda = 0.9999, float S = 1.0)",asFUNCTIONPRCONV( AS_PanSharpenBayesian,(cTMap * ,std::vector<cTMap*>,float,float),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<Map> @PanSharpenRCS(const Map &in pan, array<Map> &in bands)",asFUNCTIONPRCONV( AS_PanSharpenRCS,(cTMap * ,std::vector<cTMap*>),std::vector<cTMap*>),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @RegionSimilarityMerge(const Map &in c, array<Map> &in similarity,float min_area)",asFUNCTIONPR( AS_ClassMerge,(cTMap * ,std::vector<cTMap*>,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RegionMerge(const Map &in c, float min_area)",asFUNCTIONPR( AS_ClassMerge,(cTMap * ,float),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @NLMeans(const Map &in m, float window = 7.0, float cutoff = 1.0)",asFUNCTIONPR( AS_FastNLMeansFilter,(cTMap * ,float,float),cTMap*),  asCALL_CDECL);

    //disparity
    r = engine->RegisterGlobalSTDFunction("array<Map> @OpticalFlow(const Map &in first, const Map &in second, float windowsize = 15.0)",asFUNCTIONPRCONV( AS_OpticalFlow,(cTMap * ,cTMap *,float),std::vector<cTMap*>),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @Disparity(const Map &in first, const Map &in second, float windowsize = 5, float max_disp = 100)",asFUNCTIONPR( AS_RasterDisparity,(cTMap * ,cTMap *,float,float),cTMap*),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @DisparityFine(const Map &in first, const Map &in second, string method = \"SSDivMean\",float mindisp=-100, float maxdisp= 100)",asFUNCTIONPR( AS_RasterDisparityDetailed,(cTMap * ,cTMap *,QString,float,float),cTMap*),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @ResampleLinear(const Map &in vals, float scale)",asFUNCTIONPR( AS_ReSampleLinear,(cTMap *, float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @ResampleCubic(const Map &in vals, float scale)",asFUNCTIONPR( AS_ReSampleCubic,(cTMap *, float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @ResampleNearest(const Map &in vals, float scale)",asFUNCTIONPR( AS_ReSampleNearest,(cTMap *, float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @ResampleSparse(const Map &in vals, float scale)",asFUNCTIONPR( AS_ReSampleSparse,(cTMap *, float),cTMap *),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @ValueCorrect(const Map &in ref, const Map &in vals, int distance)",asFUNCTIONPR( AS_ValueCorrectLinear,(cTMap *, cTMap *, int),cTMap *),  asCALL_CDECL);



    //metadata
    //r = engine->RegisterGlobalFunction("array<string> @MapMetaData(string file, int band = 0)",asFUNCTIONPR( AS_GetRasterMetaData,(QString, int),std::vector<cTMap*>),  asCALL_CDECL);
    //r = engine->RegisterGlobalFunction("array<string> @MapMetaDataAbsPath(string file, int band = 0)",asFUNCTIONPR( AS_GetRasterMetaDataAbsPath,(QString, int),std::vector<cTMap*>),  asCALL_CDECL);

    //sar
    r = engine->RegisterGlobalSTDFunction("Map &SARCalibrate(array<Map> &in radar, bool lut = true, bool noise = false)",asFUNCTIONPRCONV( AS_SARCalibrate,(std::vector<cTMap*>,bool,bool),cTMap *),  asCALL_CDECL);


    //
    r = engine->RegisterGlobalFunction("Map @KuwaharaFilter(const Map &in vals, const Map &in windowsize)",asFUNCTIONPR( AS_KuwaharaFilter,(cTMap *,cTMap*),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @KuwaharaFilter(const Map &in vals, float windowsize)",asFUNCTIONPR( AS_KuwaharaFilter,(cTMap *, float),cTMap *),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @InpaintRFSR(const Map &in vals, int iterations = 0, int fftsize = 0, int intrinsicsamplingsize = 0, int correct_dist = 0, double rho = 1.0, int blocksize = 16, double blendscale = 1.0)",asFUNCTIONPR( AS_ResampleRFSR,(cTMap *, int,int, int,int, double,int,double),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @InpaintNS(const Map &in vals,float radius = 10.0)",asFUNCTIONPR( AS_ResampleNS,(cTMap *,float),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @InpaintTELEA(const Map &in vals,float radius = 10.0)",asFUNCTIONPR( AS_ResampleTELEA,(cTMap *,float),cTMap *),  asCALL_CDECL);


    r = engine->RegisterGlobalFunction("Map @SetCellSize(const Map &in val, double dx, double dy=0.0)",asFUNCTIONPR( AS_SetRasterCellSize,(cTMap *,double, double),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("vec2 CellSize(const Map &in val)",asFUNCTIONPR( AS_GetRasterCellSize,(cTMap *),LSMVector2),  asCALL_CDECL);

    r = engine->RegisterGlobalFunction("Map @SetULC(const Map &in val, double x, double y)",asFUNCTIONPR( AS_SetRasterULC,(cTMap *,double, double),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("vec2 ULC(const Map &in val)",asFUNCTIONPR( AS_GetRasterULC,(cTMap *),LSMVector2),  asCALL_CDECL);


    r = engine->RegisterGlobalFunction("Region ClassExtent(const Map &in classes, int classval)",asFUNCTION(AS_GetClassExtent), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Region ClassCellExtent(const Map &in classes, int classval)",asFUNCTION(AS_GetClassPixelExtent), asCALL_CDECL);


    r = engine->RegisterGlobalFunction("Map @RasterPaintShapes(const Map &in target, Shapes &in s, double val1, double val2, int paintop)",asFUNCTION(AS_PaintShapeFile), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterPaintShape(const Map &in target, Shape &in s, double val1, double val2, int paintop)",asFUNCTION(AS_PaintShape), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterPaintPolygon(const Map &in target, array<vec2> &in points, double val1, double val2, int paintop)",asFUNCTION(AS_PaintPolygon), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterPaintLines(const Map &in target, array<vec2> &in points, double val1, double val2, double size, double feather, int paintop)",asFUNCTION(AS_PaintLines), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterPaintEllips(const Map &in target, double x, double y, double rx, double ry, double val1, double val2, double feather, int paintop)",asFUNCTION(AS_PaintEllipsoid), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @RasterPaintRectangle(const Map &in target, double x, double y, double rx, double ry, double val1, double val2, double feather, int paintop)",asFUNCTION(AS_PaintRectangle), asCALL_CDECL);


    r = engine->RegisterGlobalFunction("Map @XSlice(array<Map> &in val, int row, double zmin, double zmax, int levelinterval = 1, int leveloffset = 0)",asFUNCTIONPR( AS_GetXSlice,(std::vector<cTMap*>, int , double , double,int,int),cTMap *),  asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @YSlice(array<Map> &in val, int row, double zmin, double zmax, int levelinterval = 1, int leveloffset = 0)",asFUNCTIONPR( AS_GetYSlice,(std::vector<cTMap*>, int , double , double,int,int),cTMap *),  asCALL_CDECL);

    engine->RegisterGlobalFunction("array<Map> @SlopeStabilityIS(const Map & in elevation, const Map &in SoilDepth, const Map &in Cohesion, const Map&in InternalFrictionAngle, const Map &in Density, const Map&in WaterHeight, float sfmax)",asFUNCTIONPR( AS_SlopeStabilityIS,(cTMap*,cTMap*,cTMap*,cTMap*,cTMap*,cTMap*,float),std::vector<cTMap*>),  asCALL_CDECL);
    engine->RegisterGlobalFunction("array<Map> @SlopeStabilityRES(const Map & in elevation, const array<Map> &in SoilDepth, const array<Map> &in Cohesion, const array<Map> &in InternalFrictionAngle, const array<Map> &in Density, const array<Map> &in Saturation, const Map&in WaterHeight, float sample_density, float h_min, float h_max, float size_min, float size_max, float size_lat_min, float size_lat_max, float size_vert_min, float size_vert_max, float rot_min, float rot_max, float rot_lat_min, float rot_lat_max, float vol_min)",asFUNCTION( AS_SlopeStabilityRES),  asCALL_CDECL);

    engine->RegisterGlobalFunction("Map @TerrainErode(const Map &in DEM, float p_erode, float p_deposit, float p_gravity, float p_tc, float p_evapo, float p_intertia, float p_minslope, float radius)", asFUNCTION(AS_LandscapeErode),asCALL_CDECL);
    //vector stuff
    engine->RegisterGlobalFunction("GeoProjection &GetCRS(const Shapes &in s)",asFUNCTIONPR( AS_GetCRS,(ShapeFile*),GeoProjection*),  asCALL_CDECL);
    engine->RegisterGlobalFunction("Shapes @SetCRS(const Shapes &in s,GeoProjection &in p)",asFUNCTIONPR( AS_SetCRS,(ShapeFile*, GeoProjection*),ShapeFile *),  asCALL_CDECL);
    engine->RegisterGlobalFunction("Region GetRegion(const Shapes &in s)", asFUNCTIONPR( AS_GetRegion,(ShapeFile*),BoundingBox),  asCALL_CDECL); assert( r >= 0 );


    //soil stuff
    engine->RegisterGlobalFunction("Map @SaxtonKSat(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel, float densityfactor =1.0)", asFUNCTION( AS_SaxtonKSat),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @SaxtonPorosity(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel, float densityfactor =1.0)", asFUNCTION( AS_SaxtonPorosity),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @SaxtonFieldCapacity(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel, float densityfactor =1.0)", asFUNCTION( AS_SaxtonFieldCapacity),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @SaxtonSuction(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel,  const Map&in Saturation,float densityfactor =1.0)", asFUNCTION( AS_SaxtonSuction),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @SaxtonA(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel, float densityfactor =1.0)", asFUNCTION( AS_SaxtonA),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @SaxtonB(const Map&in Sand, const Map&in Clay, const Map&in Organic, const Map&in Gravel, float densityfactor =1.0)", asFUNCTION( AS_SaxtonB),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @MedianGrainSize(const Map&in Sand, const Map&in Clay)", asFUNCTION(AS_MedianGrainSize),asCALL_CDECL);
    engine->RegisterGlobalFunction("Map @TextureClass(const Map&in Sand, const Map&in Clay)", asFUNCTION(AS_TextureClass),asCALL_CDECL);
    //wave
    engine->RegisterGlobalFunction("Map @WaveEquation(const Map&in U, const Map&in c, float dt)", asFUNCTION( AS_WaveEquation),  asCALL_CDECL); assert( r >= 0 );


    //mesh stuff

    engine->RegisterGlobalSTDFunction("Object @MarchingCubes(const Field &in f, float value)", GetFuncConvert(MarchingCubesF));

}


#endif // GEOALGORITHMSCRIPTING_H
