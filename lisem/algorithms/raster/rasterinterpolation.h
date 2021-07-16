#ifndef RASTERINTERPOLATION_H
#define RASTERINTERPOLATION_H


#include "GsTL/kriging/kriging_weights.h" 
#include <GsTL/kriging/SK_constraints.h>
#include <GsTL/kriging/OK_constraints.h>
#include <GsTL/kriging/KT_constraints.h>
#include <GsTL/kriging/linear_combination.h>
#include <GsTL/kriging/cokriging_weights.h>

#include <GsTL/cdf_estimator/gaussian_cdf_Kestimator.h>
#include <GsTL/cdf/gaussian_cdf.h>

#include <GsTL/geometry/covariance.h>

#include <GsTL/kriging/kriging_constraints.h>
#include <GsTL/kriging/kriging_combiner.h>

#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>




#include "geo/raster/map.h"

typedef struct IDI_Point
{
    int r;
    int c;
    float val;

} IDI_Point;


inline cTMap * AS_InverseDistance(cTMap * mask, cTMap * points,cTMap * power)
{

    /*if(mask->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for InverseDistance (mask) can not be non-spatial");
        throw 1;
    }*/

    if((points->AS_IsSingleValue))
    {

        LISEMS_ERROR("Input 2 for InverseDistance (points) can not be non-spatial");
        throw -1;

    }

    if(!(power->AS_IsSingleValue))
    {
        if(!(power->data.nr_rows() ==  points->data.nr_rows() && power->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    std::vector<IDI_Point> pointlist;

    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(), points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");


    //
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(points->data[r][c]))
            {
                IDI_Point p;
                p.r = r;
                p.c = c;
                p.val = points->data[r][c];
                pointlist.push_back(p);
                map->data[r][c] = p.val;
            }else if(pcr::isMV(mask->data[r][c]) || mask->data[r][c] < 0.5f)
            {
                pcr::setMV(map->data[r][c]);
            }
        }
    }


    float csx = map->cellSize();

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(mask->data[r][c]) && (mask->data[r][c] > 0.5f) && pcr::isMV(points->data[r][c]))
            {
                double w_total = 0.0f;
                double val_total = 0.0f;

                for(int i = 0; i < pointlist.size(); i++)
                {
                    IDI_Point p = pointlist.at(i);
                    float dx = (r-p.r) * csx;
                    float dy = (c-p.c) * csx;

                    //std::cout << r << " " << c << "  " << dx << " " << r << "  " << p.r << " " << dx << std::endl;
                    double distancew = std::pow(dx*dx + dy*dy,-0.5f * (power->AS_IsSingleValue? power->data[0][0]:power->data[r][c]));
                    val_total += p.val * distancew;
                    w_total += distancew;

                }

                //std::cout << r << "  " << c << "  " << w_total << "  " << val_total << " " << std::endl;

                if(w_total > 0.0f)
                {
                    map->data[r][c] = val_total/w_total;
                }else {
                    map->data[r][c] = 0.0f;
                }
            }
        }
    }

    return map;
}

inline cTMap * AS_InverseDistance(cTMap * mask, cTMap * points,float power)
{
     return AS_InverseDistance(mask,points,MapFactory(power));

}



