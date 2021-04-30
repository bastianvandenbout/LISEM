#ifndef RASTERCONDITIONAL_H
#define RASTERCONDITIONAL_H


#include <algorithm>
#include <QList>
#include "geo/raster/map.h"


inline cTMap * AS_MapIf(float  Other, float Other2, float Other3)
{
    MaskedRaster<float> raster_data(1, 1,0.0,0.0,1.0);
    cTMap *map = new cTMap(std::move(raster_data),"","");

    if(pcr::isMV(Other) || pcr::isMV(Other2) || pcr::isMV(Other3))
    {
        pcr::isMV(map->data[0][0]);
    }else
    {
        map->data[0][0] = LISEM_ASMAP_BOOLFROMFLOAT(Other) ? Other2: Other3;
    }

    map->AS_IsSingleValue = true;

    return map;

}

inline cTMap * AS_MapIf(float  Other, cTMap *Other2, float Other3)
{
    if(Other2->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2->data[0][0],Other3);

    }else
    {

        MaskedRaster<float> raster_data(Other2->data.nr_rows(), Other2->data.nr_cols(), Other2->data.north(), Other2->data.west(), Other2->data.cell_size(),Other2->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other2->projection(),"");

        for(int r = 0; r < Other2->data.nr_rows();r++)
        {
            for(int c = 0; c < Other2->data.nr_cols();c++)
            {
                if(pcr::isMV(Other2->data[r][c]) || pcr::isMV( Other)|| pcr::isMV( Other3))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other)? Other2->data[r][c]: Other3;
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }

}

inline cTMap * AS_MapIf(cTMap *  Other, float Other2, float Other3)
{

    if(Other->AS_IsSingleValue)
    {
        return AS_MapIf(Other->data[0][0],Other2,Other3);

    }else
    {

        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2)|| pcr::isMV( Other3))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c])? Other2: Other3;
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }

}
inline cTMap * AS_MapIf(float Other, float Other2, cTMap *Other3)
{
    if(Other3->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2,Other3->data[0][0]);

    }else
    {

        MaskedRaster<float> raster_data(Other3->data.nr_rows(), Other3->data.nr_cols(), Other3->data.north(), Other3->data.west(), Other3->data.cell_size(),Other3->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other3->projection(),"");

        for(int r = 0; r < Other3->data.nr_rows();r++)
        {
            for(int c = 0; c < Other3->data.nr_cols();c++)
            {
                if(pcr::isMV(Other3->data[r][c]) || pcr::isMV( Other2)|| pcr::isMV( Other))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other)? Other2: Other3->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }


}

inline cTMap * AS_MapIf(cTMap * Other, cTMap *Other2, float Other3)
{
    if(Other->AS_IsSingleValue)
    {
        return AS_MapIf(Other->data[0][0],Other2,Other3);

    }else if(Other2->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2->data[0][0],Other3);

    }else
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
                if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2->data[r][c])|| pcr::isMV( Other3))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c])? Other2->data[r][c]: Other3;
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }


}

