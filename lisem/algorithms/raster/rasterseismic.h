#ifndef RASTERSEISMIC_H
#define RASTERSEISMIC_H

#include "geo/raster/map.h"
#include "rasterderivative.h"
#include "raster/rastercommon.h"


//2d wave equation solution
inline static std::vector<cTMap *> AS_PressureWaveEquation( cTMap * DX, cTMap * DY, cTMap * UX, cTMap * UY, cTMap * vel, float dt)
{







    return {nullptr};


}

//2d wave equation solution
inline static cTMap * AS_WaveEquation( cTMap * inU, cTMap * cc, float dt)
{
    cTMap * U = inU->GetCopy();
    cTMap * UN = inU->GetCopy();
    cTMap * UP = inU->GetCopy0();

    double maxc = MapMaximum(cc);

    if(! (maxc > 0.0))
    {
        LISEMS_ERROR("Wave velocity must be larger then 0");
        throw 1;
    }
    float dt_req = 0.5 * std::min(std::fabs(U->cellSizeX()),std::fabs(U->cellSizeY()))/maxc;
    int iter = std::max(1, ((int)( dt/dt_req))+1);
    float dt_s = dt/((float)(iter));

    int r, c, nrRows, nrCols;

    nrRows = U->nrRows();
    nrCols = U->nrCols();

    for (r = 0; r < nrRows; r++) {
        for (c = 0; c < nrCols; c++) {

            if(!pcr::isMV(U->data[r][c]))
            {
                double ddx2 = UF2D_Derivative2(U,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(U->cellSizeX()));
                double ddy2 = UF2D_Derivative2(U,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(U->cellSizeY()));


                if(!pcr::isMV(cc->data[r][c]))
                {
                    U->data[r][c] = UP->data[r][c] + (ddx2 + ddy2)*cc->data[r][c]*cc->data[r][c];
                }
            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        SWAPMAP(UP,U);
        SWAPMAP(UN,U);

        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {

                if(!pcr::isMV(U->data[r][c]))
                {
                    double ddx2 = UF2D_Derivative2(U,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(U->cellSizeX()));
                    double ddy2 = UF2D_Derivative2(U,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(U->cellSizeY()));

                    double du = 0.0;
                    if(!pcr::isMV(cc->data[r][c]))
                    {
                        du = dt_s *(ddx2 + ddy2)*cc->data[r][c]*cc->data[r][c];

                    }
                    UN->data[r][c] = (2.0 * U->data[r][c]  - UP->data[r][c]) +( du);
                }
            }
        }

    }


    delete U;
    delete UP;
    return UN;
}



   //#include "seismic/EW.h"


#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>


//Seismic wave simulation using SW4

// for( g = 0 ; g < mNumberOfGrids; g++ )
// {   for(k=m_kStart[g]; k<= m_kEnd[g]; k++ )
//     {   for(j=m_jStart[g]; j<= m_jEnd[g]; j++ )
//         {   for(i=m_iStart[g]; i<= m_iEnd[g]; i++ )
//             {
//                  mu_tmp = mMu[g](i,j,k);

//the grids are the refinement grids, with higer detail at the top.
//each refinement decreases resolution by a factor 2
//the grids are cartesian normally, but in case of topography, there is a curvlinear grid at the top.
//curvilinear grid maintains lateral extens, but changes vertical grid cell size.
//our cartesian data cubes represent the sub-surface
inline static std::vector<cTMap *> AS_SeismicWave( std::vector<cTMap *> state, std::vector<cTMap *> Dens, std::vector<cTMap *> velP, std::vector<cTMap *> velS, float dt)
{


     int myRank = 0, nProcs = 0;

     // Initialize MPI...
     int initialized = false;
     MPI_Initialized(&initialized);
     if(!initialized)
     {
         MPI_Init(nullptr, nullptr);
     }


     MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     MPI_Comm_size(MPI_COMM_WORLD, &nProcs);

    // Save the source description here
    //vector<Source*> GlobalSources;
    // Save the time series here
    //vector<STimeSeries*> GlobalTimeSeries;

    /*EW simulation(fileName, GlobalSources, GlobalTimeSeries);

    simulation.setupRun( GlobalSources );
    simulation.solve( GlobalSources, GlobalTimeSeries );*/


    MPI_Abort(MPI_COMM_WORLD, 1);

    return {nullptr};
}


#endif // RASTERSEISMIC_H
