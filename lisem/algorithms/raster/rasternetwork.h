#pragma once

#include <algorithm>
#include <QList>
#include "geo/raster/map.h"
#include "raster/rastercommon.h"
#include <vector>
#include "rasterconstructors.h"

//defines from rastercommon.h

//#define LDD_VAL_LIST {0,1,2,3,4,5,6,7,8,9}
//#define LDD_X_LIST {0, -1, 0, 1, -1, 0, 1, -1, 0, 1}
//#define LDD_DIR_LENGTH 10
//#define LDD_Y_LIST {0, 1, 1, 1, 0, 0, 0, -1, -1, -1}
//#define LDD_DIST_DIAG 1.41421356237f
//#define LDD_DIST 1.0f
//#define LDD_DIST_LIST {0,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG,LDD_DIST, 0.0f, LDD_DIST,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG }
//#define LDD_PIT 5;


inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,cTMap * corevolume, cTMap * corearea, cTMap * precipitation, bool lddin, cTMap * demn)//, bool alter_newdem = false, cTMap * demnew = nullptr
{

    if(DEM->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!outflowdepth->AS_IsSingleValue)
    {
        if(!(DEM->data.nr_rows() ==  outflowdepth->data.nr_rows() && DEM->data.nr_cols() == outflowdepth->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!corevolume->AS_IsSingleValue)
    {
        if(!(DEM->data.nr_rows() == corevolume->data.nr_rows() && DEM->data.nr_cols() == corevolume->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!corearea->AS_IsSingleValue)
    {
        if(!(DEM->data.nr_rows() == corearea->data.nr_rows() && DEM->data.nr_cols() == corearea->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }
    if(!precipitation->AS_IsSingleValue)
    {
        if(!(DEM->data.nr_rows() == precipitation->data.nr_rows() && DEM->data.nr_cols() == precipitation->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(demn != nullptr)
    {
        if(!(DEM->data.nr_rows() == demn->data.nr_rows() && DEM->data.nr_cols() == demn->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        //initialize the map with friction values
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                demn->data[r][c] = DEM->data[r][c];
            }
        }

    }


    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;
    float dist[LDD_DIR_LENGTH] = LDD_DIST_LIST;


    //first step in the algiorithm is to produce a locally based network map.
    //this is done simply setting the value of each cell to the direction of the lowest neighbor
    //if the cell itself becomes the lowest neighbor, it becomes a pit

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");


    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                bool nbMV = false;
                float dem = DEM->data[r][c];
                float lowest = 1e31;
                int lowest_i = -1;
                for(int i = 1; i < LDD_DIR_LENGTH; i++)
                {
                    if(!(i == LDD_PIT))
                    {

                        int rn = r + dy[i];
                        int cn = c + dx[i];
                        float distn = dist[i];


                        if(rn > -1 && rn < map->data.nr_rows() && cn > -1 && cn < map->data.nr_cols())
                        {
                            if(!pcr::isMV(DEM->data[rn][cn]))
                            {
                                float slope = (DEM->data[rn][cn]-dem)/distn;
                                if(slope < lowest)
                                {
                                    lowest = slope;
                                    lowest_i = i;

                                }
                            }else {
                                nbMV = true;
                            }

                        }

                    }
                }

                //case when all surrounding cells are MV
                if(lowest_i < 0)
                {
                    map->data[r][c] = LDD_PIT;
                //else there should be at least a single best direction
                }else
                {
                    int i  = lowest_i;
                    int rn = r + dy[i];
                    int cn = c + dx[i];
                    float demn = DEM->data[rn][cn];

                    //now check if this direction is also lower than the cell itself

                    //if lower, normal flow direction
                    if(demn < dem)
                    {
                        map->data[r][c] = lowest_i;

                    //if equal, flat area (need to resolve later
                    }else if(demn == dem)
                    {
                        //if it borders a MV, we assign it as a pit (since it will become outflow)
                        if(nbMV)
                        {
                            map->data[r][c] = LDD_PIT;
                        }else
                        {
                            map->data[r][c] = LDD_FLAT;
                        }

                    //if greater, then pit
                    }else {

                        map->data[r][c] = LDD_PIT;
                    }


                }
            }else {

                pcr::setMV(map->data[r][c]);
                }

        }
    }


    //resolve flats that look like a saddle point on a function
    // this is done in an iterative manner, similar to the spread algorithm

    bool fixed_something = true;

    int iter = 0;
    while (fixed_something )
    {
        iter ++;
        int fixed_n = 0;
        fixed_something = false;



        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->data[r][c];

                    float val_current = map->data[r][c];
                    if(((int)(val_current)) == LDD_FLAT)
                    {
                        for(int i = 1; i < LDD_DIR_LENGTH; i++)
                        {
                            if(i != LDD_PIT)
                            {
                                int rn = r + dy[i];
                                int cn = c + dx[i];

                                if(rn > -1 && rn < map->data.nr_rows() && cn > -1 && cn < map->data.nr_cols())
                                {
                                    int valn = (int) (map->data[rn][cn]);
                                    int demn = DEM->data[rn][cn];

                                    if(LDD_IS_ACTUAL(valn))
                                    {
                                        int rnn = rn + dy[valn];
                                        int cnn = cn + dx[valn];
                                        float demnn = DEM->data[rnn][cnn];
                                        int valnn = map->data[rnn][cnn];

                                        if(rnn != r && cnn != c && (demn <= dem ) && !(demnn > dem))
                                        {
                                            fixed_something = true;
                                            fixed_n += 1;
                                            map->data[r][c] = LDD_MAKE_TEMP(i);
                                            break;
                                        }
                                    }

                                }
                            }

                        }

                    }
                }

            }

        }

        if(fixed_something)
        {
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float val_current = map->data[r][c];
                        if(LDD_IS_TEMP((int)(val_current)))
                        {
                            float valn = LDD_MAKE_ACTUAL(val_current);
                            map->data[r][c] = valn;
                        }
                    }

                }
            }
        }
    }



    //resolve flats that are local depressions
    // this is done in an iterative manner, similar to the spread algorithm

    fixed_something = true;
    while (fixed_something)
    {

        int fixed_n = 0;
        fixed_something = false;



        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {

                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->data[r][c];

                    float val_current = map->data[r][c];
                    if(((int)(val_current)) == LDD_FLAT)
                    {
                        for(int i = 1; i < LDD_DIR_LENGTH; i++)
                        {
                            if(i != LDD_PIT)
                            {
                                int rn = r + dy[i];
                                int cn = c + dx[i];

                                if(rn > -1 && rn < map->data.nr_rows() && cn > -1 && cn < map->data.nr_cols())
                                {
                                    int valn = (int) (map->data[rn][cn]);
                                    int demn = DEM->data[rn][cn];

                                    if(LDD_IS_ACTUAL(valn))
                                    {
                                        int rnn = rn + dy[valn];
                                        int cnn = cn + dx[valn];

                                        if(rnn == r && cnn == c)
                                        {
                                            for(int j = 1; j < LDD_DIR_LENGTH; j++)
                                            {
                                                if(j != LDD_PIT)
                                                {
                                                    int rnnn = r + dy[j];
                                                    int cnnn = c + dx[j];

                                                    if(rnnn > -1 && rnnn < map->data.nr_rows() && cnnn > -1 && cnnn < map->data.nr_cols())
                                                    {
                                                        int valnn = (int) (map->data[rnnn][cnnn]);

                                                        if(valnn == LDD_FLAT)
                                                        {
                                                            fixed_something = true;
                                                            fixed_n += 1;
                                                            map->data[r][c] = LDD_MAKE_TEMP(j);
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(fixed_something)
        {
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float val_current = map->data[r][c];
                        if(LDD_IS_TEMP((int)(val_current)))
                        {
                            float valn = LDD_MAKE_ACTUAL(val_current);
                            map->data[r][c] = valn;
                        }
                    }

                }
            }
        }


    }

    //resolve left-over 0 values (which are pits)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float val_current = map->data[r][c];
                if(LDD_FLAT== ((int)(val_current)))
                {
                    map->data[r][c] = LDD_PIT;
                }
            }

        }
    }



    MaskedRaster<int> catchmentsm(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    MaskedRaster<int> catchmentspi(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());


    //check if correct and otherwise fix
    //to do this, we follow the network attached to each pit. If not all cells are found using this method, those non-mv cells do not drain to a pit.
    //this can be the case of there are two cells directed towards each other
    //or more generally, if there is a loop within the network.

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            catchmentspi[ro][co] = -1;
        }
    }

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(DEM->data[ro][co]))
            {
                float dem = DEM->data[ro][co];

                float val_current = map->data[ro][co];
                if(((int)(val_current)) == LDD_PIT)
                {

                    LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                    while(list != nullptr)
                    {
                        int r = list->rowNr;
                        int c = list->colNr;

                        float dem = DEM->data[r][c];

                        //TODO - check cycles

                        catchmentspi[r][c] = 1;


                        list = ListReplaceFirstByUSNB(map,list);
                    }
                }
            }

        }
    }

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(DEM->data[ro][co]))
            {

                int r = ro;
                int c = co;
                bool next = true;

                while(next)
                {
                    next = false;

                    if(catchmentspi[r][c] == -1)
                    {
                        int ldd = (int) map->data[r][c];

                        if(ldd > 0 && ldd < 10 && ldd != 5)
                        {

                            int rn = rn + dy[ldd];
                            int cn = cn + dx[ldd];

                            if(!OUTORMV(map,rn,cn))
                            {
                                r = rn;
                                c = cn;
                                next = true;

                            }else {

                                map->data[r][c] = 5;
                                catchmentspi[r][c] = 1;
                            }
                        }else
                        {
                            map->data[r][c] = 5;
                            catchmentspi[r][c] = 1;
                        }

                    }
                }
            }
        }
    }


    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            catchmentspi[ro][co] = 0;
        }
    }

    bool fixed_pit = true;

    while(fixed_pit)
    {

        fixed_pit = false;

        std::vector<LDD_PITPROPERTIES> PitList;

        //create a catchment map
        int catchment=  0;
        //now check all actual pits and correct them
        for(int ro = 0; ro < map->data.nr_rows();ro++)
        {
            for(int co = 0; co < map->data.nr_cols();co++)
            {
                if(!pcr::isMV(DEM->data[ro][co]))
                {
                    float val_current = map->data[ro][co];
                    if(LDD_PIT == ((int)(val_current)))
                    {

                        LDD_PITPROPERTIES pitprops;
                        pitprops.colNr = co;
                        pitprops.rowNr = ro;
                        pitprops.catchment_nr = catchment;
                        pitprops.dem_pit = DEM->data[ro][co];

                        LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                        while(list != nullptr)
                        {
                            int r = list->rowNr;
                            int c = list->colNr;


                            pitprops.catchment_area += map->cellSize() * map->cellSize();

                            list = ListReplaceFirstByUSNB(map,list);
                            catchmentspi[r][c] = catchment;
                        }


                        PitList.push_back(pitprops);

                        catchment += 1;
                    }
                }
            }

        }

        //sort the pits based on catchment size and pit elevation
        if(PitList.size() > 1)
        {
            std::sort(PitList.begin(),PitList.end(),PitCompFunc);
        }

        int catchmentunique = 1;

        //get the best outflow point for this specific depression
        //if found, solve the
        for(int i = 0; i < PitList.size()-1; i++)
        {

              LDD_PITPROPERTIES pp = PitList.at(i);

              //fix catchment mask map
              if(pp.r_lowestnb != -1 && pp.c_lowestnb != -1)
              {
                  int ro = pp.rowNr;
                  int co = pp.colNr;

                  LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                  while(list != nullptr)
                  {
                      int r = list->rowNr;
                      int c = list->colNr;

                      catchmentsm[r][c] = catchmentunique;

                      list = ListReplaceFirstByUSNB(map,list);
                  }
             }

              int pi_flowinto = -1;

              //first find the lowest outflow point for this depression
              {
                int ro = pp.rowNr;
                int co = pp.colNr;

                float dem_pit = DEM->data[ro][co];

                float dem_lowestnb = 1e30;
                int r_lowestnb = -1;
                int c_lowestnb = -1;
                int r_lowestnbconnect = -1;
                int c_lowestnbconnect = -1;
                int connectdir = -1;

                     //we found the pit of a catchment
                     //now we get the area, depth, volume, and lowest catchment neighbor location
                     int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
                     int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};
                     LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                     while(list != nullptr)
                     {
                         int r = list->rowNr;
                         int c = list->colNr;
                         //here we do the code that needs to be done for each cell

                         for(int j = 1; j < LDD_DIR_LENGTH; j++)
                         {
                             if(j != LDD_PIT)
                             {
                                 int rn = r + dy[j];
                                 int cn = c + dx[j];

                                 if(rn > -1 && rn < map->data.nr_rows() && cn > -1 && cn < map->data.nr_cols())
                                 {
                                     //pit index of other catchment
                                     int pi_other = catchmentspi[rn][cn];
                                     LDD_PITPROPERTIES pp_other = PitList.at(pi_other);


                                     if( //&& (pp_other.pi_flowinto != i)
                                             !pcr::isMV(DEM->data[rn][cn])  && (catchmentsm[rn][cn] != catchmentunique) && LDD_IS_ACTUAL(map->data[rn][cn]))
                                     {
                                         float dem = (DEM->data[rn][cn]);

                                         if(dem < dem_lowestnb)
                                         {
                                             r_lowestnbconnect = r;
                                             c_lowestnbconnect = c;
                                             connectdir = j;
                                             r_lowestnb = rn;
                                             c_lowestnb = cn;
                                             dem_lowestnb = dem;
                                             pi_flowinto = pi_other;

                                         }
                                     }
                                 }
                             }
                         }

                         list = ListReplaceFirstByUSNB(map,list);
                     }

                 pp.r_lowestnbconnect =r_lowestnbconnect;
                 pp.c_lowestnbconnect =c_lowestnbconnect;
                 pp.dem_lowestnb = dem_lowestnb;
                 pp.dem_pit = dem_pit;
                 pp.connectdir = connectdir;
                 pp.r_lowestnb = r_lowestnb;
                 pp.c_lowestnb = c_lowestnb;
              }


              float precip_actual = 0;

              //get depression properties to determine wether we should remove
              if(pp.r_lowestnb != -1 && pp.c_lowestnb != -1)
              {
                  int ro = pp.rowNr;
                  int co = pp.colNr;

                  for(int j = 1; j < LDD_DIR_LENGTH; j++)
                  {
                      if(j != LDD_PIT)
                      {
                          int rn = ro + dy[j];
                          int cn = co + dx[j];

                          if(rn > -1 && rn < DEM->data.nr_rows() && cn > -1 && cn < DEM->data.nr_cols())
                          {
                              if(pcr::isMV(DEM->data[rn][cn]))
                                {
                                     pp.mvnb = true;
                                     break;
                                }
                          }else
                          {
                              pp.mvnb = false;
                              break;
                          }
                      }
                  }
                  LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                  while(list != nullptr)
                  {
                      int r = list->rowNr;
                      int c = list->colNr;

                      float dem = DEM->data[r][c];

                      if(dem < pp.dem_lowestnb)
                      {
                         //this cell is within the depression that must flow outwards,
                          pp.core_area += map->cellSize()* map->cellSize();
                          pp.core_volume += std::max(0.0f,pp.dem_lowestnb) - dem * map->cellSize()* map->cellSize();
                      }

                      pp.catchment_area += map->cellSize()* map->cellSize();
                      precip_actual +=map->cellSize()* map->cellSize() * precipitation->AS_IsSingleValue? precipitation->data[0][0]:precipitation->data[r][c];
                      list = ListReplaceFirstByUSNB(map,list);
                  }
             }

              pp.core_ouflowdepth = pp.dem_lowestnb -pp.dem_pit;


              //check if this pit matches the criteria
              bool fix = true;

              if((lddin) && pp.mvnb)
              {
                    fix = false;
              }
              if(pp.core_ouflowdepth > (outflowdepth->AS_IsSingleValue? outflowdepth->data[0][0] : outflowdepth->data[pp.rowNr][pp.colNr]))
              {
                    fix = false;
              }
              if(pp.core_area > (corearea->AS_IsSingleValue? corearea->data[0][0] : corearea->data[pp.rowNr][pp.colNr]))
              {
                    fix = false;
              }
              if(pp.core_volume > (corevolume->AS_IsSingleValue? corevolume->data[0][0] : corevolume->data[pp.rowNr][pp.colNr]))
              {
                    fix = false;
              }
              if(pp.core_volume > precip_actual * pp.core_area)
              {
                    fix = false;
              }


              if(fix && pp.r_lowestnbconnect > -1 && pp.c_lowestnbconnect > -1)
              {
                  pp.pi_flowinto = pi_flowinto;

                  //if we found a possible outflow point, reverse the ldd from the pit to this outflow point
                  ReversePath(map,pp.rowNr,pp.colNr,pp.r_lowestnbconnect,pp.c_lowestnbconnect);

                  //now we need to connect the connecting cell to the next catchment
                  map->data[pp.r_lowestnbconnect][pp.c_lowestnbconnect] = pp.connectdir;

                  double lowestdem = DEM->data[pp.r_lowestnbconnect][pp.c_lowestnbconnect];

                  PitList[i]= pp;

                  fixed_pit = true;

                  //if we are correcting a DEM, set all values within this catchment to be at least of the elevetion of the outflow point

                  if(demn != nullptr)
                  {
                      int ro = pp.rowNr;
                      int co = pp.colNr;

                      LDD_LINKEDLIST * list = GetListRoot(map,ro,co);

                      while(list != nullptr)
                      {
                          int r = list->rowNr;
                          int c = list->colNr;

                          float dem = DEM->data[r][c];

                          if(dem < pp.dem_lowestnb)
                          {
                             demn->data[r][c] = lowestdem;
                          }
                          list = ListReplaceFirstByUSNB(map,list);
                      }

                  }
              }

              catchmentunique ++;
         }

    }



    map->AS_IsLDD = true;
    return map;

}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap* outflowdepth,cTMap * corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,outflowdepth,corevolume,corearea,precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,cTMap * corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,corearea,precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,float corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,(outflowdepth),MapFactory(corevolume),corearea,precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,cTMap * corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,(outflowdepth),corevolume,MapFactory(corearea),precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,cTMap * corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,(outflowdepth),corevolume,corearea,MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,float corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),corearea,precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,cTMap * corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,MapFactory(corearea),precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,cTMap *  corevolume, cTMap * corearea,float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,corearea,MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,float corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,(outflowdepth),MapFactory(corevolume),MapFactory(corearea),precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap * outflowdepth,float corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,outflowdepth,MapFactory(corevolume),corearea,MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap *  outflowdepth,cTMap *  corevolume, float corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,(outflowdepth),corevolume,MapFactory(corearea),MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,float corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),MapFactory(corearea),precipitation, lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,float corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),corearea,MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float outflowdepth,cTMap *  corevolume, float corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,MapFactory(corearea),MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,cTMap *  outflowdepth, float corevolume, float corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,outflowdepth,MapFactory(corevolume),MapFactory(corearea),MapFactory(precipitation), lddin,nullptr);
}
inline cTMap * AS_DrainageNetwork(cTMap *  DEM,float  outflowdepth, float  corevolume, float corearea, float precipitation,bool lddin)
{
    return AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),MapFactory(corearea),MapFactory(precipitation), lddin,nullptr);
}

