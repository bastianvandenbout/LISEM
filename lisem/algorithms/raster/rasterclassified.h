#ifndef RASTERCLASSIFIED_H
#define RASTERCLASSIFIED_H


#include "otbMultiChannelExtractROI.h"
#include "otbExtractROI.h"

#include "otbStreamingStatisticsImageFilter.h"
#include "otbSystem.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkChangeLabelImageFilter.h"

#include "otbTileImageFilter.h"

#include <time.h>
#include <algorithm>
#include <climits>

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbStandardWriterWatcher.h"

#include "rasterotbcommon.h"

#include <iostream>
#include "raster/rastercommon.h"
#include "geo/raster/map.h"



inline cTMap * AS_RegionMerge(cTMap * Class, std::vector<cTMap *> Similarity, float area_threshold)
{
    typedef otb::VectorImage<float,2>         ImageType;
    typedef ImageType::InternalPixelType      ImagePixelType;
    typedef otb::Image<unsigned int,2>        LabelImageType;
    typedef LabelImageType::InternalPixelType LabelImagePixelType;

    typedef otb::MultiChannelExtractROI<ImagePixelType, ImagePixelType> MultiChannelExtractROIFilterType;
    typedef otb::ExtractROI<LabelImagePixelType, LabelImagePixelType>   ExtractROIFilterType;

    typedef otb::StreamingStatisticsImageFilter<LabelImageType> StatisticsImageFilterType;

    typedef itk::ImageRegionConstIterator<LabelImageType> LabelImageIterator;
    typedef itk::ImageRegionConstIterator<ImageType>      ImageIterator;

    typedef itk::ChangeLabelImageFilter<LabelImageType, LabelImageType> ChangeLabelImageFilterType;
    typedef otb::TileImageFilter<LabelImageType> TileImageFilterType;



    unsigned int minSize = area_threshold/std::fabs(Class->cellSizeX()*Class->cellSizeY());

    unsigned long sizeTilesX = 500;
    unsigned long sizeTilesY = 500;

        // Acquisition of the input image dimensions
        ImageType::Pointer imageIn = ImageType::New();

        MapToOtbVImage(Similarity,imageIn);

        imageIn->UpdateOutputInformation();
        unsigned long sizeImageX = imageIn->GetLargestPossibleRegion().GetSize()[0], sizeImageY = imageIn->GetLargestPossibleRegion().GetSize()[1];
        unsigned int  numberOfComponentsPerPixel = imageIn->GetNumberOfComponentsPerPixel();

        LabelImageType::Pointer labelIn = LabelImageType::New();
        MapToOtbImageUI(Class,labelIn);

        StatisticsImageFilterType::Pointer stats = StatisticsImageFilterType::New();
        stats->SetInput(labelIn);
        stats->GetStreamer()->SetAutomaticAdaptativeStreaming(128);
        stats->Update();
        unsigned int regionCount = stats->GetMaximum();

        std::vector<unsigned int> nbPixels;
        nbPixels.clear();
        nbPixels.resize(regionCount + 1);
        for (LabelImagePixelType curLabel = 1; curLabel <= regionCount; ++curLabel)
          nbPixels[curLabel]              = 0;

        ImageType::PixelType defaultValue(numberOfComponentsPerPixel);
        defaultValue.Fill(0);

        std::vector<ImageType::PixelType> sum(regionCount + 1, defaultValue);

        unsigned int nbTilesX = sizeImageX / sizeTilesX + (sizeImageX % sizeTilesX > 0 ? 1 : 0);
        unsigned int nbTilesY = sizeImageY / sizeTilesY + (sizeImageY % sizeTilesY > 0 ? 1 : 0);

        for (unsigned int row = 0; row < nbTilesY; row++)
          for (unsigned int column = 0; column < nbTilesX; column++)
          {
            unsigned long startX = column * sizeTilesX;
            unsigned long startY = row * sizeTilesY;
            unsigned long sizeX  = std::min(sizeTilesX, sizeImageX - startX);
            unsigned long sizeY  = std::min(sizeTilesY, sizeImageY - startY);

            // Tiles extraction of the input image
            MultiChannelExtractROIFilterType::Pointer imageROI = MultiChannelExtractROIFilterType::New();
            imageROI->SetInput(imageIn);
            imageROI->SetStartX(startX);
            imageROI->SetStartY(startY);
            imageROI->SetSizeX(sizeX);
            imageROI->SetSizeY(sizeY);
            imageROI->Update();

            // Tiles extraction of the segmented image
            ExtractROIFilterType::Pointer labelImageROI = ExtractROIFilterType::New();
            labelImageROI->SetInput(labelIn);
            labelImageROI->SetStartX(startX);
            labelImageROI->SetStartY(startY);
            labelImageROI->SetSizeX(sizeX);
            labelImageROI->SetSizeY(sizeY);
            labelImageROI->Update();

            // Sums calculation for the mean calculation per label
            LabelImageIterator itLabel(labelImageROI->GetOutput(), labelImageROI->GetOutput()->GetLargestPossibleRegion());
            ImageIterator      itImage(imageROI->GetOutput(), imageROI->GetOutput()->GetLargestPossibleRegion());

            for (itLabel.GoToBegin(), itImage.GoToBegin(); !itLabel.IsAtEnd(); ++itLabel, ++itImage)
            {
              nbPixels[itLabel.Value()]++;
              for (unsigned int comp = 0; comp < numberOfComponentsPerPixel; ++comp)
              {
                sum[itLabel.Value()][comp] += itImage.Get()[comp];
              }
            }
          }

        // LUT creation for the final relabelling
        std::vector<LabelImagePixelType> LUT;
        LUT.clear();
        LUT.resize(regionCount + 1);
        for (LabelImagePixelType curLabel = 1; curLabel <= regionCount; ++curLabel)
        {
          LUT[curLabel] = curLabel;
        }

        for (unsigned int size = 1; size < minSize; size++)
        {
          // LUTtmp creation in order to modify the LUT only at the end of the pass
          std::vector<LabelImagePixelType> LUTtmp;
          LUTtmp.clear();
          LUTtmp.resize(regionCount + 1);
          for (LabelImagePixelType curLabel = 1; curLabel <= regionCount; ++curLabel)
          {
            LUTtmp[curLabel] = LUT[curLabel];
          }

          for (unsigned int row = 0; row < nbTilesY; row++)
          {
            for (unsigned int column = 0; column < nbTilesX; column++)
            {
              std::set<int> minLabel, edgeLabel, labelMerged;
              std::map<int, std::set<int>> adjMap;

              unsigned long startX = column * sizeTilesX, startY = row * sizeTilesY;
              unsigned long sizeX = std::min(sizeTilesX + size + 1, sizeImageX - startX), sizeY = std::min(sizeTilesY + size + 1, sizeImageY - startY);

              ExtractROIFilterType::Pointer labelImageROI = ExtractROIFilterType::New();
              labelImageROI->SetInput(labelIn);
              labelImageROI->SetStartX(startX);
              labelImageROI->SetStartY(startY);
              labelImageROI->SetSizeX(sizeX);
              labelImageROI->SetSizeY(sizeY);
              labelImageROI->Update();

              LabelImageType::IndexType pixelIndex;

              //"Adjacency map" creation for the region with nbPixels=="size"
              for (pixelIndex[0] = 0; pixelIndex[0] < static_cast<long>(sizeX); ++pixelIndex[0])
                for (pixelIndex[1] = 0; pixelIndex[1] < static_cast<long>(sizeY); ++pixelIndex[1])
                {
                  LabelImagePixelType curLabel = labelImageROI->GetOutput()->GetPixel(pixelIndex);

                  if (labelMerged.count(LUT[curLabel]))
                  {
                    edgeLabel.insert(LUT[curLabel]);
                  }
                  if ((pixelIndex[0] == 0) && (startX != 0))
                  {
                    edgeLabel.insert(LUT[curLabel]);
                  }
                  if ((pixelIndex[1] == 0) && (startY != 0))
                  {
                    edgeLabel.insert(LUT[curLabel]);
                  }

                  if (pixelIndex[0] == static_cast<long>(sizeX) - 1)
                  {
                    if (startX != (nbTilesX - 1) * sizeTilesX)
                      edgeLabel.insert(LUT[curLabel]);
                  }
                  else
                  {
                    ++pixelIndex[0];
                    LabelImagePixelType adjLabelX = labelImageROI->GetOutput()->GetPixel(pixelIndex);
                    --pixelIndex[0];

                    if (LUT[adjLabelX] != LUT[curLabel])
                    {
                      if ((nbPixels[LUT[curLabel]] > 0) && (nbPixels[LUT[curLabel]] == size))
                      {
                        adjMap[LUT[curLabel]].insert(LUT[adjLabelX]);
                        minLabel.insert(LUT[curLabel]);
                      }
                      if ((nbPixels[LUT[adjLabelX]] > 0) && (nbPixels[LUT[adjLabelX]] == size))
                      {
                        adjMap[LUT[adjLabelX]].insert(LUT[curLabel]);
                        minLabel.insert(LUT[adjLabelX]);
                      }
                    }
                  }
                  if (pixelIndex[1] == static_cast<long>(sizeY) - 1)
                  {
                    if (startY != (nbTilesY - 1) * sizeTilesY)
                      edgeLabel.insert(LUT[curLabel]);
                  }
                  else
                  {
                    ++pixelIndex[1];
                    LabelImagePixelType adjLabelY = labelImageROI->GetOutput()->GetPixel(pixelIndex);
                    --pixelIndex[1];
                    if (LUT[adjLabelY] != LUT[curLabel])
                    {
                      if ((nbPixels[LUT[curLabel]] > 0) && (nbPixels[LUT[curLabel]] == size))
                      {
                        adjMap[LUT[curLabel]].insert(LUT[adjLabelY]);
                        minLabel.insert(LUT[curLabel]);
                      }
                      if ((nbPixels[LUT[adjLabelY]] > 0) && (nbPixels[LUT[adjLabelY]] == size))
                      {
                        adjMap[LUT[adjLabelY]].insert(LUT[curLabel]);
                        minLabel.insert(LUT[adjLabelY]);
                      }
                    }
                  }
                }

              // Searching the "nearest" region
              for (std::set<int>::iterator itMinLabel = minLabel.begin(); itMinLabel != minLabel.end(); ++itMinLabel)
              {
                LabelImagePixelType curLabel = *itMinLabel, adjLabel(0);
                double              err      = itk::NumericTraits<double>::max();
                if (edgeLabel.count(curLabel) == 0)
                {
                  if (nbPixels[curLabel] == size)
                  {
                    edgeLabel.insert(curLabel);
                    for (std::set<int>::iterator itAdjLabel = (adjMap[curLabel]).begin(); itAdjLabel != (adjMap[curLabel]).end(); ++itAdjLabel)
                    {
                      double              tmpError = 0;
                      LabelImagePixelType tmpLabel = *itAdjLabel;
                      if (tmpLabel != curLabel)
                      {
                        for (unsigned int comp = 0; comp < numberOfComponentsPerPixel; ++comp)
                        {
                          double curComp = static_cast<double>(sum[curLabel][comp]) / nbPixels[curLabel];
                          int    tmpComp = static_cast<double>(sum[tmpLabel][comp]) / nbPixels[tmpLabel];
                          tmpError += (curComp - tmpComp) * (curComp - tmpComp);
                        }
                        if (tmpError < err)
                        {
                          err      = tmpError;
                          adjLabel = tmpLabel;
                        }
                      }
                    }

                    // Fusion of the two regions
                    if (adjLabel != curLabel)
                    {
                      unsigned int curLabelLUT = curLabel, adjLabelLUT = adjLabel;
                      while (LUTtmp[curLabelLUT] != curLabelLUT)
                      {
                        curLabelLUT = LUTtmp[curLabelLUT];
                      }
                      while (LUTtmp[adjLabelLUT] != adjLabelLUT)
                      {
                        adjLabelLUT = LUTtmp[adjLabelLUT];
                      }
                      if (curLabelLUT < adjLabelLUT)
                      {
                        LUTtmp[adjLabelLUT] = curLabelLUT;
                      }
                      else
                      {
                        LUTtmp[LUTtmp[curLabelLUT]] = adjLabelLUT;
                        LUTtmp[curLabelLUT]         = adjLabelLUT;
                      }
                    }
                  }
                }
              }

              for (LabelImagePixelType label = 1; label < regionCount + 1; ++label)
              {
                LabelImagePixelType can = label;
                while (LUTtmp[can] != can)
                {
                  can = LUTtmp[can];
                }
                LUTtmp[label] = can;
              }
            }
          }

          for (LabelImagePixelType label = 1; label < regionCount + 1; ++label)
          {
            LUT[label] = LUTtmp[label];
            if ((nbPixels[label] != 0) && (LUT[label] != label))
            {
              nbPixels[LUT[label]] += nbPixels[label];
              nbPixels[label] = 0;
              for (unsigned int comp = 0; comp < numberOfComponentsPerPixel; ++comp)
              {
                sum[LUT[label]][comp] += sum[label][comp];
              }
            }
          }
        }
        // Relabelling
        ChangeLabelImageFilterType::Pointer m_ChangeLabelFilter = ChangeLabelImageFilterType::New();
        m_ChangeLabelFilter->SetInput(labelIn);
        for (LabelImagePixelType label = 1; label < regionCount + 1; ++label)
        {
          if (label != LUT[label])
          {
            m_ChangeLabelFilter->SetChange(label, LUT[label]);
          }
        }

        cTMap * ret = Class->GetCopy();

        ChangeLabelImageFilterType::OutputImageType::Pointer pout = m_ChangeLabelFilter->GetOutput();

        OtbImageUIToMap(ret,pout);


    return ret;
}


