#ifndef RASTERSAR_H
#define RASTERSAR_H

#include "geo/raster/map.h"
#include "error.h"
#include "QString"

#include "otbSarRadiometricCalibrationToImageFilter.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "rasterotbcommon.h"

typedef otb::Image<float,2> FloatImageType;
typedef std::complex<float>  FloatComplexPixelType;
typedef std::complex<double> DoubleComplexPixelType;

typedef otb::Image< FloatComplexPixelType, 2 >  ComplexFloatImageType;
typedef otb::Image< DoubleComplexPixelType, 2 > ComplexDoubleImageType;
typedef otb::VectorImage<FloatComplexPixelType, 2 >   ComplexFloatVectorImageType;
typedef otb::VectorImage< DoubleComplexPixelType, 2 > ComplexDoubleVectorImageType;

typedef otb::SarRadiometricCalibrationToImageFilter<ComplexFloatImageType, FloatImageType> CalibrationFilterType;



inline static cTMap * AS_SARCalibrate(std::vector<cTMap*> rad, bool lut, bool noise)
{

    CalibrationFilterType::Pointer m_CalibrationFilter;

    ComplexFloatImageType::Pointer floatComplexImage = ComplexFloatImageType::New();

    MapToOtbCImage(rad,floatComplexImage);

    m_CalibrationFilter = CalibrationFilterType::New();
    m_CalibrationFilter->SetInput(floatComplexImage);
    m_CalibrationFilter->SetEnableNoise(!noise);
    m_CalibrationFilter->SetLookupSelected(lut);

    cTMap * ret = rad.at(0)->GetCopy0();
    FloatImageType::Pointer out = m_CalibrationFilter->GetOutput();
    OtbImageToMap(ret,out);
    return ret;
}

inline static cTMap * AS_SARCalculate(std::vector<cTMap*> rad, QString value)
{


    if(value == "modulus")
    {

    }else if(value == "phase")
    {


    }else if(value == "")
    {


    }

    return nullptr;
}

inline static cTMap * AS_CORegister()
{


    return nullptr;
}

inline static cTMap * AS_Interferogram()
{


    return nullptr;
}

inline static cTMap * AS_PhaseUnwrap()
{


    return nullptr;
}
#endif // RASTERSAR_H
