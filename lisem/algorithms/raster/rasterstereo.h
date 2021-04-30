#ifndef RASTERSTEREO_H
#define RASTERSTEREO_H


#include "geo/raster/map.h"
#include "raster/rastercommon.h"
#include "otbImage.h"
#include "rasterotbcommon.h"
#include "otbStereorectificationDisplacementFieldSource.h"
#include "otbStreamingWarpImageFilter.h"
#include "otbBandMathImageFilter.h"
#include "otbSubPixelDisparityImageFilter.h"
#include "otbDisparityMapMedianFilter.h"
#include "otbDisparityMapToDEMFilter.h"
#include "otbDisparityMapTo3DFilter.h"
#include "otbDEMToImageGenerator.h"

#include "otbVarianceImageFilter.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbBCOInterpolateImageFunction.h"
#include "otbImageToNoDataMaskFilter.h"

#include "itkUnaryFunctorImageFilter.h"
#include "itkInverseDisplacementFieldImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "otbStreamingMinMaxImageFilter.h"
#include "otbExtractROI.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"


#include "otbMultiToMonoChannelExtractROI.h"

#include "otbMultiDisparityMapTo3DFilter.h"
#include "otbMulti3DMapToDEMFilter.h"
#include "otbDisparityTranslateFilter.h"


#include "otbWrapperElevationParametersHandler.h"
#include "otbWrapperMapProjectionParametersHandler.h"

#include "otbMultiToMonoChannelExtractROI.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbBijectionCoherencyFilter.h"


typedef otb::Image<float,2> FloatImageType;
typedef otb::VectorImage<float,2> FloatVectorImageType;
typedef FloatImageType::PixelType FloatPixelType;

typedef otb::StereorectificationDisplacementFieldSource<FloatImageType, FloatVectorImageType> DisplacementFieldSourceType;

typedef itk::Vector<float, 2> DisplacementType;
typedef otb::Image<DisplacementType> DisplacementFieldType;

typedef itk::VectorCastImageFilter<FloatVectorImageType, DisplacementFieldType> DisplacementFieldCastFilterType;

typedef itk::InverseDisplacementFieldImageFilter<DisplacementFieldType, DisplacementFieldType> InverseDisplacementFieldFilterType;


typedef otb::StreamingWarpImageFilter<FloatImageType, FloatImageType, DisplacementFieldType> ResampleFilterType;

typedef otb::BCOInterpolateImageFunction<FloatImageType> InterpolatorType;

typedef otb::Functor::SSDBlockMatching<FloatImageType, FloatImageType>        SSDBlockMatchingFunctorType;
typedef otb::Functor::SSDDivMeanBlockMatching<FloatImageType, FloatImageType> SSDDivMeanBlockMatchingFunctorType;

typedef otb::Functor::NCCBlockMatching<FloatImageType, FloatImageType> NCCBlockMatchingFunctorType;
typedef otb::Functor::LPBlockMatching<FloatImageType, FloatImageType>  LPBlockMatchingFunctorType;

typedef otb::PixelWiseBlockMatchingImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, SSDBlockMatchingFunctorType>
    SSDBlockMatchingFilterType;
typedef otb::PixelWiseBlockMatchingImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, SSDDivMeanBlockMatchingFunctorType>
    SSDDivMeanBlockMatchingFilterType;

typedef otb::PixelWiseBlockMatchingImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, NCCBlockMatchingFunctorType>
    NCCBlockMatchingFilterType;

typedef otb::PixelWiseBlockMatchingImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, LPBlockMatchingFunctorType>
    LPBlockMatchingFilterType;

typedef otb::BandMathImageFilter<FloatImageType> BandMathFilterType;

typedef otb::SubPixelDisparityImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, SSDBlockMatchingFunctorType> SSDSubPixelFilterType;

typedef otb::SubPixelDisparityImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, SSDDivMeanBlockMatchingFunctorType>
    SSDDivMeanSubPixelFilterType;

typedef otb::SubPixelDisparityImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, LPBlockMatchingFunctorType> LPSubPixelFilterType;

typedef otb::SubPixelDisparityImageFilter<FloatImageType, FloatImageType, FloatImageType, FloatImageType, NCCBlockMatchingFunctorType> NCCSubPixelFilterType;

typedef otb::DisparityMapMedianFilter<FloatImageType, FloatImageType, FloatImageType> MedianFilterType;


typedef otb::VarianceImageFilter<FloatImageType, FloatImageType> VarianceFilterType;

typedef otb::DisparityMapToDEMFilter<FloatImageType, FloatImageType, FloatImageType, DisplacementFieldType, FloatImageType> DisparityToElevationFilterType;

typedef otb::DEMToImageGenerator<FloatImageType> DEMToImageGeneratorType;

typedef otb::StreamingMinMaxImageFilter<FloatImageType> MinMaxFilterType;

typedef otb::ExtractROI<FloatPixelType, FloatPixelType> ExtractFilterType;

typedef otb::ImageList<FloatImageType> ImageListType;

typedef otb::ImageListToVectorImageFilter<ImageListType, FloatVectorImageType> ImageListToVectorImageFilterType;

typedef otb::ImageFileReader<FloatVectorImageType> ReaderType;

typedef otb::ImageFileWriter<FloatVectorImageType> WriterType;

typedef otb::VectorImageToImageListFilter<FloatVectorImageType, ImageListType> VectorImageToListFilterType;

typedef otb::MultiToMonoChannelExtractROI<FloatVectorImageType::InternalPixelType, FloatImageType::PixelType> ExtractROIFilterType;

typedef otb::MultiDisparityMapTo3DFilter<FloatImageType, FloatVectorImageType, FloatImageType> MultiDisparityTo3DFilterType;
typedef MultiDisparityTo3DFilterType::Pointer MultiDisparityTo3DFilterPointerType;
typedef otb::Multi3DMapToDEMFilter<FloatVectorImageType, FloatImageType, FloatImageType> Multi3DFilterType;

