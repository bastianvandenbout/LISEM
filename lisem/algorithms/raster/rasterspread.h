#ifndef RASTERSPREAD_H
#define RASTERSPREAD_H


#include "rasterconstructors.h"
#include "geo/raster/map.h"
#include "rasterderivative.h"



//state-full spread algorithm using direct delta-propagation and adaptive flowpaths based on pressure scaling
inline cTMap * AS_SpreadFlowMDCP(cTMap * source ,cTMap * fracx1o, cTMap * fracx2o, cTMap * fracy1o,cTMap * fracy2o, cTMap* scale, float power, int iter_max = 0)
{

    MaskedRaster<float> raster_data(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *mx1 = new cTMap(std::move(raster_data),source->projection(),"");
       MaskedRaster<float> raster_data2(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *my1 = new cTMap(std::move(raster_data2),source->projection(),"");
       MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *mx2 = new cTMap(std::move(raster_data3),source->projection(),"");
       MaskedRaster<float> raster_data4(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *my2 = new cTMap(std::move(raster_data4),source->projection(),"");


       MaskedRaster<float> raster_data6(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *mx1o = new cTMap(std::move(raster_data6),source->projection(),"");
       MaskedRaster<float> raster_data7(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *my1o = new cTMap(std::move(raster_data7),source->projection(),"");
       MaskedRaster<float> raster_data8(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *mx2o = new cTMap(std::move(raster_data8),source->projection(),"");
       MaskedRaster<float> raster_data9(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *my2o = new cTMap(std::move(raster_data9),source->projection(),"");


       MaskedRaster<float> raster_data10(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *fracx1 = new cTMap(std::move(raster_data10),source->projection(),"");
       MaskedRaster<float> raster_data11(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *fracx2 = new cTMap(std::move(raster_data11),source->projection(),"");
       MaskedRaster<float> raster_data12(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *fracy1 = new cTMap(std::move(raster_data12),source->projection(),"");
       MaskedRaster<float> raster_data13(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *fracy2 = new cTMap(std::move(raster_data13),source->projection(),"");


       MaskedRaster<float> raster_data5(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
       cTMap *map = new cTMap(std::move(raster_data5),source->projection(),"");


       //initialize the map with friction values

       #pragma omp parallel for collapse(2)
       for(int r = 0; r < map->data.nr_rows();r++)
       {
           for(int c = 0; c < map->data.nr_cols();c++)
           {

               fracx1->data[r][c] = fracx1o->data[r][c];
               fracx2->data[r][c] = fracx2o->data[r][c];
               fracy1->data[r][c] = fracy1o->data[r][c];
               fracy2->data[r][c] = fracy2o->data[r][c];


               mx1o->data[r][c] = 0.0;
               my1o->data[r][c] = 0.0;
               mx2o->data[r][c] = 0.0;
               my2o->data[r][c] = 0.0;
               if(pcr::isMV(source->data[r][c]))
               {
                   pcr::setMV(map->data[r][c]);
               }else {
                   map->data[r][c] = 0.0;

               }
           }
       }

       float dx = map->cellSize();

       //we keep iterating through this algorithm untill there is no change left to make
       bool change = true;
       bool first = true;

       int iter = 0;

       while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
       {
           std::cout << "iter " << iter << " " << iter_max << " " << std::endl;
           iter ++;
           change = false;


           //first do the new directions

           /*for(int r = 0; r < map->data.nr_rows();r++)
           {
               for(int c = 0; c < map->data.nr_cols();c++)
               {
                   float v_points = source->data[r][c];
                   if(!pcr::isMV(v_points))
                   {
                       //get surrounding heights

                       float m = map->data[r][c];
                       float mx1 = !OUTORMV(map,r,c-1)? map->data[r][c-1] : m;
                       float mx2 = !OUTORMV(map,r,c+1)? map->data[r][c+1] : m;
                       float my1 = !OUTORMV(map,r-1,c)? map->data[r-1][c] : m;
                       float my2 = !OUTORMV(map,r+1,c)? map->data[r+1][c] : m;

                       float mh = std::pow(scale->data[r][c] * m,power);
                       float mhx1 = std::pow(scale->data[r][c] * mx1,power);
                       float mhx2 = std::pow(scale->data[r][c] * mx2,power);
                       float mhy1 = std::pow(scale->data[r][c] * my1,power);
                       float mhy2 = std::pow(scale->data[r][c] * my2,power);

                       //get pressure ratio



                       //fracx1->data[r][c] = fracx1o->data[r][c];
                       //fracx2->data[r][c] = fracx2o->data[r][c];
                       //fracy1->data[r][c] = fracy1o->data[r][c];
                       //fracy2->data[r][c] = fracy2o->data[r][c];

                   }
               }
           }*/

           //then calculate the new fluxes
           for(int r = 0; r < map->data.nr_rows();r++)
           {
               for(int c = 0; c < map->data.nr_cols();c++)
               {
                   float v_points = source->data[r][c];
                   if(!pcr::isMV(v_points))
                   {
                       float mh = map->data[r][c] + source->data[r][c];
                       mx1->data[r][c] = fracx1->data[r][c] * mh;
                       my1->data[r][c] = fracy1->data[r][c] * mh;
                       mx2->data[r][c] = fracx2->data[r][c] * mh;
                       my2->data[r][c] = fracy2->data[r][c] * mh;

                       map->data[r][c] = 0;

                   }
               }
           }

           if(true)
           {

               //first we move in right-lower direction
               for(int r = 0; r < map->data.nr_rows();r++)
               {
                   for(int c = 0; c < map->data.nr_cols();c++)
                   {
                       float v_points = source->data[r][c];
                       if(!pcr::isMV(v_points))
                       {

                           float mxf1 = mx1->data[r][c];
                           float myf1 = my1->data[r][c];
                           float mxf2 = mx2->data[r][c];
                           float myf2 = my2->data[r][c];
                           if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                           {
                               {
                                   float delta = myf2 - my2o->data[r][c];
                                   if(delta < 0.0)
                                   {
                                       std::cout << "delta " << delta << " " << r << " "  << c <<  " " << mmyf2 << " " << my2o->data[r][c] << std::endl;
                                   }
                                   mx1->data[r+1][c] += delta * fracx1->data[r+1][c];
                                   my1->data[r+1][c] += delta * fracy1->data[r+1][c];
                                   mx2->data[r+1][c] += delta * fracx2->data[r+1][c];
                                   my2->data[r+1][c] += delta * fracy2->data[r+1][c];

                                   change  = true;
                                   map->data[r+1][c] += myf2 + delta;
                                   my2o->data[r][c] = myf2 + delta;

                               }

                               my2->data[r][c] = 0.0;
                           }

                           if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                           {
                               {
                                   float delta = mxf2 - mx2o->data[r][c];
                                   if(delta < 0.0)
                                   {
                                       std::cout << "delta " << delta << " " << r << " "  << c <<  " " << xmf2 << " " << mx2o->data[r][c] << std::endl;
                                   }
                                   mx1->data[r][c+1] += delta * fracx1->data[r][c+1];
                                   my1->data[r][c+1] += delta * fracy1->data[r][c+1];
                                   mx2->data[r][c+1] += delta * fracx2->data[r][c+1];
                                   my2->data[r][c+1] += delta * fracy2->data[r][c+1];
                                   change  = true;
                                   map->data[r][c+1] += mxf2 + delta;
                                   mx2o->data[r][c] = mxf2 + delta;

                               }

                               mx2->data[r][c] = 0.0;
                           }

                       }
                   }
               }

               //then we move in left-upper direction
               /*for(int r = map->data.nr_rows()-1; r > -1 ;r--)
               {
                   for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                   {
                       float v_points = source->data[r][c];
                       if(!pcr::isMV(v_points))
                       {

                           float mxf1 = mx1->data[r][c];
                           float myf1 = my1->data[r][c];
                           float mxf2 = mx2->data[r][c];
                           float myf2 = my2->data[r][c];
                           if((r-1 > -1) && myf1 < -1e-10)
                           {
                               {
                                   float delta = myf1 - my1o->data[r][c];
                                   mx1->data[r-1][c] -= delta * fracx1->data[r-1][c];
                                   my1->data[r-1][c] -= delta * fracy1->data[r-1][c];
                                   mx2->data[r-1][c] -= delta * fracx2->data[r-1][c];
                                   my2->data[r-1][c] -= delta * fracy2->data[r-1][c];
                                   change  = true;
                                   map->data[r-1][c] -= myf1 + delta;
                                   my1o->data[r][c] = myf1 + delta;
                               }

                               my1->data[r][c] = 0.0;
                           }

                           if((c-1 > -1) && mxf1 < -1e-10)
                           {
                               {
                                   float delta = mxf1 - mx1o->data[r][c];
                                   mx1->data[r][c-1] -= delta * fracx1->data[r][c-1];
                                   my1->data[r][c-1] -= delta * fracy1->data[r][c-1];
                                   mx2->data[r][c-1] -= delta * fracx2->data[r][c-1];
                                   my2->data[r][c-1] -= delta * fracy2->data[r][c-1];
                                   change  = true;
                                   map->data[r][c-1] -= mxf1 + delta;
                                   mx1o->data[r][c] = mxf1 + delta;

                               }

                               mx1->data[r][c] = 0.0;
                           }
                       }


                   }
               }*/
           }else
           {

               //first we move in right-lower direction
               for(int r = 0; r < map->data.nr_rows();r++)
               {

                   for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                   {
                       float v_points = source->data[r][c];
                       if(!pcr::isMV(v_points))
                       {

                           float mxf1 = mx1->data[r][c];
                           float myf1 = my1->data[r][c];
                           float mxf2 = mx2->data[r][c];
                           float myf2 = my2->data[r][c];
                           if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                           {
                               {
                                   float delta = myf2 - my2o->data[r][c];
                                   mx1->data[r+1][c] += delta * fracx1->data[r+1][c];
                                   my1->data[r+1][c] += delta * fracy1->data[r+1][c];
                                   mx2->data[r+1][c] += delta * fracx2->data[r+1][c];
                                   my2->data[r+1][c] += delta * fracy2->data[r+1][c];

                                   change  = true;
                                   map->data[r+1][c] += myf2 + delta;
                                   my2o->data[r][c] = myf2 + delta;

                               }

                               my2->data[r][c] = 0.0;
                           }

                           if((c-1 > -1) && mxf1 < -1e-10)
                           {
                               {
                                   float delta = mxf1 - mx1o->data[r][c];
                                   mx1->data[r][c-1] -= delta * fracx1->data[r][c-1];
                                   my1->data[r][c-1] -= delta * fracy1->data[r][c-1];
                                   mx2->data[r][c-1] -= delta * fracx2->data[r][c-1];
                                   my2->data[r][c-1] -= delta * fracy2->data[r][c-1];
                                   change  = true;
                                   map->data[r][c-1] -= mxf1 + delta;
                                   mx1o->data[r][c] = mxf1 + delta;
                               }

                               mx1->data[r][c] = 0.0;
                           }



                       }
                   }
               }

               //then we move in left-upper direction
               for(int r = map->data.nr_rows()-1; r > -1 ;r--)
               {
                   for(int c = 0; c < map->data.nr_cols();c++)
                   {
                       float v_points = source->data[r][c];
                       if(!pcr::isMV(v_points))
                       {

                           float mxf1 = mx1->data[r][c];
                           float myf1 = my1->data[r][c];
                           float mxf2 = mx2->data[r][c];
                           float myf2 = my2->data[r][c];
                           if((r-1 > -1) && myf1 < -1e-10)
                           {
                               {
                                   float delta = myf1 - my1o->data[r][c];
                                   mx1->data[r-1][c] -= delta * fracx1->data[r-1][c];
                                   my1->data[r-1][c] -= delta * fracy1->data[r-1][c];
                                   mx2->data[r-1][c] -= delta * fracx2->data[r-1][c];
                                   my2->data[r-1][c] -= delta * fracy2->data[r-1][c];
                                   change  = true;
                                   map->data[r-1][c] -= myf1 + delta;
                                   my1o->data[r][c] = myf1 + delta;
                               }

                               my1->data[r][c] = 0.0;
                           }

                           if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                           {
                               {
                                   float delta = mxf2 - mx2o->data[r][c];
                                   mx1->data[r][c+1] += delta * fracx1->data[r][c+1];
                                   my1->data[r][c+1] += delta * fracy1->data[r][c+1];
                                   mx2->data[r][c+1] += delta * fracx2->data[r][c+1];
                                   my2->data[r][c+1] += delta * fracy2->data[r][c+1];
                                   change  = true;
                                   map->data[r][c+1] += mxf2 + delta;
                                   mx2o->data[r][c] = mxf2 + delta;

                               }

                               mx2->data[r][c] = 0.0;
                           }
                       }


                   }
               }
           }




           first = false;

       }

       delete mx1;
       delete my1;
       delete mx2;
       delete my2;
       delete fracx1;
       delete fracx2;
       delete fracy1;
       delete fracy2;
       return map;
}

inline cTMap * AS_SpreadFlowMD2(cTMap * source ,cTMap * fracx1, cTMap * fracx2, cTMap * fracy1,cTMap * fracy2, int iter_max = 0)
{
    MaskedRaster<float> raster_data(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *mx1 = new cTMap(std::move(raster_data),source->projection(),"");
    MaskedRaster<float> raster_data2(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my1 = new cTMap(std::move(raster_data2),source->projection(),"");
    MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *mx2 = new cTMap(std::move(raster_data3),source->projection(),"");
    MaskedRaster<float> raster_data4(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my2 = new cTMap(std::move(raster_data4),source->projection(),"");

    MaskedRaster<float> raster_data5(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data5),source->projection(),"");


    //initialize the map with friction values

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(source->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 0.0;
                mx1->data[r][c] = fracx1->data[r][c] * source->data[r][c];
                my1->data[r][c] = fracy1->data[r][c] * source->data[r][c];
                mx2->data[r][c] = fracx2->data[r][c] * source->data[r][c];
                my2->data[r][c] = fracy2->data[r][c] * source->data[r][c];

            }
        }
    }

    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r-1 > -1) && myf1 < -1e-10)
                        {
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }
                    }


                }
            }
        }else
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }
                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r-1 > -1) && myf1 < -1e-10)
                        {
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }


                }
            }

        }
        first = false;

    }

    delete mx1;
    delete my1;
    delete mx2;
    delete my2;

    return map;

}


