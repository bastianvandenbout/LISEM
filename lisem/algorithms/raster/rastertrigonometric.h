#ifndef RASTERTRIGONOMETRIC_H
#define RASTERTRIGONOMETRIC_H


#include <algorithm>
#include <QList>
#include "geo/raster/map.h"


inline cTMap * AS_Mapsin(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = sinf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mapcos(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = cosf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Maptan(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = tanf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline cTMap * AS_Mapasin(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = asinf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mapacos(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = acosf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mapatan(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = atanf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mapatan2(cTMap * Other,cTMap * Other2)
{
    if((Other->AS_IsSingleValue &&  Other2->AS_IsSingleValue) ||(!(Other->AS_IsSingleValue) && !( Other2->AS_IsSingleValue)))
    {
        if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() ==  Other2->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = std::atan2(Other->data[r][c], Other2->data[r][c]);
                }
            }
        }

        map->AS_IsSingleValue = (Other->AS_IsSingleValue &&  Other2->AS_IsSingleValue);
        return map;


    }else
    {
        cTMap * map1 = Other;
        cTMap * map2 =  Other2;

        if(map1->AS_IsSingleValue)
        {
            cTMap * temp = map2;
            map2 = map1;
            map1 = temp;
        }

        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size());
        cTMap *nmap = new cTMap(std::move(raster_data),map1->projection(),"");

        float vother = map2->data[0][0];

        cTMap * target = nmap;

        for(int r = 0; r < map1->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->data[r][c]) )
                {
                    pcr::setMV(target->data[r][c]);
                }else
                {
                    target->data[r][c] = std::atan2(Other->data[r][c],vother);
                }
            }
        }
        target->AS_IsSingleValue = false;
        return target;
    }

}

inline cTMap * AS_Mapsinh(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = sinhf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}


inline cTMap * AS_Mapcosh(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = coshf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Maptanh(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = tanhf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}



#endif // RASTERTRIGONOMETRIC_H