typedef otb::DisparityTranslateFilter<FloatImageType, DisplacementFieldType, FloatImageType, FloatImageType> DisparityTranslateFilter;

typedef otb::DisparityMapTo3DFilter<FloatImageType, FloatVectorImageType, DisplacementFieldType, FloatImageType> DisparityTo3DFilter;

typedef itk::VectorIndexSelectionCastImageFilter<DisplacementFieldType, FloatImageType> IndexSelectionCastFilterType;

typedef otb::ImageListToVectorImageFilter<ImageListType, FloatVectorImageType> ImageListFilterType;

typedef ExtractROIFilterType::Pointer                    ExtractROIFilterPointerType;
typedef std::vector<ExtractROIFilterPointerType>         ExtractorListType;
typedef std::vector<MultiDisparityTo3DFilterPointerType> MultiDisparityTo3DFilterListType;

typedef otb::BijectionCoherencyFilter<FloatImageType, FloatImageType> BijectionFilterType;

typedef itk::ImageToImageFilter<FloatImageType, FloatImageType> FilterType;

typedef otb::ImageToNoDataMaskFilter<FloatImageType, FloatImageType> NoDataMaskFilterType;



template <class TInputImage, class TMetricFunctor>
inline static void
  SetBlockMatchingParameters(otb::PixelWiseBlockMatchingImageFilter<TInputImage, TInputImage, TInputImage, TInputImage, TMetricFunctor>* blockMatcherFilter,
                             otb::PixelWiseBlockMatchingImageFilter<TInputImage, TInputImage, TInputImage, TInputImage, TMetricFunctor>* invBlockMatcherFilter,
                             otb::SubPixelDisparityImageFilter<TInputImage, TInputImage, TInputImage, TInputImage, TMetricFunctor>*      subPixelFilter,
                             TInputImage* leftImage, TInputImage* rightImage, TInputImage* leftMask, TInputImage* rightMask, TInputImage* finalMask,
                             const bool minimize, double minDisp, double maxDisp)
  {
    typedef TMetricFunctor MetricFunctorType;
    typedef otb::SubPixelDisparityImageFilter<TInputImage, TInputImage, TInputImage, TInputImage, MetricFunctorType> SubPixelFilterType;

    blockMatcherFilter->SetLeftInput(leftImage);
    blockMatcherFilter->SetRightInput(rightImage);
    blockMatcherFilter->SetLeftMaskInput(leftMask);
    blockMatcherFilter->SetRightMaskInput(rightMask);
    blockMatcherFilter->SetRadius(2);//this->GetParameterInt("bm.radius"));
    blockMatcherFilter->SetMinimumHorizontalDisparity(minDisp);
    blockMatcherFilter->SetMaximumHorizontalDisparity(maxDisp);
    blockMatcherFilter->SetMinimumVerticalDisparity(0);
    blockMatcherFilter->SetMaximumVerticalDisparity(0);

    if (minimize)
    {
      blockMatcherFilter->MinimizeOn();
    }
    else
      blockMatcherFilter->MinimizeOff();

    //if (GetParameterInt("postproc.bij"))
    {
      invBlockMatcherFilter->SetLeftInput(rightImage);
      invBlockMatcherFilter->SetRightInput(leftImage);
      invBlockMatcherFilter->SetLeftMaskInput(rightMask);
      invBlockMatcherFilter->SetRightMaskInput(leftMask);
      invBlockMatcherFilter->SetRadius(2);//this->GetParameterInt("bm.radius"));
      invBlockMatcherFilter->SetMinimumHorizontalDisparity(-maxDisp);
      invBlockMatcherFilter->SetMaximumHorizontalDisparity(-minDisp);
      invBlockMatcherFilter->SetMinimumVerticalDisparity(0);
      invBlockMatcherFilter->SetMaximumVerticalDisparity(0);

      if (minimize)
      {
        invBlockMatcherFilter->MinimizeOn();
      }
      else
        invBlockMatcherFilter->MinimizeOff();
    }

    subPixelFilter->SetInputsFromBlockMatchingFilter(blockMatcherFilter);
    subPixelFilter->SetRefineMethod(SubPixelFilterType::DICHOTOMY);
    subPixelFilter->SetLeftMaskInput(finalMask);
    subPixelFilter->UpdateOutputInformation();
  }


