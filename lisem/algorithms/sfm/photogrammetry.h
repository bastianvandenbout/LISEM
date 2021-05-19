#pragma once


#define OPENMVG_USE_OPENMP

#include "openMVG/sfm/pipelines/sequential/sequential_SfM2.hpp"
#include "openMVG/sfm/pipelines/sequential/SfmSceneInitializerMaxPair.hpp"
#include "openMVG/sfm/pipelines/sequential/SfmSceneInitializerStellar.hpp"
#include "openMVG/multiview/conditioning.hpp"
#include "openMVG/cameras/Camera_Pinhole.hpp"
#include "openMVG/cameras/Camera_undistort_image.hpp"
#include "openMVG/image/image_io.hpp"
#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include "openMVG/cameras/Camera_Common.hpp"
#include "openMVG/features/feature.hpp"
#include "openMVG/features/svg_features.hpp"
#include "openMVG/geometry/frustum.hpp"
#include "openMVG/matching/indMatch.hpp"
#include "openMVG/matching/indMatch_utils.hpp"
#include "openMVG/matching_image_collection/Pair_Builder.hpp"
#include "openMVG/sfm/pipelines/structure_from_known_poses/structure_estimator.hpp"
#include "openMVG/sfm/pipelines/sfm_regions_provider_cache.hpp"
#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_BA.hpp"
#include "openMVG/sfm/sfm_data_BA_ceres.hpp"
#include "openMVG/sfm/sfm_data_filters.hpp"
#include "openMVG/sfm/sfm_data_filters_frustum.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include "openMVG/sfm/sfm_report.hpp"
#include "openMVG/sfm/sfm_data_triangulation.hpp"
#include "openMVG/tracks/tracks.hpp"
#include "openMVG/system/timer.hpp"
#include "openMVG/types.hpp"
#include "openMVG/graph/graph.hpp"
#include "openMVG/graph/graph_stats.hpp"
#include "openMVG/features/akaze/image_describer_akaze.hpp"
#include "openMVG/features/descriptor.hpp"
#include "openMVG/features/feature.hpp"
#include "openMVG/matching/indMatch.hpp"
#include "openMVG/matching/indMatch_utils.hpp"
#include "openMVG/matching_image_collection/Matcher_Regions.hpp"
#include "openMVG/matching_image_collection/Cascade_Hashing_Matcher_Regions.hpp"
#include "openMVG/matching_image_collection/GeometricFilter.hpp"
#include "openMVG/sfm/pipelines/sfm_features_provider.hpp"
#include "openMVG/sfm/pipelines/sfm_regions_provider.hpp"
#include "openMVG/sfm/pipelines/sfm_regions_provider_cache.hpp"
#include "openMVG/matching_image_collection/F_ACRobust.hpp"
#include "openMVG/matching_image_collection/E_ACRobust.hpp"
#include "openMVG/matching_image_collection/E_ACRobust_Angular.hpp"
#include "openMVG/matching_image_collection/Eo_Robust.hpp"
#include "openMVG/matching_image_collection/H_ACRobust.hpp"
#include "openMVG/matching_image_collection/Pair_Builder.hpp"
#include "openMVG/matching/pairwiseAdjacencyDisplay.hpp"
#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include "openMVG/stl/stl.hpp"
#include "openMVG/system/timer.hpp"
#include "openMVG/cameras/cameras.hpp"
#include "openMVG/exif/exif_IO_EasyExif.hpp"
#include "openMVG/exif/sensor_width_database/ParseDatabase.hpp"
#include "openMVG/geodesy/geodesy.hpp"
#include "openMVG/image/image_io.hpp"
#include "openMVG/numeric/eigen_alias_definition.hpp"
#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include "openMVG/sfm/sfm_data_utils.hpp"
#include "openMVG/sfm/sfm_view.hpp"
#include "openMVG/sfm/sfm_view_priors.hpp"
#include "openMVG/types.hpp"
#include "openMVG/graph/graph.hpp"
#include "openMVG/graph/graph_stats.hpp"
#include "openMVG/features/akaze/image_describer_akaze.hpp"
#include "openMVG/features/descriptor.hpp"
#include "openMVG/features/feature.hpp"
#include "openMVG/matching/indMatch.hpp"
#include "openMVG/matching/indMatch_utils.hpp"
#include "openMVG/matching_image_collection/Matcher_Regions.hpp"
#include "openMVG/matching_image_collection/Cascade_Hashing_Matcher_Regions.hpp"
#include "openMVG/matching_image_collection/GeometricFilter.hpp"
#include "openMVG/sfm/pipelines/sfm_features_provider.hpp"
#include "openMVG/sfm/pipelines/sfm_regions_provider.hpp"
#include "openMVG/sfm/pipelines/sfm_regions_provider_cache.hpp"
#include "openMVG/matching_image_collection/F_ACRobust.hpp"
#include "openMVG/matching_image_collection/E_ACRobust.hpp"
#include "openMVG/matching_image_collection/E_ACRobust_Angular.hpp"
#include "openMVG/matching_image_collection/Eo_Robust.hpp"
#include "openMVG/matching_image_collection/H_ACRobust.hpp"
#include "openMVG/matching_image_collection/Pair_Builder.hpp"
#include "openMVG/matching/pairwiseAdjacencyDisplay.hpp"
#include "MVS.h"

#include "openMVG/sfm/pipelines/sequential/sequential_SfM.hpp"
#include "openMVG/sfm/pipelines/sfm_features_provider.hpp"
#include "openMVG/sfm/pipelines/sfm_matches_provider.hpp"

#include "nonFree/sift/SIFT_describer.hpp"
#include "nonFree/sift/SIFT_describer_io.hpp"

#include "cereal/archives/json.hpp"

#include "openMVG/features/akaze/image_describer_akaze_io.hpp"

#include "openMVG/features/sift/SIFT_Anatomy_Image_Describer_io.hpp"
#include "cereal/details/helpers.hpp"
#include "site.h"
#include <omp.h>

/// OpenCV Includes
#include <opencv2/opencv.hpp>
#include "opencv2/core/eigen.hpp"
#include "opencv2/xfeatures2d.hpp"


//#include "opencv4/opencv2/opencv.hpp"
//#include "opencv4/opencv2/core/eigen.hpp"
//#include "opencv4/opencv2/xfeatures2d.hpp"


#include "MVS/Common.h"
#include "MVS/Scene.h"
#include <ceres/types.h>
#include "error.h"
#include "lsmio.h"


#include "photogrammetry_tovms.h"
using namespace openMVG;
using namespace openMVG::matching;
using namespace openMVG::robust;
using namespace openMVG::sfm;
using namespace openMVG::exif;
using namespace openMVG::matching_image_collection;
using namespace openMVG::geodesy;
using namespace openMVG::image;
using namespace openMVG::cameras;
using namespace openMVG::features;
//using namespace std;


typedef features::AKAZE_Float_Regions AKAZE_OpenCV_Regions;

class AKAZE_OCV_Image_describer : public Image_describer
{
public:
  using Regions_type = AKAZE_OpenCV_Regions;

    float m_Threshold;

  AKAZE_OCV_Image_describer(float threshold):Image_describer(),m_Threshold(threshold){}
  AKAZE_OCV_Image_describer():Image_describer(),m_Threshold(1.0){}

