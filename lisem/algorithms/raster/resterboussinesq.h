#ifndef RESTERBOUSSINESQ_H
#define RESTERBOUSSINESQ_H

#include "geo/raster/map.h"
#include "raster/rastercommon.h"

/*
 *Scheme description for tsunami flow according to Chow et al., 2007
 *
---------------------------------
|               |               |
|       h_i,j   |Q  h_i+1,j   /\|
|               |->          P|||
---------------------------------
| | P_i,j       |               |
| V     h_i,j+1 |Q    h_i+1,j+1 |
|               |->             |
---------------------------------
*/

/*DEM_TS,
*H_TS,
*HN_TS,
*H_ABS_TS,
*U_TS,
*V_TS,
*SLOPEX_TS,
*SLOPEY_TS,
*QF_X,
*QF_Y,
*QFN_X,
*QFN_Y;
*/




static double Limiter(double a, double b)
{
    double rec = 0.0;
    if (a >= 0 && b >= 0)
    {
      rec = std::min(a, b);
    }
    else if (a <= 0 && b <= 0)
    {
        rec = std::max(a, b);
    }

    return rec;
}




static double DEMHTS_OCEANONLY(cTMap * DEM_TS, cTMap * H_TS, int r, int c, int dr, int dc)
{

    double demh = DEM_TS->data[r][c] + H_TS->data[r][c];
    if(!INSIDE(DEM_TS,r+dr,c+dc))
    {
        return demh;
    }
    if(pcr::isMV(DEM_TS->data[r+dr][c+dc]))
    {
        return demh;
    }
    if(DEM_TS->data[r+dr][c + dc] < 0.0)
    {
        return DEM_TS->data[r+dr][c+dc] + H_TS->data[r+dr][c+dc];
    }
    return demh;



}