inline static cTMap * AS_RasterDisparityDetailed(cTMap * first, cTMap * second, QString method_match, float minDisp, float maxDisp)
{

    if(first->nrCols() != second->nrCols() || first->nrRows() != second->nrRows())
    {
        LISEMS_ERROR("first and second image are not of identical size");
        throw 1;
    }

    FloatImageType::Pointer inleft  = FloatImageType::New();
    FloatImageType::Pointer inright = FloatImageType::New();

    MapToOtbImage(first,inleft);
    MapToOtbImage(second,inright);


    FloatImageType::Pointer maskleft  = FloatImageType::New();
    FloatImageType::Pointer maskright = FloatImageType::New();

    MapMaskToOtbImage(first,maskleft);
    MapMaskToOtbImage(second,maskright);


    return nullptr;

    NCCBlockMatchingFilterType::Pointer NCCBlockMatcherFilter;
    NCCBlockMatchingFilterType::Pointer invNCCBlockMatcherFilter;
    NCCSubPixelFilterType::Pointer      NCCSubPixelFilter;

    SSDBlockMatchingFilterType::Pointer SSDBlockMatcherFilter;
    SSDBlockMatchingFilterType::Pointer invSSDBlockMatcherFilter;
    SSDSubPixelFilterType::Pointer      SSDSubPixelFilter;

    SSDDivMeanBlockMatchingFilterType::Pointer SSDDivMeanBlockMatcherFilter;
    SSDDivMeanBlockMatchingFilterType::Pointer invSSDDivMeanBlockMatcherFilter;
    SSDDivMeanSubPixelFilterType::Pointer      SSDDivMeanSubPixelFilter;

    LPBlockMatchingFilterType::Pointer LPBlockMatcherFilter;
    LPBlockMatchingFilterType::Pointer invLPBlockMatcherFilter;
    LPSubPixelFilterType::Pointer      LPSubPixelFilter;

    FilterType*                  blockMatcherFilterPointer    = nullptr;
    FilterType*                  invBlockMatcherFilterPointer = nullptr;
    FilterType*                  subPixelFilterPointer        = nullptr;
    BijectionFilterType::Pointer bijectFilter;


    BandMathFilterType::Pointer finalMaskFilter = BandMathFilterType::New();
    finalMaskFilter->SetNthInput(0, maskleft, "inmask");

    bool minimize = false;
    if(method_match == "SSDDivMean")
    {
        SSDDivMeanBlockMatcherFilter = SSDDivMeanBlockMatchingFilterType::New();
        blockMatcherFilterPointer    = SSDDivMeanBlockMatcherFilter.GetPointer();

        invSSDDivMeanBlockMatcherFilter = SSDDivMeanBlockMatchingFilterType::New();
        invBlockMatcherFilterPointer    = invSSDDivMeanBlockMatcherFilter.GetPointer();

        SSDDivMeanSubPixelFilter = SSDDivMeanSubPixelFilterType::New();
        subPixelFilterPointer    = SSDDivMeanSubPixelFilter.GetPointer();

        minimize = true;
        SetBlockMatchingParameters<FloatImageType, SSDDivMeanBlockMatchingFunctorType>(
                    SSDDivMeanBlockMatcherFilter, invSSDDivMeanBlockMatcherFilter, SSDDivMeanSubPixelFilter, inleft,
                    inright, maskleft, maskright, finalMaskFilter->GetOutput(), minimize, minDisp,
                    maxDisp);

    }else if(method_match == "NCC")
    {
        NCCBlockMatcherFilter     = NCCBlockMatchingFilterType::New();
        blockMatcherFilterPointer = NCCBlockMatcherFilter.GetPointer();


        //if (GetParameterInt("postproc.bij"))
        {
          // Reverse correlation
          invNCCBlockMatcherFilter     = NCCBlockMatchingFilterType::New();
          invBlockMatcherFilterPointer = invNCCBlockMatcherFilter.GetPointer();
        }
        NCCSubPixelFilter     = NCCSubPixelFilterType::New();
        subPixelFilterPointer = NCCSubPixelFilter.GetPointer();

        minimize = false;
        SetBlockMatchingParameters<FloatImageType, NCCBlockMatchingFunctorType>(
            NCCBlockMatcherFilter, invNCCBlockMatcherFilter, NCCSubPixelFilter, inleft,
                    inright, maskleft, maskright, finalMaskFilter->GetOutput(), minimize, minDisp,
                    maxDisp);
    }else if(method_match == "SSD")
    {
        SSDBlockMatcherFilter     = SSDBlockMatchingFilterType::New();
        blockMatcherFilterPointer = SSDBlockMatcherFilter.GetPointer();

        //if (GetParameterInt("postproc.bij"))
        {
          // Reverse correlation
          invSSDBlockMatcherFilter     = SSDBlockMatchingFilterType::New();
          invBlockMatcherFilterPointer = invSSDBlockMatcherFilter.GetPointer();
        }
        SSDSubPixelFilter     = SSDSubPixelFilterType::New();
        subPixelFilterPointer = SSDSubPixelFilter.GetPointer();

        minimize = true;
        SetBlockMatchingParameters<FloatImageType, SSDBlockMatchingFunctorType>(
            SSDBlockMatcherFilter, invSSDBlockMatcherFilter, SSDSubPixelFilter, inleft,
                    inright, maskleft, maskright, finalMaskFilter->GetOutput(), minimize, minDisp,
                    maxDisp);


    }else if(method_match == "LP")
    {
        LPBlockMatcherFilter = LPBlockMatchingFilterType::New();
        LPBlockMatcherFilter->GetFunctor().SetP(static_cast<double>(1.0));//GetParameterFloat("bm.metric.lp.p")));

        blockMatcherFilterPointer = LPBlockMatcherFilter.GetPointer();

        //if (GetParameterInt("postproc.bij"))
        {
          // Reverse correlation
          invLPBlockMatcherFilter = LPBlockMatchingFilterType::New();
          invLPBlockMatcherFilter->GetFunctor().SetP(static_cast<double>(1.0));//GetParameterFloat("bm.metric.lp.p")));
          invBlockMatcherFilterPointer = invLPBlockMatcherFilter.GetPointer();

        }
        LPSubPixelFilter      = LPSubPixelFilterType::New();
        subPixelFilterPointer = LPSubPixelFilter.GetPointer();

        minimize = false;
        SetBlockMatchingParameters<FloatImageType, LPBlockMatchingFunctorType>(
            LPBlockMatcherFilter, invLPBlockMatcherFilter, LPSubPixelFilter, inleft,
                    inright, maskleft, maskright, finalMaskFilter->GetOutput(), minimize, minDisp,
                    maxDisp);


    }else
    {
        LISEMS_ERROR("Matching method not recognized, options are SSDDivMean, SSD, NCC and LP");
        throw 1;
    }


    bijectFilter = BijectionFilterType::New();
    // bijectFilter->SetDirectHorizontalDisparityMapInput(blockMatcherFilter->GetHorizontalDisparityOutput());
    bijectFilter->SetDirectHorizontalDisparityMapInput(blockMatcherFilterPointer->GetOutput(1));
    bijectFilter->SetDirectVerticalDisparityMapInput(blockMatcherFilterPointer->GetOutput(2));
    bijectFilter->SetReverseHorizontalDisparityMapInput(invBlockMatcherFilterPointer->GetOutput(1));
    bijectFilter->SetReverseVerticalDisparityMapInput(invBlockMatcherFilterPointer->GetOutput(2));
    // bijectFilter->SetTolerance(2);
    bijectFilter->SetMinHDisp(minDisp);
    bijectFilter->SetMaxHDisp(maxDisp);
    bijectFilter->SetMinVDisp(minDisp);
    bijectFilter->SetMaxVDisp(maxDisp);

    // finalMaskFilter = BandMathFilterType::New();
    // finalMaskFilter->SetNthInput(0, lBandMathFilter->GetOutput(), "inmask");
    finalMaskFilter->SetNthInput(1, bijectFilter->GetOutput(), "lrrl");

#ifdef OTB_MUPARSER_HAS_CXX_LOGICAL_OPERATORS
    finalMaskFilter->SetExpression("(inmask > 0 and lrrl > 0) ? 255 : 0");
#else
    finalMaskFilter->SetExpression("if(inmask > 0 and lrrl > 0, 255, 0)");
#endif


    FloatImageType::Pointer hDispOutput    = subPixelFilterPointer->GetOutput(0);
  FloatImageType::Pointer finalMaskImage = finalMaskFilter->GetOutput();
  //if (GetParameterInt("postproc.med"))
  {
    MedianFilterType::Pointer hMedianFilter = MedianFilterType::New();
    hMedianFilter->SetInput(subPixelFilterPointer->GetOutput(0));
    hMedianFilter->SetRadius(2);
    hMedianFilter->SetIncoherenceThreshold(2.0);
    hMedianFilter->SetMaskInput(finalMaskFilter->GetOutput());
    hMedianFilter->UpdateOutputInformation();
    hDispOutput    = hMedianFilter->GetOutput();
    finalMaskImage = hMedianFilter->GetOutputMask();
  }

  cTMap * ret = first->GetCopy0();
  OtbImageToMap(ret,hDispOutput);

    return ret;

}


