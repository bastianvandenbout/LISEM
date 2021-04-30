#ifndef RASTERDECONVOLVE_H
#define RASTERDECONVOLVE_H

//https://onsignalandimageprocessing.blogspot.com/2017/02/lucy-richardson-deconvolution.html

#include "opencv2/opencv.hpp"
#include "geo/raster/map.h"



inline static void lucy_richardson_deconv(cv::Mat img, int num_iterations, double sigmaG, cv::Mat& result)
{

        double EPSILON=2.2204e-16;

        // Lucy-Richardson Deconvolution Function
        // input-1 img: NxM matrix image
        // input-2 num_iterations: number of iterations
        // input-3 sigma: sigma of point spread function (PSF)
        // output result: deconvolution result

        // Window size of PSF
        int winSize = 8 * sigmaG + 1 ;

        // Initializations
        cv::Mat Y = img.clone();
        cv::Mat J1 = img.clone();
        cv::Mat J2 = img.clone();
        cv::Mat wI = img.clone();
        cv::Mat imR = img.clone();
        cv::Mat reBlurred = img.clone();

        cv::Mat T1, T2, tmpMat1, tmpMat2;
        T1 = cv::Mat(img.rows,img.cols, CV_64F, 0.0);
        T2 = cv::Mat(img.rows,img.cols, CV_64F, 0.0);

        // Lucy-Rich. Deconvolution CORE

        double lambda = 0;
        for(int j = 0; j < num_iterations; j++)
        {
                if (j>1) {
                        // calculation of lambda
                        multiply(T1, T2, tmpMat1);
                        multiply(T2, T2, tmpMat2);
                        lambda=sum(tmpMat1)[0] / (sum( tmpMat2)[0]+EPSILON);
                        // calculation of lambda
                }

                Y = J1 + lambda * (J1-J2);
                Y.setTo(0, Y < 0);

                // 1)
                GaussianBlur( Y, reBlurred, cv::Size(winSize,winSize), sigmaG, sigmaG );//applying Gaussian filter
                reBlurred.setTo(EPSILON , reBlurred <= 0);

                // 2)
                divide(wI, reBlurred, imR);
                imR = imR + EPSILON;

                // 3)
                GaussianBlur( imR, imR, cv::Size(winSize,winSize), sigmaG, sigmaG );//applying Gaussian filter

                // 4)
                J2 = J1.clone();
                multiply(Y, imR, J1);

                T2 = T1.clone();
                T1 = J1 - Y;
        }

        // output
        result = J1.clone();

}


inline static cTMap * AS_RasterDeconvolve(cTMap * Mapi, int iter, double sigma)
{

    cTMap * Map = Mapi->GetCopy();
    double min,max;
    NormalizeMapInPlace(Map,min,max);

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_64F,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<double>(r,c) = Map->data[r][c];
            }else
            {
                original_.at<double>(r,c) = 0.0;
            }
        }
    }




    cv::Mat result_;
    lucy_richardson_deconv(original_,iter,sigma,result_);

    cTMap * res = Map->GetCopy0();


    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            res->data[r][c] = result_.at<double>(r,c);
        }
    }

    DeNormalizeMapInPlace(res,min,max);

    return res;

}

/*int main(){

        Mat img, result;

        // read image
        img = imread("lena_thin.png",IMREAD_GRAYSCALE );

        // convert to double
        img.convertTo(img, CV_64F);

        // sigmaG: sigma of point spread function
        double sigmaG = 6.0;
        int winSize = 8 * sigmaG + 1 ;

        // Blur the original image
        GaussianBlur( img, img, Size(winSize,winSize), sigmaG, sigmaG ); //blurring original image

        normalize(img, img, 0, 1, NORM_MINMAX);

        imshow( "Original", img );

        // num_iterations: number of iterations
        int num_iterations = 100;

        // apply Lucy-Richardson Deconvolution
        lucy_richardson_deconv(img, num_iterations, sigmaG, result);

        normalize(result, result, 0, 1, NORM_MINMAX);

        // Result
        imshow( "Result", result );

        waitKey(0);

        return 0;
}*/




