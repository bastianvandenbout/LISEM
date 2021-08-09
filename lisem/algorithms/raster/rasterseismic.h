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


/*
#include "seismic/EW.h"

#include "geo/raster/field.h"
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>

class LISEM_API SeismicModel
{
public:


    int myRank = 0, nProcs = 0;
    bool first_step = false;


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

    EW *m_Simulation;


    // Save the source description here
    vector<Source*> GlobalSources;
    // Save the time series here
    vector<STimeSeries*> GlobalTimeSeries;


    inline SeismicModel()
    {


    }

    inline ~SeismicModel()
    {

        MPI_Abort(MPI_COMM_WORLD, 1);

    }



    inline void AddVelocityModel(Field * vp, Field * vs, Field * dens)
    {


    }

    inline void SetRefinementDepths(std::vector<float> depths)
    {

    }

    inline void AddSourcePoint()
    {

    }
    inline void AddSourceTensor()
    {

    }
    inline void GetFullFields()
    {

    }

    inline void FillField(bool nearest, )
    {

    }

    inline void Step()
    {

        if(first_step)
        {
            first_step = false;

            // Initialize MPI...
            int initialized = false;
            MPI_Initialized(&initialized);
            if(!initialized)
            {
                MPI_Init(nullptr, nullptr);
            }


            MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
            MPI_Comm_size(MPI_COMM_WORLD, &nProcs);


            std::stringstream ss;

            ss << "Test file " << std::endl;
            ss << "line 2" << std::endl;

            m_Simulation = new EW(ss, GlobalSources, GlobalTimeSeries);


            m_Simulation->setupRun( GlobalSources);

            //do part of the solve function that is preparation



        }


        //we require a custom solve that only does the single timestep we want
        m_Simulation->solve( GlobalSources, GlobalTimeSeries );




    }


    inline void CopyFrom(SeismicModel* )
    {




    }

    int AS_RefCount = 1;

    inline void AS_ReleaseRef()
    {
        AS_RefCount -= 1;
        if(AS_RefCount == 0)
        {
            delete this;
        }
    }
    inline void AS_AddRef()
    {
        AS_RefCount += 1;
    }


    inline SeismicModel * AS_Assign(SeismicModel * other)
    {
        this->CopyFrom(other);

        return this;
    }
};

inline static SeismicModel *AS_SeismicModelFactory()
{
    return new SeismicModel();
}*/


#endif // RASTERSEISMIC_H