static void TsunamiSolve(cTMap * H_TS, cTMap * U_TS, cTMap * V_TS, cTMap * DEM_TS, cTMap * N, cTMap * SLOPEX_TS, cTMap * SLOPEY_TS, cTMap * QFX, cTMap * QFY,cTMap * QFX2, cTMap * QFY2, cTMap * HN_TS, double dt)
{
    //dt = dt;

    for(int r = 0; r < H_TS->nrRows(); r++)
    {
        for(int c = 0; c < H_TS->nrCols(); c++)
        {
            if(!pcr::isMV(H_TS->data[r][c]))
            {
                if(r > 0 && r < DEM_TS->nrRows()-1 && c > 0 && c < DEM_TS->nrCols()-1)
                {
                    double dt_s = dt;
                    double dx = std::fabs(H_TS->cellSizeX());
                    double dy = std::fabs(H_TS->cellSizeY());

                    HN_TS->Drc = H_TS->data[r][c] +  dt*(QFX->data[r][c] + QFY->data[r][c])/(dx*dy);
                    HN_TS->Drc = HN_TS->data[r][c] +  -dt*(QFX2->data[r][c] + QFY2->data[r][c])/(dx*dy);




                            /*

                    if(QFX->data[r][c-1] > 0.0)
                    {
                        double q = std::min(std::fabs(dt *QFX->data[r][c-1]),  HN_TS->data[r][c-1] * dx*dy);
                        HN_TS->Drc += q/( dx*dy);
                        HN_TS->data[r][c-1] -= q/( dx*dy);
                    }
                    if(QFX->data[r][c+1] < 0.0)
                    {
                        double q = std::min(std::fabs(dt *QFX->data[r][c+1]),  HN_TS->data[r][c+1] * dx*dy);
                        HN_TS->Drc += q/( dx*dy);
                        HN_TS->data[r][c+1] -= q/( dx*dy);
                    }

                    if(QFY->data[r-1][c] > 0.0)
                    {
                        double q = std::min(std::fabs(dt *QFY->data[r-1][c]),  HN_TS->data[r-1][c] *  dx*dy);
                        HN_TS->Drc += q/ (dx*dy);
                        HN_TS->data[r-1][c] -= q/( dx*dy);
                    }
                    if(QFY->data[r+1][c] < 0.0)
                    {
                        double q = std::min(std::fabs(dt *QFY->data[r+1][c]),  HN_TS->data[r+1][c] * dx*dy);
                        HN_TS->Drc += q/( dx*dy);
                        HN_TS->data[r+1][c] -= q/( dx*dy);
                    }*/
                }
            }
        }

    }

    for(int r = 0; r < H_TS->nrRows(); r++)
    {
        for(int c = 0; c < H_TS->nrCols(); c++)
        {
            if(!pcr::isMV(H_TS->data[r][c]))
            {
                if(r > 1 && r < DEM_TS->nrRows()-2 && c > 1 && c < DEM_TS->nrCols()-2 && HN_TS->Drc > 0.0)
                {
                    double dx = std::fabs(H_TS->cellSizeX());
                    double dy = std::fabs(H_TS->cellSizeY());

                    double alpha_x = (4.0 * HN_TS->Drc *HN_TS->Drc + 9.81 * HN_TS->Drc * (dt*dt) - dx*dx)/(dx*dx);
                    double gamma_x = alpha_x +1.0;

                    double alpha_y = (4.0 * HN_TS->Drc *HN_TS->Drc + 9.81 * HN_TS->Drc * (dt*dt) - dy*dy)/(dy*dy);
                    double gamma_y = alpha_y +1.0;

                    U_TS->Drc += dt*(SLOPEX_TS->Drc * 9.81);
                    V_TS->Drc += dt*(SLOPEY_TS->Drc * 9.81);

                    bool oceanx = (DEM_TS->data[r][c] < 0.0) && (DEM_TS->data[r][c+2] < 0.0) && (DEM_TS->data[r][c +1] < 0.0) && (DEM_TS->data[r][c-1] < 0.0) && (DEM_TS->data[r][c-2] < 0.0)&& (DEM_TS->data[r+1][c+1] < 0.0)&& (DEM_TS->data[r+1][c-1] < 0.0);
                    bool oceany = (DEM_TS->data[r][c] < 0.0) && (DEM_TS->data[r+2][c] < 0.0) && (DEM_TS->data[r+1][c] < 0.0) && (DEM_TS->data[r-1][c] < 0.0) && (DEM_TS->data[r][c+2] < 0.0)&& (DEM_TS->data[r+1][c+1] < 0.0)&& (DEM_TS->data[r-1][c+1] < 0.0);

                    if(oceanx)
                    {
                        U_TS->data[r][c+1] += dt*((1.0/(12.0*dx))*alpha_x * 9.81*(DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,2) - 3.0 * DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,1) + 3.0 * DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,0) - DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,-1)));
                        U_TS->data[r][c+1] += dt*((gamma_x/(12.0*dx)) * ((DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,+1,+1) -2.0* DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,+1,0) + DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,+1,-1))-(DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,+1) -2.0* DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,0) + DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,-1))));
                    }
                    if(oceany)
                    {
                        V_TS->data[r+1][c] += dt*((1.0/(12.0*dy))*alpha_y * 9.81*(DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,2,0) - 3.0 * DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,1,0) + 3.0 * DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,0) - DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,-1,0)));
                        V_TS->data[r+1][c] += dt*((gamma_y/(12.0*dy)) * ((DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,+1,+1) -2.0* DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,+1) + DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,-1,+1))-(DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,+1,0) -2.0* DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,0) + DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,-1,0))));
                    }
                }else
                {
                    U_TS->Drc = 0.0;
                    V_TS->Drc = 0.0;
                }
            }
        }
    }

}


