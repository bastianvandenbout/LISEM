#ifndef RASTERARTIFICIALFLOW_H
#define RASTERARTIFICIALFLOW_H


#include "QString"
#include "geo/raster/map.h"
#include "rasterconstructors.h"



inline cTMap * AS_DiffusiveMaxWave(cTMap * DEM,cTMap * H, int iter, float courant)
{

    if(DEM->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for DiffusiveMaxWave (DEM) can not be non-spatial");
        throw 1;
    }

    if(H->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for DiffusiveMaxWave (H) can not be non-spatial");
        throw 1;
    }

    if(iter < 0)
    {
        LISEMS_ERROR("Input 3 for DiffusiveMaxWave (iterations) must be larger then zero. Current value: "+ QString::number(iter));
        throw 1;
    }

    if(courant < 0.0 || courant > 1.0f)
    {
        LISEMS_ERROR("Input 4 for DiffusiveMaxWave (courant factor) must be between 0.0 and 1.0. Current value: "+ QString::number(courant));
        throw 1;
    }

    if(!(DEM->data.nr_rows() == H->data.nr_rows() && DEM->data.nr_cols() == H->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }


    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    float dx = 1.0;
    float sl;
    float dem,dem_xl,dem_xr,dem_yl,dem_yr;
    float h, h_xl, h_xr, h_yl, h_yr;
    float hn, qmax_xr,qmax_xl,qmax_yl,qmax_yr,signx,signy,vx,vy,qx,qy,sx,sy;
    bool mv_xl,mv_xr,mv_yl,mv_yr;
    float qmaxx, qmaxy;

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                map->data[r][c] = (!pcr::isMV(H->data[r][c]))? H->Drc : 0.0;

            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        courant = 0.5;
        if(i > iter - 10)
        {
            float in = i-(iter-10);
            courant = (1.0- in/10.0)*0.5 + (in/10.0) * 0.05;
        }
        if(i%2 == 0)
        {
            for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
            {
                for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }


                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map->data[r][c] = hn;

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map->data[r][c-1] = map->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map->data[r][c+1] = map->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map->data[r-1][c] = map->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map->data[r+1][c] = map->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }

        }else {
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }


                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map->data[r][c] = hn;

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map->data[r][c-1] = map->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map->data[r][c+1] = map->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map->data[r-1][c] = map->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map->data[r+1][c] = map->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }


        }
    }

    map->AS_IsSingleValue = false;
    return map;
}