inline cTMap * AS_DrainageNetwork(cTMap *  DEM)
{
    return AS_DrainageNetwork(DEM,MapFactory(1e31f),MapFactory(1e31f),MapFactory(1e31f),MapFactory(1e31f),0.0, nullptr);
}












inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap* outflowdepth,cTMap * corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,outflowdepth,corevolume,corearea,precipitation, lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,cTMap * corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,corearea,precipitation, lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap * outflowdepth,float corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,(outflowdepth),MapFactory(corevolume),corearea,precipitation, lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap * outflowdepth,cTMap * corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,(outflowdepth),corevolume,MapFactory(corearea),precipitation, lddin,ret);
    delete ldd;
    return ret;

}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap * outflowdepth,cTMap * corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,(outflowdepth),corevolume,corearea,MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,float corevolume, cTMap * corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),corearea,precipitation, lddin,ret);
    delete ldd;
    return ret;

}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,cTMap * corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,MapFactory(corearea),precipitation, lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,cTMap *  corevolume, cTMap * corearea,float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,corearea,MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap * outflowdepth,float corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,(outflowdepth),MapFactory(corevolume),MapFactory(corearea),precipitation, lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap * outflowdepth,float corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,outflowdepth,MapFactory(corevolume),corearea,MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap *  outflowdepth,cTMap *  corevolume, float corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,(outflowdepth),corevolume,MapFactory(corearea),MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,float corevolume, float corearea, cTMap * precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),MapFactory(corearea),precipitation, lddin,ret);
    delete ldd;
    return ret;

}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,float corevolume, cTMap * corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),corearea,MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float outflowdepth,cTMap *  corevolume, float corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd =  AS_DrainageNetwork(DEM,MapFactory(outflowdepth),corevolume,MapFactory(corearea),MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;

}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,cTMap *  outflowdepth, float corevolume, float corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd =  AS_DrainageNetwork(DEM,outflowdepth,MapFactory(corevolume),MapFactory(corearea),MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;
}
inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM,float  outflowdepth, float  corevolume, float corearea, float precipitation,bool lddin)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(outflowdepth),MapFactory(corevolume),MapFactory(corearea),MapFactory(precipitation), lddin,ret);
    delete ldd;
    return ret;

}