inline static cTMap *AS_ClassMerge(cTMap * Class, std::vector<cTMap*> in, float minsize)
{

    cTMap* ret = AS_RegionMerge(Class,in,minsize);

    return ret;
}

inline static cTMap *AS_ClassMerge(cTMap * Class, float minsize)
{
    std::vector<cTMap *> in;
    in.push_back(Class->GetCopy0());
    cTMap *ret = AS_RegionMerge(Class,in,minsize);

    delete in.at(0);
    return ret;

}











inline cTMap * AS_MapAreaArea(cTMap * Other)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += cellarea;
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    vals.at(i) += cellarea;
                }
            }

        }
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = vals.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;

}

inline cTMap * AS_MapAreaTotal(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += Other2->data[r][c];
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    vals.at(i) += Other2->data[r][c];
                }
            }

        }
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = vals.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;


}
inline cTMap * AS_MapAreaAverage(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;
    std::vector<float> ns;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    ns.push_back(0.0);
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }
            }

        }
    }

    for(int i= 0; i < classes.size(); i++)
    {
            vals.at(i) = vals.at(i)/std::max(1.0f,ns.at(i));
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = vals.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;


}


inline cTMap * AS_MapAreaVariance(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;
    std::vector<float> variances;
    std::vector<float> ns;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(0.0);
                    ns.push_back(0.0);
                    variances.push_back(0.0);
                    vals.at(i) += Other2->data[r][c];
                    ns.at(i) += 1.0f;
                }
            }

        }
    }

    for(int i= 0; i < classes.size(); i++)
    {
            vals.at(i) = vals.at(i)/std::max(1.0f,ns.at(i));
    }


    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    float mean = vals.at(i);
                    variances.at(i) += (Other2->data[r][c] - mean) * (Other2->data[r][c] - mean);

                }
            }

        }
    }

    for(int i= 0; i < classes.size(); i++)
    {
            variances.at(i) = variances.at(i)/std::max(1.0f,ns.at(i));
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = variances.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;


}