inline cTMap * AS_SpreadFlowMD(cTMap * source ,cTMap * fracx1, cTMap * fracx2, cTMap * fracy1,cTMap * fracy2, int iter_max = 0)
{
    MaskedRaster<float> raster_data(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *mx1 = new cTMap(std::move(raster_data),source->projection(),"");
    MaskedRaster<float> raster_data2(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my1 = new cTMap(std::move(raster_data2),source->projection(),"");
    MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *mx2 = new cTMap(std::move(raster_data3),source->projection(),"");
    MaskedRaster<float> raster_data4(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my2 = new cTMap(std::move(raster_data4),source->projection(),"");

    MaskedRaster<float> raster_data5(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data5),source->projection(),"");


    //initialize the map with friction values

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(source->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 0.0;
                mx1->data[r][c] = fracx1->data[r][c] * source->data[r][c];
                my1->data[r][c] = fracy1->data[r][c] * source->data[r][c];
                mx2->data[r][c] = fracx2->data[r][c] * source->data[r][c];
                my2->data[r][c] = fracy2->data[r][c] * source->data[r][c];

            }
        }
    }

    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;


        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = source->data[r][c];
                if(!pcr::isMV(v_points))
                {

                    float mxf1 = mx1->data[r][c];
                    float myf1 = my1->data[r][c];
                    float mxf2 = mx2->data[r][c];
                    float myf2 = my2->data[r][c];
                    if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                    {
                        {
                            mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                            my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                            mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                            my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                            change  = true;
                            map->data[r][c] += myf2;

                        }

                        my2->data[r][c] = 0.0;
                    }

                    if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                    {
                        {
                            mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                            my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                            mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                            my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                            change  = true;
                            map->data[r][c] += mxf2;

                        }

                        mx2->data[r][c] = 0.0;
                    }

                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = source->data[r][c];
                if(!pcr::isMV(v_points))
                {

                    float mxf1 = mx1->data[r][c];
                    float myf1 = my1->data[r][c];
                    float mxf2 = mx2->data[r][c];
                    float myf2 = my2->data[r][c];
                    if((r-1 > -1) && myf1 < -1e-10)
                    {
                        {
                            mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                            my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                            mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                            my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                            change  = true;
                            map->data[r][c] -= myf1;

                        }

                        my1->data[r][c] = 0.0;
                    }

                    if((c-1 > -1) && mxf1 < -1e-10)
                    {
                        {
                            mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                            my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                            mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                            my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                            change  = true;
                            map->data[r][c] -= mxf1;

                        }

                        mx1->data[r][c] = 0.0;
                    }
                }


            }
        }

        first = false;

    }

    delete mx1;
    delete my1;
    delete mx2;
    delete my2;

    return map;

}



