#ifndef RASTERSPREAD_H
#define RASTERSPREAD_H


#include "rasterconstructors.h"
#include "geo/raster/map.h"
#include "rasterderivative.h"



inline cTMap * AS_Spread(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(friction->AS_IsSingleValue))
    {
        if(!(friction->data.nr_rows() ==  points->data.nr_rows() && friction->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");


    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;


        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                        float vn_current = map->data[r][c-1];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                        float vn_current = map->data[r+1][c];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                        float vn_current = map->data[r][c+1];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }

            }
        }

        first = false;

    }

    return map;


}

inline cTMap * AS_Spread(cTMap * points,float friction_start ,cTMap * friction)
{
    return AS_Spread(points,MapFactory(friction_start),friction);

}
inline cTMap * AS_Spread(cTMap * points,cTMap * friction_start ,float friction)
{
    return AS_Spread(points,friction_start,MapFactory(friction));
}
inline cTMap * AS_Spread(cTMap * points,float friction_start ,float friction)
{
    return AS_Spread(points,MapFactory(friction_start),MapFactory(friction));

}


inline cTMap * AS_SpreadZone(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for SpreadZone (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(friction->AS_IsSingleValue))
    {
        if(!(friction->data.nr_rows() ==  points->data.nr_rows() && friction->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");

    MaskedRaster<float> raster_dataz(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *mapz = new cTMap(std::move(raster_dataz),points->projection(),"");

    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
                pcr::setMV(mapz->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;
                mapz->data[r][c] = points->data[r][c];

            }else {
                map->data[r][c] = 1e31;
                pcr::setMV(mapz->data[r][c]);
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;


        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r-1][c];

                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                        float vn_current = map->data[r][c-1];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] = mapz->data[r][c-1];

                        }
                    }
                }
            }
        }

        //first we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                        float vn_current = map->data[r+1][c];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r+1][c];


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                        float vn_current = map->data[r][c+1];

                        float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r][c+1];

                        }
                    }
                }

            }
        }

        first = false;

    }

    delete map;
    return mapz;

}

inline cTMap * AS_SpreadZone(cTMap * points,float friction_start ,cTMap * friction)
{
    return AS_SpreadZone(points,MapFactory(friction_start),friction);
}
inline cTMap * AS_SpreadZone(cTMap * points,cTMap * friction_start ,float friction)
{
    return AS_SpreadZone(points,friction_start,MapFactory(friction));
}
inline cTMap * AS_SpreadZone(cTMap * points,float friction_start ,float friction)
{
    return AS_SpreadZone(points,MapFactory(friction_start),MapFactory(friction));
}


inline cTMap * AS_ViewShed(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *slopes = new cTMap(std::move(raster_data2),DEM->projection(),"");


    cTMap *dem2 = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    bool newcells = true;

    for(int i = 1; i < std::max(DEM->nrCols(),DEM->nrRows()); i++)
    {


        if(newcells == false)
        {
            break;
        }



        newcells = false;

        int r1 = ro -i-1;
        int r2 = ro +i+2;
        int rc1 = co - i;
        int rc2 = co +i ;

        int c1 = co -i-1;
        int c2 = co +i+2;
        int cr1 = ro -i;
        int cr2 = ro +i;


        //exceeding circle
        for(int r = r1; r < r2; r++)
        {
            int c = rc1;
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }

                }
            }

            c = rc2;
                {
                    if(INSIDE(DEM,r,c))
                    {
                        newcells = true;

                        //get elevation from cell in direction towards source

                        float n = 0;
                        float dx = std::abs(c - co);
                        float dy = std::abs(r - ro);
                        int dxn = (c - co) > 0? 1:-1;
                        int dyn = (r - ro) > 0? 1:-1;
                        int dxn2;
                        int dyn2;

                        float fac_n2 =0.0;
                        if(dx > dy)
                        {
                            dxn2 = dxn;
                            dyn2 = 0;
                            fac_n2 = 1.0 - (dy/dx);

                        }else
                        {
                            dxn2 = 0;
                            dyn2 = dyn;
                            fac_n2 = 1.0 - (dx/dy);
                        }

                        int rn = r - dyn;
                        int cn = c - dxn;
                        int rn2 = r - dyn2;
                        int cn2 = c - dxn2;



                        if(INSIDE(DEM,rn,cn))
                        {

                            if(pcr::isMV(dem2->data[r][c]))
                            {
                                dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                                slopes->data[r][c] = slopes->data[rn][cn];
                            }else {

                                float slope_next = 0.0;
                                float w = 0.0;

                                if(INSIDE(DEM,rn,cn))
                                {
                                    slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                    w += (1.0-fac_n2);

                                    n++;
                                }
                                if(INSIDE(DEM,rn2,cn2))
                                {
                                     slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                     w+= fac_n2;
                                    n++;

                                }

                                if(w > 0.0)
                                {
                                    slope_next= slope_next/w;
                                }

                                float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                                float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                                float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                                if(i == 1)
                                {
                                    slope_next = -1e31;
                                }
                                if(slope_this > slope_next-1e-6)
                                {
                                    map->data[r][c] = 1.0;
                                }
                                slopes->data[r][c] = std::max(slope_this,slope_next);
                            }
                        }
                    }
                }
        }

        int r = cr1;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));

                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

        r = cr2;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

    }

    delete dem2;
    delete slopes;
    return map;

}