inline cTMap * AS_DrainageNetworkDEM(cTMap *  DEM)
{
    cTMap * ret = DEM->GetCopy();
    cTMap * ldd = AS_DrainageNetwork(DEM,MapFactory(1e31f),MapFactory(1e31f),MapFactory(1e31f),MapFactory(1e31f),0.0,ret);
    delete ldd;
    return ret;
}
















inline cTMap * AS_DrainageMonotonicDownstream(cTMap * LDD,cTMap * prec)
{

    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for DrainageMonotonicDownstream (LDD) can not be non-spatial");
        throw 1;
    }

    if(prec->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for DrainageMonotonicDownstream (Value) can not be non-spatial");
        throw 1;
    }

        if(!(LDD->data.nr_rows() ==  prec->data.nr_rows() && LDD->data.nr_cols() == prec->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }


    MaskedRaster<float> mapdata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(mapdata),LDD->projection(),"");

    MaskedRaster<float> tmadata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *tma = new cTMap(std::move(tmadata),LDD->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(tma->data[ro][co]);

            if(pcr::isMV(LDD->data[ro][co]))
            {
                pcr::setMV(map->data[ro][co]);
            }
        }
    }


    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {

                int pitRowNr = ro;
                int pitColNr = co;

            int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
            int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

            /// Linked list of cells in order of LDD flow network, ordered from pit upwards
            LDD_LINKEDLIST *list = NULL, *temp = NULL;
            list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
            list->prev = NULL;
            /// start gridcell: outflow point of area
            list->rowNr = pitRowNr;
            list->colNr = pitColNr;

            // _Qsn->fill(0);
            // NOT wrong when multiple outlets! DO this befiore the loop
            //VJ 12/12/12 set output substance to zero

            while (list != NULL)
            {
                int i = 0;
                bool  subCachDone = true; // are sub-catchment cells done ?
                int rowNr = list->rowNr;
                int colNr = list->colNr;

                /** put all points that have to be calculated to calculate the current point in the list,
                 before the current point */
                for (i=1; i<=9; i++)
                {
                    int r, c;
                    int ldd = 0;

                    // this is the current cell
                    if (i==5)
                        continue;

                    r = rowNr+dy[i];
                    c = colNr+dx[i];

                    if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
                        ldd = (int) LDD->data[r][c];
                    else
                        continue;

                    // check if there are more cells upstream, if not subCatchDone remains true
                    if (pcr::isMV(tma->Drc) &&
                            FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                            INSIDE(LDD,r, c))
                    {
                        temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                        temp->prev = list;
                        list = temp;
                        list->rowNr = r;
                        list->colNr = c;
                        subCachDone = false;
                    }
                }

                // all cells above a cell are linked in a "sub-catchment or branch
                // continue with water and sed calculations
                // rowNr and colNr are the last upstreM cell linked
                if (subCachDone)
                {
                    double Qin=0.0;

                    //current cell

                    float value = prec->data[rowNr][colNr];
                    float value_new = value;

                    bool found = false;
                    float val_before = 0.0;
                    float val_before_n = 0.0;
                    float dif_with_before = 0.0;


                    // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
                    // note these are values of Qn and Qsn so the new flux
                    for (i=1;i<=9;i++)
                    {
                        int r, c, ldd = 0;

                        if (i==5)  // Skip current cell
                            continue;

                        r = rowNr+dy[i];
                        c = colNr+dx[i];

                        if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->Drc))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        if (INSIDE(LDD,r, c) &&
                                FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                                !pcr::isMV(LDD->Drc) )
                        {

                            //incoming cell
                            double val_before_this = map->data[r][c];
                            float val_before_new = map->data[r][c];
                            float dif = std::max(0.0f,value - prec->data[r][c]);

                            if(val_before_this > val_before || found == false)
                            {
                                val_before = val_before_this;
                                val_before_n = val_before_new;
                                dif_with_before = dif;
                            }
                            found = true;
                        }
                    }


                    map->data[rowNr][colNr] = val_before_n + dif_with_before;

                    /* cell rowN, colNr is now done */
                    tma->data[rowNr][colNr] = 0.0;

                    temp=list;
                    list=list->prev;
                    free(temp);
                    // go to the previous cell in the list

                }/* eof subcatchment done */
            } /* eowhile list != NULL */
        }
    }}

    delete tma;
    return map;
}



