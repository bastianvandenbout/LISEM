#ifndef RASTERCONVOLVE_H
#define RASTERCONVOLVE_H



#include "geo/raster/map.h"
#include "rasterconstructors.h"


inline cTMap * AS_RasterShiftToOther(cTMap * Other,cTMap * Points, float windowsize_in)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowAverage (values) can not be non-spatial");
        throw 1;
    }

    if(Points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowAverage (values) can not be non-spatial");
        throw 1;
    }


     if(!(Points->data.nr_rows() == Other->data.nr_rows() && Points->data.nr_cols() == Other->data.nr_cols()))
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
            if(pcr::isMV(Points->data[r][c] ) || Points->data[r][c] == 0)
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = Points->data[r][c] ;
            }

        }
    }

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(!(pcr::isMV(map->data[r][c]) || map->data[r][c] == 0) && (pcr::isMV(Other->data[r][c]) || Other->data[r][c] == 0))
            {
                float windowsize = windowsize_in;
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                int rmatch = -1;
                int cmatch = -1;
                bool first_match = true;
                double dist_min = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        //if(!(r == rn && c == cn))
                        {
                            if(!(pcr::isMV(Other->data[rn][cn]) || Other->data[rn][cn] == 0) && (pcr::isMV(map->data[rn][cn])))
                            {
                                //should we try to move this point?
                                //distance
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();

                                if(first_match || dist < dist_min)
                                {
                                    first_match = false;
                                    dist_min = dist;
                                    rmatch = rn;
                                    cmatch = cn;
                                }
                            }
                        }
                    }
                }

                if(first_match == false)
                {
                    if(!(rmatch == r && cmatch == c))
                    {
                        map->data[rmatch][cmatch] = map->data[r][c];
                        pcr::setMV(map->data[r][c]);
                    }
                }
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}


inline cTMap * AS_WindowAverage(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowAverage (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::cout << "create map " << map << std::endl;
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 0.0f;
                float w = 0.0f;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                            if(rn == r && cn == c)
                            {
                                w += 1.0f;
                                val += Other->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/Other->cellSize()) + 0.5f)));
                                w += wrncn;
                                val += wrncn * Other->data[rn][cn];
                            }
                        }

                    }
                }

                if(w > 0.0f)
                {
                    map->data[r][c] = val/w;
                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowAverage(cTMap * Map,float WindowSize)
{
    return AS_WindowAverage(Map,MapFactory(WindowSize));
}


inline cTMap * AS_WindowVariance(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowVariance (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 0.0f;
                float w = 0.0f;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                            if(rn == r && cn == c)
                            {
                                w += 1.0f;
                                val += Other->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/Other->cellSize()) + 0.5f)));
                                w += wrncn;
                                val += wrncn * Other->data[rn][cn];
                            }
                        }

                    }
                }

                float mean =val/w;

                float variance = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                            if(rn == r && cn == c)
                            {

                                variance += (Other->data[rn][cn]-mean)*(Other->data[rn][cn]-mean);
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/Other->cellSize()) + 0.5f)));

                                variance += wrncn *(Other->data[rn][cn]-mean)*(Other->data[rn][cn]-mean);
                            }
                        }

                    }
                }

                variance = variance/w;

                if(w > 0.0f)
                {
                    map->data[r][c] =variance;
                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowVariance(cTMap * Map,float WindowSize)
{
    return AS_WindowVariance(Map,MapFactory(WindowSize));
}



inline cTMap * AS_WindowMajority(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowMajority (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 0.0f;
                float w = 0.0f;

                std::vector<int> classes;
                std::vector<float> vals;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                            if(rn == r && cn == c)
                            {
                                w += 1.0f;

                                bool found = false;
                                int class_current = (int)Other->data[rn][cn];
                                int i;
                                for(i= 0; i < classes.size(); i++)
                                {
                                    if(class_current == classes.at(i))
                                    {
                                        found = true;
                                        break;
                                    }

                                }

                                if(found)
                                {
                                    vals.at(i) += 1.0;
                                }else
                                {
                                    i = classes.size();
                                    classes.push_back(class_current);
                                    vals.push_back(0.0);
                                    vals.at(i) += 1.0;
                                }

                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/Other->cellSize()) + 0.5f)));
                                w += wrncn;

                                bool found = false;
                                int class_current = (int)Other->data[rn][cn];
                                int i;
                                for(i= 0; i < classes.size(); i++)
                                {
                                    if(class_current == classes.at(i))
                                    {
                                        found = true;
                                        break;
                                    }

                                }

                                if(found)
                                {
                                    vals.at(i) += wrncn;
                                }else
                                {
                                    i = classes.size();
                                    classes.push_back(class_current);
                                    vals.push_back(0.0);
                                    vals.at(i) += wrncn;
                                }

                            }
                        }

                    }
                }


                if(w > 0.0f)
                {
                    double max = vals.at(0);
                    int index = 0;

                    for(int i = 0; i < vals.size(); i++)
                    {
                        if(vals.at(i) > max){
                            index = i;
                            max = vals.at(i);
                        }
                    }
                    map->data[r][c] = classes.at(index);
                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowMajority(cTMap * Map,float WindowSize)
{
    return AS_WindowMajority(Map,MapFactory(WindowSize));
}



inline cTMap * AS_WindowDiversity(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowMajority (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");



    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 0.0f;
                float w = 0.0f;

                std::vector<int> classes;
                std::vector<float> vals;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                            if(rn == r && cn == c)
                            {
                                w += 1.0f;

                                bool found = false;
                                int class_current = (int)Other->data[rn][cn];
                                int i;
                                for(i= 0; i < classes.size(); i++)
                                {
                                    if(class_current == classes.at(i))
                                    {
                                        found = true;
                                        break;
                                    }

                                }

                                if(found)
                                {
                                    vals.at(i) += 1.0;
                                }else
                                {
                                    i = classes.size();
                                    classes.push_back(class_current);
                                    vals.push_back(0.0);
                                    vals.at(i) += 1.0;
                                }

                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * Other->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/Other->cellSize()) + 0.5f)));
                                w += wrncn;

                                bool found = false;
                                int class_current = (int)Other->data[rn][cn];
                                int i;
                                for(i= 0; i < classes.size(); i++)
                                {
                                    if(class_current == classes.at(i))
                                    {
                                        found = true;
                                        break;
                                    }

                                }

                                if(found)
                                {
                                    vals.at(i) += wrncn;
                                }else
                                {
                                    i = classes.size();
                                    classes.push_back(class_current);
                                    vals.push_back(0.0);
                                    vals.at(i) += wrncn;
                                }

                            }
                        }

                    }
                }


                if(w > 0.0f)
                {
                    map->data[r][c] = classes.size();
                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowDiversity(cTMap * Map,float WindowSize)
{
    return AS_WindowDiversity(Map,MapFactory(WindowSize));
}

inline cTMap * AS_WindowTotal(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowTotal (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 0.0f;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {
                                val += Other->data[rn][cn];

                        }

                    }
                }

                map->data[r][c] = val;
            }

        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowTotal(cTMap * Map,float WindowSize)
{
    return AS_WindowTotal(Map,MapFactory(WindowSize));
}


