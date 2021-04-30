#ifndef RASTERSHADING_H
#define RASTERSHADING_H

#include <algorithm>
#include <QList>
#include "geo/raster/map.h"
#include "raster/rastercommon.h"
#include <vector>
#include "rasterconstructors.h"
#include "rastermath.h"

inline cTMap * AS_HillShade(cTMap * DEM, float scale)
{
    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *HILLSHADE = new cTMap(std::move(raster_data),DEM->projection(),"");

    float _dx =DEM->cellSizeX();
    float _dy =DEM->cellSizeY();

    float maxDem = -1e31;
    float minDem = 1e31;

    int n_cells = 0;

    FOR_ROW_COL_MV(DEM)
    {
        n_cells++;

        float mat[9];
        float dx, dy;//, aspect;
        float factor = 1.0;

        minDem = std::min(DEM->data[r][c], minDem);
        maxDem = std::max(DEM->data[r][c], maxDem);

        for (int i = 0; i < 9; i++)
        {
            mat[i] = DEM->data[r][c];
        }
        if (r > 0 && r < DEM->nrRows()-1 && c > 0 && c < DEM->nrCols()-1)
        {
            if(!pcr::isMV(DEM->data[r-1][c-1]))
            {
                mat[0] = DEM->data[r-1][c-1];
            }
            if(!pcr::isMV(DEM->data[r-1][c  ]))
            {
                mat[1] = DEM->data[r-1][c  ];
            }
            if(!pcr::isMV(DEM->data[r-1][c+1]))
            {
                mat[2] = DEM->data[r-1][c+1];
            }
            if(!pcr::isMV(DEM->data[r  ][c-1]))
            {
                mat[3] = DEM->data[r  ][c-1];
            }

            if(!pcr::isMV(DEM->data[r  ][c+1]))
            {
                mat[5] = DEM->data[r  ][c+1];
            }
            if(!pcr::isMV(DEM->data[r+1][c-1]))
            {
                mat[6] = DEM->data[r+1][c-1];
            }
            if(!pcr::isMV(DEM->data[r+1][c  ]))
            {
                mat[7] = DEM->data[r+1][c  ];
            }
            if(!pcr::isMV(DEM->data[r+1][c+1]))
            {
                mat[8] = DEM->data[r+1][c+1];
            }
        }
        for (int i = 0; i < 9; i++)
        {
            mat[i] *= scale;
        }

        dx = (mat[2] + 2*mat[5] + mat[8] - mat[0] -2*mat[3] - mat[6])/(8*_dx);
        dy = (mat[0] + 2*mat[1] + mat[2] - mat[6] -2*mat[7] - mat[8])/(8*_dx);

        //http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm
        //Burrough, P. A. and McDonell, R.A., 1998. Principles of Geographical Information Systems (Oxford University Press, New York), p. 190.
        float z_factor = 2.0;
        float Slope_rad = atan( z_factor * sqrt ( dx*dx+dy*dy) );
        float Aspect_rad = 0;
        if( dx != 0)
        {
            Aspect_rad = atan2(dy, -dx);
            if (Aspect_rad < 0)
                Aspect_rad = 2.0f*MAPMATH_PI + Aspect_rad;
        }
        else
        {
            if(dy > 0)
            {
                Aspect_rad = MAPMATH_PI/2.0f;
            }
            else
            {
                Aspect_rad = 2.0f*MAPMATH_PI - MAPMATH_PI/2.0f;
            }
        }
        float Zenith_rad = 70.0f * MAPMATH_PI / 180.0f;
        float Azimuth_rad = 240.0f * MAPMATH_PI / 180.0f;
        HILLSHADE->Drc = 255.0f * ( ( cos(Zenith_rad) * cos(Slope_rad) ) + ( sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad) ) );


    }

    float MaxV = (float)mapMaximum(*HILLSHADE);
    float MinV = (float)mapMinimum(*HILLSHADE);

    FOR_ROW_COL_MV(DEM)
    {
        HILLSHADE->data[r][c] = (HILLSHADE->data[r][c]-MinV)/(MaxV-MinV);
        HILLSHADE->data[r][c] = 0.8f*HILLSHADE->data[r][c]+0.2f*(DEM->data[r][c] - minDem)/(maxDem-minDem);
    }

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(pcr::isMV(DEM->data[r][c]))
            {
                pcr::setMV(HILLSHADE->Drc);
            }
        }
    }
    return HILLSHADE;

}
#endif // RASTERSHADING_H
