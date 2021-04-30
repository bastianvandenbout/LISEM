#ifndef RASTERSUPERVISEDCLASSIFY_H
#define RASTERSUPERVISEDCLASSIFY_H


#define OTB_OPENCV_3

// This example illustrates the use of the
// \doxygen{otb}{MachineLearningModel} class. This class allows the
// estimation of a classification model (supervised learning) from images. In this example, we will train an SVM
// with 4 classes. We start by including the appropriate header files.
// List sample generator
#include "otbListSampleGenerator.h"
#include "itkMinimumDecisionRule.h"
#include "itkSampleClassifierFilter.h"

#include "itkKdTreeBasedKmeansEstimator.h"

#include "itkKdTree.h"
#include "itkWeightedCentroidKdTreeGenerator.h"

#include "itkVector.h"
#include "itkListSample.h"
#include "otbNeighborhoodMajorityVotingImageFilter.h"
#include "itkNormalVariateGenerator.h"

// Extract a ROI of the vectordata
#include "otbVectorDataIntoImageProjectionFilter.h"

// SVM model Estimator
#include "otbSVMMachineLearningModel.h"

#include "itkMacro.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include <iostream>

#include "otbLibSVMMachineLearningModel.h"
#include "itkImageToListSampleFilter.h"
#include "otbImageClassificationFilter.h"

#include "otbImageFileWriter.h"

#include "itkUnaryFunctorImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkBinaryThresholdImageFilter.h"
// Image
#include "otbVectorImage.h"
#include "otbVectorData.h"

// Reader
#include "otbImageFileReader.h"
#include "otbVectorDataFileReader.h"


#include "otbMachineLearningModelFactory.h"
#include "otbImageClassificationFilter.h"

#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "geo/raster/map.h"
#include "otbKMeansImageClassificationFilter.h"

//do a supervised classification of a multi-spectral image based on training image (contians classes for a part of the image)
static cTMap * SupervisedClassifyMap(std::vector<cTMap *> in_bands,cTMap * train)
{


    cTMap * ret = train->GetCopy();

    //  const char* outputModelFileName = argv[4];

    //  We define the types for the input and training images. Even if the
    //  input image will be an RGB image, we can read it as a 3 component
    //  vector image. This simplifies the interfacing with OTB's SVM
    //  framework.
    using InputPixelType         = float;
    using TrainPixelType         = unsigned short;
    const unsigned int Dimension = 2;

    using InputImageType = otb::VectorImage<InputPixelType,Dimension>;

    using TrainingImageType = otb::Image<TrainPixelType, Dimension>;

    //  The \doxygen{otb}{LibSVMMachineLearningModel} class is templated over
    //  the input (features) and the training (labels) values.
    using ModelType = otb::LibSVMMachineLearningModel<InputPixelType, TrainPixelType>;


    //create input images
    InputImageType::Pointer imagei = InputImageType::New();
    TrainingImageType::Pointer imaget = TrainingImageType::New();

    unsigned long long xs = in_bands.at(0)->nrCols();
    unsigned long long ys = in_bands.at(0)->nrRows();

    int cols =in_bands.at(0)->nrCols();
    int rows = in_bands.at(0)->nrRows();

    const InputImageType::SizeType size = {{xs,ys}};
    const InputImageType::IndexType start = {{0,0}};

    InputImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imagei->SetRegions(region);
    InputImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing );
    int components = in_bands.size();
    imagei->SetNumberOfComponentsPerPixel(components);
    imagei->Allocate();

    const TrainingImageType::SizeType size2 = {{xs,ys}};
    const TrainingImageType::IndexType start2 = {{0,0}};

    TrainingImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    imaget->SetRegions(region2);
    TrainingImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    imaget->SetSpacing( spacing );
    imaget->SetNumberOfComponentsPerPixel(1);
    imaget->Allocate();

    //upload actual data to the images
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const InputImageType::IndexType pixelIndex = {{c,r}};
            InputImageType::PixelType val;
            val.SetSize(components);
            for(int i = 0; i < components; i++)
            {
                val.SetElement(i,in_bands.at(i)->data[r][c]);
            }
            imagei->SetPixel(pixelIndex,val);
        }
    }

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const TrainingImageType::IndexType pixelIndex = {{c,r}};
            TrainingImageType::PixelType val;
            val = (unsigned short) train->data[r][c];
            imaget->SetPixel(pixelIndex,val);
        }
    }

    //  The input data is contained in images. Only label values greater than 0
    //  shall be used, so we create two iterators to fill the input and target
    //  ListSamples.


    using ThresholdFilterType                = itk::BinaryThresholdImageFilter<TrainingImageType, TrainingImageType>;
    ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
    thresholder->SetInput(imaget);
    thresholder->SetLowerThreshold(1);
    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(1);

    using ImageToListSample       = itk::Statistics::ImageToListSampleFilter<InputImageType, TrainingImageType>;
    using ImageToTargetListSample = itk::Statistics::ImageToListSampleFilter<TrainingImageType, TrainingImageType>;

    ImageToListSample::Pointer imToList = ImageToListSample::New();
    imToList->SetInput(imagei);
    imToList->SetMaskImage(thresholder->GetOutput());
    imToList->SetMaskValue(1);
    imToList->Update();

    ImageToTargetListSample::Pointer imToTargetList = ImageToTargetListSample::New();
    imToTargetList->SetInput(imaget);
    imToTargetList->SetMaskImage(thresholder->GetOutput());
    imToTargetList->SetMaskValue(1);
    imToTargetList->Update();


    //  We can now instantiate the model and set its parameters.
    ModelType::Pointer svmModel = ModelType::New();
    svmModel->SetInputListSample(const_cast<ModelType::InputListSampleType*>(imToList->GetOutput()));
    svmModel->SetTargetListSample(const_cast<ModelType::TargetListSampleType*>(imToTargetList->GetOutput()));


    //  The model training procedure is triggered by calling the
    //  model's \code{Train} method.
    svmModel->Train();


    // We have now all the elements to create a classifier. The classifier
    // is templated over the sample type (the type of the data to be
    // classified) and the label type (the type of the output of the classifier).

    using ClassifierType = otb::ImageClassificationFilter<InputImageType, TrainingImageType>;

    ClassifierType::Pointer classifier = ClassifierType::New();

    // We set the classifier parameters : number of classes, SVM model,
    // the sample data. And we trigger the classification process by
    // calling the \code{Update} method.

    classifier->SetModel(svmModel);
    classifier->SetInput(imagei);

    classifier->Update();

    TrainingImageType::Pointer outimage = classifier->GetOutput();


    TrainPixelType  * data_out = reinterpret_cast<TrainPixelType *>(outimage->GetBufferPointer());

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
        }
    }


    return ret;
}

