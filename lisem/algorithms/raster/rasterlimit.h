#ifndef RASTERLIMIT_H
#define RASTERLIMIT_H


#include <algorithm>
#include <QList>
#include "geo/raster/map.h"



inline float AS_GetMVVal()
{
    float x;
    pcr::setMV(x);
    return x;
}

inline float AS_IsValMV(float x)
{
    if(pcr::isMV(x))
    {
        return 1.0;
    }else {

        return 0.0;
    }
}

inline cTMap * AS_IsMV(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                map->data[r][c] = 1.0;
            }else
            {
                map->data[r][c] = 0.0;
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}


inline cTMap * AS_SetMV(cTMap * Other ,float Other2)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV( Other2))
            {
                pcr::setMV(map->data[r][c]);
            }else if( LISEM_ASMAP_BOOLFROMFLOAT(Other2))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = Other->data[r][c];
            }
        }
    }

    map->AS_IsSingleValue = (Other->AS_IsSingleValue);
    return map;


}

inline cTMap * AS_SetMV(cTMap * Other,cTMap * Other2)
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
                if(pcr::isMV( Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else if( LISEM_ASMAP_BOOLFROMFLOAT(Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = Other->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = (Other->AS_IsSingleValue &&  Other2->AS_IsSingleValue);
        return map;


    }else
    {

        cTMap * map1 = Other;
        cTMap * map2 =  Other2;

        if(map2->AS_IsSingleValue)
        {

            MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
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
                        if(pcr::isMV( vother))
                        {
                            pcr::setMV(target->data[r][c]);
                        }else if( LISEM_ASMAP_BOOLFROMFLOAT(vother))
                        {
                            pcr::setMV(target->data[r][c]);
                        }else
                        {
                            target->data[r][c] = Other->data[r][c];
                        }

                    }
                }
            }
            target->AS_IsSingleValue = false;
            return target;
        }else {

            MaskedRaster<float> raster_data(map2->data.nr_rows(), map2->data.nr_cols(), map2->data.north(), map2->data.west(), map2->data.cell_size(), map2->data.cell_sizeY());
            cTMap *nmap = new cTMap(std::move(raster_data),map2->projection(),"");

            float vother = map1->data[0][0];

            cTMap * target = nmap;

            for(int r = 0; r < nmap->data.nr_rows();r++)
            {
                for(int c = 0; c < nmap->data.nr_cols();c++)
                {
                    if(pcr::isMV(vother) )
                    {
                        pcr::setMV(target->data[r][c]);
                    }else
                    {
                        if(pcr::isMV( map2->data[r][c]))
                        {
                            pcr::setMV(target->data[r][c]);
                        }else if( LISEM_ASMAP_BOOLFROMFLOAT(map2->data[r][c]))
                        {
                            pcr::setMV(target->data[r][c]);
                        }else
                        {
                            target->data[r][c] = vother;
                        }

                    }
                }
            }
            target->AS_IsSingleValue = false;
            return target;

        }

    }

}

inline cTMap * AS_EdgeRight(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(), Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) )
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                if( c == Other->nrCols() - 1)
                {
                    map->data[r][c] = 1.0;
                }else
                {
                    if(pcr::isMV(Other->data[r][c+1]))
                    {
                        map->data[r][c] = 1.0;
                    }else
                    {
                        map->data[r][c] = 0.0;
                    }
                }
            }
        }
    }

    return map;


}

inline cTMap * AS_EdgeLeft(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(), Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) )
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                if(c == 0 )
                {
                    map->data[r][c] = 1.0;
                }else
                {
                    if(pcr::isMV(Other->data[r][c-1]))
                    {
                        map->data[r][c] = 1.0;
                    }else
                    {
                        map->data[r][c] = 0.0;
                    }
                }
            }
        }
    }

    return map;


}

inline cTMap * AS_EdgeTop(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(), Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) )
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                if(r == 0 )
                {
                    map->data[r][c] = 1.0;
                }else
                {
                     if(pcr::isMV(Other->data[r-1][c]))
                    {
                        map->data[r][c] = 1.0;
                    }else
                    {
                        map->data[r][c] = 0.0;
                    }
                }
            }
        }
    }

    return map;


}

inline cTMap * AS_EdgeBottom(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(), Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) )
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                if(r == Other->nrRows() - 1)
                {
                    map->data[r][c] = 1.0;
                }else
                {
                    if(pcr::isMV(Other->data[r+1][c]))
                    {
                        map->data[r][c] = 1.0;
                    }else
                    {
                        map->data[r][c] = 0.0;
                    }
                }
            }
        }
    }

    return map;


}