inline cTMap * AS_WindowMaximum(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowMaximum (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = -1e31;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {
                                val = std::max(val,Other->data[rn][cn]);
                        }
                    }
                }
                map->data[r][c] = val;
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowMaximum(cTMap * Map,float WindowSize)
{
    return AS_WindowMaximum(Map,MapFactory(WindowSize));
}


inline cTMap * AS_WindowMinimum(cTMap * Other,cTMap * WindowSize)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for WindowMinimum (values) can not be non-spatial");
        throw 1;
    }

    if(!WindowSize->AS_IsSingleValue)
    {
        if(!(WindowSize->data.nr_rows() == Other->data.nr_rows() && WindowSize->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        std::cout << "window min " << r<< std::endl;
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]) || ( !WindowSize->AS_IsSingleValue? pcr::isMV(WindowSize->data[r][c]) : false))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/Other->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r+cells,Other->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c+cells,Other->nrCols()-1);

                float val = 1e31;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(Other->data[rn][cn]))
                        {

                                val = std::min(val,Other->data[rn][cn]);
                        }
                    }
                }
                map->data[r][c] = val;
            }
        }
    }
    map->AS_IsSingleValue = false;
    return map;


}

inline cTMap * AS_WindowMinimum(cTMap * Map,float WindowSize)
{
    return AS_WindowMinimum(Map,MapFactory(WindowSize));
}