inline cTMap * AS_SpreadFlow(cTMap * source ,cTMap * fracx, cTMap * fracy, int iter_max = 0)
{
    MaskedRaster<float> raster_data(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *mx = new cTMap(std::move(raster_data),source->projection(),"");
    MaskedRaster<float> raster_data2(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my = new cTMap(std::move(raster_data2),source->projection(),"");

    MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data3),source->projection(),"");


    //initialize the map with friction values

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(source->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 0.0;
                mx->data[r][c] = fracx->data[r][c] * source->data[r][c];
                my->data[r][c] = fracy->data[r][c] * source->data[r][c];
            }
        }
    }

    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;


        //first we move in right-lower direction
        for(int r = 0; r < map->data.nr_rows();r++)
        {
            for(int c = 0; c < map->data.nr_cols();c++)
            {
                float v_points = source->data[r][c];
                if(!pcr::isMV(v_points))
                {

                    float mxf = mx->data[r][c];
                    float myf = my->data[r][c];
                    if((r+1 < map->data.nr_rows()) && myf > 1e-10)
                    {
                        {
                            mx->data[r+1][c] += myf * fracx->data[r+1][c];
                            my->data[r+1][c] += myf * fracy->data[r+1][c];
                            change  = true;
                            map->data[r][c] += myf;

                        }

                        my->data[r][c] = 0.0;
                    }

                    if((c+1 < map->data.nr_cols()) && mxf > 1e-10)
                    {
                        {
                            mx->data[r][c+1] += mxf * fracx->data[r][c+1];
                            my->data[r][c+1] += mxf * fracy->data[r][c+1];
                            change  = true;
                            map->data[r][c] += mxf;

                        }

                        mx->data[r][c] = 0.0;
                    }

                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = source->data[r][c];
                if(!pcr::isMV(v_points))
                {

                    float mxf = mx->data[r][c];
                    float myf = my->data[r][c];

                    if((r-1 > -1) && myf < -1e-10)
                    {
                        {
                            mx->data[r-1][c] -= myf * fracx->data[r-1][c];
                            my->data[r-1][c] -= myf * fracy->data[r-1][c];
                            change  = true;
                            map->data[r][c] -= myf;

                        }

                        my->data[r][c] = 0.0;
                    }

                    if((c-1 > -1) && mxf < -1e-10)
                    {
                        {
                            mx->data[r][c-1] -= mxf * fracx->data[r][c-1];
                            my->data[r][c-1] -= mxf * fracy->data[r][c-1];
                            change  = true;
                            map->data[r][c] -= mxf;

                        }

                        mx->data[r][c] = 0.0;
                    }
                }


            }
        }

        first = false;

    }

    delete mx;
    delete my;

    return map;

}

