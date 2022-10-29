#ifndef RASTERSUPERFASTFLOW_H
#define RASTERSUPERFASTFLOW_H


#include "rasterspread.h"
#include "rasterderivative.h"
#include "rasterreduce.h"
#include "rasterartificialflow.h"

inline cTMap *AS_DemMonotonicReconstruct(cTMap * DEM)
{

    float dx = DEM->cellSizeX();
    int iters= 25 * std::max(1,std::max(DEM->nrCols(),DEM->nrRows())/1000);
    cTMap * GradX1 = AS_SlopeX1(DEM);
    cTMap * GradY1 = AS_SlopeY1(DEM);
    cTMap * GradX2 = AS_SlopeX2(DEM);
    cTMap * GradY2 = AS_SlopeY2(DEM);

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *Seeds = new cTMap(std::move(raster_data3),DEM->projection(),"");

    int r = 0,c = 0;
    int seeds = 0;
    float mindem = AS_MapMinimumRed(DEM);

    #pragma omp parallel for collapse(2)
    for (r = 0; r < DEM->Rows(); r++) {
        for (c = 0; c < DEM->Cols(); c++) {
            if (!pcr::isMV(DEM->data[r][c])) {

                //if dem < smallest dem, or if it is an edge cell, make it a seed for the new dem
                if(OUTORMV(DEM,r,c) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r+1,c) || OUTORMV(DEM,r-1,c) || (DEM->data[r][c] < mindem + 1e-6))
                {
                    seeds ++;
                    Seeds->data[r][c] =1.0;
                }else
                {
                    Seeds->data[r][c] =0.0;
                }
            }else
            {
                Seeds->data[r][c] =0.0;
            }
        }
    }

    //then do the elevation model fix
    cTMap * DEM2;


    DEM2 = AS_SpreadDirectionalAbsMaxMD(Seeds,DEM,GradX1,GradX2,GradY1,GradY2);

    delete GradX1;
    delete GradX2;
    delete GradY1;
    delete GradY2;
    delete Seeds;

    return DEM2;


}


