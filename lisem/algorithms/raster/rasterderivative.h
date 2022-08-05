#ifndef RASTERDERIVATIVE_H
#define RASTERDERIVATIVE_H


#include "geo/raster/map.h"
#include "raster/rastercommon.h"


static const int UF_DIRECTION_X = 1;
static const int UF_DIRECTION_Y = 2;
static const int UF_DIRECTION_XY = 3;
static const int UF_DERIVATIVE_LR = 0;
static const int UF_DERIVATIVE_L = 1;
static const int UF_DERIVATIVE_R = 2;

static bool UF_OUTORMV(cTMap * map, int r, int c)
{
    if(r > -1 && c > -1 && r < map->nrRows() && c < map->nrCols())
    {
        return pcr::isMV(map->data[r][c]);
    }
    return true;
}
static double UF2D_Derivative(cTMap * _dem, int r, int c, int direction,double scale, int calculationside)
{
    cTMap * _in = _dem;
    float _dx = _dem->cellSize();
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {

            double dx1 = !(UF_OUTORMV(_dem,r,c+1))? _in->data[r][c+1] -_in->Drc :0.0;
            double dx2 = !(UF_OUTORMV(_dem,r,c-1))? _in->Drc - _in->data[r][c-1] :0.0;
            return (dx1 + dx2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r,c-1))? (_in->Drc -_in->data[r][c-1])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r,c+1))? (_in->data[r][c+1] -_in->Drc)/_dx :0.0;

        }
    }
    if(direction == UF_DIRECTION_Y)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {
            double dy1 = (!UF_OUTORMV(_dem,r+1,c))? _in->data[r+1][c] -_in->Drc :0.0;
            double dy2 = (!UF_OUTORMV(_dem,r-1,c))? _in->Drc - _in->data[r-1][c] :0.0;
            return (dy1 + dy2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r-1,c))? (_in->Drc - _in->data[r-1][c])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r+1,c))? (_in->data[r+1][c] -_in->Drc)/_dx :0.0;
        }
    }
    return 0;
}

static double UF2D_Derivative_scaled(cTMap * _dem, int r, int c, int direction,double scale, int calculationside)
{
    cTMap * _in = _dem;
    float _dx = _dem->cellSize();
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {

            double dx1 = !(UF_OUTORMV(_dem,r,c+1))? scale *_in->data[r][c+1] -scale *_in->Drc :0.0;
            double dx2 = !(UF_OUTORMV(_dem,r,c-1))? scale *_in->Drc - scale *_in->data[r][c-1] :0.0;
            return (dx1 + dx2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r,c-1))? (scale *_in->Drc -scale *_in->data[r][c-1])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r,c+1))? (scale *_in->data[r][c+1] -scale *_in->Drc)/_dx :0.0;

        }
    }
    if(direction == UF_DIRECTION_Y)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {
            double dy1 = (!UF_OUTORMV(_dem,r+1,c))? scale *_in->data[r+1][c] -scale *_in->Drc :0.0;
            double dy2 = (!UF_OUTORMV(_dem,r-1,c))? scale *_in->Drc - scale *_in->data[r-1][c] :0.0;
            return (dy1 + dy2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r-1,c))? (scale *_in->Drc - scale *_in->data[r-1][c])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r+1,c))? (scale *_in->data[r+1][c] -scale *_in->Drc)/_dx :0.0;
        }
    }
    return 0;

}