inline cTMap * AS_Accuflux(cTMap * LDD,cTMap * prec)
{

    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Accuflux (LDD) can not be non-spatial");
        throw 1;
    }

    if(!prec->AS_IsSingleValue)
    {
        if(!(LDD->data.nr_rows() ==  prec->data.nr_rows() && LDD->data.nr_cols() == prec->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> mapdata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(mapdata),LDD->projection(),"");

    MaskedRaster<float> tmadata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *tma = new cTMap(std::move(tmadata),LDD->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(tma->data[ro][co]);

            if(pcr::isMV(LDD->data[ro][co]))
            {
                pcr::setMV(map->data[ro][co]);
            }
        }
    }


    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {

                int pitRowNr = ro;
                int pitColNr = co;

            int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
            int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

            /// Linked list of cells in order of LDD flow network, ordered from pit upwards
            LDD_LINKEDLIST *list = NULL, *temp = NULL;
            list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
            list->prev = NULL;
            /// start gridcell: outflow point of area
            list->rowNr = pitRowNr;
            list->colNr = pitColNr;

            // _Qsn->fill(0);
            // NOT wrong when multiple outlets! DO this befiore the loop
            //VJ 12/12/12 set output substance to zero

            while (list != NULL)
            {
                int i = 0;
                bool  subCachDone = true; // are sub-catchment cells done ?
                int rowNr = list->rowNr;
                int colNr = list->colNr;

                /** put all points that have to be calculated to calculate the current point in the list,
                 before the current point */
                for (i=1; i<=9; i++)
                {
                    int r, c;
                    int ldd = 0;

                    // this is the current cell
                    if (i==5)
                        continue;

                    r = rowNr+dy[i];
                    c = colNr+dx[i];

                    if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
                        ldd = (int) LDD->data[r][c];
                    else
                        continue;

                    // check if there are more cells upstream, if not subCatchDone remains true
                    if (pcr::isMV(tma->Drc) &&
                            FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                            INSIDE(LDD,r, c))
                    {
                        temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                        temp->prev = list;
                        list = temp;
                        list->rowNr = r;
                        list->colNr = c;
                        subCachDone = false;
                    }
                }

                // all cells above a cell are linked in a "sub-catchment or branch
                // continue with water and sed calculations
                // rowNr and colNr are the last upstreM cell linked
                if (subCachDone)
                {
                    double Qin=0.0;

                    // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
                    // note these are values of Qn and Qsn so the new flux
                    for (i=1;i<=9;i++)
                    {
                        int r, c, ldd = 0;

                        if (i==5)  // Skip current cell
                            continue;

                        r = rowNr+dy[i];
                        c = colNr+dx[i];

                        if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->Drc))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        if (INSIDE(LDD,r, c) &&
                                FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                                !pcr::isMV(LDD->Drc) )
                        {

                            //incoming cell
                            Qin += map->data[r][c];

                        }
                    }

                    //current cell

                    float precip = (!prec->AS_IsSingleValue) ? prec->data[rowNr][colNr] : prec->data[0][0];
                    map->data[rowNr][colNr] = Qin + map->cellSize()*map->cellSize() * precip;

                    /* cell rowN, colNr is now done */
                    tma->data[rowNr][colNr] = 0.0;

                    temp=list;
                    list=list->prev;
                    free(temp);
                    // go to the previous cell in the list

                }/* eof subcatchment done */
            } /* eowhile list != NULL */
        }
    }}

    delete tma;
    return map;

}