inline cTMap * AS_Edge(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(), Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) )
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                if(r == 0 || c == 0 || r == Other->nrRows() - 1 || c == Other->nrCols() - 1)
                {
                    map->data[r][c] = 1.0;
                }else
                {
                    if(pcr::isMV(Other->data[r][c-1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r][c+1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r][c+1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r+1][c-1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r-1][c-1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r+1][c+1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r-1][c+1]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r-1][c]))
                    {
                        map->data[r][c] = 1.0;
                    }else if(pcr::isMV(Other->data[r+1][c]))
                    {
                        map->data[r][c] = 1.0;
                    }else
                    {
                        map->data[r][c] = 0.0;
                    }
                }
            }
        }
    }

    return map;

}


inline cTMap * AS_Cover(float Other, float Other2)
{
    MaskedRaster<float> raster_data(1,1,0.0,0.0,1.0);
    cTMap *map = new cTMap(std::move(raster_data),"","");

    map->data[0][0] = pcr::isMV(Other)? Other2:Other;

    map->AS_IsSingleValue = true;
    return map;

}

inline cTMap * AS_Cover(float Other,cTMap * Other2)
{
    MaskedRaster<float> raster_data(Other2->data.nr_rows(), Other2->data.nr_cols(), Other2->data.north(), Other2->data.west(), Other2->data.cell_size(), Other2->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other2->projection(),"");

    for(int r = 0; r < Other2->data.nr_rows();r++)
    {
        for(int c = 0; c < Other2->data.nr_cols();c++)
        {
            if(pcr::isMV(Other) && pcr::isMV( Other2->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(!pcr::isMV(Other))
            {
                map->data[r][c] = Other;
            }else {
                map->data[r][c] = Other2->data[r][c];
            }
        }
    }

    map->AS_IsSingleValue = (Other2->AS_IsSingleValue);
    return map;
}

inline cTMap * AS_Cover(cTMap * Other, float Other2)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) && pcr::isMV( Other2))
            {
                pcr::setMV(map->data[r][c]);
            }else if(!pcr::isMV(Other->data[r][c]))
            {
                map->data[r][c] = Other->data[r][c];
            }else {
                map->data[r][c] = Other2;
            }
        }
    }

    map->AS_IsSingleValue = (Other->AS_IsSingleValue);
    return map;

}

inline cTMap * AS_Cover(cTMap * Other, cTMap * Other2)
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
                if(pcr::isMV(Other->data[r][c]) && pcr::isMV( Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else if(!pcr::isMV(Other->data[r][c]))
                {
                    map->data[r][c] = Other->data[r][c];
                }else {
                    map->data[r][c] = Other2->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = (Other->AS_IsSingleValue &&  Other2->AS_IsSingleValue);
        return map;


    }else
    {
        if(Other->AS_IsSingleValue)
        {

            return AS_Cover(Other->data[0][0], Other2);


        }else {
            return AS_Cover(Other, Other2->data[0][0]);
        }

    }

}



inline cTMap * AS_Mapceil(cTMap * Other)
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
                map->data[r][c] = ceilf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline cTMap * AS_Mapfloor(cTMap * Other)
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
                map->data[r][c] = floorf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline float AS_Temp_fractionf(float v)
{
    float intPart;
    return modff(v, &intPart);
}

inline cTMap * AS_Mapfraction(cTMap * Other)
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
                map->data[r][c] = AS_Temp_fractionf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mapabs(cTMap * Other)
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
                map->data[r][c] = fabs(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline cTMap * AS_Mapmax(cTMap * Other,cTMap * Other2)
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
                    map->data[r][c] = std::max(Other->data[r][c], Other2->data[r][c]);
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

        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
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
                    target->data[r][c] = std::max(map1->data[r][c],vother);
                }
            }
        }
        target->AS_IsSingleValue = false;
        return target;
    }

}
inline cTMap * AS_Mapmin(cTMap * Other,cTMap * Other2)
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
                    map->data[r][c] = std::min(Other->data[r][c], Other2->data[r][c]);
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

        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
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
                    target->data[r][c] = std::min(map1->data[r][c],vother);
                }
            }
        }
        target->AS_IsSingleValue = false;
        return target;
    }

}

#endif // RASTERLIMIT_H
