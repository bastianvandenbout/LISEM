#ifndef RASTERFOURIER_H
#define RASTERFOURIER_H

#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbImageFileWriter.h"
#include "geo/raster/map.h"
#include "otbWaveletGenerator.h"
#include <itkConfigure.h>
#include <itkForwardFFTImageFilter.h>
#include <itkInverseFFTImageFilter.h>
#include <itkFFTShiftImageFilter.h>
#include <itkFFTWGlobalConfiguration.h>
#include "otbWaveletImageFilter.h"
#include "otbWaveletInverseImageFilter.h"
#include "opencv2/opencv.hpp"

typedef struct elem_ {
        float f1;
        float f2;
} elem_;

template<> class cv::DataType<elem_>
{
public:
    typedef elem_ value_type;
    typedef int work_type;
    typedef unsigned char channel_type;
    typedef value_type vec_type;
    enum { depth = CV_32F, channels = 2,
           type = CV_MAKETYPE(depth, channels), fmt=(int)'f' };
};

inline static cTMap * RasterInverseFourier(cTMap * real, cTMap * complex)
{

    cTMap * ret = real->GetCopy0();

    cTMap * Map = real;
    cv::Mat original_ = cv::Mat(real->nrRows(),real->nrCols(), CV_32FC2,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<elem_>(r,c).f1 = real->data[r][c];
                original_.at<elem_>(r,c).f2 = complex->data[r][c];
            }else
            {
                original_.at<elem_>(r,c).f1 = 0.0;
                original_.at<elem_>(r,c).f2 = 0.0;
            }
        }
    }

    cv::Mat g;
    cv::idft(original_,g,cv::DFT_REAL_OUTPUT);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            ret->data[r][c] = g.at<float>(r,c);
        }
    }
    return ret;
}

inline static cTMap * AS_RasterInverseFourier(cTMap * in1, cTMap * in2)
{

    return RasterInverseFourier(in1,in2);
}

inline static std::vector<cTMap *> RasterFourier(cTMap * Map)
{

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);
    cv::Mat mask_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_8UC1,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<float>(r,c) = Map->data[r][c];
                mask_.at<uchar>(r,c) = 0.0;
            }else
            {
                original_.at<float>(r,c) = 0.0;
                mask_.at<uchar>(r,c) = 1.0;
            }
        }
    }

    cv::Mat g;
    cv::dft(original_,g,cv::DFT_COMPLEX_OUTPUT);


    cv::Mat planes[2];
    cv::split(g,planes);

    cTMap * real = Map->GetCopy0();
    cTMap * imag = Map->GetCopy0();

    std::cout << g.type() << std::endl;

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            real->data[r][c] = planes[0].at<float>(r,c);
            imag->data[r][c] = planes[1].at<float>(r,c);
        }
    }
    return {real,imag};


}


inline static std::vector<cTMap *> AS_RasterFourier(cTMap * in)
{
    std::vector<cTMap *> ret =  RasterFourier(in);


  return ret;


}




#endif // RASTERFOURIER_H