inline cTMap *  AS_ValueCorrectLinear(cTMap * ref, cTMap * vals, int distance)
{
    cTMap * ret = vals->GetCopy();
    cTMap * errs = vals->GetCopy();

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(!pcr::isMV(ref->data[r][c]))
            {
                errs->data[r][c] = ref->data[r][c]-vals->data[r][c];
            }else
            {
                pcr::setMV(errs->data[r][c]);
            }
        }
    }
    //get value errors

    cTMap * map = errs->GetCopy();

    //for each pixel
    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(pcr::isMV(ref->data[r][c]))
            {

                int cells = distance;

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r,map->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c,map->nrCols()-1);

                double val1 = 0.0f;
                double w1; pcr::setMV(w1);
                int r1 = 0;
                int c1 = 0;
                int r2 = 0;
                int c2 = 0;
                int r3 = 0;
                int c3 = 0;
                int r4 = 0;
                int c4 = 0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w1))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w1 = dist;
                                val1 = map->data[rn][cn];
                                r1 = rn;
                                c1 = cn;
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w1)
                                {
                                    w1 = dist;
                                    val1 = map->data[rn][cn];

                                    r1 = rn;
                                    c1 = cn;
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r-cells,0);
                rend = std::min(r,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val2 = 0.0f;
                double w2 = 0.0; pcr::setMV(w2);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w2))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w2 = dist;
                                val2 = map->data[rn][cn];

                                r2 = rn;
                                c2 = cn;
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w2)
                                {
                                    w2 = dist;
                                    val2 = map->data[rn][cn];
                                    r2 = rn;
                                    c2 = cn;
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val3 = 0.0f;
                double w3 = 0.0; pcr::setMV(w3);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w3))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w3 = dist;
                                val3 = map->data[rn][cn];
                                r3 = rn;
                                c3 = cn;
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w3)
                                {
                                    w3 = dist;
                                    val3 = map->data[rn][cn];
                                    r3 = rn;
                                    c3 = cn;
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c-cells,0);
                cend = std::min(c,map->nrCols()-1);

                double val4 = 0.0f;
                double w4 = 0.0; pcr::setMV(w4);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w4))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w4 = dist;
                                val4 = map->data[rn][cn];
                                r4 = rn;
                                c4 = cn;
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w4)
                                {
                                    w4 = dist;
                                    val4 = map->data[rn][cn];
                                    r4 = rn;
                                    c4 = cn;
                                }
                            }
                        }
                    }
                }

                if(pcr::isMV(w1) || pcr::isMV(w2) || pcr::isMV(w3) || pcr::isMV(w4))
                {

                    pcr::setMV(errs->data[r][c]);

                }else {
                    // 1|2
                    //-----
                    // 4|3

                    //upper x fraction
                    float frac_x1 = ((float)(c - c1))/((float)(std::max(1,c2 - c1)));
                    float frac_x2 = ((float)(c - c4))/((float)(std::max(1,c3 - c4)));

                    float y_x1 = r1  + ((float)(r2-r1)) * (frac_x1);
                    float y_x2 = r3 + ((float)(r3-r4)) * (frac_x2);

                    float frac_y = (((float)(r) - y_x1))/((float)(std::max(1.0f,y_x2-y_x1)));

                    //value

                    float val1 = map->data[r1][c1];
                    float val2 = map->data[r2][c2];
                    float val3 = map->data[r3][c3];
                    float val4 = map->data[r4][c4];

                    float val = ((val1 * (1.0-frac_x1) + val2 * (frac_x1)) * (1.0 - frac_y) + (val4 * (1.0-frac_x2) + val3 * (frac_x2)) * (frac_y));

                    errs->data[r][c] = val;
                }
                //set inverse distance estimate of error


            }
        }
    }

    //subtract the interpolated error from vals

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(!pcr::isMV(errs->data[r][c]))
            {
                ret->data[r][c] = ret->data[r][c] + errs->data[r][c];
            }
        }
    }

    delete map;
    delete errs;
    return ret;

}


