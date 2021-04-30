#ifndef RASTEROTBCOMMON_H
#define RASTEROTBCOMMON_H

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

inline static void MapMaskToOtbImage(cTMap * m,otb::Image<float,2>::Pointer &imageif)
{

    typedef otb::Image<float,2> FloatImageType;

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    const FloatImageType::SizeType size = {{xs,ys}};
    const FloatImageType::IndexType start = {{0,0}};

    FloatImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imageif->SetRegions(region);
    FloatImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imageif->SetSpacing( spacing );
    int components = 1;
    imageif->SetNumberOfComponentsPerPixel(components);
    imageif->Allocate();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imageif->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const FloatImageType::IndexType pixelIndex = {{c,r}};

            if(pcr::isMV(m->data[r][c]))
            {
                imageif->SetPixel(pixelIndex,0.0);
            }else
            {
                imageif->SetPixel(pixelIndex,1.0);
            }
        }
    }
}


inline static void MapToOtbImage(cTMap * m,otb::Image<float,2>::Pointer &imageif)
{

    typedef otb::Image<float,2> FloatImageType;

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    const FloatImageType::SizeType size = {{xs,ys}};
    const FloatImageType::IndexType start = {{0,0}};

    FloatImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imageif->SetRegions(region);
    FloatImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imageif->SetSpacing( spacing );
    int components = 1;
    imageif->SetNumberOfComponentsPerPixel(components);
    imageif->Allocate();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imageif->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const FloatImageType::IndexType pixelIndex = {{c,r}};

            if(pcr::isMV(m->data[r][c]))
            {
                imageif->SetPixel(pixelIndex,0.0);
            }else
            {
                imageif->SetPixel(pixelIndex,m->data[r][c]);
            }
        }
    }
}


inline static void MapToOtbImageUI(cTMap * m,otb::Image<unsigned int,2>::Pointer &imageif)
{

    typedef otb::Image<unsigned int,2> FloatImageType;

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    const FloatImageType::SizeType size = {{xs,ys}};
    const FloatImageType::IndexType start = {{0,0}};

    FloatImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imageif->SetRegions(region);
    FloatImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imageif->SetSpacing( spacing );
    int components = 1;
    imageif->SetNumberOfComponentsPerPixel(components);
    imageif->Allocate();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imageif->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const FloatImageType::IndexType pixelIndex = {{c,r}};

            if(pcr::isMV(m->data[r][c]))
            {
                imageif->SetPixel(pixelIndex,0.0);
            }else
            {
                imageif->SetPixel(pixelIndex,((unsigned int)(std::max(0.0,m->data[r][c] + 0.5))));
            }
        }
    }
}

inline static void OtbImageToMap(cTMap * m,otb::Image<float,2>::Pointer &imageif)
{

    typedef otb::Image<float,2> FloatImageType;

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imageif->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {

            if(!pcr::isMV(m->data[r][c]))
            {
                m->data[r][c] = data[r*cols + c];
            }
        }
    }
}

inline static void OtbImageUIToMap(cTMap * m,otb::Image<unsigned int,2>::Pointer &imageif)
{

    typedef otb::Image<unsigned int,2> FloatImageType;

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imageif->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {

            if(!pcr::isMV(m->data[r][c]))
            {
                m->data[r][c] = data[r*cols + c];
            }
        }
    }
}
inline static void MapToOtbCImage(std::vector<cTMap*> in_bands,otb::Image<std::complex<float>, 2>::Pointer &imagei)
{
    typedef std::complex<float> PixelType;
    typedef otb::Image<std::complex<float>, 2> VectorImageType;


    unsigned long long xs2 = in_bands.at(0)->nrCols();
    unsigned long long ys2 = in_bands.at(0)->nrRows();

    int cols2 =in_bands.at(0)->nrCols();
    int rows2 = in_bands.at(0)->nrRows();

    const VectorImageType::SizeType size2 = {{xs2,ys2}};
    const VectorImageType::IndexType start2 = {{0,0}};

    VectorImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    imagei->SetRegions(region2);
    VectorImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing2 );
    int components2 = in_bands.size();
    imagei->SetNumberOfComponentsPerPixel(components2);
    imagei->Allocate();

    for(int r = 0; r < rows2; r++)
    {
        for(int c = 0; c < cols2; c++)
        {
            const VectorImageType::IndexType pixelIndex = {{c,r}};
            VectorImageType::PixelType val;

            if(!pcr::isMV(in_bands.at(0)->data[r][c]) && !pcr::isMV(in_bands.at(1)->data[r][c]))
            {
                val = std::complex<float>(in_bands.at(0)->data[r][c],in_bands.at(1)->data[r][c]);
            }

            imagei->SetPixel(pixelIndex,val);
        }
    }
}