inline cTMap * AS_SpreadDirectionalAbsMaxMD(cTMap * points ,cTMap * friction_start,cTMap * friction_x1, cTMap * friction_x2, cTMap * friction_y1,cTMap * friction_y2, float delta = 1e-5)
{


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
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

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
                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = friction_y2->data[r-1][c];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((vn_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction_x2->data[r][c-1];
                        float vn_current = map->data[r][c-1];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }

                        float v_new = vn_current + std::fabs((vn_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = -friction_y1->data[r+1][c];
                        float vn_current = map->data[r+1][c];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float v_new = vn_current + std::fabs((vn_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = -friction_x1->data[r][c+1];
                        float vn_current = map->data[r][c+1];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float v_new = vn_current + std::fabs((vn_fric)*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }

            }
        }

        first = false;

    }

    return map;


}



inline cTMap * AS_SpreadDirectionalAbsMax(cTMap * points ,cTMap * friction_start,cTMap * friction_x, cTMap * friction_y, float delta = 1e-5)
{


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
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

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
                float vx_fric = friction_x->data[r][c];
                if(vx_fric< 0.0)
                {
                    vx_fric = delta;
                }
                float vy_fric = friction_y->data[r][c];
                if(vy_fric< 0.0)
                {
                    vy_fric = delta;
                }
                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = (friction_x->AS_IsSingleValue? friction_x->data[0][0]:friction_y->data[r-1][c]);
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + vy_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction_x->data[r][c-1];
                        float vn_current = map->data[r][c-1];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + vx_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float vx_fric = -friction_x->data[r][c];
                if(vx_fric< 0.0)
                {
                    vx_fric = delta;
                }
                float vy_fric = -friction_y->data[r][c];
                if(vy_fric< 0.0)
                {
                    vy_fric = delta;
                }

                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = -friction_y->data[r+1][c];
                        float vn_current = map->data[r+1][c];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + vy_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = -friction_x->data[r][c+1];
                        float vn_current = map->data[r][c+1];
                        if(vn_fric< 0.0)
                        {
                            vn_fric = delta;
                        }
                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + vx_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }

            }
        }

        first = false;

    }

    return map;


}