static double UF2D_Derivative2(cTMap * _dem, int r, int c, int direction, int calculationside, double dx)
{
    cTMap * _in = _dem;
    if(dx == 0)
    {
        dx = _dem->cellSize();
    }
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        double x = _in->Drc;
        double x1 = !UF_OUTORMV(_dem,r,c+1)? _in->data[r][c+1] : x;
        double x2 = !UF_OUTORMV(_dem,r,c-1)? _in->data[r][c-1] : x;
        double x11 = !UF_OUTORMV(_dem,r,c+2)? _in->data[r][c+2] : x;
        double x22 = !UF_OUTORMV(_dem,r,c-2)? _in->data[r][c-2] : x;
        return (-x11 + 16.0*x1 -  30.0*x + 16.0*x2 - x22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_Y)
    {
        double y = _in->Drc;
        double y1 = !UF_OUTORMV(_dem,r+1,c)? _in->data[r+1][c] : y;
        double y2 = !UF_OUTORMV(_dem,r-1,c)? _in->data[r-1][c] : y;
        double y11 = !UF_OUTORMV(_dem,r+2,c)? _in->data[r+2][c] : y;
        double y22 = !UF_OUTORMV(_dem,r-2,c)? _in->data[r-2][c] : y;
        return (-y11 + 16.0*y1 -  30.0*y + 16.0*y2 - y22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_XY)
    {
        double xy = _in->Drc;
        double xy1 = !UF_OUTORMV(_dem,r+1,c+1)? _in->data[r+1][c+1] : xy;
        double xy2 = !UF_OUTORMV(_dem,r+1,c-1)? _in->data[r+1][c-1] : xy;
        double xy3 = !UF_OUTORMV(_dem,r-1,c+1)? _in->data[r-1][c+1] : xy;
        double xy4 = !UF_OUTORMV(_dem,r-1,c-1)? _in->data[r-1][c-1] : xy;

        return (xy1 - xy2 - xy3 + xy4)/(4.0*dx*dx);
    }
    return 0;

}


static double UF2D_Derivative2_mult(cTMap * _dem,cTMap * _dem2, int r, int c, int direction, int calculationside, double dx)
{
    cTMap * _in = _dem;
    cTMap * _in2 = _dem2;
    if(dx == 0)
    {
        dx = _dem->cellSize();
    }
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        double x = _in->Drc;
        double x_2 = _in2->Drc;
        double x1 = (!UF_OUTORMV(_dem,r,c+1)? _in->data[r][c+1] : x) * (!UF_OUTORMV(_dem2,r,c+1)? _in2->data[r][c+1] : x_2);
        double x2 = (!UF_OUTORMV(_dem,r,c-1)? _in->data[r][c-1] : x)* (!UF_OUTORMV(_dem2,r,c-1)? _in2->data[r][c-1] : x_2);
        double x11 = (!UF_OUTORMV(_dem,r,c+2)? _in->data[r][c+2] : x)*(!UF_OUTORMV(_dem2,r,c+2)? _in2->data[r][c+2] : x_2);
        double x22 = (!UF_OUTORMV(_dem,r,c-2)? _in->data[r][c-2] : x)*(!UF_OUTORMV(_dem2,r,c-2)? _in2->data[r][c-2] : x_2);
        return (-x11 + 16.0*x1 -  30.0*x + 16.0*x2 - x22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_Y)
    {
        double y = _in->Drc;
        double y_2 = _in2->Drc;
        double y1 = (!UF_OUTORMV(_dem,r+1,c)? _in->data[r+1][c] : y)*(!UF_OUTORMV(_dem2,r+1,c)? _in2->data[r+1][c] : y_2);
        double y2 = (!UF_OUTORMV(_dem,r-1,c)? _in->data[r-1][c] : y)*(!UF_OUTORMV(_dem2,r-1,c)? _in2->data[r-1][c] : y_2);
        double y11 = (!UF_OUTORMV(_dem,r+2,c)? _in->data[r+2][c] : y)*(!UF_OUTORMV(_dem2,r+2,c)? _in2->data[r+2][c] : y_2);
        double y22 = (!UF_OUTORMV(_dem,r-2,c)? _in->data[r-2][c] : y)*(!UF_OUTORMV(_dem2,r-2,c)? _in2->data[r-2][c] : y_2);
        return (-y11 + 16.0*y1 -  30.0*y + 16.0*y2 - y22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_XY)
    {
        double xy = _in->Drc;
        double xy_2 = _in2->Drc;
        double xy1 = (!UF_OUTORMV(_dem,r+1,c+1)? _in->data[r+1][c+1] : xy)*(!UF_OUTORMV(_dem2,r+1,c+1)? _in2->data[r+1][c+1] : xy_2);
        double xy2 = (!UF_OUTORMV(_dem,r+1,c-1)? _in->data[r+1][c-1] : xy)*(!UF_OUTORMV(_dem2,r+1,c-1)? _in2->data[r+1][c-1] : xy_2);
        double xy3 = (!UF_OUTORMV(_dem,r-1,c+1)? _in->data[r-1][c+1] : xy)*(!UF_OUTORMV(_dem2,r-1,c+1)? _in2->data[r-1][c+1] : xy_2);
        double xy4 = (!UF_OUTORMV(_dem,r-1,c-1)? _in->data[r-1][c-1] : xy)*(!UF_OUTORMV(_dem2,r-1,c-1)? _in2->data[r-1][c-1] : xy_2);

        return (xy1 - xy2 - xy3 + xy4)/(4.0*dx*dx);
    }
    return 0;

}


static double UF2D_Derivative3(cTMap * _dem, int r, int c, int direction, int calculationside, double dx)
{
    cTMap * _in = _dem;
    if(dx == 0)
    {
        dx = _dem->cellSize();
    }
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        double x = _in->Drc;
        double x1 = !UF_OUTORMV(_dem,r,c+1)? _in->data[r][c+1] : x;
        double x2 = !UF_OUTORMV(_dem,r,c-1)? _in->data[r][c-1] : x;
        double x11 = !UF_OUTORMV(_dem,r,c+2)? _in->data[r][c+2] : x;
        double x22 = !UF_OUTORMV(_dem,r,c-2)? _in->data[r][c-2] : x;
        double x111 = !UF_OUTORMV(_dem,r,c+3)? _in->data[r][c+3] : x;
        double x222 = !UF_OUTORMV(_dem,r,c-3)? _in->data[r][c-3] : x;
        return -(-x111 + 8.0*x11 -  13.0*x1 + 13.0 * x2 - 8.0*x22 +x222)/(8.0*dx*dx);
    }
    if(direction == UF_DIRECTION_Y)
    {
        double y = _in->Drc;
        double y1 = !UF_OUTORMV(_dem,r+1,c)? _in->data[r+1][c] : y;
        double y2 = !UF_OUTORMV(_dem,r-1,c)? _in->data[r-1][c] : y;
        double y11 = !UF_OUTORMV(_dem,r+2,c)? _in->data[r+2][c] : y;
        double y22 = !UF_OUTORMV(_dem,r-2,c)? _in->data[r-2][c] : y;
        double y111 = !UF_OUTORMV(_dem,r+3,c)? _in->data[r+3][c] : y;
        double y222 = !UF_OUTORMV(_dem,r-3,c)? _in->data[r-3][c] : y;
        return -(-y111 + 8.0*y11 -  13.0*y1 + 13.0 * y2 - 8.0*y22 +y222)/(8.0*dx*dx);
    }
    return 0;

}
static cTMap * AS_Slope(cTMap * Other)
{

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    float dx = Other->cellSize();

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                //calculate slope based on neighbour cell values
                //use a 3x3 kernel to estimate average slope

                float x_11 = GetMapValue_OUTORMV3x3Av(Other,r-1,c-1);
                float x_12 = GetMapValue_OUTORMV3x3Av(Other,r-1,c);
                float x_13 = GetMapValue_OUTORMV3x3Av(Other,r-1,c+1);
                float x_21 = GetMapValue_OUTORMV3x3Av(Other,r,c-1);
                float x_22 = GetMapValue_OUTORMV3x3Av(Other,r,c);
                float x_23 = GetMapValue_OUTORMV3x3Av(Other,r,c+1);
                float x_31 = GetMapValue_OUTORMV3x3Av(Other,r+1,c-1);
                float x_32 = GetMapValue_OUTORMV3x3Av(Other,r+1,c);
                float x_33 = GetMapValue_OUTORMV3x3Av(Other,r+1,c+1);

                float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dx);

                float slope = std::sqrt(dzdx*dzdx + dzdy*dzdy);

                map->data[r][c] = slope;

            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}