  bool Set_configuration_preset(EDESCRIBER_PRESET preset) override
  {
    return false;
  }
  /**
  @brief Detect regions on the image and compute their attributes (description)
  @param image Image.
  @param mask 8-bit gray image for keypoint filtering (optional).
     Non-zero values depict the region of interest.
  @return regions The detected regions and attributes (the caller must delete the allocated data)
  */
  std::unique_ptr<Regions> Describe(
      const Image<unsigned char>& image,
      const Image<unsigned char> * mask = nullptr
  ) override
  {
    return Describe_AKAZE_OCV(image, mask);
  }

  /**
  @brief Detect regions on the image and compute their attributes (description)
  @param image Image.
  @param mask 8-bit gray image for keypoint filtering (optional).
     Non-zero values depict the region of interest.
  @return regions The detected regions and attributes (the caller must delete the allocated data)
  */
  std::unique_ptr<Regions_type> Describe_AKAZE_OCV(
    const Image<unsigned char>& image,
    const Image<unsigned char> * mask = nullptr
  )
  {
    auto regions = std::unique_ptr<Regions_type>(new Regions_type);

    cv::Mat img;
    cv::eigen2cv(image.GetMat(), img);

    cv::Mat m_mask;
    if (mask != nullptr) {
      cv::eigen2cv(mask->GetMat(), m_mask);
    }

    std::vector< cv::KeyPoint > vec_keypoints;
    cv::Mat m_desc;

    cv::Ptr<cv::Feature2D> extractor = cv::AKAZE::create(cv::AKAZE::DESCRIPTOR_KAZE,0,3,m_Threshold/1000.0);
    extractor->detectAndCompute(img, m_mask, vec_keypoints, m_desc);

    if (!vec_keypoints.empty())
    {
      // reserve some memory for faster keypoint saving
      regions->Features().reserve(vec_keypoints.size());
      regions->Descriptors().reserve(vec_keypoints.size());

      using DescriptorT = Descriptor<float, 64>;
      DescriptorT descriptor;
      int cpt = 0;
      for (auto i_keypoint = vec_keypoints.begin(); i_keypoint != vec_keypoints.end(); ++i_keypoint, ++cpt){
        const SIOPointFeature feat((*i_keypoint).pt.x, (*i_keypoint).pt.y, (*i_keypoint).size, (*i_keypoint).angle);
        regions->Features().push_back(feat);

        memcpy(descriptor.data(),
               m_desc.ptr<typename DescriptorT::bin_type>(cpt),
               DescriptorT::static_size*sizeof(DescriptorT::bin_type));
        regions->Descriptors().push_back(descriptor);
      }
    }
    return regions;
  };

  /// Allocate Regions type depending of the Image_describer
  std::unique_ptr<Regions> Allocate() const override
  {
    return std::unique_ptr<Regions_type>(new Regions_type);
  }

  template<class Archive>
  void serialize( Archive & ar )
  {
  }
};
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
CEREAL_REGISTER_TYPE_WITH_NAME(AKAZE_OCV_Image_describer, "AKAZE_OCV_Image_describer");
CEREAL_REGISTER_POLYMORPHIC_RELATION(openMVG::features::Image_describer, AKAZE_OCV_Image_describer)


class SIFT_OPENCV_Image_describer : public Image_describer
{
public:
  using Regions_type = SIFT_Regions;

 float m_Threshold;
  SIFT_OPENCV_Image_describer(float threshold = 100) : Image_describer(), m_Threshold (threshold) {}

  ~SIFT_OPENCV_Image_describer() {}

  bool Set_configuration_preset(EDESCRIBER_PRESET preset){
    return true;
  }

  /**
  @brief Detect regions on the image and compute their attributes (description)
  @param image Image.
  @param mask 8-bit gray image for keypoint filtering (optional).
     Non-zero values depict the region of interest.
  @return regions The detected regions and attributes (the caller must delete the allocated data)
  */
  std::unique_ptr<Regions> Describe(
    const image::Image<unsigned char>& image,
    const image::Image<unsigned char> * mask = nullptr
  ) override
  {
    return Describe_SIFT_OPENCV(image, mask);
  }

  /**
  @brief Detect regions on the image and compute their attributes (description)
  @param image Image.
  @param mask 8-bit gray image for keypoint filtering (optional).
     Non-zero values depict the region of interest.
  @return regions The detected regions and attributes (the caller must delete the allocated data)
  */
  std::unique_ptr<Regions_type> Describe_SIFT_OPENCV(
      const image::Image<unsigned char>& image,
      const image::Image<unsigned char>* mask = nullptr
  )
  {
    std::cout << "photogrammetry siftopencv 1 " << std::endl;
    // Convert for opencv
    cv::Mat img;
    std::cout << "photogrammetry siftopencv 1 " << std::endl;
    cv::eigen2cv(image.GetMat(), img);

    std::cout << "photogrammetry siftopencv 1 " << std::endl;

    // Convert mask image into cv::Mat
    cv::Mat m_mask;
    if (mask != nullptr) {
      cv::eigen2cv(mask->GetMat(), m_mask);
    }

    std::cout << "photogrammetry siftopencv 2 " << std::endl;

    // Create a SIFT detector
    std::vector< cv::KeyPoint > v_keypoints;
    cv::Mat m_desc;
    //get surf detector (use extended surf for 128 length descriptor)

    cv::Ptr<cv::Feature2D> siftdetector = cv::xfeatures2d::SURF::create(m_Threshold * 100.0,4,3,true,false);

    std::cout << "photogrammetry siftopencv 3 " << std::endl;

    // Process SIFT computation
    siftdetector->detectAndCompute(img, m_mask, v_keypoints, m_desc);

    std::cout << "photogrammetry siftopencv 4 " << std::endl;

    auto regions = std::unique_ptr<Regions_type>(new Regions_type);

    std::cout << "photogrammetry siftopencv 5 " << std::endl;

    // reserve some memory for faster keypoint saving
    regions->Features().reserve(v_keypoints.size());
    regions->Descriptors().reserve(v_keypoints.size());

    std::cout << "photogrammetry siftopencv 6" << std::endl;

    // Prepare a column vector with the sum of each descriptor
    cv::Mat m_siftsum;
    cv::reduce(m_desc, m_siftsum, 1, cv::REDUCE_SUM);

    std::cout << "photogrammetry siftopencv 7 " << m_desc.size().width << "  " << m_desc.size().height <<  std::endl;

    // Copy keypoints and descriptors in the regions
    int cpt = 0;
    for (auto i_kp = v_keypoints.begin();
        i_kp != v_keypoints.end();
        ++i_kp, ++cpt)
    {
        //std::cout << "keypoint "<< cpt << std::endl;

      SIOPointFeature feat((*i_kp).pt.x, (*i_kp).pt.y, (*i_kp).size, (*i_kp).angle);
      regions->Features().push_back(feat);

      Descriptor<unsigned char, 128> desc;
      for (int j = 0; j < 128; j++)
      {
        desc[j] = static_cast<unsigned char>(512.0*sqrt(m_desc.at<float>(cpt, j)/m_siftsum.at<float>(cpt, 0)));
      }
      regions->Descriptors().push_back(desc);
    }
    std::cout << "photogrammetry siftopencv 8 " << std::endl;

    return regions;
  };

  /// Allocate Regions type depending of the Image_describer
  std::unique_ptr<Regions> Allocate() const override
  {
    return std::unique_ptr<Regions_type>(new Regions_type);
  }

  template<class Archive>
  void serialize( Archive & ar )
  {
  }
};
CEREAL_REGISTER_TYPE_WITH_NAME(SIFT_OPENCV_Image_describer, "SIFT_OPENCV_Image_describer");
CEREAL_REGISTER_POLYMORPHIC_RELATION(openMVG::features::Image_describer, SIFT_OPENCV_Image_describer)