inline static cTMap * AS_DistanceTransform(cTMap * Map)
{

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_8U,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<uchar>(r,c) = (!(Map->data[r][c] < 0.5))?1:0;
            }else
            {
                original_.at<uchar>(r,c) = 0.0;
            }
        }
    }


    cv::Mat result_;
    distanceTransform(original_,result_,cv::DIST_L2,5);

    cTMap * res = Map->GetCopy0();


    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            res->data[r][c] = result_.at<float>(r,c);
        }
    }

    return res;

}


inline static cTMap * AS_BilateralFilter(cTMap * Map, int size,  float sigma_space, float sigma_val)
{

    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<float>(r,c) = Map->data[r][c];
            }else
            {
                original_.at<float>(r,c) = 0.0;
            }
        }
    }


    cv::Mat result_;
    bilateralFilter(original_,result_,size,sigma_val,sigma_space);

    cTMap * res = Map->GetCopy0();


    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            res->data[r][c] = result_.at<float>(r,c);
        }
    }

    return res;

}

#include "sisr/SuperResolution.h"



inline static cTMap * AS_SuperResolutionSISR(cTMap * Mapi, float upscale = 2.0, int iterations = 1, int tilesizei = 512, int patchSize = 4, int patchOverlap = 3, double lambda = 1e-12, int neighborhoodSize = 200, double neighborhoodWeight = 1.0, bool wavelet = false)
{
    double tilesize = tilesizei;
    int tilex = std::ceil(Mapi->nrCols()/tilesize);
    int tiley = std::ceil(Mapi->nrRows()/tilesize);

    //for each tile

    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(tilesize,tilesize,0.0,0.0,1.0);
    cTMap * Map = new cTMap(std::move(datac),Mapi->projection(),"",false);

    MaskedRaster<float> raster_data(Mapi->nrRows() * upscale,Mapi->nrCols() * upscale, Mapi->data.north(), Mapi->data.west(), Mapi->data.cell_size()/upscale,Mapi->data.cell_sizeY()/upscale);
    cTMap *resall = new cTMap(std::move(raster_data),Mapi->projection(),"");


    for(int tx = 0; tx < tilex; tx++)
    {
        for(int ty = 0; ty < tiley; ty++)
        {

            int minr = ty * tilesizei;
            int maxr = std::min(minr + tilesizei,Mapi->nrRows());
            int minc = tx* tilesize;
            int maxc = std::min(minc + tilesizei,Mapi->nrCols());


            for(int r = 0; r < tilesizei; r++)
            {
                for(int c = 0; c < tilesizei; c++)
                {
                    if((minc + c < maxc) && (minr +r < maxr))
                    {

                        Map->data[r][c] = Mapi->data[minr+r][minc+c];
                    }else {
                        Map->data[r][c] = 0.0;
                    }
                }
            }


            cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);

            for(int r = 0; r < Map->nrRows(); r++)
            {
                for(int c = 0; c < Map->nrCols(); c++)
                {
                    if(!pcr::isMV(Map->data[r][c]))
                    {
                        original_.at<float>(r,c) = Map->data[r][c];
                    }else
                    {
                        original_.at<float>(r,c) = 0.0;
                    }
                }
            }


            cv::Mat* imageSR;

            int waveletP = 7;
            int waveletQ = 8;

            std::cout << "Has res0" << std::endl;

            try
            {
                if(wavelet)
                {
                   imageSR = gs::superResolve(original_, upscale, iterations, lambda, patchSize, patchOverlap, neighborhoodSize, neighborhoodWeight);
                }else {
                   neighborhoodWeight *= 4.0;
                   imageSR = gs::superResolveWavelet(original_, upscale, iterations, lambda, patchSize, patchOverlap, neighborhoodSize, neighborhoodWeight, waveletP, waveletQ);
                }
            }catch(...)
            {
                LISEMS_ERROR("Empty tile in map");
            }

            for(int r = 0; r < imageSR->rows; r++)
            {
                for(int c = 0; c < imageSR->cols; c++)
                {
                    int rn =r + ty *  tilesizei * upscale;
                    int cn =c + tx *  tilesizei * upscale;

                    if(rn < resall->nrRows() && cn < resall->nrCols())
                    {
                        resall->data[rn][cn] = imageSR->at<float>(r,c);

                    }
                }
            }


        }
    }

    delete Map;
    return resall;

}





#endif // RASTERDECONVOLVE_H