static cTMap * AS_Aspect(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    if (Dx == 0) /* exception value */
                    {
                        if (Dy == 0) /* Flat -> -1 */
                            map->data[r][c] = -1.0;
                        if (Dy > 0) /* north: 0 DEGREES */
                            map->data[r][c] = 0.0;
                        if (Dy < 0) /* south: 180 DEGREES */
                            map->data[r][c] = M_PI;
                    } else {
                        if (Dy == 0) {
                            float orientVal = M_PI / (float)2;
                            if (Dx < 0) /* west */
                            {           /* 270 DEG */
                                orientVal *= 3;
                                map->data[r][c]= orientVal;
                            } else /* east 90 DEG */
                                map->data[r][c] = orientVal;
                        } else {
                            float A = atan(Dx / Dy);
                            if (Dy < 0)
                                A += M_PI;
                            if (Dy > 0 && Dx < 0)
                                A += 2 * M_PI;
                            map->data[r][c] = A;
                        }
                    }
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}
static void ZevenbergenGrid(float *Z, cTMap *dem, int r, int c)
{
    Z[1] = GetMapValue_AvRepl(dem, r - 1, c - 1);
    Z[2] = GetMapValue_AvRepl(dem, r - 1, c);
    Z[3] = GetMapValue_AvRepl(dem, r - 1, c + 1);
    Z[4] = GetMapValue_AvRepl(dem, r, c - 1);
    Z[5] = GetMapValue_AvRepl(dem, r, c);
    Z[6] = GetMapValue_AvRepl(dem, r, c + 1);
    Z[7] = GetMapValue_AvRepl(dem, r + 1, c - 1);
    Z[8] = GetMapValue_AvRepl(dem, r + 1, c);
    Z[9] = GetMapValue_AvRepl(dem, r + 1, c + 1);
}


