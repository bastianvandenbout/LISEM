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

inline static cTMap * AS_GaussianBump(cTMap * m, LSMVector2 point, LSMVector2 size, float intensity, float stdev, float time)
{

    cTMap * mnew = m->GetCopy0();
    float period = 1.0/stdev;

    for (int r = 0; r < m->nrRows(); r++) {
        for (int c = 0; c < m->nrCols(); c++) {

            if(!pcr::isMV(m->data[r][c]))
            {
                float dx = m->west() + c * m->cellSizeX() - point.x;
                float dy = m->north() + r * m->cellSizeY() - point.y;

                float dsq2 = dx*dx + dy* dy;

                float val = intensity * (period/std::sqrt(2.0 * LISEM_PI)) * exp(-period *period*dsq2 * time*time);

                mnew->data[r][c] = val;
            }
        }
    }

    return mnew;
}

//2d wave equation solution
inline static std::vector<cTMap *> AS_SHWaveEquation( cTMap * inU,cTMap * SX,cTMap * SY, cTMap * rho, cTMap * mu, float dt)
{
    cTMap * U = inU->GetCopy();
    cTMap * UN = inU->GetCopy();
    cTMap * UP = inU->GetCopy0();
    cTMap * SXN = SX->GetCopy();
    cTMap * SYN = SY->GetCopy();

    double maxv = 1.0;
    bool found = false;
    for (int r = 0; r < rho->nrRows(); r++) {
        for (int c = 0; c < rho->nrCols(); c++) {

            if(!pcr::isMV(rho->data[r][c]))
            {
                double v = std::sqrt(mu->data[r][c] /rho->data[r][c]);
                if(found == false)
                {

                    maxv = v;
                    found = true;
                }else
                {
                    maxv = std::max(v,maxv);

                }

            }

        }
    }


    if(! (maxv > 0.0))
    {
        LISEMS_ERROR("Wave velocity must be larger then 0");
        throw 1;
    }
    float dt_req = 0.5 * std::min(std::fabs(U->cellSizeX()),std::fabs(U->cellSizeY()))/maxv;
    int iter = std::max(1, ((int)( dt/dt_req))+1);
    float dt_s = dt/((float)(iter));

    int r, c, nrRows, nrCols;

    nrRows = U->nrRows();
    nrCols = U->nrCols();

    for (r = 0; r < nrRows; r++) {
        for (c = 0; c < nrCols; c++) {

            if(!pcr::isMV(U->data[r][c]))
            {
                double ddx2 = UF2D_Derivative(SX,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(U->cellSizeX()));
                double ddy2 = UF2D_Derivative(SY,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(U->cellSizeY()));

                if(!pcr::isMV(rho->data[r][c]))
                {
                    U->data[r][c] = UP->data[r][c] + (1.0/rho->data[r][c]) * (ddx2 + ddy2);
                }
            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        SWAPMAP(UP,U);
        SWAPMAP(UN,U);

        SWAPMAP(SXN,SX);
        SWAPMAP(SYN,SY);


        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {

                if(!pcr::isMV(U->data[r][c]))
                {
                    double ddx2 = UF2D_Derivative(SX,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(SX->cellSizeX()));
                    double ddy2 = UF2D_Derivative(SY,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(SY->cellSizeY()));

                    double dudx = UF2D_Derivative(U,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(U->cellSizeX()));
                    double dudy = UF2D_Derivative(U,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR,std::fabs(U->cellSizeY()));

                    double du = 0.0;
                    if(!pcr::isMV(rho->data[r][c]))
                    {
                        du = dt_s * (1.0/rho->data[r][c]) * (ddx2 + ddy2);
                    }
                    SXN->data[r][c] = SX->data[r][c] + dt_s * dudx * mu->data[r][c];
                    SXN->data[r][c] = SX->data[r][c] + dt_s * dudy * mu->data[r][c];

                    UN->data[r][c] = (2.0 * U->data[r][c]  - UP->data[r][c]) +( du);
                }
            }
        }

    }


    delete U;
    delete UP;
    return {UN, SXN, SYN};
}



//2d wave equation solution
/*inline static std::vector<cTMap *> AS_PWaveEquation( cTMap * inUX, cTMap * inUY, cTMap * inSX,cTMap * inSXY,cTMap * inSY, cTMap * rho, cTMap * mux, cTMap * muxy, cTMap * muy, float dt)
{
    cTMap * UX = inU->GetCopy();
    cTMap * UNX = inU->GetCopy();
    cTMap * UPX = inU->GetCopy0();
    cTMap * UY = inU->GetCopy();
    cTMap * UNY = inU->GetCopy();
    cTMap * UPY = inU->GetCopy0();

    double maxc = MapMaximum(cc);

    if(! (maxc > 0.0))
    {
        LISEMS_ERROR("Wave velocity must be larger then 0");
        throw 1;
    }
    float dt_req = 0.5 * std::min(std::fabs(UX->cellSizeX()),std::fabs(UX->cellSizeY()))/maxc;
    int iter = std::max(1, ((int)( dt/dt_req))+1);
    float dt_s = dt/((float)(iter));

    int r, c, nrRows, nrCols;

    nrRows = UX->nrRows();
    nrCols = UX->nrCols();

    for (r = 0; r < nrRows; r++) {
        for (c = 0; c < nrCols; c++) {

            if(!pcr::isMV(U->data[r][c]))
            {
                double ddx2x = UF2D_Derivative2(UX,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(UX->cellSizeX()));
                double ddy2x = UF2D_Derivative2(UX,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(UX->cellSizeY()));
                double ddx2y = UF2D_Derivative2(UY,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(UY->cellSizeX()));
                double ddy2y = UF2D_Derivative2(UY,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(UY->cellSizeY()));


                if(!pcr::isMV(cc->data[r][c]))
                {
                    UX->data[r][c] = UPX->data[r][c] + (ddx2 + ddy2)*ccx->data[r][c]*ccx->data[r][c];
                    UY->data[r][c] = UPY->data[r][c] + (ddx2 + ddy2)*ccy->data[r][c]*ccy->data[r][c];
                }
            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        SWAPMAP(UPX,UX);
        SWAPMAP(UNX,UX);
        SWAPMAP(UPY,UY);
        SWAPMAP(UNY,UY);
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {

                if(!pcr::isMV(U->data[r][c]))
                {
                    double ddx2x = UF2D_Derivative2(UY,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(UX->cellSizeX()));
                    double ddy2x = UF2D_Derivative2(UY,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(UX->cellSizeY()));

                    double ddx2y = UF2D_Derivative2(UY,r,c, UF_DIRECTION_X,UF_DERIVATIVE_LR,std::fabs(UY->cellSizeX()));
                    double ddy2y = UF2D_Derivative2(UY,r,c, UF_DIRECTION_Y,UF_DERIVATIVE_LR, std::fabs(UY->cellSizeY()));


                    double dux = 0.0;
                    if(!pcr::isMV(ccx->data[r][c]))
                    {
                        dux = dt_s *(ddx2x + ddy2x)*ccx->data[r][c]*ccx->data[r][c];
                    }
                    UNX->data[r][c] = (2.0 * UX->data[r][c]  - UPX->data[r][c]) +( dux);



                    double duy = 0.0;
                    if(!pcr::isMV(ccy->data[r][c]))
                    {
                        duy = dt_s *(ddx2y + ddy2y)*ccy->data[r][c]*ccy->data[r][c];
                    }
                    UNY->data[r][c] = (2.0 * UY->data[r][c]  - UPY->data[r][c]) +( duy);
                }
            }
        }

    }


    delete UX;
    delete UPX;
    delete UY;
    delete UPY;
    return {UNX,UNY};
}*/



//inline static std::vector<cTMap *> AS_ViscoPlastic2DWaveEquation(cTMap * )
//{



//}

#include "seismic/EW.h"

#include "geo/raster/field.h"
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mpi.h>
#include "linear/lsm_vector3.h"

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
    RasterBandStats topostats;
    LSMVector3 m_DomainRef;

    float m_CellSize = 0.0;
    float m_ZMaxFrac = 0.25;

    inline SeismicModel()
    {


    }

    inline ~SeismicModel()
    {
        MPI_Finalize();
    }


    inline void SetTopography(cTMap * dem)
    {
        m_Topography = dem->GetCopy();

        topostats = m_Topography->GetRasterBandStats(false);
        if( topostats.n_mv > 0)
        {
            LISEMS_ERROR("Can not run seismic simulation for data with missing values");
            throw 1;
        }
        FOR_ROW_COL(m_Topography)
        {
            //we use z = -100 (downward is negative with sw-4) as the startof topography
           m_Topography->data[r][c] += -topostats.min + 100.0f;
        }
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
    inline Field * FillField(Field * f,  QString parameter,bool nearest)
    {

        //since there can be many cartesian grids of different sizes, and a curvlinear grid on top,
        //we must do something more complicated than copying data.

        //Option1: take, for each cell in field, the center, and sample a value.
        //This is simple but not so ideal for visualizing at lower/higher resolutions

        //Option2!: we go over all the simulation grids, and add to the relevant grid in field (if it exists)
        //keep count of the number of points actual in the cell.
        //if this number turns out lower than 2, we do trilinear interpolation
        //if this number is higher than 2, we average all the contributions of the found values
        //this works well for alternative resolutions, but will need to go through the full SW4 grids,
        //even if we are filling only a small field.
        //Could be made faster by calculating some bounds on the sw4 grid we need to check!

        Field * res = f->GetCopy0();
        Field * resn = f->GetCopy0();

        MPI_Barrier(MPI_COMM_WORLD);

        //only for thread 1, do the memory allocation

        int size = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        //get world size

        int worldsize = 1;

        if(worldsize > 1)
        {
            //we have to get the data from each process


                //for each world, get dims



                //for each world. get data


                //for each world, put data in global grid

        }else
        {
            std::cout << "sim size " << m_Simulation->m_nz_base <<" " <<  m_Simulation->m_ny_base << " " <<  m_Simulation->m_nx_base <<std::endl;

            //fill the array based on our instance of the EW class
            for(int g = 0 ; g < m_Simulation->mNumberOfGrids; g++ )
            {
                std::cout << "grid " << g << "  " << m_Simulation->m_kStart[g] << " " <<  m_Simulation->m_kEnd[g]<< " " <<  m_Simulation->m_jEnd[g]<< "  " << m_Simulation->m_jStart[g] << "  " << m_Simulation->m_iStart[g] << " " <<  m_Simulation->m_iEnd[g] <<std::endl;
                for(int k=m_Simulation->m_kStart[g]; k< m_Simulation->m_kEnd[g]; k++ )
                     {

                       {

                        double z;

                        if(g == m_Simulation->mNumberOfGrids-1)
                        {
                            int i = 1;
                            int j = 1;

                              z = m_Simulation->mZ(i,j,k);
                        }else
                        {
                            z = m_Simulation->m_zmin[g]+(k-1)*m_Simulation->mGridSize[g];

                        }

                        double z2 = z + m_DomainRef.z;
                        int l = (z - res->GetBottom())/std::fabs(res->cellSizeZ());
                        std::cout << "z " << z << " " << z2 <<  " " << l << " " << res->GetBottom() << " " <<m_Simulation->m_zmin[g] << std::endl;
                    }

                        for(int j=m_Simulation->m_jStart[g]; j< m_Simulation->m_jEnd[g]; j++ )
                         {
                            for(int i=m_Simulation->m_iStart[g]; i< m_Simulation->m_iEnd[g]; i++ )
                            {
                            if(k < 0 || j < 0 || i < 0)
                            {
                                continue;
                            }

                            double x;
                            double y;
                            double z;
                            double dx;
                            double dy;
                            double dz;

                            double val =std::sqrt(m_Simulation->m_Uacc[g](0,i,j,k) * m_Simulation->m_Uacc[g](0,i,j,k) + m_Simulation->m_Uacc[g](1,i,j,k) * m_Simulation->m_Uacc[g](1,i,j,k) + m_Simulation->m_Uacc[g](2,i,j,k) * m_Simulation->m_Uacc[g](2,i,j,k));


                            if(g == m_Simulation->mNumberOfGrids-1)
                            {

                                  x = m_Simulation->mX(i,j,k);
                                  y = m_Simulation->mY(i,j,k);
                                  z = -(m_Simulation->mZ(i,j,k));

                                  dx = m_Simulation->mGridSize[g];
                                  dy = m_Simulation->mGridSize[g];
                                  dz = 0.0;
                                  if(k == m_Simulation->m_kStart[g])
                                  {
                                      dz = m_Simulation->mZ(i,j,k+1) - m_Simulation->mZ(i,j,k);

                                  }else if( k == m_Simulation->m_kEnd[g])
                                  {
                                      dz = 0.5*(m_Simulation->mZ(i,j,k+1) - m_Simulation->mZ(i,j,k-1));
                                  }else
                                  {
                                      dz = m_Simulation->mZ(i,j,k) - m_Simulation->mZ(i,j,k-1);
                                  }
                                  double depth;
                                  /*if (m_Simulation->m_absoluteDepth)
                                  {
                                    depth = z;
                                  }
                                  else
                                  {
                                      depth = z - m_Simulation->mZ(i,j,1);

                                     }*/


                            }else
                            {
                                x = (i-1)*m_Simulation->mGridSize[g]                ;
                                y = (j-1)*m_Simulation->mGridSize[g]                ;
                                z = -(m_Simulation->m_zmin[g]+(k-1)*m_Simulation->mGridSize[g]);

                                dx = m_Simulation->mGridSize[g];
                                dy = m_Simulation->mGridSize[g];
                                dz = m_Simulation->mGridSize[g];

                                double depth;
                                /*if (m_Simulation->m_absoluteDepth)
                                {
                                  depth = z;
                                }
                                else
                                {
                                    depth = m_Simulation->getDepth(x, y, z, depth);
                                  }*/

                                //mu_tmp = mMu[g](i,j,k);

                            }


                            //now we emplace it into the field
                            //convert coordinates to real-world coords
                            x = x + m_DomainRef.x;
                            y = y + m_DomainRef.y;
                            z = z + m_DomainRef.z;

                            //now get indices to the field
                            int l = (z - res->GetBottom())/std::fabs(res->cellSizeZ());
                            int c = (x - res->GetWest())/std::fabs(res->cellSizeX());
                            int r = (y - res->GetNorth())/std::fabs(res->cellSizeY());

                            //increase counter if it is inside
                            int nl = std::max(1,((int) (0.5+std::fabs(dz/(res->cellSizeZ())))));
                            int nc = std::max(1,((int) (0.5+std::fabs(dx/(res->cellSizeX())))));
                            int nr = std::max(1,((int) (0.5+std::fabs(dy/(res->cellSizeY())))));

                            //get window to put the values in

                            int lmin = std::max(0,std::min(res->nrLevels()-1,l - (nl-1)/2));
                            int lmax = std::max(0,std::min(res->nrLevels(),l + 1 + ((nl)/2)));

                            int rmin = std::max(0,std::min(res->nrRows()-1,r - (nr-1)/2));
                            int rmax = std::max(0,std::min(res->nrRows(),r + 1 + ((nr)/2)));

                            int cmin = std::max(0,std::min(res->nrCols()-1,c - (nc-1)/2));
                            int cmax = std::max(0,std::min(res->nrCols(),c + 1 + ((nc)/2)));

                            //loop over this pixel

                            //std::cout << "k " << k << " i " << i << " j " << j << " x" << x << " y " << y << " z " << z << "  " << l << " " << r <<  " " << c <<  "  - range: " <<  lmin << " " << lmax << " " << rmin << " " << rmax << " " << cmin << " " << cmax << std::endl;

                            for(int l = lmin; l < lmax; l++)
                            {
                                for(int c = cmin; c < cmax; c++)
                                {
                                    for(int r = rmin; r < rmax; r++)
                                    {
                                        //get the distance
                                        float xc = (res->GetWest() + ((float)(c)) *res->cellSizeX() - x)/std::max(1e-6,dx);
                                        float yc = (res->GetNorth() + ((float)(r)) *res->cellSizeY() - y)/std::max(1e-6,dy);
                                        float zc = (res->GetBottom() + ((float)(l)) *res->cellSizeZ() - z)/std::max(1e-6,dz);
                                        float dist = sqrt(xc*xc + yc *yc + zc*zc);

                                        //get the weight
                                        //for bilinear interpolation it is inverse to distance
                                        //for averaging we can try to estimate the fraction that this value forms of the larger cell

                                        float w;
                                        if(nl > 1 || nc > 1 || nr > 1)
                                        {
                                            w = 1.0f/std::max(1e-10f,dist);
                                        }else
                                        {
                                            w = 1.0;
                                        }
                                        resn->at(l)->data[r][c] += w;
                                        res->at(l)->data[r][c] += w * val;

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        for(int l = 0; l < res->nrLevels(); l++)
        {
            for(int c = 0; c < res->nrCols(); c++)
            {
                for(int r = 0; r < res->nrRows(); r++)
                {


                }
            }
        }



        delete resn;

        return res;
    }

    inline void SetCellSize(float x)
    {
        m_CellSize = x;

    }

    inline void SetZMaxFrac(float x)
    {
        m_ZMaxFrac = x;
    }
    inline void Step(float dt_this)
    {
        if(m_DENSInputs.size() < 1)
        {
            LISEMS_ERROR("No material model set, at least one material model is required");
            throw 1;
        }




        Field * ref = m_DENSInputs.at(0);

        std::cout << "edges " << ref->GetWest() << " " << ref->GetNorth() << " " << m_Topography->west() << " " << m_Topography->north() << std::endl;

        float min= topostats.min;
        float max = topostats.max;
        float bottom = ref->GetBottom() + 100.0f;
        float top = ref->GetBottom() + ref->GetSizeZ() + 100.0f;

        if(bottom > top)
        {
            float temp = bottom;
            bottom = top;
            top = temp;
        }

        //The Field must be at least lower than the topo bottom
        //The top of the topo layer might be higher the the field top
        //The reference of the domain is Field ULX, Field ULY, Topo Bottom Z
        //the size of the curvlinear layer is equal to (1/5) * TopoBottom - FieldBottom

        //        - Topo Top                   (z= -500 relative to reference of domain)
        //      -- --
        //== ---     -----   ==== Field Top
        //--- Topo Bottom -------              (z= -100 relative to reference of domain)
        //
        //~~~~~~~~~~~~~~~~~~~~~~~ Curvlinear ^ (z = 1000 relative to reference of domain)
        // Field Domain
        //
        //======================= Field Bottom (e.g. z= 5000 relative to reference of domain)

        if(min < bottom)
        {
            LISEMS_ERROR("Bottom of elevation is lower than elevation")
            throw 1;
        }

        float zmax = (min - bottom)* m_ZMaxFrac;
        float size_z = (min-bottom);

        m_DomainRef = LSMVector3(ref->GetWest(),ref->GetNorth(),0.0);

        if(m_CellSize < 1e-6)
        {
            m_CellSize = std::fabs(ref->cellSizeX());
        }

        //if(m_Time < 1e-20)
        //{
        //    LISEMS_ERROR("Time duration not set or too small for simulation");
        //    throw 1;
        //}

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



            MPI_Barrier(MPI_COMM_WORLD);

            std::cout << "bottom " << bottom << " " << zmax << " "  << min << " " << max << std::endl;

            std::stringstream ss;

            //grid
            ss << "grid x=" << std::fabs(ref->GetSizeX()) << " y=" << std::fabs(ref->GetSizeY()) << " z=" << min - bottom << " h=" << std::fabs(m_CellSize) <<  std::endl;

            //time
            ss << "time t=" << 1.0 << std::endl;

            //topography
            ss << "topography input=LISEM zmax=" << zmax << " order=3"<<std::endl;

            //material
            //we put a temporary material, later we fill with actual data manually
            ss << "block vp=5600 vs=3200 rho=2720" << std::endl;


            //source x=3.0 y=3.0 z=3.0 Mxx=1 Myy=1 Mzz=1 Mxy=0 Mxz=0 Myz=0 t0=0 freq=1 type=C6SmoothBump

            //source terms
            for(int i = 0; i < m_Sources.size(); i++)
            {
                SourcePoint p = m_Sources.at(i);

                float x_rel = p.x - m_DomainRef.x;
                float y_rel = p.y - m_DomainRef.y;

                if(ref->cellSizeX() < 0.0f)
                {
                    x_rel = -x_rel;
                }
                if(ref->cellSizeY() < 0.0f)
                {
                    y_rel = -y_rel;
                }

                ss << "source x=" << x_rel << " y=" << y_rel << " z="  << p.z - m_DomainRef.z - 100.0f << " Mxx=" << p.Mxx << " Myy=" << p.Myy << " Mzz="<< p.Mzz << " Mxy="<< p.Mxy << " Mxz=" << p.Mxz << " Myz=" << p.Myz << " t0=" << p.t << " freq=" << p.freq << " type=" << p.type.toStdString() << std::endl;

            }

            ss << "Test file " << std::endl;
            ss << "line 2" << std::endl;

            m_Simulation = new EW(ss, GlobalSources, GlobalTimeSeries, false, m_Topography);

            m_Simulation->setupRun( GlobalSources);

            //do part of the solve function that is preparation

            //we require a custom solve that only does the single timestep we want
            m_Simulation->solve_prestep(GlobalSources, GlobalTimeSeries );

            MPI_Barrier(MPI_COMM_WORLD);




        }

        MPI_Barrier(MPI_COMM_WORLD);

        std::cout << "step " << std::endl;

        float dt = m_Simulation->solve_singlestep(GlobalSources, GlobalTimeSeries, m_T);

        m_T += dt;

        MPI_Barrier(MPI_COMM_WORLD);
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
