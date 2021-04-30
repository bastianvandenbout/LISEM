#ifndef RASTERALGORITHMS_H
#define RASTERALGORITHMS_H


#include "geo/raster/map.h"
#include "raster/rastercommon.h"

// C++ program for the above approach

/*#include <bits/stdc++.h>
using namespace std;

// Function for calculating
// the median
double findMedian(vector<int> a,
                  int n)
{

    // If size of the arr[] is even
    if (n % 2 == 0) {

        // Applying nth_element
        // on n/2th index
        nth_element(a.begin(),
                    a.begin() + n / 2,
                    a.end());

        // Applying nth_element
        // on (n-1)/2 th index
        nth_element(a.begin(),
                    a.begin() + (n - 1) / 2,
                    a.end());

        // Find the average of value at
        // index N/2 and (N-1)/2
        return (double)(a[(n - 1) / 2]
                        + a[n / 2])
               / 2.0;
    }

    // If size of the arr[] is odd
    else {

        // Applying nth_element
        // on n/2
        nth_element(a.begin(),
                    a.begin() + n / 2,
                    a.end());

        // Value at index (N/2)th
        // is the median
        return (double)a[n / 2];
    }
}*/




inline cTMap * AS_Move(cTMap * ,float, float )
{


    return nullptr;
}


inline cTMap * AS_Shift(cTMap * Other,int dx, int dy, float fill)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");
    size_t a;
    for(int r = 0; r <  Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            int rn = r+dy;
            int cn = c+ dx;
            if(INSIDE(Other,rn,cn))
            {
                map->data[r][c] = Other->data[rn][cn];
            }else
            {
                map->data[r][c] = fill;
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;


}


inline cTMap * AS_SpreadOne(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");
    size_t a;

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;
    float dist[LDD_DIR_LENGTH] = LDD_DIST_LIST;

    for(int r = 0; r <  Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                for(int i = 1; i < LDD_DIR_LENGTH; i++)
                {
                    if(!(i == LDD_PIT))
                    {

                        int rn = r + dy[i];
                        int cn = c + dx[i];

                        if(rn > -1 && rn < map->data.nr_rows() && cn > -1 && cn < map->data.nr_cols())
                        {
                            if(!pcr::isMV(Other->data[rn][cn]))
                            {
                                map->data[r][c] = Other->data[rn][cn];
                                found = true;
                                break;
                            }


                        }
                    }
                }
                if(!found)
                {
                    pcr::setMV(map->data[r][c] );
                }

            }else
            {
                map->data[r][c] = Other->data[r][c];
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;


}


inline cTMap * AS_ShiftMV(cTMap * Other,int dx, int dy)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");
    size_t a;
    for(int r = 0; r <  Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            int rn = r+dy;
            int cn = c+ dx;
            if(INSIDE(Other,rn,cn))
            {
                map->data[r][c] = Other->data[rn][cn];
            }else
            {
                pcr::setMV(map->data[r][c]);
            }

        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;


}

inline cTMap * AS_Shift(cTMap * Other,int dx, int dy)
{
        return AS_Shift(Other, dx,  dy,0.0f);
}

inline cTMap * AS_XIndex(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            map->data[r][c] = (float)(c);

        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}


inline cTMap * AS_YIndex(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            map->data[r][c] = (float)(r);

        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_XCoord(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            map->data[r][c] = (float)(c) * Other->cellSize();

        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}


inline cTMap * AS_YCoord(cTMap * Other)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            map->data[r][c] = (float)(r) * Other->cellSize();

        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;


}

inline cTMap * AS_Clone(cTMap * Other, float val)
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
                map->data[r][c] = val;
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_MapSatialRandom()
{

    return nullptr;
}







#endif // RASTERALGORITHMS_H