inline cTMap * AS_GaussianInfluence(cTMap * Other,cTMap * Range,cTMap * Power)
{

    if(Other->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (Values) can not be non-spatial");
        throw 1;
    }

    if(!(Range->AS_IsSingleValue))
    {
        if(!(Range->data.nr_rows() ==  Other->data.nr_rows() && Range->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(Power->AS_IsSingleValue))
    {
        if(!(Power->data.nr_rows() == Other->data.nr_rows() && Power->data.nr_cols() == Other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    MaskedRaster<float> raster_dataw(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *mapw = new cTMap(std::move(raster_dataw),Other->projection(),"");

    float dx = Other->cellSize();

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {

            if(!pcr::isMV(Other->data[r][c]))
            {
                float range = Range->AS_IsSingleValue? Range->data[0][0]: Range->data[r][c];
                float pow = Power->AS_IsSingleValue? Power->data[0][0]: Power->data[r][c];

                float rangesqr = range*range;

                float val = Other->data[r][c];

                int range_cells = std::round((range/Other->cellSize()) + 1);
                int range_cellssqr = range_cells * range_cells;

                int rstart = std::max(0,r-2*range_cells);
                int rend = std::min(Other->nrRows(),r+2*range_cells);
                int cstart = std::max(0,c-2*range_cells);
                int cend = std::min(Other->nrCols(),c+2*range_cells);


                for(int r2 = rstart; r2 < rend;r2++)
                {
                    for(int c2 = cstart; c2 < cend;c2++)
                    {

                        float rdif = (float)(r - r2) * dx;
                        float cdif = (float)(c - c2) * dx;

                        float distsqr = rdif*rdif + cdif*cdif;

                        if(range_cellssqr <= distsqr)
                        {
                            float w = std::exp(std::pow(-distsqr,0.5f*pow)/(rangesqr));
                            map->data[r2][c2] += w*val;
                            mapw->data[r2][c2] += w;


                        }
                    }
                }

            }else
            {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(map->data[r][c]))
            {
                if(mapw->data[r][c] > 0.0f)
                {
                    map->data[r][c] = map->data[r][c]/mapw->data[r][c];
                }
            }

        }
    }

    delete mapw;
    return map;

}

inline cTMap * AS_GaussianInfluence(cTMap * Other,float Range,cTMap * Power)
{
    return AS_GaussianInfluence(Other,MapFactory(Range),Power);
}

inline cTMap * AS_GaussianInfluence(cTMap * Other,cTMap * Range,float Power)
{
    return AS_GaussianInfluence(Other,Range,MapFactory(Power));
}

inline cTMap * AS_GaussianInfluence(cTMap * Other,float Range,float Power)
{
    return AS_GaussianInfluence(Other,MapFactory(Range),MapFactory(Power));
}



inline cTMap * AS_Erode(cTMap * Other, int steps)
{
    if(steps < 1)
    {
        LISEMS_ERROR("Can not erode map with negative number of steps, use Dilute instead");
        throw 1;
    }

    cTMap * temp = Other->GetCopy();

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int i = 0; i < steps; i++)
    {
        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    int cells = 1;

                    int rbegin = std::max(r-cells,0);
                    int rend = std::min(r+cells,Other->nrRows()-1);
                    int cbegin = std::max(c-cells,0);
                    int cend = std::min(c+cells,Other->nrCols()-1);

                    float val = 1e31;

                    for(int rn = rbegin; rn < rend + 1; rn++)
                    {
                        for(int cn = cbegin; cn < cend + 1; cn++)
                        {
                            if(!pcr::isMV(temp->data[rn][cn]))
                            {

                                    val = std::min(val,temp->data[rn][cn]);
                            }
                        }
                    }
                    map->data[r][c] = val;
                }
            }
        }

        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                temp->data[r][c] = map->data[r][c];
            }

        }


    }

    delete temp;
    return map;

}

inline cTMap * AS_Dilute(cTMap * Other, int steps)
{
    if(steps < 1)
    {
        LISEMS_ERROR("Can not dilute map with negative number of steps, use Erode instead");
        throw 1;
    }

    cTMap * temp = Other->GetCopy();

    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int i = 0; i < steps; i++)
    {
        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    int cells = 1;

                    int rbegin = std::max(r-cells,0);
                    int rend = std::min(r+cells,Other->nrRows()-1);
                    int cbegin = std::max(c-cells,0);
                    int cend = std::min(c+cells,Other->nrCols()-1);

                    float val = -1e31;

                    for(int rn = rbegin; rn < rend + 1; rn++)
                    {
                        for(int cn = cbegin; cn < cend + 1; cn++)
                        {
                            if(!pcr::isMV(temp->data[rn][cn]))
                            {

                                    val = std::max(val,temp->data[rn][cn]);
                            }
                        }
                    }
                    map->data[r][c] = val;
                }
            }
        }

        for(int r = 0; r < Other->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->data.nr_cols();c++)
            {
                temp->data[r][c] = map->data[r][c];
            }

        }


    }

    delete temp;
    return map;

}
#endif // RASTERCONVOLVE_H
