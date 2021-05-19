#ifndef RASTERSEGMENTIZE_H
#define RASTERSEGMENTIZE_H


#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbImageFileWriter.h"
#include "otbPrintableImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "otbMeanShiftSegmentationFilter.h"
#include "otbWatershedSegmentationFilter.h"
#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"
#include "geo/raster/map.h"
#include "itkWatershedImageFilter.h"
#include "itkVectorGradientMagnitudeImageFilter.h"
#include "raster/rastercommon.h"
#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"

inline static cTMap * RasterSegmentize(cTMap * in, float in_spatialradius, float in_rangeradius, float in_minregionsize, float in_maxiter, float in_thres, QString method = "MeanShift")
{


    cTMap * ret = in->GetCopy();

    const unsigned int Dimension = 2;


    const unsigned int spatialRadius   = in_spatialradius;
    const double       rangeRadius     = in_rangeradius;
    const unsigned int minRegionSize   = in_minregionsize;
    const unsigned int maxiter         = in_maxiter;
    const double       thres           = in_thres;

    using PixelType      = float;
    using LabelPixelType = unsigned int;

    using ImageType      = otb::VectorImage<PixelType, Dimension>;
    using LabelImageType = otb::Image<LabelPixelType, Dimension>;

    using FilterType = otb::MeanShiftSegmentationFilter<ImageType, LabelImageType, ImageType>;



    FilterType::Pointer      filter  = FilterType::New();
    filter->SetSpatialBandwidth(spatialRadius);
    filter->SetRangeBandwidth(rangeRadius);
    filter->SetMinRegionSize(minRegionSize);

    filter->SetMaxIterationNumber(maxiter);
    filter->SetThreshold(thres);


    ImageType::Pointer image = ImageType::New();
    unsigned long long xs = in->nrCols();
    unsigned long long ys = in->nrRows();
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

    image->SetNumberOfComponentsPerPixel(1);


    image->Allocate();


    PixelType * data = reinterpret_cast<PixelType*>(image->GetBufferPointer());

    memcpy(data,in->data[0],sizeof(PixelType) * size[0] * size[1]); // PixelType == FLOAT so we can copy to cTMap type


    filter->SetInput(image);

    filter->Update();

    LabelImageType::Pointer clusteredimage = filter->GetLabelOutput();
    LabelPixelType * data_out = reinterpret_cast<LabelPixelType*>(clusteredimage->GetBufferPointer());

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
        }
    }

    return ret;
}

inline static cTMap * RasterSegmentize(std::vector<cTMap*> in, float in_spatialradius, float in_rangeradius, float in_minregionsize, float in_maxiter, float in_thres, QString method = "MeanShift")
{
    cTMap * ret;

    if(in.size() > 0)
    {
       ret = in.at(0)->GetCopy();
    }else
    {
        LISEMS_ERROR("Error during segmentation algorithm, no maps as input");
        throw 1;
    }

    const unsigned int Dimension = 2;


    const unsigned int spatialRadius   = in_spatialradius;
    const double       rangeRadius     = in_rangeradius;
    const unsigned int minRegionSize   = in_minregionsize;
    const unsigned int maxiter         = in_maxiter;
    const double       thres           = in_thres;

    using PixelType      = float;
    using LabelPixelType = unsigned int;

    using ImageType      = otb::VectorImage<PixelType, Dimension>;
    using LabelImageType = otb::Image<LabelPixelType, Dimension>;


    using FilterType = otb::MeanShiftSegmentationFilter<ImageType, LabelImageType, ImageType>;


    FilterType::Pointer      filter  = FilterType::New();
    filter->SetSpatialBandwidth(spatialRadius);
    filter->SetRangeBandwidth(rangeRadius);
    filter->SetMinRegionSize(minRegionSize);

    filter->SetMaxIterationNumber(maxiter);
    filter->SetThreshold(thres);




    ImageType::Pointer image = ImageType::New();
    unsigned long long xs = in.at(0)->nrCols();
    unsigned long long ys = in.at(0)->nrRows();
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


    int components = in.size();


    image->SetNumberOfComponentsPerPixel(components);


    image->Allocate();


    PixelType * data = reinterpret_cast<PixelType*>(image->GetBufferPointer());

    int cols =ret->nrCols();
    int rows = ret->nrRows();

    ImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const ImageType::IndexType pixelIndex = {{c,r}};
            ImageType::PixelType val;
            val.SetSize(components);

            for(int i = 0; i < std::min((unsigned int)components,val.Size()); i++)
            {
                val.SetElement(i,in.at(i)->data[r][c]);
            }

            image->SetPixel(pixelIndex,val);
        }
    }
    //memcpy(data,in->data[0],sizeof(PixelType) * size[0] * size[1]); // PixelType == FLOAT so we can copy to cTMap type


    filter->SetInput(image);

    filter->Update();

    LabelImageType::Pointer clusteredimage = filter->GetLabelOutput();

    LabelPixelType * data_out = reinterpret_cast<LabelPixelType*>(clusteredimage->GetBufferPointer());

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
        }
    }

    return ret;
}