inline cTMap *  AS_ValueCorrectIDSquare(cTMap * ref, cTMap * vals, int distance)
{
    cTMap * ret = vals->GetCopy();
    cTMap * errs = vals->GetCopy();

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(!pcr::isMV(ref->data[r][c]))
            {
                errs->data[r][c] = ref->data[r][c]-vals->data[r][c];
            }else
            {
                pcr::setMV(errs->data[r][c]);
            }
        }
    }
    //get value errors

    cTMap * map = errs->GetCopy();

    //for each pixel
    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(pcr::isMV(ref->data[r][c]))
            {

                int cells = distance;

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r,map->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c,map->nrCols()-1);

                double val1 = 0.0f;
                double w1; pcr::setMV(w1);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w1))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w1 = dist;
                                val1 = map->data[rn][cn];
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w1)
                                {
                                    w1 = dist;
                                    val1 = map->data[rn][cn];
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r-cells,0);
                rend = std::min(r,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val2 = 0.0f;
                double w2 = 0.0; pcr::setMV(w2);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w2))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w2 = dist;
                                val2 = map->data[rn][cn];
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w2)
                                {
                                    w2 = dist;
                                    val2 = map->data[rn][cn];
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val3 = 0.0f;
                double w3 = 0.0; pcr::setMV(w3);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w3))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w3 = dist;
                                val3 = map->data[rn][cn];
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w3)
                                {
                                    w3 = dist;
                                    val3 = map->data[rn][cn];
                                }
                            }
                        }
                    }
                }

                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c-cells,0);
                cend = std::min(c,map->nrCols()-1);

                double val4 = 0.0f;
                double w4 = 0.0; pcr::setMV(w4);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(pcr::isMV(w4))
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                w4 = dist;
                                val4 = map->data[rn][cn];
                            }else
                            {
                                float dist = std::sqrt(((float)(std::abs(c - cn) * std::abs(c - cn) + std::abs(r - rn)*std::abs(r - rn))) * map->cellSize());
                                if(dist < w4)
                                {
                                    w4 = dist;
                                    val4 = map->data[rn][cn];
                                }
                            }
                        }
                    }
                }


                //set inverse distance estimate of error

                std::vector<double>vars;
                vars.push_back(w1);
                vars.push_back(w2);
                vars.push_back(w3);
                vars.push_back(w4);

                std::vector<double>vals;
                vals.push_back(val1);
                vals.push_back(val2);
                vals.push_back(val3);
                vals.push_back(val4);

                double wtot = 0.0;

                for(int i = 0; i < vars.size(); i++)
                {
                    if(!pcr::isMV(vars.at(i)))
                    {
                        wtot = wtot + 1.0/std::max(1e-12,(vars.at(i)*vars.at(i)));
                    }
                }

                if(wtot > 0.0)
                {
                    double vtot = 0.0;
                    for(int i = 0; i < vals.size(); i++)
                    {
                        if(!pcr::isMV(vars.at(i)))
                        {
                            vtot = vtot + vals.at(i) * 1.0/std::max(1e-12,(vars.at(i)*vars.at(i)));
                        }
                    }

                    errs->data[r][c] = vtot/wtot;
                }
            }
        }
    }

    //subtract the interpolated error from vals

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(!pcr::isMV(errs->data[r][c]))
            {
                ret->data[r][c] = ret->data[r][c] + errs->data[r][c];
            }
        }
    }

    delete map;
    delete errs;
    return ret;

}






#include <opencv2/opencv.hpp>
#include "opencv/inpaint/inpainting_fsr.hpp"
// Rapid Frequency Selectice Reconstruction


