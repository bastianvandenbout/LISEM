#ifndef RASTERMATH_H
#define RASTERMATH_H


#include <algorithm>
#include <QList>
#include "geo/raster/map.h"




inline cTMap * AS_Mapexp(cTMap * Other)
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
                map->data[r][c] = expf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline cTMap * AS_Maplog(cTMap * Other)
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
                map->data[r][c] = logf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}
inline cTMap * AS_Maplog10(cTMap * Other)
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
                map->data[r][c] = log10f(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;

}

inline cTMap * AS_Mappow(cTMap * Other,cTMap * Other2)
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
                    map->data[r][c] = std::pow(Other->data[r][c], Other2->data[r][c]);
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

        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(),map1->data.cell_sizeY());
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
                    target->data[r][c] = std::pow(Other->data[r][c],vother);
                }
            }
        }
        target->AS_IsSingleValue = false;
        return target;
    }

}


inline cTMap * AS_Mapsqrt(cTMap * Other)
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
                map->data[r][c] = sqrtf(Other->data[r][c]);
            }
        }
    }
    map->AS_IsSingleValue = Other->AS_IsSingleValue;
    return map;


}

























/////////////////
/// below are old raster math functions from the OpenLISEM project
/// For clarity, all functions that are called by the scripting environment start with AS_
/// These functions also provide more safety and use error reporting consistent with the scripting use
/// Therefore, functions below are for internal use only
//////////////////






inline double mapTotal(
    cTMap const& raster)
{
    double total = 0;
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                total = total + raster.data[r][c];
            }
        }

    }
    return (total);
}


inline double mapAverage(
    cTMap const& raster)
{
    double total = 0;
    double nrcells = 0;
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                total = total + raster.data[r][c];
                nrcells+=1;
            }
        }
    }
    return (total/nrcells);
}


inline double mapMinimum(
    cTMap const& raster)
{
    double total = +1e20;
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (total > raster.data[r][c])
                {
                    total = raster.data[r][c];
                }
            }
        }
    }
    return (total);
}


inline double mapMaximum(
    cTMap const& raster)
{
    double total = -1e20;
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (total < raster.data[r][c])
                {
                    total = raster.data[r][c];
                }
            }
        }
    }
    return (total);
}