inline cTMap * AS_Accuflux(cTMap * LDD,float prec)
{

    return AS_Accuflux(LDD,MapFactory(prec));
}
inline cTMap * AS_Accuflux(cTMap * ldd)
{
    return AS_Accuflux(ldd,MapFactory(1.0f));
}



inline cTMap * AS_AccufluxThreshold(cTMap * LDD,cTMap * prec, cTMap * fluxmax)
{

    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Accuflux (LDD) can not be non-spatial");
        throw 1;
    }

    if(!prec->AS_IsSingleValue)
    {
        if(!(LDD->data.nr_rows() ==  prec->data.nr_rows() && LDD->data.nr_cols() == prec->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(LDD->data.nr_rows() ==  fluxmax->data.nr_rows() && LDD->data.nr_cols() == fluxmax->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> mapdata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(mapdata),LDD->projection(),"");

    MaskedRaster<float> tmadata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *tma = new cTMap(std::move(tmadata),LDD->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(tma->data[ro][co]);

            if(pcr::isMV(LDD->data[ro][co]))
            {
                pcr::setMV(map->data[ro][co]);
            }
        }
    }


    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {

                int pitRowNr = ro;
                int pitColNr = co;

            int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
            int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

            /// Linked list of cells in order of LDD flow network, ordered from pit upwards
            LDD_LINKEDLIST *list = NULL, *temp = NULL;
            list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
            list->prev = NULL;
            /// start gridcell: outflow point of area
            list->rowNr = pitRowNr;
            list->colNr = pitColNr;

            // _Qsn->fill(0);
            // NOT wrong when multiple outlets! DO this befiore the loop
            //VJ 12/12/12 set output substance to zero

            while (list != NULL)
            {
                int i = 0;
                bool  subCachDone = true; // are sub-catchment cells done ?
                int rowNr = list->rowNr;
                int colNr = list->colNr;

                /** put all points that have to be calculated to calculate the current point in the list,
                 before the current point */
                for (i=1; i<=9; i++)
                {
                    int r, c;
                    int ldd = 0;

                    // this is the current cell
                    if (i==5)
                        continue;

                    r = rowNr+dy[i];
                    c = colNr+dx[i];

                    if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
                        ldd = (int) LDD->data[r][c];
                    else
                        continue;

                    // check if there are more cells upstream, if not subCatchDone remains true
                    if (pcr::isMV(tma->Drc) &&
                            FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                            INSIDE(LDD,r, c))
                    {
                        temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                        temp->prev = list;
                        list = temp;
                        list->rowNr = r;
                        list->colNr = c;
                        subCachDone = false;
                    }
                }

                // all cells above a cell are linked in a "sub-catchment or branch
                // continue with water and sed calculations
                // rowNr and colNr are the last upstreM cell linked
                if (subCachDone)
                {
                    double Qin=0.0;

                    // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
                    // note these are values of Qn and Qsn so the new flux
                    for (i=1;i<=9;i++)
                    {
                        int r, c, ldd = 0;

                        if (i==5)  // Skip current cell
                            continue;

                        r = rowNr+dy[i];
                        c = colNr+dx[i];

                        if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->Drc))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        if (INSIDE(LDD,r, c) &&
                                FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                                !pcr::isMV(LDD->Drc) )
                        {

                            //incoming cell
                            Qin += pcr::isMV(fluxmax->data[r][c])? map->data[r][c]:std::max(0.0f,std::min(fluxmax->data[r][c],map->data[r][c]));

                        }
                    }

                    //current cell

                    float precip = (!prec->AS_IsSingleValue) ? prec->data[rowNr][colNr] : prec->data[0][0];
                    map->data[rowNr][colNr] = Qin + map->cellSize()*map->cellSize() * precip;

                    /* cell rowN, colNr is now done */
                    tma->data[rowNr][colNr] = 0.0;

                    temp=list;
                    list=list->prev;
                    free(temp);
                    // go to the previous cell in the list

                }/* eof subcatchment done */
            } /* eowhile list != NULL */
        }
    }}

    delete tma;
    return map;

}