static inline cTMap * AS_ResampleRFSR(cTMap *iMap, int iterations, int fftsize, int iss, int correct_dist, double rho, int bs, double ms)
{
    // read image and error pattern

    bool powerOfTwo = !(fftsize == 0) && !(fftsize & (fftsize - 1));

    if(!powerOfTwo && fftsize > 0)
    {
        LISEMS_ERROR("The size of the fast fourier domain must be a power of two (default 0 -> 32 for fast, and 128 for slow)");
        throw 1;
    }

    cTMap * Map = iMap->GetCopy();
    cTMap * ret = Map->GetCopy0();

    int offset1 = 10;
    int offset2 = 21;

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_64F,0.0);
    cv::Mat mask_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_64F,0.0);

    cv::Mat original2_ = cv::Mat(Map->nrRows()+offset1,Map->nrCols()+offset1, CV_64F,0.0);
    cv::Mat mask2_ = cv::Mat(Map->nrRows()+offset1,Map->nrCols()+offset1, CV_64F,0.0);

    cv::Mat original3_ = cv::Mat(Map->nrRows()+offset2,Map->nrCols()+offset2, CV_64F,0.0);
    cv::Mat mask3_ = cv::Mat(Map->nrRows()+offset2,Map->nrCols()+offset2, CV_64F,0.0);

    double min = 0.0;
    double max = 0.0;

    NormalizeMapInPlace(Map,min,max);


    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<double>(r,c) = Map->data[r][c];
                original2_.at<double>(r+offset1,c+offset1) = Map->data[r][c];
                original3_.at<double>(r+offset2,c+offset2) = Map->data[r][c];

                mask_.at<double>(r,c) = 255.0;
                mask2_.at<double>(r+offset1,c+offset1) = 255.0;
                mask3_.at<double>(r+offset2,c+offset2) = 255.0;
            }else
            {
                mask_.at<double>(r,c) = 0.0;
                mask2_.at<double>(r+offset1,c+offset1) = 0.0;
                mask3_.at<double>(r+offset2,c+offset2) = 0.0;
            }
        }
    }

    cv::Mat reconstructed;
    cv::xphoto::inpaint_fsr(original_, mask_, reconstructed, INPAINT_FSR_BEST,iterations,fftsize,iss, rho, bs, ms);


    //get reconstructed data into maps


    cTMap * Map1 = iMap->GetCopy();

    cTMap * Map12 = iMap->GetCopy();
    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
             Map1->data[r][c] = reconstructed.at<double>(r,c);

        }
    }


    //value correction to ensure predicted model matches actual values in sparse input

    if(correct_dist > 0)
    {
        Map12 = AS_ValueCorrectLinear(Map,Map1,correct_dist);
    }else
    {
        Map12 = Map1;
    }


    //combine based on block-size differences

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {

            double weight1 = std::min(std::min(32 - (r) % 32,(r) % 32),std::min(32 - c % 32,c % 32));
            double weight2 = std::min(std::min(32 - (((r) +offset1)%32),(((r) +offset1)%32)), std::min(32 - ((c +offset1)%32),((c +offset1)%32)));
            double weight3 = std::min(std::min(32 - (((r) +offset2)%32),(((r) +offset2)%32)), std::min(32 - ((c +offset2)%32),((c +offset2)%32)));

            //weight1 = weight1;
            //weight2 = weight2;
            //weight3 = weight3;

            double wt = std::max(1e-12,weight1 +weight2 +weight3);


            /*if(correct_dist == 0)
            {
                ret->data[r][c] = (weight1 * Map12->data[r][c] +  weight2* Map22->data[r][c]  + weight3 *Map32->data[r][c])/wt;

            }else if(correct_dist == 1)
            {
                ret->data[r][c] = weight1;
            }else if(correct_dist == 2)
            {
                ret->data[r][c] = weight2;
            }else if(correct_dist == 3)
            {
                ret->data[r][c] = weight3;
            }else {

            }*/
            ret->data[r][c] = Map12->data[r][c];//(weight1 * Map12->data[r][c]) +  weight2* Map22->data[r][c]  + weight3 *Map32->data[r][c])/wt;
        }
    }

    DeNormalizeMapInPlace(ret,min,max);

    delete Map1;
    //delete Map2;
    //delete Map3;

    if(correct_dist > 0)
    {
        delete Map12;
        //delete Map22;
        //delete Map32;
    }
    delete Map;
    return ret;


}



static inline cTMap * AS_ResampleNS(cTMap *Map,  float radius)
{
    // read image and error pattern

    cTMap * ret = Map->GetCopy0();

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);
    cv::Mat mask_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_8UC1,0.0);

    double min = 0.0;
    double max = 0.0;

    NormalizeMapInPlace(Map,min,max);


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

    cv::Mat reconstructed(original_.size(), original_.type());
    cv::inpaint(original_, mask_, reconstructed, radius,cv::INPAINT_NS );

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            ret->data[r][c] =  reconstructed.at<float>(r,c);
        }
    }

    DeNormalizeMapInPlace(ret,min,max);

    return ret;


}


