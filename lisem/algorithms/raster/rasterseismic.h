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



#include "seismic/EW.h"

#include "geo/raster/field.h"
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>

#define LISEM_SEISMIC_PARAMETER_DENS "DENS"
#define LISEM_SEISMIC_PARAMETER_VP "VP"
#define LISEM_SEISMIC_PARAMETER_VS "VS"
#define LISEM_SEISMIC_PARAMETER_ACC "ACC"
#define LISEM_SEISMIC_PARAMETER_DISP "DISP"
#define LISEM_SEISMIC_PARAMETER_ACCX "ACCX"
#define LISEM_SEISMIC_PARAMETER_ACCY "ACCY"
#define LISEM_SEISMIC_PARAMETER_ACCZ "ACCZ"
#define LISEM_SEISMIC_PARAMETER_DISPX "DISPX"
#define LISEM_SEISMIC_PARAMETER_DISPY "DISPY"
#define LISEM_SEISMIC_PARAMETER_DISPZ "DISPZ"

typedef struct source_point
{


    float x,y,z,Mxx,Myy,Mzz,Mxz, Mxy,Myz,t,freq;

    QString type;
} SourcePoint;

class LISEM_API SeismicModel
{
public:


    int myRank = 0, nProcs = 0;
    bool first_step = true;


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
    std::vector<Source*> GlobalSources;
    // Save the time series here
    std::vector<STimeSeries*> GlobalTimeSeries;

    double m_T = 0.0;

    double m_Time = 0.0;
    cTMap * m_Topography;
    std::vector<Field *> m_VPInputs;
    std::vector<Field *> m_VSInputs;
    std::vector<Field *> m_DENSInputs;
    std::vector<float> m_RefineDepths;
    std::vector<SourcePoint> m_Sources;
    inline SeismicModel()
    {


    }

    inline ~SeismicModel()
    {
        MPI_Abort(MPI_COMM_WORLD, 1);
    }


    inline void SetTopography(cTMap * dem)
    {
        m_Topography = dem->GetCopy();
    }

    inline void AddVelocityModel(Field * vp, Field * vs, Field * dens)
    {
        m_VPInputs.push_back(vp->GetCopy());
        m_VSInputs.push_back(vs->GetCopy());
        m_DENSInputs.push_back(dens->GetCopy());

    }



    inline void SetRefinementDepths(std::vector<float> depths)
    {
        m_RefineDepths = depths;
    }

    inline void SetSuperGrid()
    {


    }

    inline void AddSourcePoint(float x, float y, float z, float Mxx, float Myy, float Mzz, float Mxy, float Mxz, float Myz, float t, float freq, QString type)
    {
        SourcePoint p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.Mxx =Mxx;
        p.Myy =Myy;
        p.Mzz =Mzz;
        p.Mxy =Mxy;
        p.Mxz =Mxz;
        p.Myz =Myz;
        p.t = t;
        p.freq = freq;
        p.type = type;
        m_Sources.push_back(p);

    }
    inline void AddSourceTensor()
    {

    }
    inline std::vector<Field *> GetFullFields(QString parameter)
    {


        return {nullptr};
    }

    inline Field * FillField(Field * f,  QString parameter,bool nearest)
    {




        //by default we do bilinear interpolation
        for( int g = 0 ; g < m_Simulation->mNumberOfCartesianGrids; g++) // Cartesian grids
        {
            for( int k = m_Simulation->m_kStart[g]; k <= m_Simulation->m_kEnd[g]; k++ )
            {
                  for( int j = m_Simulation->m_jStartInt[g]; j <= m_Simulation->m_jEndInt[g]; j++ )
                  {
                        for( int i = m_Simulation->m_iStartInt[g]; i <= m_Simulation->m_iEndInt[g] ; i++ )
                        {


                        }
                    }
            }
        }





        return nullptr;
    }

    inline void Step(float dt_this)
    {
        if(m_DENSInputs.size() < 1)
        {
            LISEMS_ERROR("No material model set, at least one material model is required");
            throw 1;
        }
        //if(m_Time < 1e-20)
        //{
        //    LISEMS_ERROR("Time duration not set or too small for simulation");
        //    throw 1;
        //}

        Field * ref = m_DENSInputs.at(0);

        if(first_step)
        {
            std::cout  << "initialize " << std::endl;
            first_step = false;


            // Initialize MPI...

            MPI_Init(nullptr,nullptr);
            MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

            std::cout  << "initialize done " << std::endl;

            int initialized = false;
            MPI_Initialized(&initialized);

            std::cout << "Init succes?  " << initialized << std::endl;
            if(!initialized)
            {
                MPI_Init(nullptr, nullptr);
            }


            MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
            MPI_Comm_size(MPI_COMM_WORLD, &nProcs);


            std::stringstream ss;

            //grid
            ss << "grid nx=" << ref->nrCols() << " ny=" << ref->nrRows() << " nz=" << ref->nrLevels() << " h=" << std::fabs(ref->cellSizeX()) <<  std::endl;

            //time
            ss << "time t=" << 1.0 << std::endl;


            float max = m_Topography->GetRasterBandStats(false).max;
            //topography
            ss << "topography input=LISEM zmax=" << max << " order=3"<<std::endl;

            //material
            //we put a temporary material, later we fill with actual data manually
            ss << "block vp=5600 vs=3200 rho=2720" << std::endl;


            //source x=3.0 y=3.0 z=3.0 Mxx=1 Myy=1 Mzz=1 Mxy=0 Mxz=0 Myz=0 t0=0 freq=1 type=C6SmoothBump

            //source terms
            for(int i = 0; i < m_Sources.size(); i++)
            {
                SourcePoint p = m_Sources.at(i);

                ss << "source x=" << p.x << " y=" << p.y << " z="  << p.z << " Mxx=" << p.Mxx << " Myy=" << p.Myy << " Mzz="<< p.Mzz << " Mxy="<< p.Mxy << " Mxz=" << p.Mxz << " Myz=" << p.Myz << " t0=" << p.t << " freq=" << p.freq << " type=" << p.type.toStdString() << std::endl;

            }

            ss << "Test file " << std::endl;
            ss << "line 2" << std::endl;

            m_Simulation = new EW(ss, GlobalSources, GlobalTimeSeries, false, m_Topography);

            m_Simulation->setupRun( GlobalSources);

            //do part of the solve function that is preparation







        }

        std::cout << "step " << std::endl;
        //we require a custom solve that only does the single timestep we want
        m_Simulation->solve_prestep(GlobalSources, GlobalTimeSeries );

        float dt = m_Simulation->solve_singlestep(GlobalSources, GlobalTimeSeries, m_T);

        m_T += dt;


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
}


#endif // RASTERSEISMIC_H