enum EGeometricModel
{
  FUNDAMENTAL_MATRIX = 0,
  ESSENTIAL_MATRIX   = 1,
  HOMOGRAPHY_MATRIX  = 2,
  ESSENTIAL_MATRIX_ANGULAR = 3,
  ESSENTIAL_MATRIX_ORTHO = 4,
  ESSENTIAL_MATRIX_UPRIGHT = 5
};

enum class ESfMSceneInitializer
{
  INITIALIZE_EXISTING_POSES,
  INITIALIZE_MAX_PAIR,
  INITIALIZE_AUTO_PAIR,
  INITIALIZE_STELLAR
};

enum EPairMode
{
  PAIR_EXHAUSTIVE = 0,
  PAIR_CONTIGUOUS = 1,
  PAIR_FROM_FILE  = 2
};

std::pair<bool, SEACAVE::MSVec3> checkGPS
(
  const std::string & filename,
  const int & GPS_to_XYZ_method = 0
)
{
  std::pair<bool, SEACAVE::MSVec3> val(false, SEACAVE::MSVec3(0.0,0.0,0.0));
  std::unique_ptr<openMVG::exif::Exif_IO> exifReader(new Exif_IO_EasyExif);
  if (exifReader)
  {
    // Try to parse EXIF metada & check existence of EXIF data
    if ( exifReader->open( filename ) && exifReader->doesHaveExifInfo() )
    {
      // Check existence of GPS coordinates
      double latitude, longitude, altitude;
      if ( exifReader->GPSLatitude( &latitude ) &&
           exifReader->GPSLongitude( &longitude ) &&
           exifReader->GPSAltitude( &altitude ) )
      {
        // Add ECEF or UTM XYZ position to the GPS position array
        val.first = true;
        switch (GPS_to_XYZ_method)
        {
          case 1:
            val.second = lla_to_utm( latitude, longitude, altitude );
            break;
          case 0:
          default:
            val.second = lla_to_ecef( latitude, longitude, altitude );
            break;
        }
      }
    }
  }
  return val;
}


inline void PhotogrammatryRun()
{
    std::unique_ptr<Matcher> collectionMatcher;
    std::cout << "Using FAST_CASCADE_HASHING_L2 matcher" << std::endl;
    collectionMatcher.reset(new Cascade_Hashing_Matcher_Regions(1));

    MVS::Scene scene();
}


features::EDESCRIBER_PRESET stringToEnum(const std::string & sPreset)
{
  features::EDESCRIBER_PRESET preset;
  if (sPreset == "NORMAL")
    preset = features::NORMAL_PRESET;
  else
  if (sPreset == "HIGH")
    preset = features::HIGH_PRESET;
  else
  if (sPreset == "ULTRA")
    preset = features::ULTRA_PRESET;
  else
    preset = features::EDESCRIBER_PRESET(-1);
  return preset;
}

inline void AS_Photogrammetry_PrepareList(std::vector<QString> files, QString temp_file, double focal_pixels = -1.0, bool use_prior = false)
{

    std::string sImageDir = ((AS_DIR)).toStdString();
      //sfileDatabase = "",
      //sOutputDir = "",
      //sKmatrix;

    bool b_Group_camera_model = true;
    std::string sPriorWeights;
    std::pair<bool, SEACAVE::MSVec3> prior_w_info(false, SEACAVE::MSVec3(1.0,1.0,1.0));

    std::string sfileDatabase = (GetSite() + LISEM_FOLDER_ASSETS + "sensor_width_camera_database.txt").toStdString();

    std::vector<Datasheet> vec_database;
      if (!sfileDatabase.empty())
      {
        if ( !parseDatabase( sfileDatabase, vec_database ) )
        {
           LISEMS_ERROR("Could not find camera sensor width database at " + QString(sfileDatabase.c_str()));
           throw 1;
        }
      }


    // Expected properties for each image
    double width = -1, height = -1, focal = -1, ppx = -1,  ppy = -1;

    int i_User_camera_model = openMVG::cameras::PINHOLE_CAMERA_RADIAL3;
    const openMVG::cameras::EINTRINSIC e_User_camera_model = openMVG::cameras::EINTRINSIC(i_User_camera_model);


    std::vector<std::string> vec_image;

    for(int i = 0; i < files.size(); i++)
    {
        vec_image.push_back(files.at(i).toStdString());
    }

    std::sort(vec_image.begin(), vec_image.end());

    SfM_Data sfm_data;
    sfm_data.s_root_path = sImageDir; // Setup main image root_path
    Views & views = sfm_data.views;
    Intrinsics & intrinsics = sfm_data.intrinsics;

    int n = 0;
    for ( std::vector<std::string>::const_iterator iter_image = vec_image.begin();
      iter_image != vec_image.end();
      ++iter_image)
    {
        // Read meta data to fill camera parameter (w,h,focal,ppx,ppy) fields.
        width = height = ppx = ppy = focal = -1.0;

        const std::string sImageFilename = stlplus::create_filespec( sImageDir, *iter_image );
        const std::string sImFilenamePart = stlplus::filename_part(sImageFilename);

        // Test if the image format is supported:
        if (openMVG::image::GetFormat(sImageFilename.c_str()) == openMVG::image::Unknown)
        {
            LISEMS_ERROR("File " + QString(sImageFilename.c_str()) + " does not contain a supported file format (png/tif/jpeg preferred)");
            throw 1;

        }

        ImageHeader imgHeader;
        if (!openMVG::image::ReadImageHeader(sImageFilename.c_str(), &imgHeader))
        {
            LISEMS_WARNING("Could not load image header for " + QString(sImageFilename.c_str()) +" Skipping file");
            continue; // image cannot be read
        }

        width = imgHeader.width;
        height = imgHeader.height;
        ppx = width / 2.0;
        ppy = height / 2.0;

        if (focal_pixels != -1 )
        {
               focal = focal_pixels;
        }

        if (focal == -1)
        {
          std::unique_ptr<Exif_IO> exifReader(new Exif_IO_EasyExif);
          exifReader->open( sImageFilename );

          const bool bHaveValidExifMetadata =
            exifReader->doesHaveExifInfo()
            && !exifReader->getModel().empty();

          if (bHaveValidExifMetadata) // If image contains meta data
          {
            const std::string sCamModel = exifReader->getModel();

            // Handle case where focal length is equal to 0
            if (exifReader->getFocal() == 0.0f)
            {
                LISEMS_WARNING("Could not get focal length for " + QString(sImageFilename.c_str()) +" Skipping file");
                continue; // image cannot be read

              focal = -1.0;
            }
            else
            // Create the image entry in the list file
            {
              Datasheet datasheet;
              if ( getInfo( sCamModel, vec_database, datasheet ))
              {
                // The camera model was found in the database so we can compute it's approximated focal length
                const double ccdw = datasheet.sensorSize_;
                focal = std::max ( width, height ) * exifReader->getFocal() / ccdw;
                std::cout << "Focal = " << focal << std::endl;
              }
              else
              {
                  LISEMS_WARNING("Could not get sensor width for " + QString(sImageFilename.c_str()) +" Skipping file");
                  continue; // image cannot be read
              }
            }
          }
        }

        // Build intrinsic parameter related to the view
            std::shared_ptr<IntrinsicBase> intrinsic;

        if (focal > 0 && ppx > 0 && ppy > 0 && width > 0 && height > 0)
            {
              // Create the desired camera type
              switch (e_User_camera_model)
              {
                case PINHOLE_CAMERA:
                  intrinsic = std::make_shared<Pinhole_Intrinsic>
                    (width, height, focal, ppx, ppy);
                break;
                case PINHOLE_CAMERA_RADIAL1:
                  intrinsic = std::make_shared<Pinhole_Intrinsic_Radial_K1>
                    (width, height, focal, ppx, ppy, 0.0); // setup no distortion as initial guess
                break;
                case PINHOLE_CAMERA_RADIAL3:
                  intrinsic = std::make_shared<Pinhole_Intrinsic_Radial_K3>
                    (width, height, focal, ppx, ppy, 0.0, 0.0, 0.0);  // setup no distortion as initial guess
                break;
                case PINHOLE_CAMERA_BROWN:
                  intrinsic = std::make_shared<Pinhole_Intrinsic_Brown_T2>
                    (width, height, focal, ppx, ppy, 0.0, 0.0, 0.0, 0.0, 0.0); // setup no distortion as initial guess
                break;
                case PINHOLE_CAMERA_FISHEYE:
                  intrinsic = std::make_shared<Pinhole_Intrinsic_Fisheye>
                    (width, height, focal, ppx, ppy, 0.0, 0.0, 0.0, 0.0); // setup no distortion as initial guess
                break;
                case CAMERA_SPHERICAL:
                   intrinsic = std::make_shared<Intrinsic_Spherical>
                     (width, height);
                break;
                default:
                  LISEMS_ERROR("Error: unknown camera model: " + QString::number( (int) e_User_camera_model));
                  throw 1;
              }
            }


        // Build the view corresponding to the image
            const std::pair<bool, SEACAVE::MSVec3> gps_info = checkGPS(sImageFilename, 1);
            if (gps_info.first && use_prior)
            {
              ViewPriors v(*iter_image, views.size(), views.size(), views.size(), width, height);

              // Add intrinsic related to the image (if any)
              if (intrinsic == nullptr)
              {
                //Since the view have invalid intrinsic data
                // (export the view, with an invalid intrinsic field value)
                v.id_intrinsic = UndefinedIndexT;
              }
              else
              {
                // Add the defined intrinsic to the sfm_container
                intrinsics[v.id_intrinsic] = intrinsic;
              }

              v.b_use_pose_center_ = true;
              v.pose_center_ = gps_info.second;
              // prior weights
              if (prior_w_info.first == true)
              {
                v.center_weight_ = prior_w_info.second;
              }

              // Add the view to the sfm_container
              views[v.id_view] = std::make_shared<ViewPriors>(v);
              n++;
            }
            else
            {
              View v(*iter_image, views.size(), views.size(), views.size(), width, height);

              // Add intrinsic related to the image (if any)
              if (intrinsic == nullptr)
              {
                //Since the view have invalid intrinsic data
                // (export the view, with an invalid intrinsic field value)
                v.id_intrinsic = UndefinedIndexT;
              }
              else
              {
                // Add the defined intrinsic to the sfm_container
                intrinsics[v.id_intrinsic] = intrinsic;
              }

              // Add the view to the sfm_container
              views[v.id_view] = std::make_shared<View>(v);
              n++;
            }
    }

    LISEMS_STATUS("Parsed files for photogrammetry. Listed a total of " +QString::number(n));


    GroupSharedIntrinsics(sfm_data);


    std::string file_out = stlplus::create_filespec( sImageDir, temp_file.toStdString() );
    if (!Save(
        sfm_data,
       file_out.c_str(),
        ESfM_Data(VIEWS|INTRINSICS)))
      {
        LISEMS_ERROR("Could not write image listing for photogrammetry, file: " + QString(file_out.c_str()));
        throw 1;
      }
}


