#ifndef RASTERCHANNEL_H
#define RASTERCHANNEL_H

#include <vector>
#include "geo/raster/map.h"

#include "raster/rastercommon.h"

//Get Closest Mask (1-value) cell location (find closest river)


inline static std::vector< cTMap *> AS_GetClosestCells(cTMap * mask)
{

    cTMap * sr = mask->GetCopy0();
    cTMap * sc = mask->GetCopy0();
    cTMap * dist = mask->GetCopy0();

    for(int r = 0; r < mask->data.nr_rows();r++)
    {
        for(int c = 0; c < mask->data.nr_cols();c++)
        {
            pcr::setMV(sr->data[r][c]);
            pcr::setMV(sc->data[r][c]);

            if(pcr::isMV(mask->data[r][c]))
            {

                pcr::setMV(dist->data[r][c]);

            }else
            {
                if(LISEM_ASMAP_BOOLFROMFLOAT(mask->data[r][c]))
                {
                    dist->data[r][c] = 0.0;
                    sr->data[r][c] = r + 0.5f;
                    sc->data[r][c] = c + 0.5f;
                }else
                {
                    dist->data[r][c] = 1e30;
                }

            }
        }
    }

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;


    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;
    float distld[LDD_DIR_LENGTH] = LDD_DIST_LIST;

    while(change)
    {
        iter ++;
        change = false;


        //first we move in right-lower direction
        for(int r = 0; r < dist->data.nr_rows();r++)
        {
            for(int c = 0; c < dist->data.nr_cols();c++)
            {
                for(int i = 1; i < LDD_DIR_LENGTH; i++)
                {
                    if(i != LDD_PIT)
                    {
                        int rn = r + dy[i];
                        int cn = c + dx[i];

                        if(rn > -1 && rn < dist->data.nr_rows() && cn > -1 && cn < dist->data.nr_cols())
                        {
                            if(!pcr::isMV(dist->data[rn][cn]))
                            {
                                float disto = dist->data[rn][cn];
                                float distldd = distld[i];
                                if( disto + distldd < dist->data[r][c]  && disto < 1e29)
                                {
                                    change = true;
                                    dist->data[r][c] = disto + distldd;
                                    sr->data[r][c] = sr->data[rn][cn];
                                    sc->data[r][c] = sc->data[rn][cn];

                                }
                            }
                        }
                    }
                }
            }
        }

        //then we move in left-upper direction
        for(int r = dist->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = dist->data.nr_cols()-1; c > -1 ;c--)
            {
                for(int i = 1; i < LDD_DIR_LENGTH; i++)
                {
                    if(i != LDD_PIT)
                    {
                        int rn = r + dy[i];
                        int cn = c + dx[i];

                        if(rn > -1 && rn < dist->data.nr_rows() && cn > -1 && cn < dist->data.nr_cols())
                        {
                            if(!pcr::isMV(dist->data[rn][cn]))
                            {
                                float disto = dist->data[rn][cn];
                                float distldd = distld[i];
                                if( disto + distldd < dist->data[r][c]  && disto < 1e29)
                                {
                                    change = true;
                                    dist->data[r][c] = disto + distldd;
                                    sr->data[r][c] = sr->data[rn][cn];
                                    sc->data[r][c] = sc->data[rn][cn];

                                }
                            }
                        }
                    }
                }

            }
        }

        first = false;

    }



    return {sr,sc};
}



inline static std::vector< cTMap *> AS_GetClosestCoordinate(cTMap * mask)
{
    std::vector<cTMap *> cells = AS_GetClosestCells(mask);

    for(int r = 0; r < mask->data.nr_rows();r++)
    {
        for(int c = 0; c < mask->data.nr_cols();c++)
        {
            float temp = cells[0]->data[r][c];

           if(!pcr::isMV(cells[1]->data[r][c]))
           {
                cells[0]->data[r][c] = mask->west() + ((float)(((int)(cells[1]->data[r][c])))) * mask->cellSizeX();
           }
           if(!pcr::isMV(temp))
           {
                cells[1]->data[r][c] = mask->west() + ((float)(((int)(temp))))* mask->cellSizeY();
           }
        }
    }


    return cells;
}

//Add value to closest mask value location (see function above to get those locations)



//height above closest channel

inline static cTMap * AS_RasterMoveToCell(cTMap * data, cTMap * row, cTMap * column)
{
    cTMap * res = data->GetCopy0();

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            res->data[r][c] = 0.0;

        }
    }

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(data->data[r][c]) & !pcr::isMV(row->data[r][c])  && !pcr::isMV(column->data[r][c]))
            {

                int rn =(int) row->data[r][c];
                int cn =(int) column->data[r][c];


                if(rn > -1 && rn < data->data.nr_rows() && cn > -1 && cn < data->data.nr_cols())
                {
                    res->data[rn][cn] += data->data[r][c];
                }

            }
        }
    }

    return res;
}

inline static cTMap *  AS_RasterCellSample(cTMap * data, cTMap * row, cTMap * column)
{
    cTMap * res = data->GetCopy0();

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            res->data[r][c] = 0.0;

        }
    }

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(row->data[r][c])  && !pcr::isMV(column->data[r][c]))
            {

                int rn =(int) row->data[r][c];
                int cn =(int) column->data[r][c];


                if(rn > -1 && rn < data->data.nr_rows() && cn > -1 && cn <data->data.nr_cols())
                {
                    res->data[r][c] = data->data[rn][cn];
                }

            }
        }
    }

    return res;
}


inline static cTMap * AS_RasterSample(cTMap * data, cTMap * x, cTMap * y)
{
    cTMap * res = data->GetCopy0();

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            res->data[r][c] = 0.0;

        }
    }

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(x->data[r][c])  && !pcr::isMV(y->data[r][c]))
            {
                float xc = x->data[r][c];
                float yc = y->data[r][c];

                int rn =(int) ((xc - data->west())/data->cellSizeX());
                int cn =(int) ((yc - data->north())/data->cellSizeY());

                if(rn > -1 && rn < data->data.nr_rows() && cn > -1 && cn <data->data.nr_cols())
                {
                    res->data[r][c] = data->data[rn][cn];
                }

            }
        }
    }
    return res;
}




#endif // RASTERCHANNEL_H