static double TsunamiFlux(cTMap * H_TS, cTMap * U_TS, cTMap * V_TS, cTMap * DEM_TS, cTMap * N, cTMap * SLOPEX_TS, cTMap * SLOPEY_TS, cTMap * QFX, cTMap * QFY, cTMap * QFX2, cTMap * QFY2, cTMap * HN_TS, double dt_max)
{
    double dt = dt_max;

    for(int r = 0; r < H_TS->nrRows(); r++)
    {
        for(int c = 0; c < H_TS->nrCols(); c++)
        {
            if(!pcr::isMV(H_TS->data[r][c]))
            {
                if(r > 1 && r < DEM_TS->nrRows()-2 && c > 1 && c < DEM_TS->nrCols()-2)
                {

                    double dx = std::fabs(H_TS->cellSizeX());
                    double dy = std::fabs(H_TS->cellSizeY());

                    double demwh = DEM_TS->data[r][c] + H_TS->data[r][c];

                    double demh = DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,0);//DEM_TS->data[r][c] + H_TS->data[r][c];
                    double demhx1 = DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,-1);//DEM_TS->data[r][c-1] + H_TS->data[r][c-1];
                    double demhx2 = DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,0,1);//DEM_TS->data[r][c+1] + H_TS->data[r][c+1];
                    double demhy1 = DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,-1,0);//DEM_TS->data[r-1][c] + H_TS->data[r-1][c];
                    double demhy2 = DEMHTS_OCEANONLY(DEM_TS,H_TS,r,c,1,0);//DEM_TS->data[r+1][c] + H_TS->data[r+1][c];

                    double slopex1 = -(demh - demhx1)/dx;
                    double slopex2 = -(demhx2 - demh)/dx;
                    double slopey1 = -(demh - demhy1)/dx;
                    double slopey2 = -(demhy2 - demh)/dx;

                    SLOPEX_TS->Drc = 0.5f * (slopex1);
                    SLOPEY_TS->Drc = 0.5f * (slopey1);

                    QFX->Drc = U_TS->Drc * 0.5*(H_TS->Drc + H_TS->data[r][c-1])*dy;
                    QFY->Drc = V_TS->Drc * 0.5*(H_TS->Drc + H_TS->data[r-1][c])*dx;

                    QFX2->Drc = U_TS->data[r][c+1] * 0.5*(H_TS->Drc + H_TS->data[r][c+1])*dy;
                    QFY2->Drc = V_TS->data[r+1][c] * 0.5*(H_TS->Drc + H_TS->data[r+1][c])*dx;


                    double vel_this = std::max(std::max(std::fabs(SLOPEX_TS->Drc * 9.81f),std::fabs(SLOPEX_TS->Drc * 9.81f)),std::max(std::fabs(U_TS->Drc),std::fabs(V_TS->Drc)));
                    double dt_this = 0.01 * ((std::min(dx,dy))/(std::max(std::sqrt(9.81 * (double)H_TS->data[r][c]),vel_this)));

                    dt = std::min(dt,dt_this);

                    HN_TS->Drc = H_TS->Drc;
                }
            }
        }

    }
    return dt;

}


//linear boussinesq equations with dispersion compensation  (Cho Sohn & lee, 2007)
static std::vector<cTMap*>  AS_TsunamiFlow( cTMap * DEM_TS, cTMap * H_TS, cTMap * U_TS, cTMap * V_TS, cTMap * N_TS, float _dt)
{

    std::cout << "create tsunami maps " << DEM_TS << " " << H_TS << " " << U_TS << " " << V_TS << " " << N_TS << std::endl;

    cTMap * HN_TS = H_TS->GetCopy();
    cTMap * UN_TS = U_TS->GetCopy();
    cTMap * VN_TS = V_TS->GetCopy();

    cTMap * SLOPEX_TS = H_TS->GetCopy0();
    cTMap * SLOPEY_TS = H_TS->GetCopy0();
    cTMap * QF_X1 = H_TS->GetCopy0();
    cTMap * QF_Y1 = H_TS->GetCopy0();
    cTMap * QF_X2 = H_TS->GetCopy0();
    cTMap * QF_Y2 = H_TS->GetCopy0();

    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {

        //first update flow heights
        double dt = TsunamiFlux(H_TS,UN_TS,VN_TS,DEM_TS,N_TS, SLOPEX_TS, SLOPEY_TS,QF_X1,QF_Y1,QF_X2,QF_Y2,HN_TS,_dt);
        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        TsunamiSolve(H_TS,UN_TS,VN_TS,DEM_TS,N_TS, SLOPEX_TS, SLOPEY_TS,QF_X1,QF_Y1,QF_X2,QF_Y2,HN_TS,dt);

        t = t+dt;
        iter ++;

        for(int r = 0; r < H_TS->nrRows(); r++)
        {
            for(int c = 0; c < H_TS->nrCols(); c++)
            {
                H_TS->Drc = HN_TS->Drc;
            }
        }
    }



    //HN_TS, UN_TS, VN_TS
    delete SLOPEX_TS;
    delete SLOPEY_TS;
    delete QF_X1;
    delete QF_Y1;
    delete QF_X2;
    delete QF_Y2;

    return {HN_TS,UN_TS,VN_TS};

}




#endif // RESTERBOUSSINESQ_H
