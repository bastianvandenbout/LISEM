#ifndef RASTERRADAR_H
#define RASTERRADAR_H

#include "otbFrostImageFilter.h"
#include "otbLeeImageFilter.h"
#include "otbGammaMAPImageFilter.h"
#include "otbKuanImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "QString"

#include "geo/raster/map.h"


typedef otb::Image<float,2> FloatImageType;
typedef itk::ImageToImageFilter<FloatImageType, FloatImageType> SpeckleFilterType;

typedef otb::LeeImageFilter<FloatImageType, FloatImageType>      LeeFilterType;
typedef otb::FrostImageFilter<FloatImageType, FloatImageType>    FrostFilterType;
typedef otb::GammaMAPImageFilter<FloatImageType, FloatImageType> GammaMAPFilterType;
typedef otb::KuanImageFilter<FloatImageType, FloatImageType>     KuanFilterType;


inline static cTMap * AS_DespeckleFilter(cTMap * m, QString Filter, float par_1, float par_2)
{

    //get input map

    FloatImageType::Pointer imagei = FloatImageType::New();

    unsigned long long xs = m->nrCols();
    unsigned long long ys = m->nrRows();

    int cols = m->nrCols();
    int rows = m->nrRows();

    const FloatImageType::SizeType size = {{xs,ys}};
    const FloatImageType::IndexType start = {{0,0}};

    FloatImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imagei->SetRegions(region);
    FloatImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing );
    int components = 1;
    imagei->SetNumberOfComponentsPerPixel(components);
    imagei->Allocate();

    FloatImageType::PixelType * data = reinterpret_cast<FloatImageType::PixelType*>(imagei->GetBufferPointer());

    FloatImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const FloatImageType::IndexType pixelIndex = {{c,r}};

            if(pcr::isMV(m->data[r][c]))
            {
                imagei->SetPixel(pixelIndex,0.0);
            }else
            {
                imagei->SetPixel(pixelIndex,m->data[r][c]);
            }
        }
    }

    SpeckleFilterType::Pointer f;

    if(Filter == "Lee")
    {
        LeeFilterType::Pointer filter = LeeFilterType::New();

          LeeFilterType::SizeType lradius;
          lradius.Fill(par_1);

          filter->SetRadius(lradius);
          filter->SetNbLooks(par_2);

          f = filter;

    }else if(Filter == "Frost")
    {
        FrostFilterType::Pointer filter = FrostFilterType::New();

              FrostFilterType::SizeType lradius;
              lradius.Fill(par_1);

              filter->SetRadius(lradius);
              filter->SetDeramp(par_2);


              f = filter;


    }else if(Filter == "Gamma")
    {
        GammaMAPFilterType::Pointer filter = GammaMAPFilterType::New();

              GammaMAPFilterType::SizeType lradius;
              lradius.Fill(par_1);

              filter->SetRadius(lradius);
              filter->SetNbLooks(par_2);


              f = filter;


    }else if(Filter == "Kuan")
    {
         KuanFilterType::Pointer filter = KuanFilterType::New();

         KuanFilterType::SizeType lradius;
         lradius.Fill(par_1);

         filter->SetRadius(lradius);
         filter->SetNbLooks(par_2);

         f = filter;


    }else
    {
        LISEMS_ERROR("Unknown despeckle filter");
        throw 1;
    }

    f->SetInput(imagei);
    f->Update();

    f->GetOutput();

    //get output map

    cTMap * ret = m->GetCopy0();

    FloatImageType::Pointer outimage = f->GetOutput();

    FloatImageType::PixelType * data_out = (outimage->GetBufferPointer());

    for(int r = 0; r < m->nrRows(); r++)
    {
        for(int c = 0; c < m->nrCols(); c++)
        {
            if(!pcr::isMV(m->data[r][c]))
            {
                FloatImageType::PixelType  pixel = data_out[r*m->nrCols() + c];
                ret->data[r][c] = pixel;
            }else
            {
                pcr::setMV(ret->data[r][c]);
            }
        }
    }
   return ret;


    return nullptr;
}

inline static cTMap * AS_NoiseFilterFrost(cTMap *m, float radius, float deramp = 0.1)
{
    return AS_DespeckleFilter(m,"Frost",radius,deramp);
}


inline static cTMap * AS_NoiseFilterLee(cTMap *m, float radius, float nlooks = 1.0)
{
    return AS_DespeckleFilter(m,"Lee",radius,nlooks);
}

inline static cTMap * AS_NoiseFilterGamma(cTMap *m, float radius, float nlooks = 1.0)
{
    return AS_DespeckleFilter(m,"Gamma",radius,nlooks);
}

inline static cTMap * AS_NoiseFilterKuan(cTMap *m, float radius, float nlooks = 1.0)
{
    return AS_DespeckleFilter(m,"Kuan",radius,nlooks);
}

#endif // RASTERRADAR_H