inline void AS_Photogrammetry_FeatureCompute(QString temp_file, QString outputdir, QString method,  float threshold ,QString order)
{

    std::string sSfM_Data_Filename = (AS_DIR + temp_file).toStdString();
      std::string sOutDir = (AS_DIR + outputdir).toStdString();
    bool bUpRight = false;
      std::string sImage_Describer_Method = method.toStdString();
    bool bForce = false;
     std::string sFeaturePreset = order.toStdString();

     // Create output dir
       if (!stlplus::folder_exists(sOutDir))
       {
         if (!stlplus::folder_create(sOutDir))
         {
             LISEMS_ERROR("Can not create output directory" + AS_DIR+ outputdir);
             throw 1;
         }
       }

    SfM_Data sfm_data;
    if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS|INTRINSICS))) {
      LISEMS_ERROR("Can not load view listing at " + AS_DIR+ temp_file);
      throw 1;
    }


    using namespace openMVG::features;
    std::unique_ptr<Image_describer> image_describer;

    {

    // Create the desired Image_describer method.
        // Don't use a factory, perform direct allocation
        if (sImage_Describer_Method == "SIFT")
        {
          image_describer.reset(new SIFT_OPENCV_Image_describer(threshold));
        }
        else
        if (sImage_Describer_Method == "SIFT_AKAZE")
        {
            image_describer.reset(new AKAZE_OCV_Image_describer(threshold));
        }
        else
        if (sImage_Describer_Method == "AKAZE_FLOAT")
        {
          image_describer = AKAZE_Image_describer::create
            (AKAZE_Image_describer::Params(AKAZE::Params(), AKAZE_MSURF), !bUpRight);
        }
        else
        if (sImage_Describer_Method == "AKAZE_MLDB")
        {
          image_describer = AKAZE_Image_describer::create
            (AKAZE_Image_describer::Params(AKAZE::Params(), AKAZE_MLDB), !bUpRight);
        }
        if (!image_describer)
        {
            LISEMS_ERROR("Can not create feature describer of type: " + QString(sImage_Describer_Method.c_str()));
            throw 1;
        }
        else
        {
          if (!sFeaturePreset.empty())
          if (!image_describer->Set_configuration_preset(stringToEnum(sFeaturePreset)))
          {
              LISEMS_ERROR("Can not create feature configuration: " + QString(sFeaturePreset.c_str()));
              throw 1;
          }
        }

        // Export the used Image_describer and region type for:
        // - dynamic future regions computation and/or loading


        const std::string sImage_describer = stlplus::create_filespec(sOutDir, "image_describer", "json");

        {
          std::ofstream stream(sImage_describer.c_str());
          if (!stream.is_open())
          {
              LISEMS_ERROR("Can not write file to output directory: " + QString(sImage_describer.c_str()));
              throw 1;
          }


          cereal::JSONOutputArchive archive(stream);
          archive(cereal::make_nvp("image_describer", image_describer));
          auto regionsType = image_describer->Allocate();
          archive(cereal::make_nvp("regions_type", regionsType));
        }


    }

    std::atomic<bool> preemptive_exit(false);

    std::atomic<int> n;
    Image<unsigned char> imageGray;
    //#pragma omp parallel for schedule(dynamic) private(imageGray)
    for (int i = 0; i < static_cast<int>(sfm_data.views.size()); ++i)
    {
        Views::const_iterator iterViews = sfm_data.views.begin();
        std::advance(iterViews, i);
        const View * view = iterViews->second.get();
        const std::string
          sView_filename = stlplus::create_filespec(sfm_data.s_root_path, view->s_Img_path),
          sFeat = stlplus::create_filespec(sOutDir, stlplus::basename_part(sView_filename), "feat"),
          sDesc = stlplus::create_filespec(sOutDir, stlplus::basename_part(sView_filename), "desc");

        if (!ReadImage(sView_filename.c_str(), &imageGray))
                  continue;


        if (!preemptive_exit)
        {

            // Compute features and descriptors and export them to files
            auto regions = image_describer->Describe(imageGray, nullptr);
            if (regions && !image_describer->Save(regions.get(), sFeat, sDesc))
            {
                LISEMS_ERROR("Can not write file to output directory: " + QString(sFeat.c_str()) + " and " + QString(sDesc.c_str()));
                throw 1;

              preemptive_exit = true;
              continue;
            }


        }

        LISEMS_STATUS("Image feature description generated (" + QString::number(n++) + "/" + QString::number(static_cast<int>(sfm_data.views.size())) + ")");
    }

}