inline cTMap * AS_SpreadMT(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
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

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(points->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;


    int changed = 0;

    int iter = 0;
    #pragma omp parallel shared(change)
    {
        while(change)
        {
            iter ++;
            change = false;



            {
                //first we move in right-lower direction
                #pragma omp for collapse(1) reduction(+:changed)
                for(int r = 0; r < map->data.nr_rows();r++)
                {
                    for(int c = 0; c < map->data.nr_cols();c++)
                    {
                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r-1 > -1))
                            {
                                float vn_points = points->data[r-1][c];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                                float vn_current = map->data[r-1][c];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    changed = changed + 1;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }
                    }
                }

                #pragma omp for collapse(1) reduction(+:changed)
                for(int r = 0; r < map->data.nr_rows();r++)
                {
                    for(int c = 0; c < map->data.nr_cols();c++)
                    {

                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((c-1 > -1))
                            {
                                float vn_points = points->data[r][c-1];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                                float vn_current = map->data[r][c-1];

                                float v_new = vn_current + std::fabs((0.5f *(vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    changed = changed + 1;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }
                    }
                }

                //then we move in left-upper direction
                #pragma omp for collapse(1) reduction(+:changed)
                for(int r = map->data.nr_rows()-1; r > -1 ;r--)
                {
                    for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                    {
                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r+1 < map->data.nr_rows()))
                            {
                                float vn_points = points->data[r+1][c];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                                float vn_current = map->data[r+1][c];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    changed = changed + 1;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }

                    }
                }

                #pragma omp for collapse(1) reduction(+:changed)
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    for(int r = map->data.nr_rows()-1; r > -1 ;r--)
                    {
                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((c+1 < map->data.nr_cols()))
                            {
                                float vn_points = points->data[r][c+1];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                                float vn_current = map->data[r][c+1];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    changed = changed + 1;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }
                    }
                }

                #pragma omp barrier

                #pragma omp single
                {
                    if(changed > 0)
                    {
                        change= true;
                    }else
                    {
                        change = false;
                    }
                    changed = 0;

                }

            }


            first = false;

        }
    }


    return map;


}