inline static cTMap * AS_RasterSegmentize(cTMap * in, float in_spatialradius, float in_rangeradius, float in_minregionsize, float in_maxiter, float in_thres)
{
    return RasterSegmentize(in,in_spatialradius,in_rangeradius,in_minregionsize,in_maxiter, in_thres);

}

inline static cTMap * AS_RasterSegmentize(std::vector<cTMap*> in, float in_spatialradius, float in_rangeradius, float in_minregionsize, float in_maxiter, float in_thres)
{

    return RasterSegmentize(in,in_spatialradius,in_rangeradius,in_minregionsize,in_maxiter, in_thres);

}





















inline static cTMap * RasterSegmentizeWS(cTMap * in, float thres, float lev)
{


    cTMap * ret = in->GetCopy();

    const unsigned int Dimension = 2;



    using PixelType      = float;
    using ImageType      = otb::Image<PixelType, Dimension>;

    using FilterType = itk::WatershedImageFilter<ImageType>;//otb::WatershedSegmentationFilter<ImageType, LabelImageType>;

    using LabelPixelType = FilterType::OutputImageType::PixelType;

    FilterType::Pointer      filter  = FilterType::New();
    filter->SetLevel(lev);

    filter->SetThreshold(thres);




    ImageType::Pointer image = ImageType::New();
    unsigned long long xs = in->nrCols();
    unsigned long long ys = in->nrRows();
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

    image->SetNumberOfComponentsPerPixel(1);


    image->Allocate();


    PixelType * data = reinterpret_cast<PixelType*>(image->GetBufferPointer());

    memcpy(data,in->data[0],sizeof(PixelType) * size[0] * size[1]); // PixelType == FLOAT so we can copy to cTMap type


    filter->SetInput(image);

    filter->Update();

    itk::Image<LabelPixelType,2> * clusteredimage = filter->GetOutput();
    LabelPixelType * data_out = reinterpret_cast<LabelPixelType*>(clusteredimage->GetBufferPointer());

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
        }
    }

    return ret;
}

inline static cTMap * AS_RasterSegmentizeWS(cTMap * in, float threshold, float level)
{
    return RasterSegmentizeWS(in,threshold,level);

}














inline static cTMap * RasterGradientMagnitude(std::vector<cTMap*> in)
{
    cTMap * ret;

    if(in.size() > 0)
    {
       ret = in.at(0)->GetCopy();
    }else
    {
        LISEMS_ERROR("Error during segmentation algorithm, no maps as input");
        throw 1;
    }

    float dx = in.at(0)->cellSizeX();
    float dy = in.at(0)->cellSizeY();


    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(pcr::isMV(ret->data[r][c]))
            {
            }else
            {
                float slope = 0.0;
                for(int i = 0; i < in.size(); i++)
                {
                    //calculate slope based on neighbour cell values
                    //use a 3x3 kernel to estimate average slope

                    float x_11 = GetMapValue_OUTORMV3x3Av(in.at(i),r-1,c-1);
                    float x_12 = GetMapValue_OUTORMV3x3Av(in.at(i),r-1,c);
                    float x_13 = GetMapValue_OUTORMV3x3Av(in.at(i),r-1,c+1);
                    float x_21 = GetMapValue_OUTORMV3x3Av(in.at(i),r,c-1);
                    float x_22 = GetMapValue_OUTORMV3x3Av(in.at(i),r,c);
                    float x_23 = GetMapValue_OUTORMV3x3Av(in.at(i),r,c+1);
                    float x_31 = GetMapValue_OUTORMV3x3Av(in.at(i),r+1,c-1);
                    float x_32 = GetMapValue_OUTORMV3x3Av(in.at(i),r+1,c);
                    float x_33 = GetMapValue_OUTORMV3x3Av(in.at(i),r+1,c+1);

                    float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                    float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dy);

                    slope += std::sqrt(dzdx*dzdx + dzdy*dzdy);
                }
                ret->data[r][c] = slope;

            }
        }
    }

    return ret;
}


