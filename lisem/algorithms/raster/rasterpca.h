#ifndef RASTERPCA_H
#define RASTERPCA_H

#ifdef FORWARD
#error "forward already declared"
#endif
#ifdef INVERSE
#error "inverse already declared"
#endif

#include "geo/raster/map.h"
#include "otbVcaImageFilter.h"
#include "otbSOMImageClassificationFilter.h"
#include "otbImage.h"
#include "otbSOMMap.h"
#include "otbVectorRescaleIntensityImageFilter.h"
#include "rasterotbcommon.h"




inline static std::vector<std::vector<float>> AS_SpectralVCA(std::vector<cTMap*> bands, int count)
{
    typedef otb::VectorImage<double,2> FloatCVectorImageType;

    using ImageType  = otb::VectorImage<double, 2>;

    using RescalerType                       = otb::VectorRescaleIntensityImageFilter<ImageType, ImageType>;
    using VectorImageToMatrixImageFilterType = otb::VectorImageToMatrixImageFilter<ImageType>;



    if(bands.size() == 0)
    {
        LISEMS_ERROR("No raster data in list");
        throw 1;
    }
    cTMap * ref = bands.at(0);
    for(int i = 0; i < bands.size(); i++)
    {
        if(ref->nrCols() != bands.at(i)->nrCols() || ref->nrRows() != bands.at(i)->nrRows())
        {
            LISEMS_ERROR("Raster sizes do not match");
            throw 1;

        }
    }
    FloatCVectorImageType::Pointer inputImage = FloatCVectorImageType::New();
    MapToOtbVDImage(bands,inputImage);


    itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance()->SetSeed(121212);

    RescalerType::Pointer rescaler = RescalerType::New();
      rescaler->SetInput(inputImage);

      ImageType::PixelType minPixel, maxPixel;
      minPixel.SetSize(inputImage->GetNumberOfComponentsPerPixel());
      maxPixel.SetSize(inputImage->GetNumberOfComponentsPerPixel());
      minPixel.Fill(0.);
      maxPixel.Fill(1.);

      rescaler->SetOutputMinimum(minPixel);
      rescaler->SetOutputMaximum(maxPixel);

    using VCAFilterType        = otb::VCAImageFilter<ImageType>;
    VCAFilterType::Pointer vca = VCAFilterType::New();

    vca->SetNumberOfEndmembers(count);
    vca->SetInput(rescaler->GetOutput());


    ImageType::Pointer imageout = vca->GetOutput();

    typedef itk::ImageRegionIterator<ImageType>      IteratorType;

    int retlength = 0;
    IteratorType it( imageout, imageout->GetRequestedRegion() );
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        retlength ++;

    }

    std::vector<std::vector<float>> array;

    int i = 0;
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {

        FloatCVectorImageType::PixelType p = it.Get();
        std::vector<float> array2;
        for(int j = 0; j < p.GetNumberOfElements(); j++)
        {
            float val = p.GetElement(j);
            array2.push_back(val);
        }

        array.push_back(array2);
        i++;
    }

    return array;
}


#endif // RASTERPCA_H