inline cTMap * AS_AccufluxThreshold(cTMap * LDD,float prec, cTMap * maxflux)
{
    return AS_AccufluxThreshold(LDD,MapFactory(prec),maxflux);
}
inline cTMap * AS_AccufluxThreshold(cTMap * ldd, cTMap * maxflux)
{
    return AS_AccufluxThreshold(ldd,MapFactory(1.0f),maxflux);
}





inline cTMap * AS_Outlets(cTMap *LDD)
{
    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    MaskedRaster<float> raster_data(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),LDD->projection(),"");

    int catchment = 1;

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(LDD->data[r][c]))
            {
                float val_current = LDD->data[r][c];
                if(((int)(val_current)) == LDD_PIT)
                {
                    map->data[r][c] = catchment;

                    catchment++;
                }else {
                    map->data[r][c] = 0.0;
                }
            }else {
                pcr::setMV(map->data[r][c]);
            }

        }
    }
    return map;


}
inline cTMap * AS_Catchments(cTMap * LDD )
{
    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Catchments (LDD) can not be non-spatial");
        throw 1;
    }

    MaskedRaster<float> raster_data(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),LDD->projection(),"");


    int catchment = 1;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(map->data[ro][co]);
        }
    }
    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {
                float val_current = LDD->data[ro][co];
                if(((int)(val_current)) == LDD_PIT)
                {

                    LDD_LINKEDLIST * list = GetListRoot(LDD,ro,co);

                    while(list != nullptr)
                    {
                        int r = list->rowNr;
                        int c = list->colNr;

                        map->data[r][c] = catchment;

                        list = ListReplaceFirstByUSNB(LDD,list);
                    }
                }

                catchment ++;
            }else {

            }

        }
    }

    return map;
}

inline cTMap * AS_Catchments(cTMap * LDD,cTMap * Outlets)
{
    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Catchments (LDD) can not be non-spatial");
        throw 1;
    }

    if(Outlets->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for Catchments (points) can not be non-spatial");
        throw 1;
    }


    if(!(LDD->data.nr_rows() == Outlets->data.nr_rows() && LDD->data.nr_cols() == Outlets->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),LDD->projection(),"");


    int catchment = 1;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(map->data[ro][co]);
        }
    }
    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {
                float val_current = Outlets->data[ro][co];
                if(((int)(val_current)) > 0)
                {

                    LDD_LINKEDLIST * list = GetListRoot(LDD,ro,co);

                    while(list != nullptr)
                    {
                        int r = list->rowNr;
                        int c = list->colNr;

                        map->data[r][c] = catchment;

                        catchment ++;
                        list = ListReplaceFirstByUSNB(LDD,list);
                    }
                }
            }
        }
    }

    return map;
}
inline cTMap * AS_StreamOrder(cTMap * LDD)
{
    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for Accuflux (LDD) can not be non-spatial");
        throw 1;
    }

    MaskedRaster<float> mapdata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(mapdata),LDD->projection(),"");

    MaskedRaster<float> tmadata(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(), LDD->data.cell_sizeY());
    cTMap *tma = new cTMap(std::move(tmadata),LDD->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(tma->data[ro][co]);

            if(pcr::isMV(LDD->data[ro][co]))
            {
                pcr::setMV(map->data[ro][co]);
            }
        }
    }


    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            if(!pcr::isMV(LDD->data[ro][co]))
            {

                int pitRowNr = ro;
                int pitColNr = co;

            int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
            int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

            /// Linked list of cells in order of LDD flow network, ordered from pit upwards
            LDD_LINKEDLIST *list = NULL, *temp = NULL;
            list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
            list->prev = NULL;
            /// start gridcell: outflow point of area
            list->rowNr = pitRowNr;
            list->colNr = pitColNr;

            // _Qsn->fill(0);
            // NOT wrong when multiple outlets! DO this befiore the loop
            //VJ 12/12/12 set output substance to zero

            while (list != NULL)
            {
                int i = 0;
                bool  subCachDone = true; // are sub-catchment cells done ?
                int rowNr = list->rowNr;
                int colNr = list->colNr;

                /** put all points that have to be calculated to calculate the current point in the list,
                 before the current point */
                for (i=1; i<=9; i++)
                {
                    int r, c;
                    int ldd = 0;

                    // this is the current cell
                    if (i==5)
                        continue;

                    r = rowNr+dy[i];
                    c = colNr+dx[i];

                    if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
                        ldd = (int) LDD->data[r][c];
                    else
                        continue;

                    // check if there are more cells upstream, if not subCatchDone remains true
                    if (pcr::isMV(tma->Drc) &&
                            FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                            INSIDE(LDD,r, c))
                    {
                        temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                        temp->prev = list;
                        list = temp;
                        list->rowNr = r;
                        list->colNr = c;
                        subCachDone = false;
                    }
                }

                // all cells above a cell are linked in a "sub-catchment or branch
                // continue with water and sed calculations
                // rowNr and colNr are the last upstreM cell linked
                if (subCachDone)
                {
                    float soin=0.0;

                    // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
                    // note these are values of Qn and Qsn so the new flux
                    for (i=1;i<=9;i++)
                    {
                        int r, c, ldd = 0;

                        if (i==5)  // Skip current cell
                            continue;

                        r = rowNr+dy[i];
                        c = colNr+dx[i];

                        if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->Drc))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        if (INSIDE(LDD,r, c) &&
                                FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                                !pcr::isMV(LDD->Drc) )
                        {

                            //incoming cell

                            if((int)soin == (int)map->data[r][c])
                            {
                                soin = soin +1;
                            }else {
                                soin = std::max(soin,map->data[r][c]);
                            }

                        }
                    }

                    //current cell

                    map->data[rowNr][colNr] = soin;

                    /* cell rowN, colNr is now done */
                    tma->data[rowNr][colNr] = 0.0;

                    temp=list;
                    list=list->prev;
                    free(temp);
                    // go to the previous cell in the list

                }/* eof subcatchment done */
            } /* eowhile list != NULL */
        }
    }}

    delete tma;
    return map;


}