//super-fast flood algorithm
//besides some multi-threading upportunities, SSE/AVX instructions, etc
//from a data-dependency perspective I dont see how any physically-based method could be significantly faster than this
//the number of iterations in space are reduced by carefully selected method so that there is only minimal number of iterations and complexity
inline cTMap * AS_FlowSuperFast(cTMap * DEM,  cTMap * N,cTMap * Rain, float duration, bool do_depressions, bool corr, int itersp = -1)
{
    if(itersp < 0)
    {
        itersp = 40;
    }
    //first we get the right gradient maps

    float dx = DEM->cellSizeX();
    int iters= 25 * std::max(1,std::max(DEM->nrCols(),DEM->nrRows())/1000);
    cTMap * GradX1 = AS_SlopeX1(DEM);
    cTMap * GradY1 = AS_SlopeY1(DEM);
    cTMap * GradX2 = AS_SlopeX2(DEM);
    cTMap * GradY2 = AS_SlopeY2(DEM);
    cTMap * one = DEM->GetCopy1();


    //get a map with seed points for corrected dem
    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *Seeds = new cTMap(std::move(raster_data3),DEM->projection(),"");

    int r = 0,c = 0;
    int seeds = 0;
    float mindem = AS_MapMinimumRed(DEM);

    #pragma omp parallel for collapse(2)
    for (r = 0; r < DEM->Rows(); r++) {
        for (c = 0; c < DEM->Cols(); c++) {
            if (!pcr::isMV(DEM->data[r][c])) {

                Rain->data[r][c] = (dx*dx*Rain->data[r][c])/3600000.0;
                //if dem < smallest dem, or if it is an edge cell, make it a seed for the new dem
                if(OUTORMV(DEM,r,c) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r+1,c) || OUTORMV(DEM,r-1,c) || (DEM->data[r][c] < mindem + 1e-6))
                {
                    seeds ++;
                    Seeds->data[r][c] =1.0;
                }else
                {
                    Seeds->data[r][c] =0.0;
                }
            }else
            {
                Seeds->data[r][c] =0.0;
            }
        }
    }

    //then do the elevation model fix
    cTMap * DEM2;

    cTMap * DEMD;
    cTMap * DEMDM;
    cTMap * DEMDID;
    //get depressions

    std::vector<int> depressionids;
    std::vector<float> depressionvols;
    std::vector<float> depressioncvols;

    std::vector<std::thread> workers1;
    workers1.push_back(std::thread([&DEM2,DEM,Seeds,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
    {
        DEM2 = AS_SpreadMonotonicReconstruct(DEM,0.00001,iters);//AS_SpreadDirectionalAbsMaxMD(Seeds,DEM,GradX1,GradX2,GradY1,GradY2);


    }));
    if(do_depressions)
    {
        workers1.push_back(std::thread([dx,&depressioncvols,&depressionvols,&depressionids,&DEMDID,DEM,&DEMD,&DEMDM,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            //DEMD will contain the depression depth
            MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
            DEMDM = new cTMap(std::move(raster_data5),DEM->projection(),"");

            DEMD = AS_SpreadDepressionFind(DEM);
            int depressioncells = 0;
             for (int r = 0; r < DEM->Rows(); r++) {
                 for (int c = 0; c < DEM->Cols(); c++) {
                     if (!pcr::isMV(DEM->data[r][c])) {
                         float valn = DEMD->data[r][c]-DEM->data[r][c];

                         if(valn > 0.0001)
                         {
                             DEMDM->data[r][c] = DEMD->data[r][c];
                             depressioncells++;

                         }else
                         {
                             pcr::setMV(DEMDM->data[r][c]);
                         }
                         DEMD->data[r][c] = valn;
                     }else
                     {
                         pcr::setMV(DEMDM->data[r][c]);
                     }
                 }
             }

             DEMDID = AS_Clump(DEMDM,0.1,true);

             //get depression volumes
             float cellarea = dx * dx;

             depressioncells = 0;
             for(int r = 0; r < DEMDID->data.nr_rows();r++)
             {
                 for(int c = 0; c < DEMDID->data.nr_cols();c++)
                 {
                     if(!pcr::isMV(DEMDID->data[r][c]))
                     {
                         depressioncells ++;
                         bool found = false;
                         int class_current = (int)DEMDID->data[r][c];
                         //std::cout << "add class " << class_current << " " << depressionids.size() <<  std::endl;

                         if(class_current > (((int)depressionids.size())-5))
                         {
                             //std::cout << "add class " << class_current << " " << depressionids.size() <<  std::endl;
                             int oldsize = depressionids.size();
                             for(int i = 0; i < (5 + class_current - depressionids.size()); i++)
                             {
                                 //std::cout << "add" << std::endl;
                                 depressionids.push_back(oldsize + i);
                                 depressionvols.push_back(0.0);
                             }

                         }
                         {
                             depressionvols.at(class_current) += cellarea * DEMD->data[r][c];
                         }
                     }

                 }
             }
             for(int i =0; i < depressionvols.size(); i++)
             {
                 depressioncvols.push_back(0.0);
             }


        }));
    }

   // Looping every thread via for_each
   // Then, joins one by one, and this works like barrier
   std::for_each(workers1.begin(), workers1.end(), [](std::thread &t)
   {
       t.join();
   });


    //get new normalized flux network
    #pragma omp parallel for collapse(2)
    for (r = 0; r < DEM->Rows(); r++) {
        for (c = 0; c < DEM->Cols(); c++) {
            if (!pcr::isMV(DEM->data[r][c])) {

                float gx1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L));
                float gx2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R));
                float gy1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L));
                float gy2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R));

                float gtot = std::max(1e-12f,std::fabs(gx1) + std::fabs(gx2) + std::fabs(gy1) + std::fabs(gy2));

                GradX1->data[r][c] = gx1/gtot;
                GradX2->data[r][c] = gx2/gtot;
                GradY1->data[r][c] = gy1/gtot;
                GradY2->data[r][c] = gy2/gtot;

            } else
            {
                pcr::setMV(GradX1->data[r][c]);
                pcr::setMV(GradX2->data[r][c]);
                pcr::setMV(GradY1->data[r][c]);
                pcr::setMV(GradY2->data[r][c]);
            }
        }
    }

    //do flow accumulation
    cTMap * q;

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *RainN = new cTMap(std::move(raster_data4),DEM->projection(),"");
    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *Vel = new cTMap(std::move(raster_data5),DEM->projection(),"");

    cTMap * accN;
    cTMap * accqreal;
    cTMap * acc1;
    cTMap * accacc1;
    cTMap * accvel;

    // vector container stores threads
    std::vector<std::thread> workers;

    if(!do_depressions)
    {
        workers.push_back(std::thread([&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);


        }));

        workers.push_back(std::thread([&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            for (int r = 0; r < DEM->Rows(); r++) {
                for (int c = 0; c < DEM->Cols(); c++) {
                    if (!pcr::isMV(DEM->data[r][c])) {

                        RainN->data[r][c] = Rain->data[r][c] * N->data[r][c];
                    }
                }
            }

            accN = AS_SpreadFlowMD2(RainN,GradX1,GradX2,GradY1,GradY2,iters);


        }));
        workers.push_back(std::thread([one,DEM,&acc1,&accacc1,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            acc1 = AS_SpreadFlowMD2(one,GradX1,GradX2,GradY1,GradY2,iters);
            accacc1 = AS_SpreadFlowMD2(acc1,GradX1,GradX2,GradY1,GradY2,iters);

        }));
        workers.push_back(std::thread([&Vel,DEM,&accvel,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            for (int r = 0; r < DEM->Rows(); r++) {
                for (int c = 0; c < DEM->Cols(); c++) {
                    if (!pcr::isMV(DEM->data[r][c])) {

                        //9.68*(accuflux.tif/1e6)**0.32
                        //Q = H * v, v =
                        Vel->data[r][c] = Rain->data[r][c] * 1.0;
                    }
                }
            }
            accvel = AS_SpreadFlowMD2(Vel,GradX1,GradX2,GradY1,GradY2,iters);

        }));
    }else
    {
        workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);


        }));
        workers.push_back(std::thread([duration,&accqreal,&DEMDID,&depressioncvols,&depressionvols,&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            accqreal = AS_SpreadFlowMD2Depression(Rain,GradX1,GradX2,GradY1,GradY2,iters,DEMDID,depressionvols,depressioncvols,true,duration * 60.0 * 60.0);


        }));

        workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            for (int r = 0; r < DEM->Rows(); r++) {
                for (int c = 0; c < DEM->Cols(); c++) {
                    if (!pcr::isMV(DEM->data[r][c])) {

                        RainN->data[r][c] = Rain->data[r][c] * N->data[r][c];
                    }
                }
            }

            accN = AS_SpreadFlowMD2(RainN,GradX1,GradX2,GradY1,GradY2,iters);


        }));
        workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,one,DEM,&acc1,&accacc1,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            acc1 = AS_SpreadFlowMD2(one,GradX1,GradX2,GradY1,GradY2,iters);
            accacc1 = AS_SpreadFlowMD2(acc1,GradX1,GradX2,GradY1,GradY2,iters);

        }));
        workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&Vel,DEM,&accvel,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
        {
            for (int r = 0; r < DEM->Rows(); r++) {
                for (int c = 0; c < DEM->Cols(); c++) {
                    if (!pcr::isMV(DEM->data[r][c])) {

                        //9.68*(accuflux.tif/1e6)**0.32
                        //Q = H * v, v =
                        Vel->data[r][c] = Rain->data[r][c] * 1.0;
                    }
                }
            }
            accvel = AS_SpreadFlowMD2(Vel,GradX1,GradX2,GradY1,GradY2,iters);

        }));
    }


    // Looping every thread via for_each
    // Then, joins one by one, and this works like barrier
    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });


    //get average distance, N and velocity
    #pragma omp parallel for collapse(2)
    for (r = 0; r < DEM->Rows(); r++) {
        for (c = 0; c < DEM->Cols(); c++) {
            if (!pcr::isMV(DEM->data[r][c])) {

                RainN->data[r][c] = accN->data[r][c]/std::max(0.00001f,q->data[r][c]);
                Vel->data[r][c] = accvel->data[r][c]/std::max(0.00001f,q->data[r][c]);
                accacc1->data[r][c] = dx * accacc1->data[r][c]/std::max(1.0f,acc1->data[r][c]);

                //get steady-state compensation
                //Map h =  pow( q* N /max(0.001,sqrt(max(0.001,Slope(dem)))),q * 0.0 + (5.0/6.0));


                /*dx = 10;
                L = 6300;
                n = 10000000;
                b = 1.54;
                Lav = 10;
                L = ((2 + b) Lav)/(1 + b) // N
                b = (-Log[dx/L] + Log[1/n])/Log[dx/L] // N*/

                float i_dx = dx;
                float i_L = accacc1->data[r][c] * 3.0/2.0;//initial guess for iteration based on average distance of circular segment to center of circle
                float i_n = acc1->data[r][c];
                float i_b = 1.0; //start iteration with linear
                float i_Lav = accacc1->data[r][c];//average travel distance to outlet

                bool do_break = false;
                //this should typically only take a small amount of iterations (<5)
                for(int i = 0; i < 10; i++)
                {

                    float Lnew = ((2.0 + i_b) *i_Lav)/(1.0 + i_b); // N
                    float bnew = (-std::log(i_dx/i_L) + std::log(1.0/i_n))/std::log(i_dx/i_L); // N*/

                    if(((Lnew - i_L) < 0.001) && ((bnew - i_b) < 0.001))
                    {
                        do_break = true;
                    }
                    i_L = Lnew;
                    i_b = bnew;
                    if(do_break)
                    {
                        break;
                    }
                }
                if(!std::isnormal(i_b) || !std::isnormal(i_L))
                {
                    i_b = 1.0;
                    i_L = accacc1->data[r][c] * 3.0/2.0;
                }

                i_b = std::max(0.00001f,i_b);
                i_L = std::max(dx,i_L);

                float ss_comp_x = duration * 60 * 60 * Vel->data[r][c]/i_L; //relative event duration
                float ss_comp;
                if(ss_comp_x < 1.0)
                {
                    ss_comp = 1.0 - std::pow(1.0 - ss_comp_x,1.0 + i_b); //1 - (1 - x)^(1 + b)
                }else
                {
                    ss_comp = 1.0;
                }

                float SlopeX = UF2D_Derivative(DEM,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                float SlopeY = UF2D_Derivative(DEM,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);
                float Slope = std::fabs(SlopeX) + std::fabs(SlopeY);

                //fill with h
                if(!do_depressions)
                {
                    one->data[r][c] = std::pow((1.0/(dx)) * ss_comp * q->data[r][c] * RainN->data[r][c]/std::max(0.001f,Slope),3.0/5.0);

                }else
                {
                    one->data[r][c] = std::pow((1.0/(dx)) *ss_comp * accqreal->data[r][c] * RainN->data[r][c]/std::max(0.001f,Slope),3.0/5.0);

                }

                if(!std::isnormal(one->data[r][c]))
                {
                    one->data[r][c] = 0.0;
                }
                if(do_depressions)
                {

                    //is this cell in a depression?
                    if(!pcr::isMV(DEMDID->data[r][c]))
                    {
                        int id = DEMDID->data[r][c];
                        //to what extent is this depression filled?
                        float frac = depressioncvols.at(id)/ std::max(0.01f,depressionvols.at(id));
                        one->data[r][c] += DEMD->data[r][c] * frac;
                    }
                }

            }
        }
    }



    std::cout << "iters " << itersp << std::endl;


    //water redistribution for pressure advection
    cTMap * hfinal;
    if(corr)
    {
        if(!do_depressions)
        {
            //if we dont do depressions, we better use the corrected dem as it improves throughflow in low resolution rough terrain
           hfinal = AS_DiffusiveQMaxWave(DEM2,one /*is filled with H*/,itersp,0.15);
        }else
        {
            hfinal = AS_DiffusiveQMaxWave(DEM,one /*is filled with H*/,itersp,0.15);
        }
    }else
    {
        if(!do_depressions)
        {
            //if we dont do depressions, we better use the corrected dem as it improves throughflow in low resolution rough terrain
           hfinal = AS_DiffusiveMaxWave(DEM2,one /*is filled with H*/,itersp,0.15);
        }else
        {
            hfinal = AS_DiffusiveMaxWave(DEM,one /*is filled with H*/,itersp,0.15);
        }
    }
    //return flow height


    delete RainN;
    delete Vel;
    delete accN;
    delete acc1;
    delete accacc1;
    delete accvel;
    delete DEM2;
    delete Seeds;
    delete q;
    delete one;
    delete GradX1;
    delete GradY1;
    delete GradX2;
    delete GradY2;

    if(do_depressions)
    {
        delete accqreal;
        delete DEMD;
        delete DEMDM;
        delete DEMDID;
    }

    return hfinal;

}




#endif // RASTERSUPERFASTFLOW_H