inline cTMap * AS_MapAreaMaximum(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i) = std::max(vals.at(i),Other2->data[r][c]);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(-1e30);
                    vals.at(i) = std::max(vals.at(i),Other2->data[r][c]);
                }
            }

        }
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = vals.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;


}
inline cTMap * AS_MapAreaMinimum(cTMap * Other,cTMap * Other2)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    if(Other2->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for AreaArea (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() == Other2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;

    float cellarea = map->cellSize() * map->cellSize();

    //we need to find averages for each class
    //first, we find all the classes
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    vals.at(i)  = std::min(vals.at(i),Other2->data[r][c]);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(1e30);
                    vals.at(i)  = std::min(vals.at(i),Other2->data[r][c]);
                }
            }

        }
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        break;
                    }

                }
                map->data[r][c] = vals.at(i);
            }else {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;

}



inline cTMap * AS_Order()
{

    return nullptr;
}

inline cTMap * AS_Clump(cTMap * Other, float threshold = 0.0, bool adjust_threshold = false)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Clump (values) can not be non-spatial");
        throw 1;
    }
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(map->data[ro][co]);
        }
    }

    int classn = 1;
    float val = 0.0;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {


            if(!pcr::isMV(Other->data[ro][co]) && pcr::isMV(map->data[ro][co]))
            {
                {
                    LDD_LINKEDLIST * list = GetListRoot(Other,ro,co);
                    val =Other->data[ro][co];

                    while(list != nullptr)
                    {
                        int r = list->rowNr;
                        int c = list->colNr;

                        map->data[r][c] = classn;


                        if(adjust_threshold)
                        {
                            list = ListReplaceFirstByEQNBF2(Other,map,list, val, threshold);

                        }else
                        {
                            list = ListReplaceFirstByEQNBF(Other,map,list, val, threshold);

                        }
                    }

                    classn++;
                }

            }

        }
    }

    return map;

}