static cTMap * AS_SupervisedClassify(std::vector<cTMap *> in, cTMap * in_train)
{


    return SupervisedClassifyMap(in,in_train);
}




//do a supervised classification of a multi-spectral image based on training image (contians classes for a part of the image)
static cTMap * UnSupervisedClassifyMap(std::vector<cTMap *> in_bands,int nclasses, cTMap * mask)
{
    cTMap * ret = in_bands.at(0)->GetCopy();

    //  const char* outputModelFileName = argv[4];

    //  We define the types for the input and training images. Even if the
    //  input image will be an RGB image, we can read it as a 3 component
    //  vector image. This simplifies the interfacing with OTB's SVM
    //  framework.
    using InputPixelType         = float;
    using TrainPixelType         = unsigned short;
    const unsigned int Dimension = 2;

    using InputImageType = otb::VectorImage<InputPixelType,Dimension>;
    using MaskImageType = otb::Image<InputPixelType,Dimension>;

    using TrainingImageType = otb::Image<TrainPixelType, Dimension>;

    //  The \doxygen{otb}{LibSVMMachineLearningModel} class is templated over
    //  the input (features) and the training (labels) values.
    using ModelType = otb::KMeansImageClassificationFilter<InputImageType,TrainingImageType>;


    //create input images
    InputImageType::Pointer imagei = InputImageType::New();

    unsigned long long xs = in_bands.at(0)->nrCols();
    unsigned long long ys = in_bands.at(0)->nrRows();

    int cols =in_bands.at(0)->nrCols();
    int rows = in_bands.at(0)->nrRows();

    const InputImageType::SizeType size = {{xs,ys}};
    const InputImageType::IndexType start = {{0,0}};

    InputImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imagei->SetRegions(region);
    InputImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing );
    int components = in_bands.size();
    imagei->SetNumberOfComponentsPerPixel(components);
    imagei->Allocate();


    //create mask images
    MaskImageType::Pointer imagem = MaskImageType::New();

    unsigned long long xs2 = mask->nrCols();
    unsigned long long ys2 = in_bands.at(0)->nrRows();

    int cols2 = mask->nrCols();
    int rows2 = mask->nrRows();

    const MaskImageType::SizeType size2 = {{xs2,ys2}};
    const MaskImageType::IndexType start2 = {{0,0}};

    MaskImageType::RegionType region2;
    region2.SetSize(size2);
    region2.SetIndex(start2);
    imagem->SetRegions(region2);
    MaskImageType::SpacingType spacing2;
    spacing2[0] = 1; // spacing along X
    spacing2[1] = 1; // spacing along Y
    imagem->SetSpacing( spacing2 );
    int components2 = 1;
    imagem->SetNumberOfComponentsPerPixel(components2);
    imagem->Allocate();


    //upload actual data to the images
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const MaskImageType::IndexType pixelIndex = {{c,r}};
            MaskImageType::PixelType val;
            if(pcr::isMV(mask->data[r][c]))
            {
                val = 0.0;
            }
            else
            {
                val = mask->data[r][c];
            }
            imagem->SetPixel(pixelIndex,val);
        }
    }



    using ThresholdFilterType                = itk::BinaryThresholdImageFilter<MaskImageType, TrainingImageType>;
    ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
    thresholder->SetInput(imagem);
    thresholder->SetLowerThreshold(1);
    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(1);

    //upload actual data to the images
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const InputImageType::IndexType pixelIndex = {{c,r}};
            InputImageType::PixelType val;
            val.SetSize(components);
            for(int i = 0; i < components; i++)
            {
                val.SetElement(i,in_bands.at(i)->data[r][c]);
            }
            imagei->SetPixel(pixelIndex,val);
        }
    }

    //  We can now instantiate the model and set its parameters.
    ModelType::Pointer svmModel = ModelType::New();
    svmModel->SetInput(imagei);
    svmModel->SetInputMask(thresholder->GetOutput());

    const unsigned int sampleSize =
        ModelType::MaxSampleDimension;
      const unsigned int   parameterSize = nclasses * sampleSize;

      typedef ModelType::KMeansParametersType KMeansParametersType;
    KMeansParametersType parameters;

    parameters.SetSize(parameterSize);
    parameters.Fill(0);




    typedef itk::VariableLengthVector<double>
    MeasurementVectorType;
    typedef itk::Statistics::ListSample < MeasurementVectorType > SampleType ;
    SampleType::Pointer sample = SampleType :: New ();
    sample->SetMeasurementVectorSize(components);




    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < ret->nrCols(); c++)
        {
            if(!pcr::isMV(mask->data[r][c]))
            {
                if(mask->data[r][c] > 0.5)
                {
                    MeasurementVectorType mv ;
                    mv.SetSize(components);

                    for(int i = 0; i < components; i++)
                    {
                        mv.SetElement(i,in_bands.at(i)->data[r][c]);
                    }

                    sample->PushBack(mv);
                }

            }
        }
    }

    typedef itk::Statistics::WeightedCentroidKdTreeGenerator<SampleType>
    TreeGeneratorType;
    TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

    treeGenerator->SetSample(sample);
    treeGenerator->SetBucketSize(16);
    treeGenerator->Update();

    typedef TreeGeneratorType::KdTreeType TreeType ;
    typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
    EstimatorType::Pointer estimator = EstimatorType:: New ();
    EstimatorType::ParametersType initialMeans(nclasses * components);

    MeasurementVectorType mv ;


    //for each band get average, min and max

    QList<double> lmin;
    QList<double> lmax;
    QList<double> lavg;

    for(int i = 0; i <components; i++)
    {

        double min = -1e30;
        double max = 1e30;
        double avg = 0.0;
        double n = 0.0;
        bool first = true;
        for(int r = 0; r < rows; r++)
        {
            for(int c = 0; c < cols; c++)
            {
                if(mask->data[r][c] > 0.5 && !pcr::isMV(mask->data[r][c]))
                {
                    double val = in_bands.at(i)->data[r][c];
                    if(first)
                    {
                        min = val;
                        max = val;
                        avg += val;
                        n += 1.0;
                        first = false;
                    }else
                    {
                        min = std::min(min,val);
                        max = std::max(max,val);
                        avg += val;
                        n += 1.0;
                    }
                }
            }
        }

        if(n > 0.0)
        {
            lmin.append(min);
            lmax.append(max);
            lavg.append(avg/n);
        }else
        {
            lmin.append(0.0);
            lmax.append(0.0);
            lavg.append(0.0);
        }


    }

    for(int i = 0; i < nclasses; i++)
    {
        double spectral_slope = 1.0 - 2.0 * ((float)(i))/((float)(nclasses));

        for(int j = 0; j < components; j++)
        {
            initialMeans[i * components + j] = lavg.at(j) - (spectral_slope* (1.0 - 2.0 * ((float)(j))/((float)(components)))) > 0.0?(std::fabs((lmax.at(j) - lavg.at(j)))*spectral_slope *  (1.0 - 2.0 * ((float)(j))/((float)(components)))):(std::fabs((lmin.at(j) - lavg.at(j)))*spectral_slope * (1.0 - 2.0 * ((float)(j))/((float)(components))));
            std::cout << "initial means " << i << " "<< j << " "<< initialMeans[i * components + j] << "    " << lavg.at(j) << " " << lmax.at(j) << " " << lmin.at(j) <<  std::endl;
        }
    }
    estimator->SetParameters( initialMeans);
    estimator->SetKdTree ( treeGenerator -> GetOutput ());
    estimator->SetMaximumIteration(200);
    estimator->SetCentroidPositionChangesThreshold(0.0);
    estimator->StartOptimization();
    EstimatorType:: ParametersType estimatedMeans = estimator -> GetParameters();

    for (unsigned int i = 0; i < nclasses; ++i)
      {
      for (unsigned int j = 0; j <
           components; ++j)
        {
          std::cout << i << " " << j << " " << estimatedMeans(i * components + j) << std::endl;
            parameters[i * sampleSize + j] = estimatedMeans(i * components + j);
        }
      }

    svmModel->SetCentroids(parameters);


    svmModel->Update();

    TrainingImageType::Pointer outimage = svmModel->GetOutput();


    TrainPixelType  * data_out = reinterpret_cast<TrainPixelType *>(outimage->GetBufferPointer());

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            ret->data[r][c] = static_cast<float>(data_out[r*ret->nrCols() + c]);
        }
    }


    return ret;
}



