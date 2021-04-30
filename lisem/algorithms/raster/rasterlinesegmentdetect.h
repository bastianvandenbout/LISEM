#ifndef RASTERLINESEGMENTDETECT_H
#define RASTERLINESEGMENTDETECT_H

#include "otbImageFileReader.h"
#include "itkBinaryFunctorImageFilter.h"
#include "otbImageFileWriter.h"
#include "otbVectorDataFileWriter.h"
#include "geo/raster/map.h"
#include "geo/shapes/shapefile.h"
#include "otbLineSegmentDetector.h"
#include "Eigen/Eigen"
#include <opencv2/imgproc.hpp>
#include "opencv2/opencv.hpp"

static inline ShapeFile * RasterLineSegmentDetect(cTMap* in_bands)
{

     using InputPixelType         = float;
     using PrecisionType          = double;
     const unsigned int Dimension = 2;

     // As usual, we start by defining the types for the input image and
     // the image file reader.
     using ImageType  = otb::Image<float, Dimension>;
     using ReaderType = otb::ImageFileReader<ImageType>;


     ImageType::Pointer image = ImageType::New();
     unsigned long long xs = in_bands->nrCols();
     unsigned long long ys = in_bands->nrRows();
     std::cout << "size " << xs << " " << ys << std::endl;
     const ImageType::SizeType size = {{xs,ys}};
     const ImageType::IndexType start = {{0,0}};

     ImageType::RegionType region;
     region.SetSize(size);
     region.SetIndex(start);
     image->SetRegions(region);
     ImageType::SpacingType spacing;
     spacing[0] = 1; // spacing along X
     spacing[1] = 1; // spacing along Y
     image->SetSpacing( spacing );

     image->Allocate();

     InputPixelType * data = reinterpret_cast<InputPixelType*>(image->GetBufferPointer());

     int cols = in_bands->nrCols();
     int rows = in_bands->nrRows();

     ImageType::PixelType valtemp;

     for(int r = 0; r < rows; r++)
     {
         for(int c = 0; c < cols; c++)
         {
             const ImageType::IndexType pixelIndex = {{c,r}};
             ImageType::PixelType val;
             val = in_bands->data[r][c];

             image->SetPixel(pixelIndex,val);
         }
     }

     // We define now the type for the segment detector filter. It is
     // templated over the input image type and the precision with which
     // the coordinates of the detected segments will be given. It is
     // recommended to set this precision to a real type. The output of the
     // filter will be a \doxygen{otb}{VectorData}.
     using LsdFilterType = otb::LineSegmentDetector<ImageType, PrecisionType>;

     LsdFilterType::Pointer lsdFilter = LsdFilterType::New();

     lsdFilter->SetInput(image);
     lsdFilter->Update();

     LsdFilterType::OutputVectorDataType::Pointer out = lsdFilter->GetOutput();


     typedef LsdFilterType::OutputVectorDataType::DataTreeType            DataTreeType;
     typedef itk::PreOrderTreeIterator<DataTreeType> TreeIteratorType;

     TreeIteratorType it(out->GetDataTree());


     ShapeFile * ret = new ShapeFile();
     ShapeLayer * lay = new ShapeLayer();
     ret->AddLayer(lay);
     ret->SetProjection(in_bands->GetProjection());

     it.GoToBegin();
      while (!it.IsAtEnd())
      {
      if (it.Get()->IsLineFeature())
        {
          LSMLine * line = new LSMLine();
            otb::PolyLineParametricPathWithValue<double,2>::Pointer pl = it.Get()->GetLine();

            otb::PolyLineParametricPathWithValue<double,2>::VertexListType *vl =  pl->GetVertexList();
            for(int j = 0; j < vl->size(); j++)
            {
                otb::PolyLineParametricPathWithValue<double,2>::VertexType v = vl->at(j);
                double x = v.GetElement(0);
                double y = v.GetElement(1);

                line->AddVertex(in_bands->west() +in_bands->cellSizeX() *x,in_bands->north() +in_bands->cellSizeY() *y);
            }

            Feature * f = new Feature();
            f->AddShape(line);
            lay->AddFeature(f);

        }
      ++it;
      }


      return ret;
}



static ShapeFile * AS_LineSegmentDetect(cTMap * map)
{

    return RasterLineSegmentDetect(map);

}


static inline ShapeFile * RasterCircleDetect(cTMap* in, float radius_min, float radius_max, float threshold = 100,int circle_subdivide = 20)
{

    cv::Mat img, gray;

    img = cv::Mat(in->nrRows(),in->nrCols(), CV_8UC1,0.0);
    gray = cv::Mat(in->nrRows(),in->nrCols(), CV_8UC1,0.0);

    bool is_set =false;
    float min = 1e31;
    float max = -1e31;

    for(int r = 0; r < in->nrRows(); r++)
    {
        for(int c = 0; c < in->nrCols(); c++)
        {
            if(!pcr::isMV(in->data[r][c]))
            {
                if(!is_set)
                {
                    min = in->data[r][c];
                    max = in->data[r][c];
                    is_set = true;
                }else
                {
                    min = std::min(min,in->data[r][c]);
                    max = std::max(max,in->data[r][c]);
                }
            }
        }
    }


    if(max-min > 1e-31)
    {
        for(int r = 0; r < in->nrRows(); r++)
        {
            for(int c = 0; c < in->nrCols(); c++)
            {
                if(!pcr::isMV(in->data[r][c]))
                {



                    img.at<unsigned char>(r,c) = (unsigned char)( 255.0* (in->data[r][c]- min)/std::max(0.0f,max-min));
                }

            }
        }
    }

    int radius_mincs = radius_min/(in->cellSize());
    int radius_maxcs = radius_max/(in->cellSize());

    std::cout << "circle detect 1 " << std::endl;

    // smooth it, otherwise a lot of false circles may be detected
    //GaussianBlur( img,gray, cv::Size(9, 9), 2, 2 );

    std::cout << "circle detect 2 " << std::endl;
    std::vector<cv::Vec3f> circles;
    HoughCircles(img, circles, cv::HOUGH_GRADIENT,
                 2, 5,threshold, threshold,radius_mincs,radius_maxcs);

    std::cout << "circle detect 3 " << std::endl;


    std::cout << "circle detect " << min << " " << max << "  " << circles.size() << std
                 ::endl;

    ShapeFile * ret = new ShapeFile();
    ShapeLayer * lay = new ShapeLayer();
    ret->AddLayer(lay);
    ret->SetProjection(in->GetProjection());

    for( size_t i = 0; i < circles.size(); i++ )
    {
        LSMPolygon * line = new LSMPolygon();

        Ring * r = new Ring();
        r->SetAsCircle(LSMVector2(in->west() + in->cellSizeX() *circles[i][0],in->north() + in->cellSizeY() *circles[i][1]),circles[i][3],in->cellSize() * circle_subdivide);
        line->SetExtRing(r);
        Feature * f = new Feature();
        f->AddShape(line);
        lay->AddFeature(f);
    }

    return ret;
}



static ShapeFile * AS_RasterCircleDetect(cTMap * map, float radius_min, float radius_max,float threshold, int circle_subdivide = 20)
{

    return RasterCircleDetect(map, radius_min, radius_max,threshold,circle_subdivide);

}

//circle detect








#endif // RASTERLINESEGMENTDETECT_H