inline cTMap * AS_ViewAngle(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    cTMap * map = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    FOR_ROW_COL_MV(map)
    {
        float dxs = (c - co) * std::fabs(DEM->cellSizeX());
        float dys = (r - ro) *std::fabs(DEM->cellSizeY());

        map->data[r][c] = (DEM->data[r][c]-dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
    }

    return map;

}


inline cTMap * AS_ViewCriticalAngle(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *slopes = new cTMap(std::move(raster_data2),DEM->projection(),"");


    cTMap *dem2 = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    bool newcells = true;

    for(int i = 1; i < std::max(DEM->nrCols(),DEM->nrRows()); i++)
    {


        if(newcells == false)
        {
            break;
        }



        newcells = false;

        int r1 = ro -i-1;
        int r2 = ro +i+2;
        int rc1 = co - i;
        int rc2 = co +i ;

        int c1 = co -i-1;
        int c2 = co +i+2;
        int cr1 = ro -i;
        int cr2 = ro +i;


        //exceeding circle
        for(int r = r1; r < r2; r++)
        {
            int c = rc1;
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }

                }
            }

            c = rc2;
                {
                    if(INSIDE(DEM,r,c))
                    {
                        newcells = true;

                        //get elevation from cell in direction towards source

                        float n = 0;
                        float dx = std::abs(c - co);
                        float dy = std::abs(r - ro);
                        int dxn = (c - co) > 0? 1:-1;
                        int dyn = (r - ro) > 0? 1:-1;
                        int dxn2;
                        int dyn2;

                        float fac_n2 =0.0;
                        if(dx > dy)
                        {
                            dxn2 = dxn;
                            dyn2 = 0;
                            fac_n2 = 1.0 - (dy/dx);

                        }else
                        {
                            dxn2 = 0;
                            dyn2 = dyn;
                            fac_n2 = 1.0 - (dx/dy);
                        }

                        int rn = r - dyn;
                        int cn = c - dxn;
                        int rn2 = r - dyn2;
                        int cn2 = c - dxn2;



                        if(INSIDE(DEM,rn,cn))
                        {

                            if(pcr::isMV(dem2->data[r][c]))
                            {
                                dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                                slopes->data[r][c] = slopes->data[rn][cn];
                            }else {

                                float slope_next = 0.0;
                                float w = 0.0;

                                if(INSIDE(DEM,rn,cn))
                                {
                                    slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                    w += (1.0-fac_n2);

                                    n++;
                                }
                                if(INSIDE(DEM,rn2,cn2))
                                {
                                     slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                     w+= fac_n2;
                                    n++;

                                }

                                if(w > 0.0)
                                {
                                    slope_next= slope_next/w;
                                }

                                float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                                float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                                float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                                if(i == 1)
                                {
                                    slope_next = -1e31;
                                }
                                if(slope_this > slope_next-1e-6)
                                {
                                    map->data[r][c] = 1.0;
                                }
                                slopes->data[r][c] = std::max(slope_this,slope_next);
                            }
                        }
                    }
                }
        }

        int r = cr1;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

        r = cr2;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

    }



    delete dem2;
    delete map;
    return slopes;

}


#endif // RASTERSPREAD_H