static inline cTMap * AS_ResampleTELEA(cTMap *Map, float radius)
{
    // read image and error pattern

    cTMap * ret = Map->GetCopy0();

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);
    cv::Mat mask_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_8UC1,0.0);

    double min = 0.0;
    double max = 0.0;

    //NormalizeMapInPlace(Map,min,max);


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

    cv::Mat reconstructed(original_.size(), original_.type());
    cv::inpaint(original_, mask_, reconstructed, radius,cv::INPAINT_TELEA );

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            ret->data[r][c] = reconstructed.at<float>(r,c);
        }
    }

    //DeNormalizeMapInPlace(ret,min,max);

    return ret;


}



//sparse upscaling


inline static cTMap * AS_ReSampleNearest(cTMap * m , float scale)
{

    if(!(scale > 0.0))
    {
        LISEMS_ERROR("Scale for resampling must be greater than 0.0");
        throw 1;
    }

    int colsn = std::max(1,(int)(scale * m->nrCols()));
    int rowsn = std::max(1,(int)(scale * m->nrRows()));

    cTMap * dup = m;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(rowsn, colsn, dup->north(),
        dup->west(), dup->cellSizeX() *(((double)(m->nrCols()))/(colsn)) ,dup->cellSizeY()*(((double)(m->nrRows()))/(rowsn)));

    cTMap * ret = new cTMap(std::move(datac),m->projection(),"",false);

    if(scale > 1.0) // linear interpolation, sparse
    {

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {
                ret->data[r][c] = m->Sample(ret->west() + ((double)(c)) * ret->cellSizeX(),ret->north() + ((double)(r)) * ret->cellSizeY());
            }
        }

    }else // averaging
    {

        cTMap * w = ret->GetCopy0();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {

                int indexr = (((double)(r)) * m->cellSizeX())/(ret->cellSizeX());
                int indexc = (((double)(c)) * m->cellSizeY())/(ret->cellSizeY());

                if(indexr > -1 && indexr < ret->nrRows() && indexc > -1 && indexc < ret->nrCols())
                {
                    ret->data[indexr][indexc] += m->data[r][c];
                    w->data[indexr][indexc] += 1.0;
                }
            }
        }

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {

                if(w->data[r][c] > 0.0)
                {
                    ret->data[r][c] = ret->data[r][c]/w->data[r][c];
                }else
                {
                    pcr::setMV(ret->data[r][c]);
                }
            }
        }

        delete w;
    }

    return ret;

}



inline static cTMap * AS_ReSampleLinear(cTMap * m , float scale)
{

    if(!(scale > 0.0))
    {
        LISEMS_ERROR("Scale for resampling must be greater than 0.0");
        throw 1;
    }

    int colsn = std::max(1,(int)(scale * m->nrCols()));
    int rowsn = std::max(1,(int)(scale * m->nrRows()));

    cTMap * dup = m;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(rowsn, colsn, dup->north(),
        dup->west(), dup->cellSizeX() *(((double)(m->nrCols()))/(colsn)) ,dup->cellSizeY()*(((double)(m->nrRows()))/(rowsn)));

    cTMap * ret = new cTMap(std::move(datac),m->projection(),"",false);

    if(scale > 1.0) // linear interpolation, sparse
    {

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {
                ret->data[r][c] = m->SampleLinear(ret->west() + ((double)(c)) * ret->cellSizeX(),ret->north() + ((double)(r)) * ret->cellSizeY());
            }
        }

    }else // averaging
    {

        cTMap * w = ret->GetCopy0();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {

                int indexr = (((double)(r)) * m->cellSizeX())/(ret->cellSizeX());
                int indexc = (((double)(c)) * m->cellSizeY())/(ret->cellSizeY());

                if(indexr > -1 && indexr < ret->nrRows() && indexc > -1 && indexc < ret->nrCols())
                {
                    ret->data[indexr][indexc] += m->data[r][c];
                    w->data[indexr][indexc] += 1.0;
                }
            }
        }

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {

                if(w->data[r][c] > 0.0)
                {
                    ret->data[r][c] = ret->data[r][c]/w->data[r][c];
                }else
                {
                    pcr::setMV(ret->data[r][c]);
                }
            }
        }

        delete w;
    }

    return ret;

}