static void PCR_CurvStuff(float dx,float *D, float *E, float *F, float *G, float *H, const float *Z)
{
    float L = dx;

    *D = ((Z[4] + Z[6]) / 2 - Z[5]) / std::sqrt(L);
    *E = ((Z[2] + Z[8]) / 2 - Z[5]) / std::sqrt(L);
    *F = (-Z[1] + Z[3] + Z[7] - Z[9]) / (4 * std::sqrt(L));
    *G = (-Z[4] + Z[6]) / (2 * L);
    *H = (Z[2] - Z[8]) / (2 * L);
}



inline cTMap * AS_ProfileCurvature(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float val;
    float Z[10], D, E, F, G, H;

     int nrRows, nrCols, r, c;

     nrRows = dem->nrRows();
     nrCols = dem->nrCols();

     for (r = 0; r < nrRows; r++)
     {
             for (c = 0; c < nrCols; c++)
             {
                 if (!pcr::isMV(dem->data[r][c]))
                 {
                     ZevenbergenGrid(Z, dem, r, c);
                     PCR_CurvStuff(dem->cellSizeX(),&D, &E, &F, &G, &H, Z);
                     if (G == 0 && H == 0)
                     {
                         val = 0;
                     }
                     else
                     {
                         val = -2 * (D * std::sqrt(std::fabs(G)) + E * std::sqrt(std::fabs(H)) + F * G * H) / (std::sqrt(std::fabs(G)) + std::sqrt(std::fabs(H)));
                     }
                     map->data[r][c] = val;
                 } else
                 {
                     pcr::setMV(map->data[r][c]);
                 }
             }
     }
    return map;
}

inline cTMap * AS_PlanarCurvature(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float val; /* value read in dem.map */
        int nrRows, nrCols, r, c;
        float Z[10], D, E, F, G, H;


        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the curv. */
        for (r = 0; r < nrRows; r++)
        {
            for (c = 0; c < nrCols; c++)
            {
                if(!pcr::isMV(dem->data[r][c])) {
                    ZevenbergenGrid(Z, dem, r, c);
                    PCR_CurvStuff(dem->cellSizeX(),&D, &E, &F, &G, &H, Z);
                    if (G == 0 && H == 0)
                    {
                        val = 0;
                    }
                    else
                    {
                        val = 2 * ((D) * std::sqrt(std::fabs(H)) + E * std::sqrt(std::fabs(G)) - F * G * H) / (std::sqrt(std::fabs(G)) + std::sqrt(std::fabs(H)));
                    }
                    map->data[r][c] = val;
                } else
                {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
        return map;
}
inline cTMap * AS_SlopeX(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dx;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}

inline cTMap * AS_SlopeX1(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dx;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}

inline cTMap * AS_SlopeX2(cTMap * dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dx;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}




inline cTMap * AS_SlopeY(cTMap *dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}




inline cTMap * AS_SlopeY1(cTMap *dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}





inline cTMap * AS_SlopeY2(cTMap *dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}



inline cTMap * AS_SlopeDx2(cTMap *dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative2(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    //Dy = UF2D_Derivative2(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}


inline cTMap * AS_SlopeDy2(cTMap *dem)
{
    MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative2(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative2(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}


#endif // RASTERDERIVATIVE_H
