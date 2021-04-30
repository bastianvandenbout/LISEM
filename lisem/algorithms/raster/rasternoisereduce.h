#ifndef RASTERNOISEREDUCE_H
#define RASTERNOISEREDUCE_H

#include "geo/raster/map.h"
#include "rasterconstructors.h"

inline cTMap * AS_KuwaharaFilter(cTMap * map, cTMap * WindowSize)
{
    cTMap * ret = map->GetCopy();

    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols(); c++)
        {
            if(!pcr::isMV(map->data[r][c]))
            {

                float windowsize = WindowSize->AS_IsSingleValue? WindowSize->data[0][0] : WindowSize->data[r][c];
                int cells = std::max(0,(int)(std::floor(0.5*windowsize/map->cellSize())));

                int rbegin = std::max(r-cells,0);
                int rend = std::min(r,map->nrRows()-1);
                int cbegin = std::max(c-cells,0);
                int cend = std::min(c,map->nrCols()-1);

                double val1 = 0.0f;
                double var1 = 0.0f;
                double w1 = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                w1 += 1.0f;
                                val1 += map->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                w1 += wrncn;
                                val1 += wrncn * map->data[rn][cn];
                            }
                        }
                    }
                }

                if(w1 > 0)
                {
                    val1 = val1/w1;
                }

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                var1 += (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                var1 += wrncn * (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }
                        }
                    }
                }

                rbegin = std::max(r-cells,0);
                rend = std::min(r,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val2 = 0.0f;
                double var2 = 0.0f;
                double w2 = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                w2 += 1.0f;
                                val2 += map->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                w2 += wrncn;
                                val2 += wrncn * map->data[rn][cn];
                            }
                        }
                    }
                }

                if(w2 > 0)
                {
                    val2 = val2/w2;
                }

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                var2 += (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                var2 += wrncn * (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }
                        }
                    }
                }



                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c,0);
                cend = std::min(c+cells,map->nrCols()-1);

                double val3 = 0.0f;
                double var3 = 0.0f;
                double w3 = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                w3 += 1.0f;
                                val3 += map->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                w3 += wrncn;
                                val3 += wrncn * map->data[rn][cn];
                            }
                        }
                    }
                }

                if(w3 > 0)
                {
                    val3 = val3/w3;
                }

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                var3 += (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                var3 += wrncn * (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }
                        }
                    }
                }



                rbegin = std::max(r,0);
                rend = std::min(r+cells,map->nrRows()-1);
                cbegin = std::max(c-cells,0);
                cend = std::min(c,map->nrCols()-1);

                double val4 = 0.0f;
                double var4 = 0.0f;
                double w4 = 0.0;

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                w4 += 1.0f;
                                val4 += map->data[rn][cn];
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                w4 += wrncn;
                                val4 += wrncn * map->data[rn][cn];
                            }
                        }
                    }
                }

                if(w4 > 0)
                {
                    val4 = val4/w4;
                }

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(map->data[rn][cn]))
                        {
                            if(rn == r && cn == c)
                            {
                                var4 += (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }else
                            {
                                float dist = (float)(std::min(std::abs(c - cn),std::abs(r - rn))) * map->cellSize();
                                float wrncn = std::max(0.0,std::min(1.0,(((windowsize-dist)/map->cellSize()) + 0.5f)));
                                var4 += wrncn * (map->data[rn][cn] - val1)*(map->data[rn][cn] - val1);
                            }
                        }
                    }
                }


                std::vector<float>vars;
                vars.push_back(var1);
                vars.push_back(var2);
                vars.push_back(var3);
                vars.push_back(var4);

                std::vector<float>vals;
                vals.push_back(val1);
                vals.push_back(val2);
                vals.push_back(val3);
                vals.push_back(val4);

                double minvar = 1e60;
                double val = 0.0;

                for(int i = 0; i < vars.size(); i++)
                {
                    if(vars.at(i) < minvar)
                    {
                        minvar = vars.at(i);
                        val = vals.at(i);

                    }
                }

                ret->data[r][c] = val;
            }
        }
    }

    return ret;

}


inline cTMap * AS_KuwaharaFilter(cTMap * map, float WindowSize)
{
    return AS_KuwaharaFilter(map,MapFactory(WindowSize));
}

#endif // RASTERNOISEREDUCE_H
