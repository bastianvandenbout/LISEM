#ifndef RASTERDISPARITY_H
#define RASTERDISPARITY_H

#include "geo/raster/map.h"
#include <opencv2/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/ximgproc.hpp"

using namespace cv;
using namespace cv::ximgproc;

inline static void MapToCVMatChar(cTMap * in, cv::Mat &img)
{
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

}

inline static cTMap * AS_RasterDisparity(cTMap * first, cTMap * second, float wsize, float max_disp)
{
    if(first->nrCols() != second->nrCols() || first->nrRows() != second->nrRows())
    {
        LISEMS_ERROR("first and second image are not of identical size");
        throw 1;
    }

    if(wsize < 1)
    {
        LISEMS_ERROR("Window size (in pixels) must be at least 1");
        throw 1;
    }
    if(max_disp < 1)
    {
        LISEMS_ERROR("Maximum disparity must be at least 1");
        throw 1;
    }
    cv::Mat left,right;

    left = cv::Mat(first->nrRows(),first->nrCols(), CV_8UC1,0.0);
    right = cv::Mat(second->nrRows(),second->nrCols(), CV_8UC1,0.0);

    MapToCVMatChar(first,left);
    MapToCVMatChar(second,right);

    float lambda = 8000.0;
    float sigma = 1.5;

    cv::Mat left_for_matcher, right_for_matcher;
    cv::Mat left_disp,right_disp;
    cv::Mat filtered_disp,solved_disp,solved_filtered_disp;

    left_for_matcher  = left.clone();
    right_for_matcher = right.clone();

    Ptr<DisparityWLSFilter> wls_filter;

    Ptr<StereoSGBM> left_matcher  = StereoSGBM::create(0,max_disp,wsize);

    left_matcher->setP1(24*wsize*wsize);
    left_matcher->setP2(96*wsize*wsize);
    left_matcher->setPreFilterCap(63);
    left_matcher->setMode(StereoSGBM::MODE_SGBM_3WAY);

    wls_filter = createDisparityWLSFilter(left_matcher);

    Ptr<StereoMatcher> right_matcher = createRightMatcher(left_matcher);

    left_matcher-> compute(left_for_matcher, right_for_matcher,left_disp);

    right_matcher->compute(right_for_matcher,left_for_matcher, right_disp);

    wls_filter->setLambda(lambda);
    wls_filter->setSigmaColor(sigma);
    wls_filter->filter(left_disp,left,filtered_disp,right_disp);

    cTMap * dispmap = first->GetCopy0();

    std::cout << "8b" << std::endl;
    for(int r = 0; r < first->nrRows(); r++)
    {
        for(int c = 0; c < first->nrCols(); c++)
        {
            if(r < filtered_disp.rows && c < filtered_disp.cols)
            {
                dispmap->data[r][c] = filtered_disp.at<int16_t>(r,c);
            }
        }
    }
    std::cout << 9 << std::endl;
    return dispmap;
}



inline static std::vector<cTMap *> AS_OpticalFlow(cTMap * first, cTMap * second, float windowsize)
{

    if(first->nrCols() != second->nrCols() || first->nrRows() != second->nrRows())
    {
        LISEMS_ERROR("first and second image are not of identical size");
        throw 1;
    }
    cv::Mat prvs,next;

    prvs = cv::Mat(first->nrRows(),first->nrCols(), CV_32FC1,0.0);
    next = cv::Mat(second->nrRows(),second->nrCols(), CV_32FC1,0.0);

    for(int r = 0; r < first->nrRows(); r++)
    {
        for(int c = 0; c < first->nrCols(); c++)
        {
            if(!pcr::isMV(first->data[r][c]))
            {
                prvs.at<float>(r,c) = first->data[r][c];
            }
            if(!pcr::isMV(second->data[r][c]))
            {
                next.at<unsigned char>(r,c) = second->data[r][c];
            }

        }
    }

    cv::Mat flow(prvs.size(), CV_32FC2);
    calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, windowsize, 3, 5, 1.2, 0);

    cTMap * flow_u = first->GetCopy0();
    cTMap * flow_v = first->GetCopy0();

    for(int r = 0; r < first->nrRows(); r++)
    {
        for(int c = 0; c < first->nrCols(); c++)
        {
           flow_u->data[r][c] =  flow.at<cv::Point2f>(r,c).x;
           flow_v->data[r][c] =  flow.at<cv::Point2f>(r,c).y;

        }
    }


    return {flow_u,flow_v};
}




#endif // RASTERDISPARITY_H