inline void AS_Photogrammetry_FeatureMatch(QString temp_file, QString outputdir, QString geometrymodel)
{

    std::string sMatchesDirectory = (AS_DIR + outputdir).toStdString();
    std::string sGeometricModel = geometrymodel.toStdString();
    float fDistRatio = 0.8f;
    int iMatchingVideoMode = -1;
    std::string sPredefinedPairList = "";
    std::string sNearestMatchingMethod = "AUTO";
    bool bForce = false;
    bool bGuided_matching = false;
    int imax_iteration = 2048;
    unsigned int ui_max_cache_size = 0;
    EPairMode ePairmode = (iMatchingVideoMode == -1 ) ? PAIR_EXHAUSTIVE : PAIR_CONTIGUOUS;
    std::string sSfM_Data_Filename = (AS_DIR + temp_file).toStdString();
    std::string sOutDir = (AS_DIR + outputdir).toStdString();

    if (sMatchesDirectory.empty() || !stlplus::is_folder(sMatchesDirectory))  {
        LISEMS_ERROR("Invalid output directory, either empty or doesnt exist");
        throw 1;
      }

    EGeometricModel eGeometricModelToCompute = FUNDAMENTAL_MATRIX;
      std::string sGeometricMatchesFilename = "";
      switch (sGeometricModel[0])
      {
        case 'f': case 'F':
          eGeometricModelToCompute = FUNDAMENTAL_MATRIX;
          sGeometricMatchesFilename = "matches.f.bin";
        break;
        case 'e': case 'E':
          eGeometricModelToCompute = ESSENTIAL_MATRIX;
          sGeometricMatchesFilename = "matches.e.bin";
        break;
        case 'h': case 'H':
          eGeometricModelToCompute = HOMOGRAPHY_MATRIX;
          sGeometricMatchesFilename = "matches.h.bin";
        break;
        case 'a': case 'A':
          eGeometricModelToCompute = ESSENTIAL_MATRIX_ANGULAR;
          sGeometricMatchesFilename = "matches.f.bin";
        break;
        case 'o': case 'O':
          eGeometricModelToCompute = ESSENTIAL_MATRIX_ORTHO;
          sGeometricMatchesFilename = "matches.o.bin";
        break;
        case 'u': case 'U':
          eGeometricModelToCompute = ESSENTIAL_MATRIX_UPRIGHT;
          sGeometricMatchesFilename = "matches.f.bin";
        break;
        default:
          LISEMS_ERROR("Unknown geometry model type");
          throw 1;
      }

      SfM_Data sfm_data;
      if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS|INTRINSICS))) {
        LISEMS_ERROR("Can not load view listing at " + AS_DIR+ temp_file);
        throw 1;
      }

      // Load the corresponding view regions
      std::shared_ptr<Regions_Provider> regions_provider;
      if (ui_max_cache_size == 0)
      {
        // Default regions provider (load & store all regions in memory)
        regions_provider = std::make_shared<Regions_Provider>();
      }
      else
      {
        // Cached regions provider (load & store regions on demand)
        regions_provider = std::make_shared<Regions_Provider_Cache>(ui_max_cache_size);
      }

      using namespace openMVG::features;
        const std::string sImage_describer = stlplus::create_filespec(sMatchesDirectory, "image_describer", "json");
        std::unique_ptr<Regions> regions_type = Init_region_type_from_file(sImage_describer);
        if (!regions_type)
        {
            LISEMS_ERROR("Invalid regions");
            throw 1;
        }

      C_Progress_display progress;
      if (!regions_provider->load(sfm_data, sMatchesDirectory, regions_type, &progress)) {
          LISEMS_ERROR("Invalid regions");
          throw 1;
        }

      PairWiseMatches map_PutativesMatches;

        // Build some alias from SfM_Data Views data:
        // - List views as a vector of filenames & image sizes
        std::vector<std::string> vec_fileNames;
        std::vector<std::pair<size_t, size_t>> vec_imagesSize;
        {
          vec_fileNames.reserve(sfm_data.GetViews().size());
          vec_imagesSize.reserve(sfm_data.GetViews().size());
          for (Views::const_iterator iter = sfm_data.GetViews().begin();
            iter != sfm_data.GetViews().end();
            ++iter)
          {
            const View * v = iter->second.get();
            vec_fileNames.push_back(stlplus::create_filespec(sfm_data.s_root_path,
                v->s_Img_path));
            vec_imagesSize.push_back( std::make_pair( v->ui_width, v->ui_height) );
          }
        }


        std::unique_ptr<Matcher> collectionMatcher;
            if (sNearestMatchingMethod == "AUTO")
            {
              if (regions_type->IsScalar())
              {
                std::cout << "Using FAST_CASCADE_HASHING_L2 matcher" << std::endl;
                collectionMatcher.reset(new Cascade_Hashing_Matcher_Regions(fDistRatio));
              }
              else
              if (regions_type->IsBinary())
              {
                std::cout << "Using BRUTE_FORCE_HAMMING matcher" << std::endl;
                collectionMatcher.reset(new Matcher_Regions(fDistRatio, BRUTE_FORCE_HAMMING));
              }
            }
            else
            if (sNearestMatchingMethod == "BRUTEFORCEL2")
            {
              std::cout << "Using BRUTE_FORCE_L2 matcher" << std::endl;
              collectionMatcher.reset(new Matcher_Regions(fDistRatio, BRUTE_FORCE_L2));
            }
            else
            if (sNearestMatchingMethod == "BRUTEFORCEHAMMING")
            {
              std::cout << "Using BRUTE_FORCE_HAMMING matcher" << std::endl;
              collectionMatcher.reset(new Matcher_Regions(fDistRatio, BRUTE_FORCE_HAMMING));
            }
            else
            if (sNearestMatchingMethod == "HNSWL2")
            {
              std::cout << "Using HNSWL2 matcher" << std::endl;
              collectionMatcher.reset(new Matcher_Regions(fDistRatio, HNSW_L2));
            }
            else
            if (sNearestMatchingMethod == "ANNL2")
            {
              std::cout << "Using ANN_L2 matcher" << std::endl;
              collectionMatcher.reset(new Matcher_Regions(fDistRatio, ANN_L2));
            }
            else
            if (sNearestMatchingMethod == "CASCADEHASHINGL2")
            {
              std::cout << "Using CASCADE_HASHING_L2 matcher" << std::endl;
              collectionMatcher.reset(new Matcher_Regions(fDistRatio, CASCADE_HASHING_L2));
            }
            else
            if (sNearestMatchingMethod == "FASTCASCADEHASHINGL2")
            {
              std::cout << "Using FAST_CASCADE_HASHING_L2 matcher" << std::endl;
              collectionMatcher.reset(new Cascade_Hashing_Matcher_Regions(fDistRatio));
            }


            if (!collectionMatcher)
            {
                LISEMS_ERROR("Could not create feature matcher");
                throw 1;


            }

            {
              // From matching mode compute the pair list that have to be matched:
              Pair_Set pairs;
              switch (ePairmode)
              {
                case PAIR_EXHAUSTIVE: pairs = exhaustivePairs(sfm_data.GetViews().size()); break;
                case PAIR_CONTIGUOUS: pairs = contiguousWithOverlap(sfm_data.GetViews().size(), iMatchingVideoMode); break;

              }
              // Photometric matching of putative pairs
              collectionMatcher->Match(regions_provider, pairs, map_PutativesMatches, &progress);
              //---------------------------------------
              //-- Export putative matches
              //---------------------------------------
              if (!Save(map_PutativesMatches, std::string(sMatchesDirectory + "/matches.putative.bin")))
              {
                LISEMS_ERROR("Could not save matches in" +QString(std::string(sMatchesDirectory).c_str()) + "/matches.putative.bin");
                throw 1;
              }
            }

            PairWiseMatchingToAdjacencyMatrixSVG(vec_fileNames.size(),
                map_PutativesMatches,
                stlplus::create_filespec(sMatchesDirectory, "PutativeAdjacencyMatrix", "svg"));
              //-- export view pair graph once putative graph matches have been computed
              {
                std::set<IndexT> set_ViewIds;
                std::transform(sfm_data.GetViews().begin(), sfm_data.GetViews().end(),
                  std::inserter(set_ViewIds, set_ViewIds.begin()), stl::RetrieveKey());
                graph::indexedGraph putativeGraph(set_ViewIds, getPairs(map_PutativesMatches));
                graph::exportToGraphvizData(
                  stlplus::create_filespec(sMatchesDirectory, "putative_matches"),
                  putativeGraph);
              }

            std::unique_ptr<ImageCollectionGeometricFilter> filter_ptr(
                new ImageCollectionGeometricFilter(&sfm_data, regions_provider));

            if (!filter_ptr)
              {
                LISEMS_ERROR("Could not create match geometry filter");
                throw 1;
            }

            const double d_distance_ratio = 0.6;

            PairWiseMatches map_GeometricMatches;
                switch (eGeometricModelToCompute)
                {
                  case HOMOGRAPHY_MATRIX:
                  {
                    const bool bGeometric_only_guided_matching = true;
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_HMatrix_AC(4.0, imax_iteration),
                      map_PutativesMatches, bGuided_matching,
                      bGeometric_only_guided_matching ? -1.0 : d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                  }
                  break;
                  case FUNDAMENTAL_MATRIX:
                  {
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_FMatrix_AC(4.0, imax_iteration),
                      map_PutativesMatches, bGuided_matching, d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                  }
                  break;
                  case ESSENTIAL_MATRIX:
                  {
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_EMatrix_AC(4.0, imax_iteration),
                      map_PutativesMatches, bGuided_matching, d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();

                    //-- Perform an additional check to remove pairs with poor overlap
                    std::vector<PairWiseMatches::key_type> vec_toRemove;
                    for (const auto & pairwisematches_it : map_GeometricMatches)
                    {
                      const size_t putativePhotometricCount = map_PutativesMatches.find(pairwisematches_it.first)->second.size();
                      const size_t putativeGeometricCount = pairwisematches_it.second.size();
                      const float ratio = putativeGeometricCount / static_cast<float>(putativePhotometricCount);
                      if (putativeGeometricCount < 50 || ratio < .3f)  {
                        // the pair will be removed
                        vec_toRemove.push_back(pairwisematches_it.first);
                      }
                    }
                    //-- remove discarded pairs
                    for (const auto & pair_to_remove_it : vec_toRemove)
                    {
                      map_GeometricMatches.erase(pair_to_remove_it);
                    }
                  }
                  break;
                  case ESSENTIAL_MATRIX_ANGULAR:
                  {
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_ESphericalMatrix_AC_Angular<false>(4.0, imax_iteration),
                      map_PutativesMatches, bGuided_matching, d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                  }
                  break;
                  case ESSENTIAL_MATRIX_ORTHO:
                  {
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_EOMatrix_RA(2.0, imax_iteration),
                      map_PutativesMatches, bGuided_matching, d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                  }
                  break;
                  case ESSENTIAL_MATRIX_UPRIGHT:
                  {
                    filter_ptr->Robust_model_estimation(
                      GeometricFilter_ESphericalMatrix_AC_Angular<true>(4.0, imax_iteration),
                        map_PutativesMatches, bGuided_matching, d_distance_ratio, &progress);
                    map_GeometricMatches = filter_ptr->Get_geometric_matches();
                  }
                  break;
                }



                    if (!Save(map_GeometricMatches,
                          std::string(sMatchesDirectory + "/" + sGeometricMatchesFilename)))
                        {
                        LISEMS_ERROR("Could not save matches to " + QString(std::string(sMatchesDirectory + "/" + sGeometricMatchesFilename).c_str()));
                        throw 1;
                        }

                    graph::getGraphStatistics(sfm_data.GetViews().size(), getPairs(map_GeometricMatches));

                        //-- export Adjacency matrix
                        std::cout << "\n Export Adjacency Matrix of the pairwise's geometric matches"
                          << std::endl;
                        PairWiseMatchingToAdjacencyMatrixSVG(vec_fileNames.size(),
                          map_GeometricMatches,
                          stlplus::create_filespec(sMatchesDirectory, "GeometricAdjacencyMatrix", "svg"));

                        //-- export view pair graph once geometric filter have been done
                        {
                          std::set<IndexT> set_ViewIds;
                          std::transform(sfm_data.GetViews().begin(), sfm_data.GetViews().end(),
                            std::inserter(set_ViewIds, set_ViewIds.begin()), stl::RetrieveKey());
                          graph::indexedGraph putativeGraph(set_ViewIds, getPairs(map_GeometricMatches));
                          graph::exportToGraphvizData(
                            stlplus::create_filespec(sMatchesDirectory, "geometric_matches"),
                            putativeGraph);
                        }


}