inline cTMap * AS_UniqueID(cTMap * Other)
{
    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for AreaArea (Class) can not be non-spatial");
        throw 1;
    }
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    int classn = 1;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {

        for(int co = 0; co < map->data.nr_cols();co++)
        {

            if(!pcr::isMV(Other->data[ro][co]))
            {
                {
                    map->data[ro][co] = classn;
                    classn++;
                }

            }else {
                pcr::setMV(map->data[ro][co]);
            }

        }
    }

    return map;
}


inline BoundingBox AS_GetClassPixelExtent(cTMap * map, int mclass)
{

    bool found = false;
    int minx = 0;
    int maxx = 0;
    int maxy = 0;
    int miny = 0;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {

        for(int co = 0; co < map->data.nr_cols();co++)
        {

            if(!pcr::isMV(map->data[ro][co]))
            {
                if(((int)map->data[ro][co]+0.5f) == mclass)
                {
                    if(!found)
                    {
                        found = true;
                        minx = co;
                        maxx = co;
                        miny = ro;
                        maxy = ro;

                    }else
                    {
                        minx = std::min(minx,co);
                        maxx = std::max(maxx,co);
                        miny = std::min(miny,ro);
                        maxy = std::max(maxy,ro);
                    }
                }

            }
        }
    }

    return BoundingBox(minx,maxx,miny,maxy);
}

inline BoundingBox AS_GetClassExtent(cTMap * Other, int mclass)
{
    bool found = false;
    int minx = 0;
    int maxx = 0;
    int maxy = 0;
    int miny = 0;

    for(int ro = 0; ro < Other->data.nr_rows();ro++)
    {

        for(int co = 0; co < Other->data.nr_cols();co++)
        {

            if(!pcr::isMV(Other->data[ro][co]))
            {
                if(((int)Other->data[ro][co]+0.5f) == mclass)
                {
                    if(!found)
                    {
                        found = true;
                        minx = co;
                        maxx = co;
                        miny = ro;
                        maxy = ro;

                    }else
                    {
                        minx = std::min(minx,co);
                        maxx = std::max(maxx,co);
                        miny = std::min(miny,ro);
                        maxy = std::max(maxy,ro);
                    }
                }

            }
        }
    }

    return BoundingBox(Other->west() + Other->cellSizeX() * minx,Other->west() + Other->cellSizeX() * maxx,Other->north() + Other->cellSizeY() * miny,Other->north() + Other->cellSizeY() *maxy);
}



#endif // RASTERCLASSIFIED_H
