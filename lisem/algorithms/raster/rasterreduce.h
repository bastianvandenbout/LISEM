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


    #pragma omp parallel for collapse(2) reduction(+:total)
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


    #pragma omp parallel for collapse(2)
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


    #pragma omp parallel for collapse(2) reduction(+:total)
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


    #pragma omp parallel for collapse(2) reduction(+:total,n)
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



    #pragma omp parallel for collapse(2)
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


    #pragma omp parallel for collapse(2) reduction(+:total,n)
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

    float maxv = -1e31;
    double n = 0.0;
    bool found = false;

    #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                maxv = std::max(maxv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {

    }else {
        pcr::setMV(maxv);
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
                map->data[r][c] = maxv;
            }
        }
    }


    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;
}

inline float AS_MapMaximumRed(cTMap * Other)
{
    float maxv = -1e31;
    double n = 0.0;
    bool found = false;

    #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                maxv = std::max(maxv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return maxv;
    }else {
        return maxv;
    }

}

inline cTMap * AS_MapMinimum(cTMap * Other)
{

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    float minv = 1e31;
    double n = 0.0;
    bool found = false;


    #pragma omp parallel for shared(found) collapse(2) reduction(min:minv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                minv = std::min(minv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {

    }else {
        pcr::setMV(minv);
    }

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = minv;
            }
        }
    }



    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;
}

inline float AS_MapMinimumRed(cTMap * Other)
{
    float minv = 1e31;
    double n = 0.0;
    bool found = false;

    #pragma omp parallel for shared(found) collapse(2) reduction(min:minv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                minv = std::min(minv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return minv;
    }else {
        return minv;
    }


}


#endif // RASTERREDUCE_H