inline cTMap * AS_DiffusiveMaxWaveCG(cTMap * DEM,cTMap * H,cTMap * SLOPEDEM,float slope_factor, int iter, float courant)
{

    if(DEM->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for DiffusiveMaxWave (DEM) can not be non-spatial");
        throw 1;
    }

    if(H->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for DiffusiveMaxWave (H) can not be non-spatial");
        throw 1;
    }

    if(iter < 0)
    {
        LISEMS_ERROR("Input 3 for DiffusiveMaxWave (iterations) must be larger then zero. Current value: "+ QString::number(iter));
        throw 1;
    }

    if(courant < 0.0 || courant > 1.0f)
    {
        LISEMS_ERROR("Input 4 for DiffusiveMaxWave (courant factor) must be between 0.0 and 1.0. Current value: "+ QString::number(courant));
        throw 1;
    }

    if(!(DEM->data.nr_rows() == H->data.nr_rows() && DEM->data.nr_cols() == H->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    if(!(DEM->data.nr_rows() == SLOPEDEM->data.nr_rows() && DEM->data.nr_cols() == SLOPEDEM->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }


    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    float dx = DEM->cellSizeX();
    float sl;
    float dem,dem_xl,dem_xr,dem_yl,dem_yr;
    float dems,dems_xl,dems_xr,dems_yl,dems_yr;
    float h, h_xl, h_xr, h_yl, h_yr;
    float hn, qmax_xr,qmax_xl,qmax_yl,qmax_yr,signx,signy,vx,vy,qx,qy,sx,sy,sx2,sy2;
    bool mv_xl,mv_xr,mv_yl,mv_yr;
    float qmaxx, qmaxy;

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                map->data[r][c] = (!pcr::isMV(H->data[r][c]))? H->Drc : 0.0;

            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        courant = 0.5;
        if(i > iter - 10)
        {
            float in = i-(iter-10);
            courant = (1.0- in/10.0)*0.5 + (in/10.0) * 0.05;
        }

        if(i%2 == 0)
        {
            for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
            {
                for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];
                        dems =SLOPEDEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        //slope2
                        if(c > 0)
                        {
                            dems_xl = pcr::isMV(SLOPEDEM->data[r][c-1])?dems:SLOPEDEM->data[r][c-1];
                        }else {
                            dems_xl = dems;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dems_xr = pcr::isMV(SLOPEDEM->data[r][c+1])?dems:SLOPEDEM->data[r][c+1];
                        }else {
                            dems_xr = dems;
                        }
                        if(r > 0)
                        {
                            dems_yl = pcr::isMV(SLOPEDEM->data[r-1][c])?dems:SLOPEDEM->data[r-1][c];
                        }else {
                            dems_yl = dems;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dems_yr = pcr::isMV(SLOPEDEM->data[r+1][c])?dems:SLOPEDEM->data[r+1][c];
                        }else {
                            dems_yr = dems;

                        }


                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }




                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);

                        sx2 = (h_xr + dems_xr-dems_xl - h_xl)/dx;
                        sy2 = (h_yr + dems_yr-dems_yl - h_yl)/dx;

                        sl = std::sqrt(sx2*sx2 + sy2*sy2);
                        float slopefac = std::max(0.0f,std::min(1.0f,(sl - slope_factor/3.0f)/slope_factor));
                        //velocity
                        sl = slopefac * sl + (1.0-slopefac)*std::sqrt(sx*sx + sy*sy);
                        vx = (slopefac * sx2 + (1.0-slopefac)*sx)/sl;
                        vy = (slopefac * sy2 + (1.0-slopefac)*sy)/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter
                        slopefac = 1.0;
                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);


                        qmax_xl = (1.0-slopefac)*qmax_xl + slopefac *std::max(0.0f,dems+h - dems_xl - h_xl);
                        qmax_xr = (1.0-slopefac)*qmax_xr + slopefac *std::max(0.0f,dems+h - dems_xr - h_xr);
                        qmax_yl = (1.0-slopefac)*qmax_yl + slopefac *std::max(0.0f,dems+h - dems_yl - h_yl);
                        qmax_yr = (1.0-slopefac)*qmax_yr + slopefac *std::max(0.0f,dems+h - dems_yr - h_yr);


                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map->data[r][c] = hn;

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map->data[r][c-1] = map->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map->data[r][c+1] = map->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map->data[r-1][c] = map->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map->data[r+1][c] = map->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }

        }else {
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];
                        dems = SLOPEDEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        //slope2
                        if(c > 0)
                        {
                            dems_xl = pcr::isMV(SLOPEDEM->data[r][c-1])?dems:SLOPEDEM->data[r][c-1];
                        }else {
                            dems_xl = dems;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dems_xr = pcr::isMV(SLOPEDEM->data[r][c+1])?dems:SLOPEDEM->data[r][c+1];
                        }else {
                            dems_xr = dems;
                        }
                        if(r > 0)
                        {
                            dems_yl = pcr::isMV(SLOPEDEM->data[r-1][c])?dems:SLOPEDEM->data[r-1][c];
                        }else {
                            dems_yl = dems;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dems_yr = pcr::isMV(SLOPEDEM->data[r+1][c])?dems:SLOPEDEM->data[r+1][c];
                        }else {
                            dems_yr = dems;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }

                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);

                        sx2 = (h_xr + dems_xr-dems_xl - h_xl)/dx;
                        sy2 = (h_yr + dems_yr-dems_yl - h_yl)/dx;

                        sl = std::sqrt(sx2*sx2 + sy2*sy2);
                        float slopefac = 1.0-std::max(0.0f,std::min(1.0f,(sl - slope_factor/3.0f)/slope_factor));
                        //velocity
                        sl = slopefac * sl + (1.0-slopefac)*std::sqrt(sx*sx + sy*sy);
                        vx = (slopefac * sx2 + (1.0-slopefac)*sx)/sl;
                        vy = (slopefac * sy2 + (1.0-slopefac)*sy)/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        slopefac = 1.0;
                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        qmax_xl = (1.0-slopefac)*qmax_xl + slopefac *std::max(0.0f,dems+h - dems_xl - h_xl);
                        qmax_xr = (1.0-slopefac)*qmax_xr + slopefac *std::max(0.0f,dems+h - dems_xr - h_xr);
                        qmax_yl = (1.0-slopefac)*qmax_yl + slopefac *std::max(0.0f,dems+h - dems_yl - h_yl);
                        qmax_yr = (1.0-slopefac)*qmax_yr + slopefac *std::max(0.0f,dems+h - dems_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map->data[r][c] = hn;

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map->data[r][c-1] = map->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map->data[r][c+1] = map->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map->data[r-1][c] = map->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map->data[r+1][c] = map->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }


        }
    }

    map->AS_IsSingleValue = false;
    return map;
}
































#endif // RASTERARTIFICIALFLOW_H
