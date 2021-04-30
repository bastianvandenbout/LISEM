#ifndef RASTERPANSHARPEN_H
#define RASTERPANSHARPEN_H


#include "otbFrostImageFilter.h"
#include "otbLeeImageFilter.h"
#include "otbGammaMAPImageFilter.h"
#include "otbKuanImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "QString"

#include "otbImage.h"

#include "otbBayesianFusionFilter.h"

#include "otbSimpleRcsPanSharpeningFusionImageFilter.h"

#include "otbLmvmPanSharpeningFusionImageFilter.h"

#include "geo/raster/map.h"

#include "gdal.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "gdal_utils.h"
#include "geo/shapes/shapefile.h"
#include "geo/shapes/shapelayer.h"
#include <QString>
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include "gdal_utils.h"
#include "gdal.h"
#include "vector"
#include "geo/raster/map.h"
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "rasterwarp.h"

#include "rasterotbcommon.h"


inline static std::vector<cTMap*> PanSharpen(cTMap * m, std::vector<cTMap*> in_bands, QString method = "RCS", float par_1 = 0.9999, float par_2 = 1.0)
{

    std::vector<cTMap*> cubics;

    for(int i = 0; i < in_bands.size(); i++)
    {
        cubics.push_back(m->GetCopy0());

    }


    const unsigned int Dimension = 2;
    typedef float    PixelType;

    typedef otb::VectorImage<PixelType, Dimension> VectorImageType;
    typedef otb::Image<PixelType, Dimension>       PanchroImageType;

    typedef otb::Image<float,2> FloatImageType;
    typedef itk::ImageToImageFilter<VectorImageType, VectorImageType> BaseFilterType;

    BaseFilterType::Pointer f;

    //create vector image
    PanchroImageType::Pointer imageif = PanchroImageType::New();

    MapToOtbImage(m,imageif);

    //create input images
    VectorImageType::Pointer imagei = VectorImageType::New();



    if(method == "Bayesian")
    {

        std::vector<cTMap *> linears;

        for(int i = 0; i < in_bands.size(); i++)
        {
            linears.push_back(m->GetCopy0());

        }

        for(int i = 0; i < cubics.size(); i++)
        {
            RasterWarp(cubics.at(i),{in_bands.at(i)},"cubic");
            RasterWarp(linears.at(i),{in_bands.at(i)},"nearest");
        }


        VectorImageType::Pointer imagei2 = VectorImageType::New();

        MapToOtbVImage(cubics,imagei2);

        VectorImageType::Pointer imagei3 = VectorImageType::New();

        MapToOtbVImage(linears,imagei3);


        for(int i = 0; i < cubics.size(); i++)
        {
            //delete cubics.at(i);
            delete linears.at(i);
        }


        cubics.clear();
        linears.clear();


        typedef otb::BayesianFusionFilter<VectorImageType, VectorImageType, PanchroImageType, VectorImageType> FilterType;
        FilterType::Pointer filter = FilterType::New();



        filter->SetLambda(par_1);
        filter->SetS(par_2);
        filter->SetPanchro(imageif);
        filter->SetMultiSpect(imagei);
        filter->SetMultiSpectInterp(imagei2);

        filter->Update();


        VectorImageType::Pointer imageout = filter->GetOutput();

        OtbVImageToMap(cubics,imageout);


    }else if(method == "LMVM")
    {
        MapToOtbVImage(in_bands,imagei);

        typedef otb::LmvmPanSharpeningFusionImageFilter<PanchroImageType, VectorImageType, VectorImageType, double> FilterType;
        FilterType::Pointer filter = FilterType::New();

        PanchroImageType::SizeType radius;
        radius[0] = 5;
        radius[1] = 5;
        itk::Array<double> filterCoeffs;
        filterCoeffs.SetSize((2 * radius[0] + 1) * (2 * radius[1] + 1));
        filterCoeffs.Fill(1);
        f = filter;
        filter->SetRadius(radius);
        filter->SetFilter(filterCoeffs);


        filter->SetPanInput(imageif);
        filter->SetXsInput(imagei);


        filter->Update();

        VectorImageType::Pointer imageout = filter->GetOutput();

        OtbVImageToMap(cubics,imageout);



    }else if(method == "RCS")
    {
        typedef otb::SimpleRcsPanSharpeningFusionImageFilter<PanchroImageType, VectorImageType, VectorImageType, double> FilterType;
        FilterType::Pointer filter = FilterType::New();

        PanchroImageType::SizeType radius;
        radius[0] = 5;
        radius[1] = 5;
        itk::Array<double> filterCoeffs;
        filterCoeffs.SetSize((2 * radius[0] + 1) * (2 * radius[1] + 1));
        filterCoeffs.Fill(1);
        f = filter;
        filter->SetRadius(radius);
        filter->SetFilter(filterCoeffs);


        filter->SetPanInput(imageif);
        filter->SetXsInput(imagei);
        filter->Update();

        VectorImageType::Pointer imageout = filter->GetOutput();

        OtbVImageToMap(cubics,imageout);


    }else {

     for(int i = 0; i < cubics.size(); i++)
     {
         delete cubics.at(i);
     }
    LISEMS_ERROR("Method not recognized, available are: Bayesian, LMVM and RCS");
    throw 1;
    }


    return cubics;
}

inline std::vector<cTMap*> AS_PanSharpenRCS(cTMap * pan, std::vector<cTMap*> in)
{

    std::vector<cTMap*> ret = PanSharpen(pan,in,"RCS");

    return ret;

}

inline std::vector<cTMap*> AS_PanSharpenBayesian(cTMap * pan, std::vector<cTMap*> in, float par_1, float par_2)
{


    std::vector<cTMap*> ret = PanSharpen(pan,in,"Bayesian", par_1,par_2);

    return ret;

}


inline std::vector<cTMap*> AS_PanSharpenLMVM(cTMap * pan, std::vector<cTMap*> in)
{
    std::vector<cTMap *> ret = PanSharpen(pan,in,"LMVM");


    return ret;

}


#endif // RASTERPANSHARPEN_H