bool computeIndexFromImageNames(
  const SfM_Data & sfm_data,
  const std::pair<std::string,std::string>& initialPairName,
  Pair& initialPairIndex)
{
  if (initialPairName.first == initialPairName.second)
  {
    std::cerr << "\nInvalid image names. You cannot use the same image to initialize a pair." << std::endl;
    return false;
  }

  initialPairIndex = {UndefinedIndexT, UndefinedIndexT};

  /// List views filenames and find the one that correspond to the user ones:
  for (Views::const_iterator it = sfm_data.GetViews().begin();
    it != sfm_data.GetViews().end(); ++it)
  {
    const View * v = it->second.get();
    const std::string filename = stlplus::filename_part(v->s_Img_path);
    if (filename == initialPairName.first)
    {
      initialPairIndex.first = v->id_view;
    }
    else{
      if (filename == initialPairName.second)
      {
        initialPairIndex.second = v->id_view;
      }
    }
  }
  return (initialPairIndex.first != UndefinedIndexT &&
      initialPairIndex.second != UndefinedIndexT);
}


#include "openMVG/sfm/sfm_data_colorization.hpp"

#include "SfMPlyHelper.hpp"

inline void AS_Photogrammetry_IncrementalSFM(QString file_temp, QString outputdir)
{

    std::string sSfM_Data_Filename = (AS_DIR + file_temp).toStdString();
    std::string sMatchesDir = (AS_DIR +outputdir).toStdString();
    std::string sMatchFilename;
    std::string sOutDir = (AS_DIR +outputdir).toStdString();
    std::pair<std::string,std::string> initialPairString("","");
    std::string sIntrinsic_refinement_options = "ADJUST_ALL";
    int i_User_camera_model = PINHOLE_CAMERA_RADIAL3;
    bool b_use_motion_priors = false;
    int triangulation_method = static_cast<int>(ETriangulationMethod::DEFAULT);
    int resection_method  = static_cast<int>(resection::SolverType::DEFAULT);

    const cameras::Intrinsic_Parameter_Type intrinsic_refinement_options =
       cameras::Intrinsic_Parameter_Type::ADJUST_ALL;

    // Load input SfM_Data scene
      SfM_Data sfm_data;
      if (!Load(sfm_data, sSfM_Data_Filename, ESfM_Data(VIEWS|INTRINSICS))) {
          LISEMS_ERROR("Can not load view listing at " + AS_DIR+ file_temp);
          throw 1;
      }

      using namespace openMVG::features;
        const std::string sImage_describer = stlplus::create_filespec(sMatchesDir, "image_describer", "json");
        std::unique_ptr<Regions> regions_type = Init_region_type_from_file(sImage_describer);
        if (!regions_type)
        {
            LISEMS_ERROR("Invalid Regions file type: " + QString(sImage_describer.c_str()));
            throw 1;
        }


        // Features reading
          std::shared_ptr<Features_Provider> feats_provider = std::make_shared<Features_Provider>();
          if (!feats_provider->load(sfm_data, sMatchesDir, regions_type)) {
              LISEMS_ERROR("Invalid features ");
              throw 1;
          }
          // Matches reading
          std::shared_ptr<Matches_Provider> matches_provider = std::make_shared<Matches_Provider>();
          if // Try to read the provided match filename or the default one (matches.f.txt/bin)
          (
            !(matches_provider->load(sfm_data, sMatchFilename) ||
              matches_provider->load(sfm_data, stlplus::create_filespec(sMatchesDir, "matches.f.txt")) ||
              matches_provider->load(sfm_data, stlplus::create_filespec(sMatchesDir, "matches.f.bin")))
          )
          {
              LISEMS_ERROR("Invalid matches ");
              throw 1;
          }

          ESfMSceneInitializer scene_initializer_enum = ESfMSceneInitializer::INITIALIZE_MAX_PAIR;


          std::unique_ptr<SfMSceneInitializer> scene_initializer;
          switch(scene_initializer_enum)
          {
            case ESfMSceneInitializer::INITIALIZE_AUTO_PAIR:
              LISEMS_ERROR("AUTO pair initialization not implemented");
              throw 1;
            break;
            case ESfMSceneInitializer::INITIALIZE_MAX_PAIR:
              scene_initializer.reset(new SfMSceneInitializerMaxPair(sfm_data,
                feats_provider.get(),
                matches_provider.get()));
            break;
            case ESfMSceneInitializer::INITIALIZE_EXISTING_POSES:
              scene_initializer.reset(new SfMSceneInitializer(sfm_data,
                feats_provider.get(),
                matches_provider.get()));
            break;
            case ESfMSceneInitializer::INITIALIZE_STELLAR:
              scene_initializer.reset(new SfMSceneInitializerStellar(sfm_data,
                feats_provider.get(),
                matches_provider.get()));
            break;
            default:
              LISEMS_ERROR("Invalid pair initialization (not recognized)");
              throw 1;
          }
          if (!scene_initializer)
          {
              LISEMS_ERROR("Invalid pair initialization");
              throw 1;
          }


            SequentialSfMReconstructionEngine2 sfmEngine(
              scene_initializer.get(),
              sfm_data,
              sOutDir,
              stlplus::create_filespec(sOutDir, "Reconstruction_Report.html"));

            // Configure the features_provider & the matches_provider
            sfmEngine.SetFeaturesProvider(feats_provider.get());
            sfmEngine.SetMatchesProvider(matches_provider.get());

            // Configure reconstruction parameters
            sfmEngine.Set_Intrinsics_Refinement_Type(intrinsic_refinement_options);
            sfmEngine.SetUnknownCameraType(EINTRINSIC(i_User_camera_model));
            b_use_motion_priors = false;
            sfmEngine.Set_Use_Motion_Prior(b_use_motion_priors);
            sfmEngine.SetTriangulationMethod(static_cast<ETriangulationMethod>(triangulation_method));
            sfmEngine.SetResectionMethod(static_cast<resection::SolverType>(resection_method));



            if (sfmEngine.Process())
            {
              Generate_SfM_Report(sfmEngine.Get_SfM_Data(),
                stlplus::create_filespec(sOutDir, "SfMReconstruction_Report.html"));

              Save(sfmEngine.Get_SfM_Data(),
                stlplus::create_filespec(sOutDir, "sfm_data", ".bin"),
                ESfM_Data(ALL));

              Save(sfmEngine.Get_SfM_Data(),
                stlplus::create_filespec(sOutDir, "cloud_and_poses", ".ply"),
                ESfM_Data(ALL));

            }else
            {
                LISEMS_ERROR("Error during geometry reconstruction for sparse point-cloud");
                throw 1;
            }

            std::vector<openMVG::Vec3> vec_3dPoints, vec_tracksColor, vec_camPosition;
            if (ColorizeTracks(sfmEngine.Get_SfM_Data(), vec_3dPoints, vec_tracksColor))
            {
                std::string colorply = stlplus::create_filespec(sOutDir, "cloud_and_poses_color", ".ply");

                if (!openMVG::plyHelper::exportToPly(vec_3dPoints, vec_camPosition, colorply, &vec_tracksColor))
                    {
                    LISEMS_WARNING("Could not save colorized point cloud");
                }

            }else
            {
                LISEMS_ERROR("Could not colorize point cloud");
                throw 1;
            }

            AS_MGStoVMS(sfmEngine.Get_SfM_Data(),sSfM_Data_Filename,AS_DIR.toStdString() + outputdir.toStdString());

}