static cTMap * AS_UnSupervisedClassify(std::vector<cTMap*> in, int classes, cTMap * mask)
{

    return UnSupervisedClassifyMap(in, classes, mask);
}




static std::vector<cTMap *> ClassToRGB(cTMap * clas)
{
    using PixelType                      = unsigned long;
    using ImageType                      = otb::Image<PixelType, 2>;

    using RGBPixelType                      = itk::RGBPixel<unsigned char>;
    using RGBImageType                      = otb::Image<RGBPixelType, 2>;
    using ColorMapFunctorType               = itk::Functor::ScalarToRGBPixelFunctor<unsigned long>;

    /*using ColorMapFilterType                = itk::UnaryFunctorImageFilter<ImageType, RGBImageType, ColorMapFunctorType>;
    ColorMapFilterType::Pointer colormapper = ColorMapFilterType::New();

    ImageType::Pointer imagei = ImageType::New();

    unsigned long long xs = clas->nrCols();
    unsigned long long ys = clas->nrRows();

    int cols = clas->nrCols();
    int rows = clas->nrRows();

    const ImageType::SizeType size = {{xs,ys}};
    const ImageType::IndexType start = {{0,0}};

    ImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);
    imagei->SetRegions(region);
    ImageType::SpacingType spacing;
    spacing[0] = 1; // spacing along X
    spacing[1] = 1; // spacing along Y
    imagei->SetSpacing( spacing );
    int components = 1;
    imagei->SetNumberOfComponentsPerPixel(components);
    imagei->Allocate();



    PixelType * data = reinterpret_cast<PixelType*>(imagei->GetBufferPointer());

    ImageType::PixelType valtemp;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            const ImageType::IndexType pixelIndex = {{c,r}};

            imagei->SetPixel(pixelIndex,clas->data[r][c]);
        }
    }

    colormapper->SetInput(imagei);
    colormapper->Update();*/

    int vl = 3;

    std::vector<cTMap *> retm;

    for(int i = 0; i < vl; i++)
    {
        retm.push_back(clas->GetCopy());
    }

    //RGBImageType::Pointer clusteredimage = colormapper->GetOutput();

    //RGBImageType::PixelType * data_out = (clusteredimage->GetBufferPointer());

    ColorMapFunctorType colormapper;

    for(int r = 0; r < clas->nrRows(); r++)
    {
        for(int c = 0; c < clas->nrCols(); c++)
        {
            if(!pcr::isMV(clas->data[r][c]))
            {
                RGBImageType::PixelType  pixel = colormapper((unsigned long) (clas->data[r][c]));
                for(int i = 0; i < vl; i++)
                {
                    retm.at(i)->data[r][c] = pixel.GetElement(i);
                }
            }else
            {
                for(int i = 0; i < vl; i++)
                {
                    pcr::setMV(retm.at(i)->data[r][c]);
                }
            }
        }
    }

    std::cout << "rgb done" << std::endl;
    return retm;


}