inline cTMap * AS_SpreadLDD(cTMap * LDD,cTMap * points, cTMap * friction_source, cTMap * friction)
{
    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(LDD->data.nr_rows() ==  points->data.nr_rows() &&LDD->data.nr_cols() ==  points->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    if(!(friction_source->AS_IsSingleValue))
    {
        if(!(friction_source->data.nr_rows() ==  points->data.nr_rows() && friction_source->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(friction->AS_IsSingleValue))
    {
        if(!(friction->data.nr_rows() ==  points->data.nr_rows() && friction->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");


    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_source->AS_IsSingleValue? friction_source->data[0][0]:friction_source->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }

    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};



    float _dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;

        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(LDD->data[r][c]) &&  !pcr::isMV(v_points))
                {

                    float val;
                    for (int i=1; i<=9; i++)
                    {
                        // this is the current cell
                        if (i==5)
                            continue;

                        // look around in 8 directions
                        int row = r+dy[i];
                        int col = c+dx[i];
                        int ldd = 0;

                        if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        // if no MVs and row,col flows to central cell r,c
                        if (  //INSIDE(row, col) &&
                              // !pcr::isMV(_LDD->data[row][col]) &&
                              FLOWS_TO(ldd, row, col, r, c)
                              )
                        {

                            float vn_points = points->data[row][col];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                            float vn_current = map->data[row][col];

                            float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;

                            }
                        }
                    }
                    if(!pcr::isMV(LDD->data[r][c]))
                    {
                        float val;

                        int ldd = (int) LDD->data[r][c];

                        if(ldd == 5)
                        {
                        }else if(ldd > 0 && ldd < 10)
                        {
                            int row = r+dy[ldd];
                            int col = c+dx[ldd];

                            if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                            {
                                float vn_points = points->data[row][col];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                                float vn_current = map->data[row][col];

                                float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;

                                }


                            }else {
                            }
                        }else {
                        }
                    }

                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }

    return map;
}

inline cTMap * AS_SpreadLDDZone(cTMap * LDD,cTMap * points, cTMap * friction_source, cTMap * friction)
{
    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(LDD->data.nr_rows() ==  points->data.nr_rows() &&LDD->data.nr_cols() ==  points->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    if(!(friction_source->AS_IsSingleValue))
    {
        if(!(friction_source->data.nr_rows() ==  points->data.nr_rows() && friction_source->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(friction->AS_IsSingleValue))
    {
        if(!(friction->data.nr_rows() ==  points->data.nr_rows() && friction->data.nr_cols() ==  points->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }
    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");
    MaskedRaster<float> raster_dataz(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *mapz = new cTMap(std::move(raster_dataz),points->projection(),"");

    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_source->AS_IsSingleValue? friction_source->data[0][0]:friction_source->data[r][c];
                map->data[r][c] = v_fricstart;
                mapz->data[r][c] = points->data[r][c];

            }else {
                map->data[r][c] = 1e31;
                pcr::setMV(map->data[r][c]);
            }
        }
    }
    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};



    float _dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;

        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(LDD->data[r][c]) &&  !pcr::isMV(v_points))
                {

                    float val;
                    for (int i=1; i<=9; i++)
                    {
                        // this is the current cell
                        if (i==5)
                            continue;

                        // look around in 8 directions
                        int row = r+dy[i];
                        int col = c+dx[i];
                        int ldd = 0;

                        if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                            ldd = (int) LDD->Drc;
                        else
                            continue;

                        // if no MVs and row,col flows to central cell r,c
                        if (  //INSIDE(row, col) &&
                              // !pcr::isMV(_LDD->data[row][col]) &&
                              FLOWS_TO(ldd, row, col, r, c)
                              )
                        {

                            float vn_points = points->data[row][col];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                            float vn_current = map->data[row][col];

                            float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;
                                mapz->data[r][c] =  mapz->data[row][col];

                            }
                        }
                    }
                    if(!pcr::isMV(LDD->data[r][c]))
                    {
                        float val;

                        int ldd = (int) LDD->data[r][c];

                        if(ldd == 5)
                        {
                           val = 0.0;
                        }else if(ldd > 0 && ldd < 10)
                        {
                            int row = r+dy[ldd];
                            int col = c+dx[ldd];

                            if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                            {
                                float vn_points = points->data[row][col];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                                float vn_current = map->data[row][col];

                                float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                    mapz->data[r][c] =  mapz->data[row][col];

                                }


                            }else {
                                pcr::setMV(val);
                            }
                        }else {
                            pcr::setMV(val);
                        }
                    }

                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }

    delete map;
    return mapz;
}


inline cTMap * AS_SpreadLDDZone(cTMap * LDD,cTMap * points, float friction_source, cTMap * friction)
{
    return AS_SpreadLDDZone(LDD,points, MapFactory(friction_source),friction);
}
inline cTMap * AS_SpreadLDDZone(cTMap * LDD,cTMap * points, cTMap * friction_source, float friction)
{
    return AS_SpreadLDDZone(LDD,points, friction_source,MapFactory(friction));
}
inline cTMap * AS_SpreadLDDZone(cTMap * LDD,cTMap * points, float friction_source, float friction)
{
    return AS_SpreadLDDZone(LDD,points, MapFactory(friction_source),MapFactory(friction));
}

inline cTMap * AS_SpreadLDD(cTMap * LDD,cTMap * points, float friction_source, cTMap * friction)
{
    return AS_SpreadLDD(LDD,points, MapFactory(friction_source),friction);
}
inline cTMap * AS_SpreadLDD(cTMap * LDD,cTMap * points, cTMap * friction_source, float friction)
{
    return AS_SpreadLDD(LDD,points, friction_source,MapFactory(friction));
}
inline cTMap * AS_SpreadLDD(cTMap * LDD,cTMap * points, float friction_source, float friction)
{
    return AS_SpreadLDD(LDD,points, MapFactory(friction_source),MapFactory(friction));
}


