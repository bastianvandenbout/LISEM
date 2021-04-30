#ifndef RASTERSPECTRALUNMIXING_H
#define RASTERSPECTRALUNMIXING_H


#include "geo/raster/map.h"
#include "raster/rastercommon.h"
#include "otbImage.h"
#include "rasterotbcommon.h"


#include "otbUnConstrainedLeastSquareImageFilter.h"
#include "otbISRAUnmixingImageFilter.h"
#include "otbMDMDNMFImageFilter.h"
#include "otbStreamingStatisticsVectorImageFilter.h"
#include "otbEigenvalueLikelihoodMaximisation.h"
#include "otbVirtualDimensionality.h"


typedef otb::VectorImage<float,2> FloatVectorImageType;
typedef otb::StreamingStatisticsVectorImageFilter<FloatVectorImageType, double> StreamingStatisticsVectorImageFilterType;
typedef otb::VirtualDimensionality<double>            VirtualDimensionalityType;
typedef otb::EigenvalueLikelihoodMaximisation<double> EigenvalueLikelihoodMaximisationType;


inline static int AS_SpectralEndmemberEstimate(std::vector<cTMap *> bands, QString method)
{

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
    FloatVectorImageType::Pointer inputImage = FloatVectorImageType::New();
    MapToOtbVImage(bands,inputImage);



    auto statisticsFilter = StreamingStatisticsVectorImageFilterType::New();
        statisticsFilter->SetInput(inputImage);

    auto correlationMatrix = statisticsFilter->GetCorrelation().GetVnlMatrix();
    auto covarianceMatrix  = statisticsFilter->GetCovariance().GetVnlMatrix();
    auto numberOfPixels    = inputImage->GetLargestPossibleRegion().GetNumberOfPixels();

    int numberOfEndmembers = 1;
    if(method == "HFC-VD")
    {
        auto elm = EigenvalueLikelihoodMaximisationType::New();
              elm->SetCovariance(covarianceMatrix);
              elm->SetCorrelation(correlationMatrix);
              elm->SetNumberOfPixels(numberOfPixels);
              elm->Compute();
              numberOfEndmembers = elm->GetNumberOfEndmembers();
    }else if(method == "ELM")
    {
        auto vd = VirtualDimensionalityType::New();
              vd->SetCovariance(covarianceMatrix);
              vd->SetCorrelation(correlationMatrix);
              vd->SetNumberOfPixels(numberOfPixels);
              vd->SetFAR(1.0E-3);
              vd->Compute();
              numberOfEndmembers = vd->GetNumberOfEndmembers();
    }else
    {
        LISEMS_ERROR("Method not recognized, options are HFC-VD and ELM");
        throw 1;
    }


    return numberOfEndmembers;
}




typedef otb::VectorImage<float,2> DoubleVectorImageType;
typedef otb::UnConstrainedLeastSquareImageFilter<DoubleVectorImageType, DoubleVectorImageType, float> UCLSUnmixingFilterType;
typedef otb::ISRAUnmixingImageFilter<DoubleVectorImageType, DoubleVectorImageType, float>             ISRAUnmixingFilterType;
typedef otb::MDMDNMFImageFilter<DoubleVectorImageType, DoubleVectorImageType> MDMDNMFUnmixingFilterType;
typedef otb::VectorImageToMatrixImageFilter<DoubleVectorImageType> VectorImageToMatrixImageFilterType;
typedef otb::VectorImageToMatrixImageFilter<DoubleVectorImageType> VectorImageToMatrixImageFilterType;

typedef vnl_vector<float> VectorType;
typedef vnl_matrix<float> MatrixType;
typedef vnl_matrix<double> MatrixDType;

