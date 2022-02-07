#ifndef RASTERDITHER_H
#define RASTERDITHER_H

#include "error.h"
#include "geo/raster/map.h"
#include "rastercommon.h"

inline cTMap * AS_Quantize(cTMap * data, int i)
{
    if( i < 1)
    {
       LISEMS_ERROR("Numbers of quantiles can not be negative");
       throw -1;
    }

    cTMap * ret = data->GetCopy();

    //get min and max first

    double frac = 1.0/((double)(i));
    double max = MapMaximum(data);
    double min = MapMinimum(data);

    //now do the quantization
    for(int r = 0; r < data->nrRows(); r++)
    {
        for(int c = 0; c < data->nrCols();c++)
        {
            if(!pcr::isMV(data->data[r][c]))
            {
                ret->data[r][c] = min +(frac * (max- min))*std::round((data->data[r][c]-min)/(frac * (max-min)));
            }
        }
    }

    return ret;
}

inline cTMap * AS_Dither(cTMap * data, int quanta)
{



    return nullptr;

}


#endif // RASTERDITHER_H