inline cTMap * AS_SpreadMD(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
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
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change)
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {
            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = points->data[r][c];
                    float v_current = map->data[r][c];
                    float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r-1 > -1))
                        {
                            float vn_points = points->data[r-1][c];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                            float vn_current = map->data[r-1][c];

                            float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;


                            }
                        }

                        if((c-1 > -1))
                        {
                            float vn_points = points->data[r][c-1];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                            float vn_current = map->data[r][c-1];

                            float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;
                            }
                        }
                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = points->data[r][c];
                    float v_current = map->data[r][c];
                    float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r+1 < map->data.nr_rows()))
                        {
                            float vn_points = points->data[r+1][c];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                            float vn_current = map->data[r+1][c];

                            float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;


                            }
                        }

                        if((c+1 < map->data.nr_cols()))
                        {
                            float vn_points = points->data[r][c+1];
                            float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                            float vn_current = map->data[r][c+1];

                            float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                            if(v_new < v_current)
                            {
                                change  = true;
                                map->data[r][c] = v_new;
                                v_current = v_new;
                            }
                        }
                    }

                }

            }
        }else
            {
                //first we move in right-lower direction
                for(int r = 0; r < map->data.nr_rows();r++)
                {
                    for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                    {
                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r-1 > -1))
                            {
                                float vn_points = points->data[r-1][c];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                                float vn_current = map->data[r-1][c];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;


                                }
                            }


                            if((c+1 < map->data.nr_cols()))
                            {
                                float vn_points = points->data[r][c+1];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                                float vn_current = map->data[r][c+1];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }
                    }
                }

                //then we move in left-upper direction
                for(int r = map->data.nr_rows()-1; r > -1 ;r--)
                {
                    for(int c = 0; c < map->data.nr_cols();c++)
                    {
                        float v_points = points->data[r][c];
                        float v_current = map->data[r][c];
                        float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r+1 < map->data.nr_rows()))
                            {
                                float vn_points = points->data[r+1][c];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                                float vn_current = map->data[r+1][c];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;


                                }
                            }

                            if((c-1 > -1))
                            {
                                float vn_points = points->data[r][c-1];
                                float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                                float vn_current = map->data[r][c-1];

                                float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                                if(v_new < v_current)
                                {
                                    change  = true;
                                    map->data[r][c] = v_new;
                                    v_current = v_new;
                                }
                            }
                        }

                    }
            }


        }


        first = false;

    }

    return map;


}


inline cTMap * AS_Spread(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for spread (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
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
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;

            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

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

                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                        float vn_current = map->data[r][c-1];

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }
            }
        }

        //then we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                        float vn_current = map->data[r+1][c];

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                        float vn_current = map->data[r][c+1];

                        float v_new = vn_current + std::fabs((0.5f * (vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                        }
                    }
                }

            }
        }

        first = false;

    }

    return map;


}

inline cTMap * AS_Spread(cTMap * points,float friction_start ,cTMap * friction)
{
    return AS_Spread(points,MapFactory(friction_start),friction);

}
inline cTMap * AS_Spread(cTMap * points,cTMap * friction_start ,float friction)
{
    return AS_Spread(points,friction_start,MapFactory(friction));
}
inline cTMap * AS_Spread(cTMap * points,float friction_start ,float friction)
{
    return AS_Spread(points,MapFactory(friction_start),MapFactory(friction));

}


inline cTMap * AS_SpreadZone(cTMap * points ,cTMap * friction_start,cTMap * friction)
{

    if(points->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for SpreadZone (points) can not be non-spatial");
        throw 1;
    }

    if(!(friction_start->AS_IsSingleValue))
    {
        if(!(friction_start->data.nr_rows() ==  points->data.nr_rows() && friction_start->data.nr_cols() ==  points->data.nr_cols()))
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
                pcr::setMV(mapz->data[r][c]);
            }else if(LISEM_ASMAP_BOOLFROMFLOAT(points->data[r][c]))
            {
                float v_fricstart = friction_start->AS_IsSingleValue? friction_start->data[0][0]:friction_start->data[r][c];
                map->data[r][c] = v_fricstart;
                mapz->data[r][c] = points->data[r][c];

            }else {
                map->data[r][c] = 1e31;
                pcr::setMV(mapz->data[r][c]);
            }
        }
    }


    float dx = map->cellSize();

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

                if(!pcr::isMV(v_points))
                {
                    if((r-1 > -1))
                    {
                        float vn_points = points->data[r-1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r-1][c];
                        float vn_current = map->data[r-1][c];

                        float v_new = vn_current + std::fabs((0.5f *( vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r-1][c];

                        }
                    }

                    if((c-1 > -1))
                    {
                        float vn_points = points->data[r][c-1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c-1];
                        float vn_current = map->data[r][c-1];

                        float v_new = vn_current + std::fabs((0.5f *( vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] = mapz->data[r][c-1];

                        }
                    }
                }
            }
        }

        //first we move in left-upper direction
        for(int r = map->data.nr_rows()-1; r > -1 ;r--)
        {
            for(int c = map->data.nr_cols()-1; c > -1 ;c--)
            {
                float v_points = points->data[r][c];
                float v_current = map->data[r][c];
                float v_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c];

                if(!pcr::isMV(v_points))
                {
                    if((r+1 < map->data.nr_rows()))
                    {
                        float vn_points = points->data[r+1][c];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r+1][c];
                        float vn_current = map->data[r+1][c];

                        float v_new = vn_current + std::fabs((0.5f *( vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r+1][c];


                        }
                    }

                    if((c+1 < map->data.nr_cols()))
                    {
                        float vn_points = points->data[r][c+1];
                        float vn_fric = friction->AS_IsSingleValue? friction->data[0][0]:friction->data[r][c+1];
                        float vn_current = map->data[r][c+1];

                        float v_new = vn_current + std::fabs((0.5f *( vn_fric + v_fric))*dx);

                        if(v_new < v_current)
                        {
                            change  = true;
                            map->data[r][c] = v_new;
                            v_current = v_new;
                            mapz->data[r][c] =  mapz->data[r][c+1];

                        }
                    }
                }

            }
        }

        first = false;

    }

    delete map;
    return mapz;

}