inline cTMap * AS_MapIf(cTMap * Other, float Other2, cTMap *Other3)
{

    if(Other->AS_IsSingleValue)
    {
        return AS_MapIf(Other->data[0][0],Other2,Other3);

    }else if(Other3->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2,Other3->data[0][0]);

    }else
    {
        if(!(Other->data.nr_rows() ==  Other3->data.nr_rows() && Other->data.nr_cols() ==  Other3->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        MaskedRaster<float> raster_data(Other3->data.nr_rows(), Other3->data.nr_cols(), Other3->data.north(), Other3->data.west(), Other3->data.cell_size(),Other3->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other3->projection(),"");

        for(int r = 0; r < Other3->data.nr_rows();r++)
        {
            for(int c = 0; c < Other3->data.nr_cols();c++)
            {
                if(pcr::isMV(Other3->data[r][c]) || pcr::isMV( Other2)|| pcr::isMV( Other->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c])? Other2: Other3->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }

}

inline cTMap * AS_MapIf(float Other, cTMap *Other2, cTMap *Other3)
{
    if(Other3->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2,Other3->data[0][0]);

    }else if(Other2->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2->data[0][0],Other3);

    }else
    {
        if(!(Other3->data.nr_rows() ==  Other2->data.nr_rows() && Other3->data.nr_cols() ==  Other2->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        MaskedRaster<float> raster_data(Other3->data.nr_rows(), Other3->data.nr_cols(), Other3->data.north(), Other3->data.west(), Other3->data.cell_size(),Other3->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other3->projection(),"");

        for(int r = 0; r < Other3->data.nr_rows();r++)
        {
            for(int c = 0; c < Other3->data.nr_cols();c++)
            {
                if(pcr::isMV(Other3->data[r][c]) || pcr::isMV( Other2->data[r][c])|| pcr::isMV( Other))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other)? Other2->data[r][c]: Other3->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;
    }


}



inline cTMap * AS_MapIf(cTMap * Other, cTMap *Other2, cTMap *Other3)
{


    if(Other->AS_IsSingleValue)
    {
        if(Other2->AS_IsSingleValue && Other3->AS_IsSingleValue)
        {
            return AS_MapIf(Other->data[0][0],Other2->data[0][0],Other3->data[0][0]);

        }else if(Other2->AS_IsSingleValue || Other3->AS_IsSingleValue)
        {
            if(Other2->AS_IsSingleValue)
            {
                return AS_MapIf(Other->data[0][0],Other2,Other3->data[0][0]);
            }else
            {
                return AS_MapIf(Other->data[0][0],Other2->data[0][0],Other3);
            }

        }else
        {

            return AS_MapIf(Other->data[0][0],Other2,Other3);
        }
    }else
    {
        if(Other2->AS_IsSingleValue && Other3->AS_IsSingleValue)
        {
            return AS_MapIf(Other,Other2->data[0][0],Other3->data[0][0]);

        }else if(Other2->AS_IsSingleValue || Other3->AS_IsSingleValue)
        {
            if(Other2->AS_IsSingleValue)
            {
                return AS_MapIf(Other,Other2,Other3->data[0][0]);
            }else
            {
                return AS_MapIf(Other,Other2->data[0][0],Other3);
            }
        }else
        {
            MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
            cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

            if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() ==  Other2->data.nr_cols()))
            {
                LISEMS_ERROR("Numbers of rows and column do not match");
                throw -1;
            }

            if(!(Other3->data.nr_rows() ==  Other2->data.nr_rows() && Other3->data.nr_cols() ==  Other2->data.nr_cols()))
            {
                LISEMS_ERROR("Numbers of rows and column do not match");
                throw -1;
            }

            for(int r = 0; r < Other->data.nr_rows();r++)
            {
                for(int c = 0; c < Other->data.nr_cols();c++)
                {
                    if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2->data[r][c])|| pcr::isMV( Other3->data[r][c]))
                    {
                        pcr::setMV(map->data[r][c]);
                    }else
                    {
                        map->data[r][c] = LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c])? Other2->data[r][c]: Other3->data[r][c];
                    }
                }
            }

            map->AS_IsSingleValue = false;
            return map;
        }


    }



}

inline cTMap * AS_MapIf(float Other, float Other2)
{

    MaskedRaster<float> raster_data(1, 1,0.0,0.0,1.0);
    cTMap *map = new cTMap(std::move(raster_data),"","");

    if(pcr::isMV(Other))
    {
        pcr::setMV(map->data[0][0]);
    }else {
        if(LISEM_ASMAP_BOOLFROMFLOAT(Other))
        {
           map->data[0][0] = Other2;
        }else {
           pcr::setMV(map->data[0][0]);
        }

    }

    map->AS_IsSingleValue = true;

    return map;

}

inline cTMap * AS_MapIf(float Other, cTMap *Other2)
{
    if(Other2->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2->data[0][0]);
    }else
    {
        MaskedRaster<float> raster_data(Other2->data.nr_rows(), Other2->data.nr_cols(), Other2->data.north(), Other2->data.west(), Other2->data.cell_size(),Other2->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other2->projection(),"");

        for(int r = 0; r < Other2->data.nr_rows();r++)
        {
            for(int c = 0; c < Other2->data.nr_cols();c++)
            {
                if(pcr::isMV(Other) || pcr::isMV( Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    if(LISEM_ASMAP_BOOLFROMFLOAT(Other))
                    {
                        map->data[r][c] = Other2->data[r][c];
                    }else {
                        pcr::setMV(map->data[r][c]);
                    }
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;


    }
}


inline cTMap * AS_MapIf(cTMap * Other,float Other2)
{
    if(Other->AS_IsSingleValue)
    {
        return AS_MapIf(Other->data[0][0],Other2);
    }else
    {
        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");


        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    if(LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c]))
                    {
                        map->data[r][c] = Other2;
                    }else {
                        pcr::setMV(map->data[r][c]);
                    }
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;

    }

}

inline cTMap * AS_MapIf(cTMap * Other, cTMap *Other2)
{
    if(Other->AS_IsSingleValue)
    {
        return AS_MapIf(Other->data[0][0],Other2);
    }else if(Other2->AS_IsSingleValue)
    {
        return AS_MapIf(Other,Other2->data[0][0]);
    }else
    {
        MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

        if(!(Other->data.nr_rows() ==  Other2->data.nr_rows() && Other->data.nr_cols() ==  Other2->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]) || pcr::isMV( Other2->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    if(LISEM_ASMAP_BOOLFROMFLOAT(Other->data[r][c]))
                    {
                        map->data[r][c] = Other2->data[r][c];
                    }else {
                        pcr::setMV(map->data[r][c]);
                    }
                }
            }
        }

        map->AS_IsSingleValue = false;
        return map;

    }

}

#endif // RASTERCONDITIONAL_H
