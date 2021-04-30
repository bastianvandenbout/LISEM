    #ifndef PHOTOGRAMMETRYMESH_H
#define PHOTOGRAMMETRYMESH_H

#include "MVS.h"
#include "QString"
#include "omp.h"
#include <boost/program_options.hpp>
#include "error.h"
#include <string>
#include "lsmio.h"
#include "defines.h"

LISEM_API extern bool optdense_done_init;

inline static void AS_PhotogrammetryDensifyPointCloud(QString outputdir)
{
    outputdir = outputdir + "/MVS";
    std::string strInputFileName = (AS_DIR + outputdir + "/scene.mvs").toStdString();

    if(!optdense_done_init)
    {
        optdense_done_init = true;

        MVS::OPTDENSE::init();
    }

    MVS::OPTDENSE::nResolutionLevel = 1;
    MVS::OPTDENSE::nMaxResolution = 3200;
    MVS::OPTDENSE::nMinResolution = 640;
    MVS::OPTDENSE::nNumViews = 5;
    MVS::OPTDENSE::nMinViewsFuse = 3;
    MVS::OPTDENSE::nEstimateColors = 2;
    MVS::OPTDENSE::nEstimateNormals = 2;


    MVS::Scene scene(omp_get_num_threads());

    // load and estimate a dense point-cloud
    if (!scene.Load(MAKE_PATH_SAFE(strInputFileName.c_str())))
    {

        LISEMS_ERROR("Could not load scene");
        throw 1;
    }
    if (scene.pointcloud.IsEmpty()) {

        LISEMS_ERROR("Empty scene");
        throw 1;
    }

    std::cout << "npoints before: " << scene.pointcloud.GetSize() << std::endl;
    std::cout << scene.images.size() << std::endl;

    if ((ARCHIVE_TYPE)(0) != ARCHIVE_MVS) {
            if (scene.DenseReconstruction(0)) {

            }else
            {
                LISEMS_ERROR("Could not densify point cloud");
                throw 1;
            }
    }

    std::cout << "npoints after: " << scene.pointcloud.GetSize() << std::endl;


    if (scene.pointcloud.IsEmpty()) {

        LISEMS_ERROR("Empty dense reconstruction");
        throw 1;
    }

    scene.Save((AS_DIR + outputdir + "/scene_dense.mvs").toStdString(), (ARCHIVE_TYPE)(0));
    scene.pointcloud.Save((AS_DIR + outputdir + "/dense.ply").toStdString());

}


inline static void AS_PhotogrammetryToMesh(QString outputdir)
{
    outputdir = outputdir + "/MVS";
    std::string strInputFileName = (AS_DIR + outputdir + "/scene_dense.mvs").toStdString();
    std::string strOutputFileName = (AS_DIR + outputdir + "/scene_dense.mvs").toStdString();


    float fDecimateMesh = 1.0;
    float fRemoveSpurious = 20.0;
    bool bRemoveSpikes = true;
    unsigned int nCloseHoles = 30;
    unsigned int nSmoothMesh = 2;
    bool bUseFreeSpaceSupport = false;
    float fQualityFactor = 1.0;
    float fThicknessFactor = 1.0;
    float fDistInsert = 2.5f;

    MVS::Scene scene(omp_get_num_threads());

    // load and estimate a dense point-cloud
    if (!scene.Load(MAKE_PATH_SAFE(strInputFileName.c_str())))
    {

        LISEMS_ERROR("Could not load scene");
        throw 1;
    }
    if (scene.pointcloud.IsEmpty()) {

        LISEMS_ERROR("Empty scene");
        throw 1;
    }


    bool bAbort(false);
    #pragma omp parallel for
    for (int_t idx=0; idx<(int_t)scene.images.GetSize(); ++idx) {
        #pragma omp flush (bAbort)
        if (bAbort)
            continue;
        const uint32_t idxImage((uint32_t)idx);
        MVS::Image& imageData = scene.images[idxImage];
        if (!imageData.IsValid())
            continue;
        // reset image resolution
        if (!imageData.ReloadImage(0, false)) {
            bAbort = true;
            #pragma omp flush (bAbort)
            continue;
        }
        imageData.UpdateCamera(scene.platforms);
        // select neighbor views
        if (imageData.neighbors.IsEmpty()) {
            IndexArr points;
            scene.SelectNeighborViews(idxImage, points);
        }
    }

    if (bAbort)
    {
        LISEMS_ERROR("Could not analyze views for mesh creation");
        throw 1;
    }

    if (!scene.ReconstructMesh(fDistInsert, bUseFreeSpaceSupport, 4, fThicknessFactor, fQualityFactor))
    {
        LISEMS_ERROR("Error during mesh reconstruction");
    }

    // clean the mesh
    scene.mesh.Clean(fDecimateMesh, fRemoveSpurious, bRemoveSpikes, nCloseHoles, nSmoothMesh, false);
    scene.mesh.Clean(1.f, 0.f, bRemoveSpikes, nCloseHoles, 0, false); // extra cleaning trying to close more holes
    scene.mesh.Clean(1.f, 0.f, false, 0, 0, true); // extra cleaning to remove non-manifold problems created by closing holes

    // save the final mesh
    scene.Save((AS_DIR + outputdir + "/scene_mesh.mvs").toStdString(), (ARCHIVE_TYPE)(0));
    scene.mesh.Save((AS_DIR + outputdir + "/mesh.ply").toStdString());

}