inline static cTMap * AS_RasterStereoElevation(cTMap * left, cTMap * right, QString method_match, float elevation_range, bool use_refdem, cTMap * refdem)
{

    //first get impage pair

    FloatImageType::Pointer inleft  = FloatImageType::New();
    FloatImageType::Pointer inright = FloatImageType::New();

    MapToOtbImage(left,inleft);
    MapToOtbImage(right,inright);

    //epipolar transform

    Multi3DFilterType::Pointer       m_Multi3DMapToDEMFilter;

    m_Multi3DMapToDEMFilter = Multi3DFilterType::New();

    m_Multi3DMapToDEMFilter->SetNumberOf3DMaps(1);
    float mv = 0.0;
    pcr::setMV(mv);
    m_Multi3DMapToDEMFilter->SetNoDataValue(mv);

    MultiDisparityTo3DFilterType::Pointer m_MultiDisparityTo3DFilterList                    = MultiDisparityTo3DFilterType::New();
    DisplacementFieldSourceType::Pointer epipolarGridSource = DisplacementFieldSourceType::New();
    epipolarGridSource->SetLeftImage(inleft);
    epipolarGridSource->SetRightImage(inright);
    epipolarGridSource->SetGridStep(8);//fwdgridstep
    epipolarGridSource->SetScale(1.0);
    epipolarGridSource->UpdateOutputInformation();
    epipolarGridSource->Update();
    FloatVectorImageType::SizeType gridSize    = epipolarGridSource->GetLeftDisplacementFieldOutput()->GetLargestPossibleRegion().GetSize();
    double                         storageSize = static_cast<double>(gridSize[0]) * static_cast<double>(gridSize[1]) * 4.0 * 8.0 / 1000000.0;
    double globalEpiStorageSize = 0.0;
    globalEpiStorageSize += storageSize;

    epipolarGridSource->Update();

    FloatImageType::SpacingType epiSpacing;
    epiSpacing[0] = 0.5 * (std::abs(inleft->GetSignedSpacing()[0]) + std::abs(inleft->GetSignedSpacing()[1]));
    epiSpacing[1] = 0.5 * (std::abs(inleft->GetSignedSpacing()[0]) + std::abs(inleft->GetSignedSpacing()[1]));

    FloatImageType::SizeType epiSize;
    epiSize = epipolarGridSource->GetRectifiedImageSize();
    FloatImageType::PointType epiOrigin;
    epiOrigin[0] = 0.0;
    epiOrigin[1] = 0.0;

    FloatImageType::PixelType defaultValue = 0;

    double meanBaseline = epipolarGridSource->GetMeanBaselineRatio();

    double underElev = -20.0;//this->GetParameterFloat("bm.minhoffset");
    double overElev  = 20.0;//this->GetParameterFloat("bm.maxhoffset");
    double minDisp = std::floor((-1.0) * overElev * meanBaseline / epiSpacing[0]);
    double maxDisp = std::ceil((-1.0) * underElev * meanBaseline / epiSpacing[0]);

    //displacement field
    DisplacementFieldCastFilterType::Pointer leftGridCaster = DisplacementFieldCastFilterType::New();
    leftGridCaster->SetInput(epipolarGridSource->GetLeftDisplacementFieldOutput());
    leftGridCaster->Update();

    DisplacementFieldType::Pointer leftDisplacement;
    leftDisplacement = leftGridCaster->GetOutput();
    leftDisplacement->DisconnectPipeline();

    InverseDisplacementFieldFilterType::Pointer leftInverseDisplacementFieldFilter = InverseDisplacementFieldFilterType::New();
    leftInverseDisplacementFieldFilter->SetInput(leftDisplacement);

    FloatVectorImageType::PointType   lorigin  = inleft->GetOrigin();
    FloatVectorImageType::SpacingType lspacing = inleft->GetSignedSpacing();
    FloatVectorImageType::SizeType    lsize    = inleft->GetLargestPossibleRegion().GetSize();
    double                            gridStep = epipolarGridSource->GetGridStep();
    lspacing[0] *= gridStep;
    lspacing[1] *= gridStep;
    lsize[0] /= gridStep;
    lsize[1] /= gridStep;

    lsize[0] += 1;
    lsize[1] += 1;
    leftInverseDisplacementFieldFilter->SetOutputOrigin(lorigin);
    leftInverseDisplacementFieldFilter->SetOutputSpacing(lspacing);
    leftInverseDisplacementFieldFilter->SetSize(lsize);
      // change value
    leftInverseDisplacementFieldFilter->SetSubsamplingFactor(10);//"stereorect.invgridssrate"));

    leftInverseDisplacementFieldFilter->Update();

    //resampler

    InterpolatorType::Pointer m_Interpolator          = InterpolatorType::New();
    m_Interpolator->SetRadius(2);

    DisplacementFieldType::Pointer leftInverseDisplacement;
    leftInverseDisplacement = leftInverseDisplacementFieldFilter->GetOutput();
    leftInverseDisplacement->DisconnectPipeline();

    ResampleFilterType::Pointer leftResampleFilter = ResampleFilterType::New();
    leftResampleFilter->SetInput(inleft);
    leftResampleFilter->SetDisplacementField(leftDisplacement);
    leftResampleFilter->SetInterpolator(m_Interpolator);
    leftResampleFilter->SetOutputSize(epiSize);
    leftResampleFilter->SetOutputSpacing(epiSpacing);
    leftResampleFilter->SetOutputOrigin(epiOrigin);
    leftResampleFilter->SetEdgePaddingValue(defaultValue);

    DisplacementFieldCastFilterType::Pointer rightGridCaster = DisplacementFieldCastFilterType::New();
    rightGridCaster->SetInput(epipolarGridSource->GetRightDisplacementFieldOutput());
    rightGridCaster->Update();

    DisplacementFieldType::Pointer rightDisplacement;
    rightDisplacement = rightGridCaster->GetOutput();
    rightDisplacement->DisconnectPipeline();

    ResampleFilterType::Pointer rightResampleFilter = ResampleFilterType::New();
    rightResampleFilter->SetInput(inright);
    rightResampleFilter->SetDisplacementField(rightDisplacement);
    rightResampleFilter->SetInterpolator(m_Interpolator);
    rightResampleFilter->SetOutputSize(epiSize);
    rightResampleFilter->SetOutputSpacing(epiSpacing);
    rightResampleFilter->SetOutputOrigin(epiOrigin);
    rightResampleFilter->SetEdgePaddingValue(defaultValue);


    //masking

    FloatImageType::Pointer     leftmask;
    FloatImageType::Pointer     rightmask;
    BandMathFilterType::Pointer lBandMathFilter = BandMathFilterType::New();
    BandMathFilterType::Pointer rBandMathFilter = BandMathFilterType::New();
    unsigned int                inputIdLeft     = 0;
    unsigned int                inputIdRight    = 0;

    lBandMathFilter->SetNthInput(inputIdLeft, leftResampleFilter->GetOutput(), "inleft");
    ++inputIdLeft;
    std::ostringstream leftCondition;
    leftCondition << "(inleft > 0)";

    ResampleFilterType::Pointer leftMaskResampleFilter = ResampleFilterType::New();

    {
        cTMap * maskleft = left->GetCopyMask();
           leftmask = FloatImageType::New();
           MapToOtbImage(maskleft,leftmask);

           leftMaskResampleFilter->SetInput(leftmask);
           leftMaskResampleFilter->SetDisplacementField(leftDisplacement);
           leftMaskResampleFilter->SetInterpolator(m_Interpolator);
           leftMaskResampleFilter->SetOutputSize(epiSize);
           leftMaskResampleFilter->SetOutputSpacing(epiSpacing);
           leftMaskResampleFilter->SetOutputOrigin(epiOrigin);
           leftMaskResampleFilter->SetEdgePaddingValue(defaultValue);

           lBandMathFilter->SetNthInput(inputIdLeft, leftMaskResampleFilter->GetOutput(), "maskleft");
           ++inputIdLeft;
           leftCondition << " and (maskleft > 0)";
     }
     // Handle variance threshold if present

     {
       // Left side
       VarianceFilterType::Pointer leftVarianceFilter = VarianceFilterType::New();
       leftVarianceFilter->SetInput(leftResampleFilter->GetOutput());
       VarianceFilterType::InputSizeType vradius;
       vradius.Fill(2);//"bm.radius"));
       leftVarianceFilter->SetRadius(vradius);

       lBandMathFilter->SetNthInput(inputIdLeft, leftVarianceFilter->GetOutput(), "variance");
       ++inputIdLeft;
       leftCondition << " and variance > " << 50.0;// GetParameterFloat("mask.variancet");

     }


    const std::string state     = "255";
    const std::string elseState = "0";

    std::ostringstream leftFormula;

#ifdef OTB_MUPARSER_HAS_CXX_LOGICAL_OPERATORS
    leftFormula << leftCondition.str() << " ? " << state << " : " << elseState;
#else
    leftFormula << "if(" << leftCondition.str() << "," << state << "," << elseState << ")";
#endif

    lBandMathFilter->SetExpression(leftFormula.str());

    rBandMathFilter->SetNthInput(inputIdRight, rightResampleFilter->GetOutput(), "inright");
      ++inputIdRight;
      std::ostringstream rightCondition;
      rightCondition << "(inright > 0)";
      ResampleFilterType::Pointer rightMaskResampleFilter = ResampleFilterType::New();

      //if (IsParameterEnabled("mask.right") && HasValue("mask.right"))
      {
          cTMap * maskright = left->GetCopyMask();
         rightmask = FloatImageType::New();
         MapToOtbImage(maskright,rightmask);

        rightMaskResampleFilter->SetInput(rightmask);
        rightMaskResampleFilter->SetDisplacementField(rightDisplacement);
        rightMaskResampleFilter->SetInterpolator(m_Interpolator);
        rightMaskResampleFilter->SetOutputSize(epiSize);
        rightMaskResampleFilter->SetOutputSpacing(epiSpacing);
        rightMaskResampleFilter->SetOutputOrigin(epiOrigin);
        rightMaskResampleFilter->SetEdgePaddingValue(defaultValue);

        rBandMathFilter->SetNthInput(inputIdRight, rightMaskResampleFilter->GetOutput(), "maskright");
        ++inputIdRight;
        rightCondition << " and (maskright > 0)";
      }
      //if (IsParameterEnabled("mask.variancet"))
      {
        // right side
        VarianceFilterType::Pointer rightVarianceFilter = VarianceFilterType::New();
        rightVarianceFilter->SetInput(rightResampleFilter->GetOutput());
        VarianceFilterType::InputSizeType vradius;
        vradius.Fill(2);//"bm.radius"));
        rightVarianceFilter->SetRadius(vradius);

        rBandMathFilter->SetNthInput(inputIdRight, rightVarianceFilter->GetOutput(), "variance");
        ++inputIdRight;
        rightCondition << " and variance > " << 50.0;//GetParameterFloat("mask.variancet");
      }

      std::ostringstream rightFormula;

#ifdef OTB_MUPARSER_HAS_CXX_LOGICAL_OPERATORS
      rightFormula << rightCondition.str() << " ? " << state << " : " << elseState;
#else
      rightFormula << "if(" << rightCondition.str() << "," << state << "," << elseState << ")";
#endif

      rBandMathFilter->SetExpression(rightFormula.str());


      BandMathFilterType::Pointer finalMaskFilter;
      //if (GetParameterInt("postproc.bij"))
      {
        finalMaskFilter = BandMathFilterType::New();
        finalMaskFilter->SetNthInput(0, lBandMathFilter->GetOutput(), "inmask");
      }


    //block matching
  // Compute disparities
        FilterType*                  blockMatcherFilterPointer    = nullptr;
        FilterType*                  invBlockMatcherFilterPointer = nullptr;
        FilterType*                  subPixelFilterPointer        = nullptr;
        BijectionFilterType::Pointer bijectFilter;

        // pointer
        bool minimize = false;
        // switch


         NCCBlockMatchingFilterType::Pointer NCCBlockMatcherFilter;
         NCCBlockMatchingFilterType::Pointer invNCCBlockMatcherFilter;
         NCCSubPixelFilterType::Pointer      NCCSubPixelFilter;

         SSDBlockMatchingFilterType::Pointer SSDBlockMatcherFilter;
         SSDBlockMatchingFilterType::Pointer invSSDBlockMatcherFilter;
         SSDSubPixelFilterType::Pointer      SSDSubPixelFilter;

         SSDDivMeanBlockMatchingFilterType::Pointer SSDDivMeanBlockMatcherFilter;
         SSDDivMeanBlockMatchingFilterType::Pointer invSSDDivMeanBlockMatcherFilter;
         SSDDivMeanSubPixelFilterType::Pointer      SSDDivMeanSubPixelFilter;

         LPBlockMatchingFilterType::Pointer LPBlockMatcherFilter;
         LPBlockMatchingFilterType::Pointer invLPBlockMatcherFilter;
         LPSubPixelFilterType::Pointer      LPSubPixelFilter;


         if(method_match == "SSDDivMean")
         {
             SSDDivMeanBlockMatcherFilter = SSDDivMeanBlockMatchingFilterType::New();
             blockMatcherFilterPointer    = SSDDivMeanBlockMatcherFilter.GetPointer();

             invSSDDivMeanBlockMatcherFilter = SSDDivMeanBlockMatchingFilterType::New();
             invBlockMatcherFilterPointer    = invSSDDivMeanBlockMatcherFilter.GetPointer();

             SSDDivMeanSubPixelFilter = SSDDivMeanSubPixelFilterType::New();
             subPixelFilterPointer    = SSDDivMeanSubPixelFilter.GetPointer();

             minimize = true;
             SetBlockMatchingParameters<FloatImageType, SSDDivMeanBlockMatchingFunctorType>(
                         SSDDivMeanBlockMatcherFilter, invSSDDivMeanBlockMatcherFilter, SSDDivMeanSubPixelFilter, leftResampleFilter->GetOutput(),
                         rightResampleFilter->GetOutput(), lBandMathFilter->GetOutput(), rBandMathFilter->GetOutput(), finalMaskFilter->GetOutput(), minimize, minDisp,
                         maxDisp);

         }else if(method_match == "NCC")
         {
             NCCBlockMatcherFilter     = NCCBlockMatchingFilterType::New();
             blockMatcherFilterPointer = NCCBlockMatcherFilter.GetPointer();


             //if (GetParameterInt("postproc.bij"))
             {
               // Reverse correlation
               invNCCBlockMatcherFilter     = NCCBlockMatchingFilterType::New();
               invBlockMatcherFilterPointer = invNCCBlockMatcherFilter.GetPointer();
             }
             NCCSubPixelFilter     = NCCSubPixelFilterType::New();
             subPixelFilterPointer = NCCSubPixelFilter.GetPointer();

             minimize = false;
             SetBlockMatchingParameters<FloatImageType, NCCBlockMatchingFunctorType>(
                 NCCBlockMatcherFilter, invNCCBlockMatcherFilter, NCCSubPixelFilter, leftResampleFilter->GetOutput(), rightResampleFilter->GetOutput(),
                 lBandMathFilter->GetOutput(), rBandMathFilter->GetOutput(), finalMaskFilter->GetOutput(), minimize, minDisp, maxDisp);
         }else if(method_match == "SSD")
         {
             SSDBlockMatcherFilter     = SSDBlockMatchingFilterType::New();
             blockMatcherFilterPointer = SSDBlockMatcherFilter.GetPointer();

             //if (GetParameterInt("postproc.bij"))
             {
               // Reverse correlation
               invSSDBlockMatcherFilter     = SSDBlockMatchingFilterType::New();
               invBlockMatcherFilterPointer = invSSDBlockMatcherFilter.GetPointer();
             }
             SSDSubPixelFilter     = SSDSubPixelFilterType::New();
             subPixelFilterPointer = SSDSubPixelFilter.GetPointer();

             minimize = true;
             SetBlockMatchingParameters<FloatImageType, SSDBlockMatchingFunctorType>(
                 SSDBlockMatcherFilter, invSSDBlockMatcherFilter, SSDSubPixelFilter, leftResampleFilter->GetOutput(), rightResampleFilter->GetOutput(),
                 lBandMathFilter->GetOutput(), rBandMathFilter->GetOutput(), finalMaskFilter->GetOutput(), minimize, minDisp, maxDisp);


         }else if(method_match == "LP")
         {
             LPBlockMatcherFilter = LPBlockMatchingFilterType::New();
             LPBlockMatcherFilter->GetFunctor().SetP(static_cast<double>(1.0));//GetParameterFloat("bm.metric.lp.p")));

             blockMatcherFilterPointer = LPBlockMatcherFilter.GetPointer();

             //if (GetParameterInt("postproc.bij"))
             {
               // Reverse correlation
               invLPBlockMatcherFilter = LPBlockMatchingFilterType::New();
               invLPBlockMatcherFilter->GetFunctor().SetP(static_cast<double>(1.0));//GetParameterFloat("bm.metric.lp.p")));
               invBlockMatcherFilterPointer = invLPBlockMatcherFilter.GetPointer();

             }
             LPSubPixelFilter      = LPSubPixelFilterType::New();
             subPixelFilterPointer = LPSubPixelFilter.GetPointer();

             minimize = false;
             SetBlockMatchingParameters<FloatImageType, LPBlockMatchingFunctorType>(
                 LPBlockMatcherFilter, invLPBlockMatcherFilter, LPSubPixelFilter, leftResampleFilter->GetOutput(), rightResampleFilter->GetOutput(),
                 lBandMathFilter->GetOutput(), rBandMathFilter->GetOutput(), finalMaskFilter->GetOutput(), minimize, minDisp, maxDisp);


         }else
         {
             LISEMS_ERROR("Matching method not recognized, options are SSDDivMean, SSD, NCC and LP");
             throw 1;
         }

    //post-process incoherency filter
     //if (GetParameterInt("postproc.bij"))
           {
             bijectFilter = BijectionFilterType::New();
             // bijectFilter->SetDirectHorizontalDisparityMapInput(blockMatcherFilter->GetHorizontalDisparityOutput());
             bijectFilter->SetDirectHorizontalDisparityMapInput(blockMatcherFilterPointer->GetOutput(1));
             bijectFilter->SetReverseHorizontalDisparityMapInput(invBlockMatcherFilterPointer->GetOutput(1));
             // bijectFilter->SetTolerance(2);
             bijectFilter->SetMinHDisp(minDisp);
             bijectFilter->SetMaxHDisp(maxDisp);
             bijectFilter->SetMinVDisp(0);
             bijectFilter->SetMaxVDisp(0);

             // finalMaskFilter = BandMathFilterType::New();
             // finalMaskFilter->SetNthInput(0, lBandMathFilter->GetOutput(), "inmask");
             finalMaskFilter->SetNthInput(1, bijectFilter->GetOutput(), "lrrl");

     #ifdef OTB_MUPARSER_HAS_CXX_LOGICAL_OPERATORS
             finalMaskFilter->SetExpression("(inmask > 0 and lrrl > 0) ? 255 : 0");
     #else
             finalMaskFilter->SetExpression("if(inmask > 0 and lrrl > 0, 255, 0)");
     #endif

           }


         //median filter
     FloatImageType::Pointer hDispOutput    = subPixelFilterPointer->GetOutput(0);
           FloatImageType::Pointer finalMaskImage = finalMaskFilter->GetOutput();
           //if (GetParameterInt("postproc.med"))
           {
             MedianFilterType::Pointer hMedianFilter = MedianFilterType::New();
             hMedianFilter->SetInput(subPixelFilterPointer->GetOutput(0));
             hMedianFilter->SetRadius(2);
             hMedianFilter->SetIncoherenceThreshold(2.0);
             hMedianFilter->SetMaskInput(finalMaskFilter->GetOutput());
             hMedianFilter->UpdateOutputInformation();
             hDispOutput    = hMedianFilter->GetOutput();
             finalMaskImage = hMedianFilter->GetOutputMask();
             //m_Filters.push_back(hMedianFilter.GetPointer());
           }

    //disparity translation filter
     DisparityTranslateFilter::Pointer disparityTranslateFilter = DisparityTranslateFilter::New();
     disparityTranslateFilter->SetHorizontalDisparityMapInput(hDispOutput);
     disparityTranslateFilter->SetVerticalDisparityMapInput(subPixelFilterPointer->GetOutput(1));
     disparityTranslateFilter->SetInverseEpipolarLeftGrid(leftInverseDisplacement);
     disparityTranslateFilter->SetDirectEpipolarRightGrid(rightDisplacement);
     // disparityTranslateFilter->SetDisparityMaskInput()
     disparityTranslateFilter->SetLeftSensorImageInput(inleft);
     disparityTranslateFilter->SetNoDataValue(-32768);
     disparityTranslateFilter->UpdateOutputInformation();


     NoDataMaskFilterType::Pointer dispTranslateMaskFilter = NoDataMaskFilterType::New();
     dispTranslateMaskFilter->SetInput(disparityTranslateFilter->GetHorizontalDisparityMapOutput());
     dispTranslateMaskFilter->SetInsideValue(1);
     dispTranslateMaskFilter->SetOutsideValue(0);


     FloatImageType::Pointer hDispOutput2        = disparityTranslateFilter->GetHorizontalDisparityMapOutput();
           FloatImageType::Pointer vDispOutput2        = disparityTranslateFilter->GetVerticalDisparityMapOutput();
           FloatImageType::Pointer translatedMaskImage = dispTranslateMaskFilter->GetOutput();


          // if (GetParameterInt("postproc.med"))
                 {
                   MedianFilterType::Pointer hMedianFilter2 = MedianFilterType::New();
                   MedianFilterType::Pointer vMedianFilter2 = MedianFilterType::New();

                   // TODO JGT Check if medianfiltering is necessary after disparitytranslate
                   hMedianFilter2->SetInput(disparityTranslateFilter->GetHorizontalDisparityMapOutput());
                   hMedianFilter2->SetRadius(2);
                   hMedianFilter2->SetIncoherenceThreshold(2.0);
                   hMedianFilter2->SetMaskInput(dispTranslateMaskFilter->GetOutput());
                   hMedianFilter2->UpdateOutputInformation();
                   hDispOutput2        = hMedianFilter2->GetOutput();
                   translatedMaskImage = hMedianFilter2->GetOutputMask();

                   vMedianFilter2->SetInput(disparityTranslateFilter->GetVerticalDisparityMapOutput());
                   vMedianFilter2->SetRadius(2);
                   vMedianFilter2->SetIncoherenceThreshold(2.0);
                   vMedianFilter2->SetMaskInput(dispTranslateMaskFilter->GetOutput());
                   vMedianFilter2->UpdateOutputInformation();
                   vDispOutput2 = vMedianFilter2->GetOutput();

                 }


           double minElev = 0.0;
           double maxElev = 0.0;

         m_MultiDisparityTo3DFilterList->SetReferenceKeywordList(inleft->GetImageKeywordlist());
         m_MultiDisparityTo3DFilterList->SetNumberOfMovingImages(1);
         m_MultiDisparityTo3DFilterList->SetHorizontalDisparityMapInput(0, hDispOutput2);
         m_MultiDisparityTo3DFilterList->SetVerticalDisparityMapInput(0, vDispOutput2);
         m_MultiDisparityTo3DFilterList->SetMovingKeywordList(0, inright->GetImageKeywordlist());
         m_MultiDisparityTo3DFilterList->SetDisparityMaskInput(0, translatedMaskImage);
         m_MultiDisparityTo3DFilterList->UpdateOutputInformation();


         m_Multi3DMapToDEMFilter->SetElevationMin(minElev + underElev);
         // m_Multi3DMapToDEMFilter->SetNoDataValue(minElev);
         m_Multi3DMapToDEMFilter->SetElevationMax(maxElev + overElev);

         // Compute disparity mask
         BandMathFilterType::Pointer dispMaskFilter = BandMathFilterType::New();
         dispMaskFilter->SetNthInput(0, hDispOutput, "hdisp");
         dispMaskFilter->SetNthInput(1, finalMaskImage, "mask");
         std::ostringstream maskCondition;
         maskCondition << "(hdisp > " << minDisp << ") and (hdisp < " << maxDisp << ") and (mask>0)";
         /*if (IsParameterEnabled("postproc.metrict"))
         {
           dispMaskFilter->SetNthInput(2, subPixelFilterPointer->GetOutput(2), "metric");
           maskCondition << " and (metric ";
           if (minimize == true)
           {
             maskCondition << " < " << this->GetParameterFloat("postproc.metrict");
           }
           else
           {
             maskCondition << " > " << this->GetParameterFloat("postproc.metrict");
           }
           maskCondition << ")";
         }*/

         std::ostringstream maskFormula;

   #ifdef OTB_MUPARSER_HAS_CXX_LOGICAL_OPERATORS
         maskFormula << maskCondition.str() << " ? " << state << " : " << elseState;
   #else
         maskFormula << "if(" << maskCondition.str() << "," << state << "," << elseState << ")";
   #endif

         dispMaskFilter->SetExpression(maskFormula.str());
         disparityTranslateFilter->SetDisparityMaskInput(dispMaskFilter->GetOutput());
         m_Multi3DMapToDEMFilter->Set3DMapInput(0, m_MultiDisparityTo3DFilterList->GetOutput());

         // PARAMETER ESTIMATION
         FloatVectorImageType::IndexType start;
       start[0] = 0;
       start[1] = 0;
       m_Multi3DMapToDEMFilter->SetOutputStartIndex(start);

       FloatVectorImageType::SizeType size;
       size[0] = left->nrCols();//this->GetParameterInt("output.mode.user.sizex"); // X size
       size[1] = left->nrRows(); // Y size
       m_Multi3DMapToDEMFilter->SetOutputSize(size);

       FloatVectorImageType::SpacingType spacing;
       spacing[0] = left->cellSizeX();
       spacing[1] = left->cellSizeY();
       m_Multi3DMapToDEMFilter->SetOutputSpacing(spacing);

       FloatVectorImageType::PointType origin;
       origin[0] = left->west();
       origin[1] = left->north();
       m_Multi3DMapToDEMFilter->SetOutputOrigin(origin);


        m_Multi3DMapToDEMFilter->SetProjectionRef(left->GetProjection().GetWKT().toStdString());
        m_Multi3DMapToDEMFilter->SetDEMGridStep(1);//this->GetParameterFloat("output.res"));
        m_Multi3DMapToDEMFilter->SetCellFusionMode(otb::CellFusionMode::MEAN);
        m_Multi3DMapToDEMFilter->UpdateOutputInformation();


     cTMap * dem = left->GetCopy0();
     FloatImageType::Pointer out =m_Multi3DMapToDEMFilter->GetOutput();
     OtbImageToMap(dem,out);

    return dem;
}

#endif // RASTERSTEREO_H
