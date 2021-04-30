#ifndef RASTERREDUCE_H
#define RASTERREDUCE_H



#include <algorithm>
#include <QList>
#include "geo/raster/map.h"



inline cTMap * AS_MapTotal(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    double total = 0.0;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                total += Other->data[r][c];

            }
        }
    }

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = total;
            }
        }
    }


    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline float AS_MapTotalRed(cTMap * Other)
{

    double total = 0.0;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                total += Other->data[r][c];

            }
        }
    }

   return total;

}

inline cTMap * AS_MapAverage(cTMap * Other)
{

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    double total = 0.0;
    double n = 0.0;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                total += Other->data[r][c];
                n = n+ 1.0;

            }
        }
    }
    if(n > 0)
    {
        total = total/n;
    }


    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = total;
            }
        }
    }



    map->AS_IsSingleValue = Other->AS_IsSingleValue;
   return map;
}

inline float AS_MapAverageRed(cTMap * Other)
{

    double total = 0.0;
    double n = 0.0;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                total += Other->data[r][c];
                n = n+ 1.0;

            }
        }
    }
    if(n > 0)
    {
        total = total/n;
    }

   return total;

}

inline cTMap * AS_MapMaximum(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    float max = -1e31;
    double n = 0.0;
    bool found = false;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                max = std::max(max,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {

    }else {

    }

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = max;
            }
        }
    }


    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;
}

inline float AS_MapMaximumRed(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");
    float max = -1e31;
    double n = 0.0;
    bool found = false;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                max = std::max(max,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return max;
    }else {
        return max;
    }

}

inline cTMap * AS_MapMinimum(cTMap * Other)
{

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    float min = -1e31;
    double n = 0.0;
    bool found = false;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                min = std::min(min,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {

    }else {

    }

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = min;
            }
        }
    }



    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;
}

inline float AS_MapMinimumRed(cTMap * Other)
{
    float min = 1e31;
    double n = 0.0;
    bool found = false;

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                min = std::min(min,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return min;
    }else {
        return min;
    }


}


#endif // RASTERREDUCE_H