inline cTMap * AS_SpreadZone(cTMap * points,float friction_start ,cTMap * friction)
{
    return AS_SpreadZone(points,MapFactory(friction_start),friction);
}
inline cTMap * AS_SpreadZone(cTMap * points,cTMap * friction_start ,float friction)
{
    return AS_SpreadZone(points,friction_start,MapFactory(friction));
}
inline cTMap * AS_SpreadZone(cTMap * points,float friction_start ,float friction)
{
    return AS_SpreadZone(points,MapFactory(friction_start),MapFactory(friction));
}


inline cTMap * AS_ViewShed(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *slopes = new cTMap(std::move(raster_data2),DEM->projection(),"");


    cTMap *dem2 = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    bool newcells = true;

    for(int i = 1; i < std::max(DEM->nrCols(),DEM->nrRows()); i++)
    {


        if(newcells == false)
        {
            break;
        }



        newcells = false;

        int r1 = ro -i-1;
        int r2 = ro +i+2;
        int rc1 = co - i;
        int rc2 = co +i ;

        int c1 = co -i-1;
        int c2 = co +i+2;
        int cr1 = ro -i;
        int cr2 = ro +i;


        //exceeding circle
        for(int r = r1; r < r2; r++)
        {
            int c = rc1;
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }

                }
            }

            c = rc2;
                {
                    if(INSIDE(DEM,r,c))
                    {
                        newcells = true;

                        //get elevation from cell in direction towards source

                        float n = 0;
                        float dx = std::abs(c - co);
                        float dy = std::abs(r - ro);
                        int dxn = (c - co) > 0? 1:-1;
                        int dyn = (r - ro) > 0? 1:-1;
                        int dxn2;
                        int dyn2;

                        float fac_n2 =0.0;
                        if(dx > dy)
                        {
                            dxn2 = dxn;
                            dyn2 = 0;
                            fac_n2 = 1.0 - (dy/dx);

                        }else
                        {
                            dxn2 = 0;
                            dyn2 = dyn;
                            fac_n2 = 1.0 - (dx/dy);
                        }

                        int rn = r - dyn;
                        int cn = c - dxn;
                        int rn2 = r - dyn2;
                        int cn2 = c - dxn2;



                        if(INSIDE(DEM,rn,cn))
                        {

                            if(pcr::isMV(dem2->data[r][c]))
                            {
                                dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                                slopes->data[r][c] = slopes->data[rn][cn];
                            }else {

                                float slope_next = 0.0;
                                float w = 0.0;

                                if(INSIDE(DEM,rn,cn))
                                {
                                    slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                    w += (1.0-fac_n2);

                                    n++;
                                }
                                if(INSIDE(DEM,rn2,cn2))
                                {
                                     slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                     w+= fac_n2;
                                    n++;

                                }

                                if(w > 0.0)
                                {
                                    slope_next= slope_next/w;
                                }

                                float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                                float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                                float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                                if(i == 1)
                                {
                                    slope_next = -1e31;
                                }
                                if(slope_this > slope_next-1e-6)
                                {
                                    map->data[r][c] = 1.0;
                                }
                                slopes->data[r][c] = std::max(slope_this,slope_next);
                            }
                        }
                    }
                }
        }

        int r = cr1;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));

                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

        r = cr2;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

    }

    delete dem2;
    delete slopes;
    return map;

}