inline static void MapToOtbVImage(std::vector<cTMap*> in_bands,otb::VectorImage<float, 2>::Pointer &imagei)
{
    typedef float PixelType;
    typedef otb::VectorImage<PixelType, 2> VectorImageType;


    unsigned long long xs2 = in_bands.at(0)->nrCols();
    unsigned long long ys2 = in_bands.at(0)->nrRows();

    int cols2 =in_bands.at(0)->nrCols();
    int rows2 = in_bands.at(0)->nrRows();

    const VectorImageType::SizeType size2 = {{xs2,ys2}};
    const VectorImageType::IndexType start2 = {{0,0}};

    VectorImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    imagei->SetRegions(region2);
    VectorImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing2 );
    int components2 = in_bands.size();
    imagei->SetNumberOfComponentsPerPixel(components2);
    imagei->Allocate();

    for(int r = 0; r < rows2; r++)
    {
        for(int c = 0; c < cols2; c++)
        {
            const VectorImageType::IndexType pixelIndex = {{c,r}};
            VectorImageType::PixelType val;
            val.SetSize(components2);
            for(int i = 0; i < components2; i++)
            {
                if(!pcr::isMV(in_bands.at(i)->data[r][c]))
                {
                    val.SetElement(i,in_bands.at(i)->data[r][c]);
                }
            }
            imagei->SetPixel(pixelIndex,val);
        }
    }
}


inline static void MapToOtbVDImage(std::vector<cTMap*> in_bands,otb::VectorImage<double, 2>::Pointer &imagei)
{
    typedef double PixelType;
    typedef otb::VectorImage<PixelType, 2> VectorImageType;


    unsigned long long xs2 = in_bands.at(0)->nrCols();
    unsigned long long ys2 = in_bands.at(0)->nrRows();

    int cols2 =in_bands.at(0)->nrCols();
    int rows2 = in_bands.at(0)->nrRows();

    const VectorImageType::SizeType size2 = {{xs2,ys2}};
    const VectorImageType::IndexType start2 = {{0,0}};

    VectorImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    imagei->SetRegions(region2);
    VectorImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing2 );
    int components2 = in_bands.size();
    imagei->SetNumberOfComponentsPerPixel(components2);
    imagei->Allocate();

    for(int r = 0; r < rows2; r++)
    {
        for(int c = 0; c < cols2; c++)
        {
            const VectorImageType::IndexType pixelIndex = {{c,r}};
            VectorImageType::PixelType val;
            val.SetSize(components2);
            for(int i = 0; i < components2; i++)
            {
                if(!pcr::isMV(in_bands.at(i)->data[r][c]))
                {
                    val.SetElement(i,in_bands.at(i)->data[r][c]);
                }
            }
            imagei->SetPixel(pixelIndex,val);
        }
    }
}



inline static void OtbVImageToMap(std::vector<cTMap*> in_bands,otb::VectorImage<float, 2>::Pointer &imagei)
{
    typedef float PixelType;
    typedef otb::VectorImage<PixelType, 2> VectorImageType;

    VectorImageType::PixelType * data_out = reinterpret_cast<VectorImageType::PixelType*>(imagei->GetBufferPointer());

    cTMap * ret = in_bands.at(0);
    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            VectorImageType::PixelType pixel = data_out[r*ret->nrCols() + c];
            for(int i = 0; i < pixel.Size(); i++)
            {
                in_bands.at(i)->data[r][c] = pixel.GetElement(i);
            }

        }
    }
}

inline static void OtbVDImageToMap(std::vector<cTMap*> in_bands,otb::VectorImage<double, 2>::Pointer &imagei)
{
    typedef double PixelType;
    typedef otb::VectorImage<PixelType, 2> VectorImageType;

    VectorImageType::PixelType * data_out = reinterpret_cast<VectorImageType::PixelType*>(imagei->GetBufferPointer());

    cTMap * ret = in_bands.at(0);
    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            VectorImageType::PixelType pixel = data_out[r*ret->nrCols() + c];
            for(int i = 0; i < pixel.Size(); i++)
            {
                in_bands.at(i)->data[r][c] = pixel.GetElement(i);
            }

        }
    }
}

#endif // RASTEROTBCOMMON_H