inline static cTMap * AS_ReSampleCubic(cTMap * m , float scale)
{

    if(!(scale > 0.0))
    {
        LISEMS_ERROR("Scale for resampling must be greater than 0.0");
        throw 1;
    }

    int colsn = std::max(1,(int)(scale * m->nrCols()));
    int rowsn = std::max(1,(int)(scale * m->nrRows()));

    cTMap * dup = m;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(rowsn, colsn, dup->north(),
        dup->west(), dup->cellSizeX() *(((double)(m->nrCols()))/(colsn)) ,dup->cellSizeY()*(((double)(m->nrRows()))/(rowsn)));

    cTMap * ret = new cTMap(std::move(datac),m->projection(),"",false);

    if(scale > 1.0) // linear interpolation, sparse
    {

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {
                ret->data[r][c] = m->SampleCubic(ret->west() + ((double)(c)) * ret->cellSizeX(),ret->north() + ((double)(r)) * ret->cellSizeY());
            }
        }

    }else // averaging
    {

        cTMap * w = ret->GetCopy0();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {

                int indexr = (((double)(r)) * m->cellSizeX())/(ret->cellSizeX());
                int indexc = (((double)(c)) * m->cellSizeY())/(ret->cellSizeY());

                if(indexr > -1 && indexr < ret->nrRows() && indexc > -1 && indexc < ret->nrCols())
                {
                    ret->data[indexr][indexc] += m->data[r][c];
                    w->data[indexr][indexc] += 1.0;
                }
            }
        }

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {

                if(w->data[r][c] > 0.0)
                {
                    ret->data[r][c] = ret->data[r][c]/w->data[r][c];
                }else
                {
                    pcr::setMV(ret->data[r][c]);
                }
            }
        }

        delete w;
    }

    return ret;

}

inline static cTMap * AS_ReSampleSparse(cTMap * m , float scale)
{
    if(!(scale > 0.0))
    {
        LISEMS_ERROR("Scale for resampling must be greater than 0.0");
        throw 1;
    }

    int colsn = std::max(1,(int)(scale * m->nrCols()));
    int rowsn = std::max(1,(int)(scale * m->nrRows()));

    cTMap * dup = m;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(rowsn, colsn, dup->north(),
        dup->west(), dup->cellSizeX() *(((double)(m->nrCols()))/(colsn)) ,dup->cellSizeY()*(((double)(m->nrRows()))/(rowsn)));

    cTMap * ret = new cTMap(std::move(datac),m->projection(),"",false);

    if(scale > 1.0) // linear interpolation, sparse
    {

        ret->setAllMV();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {

                int indexr = (((double)(r)) * m->cellSizeX())/(ret->cellSizeX());
                int indexc = (((double)(c)) * m->cellSizeY())/(ret->cellSizeY());

                if(indexr > -1 && indexr < ret->nrRows() && indexc > -1 && indexc < ret->nrCols())
                {
                    ret->data[indexr][indexc] = m->data[r][c];

                }
            }
        }

    }else // averaging
    {

        cTMap * w = ret->GetCopy0();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {

                int indexr = (((double)(r)) * m->cellSizeX())/(ret->cellSizeX());
                int indexc = (((double)(c)) * m->cellSizeY())/(ret->cellSizeY());

                if(indexr > -1 && indexr < ret->nrRows() && indexc > -1 && indexc < ret->nrCols())
                {
                    ret->data[indexr][indexc] += m->data[r][c];
                    w->data[indexr][indexc] += 1.0;
                }
            }
        }

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {

                if(w->data[r][c] > 0.0)
                {
                    ret->data[r][c] = ret->data[r][c]/w->data[r][c];
                }else
                {
                    pcr::setMV(ret->data[r][c]);
                }
            }
        }

        delete w;
    }

    return ret;




}






#endif // RASTERINTERPOLATION_H