inline static std::vector<cTMap *> AS_SpectralUnmix(std::vector<cTMap *> bands, std::vector<std::vector<double>>  spectra_all, QString method)
{


    std::vector<cTMap *> ret;
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

    if(spectra_all.size() == 0)
    {
        LISEMS_ERROR("No endmembers provided");
        throw 1;
    }
    for(int i = 0; i < spectra_all.size(); i++)
    {
        std::vector<double> spectra_this = spectra_all.at(i);

        if(spectra_this.size() != bands.size())
        {
            LISEMS_ERROR("Spectra not of right size");
            throw 1;
        }
    }

    for(int i = 0; i < bands.size(); i++)
    {
        ret.push_back(bands.at(i)->GetCopy0());
    }


    DoubleVectorImageType::Pointer inputImage = DoubleVectorImageType::New();
    MapToOtbVImage(bands,inputImage);

    DoubleVectorImageType::Pointer abundanceMap;

    DoubleVectorImageType::Pointer endmembersImage = DoubleVectorImageType::New();
    typedef float PixelType;
    typedef otb::VectorImage<PixelType, 2> VectorImageType;



    unsigned long long xs2 = spectra_all.size();
    unsigned long long ys2 = 1;

    int cols2 =spectra_all.size();
    int rows2 =1;

    const VectorImageType::SizeType size2 = {{xs2,ys2}};
    const VectorImageType::IndexType start2 = {{0,0}};

    VectorImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    endmembersImage->SetRegions(region2);
    VectorImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    endmembersImage->SetSpacing( spacing2 );
    int components2 = ret.size();
    endmembersImage->SetNumberOfComponentsPerPixel(components2);
    endmembersImage->Allocate();

    for(int r = 0; r < rows2; r++)
    {
        for(int c = 0; c < cols2; c++)
        {
            const VectorImageType::IndexType pixelIndex = {{c,r}};
            VectorImageType::PixelType val;
            val.SetSize(components2);

            std::vector<double> spectra_this = spectra_all.at(c);


            for(int i = 0; i < components2; i++)
            {
                 val.SetElement(i,spectra_this.at(i));
            }
            endmembersImage->SetPixel(pixelIndex,val);
        }
    }



    VectorImageToMatrixImageFilterType::Pointer endMember2Matrix = VectorImageToMatrixImageFilterType::New();
    endMember2Matrix->SetInput(endmembersImage);
    endMember2Matrix->Update();

    MatrixType endMembersMatrix = endMember2Matrix->GetMatrix();
    MatrixDType endMembersMatrixd;
    endMembersMatrixd.set_size(endMembersMatrix.rows(),endMembersMatrix.cols());
    for(int r = 0; r < endMembersMatrix.rows(); r++)
    {
        for(int c = 0; c < endMembersMatrix.cols(); c++)
        {
           endMembersMatrixd[r][c] = endMembersMatrix[r][c];
        }

    }

   if(method == "UCLS")
    {

      UCLSUnmixingFilterType::Pointer unmixer = UCLSUnmixingFilterType::New();

      unmixer->SetInput(inputImage);
      unmixer->GetModifiableFunctor().SetMatrix(endMembersMatrix);
      unmixer->SetNumberOfThreads(1); // FIXME : currently buggy

      abundanceMap = unmixer->GetOutput();
    }else if(method == "ISRA")
    {

      ISRAUnmixingFilterType::Pointer unmixer = ISRAUnmixingFilterType::New();

      unmixer->SetInput(inputImage);
      unmixer->GetModifiableFunctor().SetEndmembersMatrix(endMembersMatrix);
      abundanceMap = unmixer->GetOutput();
    }else if(method == "MDMD-NMF")
    {
      MDMDNMFUnmixingFilterType::Pointer unmixer = MDMDNMFUnmixingFilterType::New();

      unmixer->SetInput(inputImage);
      unmixer->SetEndmembersMatrix(endMembersMatrixd);
      abundanceMap = unmixer->GetOutput();
    }else {

        LISEMS_ERROR("Method not regocnized, options are ISRA, MDMD-NMF and UCLS");
        throw 1;
    }

   OtbVImageToMap(ret,abundanceMap);


   return ret;
}

inline static std::vector<cTMap *>  AS_GetSpectra(QList<cTMap *> image, cTMap * train)
{

    //get area-averaged vale for spectra




    return {nullptr};
}



#endif // RASTERSPECTRALUNMIXING_H
