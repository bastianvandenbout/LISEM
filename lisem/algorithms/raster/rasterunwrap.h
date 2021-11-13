#ifndef RASTERUNWRAP_H
#define RASTERUNWRAP_H

#include "geo/raster/map.h"
#include "unwrap/unwrap2D.c"
//#include "unwrap/unwrap3D.c"



inline static cTMap * AS_Unwrap2D(cTMap * Phase)
{
    cTMap * unwrapped = Phase->GetCopy0();

    std::vector<float> data;
    std::vector<float> datan;
    std::vector<unsigned char> mask;

    for(int r = 0; r < Phase->nrRows(); r++)
    {
        for(int c = 0; c < Phase->nrCols(); c++)
        {
            if(!pcr::isMV(Phase->data[r][c]))
            {
                data.push_back(Phase->data[r][c]);
                datan.push_back(0.0);
                mask.push_back(255);
            }else
            {
                data.push_back(0.0);
                datan.push_back(0.0);
                mask.push_back(0);
            }
        }
    }

    unwrap2D(data.data(),datan.data(),mask.data(),Phase->nrCols(), Phase->nrRows(), 0,0);

    for(int r = 0; r < Phase->nrRows(); r++)
    {
        for(int c = 0; c < Phase->nrCols(); c++)
        {
            if(!pcr::isMV(Phase->data[r][c]))
            {
                unwrapped->data[r][c] = (datan.at(r*Phase->nrCols() + c));
            }
        }
    }

    return unwrapped;
}















#endif // RASTERUNWRAP_H