inline cTMap * AS_DownStream(cTMap *_LDD, cTMap *_M)
{
    if(!(_LDD->data.nr_rows() == _M->data.nr_rows() && _LDD->data.nr_cols() == _M->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(_M->data.nr_rows(), _M->data.nr_cols(), _M->data.north(), _M->data.west(), _M->data.cell_size(),_M->data.cell_sizeY());
    cTMap *out = new cTMap(std::move(raster_data),_M->projection(),"");


    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

    for(int r = 0; r < _M->data.nr_rows();r++)
    {
        for(int c = 0; c < _M->data.nr_cols();c++)
        {
            if(!pcr::isMV(_LDD->data[r][c]))
            {
                float val;

                int ldd = (int) _LDD->data[r][c];

                if(ldd == 5)
                {
                   val = 0.0;
                }else if(ldd > 0 && ldd < 10)
                {
                    int row = r+dy[ldd];
                    int col = c+dx[ldd];

                    if (INSIDE(_LDD,row, col) && !pcr::isMV(_LDD->data[row][col]))
                    {
                        val = _M->data[row][col];

                    }else {
                        pcr::setMV(val);
                    }
                }else {
                    pcr::setMV(val);
                }

                out->Drc = val;
            }else {
                pcr::setMV(out->data[r][c]);
            }
        }
    }

        return out;
}



inline cTMap * AS_UpStream(cTMap *_LDD, cTMap *_M)
{
    if(!(_LDD->data.nr_rows() == _M->data.nr_rows() && _LDD->data.nr_cols() == _M->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    MaskedRaster<float> raster_data(_M->data.nr_rows(), _M->data.nr_cols(), _M->data.north(), _M->data.west(), _M->data.cell_size(),_M->data.cell_sizeY());
    cTMap *out = new cTMap(std::move(raster_data),_M->projection(),"");

    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

    for(int r = 0; r < _M->data.nr_rows();r++)
    {
        for(int c = 0; c < _M->data.nr_cols();c++)
        {
            if(!pcr::isMV(_LDD->data[r][c]))
            {

            double tot = 0;
            for (int i=1; i<=9; i++)
            {
                // this is the current cell
                if (i==5)
                    continue;

                // look around in 8 directions
                int row = r+dy[i];
                int col = c+dx[i];
                int ldd = 0;

                if (INSIDE(_LDD,row, col) && !pcr::isMV(_LDD->data[row][col]))
                    ldd = (int) _LDD->Drc;
                else
                    continue;

                // if no MVs and row,col flows to central cell r,c
                if (  //INSIDE(row, col) &&
                      // !pcr::isMV(_LDD->data[row][col]) &&
                      FLOWS_TO(ldd, row, col, r, c)
                      )
                {
                    tot += _M->data[row][col];
                }
            }
            out->Drc = tot;
            }else {
                pcr::setMV(out->data[r][c]);
            }
        }
    }

        return out;
}





inline cTMap * AS_SlopeLength(cTMap * LDD, cTMap * friction_source, cTMap * friction, bool two_way = true)
{
    if(LDD->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for SlopeLength (LDD) can not be non-spatial");
        throw 1;
    }

    if(!(friction_source->AS_IsSingleValue))
    {
        if(!(friction_source->data.nr_rows() ==  LDD->data.nr_rows() && friction_source->data.nr_cols() ==  LDD->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }

    if(!(friction->AS_IsSingleValue))
    {
        if(!(friction->data.nr_rows() ==  LDD->data.nr_rows() && friction->data.nr_cols() ==  LDD->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }
    }



    MaskedRaster<float> raster_data0(LDD->data.nr_rows(), LDD->data.nr_cols(), LDD->data.north(), LDD->data.west(), LDD->data.cell_size(),LDD->data.cell_sizeY());
    cTMap *points = new cTMap(std::move(raster_data0),LDD->projection(),"");

    MaskedRaster<float> raster_data(points->data.nr_rows(), points->data.nr_cols(), points->data.north(), points->data.west(), points->data.cell_size(),points->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),points->projection(),"");



    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};



    float _dx = map->cellSize();


    //set up the initial points at the watershed devide
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {

            int n_flowto = 0;

            if(!pcr::isMV(LDD->data[r][c]) )
            {

                float val;
                for (int i=1; i<=10; i++)
                {
                    // this is the current cell
                    if (i==5)
                    {
                        continue;
                    }

                    // look around in 8 directions
                    int row = r+dy[i];
                    int col = c+dx[i];
                    int ldd = 0;

                    if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                        ldd = (int) LDD->Drc;
                    else
                        continue;

                    // if no MVs and row,col flows to central cell r,c
                    if (  //INSIDE(row, col) &&
                          // !pcr::isMV(_LDD->data[row][col]) &&
                          FLOWS_TO(ldd, row, col, r, c)
                          )
                    {

                        n_flowto ++;
                    }
                }

            }

            if(n_flowto == 0)
            {
                points->data[r][c] = 1.0;
            }
        }
    }

    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_source->AS_IsSingleValue? friction_source->data[0][0]:friction_source->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;

        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(LDD->data[r][c]) &&  !pcr::isMV(v_points))
                {

                    //if(two_way)
                    {

                        float val;
                        for (int i=1; i<=10; i++)
                        {
                            // this is the current cell
                            if (i==5)
                                continue;

                            // look around in 8 directions
                            int row = r+dy[i];
                            int col = c+dx[i];
                            int ldd = 0;

                            if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                                ldd = (int) LDD->Drc;
                            else
                                continue;

                            // if no MVs and row,col flows to central cell r,c
                            if (  //INSIDE(row, col) &&
                                  // !pcr::isMV(_LDD->data[row][col]) &&
                                  FLOWS_TO(ldd, row, col, r, c)
                                  )
                            {

                                float vn_points = points->data[row][col];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                                float vn_current = map->data[row][col];

                                float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                                if(v_new < v_current)
                                {
                                    if(!two_way)
                                    {

                                        change  = true;
                                        map->data[r][c] = v_new;
                                        v_current = v_new;
                                    }


                                }
                            }
                        }
                    }
                    if(!pcr::isMV(LDD->data[r][c]) )
                    {
                        float val;

                        int ldd = (int) LDD->data[r][c];

                        if(ldd == 5)
                        {
                        }else if(ldd > 0 && ldd < 10)
                        {
                            int row = r+dy[ldd];
                            int col = c+dx[ldd];

                            if (INSIDE(LDD,row, col) && !pcr::isMV(LDD->data[row][col]))
                            {
                                float vn_points = points->data[row][col];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[row][col];
                                float vn_current = map->data[row][col];

                                float v_new = vn_current + std::fabs((0.5f * vn_fric + v_fric)*_dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    if(two_way)
                                    {
                                        map->data[r][c] = v_new;
                                    }else {
                                        map->data[r][c] = v_new;
                                    }
                                    v_current = v_new;

                                }


                            }else {
                            }
                        }else {
                        }
                    }

                }else {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
    }

    return map;
}


inline cTMap * AS_SlopeLength(cTMap * LDD, float friction_source, cTMap * friction, bool two_way)
{
    return AS_SlopeLength(LDD, MapFactory(friction_source),friction, two_way);
}
inline cTMap * AS_SlopeLength(cTMap * LDD, cTMap * friction_source, float friction, bool two_way)
{
    return AS_SlopeLength(LDD, friction_source,MapFactory(friction,two_way));
}
inline cTMap * AS_SlopeLength(cTMap * LDD, float friction_source, float friction, bool two_way)
{
    return AS_SlopeLength(LDD, MapFactory(friction_source),MapFactory(friction), two_way);
}