/*
#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define POW 4
#define MIN 5  //VJ 041120 added this functionality
#define MAX 6
#define LARGER 7
#define SMALLER 8
#define LARGEREQUAL 9
#define SMALLEREQUAL 10
#define HIGHER 11
#define LOWER 12




inline void copy(
    cTMap& raster,
    cTMap const& other)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(other.data[r][c])&& !pcr::isMV(raster.data[r][c]))
            {
                raster.data[r][c] = other.data[r][c];
            }
            else
                pcr::setMV(raster.data[r][c]);
        }
    }
}




inline void fill(
    cTMap& raster,
    double value)
{
    int r, c;

    for (r = 0; r < raster.nrRows(); r++)
    {
        for (c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                raster.data[r][c] = value;
            }
        }
    }

}



inline double getWindowAverage(
    cTMap const& raster,
    int r,
    int c,
    bool center)
{
  double i = 0;
  double sum = 0, avg = 0;
  if (r > 0 && c > 0 && !pcr::isMV(raster.data[r-1][c-1]) && raster.data[r-1][c-1]> 0) { sum += raster.data[r-1][c-1]; i+=1.0;}
  if (r > 0 && !pcr::isMV(raster.data[r-1][c  ]) && raster.data[r-1][c  ]> 0) { sum += raster.data[r-1][c  ]; i+=1.0;}
  if (r > 0 && c < raster.nrCols()-1 && !pcr::isMV(raster.data[r-1][c+1]) && raster.data[r-1][c+1]> 0) { sum += raster.data[r-1][c+1]; i+=1.0;}
  if (c < raster.nrCols()-1 && !pcr::isMV(raster.data[r  ][c-1]) && raster.data[r  ][c-1]> 0) { sum += raster.data[r  ][c-1]; i+=1.0;}
  if (center && !pcr::isMV(raster.data[r][c]) && raster.data[r][c]> 0) { sum += raster.data[r  ][c]; i+=1.0;}
  if (c < raster.nrCols()-1 && !pcr::isMV(raster.data[r  ][c+1]) && raster.data[r  ][c+1]> 0) { sum += raster.data[r  ][c+1]; i+=1.0;}
  if (r < raster.nrRows()-1 && c > 0 && !pcr::isMV(raster.data[r+1][c-1]) && raster.data[r+1][c-1]> 0) { sum += raster.data[r+1][c-1]; i+=1.0;}
  if (r < raster.nrRows()-1 && !pcr::isMV(raster.data[r+1][c  ]) && raster.data[r+1][c  ]> 0) { sum += raster.data[r+1][c  ]; i+=1.0;}
  if (r < raster.nrRows()-1 && c < raster.nrCols()-1 && !pcr::isMV(raster.data[r+1][c+1]) && raster.data[r+1][c+1]> 0) { sum += raster.data[r+1][c+1]; i+=1.0;}
  avg = (i > 0 ? sum / i : 0);

  return(avg);
}


inline void cover(
    cTMap& raster,
    cTMap const& value1,
    float value2)
{
    int r, c;

    for (r = 0; r < raster.nrRows(); r++)
    {
        for (c = 0; c < raster.nrCols(); c++)
        {
            if (pcr::isMV(raster.data[r][c]) && !pcr::isMV(value1.data[r][c]))
            {
                raster.data[r][c] = value2;
            }
        }
    }
    //     else
    //       pcr::setMV(raster.data[r][c]);
}


inline void calcValue(
    cTMap& raster,
    float value,
    int oper)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                switch (oper)
                {
                case ADD: raster.data[r][c] += value; break;
                case SUB: raster.data[r][c] -= value; break;
                case MUL: raster.data[r][c] *= value; break;
                case DIV: if (value > 0) raster.data[r][c] /= value;
                    else pcr::setMV(raster.data[r][c]); break;
                case POW: raster.data[r][c] = pow(raster.data[r][c],value); break;
                case MIN: raster.data[r][c] = std::min(raster.data[r][c],value); break;//VJ 110420 new
                case MAX: raster.data[r][c] = std::max(raster.data[r][c],value); break;
                }
            }
        }
    }
}


inline void calcMap(
    cTMap& raster,
    cTMap const& value,
    int oper)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (!pcr::isMV(value.data[r][c]))
                {
                    switch (oper)
                    {
                    case ADD: raster.data[r][c] += value.data[r][c]; break;
                    case SUB: raster.data[r][c] -= value.data[r][c]; break;
                    case MUL: raster.data[r][c] *= value.data[r][c]; break;
                    case DIV: if (value.data[r][c] > 0) raster.data[r][c] /= value.data[r][c];
                        else pcr::setMV(raster.data[r][c]); break;
                    case POW: raster.data[r][c] = powl(raster.data[r][c],value.data[r][c]); break;
                    case MIN: raster.data[r][c] = std::min(value.data[r][c], raster.data[r][c]); break; //VJ 110420 new
                    case MAX: raster.data[r][c] = std::max(value.data[r][c], raster.data[r][c]); break;
                    }
                }
                else
                {
                    pcr::setMV(raster.data[r][c]);
                }
            }
        }
    }
}


inline void calc2Maps(
    cTMap& raster,
    cTMap const& value1,
    cTMap const& value2,
    int oper)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (!pcr::isMV(value1.data[r][c]) && !pcr::isMV(value2.data[r][c]))
                {
                    switch (oper)
                    {
                    case ADD: raster.data[r][c] = value1.data[r][c] + value2.data[r][c]; break;
                    case SUB: raster.data[r][c] = value1.data[r][c] - value2.data[r][c]; break;
                    case MUL: raster.data[r][c] = value1.data[r][c] * value2.data[r][c]; break;
                    case DIV: if (value2.data[r][c] > 0) raster.data[r][c] = value1.data[r][c] / value2.data[r][c];
                        else pcr::setMV(raster.data[r][c]); break;
                    case POW: raster.data[r][c] = pow(value1.data[r][c], value2.data[r][c]); break;
                    case MIN: raster.data[r][c] = std::min(value1.data[r][c], value2.data[r][c]); break; //VJ 110420 new
                    case MAX: raster.data[r][c] = std::max(value1.data[r][c], value2.data[r][c]); break;
                    }
                }
                else
                {
                    pcr::setMV(raster.data[r][c]);
                }
            }
        }
    }

}


inline void calcMapValue(
    cTMap& raster,
    cTMap const& value1,
    float value2,
    int oper)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (!pcr::isMV(value1.data[r][c]))
                {
                    switch (oper)
                    {
                    case ADD: raster.data[r][c] = value1.data[r][c] + value2; break;
                    case SUB: raster.data[r][c] = value1.data[r][c] - value2; break;
                    case MUL: raster.data[r][c] = value1.data[r][c] * value2; break;
                    case DIV: if (value2 > 0) raster.data[r][c] = value1.data[r][c] / value2;
                        else pcr::setMV(raster.data[r][c]); break;
                    case POW: raster.data[r][c] = pow(value1.data[r][c],value2); break;
                    case MIN: raster.data[r][c] = std::min(value1.data[r][c],value2); break;//VJ 110420 new
                    case MAX: raster.data[r][c] = std::max(value1.data[r][c],value2); break;
                    }
                }
                else
                {
                    pcr::setMV(raster.data[r][c]);
                }
            }
        }

    }
}


inline void checkMap(
    cTMap const& raster,
    int oper,
    double value,
    QString SS)
{
    for (int r = 0; r < raster.nrRows(); r++)
    {
        for (int c = 0; c < raster.nrCols(); c++)
        {
            if (!pcr::isMV(raster.data[r][c]))
            {
                if (oper == LARGER && raster.data[r][c] > value)
                {
                    //ErrorString = QString("Value at row=%1 and col=%2 in %3 is larger than %4.\n").arg(r).arg(c).arg(raster.mapName()).arg(value) + SS;
                    throw 1;
                }
                else
                    if (oper == SMALLER && raster.data[r][c] < value)
                    {
                        //ErrorString = QString("Value at row=%1 and col=%2 in %3 is smaller than %4.\n").arg(r).arg(c).arg(raster.mapName()).arg(value) + SS;
                        throw 1;
                    }
                    else
                        if (oper == LARGEREQUAL && raster.data[r][c] >= value)
                        {
                            //ErrorString = QString("Value at row=%1 and col=%2 in %3 is larger or equal than %4.\n").arg(r).arg(c).arg(raster.mapName()).arg(value) + SS;
                            throw 1;
                        }
                        else
                            if (oper == SMALLEREQUAL && raster.data[r][c] <= value)
                            {
                                //ErrorString = QString("Value at row=%1 and col=%2 in %3 is smaller or equal than %4.\n").arg(r).arg(c).arg(raster.mapName()).arg(value) + SS;
                                throw 1;
                            }
            }
        }
    }
}*/



#endif // RASTERMATH_H
