#ifndef RASTERFIBERBUNDLE_H
#define RASTERFIBERBUNDLE_H


#include "geo/raster/map.h"
#include <vector>

std::vector<cTMap*> AS_LoadRedistribute(cTMap * LoadMax, cTMap * LoadMaxX, cTMap * LoadMaxY, cTMap * Frac_Broken_Base,cTMap * Frac_Broken_LatX, cTMap * Frac_Broken_LatY, cTMap * LoadX, cTMap * LoadY, cTMap * CapacityBase, cTMap * CapacityX, cTMap * CapacityY, cTMap * K_comp, cTMap * K_tensile, cTMap * alpha, int iterations, float m_base, float m_lat, float stddev_base, float stddev_lat)
{

    cTMap * LoadTotal = LoadX->GetCopy0();
    cTMap * LoadTransferX1 = LoadX->GetCopy0();
    cTMap * LoadTransferX2 = LoadX->GetCopy0();
    cTMap * LoadTransferY1 = LoadX->GetCopy0();
    cTMap * LoadTransferY2 = LoadX->GetCopy0();
    cTMap * LoadExcessX = LoadX->GetCopy0();
    cTMap * LoadExcessY = LoadY->GetCopy0();



    for(int i = 0; i < iterations; i++)
    {

        for(int r = 0; r < LoadTotal->nrRows(); r++)
        {
            for(int c = 0; c < LoadTotal->nrCols(); c++)
            {
                if(!pcr::isMV(LoadX->data[r][c]))
                {

                    //get total load

                    LoadTotal->data[r][c] = std::sqrt(LoadX->data[r][c] *LoadX->data[r][c] + LoadY->data[r][c] *LoadY->data[r][c]);

                    //check if this exceeds maximum


                    //now esimate fraction of broken fibers with this load


                    //redistribute forces to other fibers (globally)



                    //check for failure


                    //get load excess

                    float load_excess = std::max(0.0,LoadTotal->data[r][c] - CapacityBase->data[r][c]);

                    //get excess load x and y components
                    float ratio_x = LoadX->data[r][c]/std::max(1e-6,LoadTotal->data[r][c]);
                    float ratio_y = LoadY->data[r][c]/std::max(1e-6,LoadTotal->data[r][c]);

                    LoadExcessX->data[r][c] = ratio_x * load_excess;
                    LoadExcessY->data[r][c] = ratio_y * load_excess;

                }
            }
        }



        //get transfer load
        for(int r = 0; r < LoadTotal->nrRows(); r++)
        {
            for(int c = 0; c < LoadTotal->nrCols(); c++)
            {
                if(!pcr::isMV(LoadX->data[r][c]))
                {

                    //get k constants


                    //transfer loads


                    //


                }
            }
        }


        //transfer load




    }

    //return:
    //LoadX Base (pa)
    //LoadY Base (pa)
    //LoadX Lat (pa)
    //LoadY lat (pa)
    //Load Base Max (pa)
    //LoadX Lat Max (pa)
    //LoadY Lat Max (pa)
    //Base Damage (1 = failure)
    //LatX Damange (1 = failure)
    //LatY Damage (1 = failure)
    //

    return {};
}


std::vector<cTMap*> AS_FiberWeibullFail(cTMap * LoadX, cTMap * LoadY, cTMap * CapacityX, cTMap * CapacityY)
{


    return {};
}


#endif // RASTERFIBERBUNDLE_H