inline static cTMap * AS_RasterGradientMagnitude(std::vector<cTMap*> in)
{

    return RasterGradientMagnitude(in);

}



inline static cTMap * RasterMagnitude(std::vector<cTMap*> in)
{
    cTMap * ret;

    if(in.size() > 0)
    {
       ret = in.at(0)->GetCopy();
    }else
    {
        LISEMS_ERROR("Error during segmentation algorithm, no maps as input");
        throw 1;
    }

    float dx = in.at(0)->cellSizeX();
    float dy = in.at(0)->cellSizeY();


    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(pcr::isMV(ret->data[r][c]))
            {
            }else
            {
                float slope = 0.0;
                for(int i = 0; i < in.size(); i++)
                {
                    slope += in.at(i)->data[r][c];
                }
                ret->data[r][c] = slope;

            }
        }
    }

    return ret;
}


inline static cTMap * AS_RasterMagnitude(std::vector<cTMap*> in)
{

    return RasterMagnitude(in);

}






template<int vlength>
inline static std::vector<cTMap*> RasterAnisotropicDiffusion(std::vector<cTMap*> in, int iter, float conductance, float timestep)
{
    //vlength must be equal to in.length()

    constexpr const int vl = vlength;

    if(vl == 1)
    {

        cTMap * ret = in.at(0)->GetCopy();

        const unsigned int Dimension = 2;

        using PixelType      = float;
        using LabelPixelType = float;

        using ImageType      = otb::Image<PixelType, Dimension>;
        using LabelImageType = otb::Image<LabelPixelType, Dimension>;

        using FilterType = itk::GradientAnisotropicDiffusionImageFilter<ImageType,ImageType>;

        FilterType::Pointer      filter  = FilterType::New();
        filter->SetConductanceParameter(conductance);//0.2);
        filter->SetTimeStep(timestep);//0.125);
        filter->SetNumberOfIterations(iter);//100);


        ImageType::Pointer image = ImageType::New();
        unsigned long long xs = in.at(0)->nrCols();
        unsigned long long ys = in.at(0)->nrRows();
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

        image->SetNumberOfComponentsPerPixel(1);


        image->Allocate();


        PixelType * data = reinterpret_cast<PixelType*>(image->GetBufferPointer());

        memcpy(data,in.at(0)->data[0],sizeof(PixelType) * size[0] * size[1]); // PixelType == FLOAT so we can copy to cTMap type


        filter->SetInput(image);

        filter->Update();

        LabelImageType::Pointer clusteredimage = filter->GetOutput();
        LabelPixelType * data_out = reinterpret_cast<LabelPixelType*>(clusteredimage->GetBufferPointer());

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {
                ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
            }
        }


        std::vector<cTMap*> retm;
        retm.push_back(ret);
        return retm;

    }else
    {
        cTMap * ret;

        std::vector<cTMap*> retm;


        if(in.size() > 0)
        {
           ret = in.at(0)->GetCopy();
           retm.push_back(ret);

           for(int i = 1; i < in.size(); i++)
           {
               retm.push_back(in.at(i)->GetCopy());
           }
        }else
        {
            LISEMS_ERROR("Error during segmentation algorithm, no maps as input");
            throw 1;
        }

        const unsigned int Dimension = 2;

        using PixelType      = float;
        using LabelPixelType = float;

        using ImageType      = itk::Image<itk::Vector<float,vl>, Dimension>;
        using LabelImageType = itk::Image<itk::Vector<float,vl>, Dimension>;

        using FilterType = itk::VectorGradientAnisotropicDiffusionImageFilter<ImageType,ImageType>;

        typename itk::VectorGradientAnisotropicDiffusionImageFilter<ImageType,ImageType>::Pointer filter  = itk::VectorGradientAnisotropicDiffusionImageFilter<ImageType,ImageType>::New();

        filter->SetNumberOfIterations(iter);//100);
        filter->SetConductanceParameter(conductance);//0.2);
        filter->SetTimeStep(0.125);

        typename itk::Image<itk::Vector<float,vl>, Dimension>::Pointer image = itk::Image<itk::Vector<float,vl>, Dimension>::New();
        unsigned long long xs = in.at(0)->nrCols();
        unsigned long long ys = in.at(0)->nrRows();
        const typename itk::Image<itk::Vector<float,vl>, Dimension>::SizeType size = {{xs,ys}};
        const typename itk::Image<itk::Vector<float,vl>, Dimension>::IndexType start = {{0,0}};

        typename itk::Image<itk::Vector<float,vl>, Dimension>::RegionType region;
        region.SetSize(size);
        region.SetIndex(start);
        image->SetRegions(region);
        typename itk::Image<itk::Vector<float,vl>, Dimension>::SpacingType spacing;
        spacing[0] = 1; // spacing along X
        spacing[1] = 1; // spacing along Y
        image->SetSpacing( spacing );


        int components = vl;

        image->Allocate();


        typename itk::Vector<float,vl> * data = (image->GetBufferPointer());

        int cols =ret->nrCols();
        int rows = ret->nrRows();

        typename ImageType::PixelType valtemp;

        for(int r = 0; r < rows; r++)
        {
            for(int c = 0; c < cols; c++)
            {
                const typename ImageType::IndexType pixelIndex = {{c,r}};
                typename ImageType::PixelType val;

                for(int i = 0; i < vl; i++)
                {
                    val.SetElement(i,in.at(i)->data[r][c]);
                }

                image->SetPixel(pixelIndex,val);
            }
        }


        filter->SetInput(image);

        filter->Update();

        typename itk::Image<itk::Vector<float,vl>,2>::Pointer clusteredimage = filter->GetOutput();

        typename itk::Vector<float,vl> * data_out = reinterpret_cast<itk::Vector<float,vl>*>(clusteredimage->GetBufferPointer());

        for(int r = 0; r < ret->nrRows(); r++)
        {
            for(int c = 0; c < ret->nrCols(); c++)
            {
                itk::Vector<float,vl> pixel = data_out[r*ret->nrCols() + c];
                for(int i = 0; i < vl; i++)
                {
                    retm.at(i)->data[r][c] = pixel.GetElement(i);
                }

            }
        }

        return retm;
    }

    return {nullptr};

}