static std::vector<cTMap *> AS_ClassToRGB(cTMap * clas)
{


    std::vector<cTMap *> ret = ClassToRGB(clas);



    return ret;

}


static cTMap * RasterClassRegularize(cTMap * clas, float radius)
{

    cTMap * ret = clas->GetCopy();

    using IOLabelPixelType       = unsigned int; // 8 bits
    const unsigned int Dimension = 2;

    using IOLabelImageType = otb::Image<IOLabelPixelType, Dimension>;

    using NeighborhoodMajorityVotingFilterType = otb::NeighborhoodMajorityVotingImageFilter<IOLabelImageType>;

    using StructuringType = NeighborhoodMajorityVotingFilterType::KernelType;
    using RadiusType      = StructuringType::RadiusType;

    NeighborhoodMajorityVotingFilterType::Pointer NeighMajVotingFilter;
    NeighMajVotingFilter = NeighborhoodMajorityVotingFilterType::New();

    unsigned int     radiusX                  = radius;
    unsigned int     radiusY                  = radius;
    IOLabelPixelType noDataValue              = UINT_MAX;
    IOLabelPixelType undecidedValue           = UINT_MAX-1;

    StructuringType seBall;
     RadiusType      rad;

     rad[0] = radiusX;
     rad[1] = radiusY;

     seBall.SetRadius(rad);
     seBall.CreateStructuringElement();

     NeighMajVotingFilter->SetKernel(seBall);
     NeighMajVotingFilter->SetLabelForNoDataPixels(noDataValue);
     NeighMajVotingFilter->SetLabelForUndecidedPixels(undecidedValue);
     NeighMajVotingFilter->SetKeepOriginalLabelBool(false);


     IOLabelImageType::Pointer imagei = IOLabelImageType::New();

     unsigned long long xs = clas->nrCols();
     unsigned long long ys = clas->nrRows();

     int cols = clas->nrCols();
     int rows = clas->nrRows();

     const IOLabelImageType::SizeType size = {{xs,ys}};
     const IOLabelImageType::IndexType start = {{0,0}};

     IOLabelImageType::RegionType region;
     region.SetSize(size);
     region.SetIndex(start);
     imagei->SetRegions(region);
     IOLabelImageType::SpacingType spacing;
     spacing[0] = 1; // spacing along X
     spacing[1] = 1; // spacing along Y
     imagei->SetSpacing( spacing );
     int components = 1;
     imagei->SetNumberOfComponentsPerPixel(components);
     imagei->Allocate();



     IOLabelPixelType * data = reinterpret_cast<IOLabelPixelType*>(imagei->GetBufferPointer());

     IOLabelImageType::PixelType valtemp;

     for(int r = 0; r < rows; r++)
     {
         for(int c = 0; c < cols; c++)
         {
             const IOLabelImageType::IndexType pixelIndex = {{c,r}};

             if(pcr::isMV(clas->data[r][c]))
             {
                 imagei->SetPixel(pixelIndex,UINT_MAX);
             }else
             {
                 imagei->SetPixel(pixelIndex,clas->data[r][c]);
             }
         }
     }

     NeighMajVotingFilter->SetInput(imagei);
     NeighMajVotingFilter->Update();

     IOLabelImageType::Pointer clusteredimage = NeighMajVotingFilter->GetOutput();

     IOLabelImageType::PixelType * data_out = (clusteredimage->GetBufferPointer());

     for(int r = 0; r < clas->nrRows(); r++)
     {
         for(int c = 0; c < clas->nrCols(); c++)
         {
             if(!pcr::isMV(clas->data[r][c]))
             {
                 IOLabelImageType::PixelType  pixel = data_out[r*clas->nrCols() + c];

                 if(pixel != undecidedValue)
                 {
                     ret->data[r][c] = pixel;
                 }
             }
         }
     }

    return ret;

}

static cTMap * AS_RasterClassRegularize(cTMap * clas, float radius)
{
    return RasterClassRegularize(clas, radius);
}


#endif // RASTERSUPERVISEDCLASSIFY_H