inline static void AS_PhotogrammetryRefineMesh(QString outputdir)
{
    outputdir = outputdir + "/MVS";
    std::string strInputFileName = (AS_DIR + outputdir + "/scene_mesh.mvs").toStdString();

    float nResolutionLevel = 0;
    float nMinResolution = 640;
    float nMaxViews = 8;
    float fDecimateMesh = 0.0f;
    float nCloseHoles = 30;
    float nEnsureEdgeSize = 1;
    float nMaxFaceArea = 64;
    float nScales = 3;
    float fScaleStep = 0.5;
    float fRatioRigidityElasticity = 0.9f;
    float fGradientStep = 45.05;
    int nAlternatePair = 0;
    int nReduceMemory = 1;
    float fRegularityWeight = 0.2;
    float fPlanarVertexRatio = 0.0f;

    MVS::Scene scene(omp_get_num_threads());

    // load and estimate a dense point-cloud
    if (!scene.Load(MAKE_PATH_SAFE(strInputFileName.c_str())))
    {

        LISEMS_ERROR("Could not load scene");
        throw 1;
    }

    if (!scene.RefineMesh(nResolutionLevel, nMinResolution, nMaxViews,
                              fDecimateMesh, nCloseHoles, nEnsureEdgeSize,
                              nMaxFaceArea,
                              nScales, fScaleStep,
                              nReduceMemory, nAlternatePair,
                              fRegularityWeight,
                              fRatioRigidityElasticity,
                              fPlanarVertexRatio,
                              fGradientStep))
    {
        LISEMS_ERROR("Mesh refinement error");
        throw 1;
    }

    scene.Save((AS_DIR + outputdir + "/scene_meshrefined.mvs").toStdString(), (ARCHIVE_TYPE)(0));
    scene.mesh.Save((AS_DIR + outputdir + "/meshrefined.ply").toStdString());

}

inline static void AS_PhotogrammetryTextureMesh(QString outputdir, int ortho_resolution)
{
    outputdir = outputdir + "/MVS";
    std::string strInputFileName = (AS_DIR + outputdir + "/scene_meshrefined.mvs").toStdString();

    unsigned int nCloseHoles = 30;
    float fDecimateMesh = 1.0f;
    unsigned int nResolutionLevel = 1.0;
    unsigned int nMinResolution = 640;
    float fOutlierThreshold = 6e-2f;
    float fRatioDataSmoothness = 0.1;
    bool bGlobalSeamLeveling = true;
    bool bLocalSeamLeveling = true;
    unsigned int nTextureSizeMultiple= 0;
    unsigned int nRectPackingHeuristic = 3;
    uint32_t nColEmpty =0x00FF7F27;

    MVS::Scene scene(omp_get_num_threads());

    // load and estimate a dense point-cloud
    if (!scene.Load(MAKE_PATH_SAFE(strInputFileName.c_str())))
    {

        LISEMS_ERROR("Could not load scene");
        throw 1;
    }

    if (!scene.TextureMesh(nResolutionLevel, nMinResolution, fOutlierThreshold, fRatioDataSmoothness, bGlobalSeamLeveling, bLocalSeamLeveling, nTextureSizeMultiple, nRectPackingHeuristic, Pixel8U(nColEmpty)))
    {
        LISEMS_ERROR("Error during mesh texturing");
        throw 1;
    }


    scene.Save((AS_DIR + outputdir + "/scene_meshtextured.mvs").toStdString(), (ARCHIVE_TYPE)(0));
    scene.mesh.Save((AS_DIR + outputdir + "/meshtextured.ply").toStdString());

    Image8U3 imageRGB;
    Image8U imageRGBA[4];
    Point3 center;

    scene.mesh.ProjectOrthoTopDown(2000, imageRGB, imageRGBA[3], center);
    std::cout << "projection done 1" << std::endl;
    Image8U4 image;
    std::cout << "projection done 2" << std::endl;
    cv::split(imageRGB, imageRGBA);
    std::cout << "projection done 3" << std::endl;
    cv::merge(imageRGBA, 4, image);
    std::cout << "projection done 4" << std::endl;
    image.Save((AS_DIR + outputdir + "/ortho_color.png").toStdString());
    std::cout << "projection done 5" << std::endl;
    SML sml(_T("OrthoMap"));
    std::cout << "projection done 6" << std::endl;
    sml[_T("Center")].val = String::FormatString(_T("%g %g %g"), center.x, center.y, center.z);
    std::cout << "projection done 7" << std::endl;
    sml.Save((AS_DIR + outputdir + "/ortho_color.txt").toStdString());
    std::cout << "projection done 8" << std::endl;

}

#endif // PHOTOGRAMMETRYMESH_H
