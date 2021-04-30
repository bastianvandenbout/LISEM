#include "photogrammetry_tovms.h"


#include "openMVG/cameras/Camera_Pinhole.hpp"
#include "openMVG/cameras/Camera_undistort_image.hpp"
#include "openMVG/image/image_io.hpp"
#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"

#define _USE_EIGEN
#include "MVS/Interface.h"

#include "third_party/cmdLine/cmdLine.h"
#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"
#include "third_party/progress/progress_display.hpp"

using namespace openMVG;
using namespace openMVG::cameras;
using namespace openMVG::geometry;
using namespace openMVG::image;
using namespace openMVG::sfm;

#include <atomic>
#include <cstdlib>
#include <string>

#include <omp.h>



bool exportToOpenMVS(
  const SfM_Data & sfm_data,
  const std::string & sOutFile,
  const std::string & sOutFiletemp,
        const std::string & sOutDir,
  const int iNumThreads
  )
{
    std::cout << "openMVStoMVG1 " << sOutFile << " " << sOutDir << std::endl;
  // Create undistorted images directory structure
  if (!stlplus::is_folder(sOutDir))
  {
    stlplus::folder_create(sOutDir);
    if (!stlplus::is_folder(sOutDir))
    {
      std::cerr << "Cannot access to one of the desired output directory" << std::endl;
      return false;
    }
  }


  // Export data :
  MVS::Interface scene;
  size_t nPoses(0);
  const uint32_t nViews((uint32_t)sfm_data.GetViews().size());

  C_Progress_display my_progress_bar(nViews,
      std::cout, "\n- PROCESS VIEWS -\n");


  // OpenMVG can have not contiguous index, use a map to create the required OpenMVS contiguous ID index
  std::map<openMVG::IndexT, uint32_t> map_intrinsic, map_view;

  // define a platform with all the intrinsic group
  for (const auto& intrinsic: sfm_data.GetIntrinsics())
  {
    if (isPinhole(intrinsic.second->getType()))
    {
      Pinhole_Intrinsic * cam = dynamic_cast<Pinhole_Intrinsic*>(intrinsic.second.get());
      if (map_intrinsic.count(intrinsic.first) == 0)
        map_intrinsic.insert(std::make_pair(intrinsic.first, scene.platforms.size()));

      MVS::Interface::Platform platform;
      // add the camera
      MVS::Interface::Platform::Camera camera;
      camera.width = cam->w();
      camera.height = cam->h();

      for(int r = 0; r < 3; r++)
      {
          for(int c = 0; c < 3; c++)
          {
              camera.K.operator()(r,c) = cam->K()(r,c);
              camera.R.operator()(r,c) = (r==c)?1.0:0.0;
          }
      }

      camera.C = {0.0,0.0,0.0};

      // sub-pose


      platform.cameras.push_back(camera);
      scene.platforms.push_back(platform);

      std::cout << "intrinsic added " << intrinsic.first << std::endl;
    }
  }

  std::cout << "openMVStoMVG3" << std::endl;

  // define images & poses
  scene.images.reserve(nViews);
  int n = 0;
  for (const auto& view : sfm_data.GetViews())
  {
    ++my_progress_bar;

    const std::string srcImage = stlplus::create_filespec(sfm_data.s_root_path, view.second->s_Img_path);
    if (!stlplus::is_file(srcImage))
    {

        LISEM_ERROR("Could not create an image file: " + QString(srcImage.c_str()));
        throw 1;
    }

    std::cout << "view " << n++ << " " << view.second->id_view <<  "  " << view.second->id_pose << " "<< view.second->s_Img_path << " "<< sfm_data.poses.size() << " "<< sfm_data.intrinsics.size() << std::endl;



    std::cout << "1: " << view.second->id_intrinsic << std::endl;
    std::cout << "2: " << view.second->id_pose << std::endl;
    std::cout << "3: " << UndefinedIndexT << std::endl;
    std::cout << "4: " << (sfm_data.poses.find(view.second->id_pose) != sfm_data.poses.end()) << std::endl;
    std::cout << "5: " << (sfm_data.intrinsics.find(view.second->id_intrinsic) != sfm_data.intrinsics.end()) << std::endl;






      if (sfm_data.IsPoseAndIntrinsicDefined(view.second.get()))
      {
          map_view[view.first] = scene.images.size();

          MVS::Interface::Image image;
          image.name = stlplus::create_filespec(sOutDir, view.second->s_Img_path);
          image.platformID = map_intrinsic.at(view.second->id_intrinsic);
          MVS::Interface::Platform& platform = scene.platforms[image.platformID];
          image.cameraID = 0;

          MVS::Interface::Platform::Pose pose;
          image.poseID = platform.poses.size();

          const openMVG::geometry::Pose3 poseMVG(sfm_data.GetPoseOrDie(view.second.get()));

          for(int r = 0; r < 3; r++)
          {
              for(int c = 0; c < 3; c++)
              {
                  pose.R.operator()(r,c) = poseMVG.rotation()(r,c);
              }
          }

          pose.C = {poseMVG.center().x(),poseMVG.center().y(),poseMVG.center().z()};

          platform.poses.push_back(pose);
          ++nPoses;

          scene.images.emplace_back(image);
      }else
      {
          //image.poseID = NO_ID;

          //scene.images.emplace_back(image);
      }
    }



  std::cout << "openMVStoMVG4" << std::endl;

  // Export undistorted images
  std::atomic<bool> bOk(true); // Use a boolean to track the status of the loop process

  const unsigned int nb_max_thread = (iNumThreads > 0)? iNumThreads : omp_get_max_threads();

  #pragma omp parallel for schedule(dynamic) num_threads(nb_max_thread)
  for (int i = 0; i < static_cast<int>(sfm_data.views.size()); ++i)
  {

    if (!bOk)
      continue;

    Views::const_iterator iterViews = sfm_data.views.begin();
    std::advance(iterViews, i);
    const View * view = iterViews->second.get();

    // Get image paths
    const std::string srcImage = stlplus::create_filespec(sfm_data.s_root_path, view->s_Img_path);
    const std::string imageName = stlplus::create_filespec(sOutDir, view->s_Img_path);

    if (sfm_data.IsPoseAndIntrinsicDefined(view))
    {
      // export undistorted images
      const openMVG::cameras::IntrinsicBase * cam = sfm_data.GetIntrinsics().at(view->id_intrinsic).get();
      if (cam->have_disto())
      {
        // undistort image and save it
        Image<openMVG::image::RGBColor> imageRGB, imageRGB_ud;
        Image<uint8_t> image_gray, image_gray_ud;
        try
        {
          if (ReadImage(srcImage.c_str(), &imageRGB))
          {
            UndistortImage(imageRGB, cam, imageRGB_ud, BLACK);
            bOk = WriteImage(imageName.c_str(), imageRGB_ud);
          }
          else // If RGBColor reading fails, try to read as gray image
          /*if (ReadImageGS(srcImage.c_str(), &image_gray))
          {
            UndistortImage(image_gray, cam, image_gray_ud, BLACK);
            const bool bRes = WriteImage(imageName.c_str(), image_gray_ud);
            bOk = bOk & bRes;
          }
          else*/
          {
            bOk = false;
          }
        }
        catch (const std::bad_alloc& e)
        {
          bOk = false;
        }
      }
      else
      {
        // just copy image
        stlplus::file_copy(srcImage, imageName);
      }
    }
    else
    {
      // just copy the image
      stlplus::file_copy(srcImage, imageName);
    }
  }

  if (!bOk)
  {
    LISEMS_ERROR("Memory error durin conversions");
    throw 1;
  }

  std::cout << "openMVStoMVG5 " << sfm_data.GetLandmarks().size() << std::endl;

  // define structure
  scene.vertices.reserve(sfm_data.GetLandmarks().size());
  for (const auto& vertex: sfm_data.GetLandmarks())
  {
    const Landmark & landmark = vertex.second;
    MVS::Interface::Vertex vert;
    MVS::Interface::Vertex::ViewArr& views = vert.views;
    for (const auto& observation: landmark.obs)
    {
      const auto it(map_view.find(observation.first));
      if (it != map_view.end()) {
        MVS::Interface::Vertex::View view;
        view.imageID = it->second;
        view.confidence = 0;
        views.push_back(view);
      }
    }
    if (views.size() < 2)
      continue;
    std::sort(
      views.begin(), views.end(),
      [] (const MVS::Interface::Vertex::View& view0, const MVS::Interface::Vertex::View& view1)
      {
        return view0.imageID < view1.imageID;
      }
    );
    vert.X = {(float) landmark.X.x(),(float)landmark.X.y(),(float)landmark.X.z()};
    scene.vertices.push_back(vert);
  }

  std::cout << "openMVStoMVG6" << std::endl;

  // write OpenMVS data

  if (!MVS::ARCHIVE::SerializeSave(scene, sOutFile))
  {
        LISEMS_ERROR("Could not save scene");
        throw 1;
  }

  std::cout << "openMVStoMVG7" << std::endl;

  return true;
}