inline static std::vector<cTMap*> AS_RasterAnisotropicDiffusion(std::vector<cTMap*> inputmapsq, int iter, float conductance, float timestep)
{


    std::vector<cTMap*> ret;

    if(inputmapsq.size() == 1)
    {
         ret = RasterAnisotropicDiffusion<1>(inputmapsq,iter,conductance,timestep);
    }else if(inputmapsq.size() == 2)
    {
         ret = RasterAnisotropicDiffusion<2>(inputmapsq,iter,conductance,timestep);
    }else if(inputmapsq.size() == 3)
    {
         ret = RasterAnisotropicDiffusion<3>(inputmapsq,iter,conductance,timestep);
    }else if(inputmapsq.size() == 4)
    {
         ret = RasterAnisotropicDiffusion<4>(inputmapsq,iter,conductance,timestep);
    }else if(inputmapsq.size() == 5)
    {
         ret = RasterAnisotropicDiffusion<5>(inputmapsq,iter,conductance,timestep);
    }else if(inputmapsq.size() == 6)
    {
         ret = RasterAnisotropicDiffusion<6>(inputmapsq,iter,conductance,timestep);
    }else
    {
        LISEMS_ERROR("Can not do anisotropic filter with more than 6  bands");
        throw 1;
    }

    return ret;

}

inline static cTMap * AS_RasterAnisotropicDiffusion(cTMap * in, int iter, float conductance, float timestep)
{
    std::vector<cTMap*> inputmapsq = {in};

    std::vector<cTMap*> ret;

    ret = RasterAnisotropicDiffusion<1>(inputmapsq,iter,conductance,timestep);

    return ret.at(0);
}



#include "otb/otbFastNLMeansImageFilter.h"
#include "rasterotbcommon.h"

inline static cTMap * AS_FastNLMeansFilter(cTMap * in, float window = 7 , float cutoff = 1.0)
{
    const unsigned int Dimension = 2;

    using PixelType      = float;
    using LabelPixelType = float;

    using ImageType      = otb::Image<PixelType, Dimension>;
    using LabelImageType = otb::Image<LabelPixelType, Dimension>;

    using FilterType = otb::NLMeansFilter<ImageType,ImageType>;

    ImageType::Pointer imagein = ImageType::New();

    MapToOtbImage(in,imagein);

    cTMap * ret = in->GetCopy();

    FilterType::Pointer f= FilterType::New();

    f->SetCutOffDistance(cutoff);
    f->SetSigma(0.0);
    f->SetHalfWindowSize(window);
    f->SetHalfSearchSize(2);
    f->SetInput(imagein);

    f->Update();

    ImageType::Pointer imageout = f->GetOutput();
    OtbImageToMap(ret,imageout);

    return ret;

}








#endif // RASTERSEGMENTIZE_H