inline cTMap * AS_ViewAngle(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    cTMap * map = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    FOR_ROW_COL_MV(map)
    {
        float dxs = (c - co) * std::fabs(DEM->cellSizeX());
        float dys = (r - ro) *std::fabs(DEM->cellSizeY());

        map->data[r][c] = (DEM->data[r][c]-dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
    }

    return map;

}


inline cTMap * AS_ViewCriticalAngle(cTMap * DEM, float x, float y, float height)
{

    //get originating pixel
    int ro = (y - DEM->north())/DEM->cellSizeY();
    int co = (x- DEM->west())/DEM->cellSizeX();

    if(!INSIDE(DEM,ro,co))
    {

        LISEMS_ERROR("Can not do viewshed operation with source outside map area");
        throw 1;
    }

    if(pcr::isMV(DEM->data[ro][co]))
    {

        LISEMS_ERROR("Can not do viewshed operation with source as MV location");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *slopes = new cTMap(std::move(raster_data2),DEM->projection(),"");


    cTMap *dem2 = DEM->GetCopy();

    map->data[ro][co] = 1.0;
    float dems = DEM->data[ro][co] + height;

    bool newcells = true;

    for(int i = 1; i < std::max(DEM->nrCols(),DEM->nrRows()); i++)
    {


        if(newcells == false)
        {
            break;
        }



        newcells = false;

        int r1 = ro -i-1;
        int r2 = ro +i+2;
        int rc1 = co - i;
        int rc2 = co +i ;

        int c1 = co -i-1;
        int c2 = co +i+2;
        int cr1 = ro -i;
        int cr2 = ro +i;


        //exceeding circle
        for(int r = r1; r < r2; r++)
        {
            int c = rc1;
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }

                }
            }

            c = rc2;
                {
                    if(INSIDE(DEM,r,c))
                    {
                        newcells = true;

                        //get elevation from cell in direction towards source

                        float n = 0;
                        float dx = std::abs(c - co);
                        float dy = std::abs(r - ro);
                        int dxn = (c - co) > 0? 1:-1;
                        int dyn = (r - ro) > 0? 1:-1;
                        int dxn2;
                        int dyn2;

                        float fac_n2 =0.0;
                        if(dx > dy)
                        {
                            dxn2 = dxn;
                            dyn2 = 0;
                            fac_n2 = 1.0 - (dy/dx);

                        }else
                        {
                            dxn2 = 0;
                            dyn2 = dyn;
                            fac_n2 = 1.0 - (dx/dy);
                        }

                        int rn = r - dyn;
                        int cn = c - dxn;
                        int rn2 = r - dyn2;
                        int cn2 = c - dxn2;



                        if(INSIDE(DEM,rn,cn))
                        {

                            if(pcr::isMV(dem2->data[r][c]))
                            {
                                dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                                slopes->data[r][c] = slopes->data[rn][cn];
                            }else {

                                float slope_next = 0.0;
                                float w = 0.0;

                                if(INSIDE(DEM,rn,cn))
                                {
                                    slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                    w += (1.0-fac_n2);

                                    n++;
                                }
                                if(INSIDE(DEM,rn2,cn2))
                                {
                                     slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                     w+= fac_n2;
                                    n++;

                                }

                                if(w > 0.0)
                                {
                                    slope_next= slope_next/w;
                                }

                                float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                                float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                                float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                                if(i == 1)
                                {
                                    slope_next = -1e31;
                                }
                                if(slope_this > slope_next-1e-6)
                                {
                                    map->data[r][c] = 1.0;
                                }
                                slopes->data[r][c] = std::max(slope_this,slope_next);
                            }
                        }
                    }
                }
        }

        int r = cr1;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

        r = cr2;
        {
            for(int c = c1; c < c2; c++)
            {
                if(INSIDE(DEM,r,c))
                {
                    newcells = true;

                    //get elevation from cell in direction towards source

                    float n = 0;
                    float dx = std::abs(c - co);
                    float dy = std::abs(r - ro);
                    int dxn = (c - co) > 0? 1:-1;
                    int dyn = (r - ro) > 0? 1:-1;
                    int dxn2;
                    int dyn2;

                    float fac_n2 =0.0;
                    if(dx > dy)
                    {
                        dxn2 = dxn;
                        dyn2 = 0;
                        fac_n2 = 1.0 - (dy/dx);

                    }else
                    {
                        dxn2 = 0;
                        dyn2 = dyn;
                        fac_n2 = 1.0 - (dx/dy);
                    }

                    int rn = r - dyn;
                    int cn = c - dxn;
                    int rn2 = r - dyn2;
                    int cn2 = c - dxn2;



                    if(INSIDE(DEM,rn,cn))
                    {

                        if(pcr::isMV(dem2->data[r][c]))
                        {
                            dem2->data[r][c] = GetMapValue_OUTORMV3x3Av(dem2,r,c);
                            slopes->data[r][c] = slopes->data[rn][cn];
                        }else {

                            float slope_next = 0.0;
                            float w = 0.0;

                            if(INSIDE(DEM,rn,cn))
                            {
                                slope_next += (1.0-fac_n2) * slopes->data[rn][cn];
                                w += (1.0-fac_n2);

                                n++;
                            }
                            if(INSIDE(DEM,rn2,cn2))
                            {
                                 slope_next += (fac_n2) * slopes->data[rn2][cn2];
                                 w+= fac_n2;
                                n++;

                            }

                            if(w > 0.0)
                            {
                                slope_next= slope_next/w;
                            }

                            float dxs = (c - co) * std::fabs(DEM->cellSizeX());
                            float dys = (r - ro) *std::fabs(DEM->cellSizeY());

                            float slope_this = (dem2->data[r][c] - dems)/std::sqrt(std::max(0.0000001f,dxs *dxs + dys * dys));
                            if(i == 1)
                            {
                                slope_next = -1e31;
                            }
                            if(slope_this > slope_next-1e-6)
                            {
                                map->data[r][c] = 1.0;
                            }
                            slopes->data[r][c] = std::max(slope_this,slope_next);
                        }
                    }
                }

            }
        }

    }



    delete dem2;
    delete map;
    return slopes;

}


#endif // RASTERSPREAD_H