#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include "openMVG/cameras/Camera_Intrinsics.hpp"
#include "openMVG/multiview/triangulation_nview.hpp"
#include "openMVG/sfm/sfm_data_triangulation.hpp"
#include "openMVG/geometry/rigid_transformation3D_srt.hpp"
#include "openMVG/geometry/Similarity3.hpp"
#include "openMVG/sfm/sfm_data_BA_ceres.hpp"
#include "openMVG/sfm/sfm_data_transform.hpp"

#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"

inline void AS_Photogrammetry_Register(std::vector<LSMVector3> pos_gcp, std::vector<QString> image1_name,std::vector<LSMVector2> pos1_gcp, std::vector<QString> image2_name,std::vector<LSMVector2> pos2_gcp ,QString temp_file,QString outputdir)
{
    std::string sSfM_Data_Filename = (AS_DIR+temp_file).toStdString();

    std::string sMatchesDir = (AS_DIR +outputdir).toStdString();
    std::string sMatchFilename;
    std::string sOutDir = (AS_DIR +outputdir).toStdString();
    std::pair<std::string,std::string> initialPairString("","");
    std::string sIntrinsic_refinement_options = "ADJUST_ALL";
    int i_User_camera_model = PINHOLE_CAMERA_RADIAL3;
    bool b_use_motion_priors = false;
    int triangulation_method = static_cast<int>(ETriangulationMethod::DEFAULT);
    int resection_method  = static_cast<int>(resection::SolverType::DEFAULT);

    const cameras::Intrinsic_Parameter_Type intrinsic_refinement_options =
       cameras::Intrinsic_Parameter_Type::ADJUST_ALL;

    // Load input SfM_Data scene
      SfM_Data sfm_data;
      if(!openMVG::sfm::Load(sfm_data,
                                    sSfM_Data_Filename,
                                    openMVG::sfm::ESfM_Data(openMVG::sfm::ALL)))
      {
          LISEMS_ERROR("Could not import geometric structure data");
          throw 1;
      }


      //get the input data

      if(!(pos_gcp.size() == image1_name.size() && pos_gcp.size() == pos1_gcp.size() && pos_gcp.size() == pos2_gcp.size() && pos_gcp.size() == image2_name.size()))
      {
          LISEMS_ERROR("Control point data not of equal size");
          throw 1;
      }

      //now set the control points
      for(int i = 0; i< pos_gcp.size(); i++)
      {

          int v1 = -1;
          int v2 = -1;

          for(int j = 0; j < sfm_data.GetViews().size(); j++)
          {
             if( QString(sfm_data.GetViews().at(j)->s_Img_path.c_str()) == image1_name.at(i))
             {
                 v1 =sfm_data.GetViews().at(j)->id_view;
                    break;
             }
          }

          for(int j = 0; j < sfm_data.GetViews().size(); j++)
          {
             if( QString(sfm_data.GetViews().at(j)->s_Img_path.c_str()) == image2_name.at(i))
             {
                 v2 =sfm_data.GetViews().at(j)->id_view;
                    break;
             }
          }

          if(v1 > -1 && v2 > -1)
          {
              Landmark l;
              l.X = openMVG::Vec3(pos_gcp.at(i).x,pos_gcp.at(i).y,pos_gcp.at(i).z);
              Observation o1(openMVG::Vec2(pos1_gcp.at(i).x,pos1_gcp.at(i).y),0);
              Observation o2(openMVG::Vec2(pos2_gcp.at(i).x,pos2_gcp.at(i).y),0);
              l.obs[v1] = o1;
              l.obs[v2] = o2;
              sfm_data.control_points[i] = l;
          }else
          {
              LISEMS_WARNING("Can not find control point " + QString::number(i));

          }

      }



      //do the actual dataset registration based on the real gcp positions (in pos array)
      //and the actual images in which they are visible


      //---
        // registration (coarse):
        // - compute the 3D points corresponding to the control point observation for the SfM scene
        // - compute a coarse registration between the controls points & the triangulated point
        // - transform the scene according the found transformation
        //---
        std::map<IndexT, openMVG::Vec3> vec_control_points, vec_triangulated;
        std::map<IndexT, double> vec_triangulation_errors;
        for (const auto & control_point_it : sfm_data.control_points)
        {
          const Landmark & landmark = control_point_it.second;
          //Triangulate the observations:
          const Observations & obs = landmark.obs;
          std::vector<openMVG::Vec3> bearing;
          std::vector<Mat34> poses;
          bearing.reserve(obs.size());
          poses.reserve(obs.size());
          for (const auto & obs_it : obs)
          {
            const View * view = sfm_data.views.at(obs_it.first).get();
            if (!sfm_data.IsPoseAndIntrinsicDefined(view))
              continue;
            const openMVG::cameras::IntrinsicBase * cam = sfm_data.GetIntrinsics().at(view->id_intrinsic).get();
            const openMVG::geometry::Pose3 pose = sfm_data.GetPoseOrDie(view);
            const openMVG::Vec2 pt = obs_it.second.x;
            bearing.emplace_back((*cam)(cam->get_ud_pixel(pt)));
            poses.emplace_back(pose.asMatrix());
          }
          const Eigen::Map<const Mat3X> bearing_matrix(bearing[0].data(), 3, bearing.size());
          openMVG::Vec4 Xhomogeneous;
          if (!TriangulateNViewAlgebraic(bearing_matrix, poses, &Xhomogeneous))
          {
            std::cout << "Invalid triangulation" << std::endl;
            LISEMS_WARNING("Control Point cannot be triangulated (not in front of the cameras)");
            //sfm_data.control_points.erase(control_point_it.first);
            continue;
          }
          const openMVG::Vec3 X = Xhomogeneous.hnormalized();
          // Test validity of the hypothesis (front of the cameras):
          bool bChierality = true;
          int i(0);
          double reprojection_error_sum(0.0);
          for (const auto & obs_it : obs)
          {
            const View * view = sfm_data.views.at(obs_it.first).get();
            if (!sfm_data.IsPoseAndIntrinsicDefined(view))
              continue;

            const Pose3 pose = sfm_data.GetPoseOrDie(view);
            bChierality &= CheiralityTest(bearing[i], pose, X);
            const openMVG::cameras::IntrinsicBase * cam = sfm_data.GetIntrinsics().at(view->id_intrinsic).get();
            const openMVG::Vec2 pt = obs_it.second.x;
            const openMVG::Vec2 residual = cam->residual(pose(X), pt);
            reprojection_error_sum += residual.norm();
            ++i;
          }
          if (bChierality) // Keep the point only if it has a positive depth
          {
            vec_triangulated[control_point_it.first] = X;
            vec_control_points[control_point_it.first] = landmark.X;
            vec_triangulation_errors[control_point_it.first] = reprojection_error_sum/(double)bearing.size();
          }
          else
          {
            LISEMS_WARNING("Control Point cannot be triangulated (not in front of the cameras)");
            //sfm_data.control_points.erase(control_point_it.first);
            continue;
          }
        }

        if (vec_control_points.size() < 3)
        {
          LISEMS_ERROR("Not enough control points left, must be at least 3 to solve inequality.");
          throw 1;
        }

        // compute the similarity
        {
          // data conversion to appropriate container
          Mat x1(3, vec_control_points.size()),
              x2(3, vec_control_points.size());
          for (size_t i=0; i < vec_control_points.size(); ++i)
          {
            x1.col(i) = vec_triangulated[i];
            x2.col(i) = vec_control_points[i];
          }

          std::cout
            << "Control points observation triangulations:\n"
            << x1 << std::endl << std::endl
            << "Control points coords:\n"
            << x2 << std::endl << std::endl;

          openMVG::Vec3 t;
          Mat3 R;
          double S;
          if (openMVG::geometry::FindRTS(x1, x2, &S, &t, &R))
          {
            openMVG::geometry::Refine_RTS(x1,x2,&S,&t,&R);
            std::cout << "Found transform:\n"
              << " scale: " << S << "\n"
              << " rotation:\n" << R << "\n"
              << " translation: "<< t.transpose() << std::endl;


            //--
            // Apply the found transformation as a 3D Similarity transformation matrix // S * R * X + t
            //--

            const openMVG::geometry::Similarity3 sim(openMVG::geometry::Pose3(R, -R.transpose() * t/S), S);
            openMVG::sfm::ApplySimilarity(sim, sfm_data);
          }else
          {

                LISEM_ERROR("Could not find RTS system for registration");
                throw 1;
          }
        }



       openMVG::sfm::Save(sfm_data,
                                    stlplus::create_filespec(sOutDir, "sfm_data", ".bin"),
                                    openMVG::sfm::ESfM_Data(openMVG::sfm::ALL));



       AS_MGStoVMS(sfm_data,sSfM_Data_Filename,AS_DIR.toStdString() + outputdir.toStdString());

}


