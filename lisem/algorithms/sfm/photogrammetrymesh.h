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
#include "QApplication"
#include "QProcess"


#define DENSE_USE_OPENMP

LISEM_API extern bool optdense_done_init;

static inline void logMVS(std::string message)
{
    std::cout << "message MVS: " << message << std::endl;

}

inline static void AS_PhotogrammetryDensifyPointCloud(QString outputdir)
{

    //old approach, for some reason i can not get the MVS patch-match algorithm to work
    //somewhere in the random iterative part something goes wrong whereby seemingly random depths in the view spectrum get higher accuracies and
    //are therefore accepted.. sort of, since in the final filtering nothing is left

    /*GET_LOG().RegisterListener(logMVS);
*/




    //now instead, we just call the pre-compiled executable through QProcess


   QString Dir = QCoreApplication::applicationDirPath();

   Dir = Dir + "/mvs/";

   QProcess p;
   QStringList params;
   QString program = QString(Dir + "DensifyPointCloud.exe");
   program = program.replace("/","\\");
   std::cout << "program " << program.toStdString()<<std::endl;
   p.setProgram(program);
   p.setWorkingDirectory(QString(AS_DIR + outputdir + "/MVS/").replace("/","\\"));
   std::cout << "run program " << Dir.toStdString() << std::endl;
   std::cout << "wd " << QString(AS_DIR).toStdString() << std::endl;
   std::string arg  = QString( "-i " + AS_DIR + outputdir + "/MVS/scene.mvs").toStdString();
   std::cout << "arg " << arg << std::endl;

   QObject::connect(&p, &QProcess::errorOccurred, [=](QProcess::ProcessError error)
   {
       std::cout << "error enum val = " << error << endl;
   });

   params << QString(arg.c_str()) << "-o scene_dense" << " -v 4";
   p.setArguments(params);

   QEventLoop loop;
   QObject::connect(&p, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), [&loop](int e, QProcess::ExitStatus s)
   {
       std::cout << "Program done " << e << " " << ((s == QProcess::NormalExit) ? 1.0:0.0) <<std::endl;
       loop.quit();
   });


   p.start(program + QString( " -i " + AS_DIR + outputdir + "/MVS/scene.mvs") + " -o scene_dense -v 4");

   if(!p.waitForStarted(3000))
   {
       LISEMS_ERROR("Could not start cloud densification");
       throw 1;
   }


   p.waitForReadyRead();
   std::cout << "process output "<< std::endl;
   std::cout << p.readAllStandardOutput().toStdString() << std::endl;

   loop.exec();



   p.waitForReadyRead();
   std::cout << "process output "<< std::endl;
   std::cout << p.readAllStandardOutput().toStdString() << std::endl;

   p.waitForFinished(-1);
   QProcess::ProcessError e = p.error();
   switch(e) {
   case QProcess::FailedToStart:
       LISEMS_ERROR("Could not start cloud densification");
       throw 1;
   case QProcess::Crashed:
       LISEMS_ERROR("Crash during cloud densification");
       throw 1;
   case QProcess::WriteError:
       LISEMS_ERROR("Could communicate with cloud densification");
       throw 1;
   case QProcess::ReadError:
       LISEMS_ERROR("Could not read cloud densification");
       throw 1;
   }

   std::cout << "process output "<< std::endl;
   std::cout << p.readAllStandardOutput().toStdString() << std::endl;
   std::cout << "error " << std::endl;
   std::cout << p.readAllStandardError().toStdString() << std::endl;

   std::cout << "done " << std::endl;

   //remove the generated archive

   QFile file (QString(AS_DIR + outputdir + "/MVS/scene_dense.mvs"));
       file.remove();

   //generate the archive using our version of MVS and boost::archive but re-using the depth maps (vast majority of the computation




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


       MVS::Scene scene(omp_get_num_threads(),(AS_DIR +outputdir + "/").toStdString());

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

    std::cout << "load mvs " << strInputFileName << std::endl;    // load and estimate a dense point-cloud
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
