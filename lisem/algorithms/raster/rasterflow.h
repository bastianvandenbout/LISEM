#ifndef RASTERFLOW_H
#define RASTERFLOW_H

#include "QString"
#include "geo/raster/map.h"
#include "rasterconstructors.h"
#include "linear/lsm_vector3.h"
#include "raster/rastercommon.h"
#include "rasterderivative.h"
#include "navierstokes/fluid2p2d.h"

static int ch_dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
static int ch_dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};
#define GRAV 9.81

static float minmod(float a, float b)
{
    float rec = 0;
    if (a >= 0 && b >= 0)
    {
        rec = std::min(a, b);
    }else if(a <= 0 && b <= 0)
    {
        rec = std::max(a, b);
    }
    return rec;
}


static LSMVector3 F_HLL2FL(float h_L,float u_L,float v_L,float h_R,float u_R,float v_R)
{
        LSMVector3 ret;

        float grav_h_L = GRAV*h_L;
        float grav_h_R = GRAV*h_R;
        float sqrt_grav_h_L = std::sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = std::sqrt((float)(grav_h_R));
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;

        float sc1 = std::min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = std::max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
        float tmp = 1.0/std::max((float)0.1,(float)(sc2-sc1));
        float t1 = (std::min((float)(sc2),(float)(0.)) - std::min((float)(sc1),(float)(0.)))*tmp;
        float t2 = 1.0 - t1;
        float t3_1 = std::fabs((float)(sc1));
        float t3_2 = std::fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5*tmp;

        if(h_L > 0.0 || h_R > 0.0 )
        {

            //std::cout << h_L << " " << h_R << " "<< t1 << " "  << t2 << std::endl;
        }
        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5) + t2*(q_L*u_L + grav_h_L*h_L*0.5) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);

        return ret;
}


static inline float flow_saintvenantdt(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HN,cTMap * VXN,cTMap * VYN, cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr)
{

    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();

    float dt_min = 1e6;

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {

                int x = r;
                int y = c;

                const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                float vmax = 1000.0;

                float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
                float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

                float dt_req = 0.25f *dx/( std::min(1000.0f,std::max(0.01f,(sqrt(vx*vx + vy * vy)))));
                dt_min = std::min(dt_min,dt_req);

            }

        }
    }

    return dt_min;
}



static inline void flow_saintvenant(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HN,cTMap * VXN,cTMap * VYN, cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr, float dt = 0.1)
{

    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();

    float tx = dt/dx;
    /*float dt_min = 1e6;

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {

            int x = r;
            int y = c;

            const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
            const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

            float vmax = 0.1 * dx/dt;

            float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
            float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

            float dt_req = 0.25f *dx/( std::min(100.0f,std::max(0.01f,(sqrt(vx*vx + vy * vy)))));
            dt_min = std::min(dt_min,dt_req);

        }
    }*/



    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                int x = r;
                int y = c;
                const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                const int gy_x1 = gy;
                const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                const int gy_x2 = gy;
                const int gx_y1 = gx;
                const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                const int gx_y2 = gx;
                const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));

                float z = DEM->data[gy][gx];
                float n = std::max(0.00001f,N->data[gy][gx]);

                float z_x1 = DEM->data[gy][gx_x1];
                float z_x2 = DEM->data[gy][gx_x2];
                float z_y1 = DEM->data[gy_y1][gx];
                float z_y2 = DEM->data[gy_y2][gx];


                if(gx + 1 > dim0-1)
                {
                    z_x2 = -99999;
                }
                if(gx - 1 < 0)
                {
                    z_x1 = -99999;
                }
                if(gy + 1 > dim1-1)
                {
                    z_y2 = -99999;
                }
                if(gy - 1 < 0)
                {
                    z_y1 = -99999;
                }
                float zc_x1 = z_x1 < -99995? z : z_x1;
                float zc_x2 = z_x2 < -99995? z : z_x2;
                float zc_y1 = z_y1 < -99995? z : z_y1;
                float zc_y2 = z_y2 < -99995? z : z_y2;

                float h = std::max(0.0f,H->data[gy][gx]);
                float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                float h_corr_x1 =  std::max(0.0f,(h_x1 - std::max(0.0f,z - zc_x1)));
                float h_corr_x1b =  std::max(0.0f,(h - std::max(0.0f,zc_x1-z)));
                float h_corr_x2 =  std::max(0.0f,(h - std::max(0.0f,zc_x2 - z)));
                float h_corr_x2b =  std::max(0.0f,(h_x2 - std::max(0.0f,z - zc_x2)));
                float h_corr_y1 =  std::max(0.0f,(h_y1 - std::max(0.0f,z - zc_y1)));
                float h_corr_y1b =  std::max(0.0f,(h - std::max(0.0f,zc_y1 - z)));
                float h_corr_y2 =  std::max(0.0f,(h - std::max(0.0f,zc_y2 - z)));
                float h_corr_y2b =  std::max(0.0f,(h_y2 - std::max(0.0f,z - zc_y2)));

                float vmax = 0.1 * dx/dt;

                float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
                float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

                float vx_x1 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x1]));
                float vy_x1 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x1]));
                float vx_x2 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x2]));
                float vy_x2 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x2]));
                float vx_y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx]));
                float vy_y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx]));
                float vx_y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx]));
                float vy_y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx]));

                float zh = z + h;
                float zh_x1 =std::min(1.0f,h_x1/std::max(0.0001f,h)) * (zc_x1 + h_x1) + (1.0f-std::min(1.0f,h_x1/std::max(0.0001f,h)))*std::min(zh,zc_x1 + h_x1);
                float zh_x2 =std::min(1.0f,h_x2/std::max(0.0001f,h)) * (zc_x2 + h_x2) + (1.0f-std::min(1.0f,h_x2/std::max(0.0001f,h)))*std::min(zh,zc_x2 + h_x2);
                float zh_y1 =std::min(1.0f,h_y1/std::max(0.0001f,h)) * (zc_y1 + h_y1) + (1.0f-std::min(1.0f,h_y1/std::max(0.0001f,h)))*std::min(zh,zc_y1 + h_y1);
                float zh_y2 =std::min(1.0f,h_y2/std::max(0.0001f,h)) * (zc_y2 + h_y2) + (1.0f-std::min(1.0f,h_y2/std::max(0.0001f,h)))*std::min(zh,zc_y2 + h_y2);

                float sx_zh_x2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_x2-zh)/dx)));
                float sy_zh_y1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_y1)/dx)));
                float sx_zh_x1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_x1)/dx)));
                float sy_zh_y2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_y2-zh)/dx)));

                float sx_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sx_zh_x1 + sx_zh_x2)));
                float sy_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sy_zh_y1 + sy_zh_y2)));

                LSMVector3 hll_x1 = z_x1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x1,vx_x1,vy_x1,h_corr_x1b,vx,vy);
                LSMVector3 hll_x2 = z_x2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x2,vx,vy,h_corr_x2b,vx_x2,vy_x2);
                LSMVector3 hll_y1 = z_y1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y1,vy_y1,vx_y1,h_corr_y1b,vy,vx);
                LSMVector3 hll_y2 = z_y2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y2,vy,vx,h_corr_y2b,vy_y2,vx_y2);

                float C = 0.1f;

                float flux_x1 = z_x1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_x1.x)),h_x1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_x2 = z_x2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_x2.x)),h_x2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y1 = z_y1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_y1.x)),h_y1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y2 = z_y2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_y2.x)),h_y2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))

                int edges = ((z_x1 < -99995)?1:0) +((z_x2 < -99995)?1:0)+((z_y1 < -99995)?1:0)+((z_y2 < -99995)?1:0);

                bool edge = (z_x1 < -99995 || z_x2 < -99995 || z_y1 < -99995 || z_y2 < -99995);
                float hold = h;
                float hn = ((std::max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));


                float f_centre_x = 0.5 * GRAV*((z<zc_x2?std::min(zc_x2,z + h) : std::min(z,zc_x2 + h_x2)) - (z<zc_x1?std::min(zc_x1,z + h) : std::min(z,zc_x1 + h_x1)))*(h_corr_x1 + h_corr_x2);
                float f_centre_y = 0.5 * GRAV*((z<zc_y2?std::min(zc_y2,z + h) : std::min(z,zc_y2 + h_y2)) - (z<zc_y1?std::min(zc_y1,z + h) : std::min(z,zc_y1 + h_y1)))*(h_corr_y1 + h_corr_y2);


                if(h > 0.01f )
                {
                    //std::cout << "h: " << h << " - " << sx_zh << " " << sy_zh << " " << h_x1 << " " << h_x2 << " " << h_corr_x1 << " " << h_corr_x2 << " " << h_corr_x1b << " " << h_corr_x2b << " " << hll_x1.y << "  " << hll_x2.y  <<  " " << zc_x1 << " " << zc_x2 << std::endl;
                    //std::cout << "f: " << tx*(hll_x2.y - hll_x1.y + f_centre_x) << " " << hll_x2.y - hll_x1.y << " " <<  f_centre_x << " " << hll_y2.z - hll_y1.z << std::endl;
                }

                float qxn = h * vx - tx*(hll_x2.y - hll_x1.y + f_centre_x) - tx*(hll_y2.z - hll_y1.z);//- 0.5 * GRAV *hn*sx_zh * dt;
                float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y + f_centre_y);//- 0.5 * GRAV *hn*sy_zh * dt;

                float vsq = sqrt((float)(vx * vx + vy * vy));
                float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
                float nsq = nsq1*vsq*dt;

                float vxn = (float)((qxn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));
                float vyn = (float)((qyn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));

                float threshold = std::min(0.1f,0.01f * dx);
                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vxn -vx)/std::max(0.0001f,dt);

                    float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

                    vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

                }

                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vyn -vy)/std::max(0.0001f,dt);

                    float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

                    vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

                }

                hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
                vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
                vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;


                //write output
                HN->data[gy][gx] = hn;
                VXN->data[gy][gx] = vxn;
                VYN->data[gy][gx] = vyn;

                if(QX1 != nullptr && QX2 != nullptr && QY1 != nullptr && QY2 != nullptr)
                {

                    QX1->data[gy][gx] = flux_x1 *(dx*dx);
                    QX2->data[gy][gx] = flux_x2 *(dx*dx);
                    QY1->data[gy][gx] = flux_y1 *(dx*dx);
                    QY2->data[gy][gx] = flux_y2 *(dx*dx);
                }
            }
        }
    }
    return;
}

static inline void flow_saintvenantdtandflow(double _dt, cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HN,cTMap * VXN,cTMap * VYN, cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr, float courant = 0.1)
{



    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();


    double dt = 1e6;
    float dt_min = 1e6;
    float dt_store = _dt;
    bool stop = false;
    #pragma omp parallel shared(stop,dt_min,dt)
    {
        double t = 0;
        double t_end = _dt;

        int iter = 0;
        while(true)
        {

            #pragma omp single
            {
                dt_min = 1e6;
                dt = 1e6;
            }

            if(!(t < t_end))
            {
                stop = true;
            }
            #pragma omp barrier
            if(stop)
            {
                break;
            }

            double dt_min_this = 1e6;
             //get dt
            #pragma omp for collapse(2) reduction(min:dt_min,dt)
            for(int r = 0; r < DEM->nrRows(); r++)
            {
                for (int c = 0; c < DEM->nrCols(); c++)
                {

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        int x = r;
                        int y = c;

                        const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                        const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));


                        const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                        const int gy_x1 = gy;
                        const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                        const int gy_x2 = gy;
                        const int gx_y1 = gx;
                        const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                        const int gx_y2 = gx;
                        const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));


                        float z = DEM->data[gy][gx];

                        float z_x1 = DEM->data[gy][gx_x1];
                        float z_x2 = DEM->data[gy][gx_x2];
                        float z_y1 = DEM->data[gy_y1][gx];
                        float z_y2 = DEM->data[gy_y2][gx];


                        if(gx + 1 > dim0-1)
                        {
                            z_x2 = -99999;
                        }
                        if(gx - 1 < 0)
                        {
                            z_x1 = -99999;
                        }
                        if(gy + 1 > dim1-1)
                        {
                            z_y2 = -99999;
                        }
                        if(gy - 1 < 0)
                        {
                            z_y1 = -99999;
                        }
                        float zc_x1 = z_x1 < -99995? z : z_x1;
                        float zc_x2 = z_x2 < -99995? z : z_x2;
                        float zc_y1 = z_y1 < -99995? z : z_y1;
                        float zc_y2 = z_y2 < -99995? z : z_y2;

                        float h = std::max(0.0f,H->data[gy][gx]);
                        float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                        float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                        float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                        float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                        float hw_x1 = h > h_x1? std::max(0.0f,std::min(h,h+z - (h_x1 +z_x1))) : 0.0;
                        float hw_x2 = h > h_x2? std::max(0.0f,std::min(h,h+z - (h_x2 +z_x2))) : 0.0;
                        float hw_y1 = h > h_y1? std::max(0.0f,std::min(h,h+z - (h_y1 +z_y1))) : 0.0;
                        float hw_y2 = h > h_y2? std::max(0.0f,std::min(h,h+z - (h_y2 +z_y2))) : 0.0;

                        float vmax = 1000.0;

                        float vx = std::max(std::sqrt(9.81f * std::fabs(hw_x2)),std::max(std::sqrt(9.81f * std::fabs(hw_x1)),std::min(vmax,std::max(-vmax,VX->data[gy][gx]))));
                        float vy = std::max(std::sqrt(9.81f * std::fabs(hw_y2)),std::max(std::sqrt(9.81f * std::fabs(hw_y1)),std::min(vmax,std::max(-vmax,VY->data[gy][gx]))));

                        double dt_req = courant *dx/( std::min(1000.0f,std::max(0.01f,(sqrt(vx*vx + vy * vy)))));
                        dt_min = std::min((double)dt_min,dt_req);
                        dt = std::min((double)dt_min,dt);
                    }

                }
            }

            //#pragma omp critical
            {
                //dt = dt_min;
                //dt_min = std::min(dt_min,(float)dt_min_this);
                //dt_store = std::min(t_end - t,std::max((double)dt_min,1e-6));

                  //std::cout << "thread  " <<omp_get_thread_num() << " " <<  dt << " " << dt_min << " " << dt_min_this << " " << dt_store << std::endl;

                //reset already for next time
                //dt_min = 1e6;
            }

            //wait
            #pragma omp barrier

            //reduce for minimum dt
            //#pragma omp single
            {
               //dt = dt_store;
            }


            //wait
            #pragma omp barrier


        //#pragma omp single
        //{
        //    std::cout << "dt  " << dt << std::endl;
        //}

            //do flow calc


            float tx = dt/dx;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->nrRows(); r++)
            {
                for (int c = 0; c < DEM->nrCols(); c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        int x = r;
                        int y = c;
                        const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                        const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                        const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                        const int gy_x1 = gy;
                        const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                        const int gy_x2 = gy;
                        const int gx_y1 = gx;
                        const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                        const int gx_y2 = gx;
                        const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));

                        float z = DEM->data[gy][gx];
                        float n = std::max(0.00001f,N->data[gy][gx]);

                        float z_x1 = DEM->data[gy][gx_x1];
                        float z_x2 = DEM->data[gy][gx_x2];
                        float z_y1 = DEM->data[gy_y1][gx];
                        float z_y2 = DEM->data[gy_y2][gx];


                        if(gx + 1 > dim0-1)
                        {
                            z_x2 = -99999;
                        }
                        if(gx - 1 < 0)
                        {
                            z_x1 = -99999;
                        }
                        if(gy + 1 > dim1-1)
                        {
                            z_y2 = -99999;
                        }
                        if(gy - 1 < 0)
                        {
                            z_y1 = -99999;
                        }
                        float zc_x1 = z_x1 < -99995? z : z_x1;
                        float zc_x2 = z_x2 < -99995? z : z_x2;
                        float zc_y1 = z_y1 < -99995? z : z_y1;
                        float zc_y2 = z_y2 < -99995? z : z_y2;

                        float h = std::max(0.0f,H->data[gy][gx]);
                        float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                        float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                        float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                        float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                        float h_corr_x1 =  std::max(0.0f,(h_x1 - std::max(0.0f,z - zc_x1)));
                        float h_corr_x1b =  std::max(0.0f,(h - std::max(0.0f,zc_x1-z)));
                        float h_corr_x2 =  std::max(0.0f,(h - std::max(0.0f,zc_x2 - z)));
                        float h_corr_x2b =  std::max(0.0f,(h_x2 - std::max(0.0f,z - zc_x2)));
                        float h_corr_y1 =  std::max(0.0f,(h_y1 - std::max(0.0f,z - zc_y1)));
                        float h_corr_y1b =  std::max(0.0f,(h - std::max(0.0f,zc_y1 - z)));
                        float h_corr_y2 =  std::max(0.0f,(h - std::max(0.0f,zc_y2 - z)));
                        float h_corr_y2b =  std::max(0.0f,(h_y2 - std::max(0.0f,z - zc_y2)));

                        float vmax = 0.1 * dx/dt;

                        float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
                        float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

                        float vx_x1 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x1]));
                        float vy_x1 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x1]));
                        float vx_x2 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x2]));
                        float vy_x2 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x2]));
                        float vx_y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx]));
                        float vy_y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx]));
                        float vx_y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx]));
                        float vy_y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx]));

                        float zh = z + h;
                        float zh_x1 =std::min(1.0f,h_x1/std::max(0.0001f,h)) * (zc_x1 + h_x1) + (1.0f-std::min(1.0f,h_x1/std::max(0.0001f,h)))*std::min(zh,zc_x1 + h_x1);
                        float zh_x2 =std::min(1.0f,h_x2/std::max(0.0001f,h)) * (zc_x2 + h_x2) + (1.0f-std::min(1.0f,h_x2/std::max(0.0001f,h)))*std::min(zh,zc_x2 + h_x2);
                        float zh_y1 =std::min(1.0f,h_y1/std::max(0.0001f,h)) * (zc_y1 + h_y1) + (1.0f-std::min(1.0f,h_y1/std::max(0.0001f,h)))*std::min(zh,zc_y1 + h_y1);
                        float zh_y2 =std::min(1.0f,h_y2/std::max(0.0001f,h)) * (zc_y2 + h_y2) + (1.0f-std::min(1.0f,h_y2/std::max(0.0001f,h)))*std::min(zh,zc_y2 + h_y2);

                        float sx_zh_x2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_x2-zh)/dx)));
                        float sy_zh_y1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_y1)/dx)));
                        float sx_zh_x1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_x1)/dx)));
                        float sy_zh_y2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_y2-zh)/dx)));

                        float sx_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sx_zh_x1 + sx_zh_x2)));
                        float sy_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sy_zh_y1 + sy_zh_y2)));

                        LSMVector3 hll_x1 = z_x1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x1,vx_x1,vy_x1,h_corr_x1b,vx,vy);
                        LSMVector3 hll_x2 = z_x2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x2,vx,vy,h_corr_x2b,vx_x2,vy_x2);
                        LSMVector3 hll_y1 = z_y1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y1,vy_y1,vx_y1,h_corr_y1b,vy,vx);
                        LSMVector3 hll_y2 = z_y2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y2,vy,vx,h_corr_y2b,vy_y2,vx_y2);

                        float C = 0.1f;

                        float flux_x1 = z_x1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_x1.x)),h_x1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                        float flux_x2 = z_x2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_x2.x)),h_x2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                        float flux_y1 = z_y1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_y1.x)),h_y1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                        float flux_y2 = z_y2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_y2.x)),h_y2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))

                        int edges = ((z_x1 < -99995)?1:0) +((z_x2 < -99995)?1:0)+((z_y1 < -99995)?1:0)+((z_y2 < -99995)?1:0);

                        bool edge = (z_x1 < -99995 || z_x2 < -99995 || z_y1 < -99995 || z_y2 < -99995);
                        float hold = h;
                        float hn = ((std::max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));


                        float f_centre_x = 0.5 * GRAV*((z<zc_x2?std::min(zc_x2,z + h) : std::min(z,zc_x2 + h_x2)) - (z<zc_x1?std::min(zc_x1,z + h) : std::min(z,zc_x1 + h_x1)))*(h_corr_x1 + h_corr_x2);
                        float f_centre_y = 0.5 * GRAV*((z<zc_y2?std::min(zc_y2,z + h) : std::min(z,zc_y2 + h_y2)) - (z<zc_y1?std::min(zc_y1,z + h) : std::min(z,zc_y1 + h_y1)))*(h_corr_y1 + h_corr_y2);


                        if(h > 0.01f )
                        {
                            //std::cout << "h: " << h << " - " << sx_zh << " " << sy_zh << " " << h_x1 << " " << h_x2 << " " << h_corr_x1 << " " << h_corr_x2 << " " << h_corr_x1b << " " << h_corr_x2b << " " << hll_x1.y << "  " << hll_x2.y  <<  " " << zc_x1 << " " << zc_x2 << std::endl;
                            //std::cout << "f: " << tx*(hll_x2.y - hll_x1.y + f_centre_x) << " " << hll_x2.y - hll_x1.y << " " <<  f_centre_x << " " << hll_y2.z - hll_y1.z << std::endl;
                        }

                        float qxn = h * vx - tx*(hll_x2.y - hll_x1.y + f_centre_x) - tx*(hll_y2.z - hll_y1.z);//- 0.5 * GRAV *hn*sx_zh * dt;
                        float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y + f_centre_y);//- 0.5 * GRAV *hn*sy_zh * dt;

                        float vsq = sqrt((float)(vx * vx + vy * vy));
                        float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
                        float nsq = nsq1*vsq*dt;

                        float vxn = (float)((qxn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));
                        float vyn = (float)((qyn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));

                        float threshold = std::min(0.1f,0.01f * dx);
                        if(hn < threshold)
                        {
                            float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                            float acc_eff = (vxn -vx)/std::max(0.0001,dt);

                            float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

                            vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

                        }

                        if(hn < threshold)
                        {
                            float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                            float acc_eff = (vyn -vy)/std::max(0.0001,dt);

                            float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

                            vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

                        }

                        hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
                        vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
                        vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;


                        //write output
                        HN->data[gy][gx] = hn;
                        VXN->data[gy][gx] = vxn;
                        VYN->data[gy][gx] = vyn;

                        if(QX1 != nullptr && QX2 != nullptr && QY1 != nullptr && QY2 != nullptr)
                        {

                            QX1->data[gy][gx] = flux_x1 *(dx*dx);
                            QX2->data[gy][gx] = flux_x2 *(dx*dx);
                            QY1->data[gy][gx] = flux_y1 *(dx*dx);
                            QY2->data[gy][gx] = flux_y2 *(dx*dx);
                        }
                    }
                }
            }

            //wait
            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->nrRows(); r++)
            {
                for(int c = 0; c < H->nrCols(); c++)
                {
                    H->Drc = HN->Drc;
                    VX->Drc = VXN->Drc;
                    VY->Drc = VYN->Drc;


                }
            }

            //wait
            #pragma omp barrier

            t = t+dt;
            iter ++;


        }

    }
    return;
}






static inline void flow_saintvenant_rigid(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HN,cTMap * VXN,cTMap * VYN, cTMap * BLOCKX, cTMap * BLOCKY, cTMap * BLOCKFX, cTMap * BLOCKFY, cTMap * BLOCKVELX, cTMap * BLOCKVELY, cTMap * HCorrect,cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr, float dt = 0.1)
{

    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();

    float tx = dt/dx;



    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                int x = r;
                int y = c;
                const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                const int gx_x11 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 2 )));
                const int gy_x1 = gy;
                const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                const int gx_x22 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 2)));
                const int gy_x2 = gy;
                const int gx_y1 = gx;
                const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                const int gy_y11 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 2)));
                const int gx_y2 = gx;
                const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));
                const int gy_y22 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 2)));


                float blockx = BLOCKX->data[gy][gx];
                float blocky = BLOCKY->data[gy][gx];

                float blockfx = BLOCKFX->data[gy][gx];
                float blockfy = BLOCKFY->data[gy][gx];

                float blockvelx = BLOCKVELX->data[gy][gx];
                float blockvely = BLOCKVELY->data[gy][gx];

                float blockx_x1 = BLOCKX->data[gy][gx_x1];
                float blockx_x2 = BLOCKX->data[gy][gx_x2];
                float blocky_y1 = BLOCKY->data[gy_y1][gx];;
                float blocky_y2 = BLOCKY->data[gy_y2][gx];

                float blockvelx_x1 = BLOCKVELX->data[gy][gx_x1];
                float blockvelx_x2 = BLOCKVELX->data[gy][gx_x2];
                float blockvely_y1 = BLOCKVELY->data[gy_y1][gx];
                float blockvely_y2 = BLOCKVELY->data[gy_y1][gx];

                float blockvely_x1 = BLOCKVELY->data[gy][gx_x1];
                float blockvely_x2 = BLOCKVELY->data[gy][gx_x2];
                float blockvelx_y1 = BLOCKVELX->data[gy_y1][gx];
                float blockvelx_y2 = BLOCKVELX->data[gy_y2][gx];

                float capture_x = std::max(0.0f,std::min(1.0f,blockx));//max(0.0f,min(1.0f,blockx - (1.0f-blockx)));
                float capture_y = std::max(0.0f,std::min(1.0f,blocky));//max(0.0f,min(1.0f,blocky - (1.0f-blocky)));

                float capture_x1 = std::max(0.0f,std::min(1.0f,blockx_x1));//max(0.0f,min(1.0f,blockx_x1 - (1.0f-blockx_x1)));
                float capture_y1 = std::max(0.0f,std::min(1.0f,blocky_y1));//max(0.0f,min(1.0f,blocky_y1 - (1.0f-blocky_y1)));
                float capture_x2 = std::max(0.0f,std::min(1.0f,blockx_x2));//max(0.0f,min(1.0f,blockx_x2 - (1.0f-blockx_x2)));
                float capture_y2 = std::max(0.0f,std::min(1.0f,blocky_y2));//max(0.0f,min(1.0f,blocky_y2 - (1.0f-blocky_y2)));
                float zhc = HCorrect->data[gy][gx];


                float zhc_x1 = HCorrect->data[gy][gx_x1];
                float zhc_x2 = HCorrect->data[gy][gx_x2];
                float zhc_y1 = HCorrect->data[gy_y1][gx];
                float zhc_y2 = HCorrect->data[gy_y2][gx];

                float z = DEM->data[gy][gx];
                float n = std::max(0.00001f,N->data[gy][gx]);

                z = z + zhc;

                float z_x1 = zhc_x1 + DEM->data[gy][gx_x1];
                float z_x2 = zhc_x2 + DEM->data[gy][gx_x2];
                float z_y1 = zhc_y1 + DEM->data[gy_y1][gx];
                float z_y2 = zhc_y2 + DEM->data[gy_y2][gx];


                if(gx + 1 > dim0-1)
                {
                    z_x2 = -99999;
                }
                if(gx - 1 < 0)
                {
                    z_x1 = -99999;
                }
                if(gy + 1 > dim1-1)
                {
                    z_y2 = -99999;
                }
                if(gy - 1 < 0)
                {
                    z_y1 = -99999;
                }
                float zc_x1 = z_x1 < -99995? z : z_x1;
                float zc_x2 = z_x2 < -99995? z : z_x2;
                float zc_y1 = z_y1 < -99995? z : z_y1;
                float zc_y2 = z_y2 < -99995? z : z_y2;

                float zcc_x1 =std::max(z,zc_x1);
                float zcc_x2 =std::max(z,zc_x2);
                float zcc_y1 =std::max(z,zc_y1);
                float zcc_y2 =std::max(z,zc_y2);

                float h = std::max(0.0f,H->data[gy][gx]);
                float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                float h_x11 = std::max(0.0f,H->data[gy][gx_x11]);
                float h_x22 = std::max(0.0f,H->data[gy][gx_x22]);
                float h_y11 = std::max(0.0f,H->data[gy_y11][gx]);
                float h_y22 = std::max(0.0f,H->data[gy_y22][gx]);

                float h_corr_x1 =  std::max(0.0f,(h_x1 - std::max(0.0f,z - zc_x1)));
                float h_corr_x1b =  std::max(0.0f,(h - std::max(0.0f,zc_x1-z)));
                float h_corr_x2 =  std::max(0.0f,(h - std::max(0.0f,zc_x2 - z)));
                float h_corr_x2b =  std::max(0.0f,(h_x2 - std::max(0.0f,z - zc_x2)));
                float h_corr_y1 =  std::max(0.0f,(h_y1 - std::max(0.0f,z - zc_y1)));
                float h_corr_y1b =  std::max(0.0f,(h - std::max(0.0f,zc_y1 - z)));
                float h_corr_y2 =  std::max(0.0f,(h - std::max(0.0f,zc_y2 - z)));
                float h_corr_y2b =  std::max(0.0f,(h_y2 - std::max(0.0f,z - zc_y2)));

                float vmax = 0.1 * dx/dt;

                float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
                float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

                float vx_x1 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x1]));
                float vy_x1 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x1]));
                float vx_x2 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x2]));
                float vy_x2 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x2]));
                float vx_y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx]));
                float vy_y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx]));
                float vx_y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx]));
                float vy_y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx]));

                float zh = z + h;
                float zh_x1 =std::min(1.0f,h_x1/std::max(0.0001f,h)) * (zc_x1 + h_x1) + (1.0f-std::min(1.0f,h_x1/std::max(0.0001f,h)))*std::min(zh,zc_x1 + h_x1);
                float zh_x2 =std::min(1.0f,h_x2/std::max(0.0001f,h)) * (zc_x2 + h_x2) + (1.0f-std::min(1.0f,h_x2/std::max(0.0001f,h)))*std::min(zh,zc_x2 + h_x2);
                float zh_y1 =std::min(1.0f,h_y1/std::max(0.0001f,h)) * (zc_y1 + h_y1) + (1.0f-std::min(1.0f,h_y1/std::max(0.0001f,h)))*std::min(zh,zc_y1 + h_y1);
                float zh_y2 =std::min(1.0f,h_y2/std::max(0.0001f,h)) * (zc_y2 + h_y2) + (1.0f-std::min(1.0f,h_y2/std::max(0.0001f,h)))*std::min(zh,zc_y2 + h_y2);

                float sx_zh_x2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_x2-zh)/dx)));
                float sy_zh_y1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_y1)/dx)));
                float sx_zh_x1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_x1)/dx)));
                float sy_zh_y2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_y2-zh)/dx)));

                float sx_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sx_zh_x1 + sx_zh_x2)));
                float sy_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sy_zh_y1 + sy_zh_y2)));


                float vx_adaptl = blockvelx < 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_x * std::fabs(blockvelx) /dx))*vx + std::min(0.5f,dt * capture_x *std::fabs(blockvelx) /dx) * blockvelx : vx;
                float vy_adaptl = blockvely < 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_y *std::fabs(blockvely) /dx))*vy + std::min(0.5f,dt * capture_y *std::fabs(blockvely) /dx) * blockvely : vy;
                float vx_adaptr = blockvelx > 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_x * std::fabs(blockvelx) /dx))*vx + std::min(0.5f,dt * capture_x *std::fabs(blockvelx) /dx) * blockvelx : vx;
                float vy_adaptr = blockvely > 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_y *std::fabs(blockvely) /dx))*vy + std::min(0.5f,dt * capture_y *std::fabs(blockvely) /dx) * blockvely : vy;
                float vx_adapt_x1 = blockvelx_x1 > 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_x1 * std::fabs(blockvelx_x1) /dx))*vx_x1 + std::min(0.5f,dt * capture_x1 *std::fabs(blockvelx_x1) /dx) * blockvelx_x1 : vx_x1;
                float vy_adapt_y1 = blockvely_y1 > 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_y1 *std::fabs(blockvely_y1) /dx))*vy_y1 + std::min(0.5f,dt * capture_y1 *std::fabs(blockvely_y1) /dx) * blockvely_y1 : vy_y1;
                float vx_adapt_x2 = blockvelx_x2 < 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_x2 * std::fabs(blockvelx_x2) /dx))*vx_x2 + std::min(0.5f,dt * capture_x2 *std::fabs(blockvelx_x2) /dx) * blockvelx_x2 : vx_x2;
                float vy_adapt_y2 = blockvely_y2 < 0.0? std::max(0.5f,1.0f-std::min(0.5f,dt * capture_y2 *std::fabs(blockvely_y2) /dx))*vy_y2 + std::min(0.5f,dt * capture_y2 *std::fabs(blockvely_y2) /dx) * blockvely_y2 : vy_y2;

                /*float h_corr_x1 = std::max(0.0f,(h_x1 -std::max(0.0f,z - z_x1)));
                float h_corr_x1b = std::max(0.0f,(h -std::max(0.0f,z_x1-z)));
                float h_corr_x2 = std::max(0.0f,(h -std::max(0.0f,z_x2 - z)));
                float h_corr_x2b = std::max(0.0f,(h_x2 -std::max(0.0f,z - z_x2)));
                float h_corr_y1 = std::max(0.0f,(h_y1 -std::max(0.0f,z - z_y1)));
                float h_corr_y1b = std::max(0.0f,(h -std::max(0.0f,z_y1 - z)));
                float h_corr_y2 = std::max(0.0f,(h -std::max(0.0f,z_y2 - z)));
                float h_corr_y2b = std::max(0.0f,(h_y2 -std::max(0.0f,z - z_y2)));
                */

                float factor_flowx1b = h_x1 < h_x2 ? 1.0-blockx : 1.0;
                float factor_flowy1b = h_y1 < h_y2 ? 1.0-blocky : 1.0;
                float factor_flowx2b = h_x1 < h_x2 ? 1.0 :  1.0-blockx;
                float factor_flowy2b = h_y1 < h_y2 ? 1.0 : 1.0-blocky;

                float factor_flowx1bt = h_x11 < h ? 1.0 : 1.0-blockx_x1;
                float factor_flowy1bt = h_y11 < h ? 1.0 : 1.0-blocky_y1;
                float factor_flowx2bt = h < h_x22 ? 1.0-blockx_x2 : 1.0;
                float factor_flowy2bt = h < h_y22 ? 1.0-blocky_y2 : 1.0;

                float factor_flowx1f = 1.0-std::min(1.0f,std::max(0.0f,z-z_x1)/std::max(1e-6f,h_x1));
                float factor_flowy1f = 1.0-std::min(1.0f,std::max(0.0f,z-z_y1)/std::max(1e-6f,h_y1));
                float factor_flowx2f = 1.0-std::min(1.0f,std::max(0.0f,z-z_x2)/std::max(1e-6f,h_x2));
                float factor_flowy2f = 1.0-std::min(1.0f,std::max(0.0f,z-z_y2)/std::max(1e-6f,h_y2));

                float factor_flowx1t = 1.0-std::min(1.0f,std::max(0.0f,z_x1-z)/std::max(1e-6f,h));
                float factor_flowy1t = 1.0-std::min(1.0f,std::max(0.0f,z_y1-z)/std::max(1e-6f,h));
                float factor_flowx2t = 1.0-std::min(1.0f,std::max(0.0f,z_x2-z)/std::max(1e-6f,h));
                float factor_flowy2t = 1.0-std::min(1.0f,std::max(0.0f,z_y2-z)/std::max(1e-6f,h));


                float fb_x1 = std::max(0.0f,std::min(factor_flowx1bt,factor_flowx1b));
                float fb_x2 = std::max(0.0f,std::min(factor_flowx2bt,factor_flowx2b));
                float fb_y1 = std::max(0.0f,std::min(factor_flowy1bt,factor_flowy1b));
                float fb_y2 = std::max(0.0f,std::min(factor_flowy2bt,factor_flowy2b));

                LSMVector3 hll_x1 = z_x1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x1,vx_adapt_x1,vy_x1,h_corr_x1b,vx_adaptl,vy);
                LSMVector3 hll_x2 = z_x2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x2,vx_adaptr,vy,h_corr_x2b,vx_adapt_x2,vy_x2);
                LSMVector3 hll_y1 = z_y1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y1,vy_adapt_y1,vx_y1,h_corr_y1b,vy_adaptl,vx);
                LSMVector3 hll_y2 = z_y2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y2,vy_adaptr,vx,h_corr_y2b,vy_adapt_y2,vx_y2);

                //float3 hll_x1 = z_x1 < -99995? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_x1,vx_adapt_x1,vy_x1,fb_x1, h_corr_x1b,vx_adaptl,vy, fb_x1);
                //float3 hll_x2 = z_x2 < -99995? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_x2,vx_adaptr,vy,fb_x2,h_corr_x2b,vx_adapt_x2,vy_x2,fb_x2);
                //float3 hll_y1 = z_y1 < -99995? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_y1,vy_adapt_y1,vx_y1,fb_y1,h_corr_y1b,vy_adaptl,vx,fb_y1);
                //float3 hll_y2 = z_y2 < -99995? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_y2,vy_adaptr,vx,fb_y2,h_corr_y2b,vy_adapt_y2,vx_y2,fb_y2);


                float C = 0.1f;
                float fluxo_x1 = +tx*(hll_x1.x) + (blockvelx < 0? h * dt * capture_x * blockvelx/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvelx_x1/dx : 0.0);
                float fluxo_x2 = -tx*(hll_x2.x) + (blockvelx > 0? -h * dt *capture_x * blockvelx/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvelx_x2/dx : 0.0);
                float fluxo_y1 = +tx*(hll_y1.x) + (blockvely < 0? h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvely_y1/dx : 0.0);
                float fluxo_y2 = -tx*(hll_y2.x) + (blockvely > 0? -h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvely_y2/dx : 0.0);

                float flux_x1 = z_x1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h  * factor_flowx1t * C,std::min((float)(fluxo_x1),h_x1 *factor_flowx1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_x2 = z_x2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h  * factor_flowx2t * C,std::min((float)(fluxo_x2),h_x2 *factor_flowx2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y1 = z_y1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h  * factor_flowy1t * C,std::min((float)(fluxo_y1),h_y1 *factor_flowy1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y2 = z_y2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h  * factor_flowy2t * C,std::min((float)(fluxo_y2),h_y2 *factor_flowy2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))

                float fluxor_x1 = 1.0;//min(1.0f,max(-1.0f,flux_x1/maxmod(1e-4f,fluxo_x1)));
                float fluxor_x2 = 1.0;// min(1.0f,max(-1.0f,flux_x2/maxmod(1e-4f,fluxo_x2)));
                float fluxor_y1 = 1.0;// min(1.0f,max(-1.0f,flux_y1/maxmod(1e-4f,fluxo_y1)));
                float fluxor_y2 = 1.0;// min(1.0f,max(-1.0f,flux_y2/maxmod(1e-4f,fluxo_y2)));

                int edges = ((z_x1 < -99995)?1:0) +((z_x2 < -99995)?1:0)+((z_y1 < -99995)?1:0)+((z_y2 < -99995)?1:0);

                bool edge = (z_x1 < -99995 || z_x2 < -99995 || z_y1 < -99995 || z_y2 < -99995);
                float hold = h;
                float hn = ((std::max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));


                float f_centre_x = 0.5 * GRAV*((z<zc_x2?std::min(zc_x2,z + h) : std::min(z,zc_x2 + h_x2)) - (z<zc_x1?std::min(zc_x1,z + h) : std::min(z,zc_x1 + h_x1)))*(h_corr_x1 + h_corr_x2);
                float f_centre_y = 0.5 * GRAV*((z<zc_y2?std::min(zc_y2,z + h) : std::min(z,zc_y2 + h_y2)) - (z<zc_y1?std::min(zc_y1,z + h) : std::min(z,zc_y1 + h_y1)))*(h_corr_y1 + h_corr_y2);



                float fluxmc_x1x = (blockvelx < 0? h * dt *capture_x * blockvelx* blockvelx/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvelx_x1* blockvelx_x1/dx : 0.0);
                float fluxmc_x2x = (blockvelx > 0? -h * dt *capture_x * blockvelx* blockvelx/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvelx_x2 *blockvelx_x2/dx : 0.0);
                float fluxmc_y1x = (blockvely < 0? h * dt *capture_y * blockvely* blockvelx/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvelx_y1* blockvely_y1/dx : 0.0);
                float fluxmc_y2x = (blockvely > 0? -h * dt *capture_y * blockvely* blockvelx/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvelx_y2* blockvely_y2/dx : 0.0);

                float fluxmc_x1y = (blockvelx < 0? h * dt *capture_x * blockvelx * blockvely/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvely_x1* blockvely_y1/dx : 0.0);
                float fluxmc_x2y = (blockvelx > 0? -h * dt *capture_x * blockvelx * blockvely/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvely_x2* blockvely_y2/dx : 0.0);
                float fluxmc_y1y = (blockvely < 0? h * dt *capture_y * blockvely* blockvely/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvely_y1* blockvely_y1/dx : 0.0);
                float fluxmc_y2y = (blockvely > 0? -h * dt *capture_y * blockvely* blockvely/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvely_y2* blockvely_y2/dx : 0.0);

                float qxn = h * vx - tx*(fluxor_x2 *hll_x2.y - fluxor_x1 *hll_x1.y + f_centre_x)  - tx*(fluxor_y2*hll_y2.z - fluxor_y1*hll_y1.z);// + fluxmc_x1x + fluxmc_x2x + fluxmc_y1x + fluxmc_y2x ;//- 0.5 * GRAV *hn*sx_z * dt
                float qyn = h * vy - tx*(fluxor_x2 *hll_x2.z - fluxor_x1 *hll_x1.z) - tx*(fluxor_y2 *hll_y2.y - fluxor_y1 *hll_y1.y + f_centre_y);// + fluxmc_x1y + fluxmc_x2y + fluxmc_y1y + fluxmc_y2y ;//- 0.5 * GRAV *hn*sy_z * dt

                float vsq = sqrt((float)(vx * vx + vy * vy));
                float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
                float nsq = nsq1*vsq*dt;

                float vxn = std::max(0.5,(1.0 -  dt * capture_x * std::fabs(blockvelx)/dx)) *(float)((qxn/(1.0f+nsq)))/std::max(0.01f,(float)(hn)) + std::min(0.5,1.0 * dt * capture_x *blockvelx* std::fabs(blockvelx)/dx) + blockfx * dt/(std::max(0.01f * dx,hn)*dx*dx * 1000.0);
                float vyn = std::max(0.5,(1.0 -  dt * capture_y * std::fabs(blockvely)/dx)) *(float)((qyn/(1.0f+nsq)))/std::max(0.01f,(float)(hn)) + std::min(0.5,1.0 * dt * capture_y *blockvely* std::fabs(blockvely)/dx) + blockfy * dt/(std::max(0.01f * dx,hn)*dx*dx * 1000.0);

                float threshold = std::min(0.1f,0.01f * dx);
                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vxn -vx)/std::max(0.0001f,dt);

                    float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

                    vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

                }

                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vyn -vy)/std::max(0.0001f,dt);

                    float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

                    vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

                }

                hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
                vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
                vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;


                //write output
                HN->data[gy][gx] = hn;
                VXN->data[gy][gx] = vxn;
                VYN->data[gy][gx] = vyn;

                if(QX1 != nullptr && QX2 != nullptr && QY1 != nullptr && QY2 != nullptr)
                {

                    QX1->data[gy][gx] =  flux_x1 *(dx*dx);
                    QX2->data[gy][gx] =  flux_x2 *(dx*dx);
                    QY1->data[gy][gx] =  flux_y1 *(dx*dx);
                    QY2->data[gy][gx] =  flux_y2 *(dx*dx);
                }
            }
        }
    }
    return;
}



static inline void flow_boussinesq(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HN,cTMap * VXN,cTMap * VYN,cTMap * HO, cTMap * VXO,cTMap * VYO, cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr, float dt = 0.1)
{

    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();

    float tx = dt/dx;
    /*float dt_min = 1e6;

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {

            int x = r;
            int y = c;

            const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
            const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

            float vmax = 0.1 * dx/dt;

            float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
            float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

            float dt_req = 0.25f *dx/( std::min(100.0f,std::max(0.01f,(sqrt(vx*vx + vy * vy)))));
            dt_min = std::min(dt_min,dt_req);

        }
    }*/



    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                int x = r;
                int y = c;
                const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                const int gy_x1 = gy;
                const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                const int gy_x2 = gy;
                const int gx_y1 = gx;
                const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                const int gx_y2 = gx;
                const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));

                float z = DEM->data[gy][gx];
                float n = std::max(0.00001f,N->data[gy][gx]);

                float z_x1 = DEM->data[gy][gx_x1];
                float z_x2 = DEM->data[gy][gx_x2];
                float z_y1 = DEM->data[gy_y1][gx];
                float z_y2 = DEM->data[gy_y2][gx];


                if(gx + 1 > dim0-1)
                {
                    z_x2 = -99999;
                }
                if(gx - 1 < 0)
                {
                    z_x1 = -99999;
                }
                if(gy + 1 > dim1-1)
                {
                    z_y2 = -99999;
                }
                if(gy - 1 < 0)
                {
                    z_y1 = -99999;
                }
                float zc_x1 = z_x1 < -99995? z : z_x1;
                float zc_x2 = z_x2 < -99995? z : z_x2;
                float zc_y1 = z_y1 < -99995? z : z_y1;
                float zc_y2 = z_y2 < -99995? z : z_y2;

                float h = std::max(0.0f,H->data[gy][gx]);
                float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                float h_corr_x1 =  std::max(0.0f,(h_x1 - std::max(0.0f,z - zc_x1)));
                float h_corr_x1b =  std::max(0.0f,(h - std::max(0.0f,zc_x1-z)));
                float h_corr_x2 =  std::max(0.0f,(h - std::max(0.0f,zc_x2 - z)));
                float h_corr_x2b =  std::max(0.0f,(h_x2 - std::max(0.0f,z - zc_x2)));
                float h_corr_y1 =  std::max(0.0f,(h_y1 - std::max(0.0f,z - zc_y1)));
                float h_corr_y1b =  std::max(0.0f,(h - std::max(0.0f,zc_y1 - z)));
                float h_corr_y2 =  std::max(0.0f,(h - std::max(0.0f,zc_y2 - z)));
                float h_corr_y2b =  std::max(0.0f,(h_y2 - std::max(0.0f,z - zc_y2)));

                float vmax = 0.1 * dx/dt;

                float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
                float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));

                float vx_x1 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x1]));
                float vy_x1 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x1]));
                float vx_x2 = std::min(vmax,std::max(-vmax,VX->data[gy][gx_x2]));
                float vy_x2 = std::min(vmax,std::max(-vmax,VY->data[gy][gx_x2]));
                float vx_y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx]));
                float vy_y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx]));
                float vx_y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx]));
                float vy_y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx]));


                float zh = z + h;
                float zh_x1 =std::min(1.0f,h_x1/std::max(0.0001f,h)) * (zc_x1 + h_x1) + (1.0f-std::min(1.0f,h_x1/std::max(0.0001f,h)))*std::min(zh,zc_x1 + h_x1);
                float zh_x2 =std::min(1.0f,h_x2/std::max(0.0001f,h)) * (zc_x2 + h_x2) + (1.0f-std::min(1.0f,h_x2/std::max(0.0001f,h)))*std::min(zh,zc_x2 + h_x2);
                float zh_y1 =std::min(1.0f,h_y1/std::max(0.0001f,h)) * (zc_y1 + h_y1) + (1.0f-std::min(1.0f,h_y1/std::max(0.0001f,h)))*std::min(zh,zc_y1 + h_y1);
                float zh_y2 =std::min(1.0f,h_y2/std::max(0.0001f,h)) * (zc_y2 + h_y2) + (1.0f-std::min(1.0f,h_y2/std::max(0.0001f,h)))*std::min(zh,zc_y2 + h_y2);

                float sx_zh_x2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_x2-zh)/dx)));
                float sy_zh_y1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_y1)/dx)));
                float sx_zh_x1 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh-zh_x1)/dx)));
                float sy_zh_y2 = std::min((float)(0.5),std::max((float)(-0.5),(float)((zh_y2-zh)/dx)));

                float sx_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sx_zh_x1 + sx_zh_x2)));
                float sy_zh = std::min(1.0f,std::max(-1.0f,0.5f * (sy_zh_y1 + sy_zh_y2)));


                LSMVector3 hll_x1 = z_x1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x1,vx_x1,vy_x1,h_corr_x1b,vx,vy);
                LSMVector3 hll_x2 = z_x2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_x2,vx,vy,h_corr_x2b,vx_x2,vy_x2);
                LSMVector3 hll_y1 = z_y1 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y1,vy_y1,vx_y1,h_corr_y1b,vy,vx);
                LSMVector3 hll_y2 = z_y2 < -99995? LSMVector3(0.0,0.0,0.0):F_HLL2FL(h_corr_y2,vy,vx,h_corr_y2b,vy_y2,vx_y2);

                float C = 0.1f;

                float flux_x1 = z_x1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_x1.x)),h_x1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_x2 = z_x2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_x2.x)),h_x2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y1 = z_y1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(+tx*(hll_y1.x)),h_y1 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
                float flux_y2 = z_y2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):std::max(-h * C,std::min((float)(-tx*(hll_y2.x)),h_y2 * C));//std::max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))

                int edges = ((z_x1 < -99995)?1:0) +((z_x2 < -99995)?1:0)+((z_y1 < -99995)?1:0)+((z_y2 < -99995)?1:0);

                bool edge = (z_x1 < -99995 || z_x2 < -99995 || z_y1 < -99995 || z_y2 < -99995);
                float hold = h;

                float a_disp_x = (1.0/6.0)*h*h*h*UF2D_Derivative3(VX,gy,gx,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx);
                float a_disp_y = (1.0/6.0)*h*h*h*UF2D_Derivative3(VY,gy,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx);
                float a_disp_x1 = (1.0/6.0)*h*h*h*UF2D_Derivative3(VX,gy,gx_x1,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx);
                float a_disp_y1 = (1.0/6.0)*h*h*h*UF2D_Derivative3(VY,gy_y1,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx);
                float a_disp_x2 = (1.0/6.0)*h*h*h*UF2D_Derivative3(VX,gy,gx_x2,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx);
                float a_disp_y2 = (1.0/6.0)*h*h*h*UF2D_Derivative3(VY,gy_y2,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx);


                float a_vdisp_x = (1.0/dt)*((1.0/6.0)*h*h*UF2D_Derivative2(VX,gy,gx,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx) - (1.0/6.0)*h*h*UF2D_Derivative2(VXO,gy,gx,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx));
                float a_vdisp_y = (1.0/dt)*((1.0/6.0)*h*h*UF2D_Derivative2(VY,gy,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx) - (1.0/6.0)*h*h*UF2D_Derivative2(VYO,gy,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx));
                float a_vdisp_x1 = (1.0/dt)*((1.0/6.0)*h*h*UF2D_Derivative2(VY,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx) - (1.0/6.0)*h*h*UF2D_Derivative2(VYO,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx));
                float a_vdisp_y1 = (1.0/dt)*((1.0/6.0)*h*h*UF2D_Derivative2(VX,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx) - (1.0/6.0)*h*h*UF2D_Derivative2(VXO,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx));

                float a_vdisp2_x =vx* (1.0/dt)*((1.0/2.0)*h*UF2D_Derivative2(H,gy,gx,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx) - (1.0/2.0)*h*UF2D_Derivative2(HO,gy,gx,UF_DIRECTION_X,UF_DERIVATIVE_LR,dx));
                float a_vdisp2_y =vy* (1.0/dt)*((1.0/2.0)*h*UF2D_Derivative2(H,gy,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx) - (1.0/2.0)*h*UF2D_Derivative2(HO,gy,gx,UF_DIRECTION_Y,UF_DERIVATIVE_LR,dx));
                float a_vdisp2_x2 =vy* (1.0/dt)*((1.0/2.0)*h*UF2D_Derivative2(H,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx) - (1.0/2.0)*h*UF2D_Derivative2(HO,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx));
                float a_vdisp2_y2 =vx* (1.0/dt)*((1.0/2.0)*h*UF2D_Derivative2(H,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx) - (1.0/2.0)*h*UF2D_Derivative2(HO,gy,gx,UF_DIRECTION_XY,UF_DERIVATIVE_LR,dx));

                float hn = ((std::max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));

                float f_centre_x = 0.5 * GRAV*((z<zc_x2?std::min(zc_x2,z + h) : std::min(z,zc_x2 + h_x2)) - (z<zc_x1?std::min(zc_x1,z + h) : std::min(z,zc_x1 + h_x1)))*(h_corr_x1 + h_corr_x2);
                float f_centre_y = 0.5 * GRAV*((z<zc_y2?std::min(zc_y2,z + h) : std::min(z,zc_y2 + h_y2)) - (z<zc_y1?std::min(zc_y1,z + h) : std::min(z,zc_y1 + h_y1)))*(h_corr_y1 + h_corr_y2);


                float qxn = h * vx - tx*(hll_x2.y - hll_x1.y + f_centre_x) - tx*(hll_y2.z - hll_y1.z) - h*dt*( -a_vdisp_x -a_vdisp_x1 + a_vdisp2_x + a_vdisp2_x2);//- 0.5 * GRAV *hn*sx_zh * dt;
                float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y + f_centre_y) - h*dt*( -a_vdisp_y -a_vdisp_y1 + a_vdisp2_y + a_vdisp2_y2);//- 0.5 * GRAV *hn*sy_zh * dt;

                float vsq = sqrt((float)(vx * vx + vy * vy));
                float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
                float nsq = nsq1*vsq*dt;

                float vxn = (float)((qxn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));
                float vyn = (float)((qyn/(1.0f+nsq)))/std::max(0.01f,(float)(hn));

                float threshold = std::min(0.1f,0.01f * dx);
                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vxn -vx)/std::max(0.0001f,dt);

                    float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

                    vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

                }

                if(hn < threshold)
                {
                    float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
                    float acc_eff = (vyn -vy)/std::max(0.0001f,dt);

                    float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * sqrt(std::max(0.0f,sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

                    vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

                }

                hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
                vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
                vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;


                //write output
                HN->data[gy][gx] = hn;
                VXN->data[gy][gx] = vxn;
                VYN->data[gy][gx] = vyn;

                if(QX1 != nullptr && QX2 != nullptr && QY1 != nullptr && QY2 != nullptr)
                {

                    QX1->data[gy][gx] =  flux_x1 *(dx*dx);
                    QX2->data[gy][gx] =  flux_x2 *(dx*dx);
                    QY1->data[gy][gx] =  flux_y1 *(dx*dx);
                    QY2->data[gy][gx] =  flux_y2 *(dx*dx);
                }
            }
        }
    }
    return;



}

inline std::vector<cTMap*> AS_DynamicWave(cTMap * DEM,cTMap * N,cTMap * HI, cTMap * VXI, cTMap * VYI, float _dt, float courant)
{
    if(!(DEM->data.nr_rows() == N->data.nr_rows() && DEM->data.nr_cols() == N->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == HI->data.nr_rows() && DEM->data.nr_cols() == HI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for H");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VXI->data.nr_rows() && DEM->data.nr_cols() == VXI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VYI->data.nr_rows() && DEM->data.nr_cols() == VYI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }


    cTMap * H = HI->GetCopy();
    cTMap * VX = VXI->GetCopy();
    cTMap * VY = VYI->GetCopy();

    cTMap * HN = H->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();

    flow_saintvenantdtandflow(_dt,DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr,courant);


    /*double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {
        //first update flow heights
        double dt = flow_saintvenantdt(DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr);

        dt = std::min(t_end - t,std::max(dt,1e-6));


        std::cout << "dt " << dt << std::endl;

        //then update fluxes
        flow_saintvenant(DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr,dt);

        t = t+dt;
        iter ++;

        for(int r = 0; r < H->nrRows(); r++)
        {
            for(int c = 0; c < H->nrCols(); c++)
            {
                H->Drc = HN->Drc;
                VX->Drc = VXN->Drc;
                VY->Drc = VYN->Drc;


            }
        }
    }

    std::cout << "iter " << iter << std::endl;*/


    delete H;
    delete VX;
    delete VY;

    return {HN,VXN,VYN};
}


inline cTMap * AS_DynamicWaveSuspendedTransport(cTMap * SI, cTMap * DEM,cTMap * N,cTMap * HI, cTMap * VXI, cTMap * VYI, float _dt, float courant)
{
    if(!(DEM->data.nr_rows() == N->data.nr_rows() && DEM->data.nr_cols() == N->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == HI->data.nr_rows() && DEM->data.nr_cols() == HI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for H");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VXI->data.nr_rows() && DEM->data.nr_cols() == VXI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VYI->data.nr_rows() && DEM->data.nr_cols() == VYI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == SI->data.nr_rows() && DEM->data.nr_cols() ==SI->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for S");
       throw -1;
    }


    cTMap * H = HI->GetCopy();
    cTMap * VX = VXI->GetCopy();
    cTMap * VY = VYI->GetCopy();
    cTMap * S = SI->GetCopy();

    cTMap * HN = H->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();

    cTMap * QX1 = H->GetCopy();
    cTMap * QX2 = H->GetCopy();
    cTMap * QY1 = H->GetCopy();
    cTMap * QY2 = H->GetCopy();

    cTMap * SN = S->GetCopy();
    double t = 0;
    double t_end = _dt;


    int dim0 = H->nrCols();
    int dim1 = H->nrRows();

    float dx = std::fabs(H->cellSizeX());

    float tx = _dt/dx;
    int iter = 0;
    while(t < t_end)
    {

        //first update flow heights
        double dt = flow_saintvenantdt(DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr);

        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        flow_saintvenant(DEM,N,H,VX,VY,HN,VXN,VYN,QX1,QX2,QY1,QY2,dt);

        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for (int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {


                    int x = r;
                    int y = c;
                    const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
                    const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

                    const int gx_x1 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx - 1 )));
                    const int gy_x1 = gy;
                    const int gx_x2 = std::min(dim0-(int)(1),std::max((int)(0),(int)(gx + 1)));
                    const int gy_x2 = gy;
                    const int gx_y1 = gx;
                    const int gy_y1 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy - 1)));
                    const int gx_y2 = gx;
                    const int gy_y2 = std::min(dim1-(int)(1),std::max((int)(0),(int)(gy + 1)));

                    float h = std::max(0.0f,H->data[gy][gx]);
                    float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                    float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                    float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                    float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                    float s = std::max(0.0f,S->data[gy][gx]);
                    float s_x1 = h_x1 > 0.0f? std::max(0.0f,S->data[gy][gx_x1]) : s;
                    float s_x2 = h_x2 > 0.0f? std::max(0.0f,S->data[gy][gx_x2]) : s;
                    float s_y1 = h_y1 > 0.0f? std::max(0.0f,S->data[gy_y1][gx]) : s;
                    float s_y2 = h_y2 > 0.0f? std::max(0.0f,S->data[gy_y2][gx]) : s;


                    float watervol = h * dx*dx;
                    float x1_watervol = h_x1 > 0.0f? h_x1 * dx*dx : watervol;
                    float x2_watervol = h_x2 > 0.0f? h_x2 * dx*dx : watervol;
                    float y1_watervol = h_y1 > 0.0f? h_y1 * dx*dx : watervol;
                    float y2_watervol = h_y2 > 0.0f? h_y2 * dx*dx : watervol;

                    float sq_x1 = QX1->data[r][c]/std::max(1e-10f,x1_watervol);
                    float sq_y1 = QY1->data[r][c]/std::max(1e-10f,y1_watervol);
                    float sq_x2 = QX2->data[r][c]/std::max(1e-10f,x2_watervol);
                    float sq_y2 = QY2->data[r][c]/std::max(1e-10f,y2_watervol);


                    sq_x1 = (sq_x1> 0.0f? 1.0:-1.0) * std::min(std::fabs(sq_x1) * s_x1,s_x1);
                    sq_x2 = (sq_x2> 0.0f? 1.0:-1.0) * std::min(std::fabs(sq_x2) * s_x2,s_x2);
                    sq_y1 = (sq_y1> 0.0f? 1.0:-1.0) * std::min(std::fabs(sq_y1) * s_y1,s_y1);
                    sq_y2 = (sq_y2> 0.0f? 1.0:-1.0) * std::min(std::fabs(sq_y2) * s_y2,s_y2);

                    float sn = std::max(0.0f,s + sq_x1 + sq_x2 + sq_y1 + sq_y2);
                    if(!std::isfinite((sn)))
                    {
                        sn = 0.0;
                    }
                    SN->data[r][c] = sn;

                }

            }
        }


        t = t+dt;
        iter ++;

        for(int r = 0; r < H->nrRows(); r++)
        {
            for(int c = 0; c < H->nrCols(); c++)
            {
                H->Drc = HN->Drc;
                VX->Drc = VXN->Drc;
                VY->Drc = VYN->Drc;

                S->Drc = SN->data[r][c];


            }
        }
    }



    delete HN;
    delete VXN;
    delete VYN;

    delete H;
    delete VX;
    delete VY;
    delete S;

    return {SN};
}


inline std::vector<cTMap*> AS_DynamicWaveRigid(cTMap * DEM,cTMap * N,cTMap * H, cTMap * VX, cTMap * VY, cTMap * BlockX, cTMap * BlockY, cTMap * BlockFX, cTMap * BlockFY, cTMap * BlockVelX, cTMap * BlockVelY, cTMap * HCorrect, float _dt, float courant)
{
    if(!(DEM->data.nr_rows() == N->data.nr_rows() && DEM->data.nr_cols() == N->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == H->data.nr_rows() && DEM->data.nr_cols() == H->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for H");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VX->data.nr_rows() && DEM->data.nr_cols() == VX->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VY->data.nr_rows() && DEM->data.nr_cols() == VY->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }

    cTMap * HN = H->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();

    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {

        //first update flow heights
        double dt = flow_saintvenantdt(DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr);

        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        flow_saintvenant_rigid(DEM,N,H,VX,VY,HN,VXN,VYN,BlockX, BlockY, BlockFX, BlockFY, BlockVelX, BlockVelY, HCorrect, nullptr,nullptr,nullptr,nullptr,dt);

        t = t+dt;
        iter ++;

        for(int r = 0; r < H->nrRows(); r++)
        {
            for(int c = 0; c < H->nrCols(); c++)
            {
                H->Drc = HN->Drc;
                VX->Drc = VXN->Drc;
                VY->Drc = VYN->Drc;


            }
        }
    }



    return {HN,VXN,VYN};
}


inline std::vector<cTMap*> AS_BoussinesqWave(cTMap * DEM,cTMap * N,cTMap * H, cTMap * VX, cTMap * VY, float _dt, float courant)
{
    if(!(DEM->data.nr_rows() == N->data.nr_rows() && DEM->data.nr_cols() == N->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == H->data.nr_rows() && DEM->data.nr_cols() == H->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for H");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VX->data.nr_rows() && DEM->data.nr_cols() == VX->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == VY->data.nr_rows() && DEM->data.nr_cols() == VY->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }

    cTMap * HO = H->GetCopy();
    cTMap * HN = H->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();
    cTMap * VXO = VX->GetCopy();
    cTMap * VYO = VY->GetCopy();

    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {

        //first update flow heights
        double dt = flow_saintvenantdt(DEM,N,H,VX,VY,HN,VXN,VYN,nullptr,nullptr,nullptr,nullptr);

        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        flow_boussinesq(DEM,N,H,VX,VY,HN,VXN,VYN,HO,VXO,VYO,nullptr,nullptr,nullptr,nullptr,dt);

        t = t+dt;
        iter ++;

        for(int r = 0; r < H->nrRows(); r++)
        {
            for(int c = 0; c < H->nrCols(); c++)
            {
                HO->Drc = H->Drc;
                H->Drc = HN->Drc;


                VXO->Drc = VX->Drc;
                VYO->Drc = VY->Drc;
                VX->Drc = VXN->Drc;
                VY->Drc = VYN->Drc;


            }
        }
    }



    return {HN,VXN,VYN};
}


//TODO later!!




inline cTMap * AS_SuspendedTransport(cTMap * H, cTMap * mass,cTMap * Qx,cTMap * Qy)
{


    return nullptr;
}

inline cTMap * AS_SteadyStateDecayWave(cTMap * DEM,cTMap * N, cTMap * Source,cTMap * Decayf,cTMap * Sink, int iter_max, float courant)
{



    return nullptr;
}

inline cTMap * AS_SteadyStateWave(cTMap * DEM,cTMap * N, cTMap * Source,cTMap * Sink, int iter_max, float courant)
{
    if(!(DEM->data.nr_rows() == N->data.nr_rows() && DEM->data.nr_cols() == N->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == Source->data.nr_rows() && DEM->data.nr_cols() == Source->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == Sink->data.nr_rows() && DEM->data.nr_cols() == Sink->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-10 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-10 and 1");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");


    double _dx = std::fabs(DEM->cellSizeX());

    //do the loop
    for(int i = 0; i < iter_max; i++)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demh =DEM->data[r][c]+H->data[r][c];
                    bool outlet = false;
                    if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                    {
                        outlet = 1.0;
                    }

                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                    dem = dem + pit;
                    demh = demh + pit;

                    double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+H->data[r][c-1] : dem;
                    double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+H->data[r][c+1] : dem;
                    double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+H->data[r-1][c] : dem;
                    double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+H->data[r+1][c] : dem;

                    double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                    double h = std::max(0.0f,H->data[r][c]);

                    double Shx1 = (demhx1 - (demh))/(_dx);
                    double Shx2 = ((demh)- (demhx2))/(_dx);
                    double Shy1 = (demhy1 - (demh))/(_dx);
                    double Shy2 = ((demh)- (demhy2))/(_dx);

                    double Sx1 = (demx1 - (dem))/(_dx);
                    double Sx2 = ((dem)- (demx2))/(_dx);
                    double Sy1 = (demy1 - (dem))/(_dx);
                    double Sy2 = ((dem)- (demy2))/(_dx);

                    if(demhx1 < demhx2)
                    {
                        SX->Drc = Shx1;
                    }else
                    {
                        SX->Drc = Shx2;
                    }

                    if(demhy1 < demhy2)
                    {
                        SY->Drc = Shy1;
                    }else
                    {
                        SY->Drc = Shy2;
                    }

                    double flowwidth = _dx;

                    S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                    double HL = h;//std::min(h,std::max(0.0,H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));

                    double q = 1.0 *flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0/N->Drc) *std::pow((HL),2.0/3.0);

                    q = std::min(q,h * _dx * _dx);
                    double V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001))*(1.0/N->Drc) *std::pow((HL*_dx)/(2.0 * HL + _dx),2.0/3.0) : 0.0;

                    double qx = (demhx1 < demhx2 > 0? -1.0:1.0) * (std::fabs(SX->Drc)/S->Drc) * q;
                    double qy = (demhy1 < demhy2 > 0? -1.0:1.0) * (std::fabs(SY->Drc)/S->Drc) * q;

                    QX1->Drc = qx < 0? qx :0.0;
                    QX2->Drc = qx > 0? qx :0.0;
                    QY1->Drc = qy < 0? qy :0.0;
                    QY2->Drc = qy > 0? qy :0.0;
                }

            }
        }

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double dem = DEM->Drc;

                            double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                            double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                            double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                            double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                            double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                            dem = dem + pit;

                            double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+H->data[r][c-1] : dem;
                            double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+H->data[r][c+1] : dem;
                            double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+H->data[r-1][c] : dem;
                            double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+H->data[r+1][c] : dem;

                            double demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+H->data[r-1][c-1] : dem;
                            double demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+H->data[r+1][c+1] : dem;
                            double demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+H->data[r-1][c+1] : dem;
                            double demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+H->data[r+1][c-1] : dem;

                            double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                            demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                            double qx1 = std::max(0.0,((!OUTORMV(DEM,r,c-1))? (std::fabs(QX2->data[r][c-1])) : 0.0));
                            double qx2 = std::max(0.0,((!OUTORMV(DEM,r,c+1))? (std::fabs(QX1->data[r][c+1])) : 0.0));
                            double qy1 = std::max(0.0,((!OUTORMV(DEM,r-1,c))? (std::fabs(QY2->data[r-1][c])) : 0.0));
                            double qy2 = std::max(0.0,((!OUTORMV(DEM,r+1,c))? (std::fabs(QY1->data[r+1][c])) : 0.0));

                            double qoutx1 = std::max(0.0f,(std::fabs(QX2->data[r][c])));
                            double qoutx2 = std::max(0.0f,(std::fabs(QX1->data[r][c])));
                            double qouty1 = std::max(0.0f,(std::fabs(QY2->data[r][c])));
                            double qouty2 = std::max(0.0f,(std::fabs(QY1->data[r][c])));

                            //total net incoming discharge
                            double QIN = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                            double Q = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2) + Source->data[r][c] - Sink->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                            //solve for height, so that new discharge is incoming discharge
                            double sol =std::pow( Q*  N->Drc /(sqrt(S->Drc + 0.001)*_dx),3.0/5.0);

                            double hnew = std::max(std::max(0.0,demhmin- dem),sol);
                            HN->data[r][c] = H->Drc * (1.0-courant) + (courant) *hnew;//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                }
            }
        }

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                H->data[r][c] = HN->data[r][c];
            }
        }
    }


    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;

    delete S;
    delete SX;
    delete SY;

    return H;
}


#include "navierstokes/StableFluid2d.h"

inline std::vector<cTMap*> AS_NavierStokesWave(cTMap * DENS, cTMap * VX, cTMap * VY, float visc, float diff, float _dt, float courant)
{

    if(!(DENS->data.nr_rows() == VX->data.nr_rows() && DENS->data.nr_cols() == VX->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(DENS->data.nr_rows() == VY->data.nr_rows() && DENS->data.nr_cols() == VY->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }

    cTMap * HN = DENS->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();

    StableFluid2d f(HN->nrCols(),HN->nrRows(),(float)(HN->nrCols()) * std::fabs(HN->cellSizeX()), (float)(HN->nrRows()) * std::fabs(HN->cellSizeY()));

    int * b = f.Block();
    float * u = f.U();
    float * v = f.V();
    float * d = f.Density();

    for(int r = 0; r < DENS->nrRows(); r++)
    {
        for(int c = 0;c < DENS->nrCols(); c++)
        {
            int index = f.IX(r,c);
            if(!pcr::isMV(DENS->data[r][c]))
            {
                u[index] = VX->data[r][c];
                v[index] = VY->data[r][c];
                d[index] = DENS->data[r][c];
                b[index] = 1;

            }else {

                b[index] = 0;
            }

        }

    }
    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {
        //first update flow heights
        double dt = t_end - t;

        for(int r = 0; r < DENS->nrRows(); r++)
        {
            for(int c = 0;c < DENS->nrCols(); c++)
            {
                int index = f.IX(r,c);
                if(!pcr::isMV(DENS->data[r][c]))
                {
                    double dt_this = 0.25* std::min( std::fabs(HN->cellSizeX()), std::fabs(HN->cellSizeY()))/std::max(0.0001f,std::max(std::fabs(u[index]),std::fabs(v[index])));

                    dt = std::min(dt_this,dt);
                }
            }
        }
        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        f.Update(visc,1.0,dt);

        t = t+dt;
        iter ++;

    }

    for(int r = 0; r < DENS->nrRows(); r++)
    {
        for(int c = 0;c < DENS->nrCols(); c++)
        {
            int index = f.IX(r,c);
            if(!pcr::isMV(DENS->data[r][c]))
            {

                HN->data[r][c] = d[index];
                VXN->data[r][c] = u[index];
                VYN->data[r][c] = v[index];

            }
        }
    }

    return {HN,VXN,VYN};
}


#include "navierstokes/StableFluid3d.h"
#include "geo/raster/field.h"

inline Field * AS_FieldMaskDem(Field * map,cTMap * DEM, float value)
{


    std::vector<cTMap*> maps = map->GetMapList();

    Field * res = map->GetCopy();

     for(int i = 0; i < maps.size(); i++)
     {
         if(!(maps.at(i)->data.nr_rows() == DEM->data.nr_rows() && maps.at(i)->data.nr_cols() == DEM->data.nr_cols()))
         {
            LISEMS_ERROR("Numbers of rows and column do not match for dem and input state");
            throw -1;
         }

     }

     float z_min = map->m_ZStart;
     float z_max = map->m_ZStart + map->m_dz * ((float)( map->nrLevels()));


     //estimate blocking based on DEM Geometry!
     for(int r = 0; r < DEM->nrRows(); r++)
     {
         for(int c = 0;c < DEM->nrCols(); c++)
         {
             if(!pcr::isMV(DEM->data[r][c]))
             {
                 for(int i = 0;i < maps.size(); i++)
                 {

                     float z = z_min + ((float)(i) * (z_max-z_min))/((float)( map->nrLevels()));
                     if(z < DEM->data[r][c])
                     {
                         res->at(i)->data[r][c] = value;
                     }
                 }
             }
         }
     }

     return res;
}



inline std::vector<cTMap *> AS_VoxelMaskDem(std::vector<cTMap *> maps, float z_min, float z_max,cTMap * DEM)
{


     std::vector<cTMap *> res;

     for(int i = 0; i < maps.size(); i++)
     {
         if(!(maps.at(i)->data.nr_rows() == DEM->data.nr_rows() && maps.at(i)->data.nr_cols() == DEM->data.nr_cols()))
         {
            LISEMS_ERROR("Numbers of rows and column do not match for dem and input state");
            throw -1;
         }
         res.push_back(maps.at(i)->GetCopy());

     }


     //estimate blocking based on DEM Geometry!
     for(int r = 0; r < DEM->nrRows(); r++)
     {
         for(int c = 0;c < DEM->nrCols(); c++)
         {
             if(!pcr::isMV(DEM->data[r][c]))
             {
                 for(int i = 0;i < res.size(); i++)
                 {

                     float z = z_min + ((float)(i) * (z_max-z_min));

                     if(z < DEM->data[r][c])
                     {
                         pcr::setMV(res.at(i)->data[r][c]);
                     }
                 }
             }
         }
     }



     return res;
}


inline std::vector<cTMap *> AS_NavierStokesWave3D( std::vector<cTMap *> maps, float z_min, float z_max, float visc, float diff, float _dt, float courant)
{

    if(maps.size() < 8)
    {
        LISEMS_ERROR("Unable to work with less than 2 vertical layers");
        throw -1;
    }

    if(!(maps.size() % 4 == 0))
    {
        LISEMS_ERROR("number of maps must be multiple of 4 (dens, vx,vy,vz, dens,vx,vy,vz,... from bottom to top)");
        throw -1;
    }

    std::vector<cTMap *> res;

    QList<cTMap*> DENS;
    QList<cTMap*> VX;
    QList<cTMap*> VY;
    QList<cTMap*> VZ;

    for(int i = 0; i < maps.size(); i++)
    {
        if(!(maps.at(i)->data.nr_rows() == maps.at(0)->data.nr_rows() && maps.at(i)->data.nr_cols() == maps.at(0)->data.nr_cols()))
        {
           LISEMS_ERROR("Numbers of rows and column do not match for dem and input state");
           throw -1;
        }

        if(i % 4 == 0)
        {
            DENS.append(maps.at(i));

        }
        if(i % 4 == 1)
        {
            VX.append(maps.at(i));
        }
        if(i % 4 == 2)
        {
            VY.append(maps.at(i));
        }
        if(i % 4 == 3)
        {
            VZ.append(maps.at(i));
        }

    }

    cTMap * HN =DENS.at(0);

    StableFluid3d f(HN->nrCols(),HN->nrRows(),DENS.length(),(float)(HN->nrCols()) * std::fabs(HN->cellSizeX()), (float)(HN->nrRows()) * std::fabs(HN->cellSizeY()),z_max-z_min);

    int * b = f.Block();
    float * u = f.U();
    float * v = f.V();
    float * w = f.W();
    float * d = f.Density();

    for(int r = 0; r < DENS.at(0)->nrRows(); r++)
    {
        for(int c = 0;c < DENS.at(0)->nrCols(); c++)
        {
            for(int i = 0;i < DENS.length(); i++)
            {
                //get z

                //check if below DEM


                int index = f.IX(i,r,c);
                if(!pcr::isMV(DENS.at(i)->data[r][c]))
                {
                    u[index] = VX.at(i)->data[r][c];
                    v[index] = VY.at(i)->data[r][c];
                    w[index] = VZ.at(i)->data[r][c];
                    d[index] = DENS.at(i)->data[r][c];
                    b[index] = 1;

                }else {

                    b[index] = 0;
                }
            }


        }

    }


    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {
        //first update flow heights
        double dt = t_end - t;

        for(int r = 0; r < DENS.at(0)->nrRows(); r++)
        {
            for(int c = 0;c < DENS.at(0)->nrCols(); c++)
            {
                for(int i = 0;i < DENS.length(); i++)
                {

                    int index = f.IX(i, r,c);
                    if(!pcr::isMV(DENS.at(0)->data[r][c]))
                    {
                        double dt_this = 0.25* std::min( std::fabs(HN->cellSizeX()), std::fabs(HN->cellSizeY()))/std::max(0.0001f,std::max(std::fabs(w[index]),std::max(std::fabs(u[index]),std::fabs(v[index]))));

                        dt = std::min(dt_this,dt);
                    }
                }
            }
        }
        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        f.Update(visc,diff,dt);

        t = t+dt;
        iter ++;

    }




    for(int r = 0; r < DENS.at(0)->nrRows(); r++)
    {
        for(int c = 0;c < DENS.at(0)->nrCols(); c++)
        {
            for(int i = 0;i < res.size()/4; i++)
            {
                //get z

                //check if below DEM

                int index = f.IX(i,r,c);
                if(!pcr::isMV(DENS.at(i)->data[r][c]))
                {
                    VX.at(i* 4 + 1)->data[r][c] = u[index];
                    VY.at(i * 4 + 2)->data[r][c] = v[index];
                    VZ.at(i * 4 + 3)->data[r][c] = w[index];
                    DENS.at(i*4 + 0)->data[r][c] = d[index];

                }else {

                }
            }


        }

    }

    return res;
}

inline cTMap * AS_FlowSteadyDiffusive(cTMap * DEM, cTMap * FlowSource, int iter_max, float courant, float hscale = 1.0)
{



    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");



    float N = 0.1;
    double _dx = std::fabs(DEM->cellSizeX());

    //do the loop
    for(int i = 0; i < iter_max; i++)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demh =DEM->data[r][c]+hscale *H->data[r][c];
                    bool outlet = false;
                    if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                    {
                        outlet = 1.0;
                    }

                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                    dem = dem + pit;
                    demh = demh + pit;

                    double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                    double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                    double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                    double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                    double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                    double h = std::max(0.0f,H->data[r][c]);

                    double Shx1 = (demhx1 - (demh))/(_dx);
                    double Shx2 = ((demh)- (demhx2))/(_dx);
                    double Shy1 = (demhy1 - (demh))/(_dx);
                    double Shy2 = ((demh)- (demhy2))/(_dx);

                    double Sx1 = (demx1 - (dem))/(_dx);
                    double Sx2 = ((dem)- (demx2))/(_dx);
                    double Sy1 = (demy1 - (dem))/(_dx);
                    double Sy2 = ((dem)- (demy2))/(_dx);

                    if(demhx1 < demhx2)
                    {
                        SX->Drc = Shx1;
                    }else
                    {
                        SX->Drc = Shx2;
                    }

                    if(demhy1 < demhy2)
                    {
                        SY->Drc = Shy1;
                    }else
                    {
                        SY->Drc = Shy2;
                    }

                    double flowwidth = _dx;

                    S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                    double HL =  std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                    double Hm = std::min(hscale*h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))))/hscale;

                    double q = 1.0 *flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0/N) *std::pow((HL),2.0/3.0);
                    double qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0/N) *std::pow((Hm),2.0/3.0);



                    q = std::min(q,h * _dx * _dx);
                    double V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001))*(1.0/N) *std::pow((HL*_dx)/(2.0 * HL + _dx),2.0/3.0) : 0.0;

                    double qx = (demhx1 < demhx2 > 0? -1.0:1.0) * (std::fabs(SX->Drc)/S->Drc) * q;
                    double qy = (demhy1 < demhy2 > 0? -1.0:1.0) * (std::fabs(SY->Drc)/S->Drc) * q;

                    QX1->Drc = qx < 0? qx :0.0;
                    QX2->Drc = qx > 0? qx :0.0;
                    QY1->Drc = qy < 0? qy :0.0;
                    QY2->Drc = qy > 0? qy :0.0;
                }

            }
        }

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double dem = DEM->Drc;

                            double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                            double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                            double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                            double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                            double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                            dem = dem + pit;

                            double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                            double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                            double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                            double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                            double demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                            double demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                            double demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                            double demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                            double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                            demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                            double qx1 = std::max(0.0,((!OUTORMV(DEM,r,c-1))? (std::fabs(QX2->data[r][c-1])) : 0.0));
                            double qx2 = std::max(0.0,((!OUTORMV(DEM,r,c+1))? (std::fabs(QX1->data[r][c+1])) : 0.0));
                            double qy1 = std::max(0.0,((!OUTORMV(DEM,r-1,c))? (std::fabs(QY2->data[r-1][c])) : 0.0));
                            double qy2 = std::max(0.0,((!OUTORMV(DEM,r+1,c))? (std::fabs(QY1->data[r+1][c])) : 0.0));

                            double qoutx1 = std::max(0.0f,(std::fabs(QX2->data[r][c])));
                            double qoutx2 = std::max(0.0f,(std::fabs(QX1->data[r][c])));
                            double qouty1 = std::max(0.0f,(std::fabs(QY2->data[r][c])));
                            double qouty2 = std::max(0.0f,(std::fabs(QY1->data[r][c])));

                            //total net incoming discharge
                            double QIN = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                            double Q = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2



                            //solve for height, so that new discharge is incoming discharge
                            double sol = std::max(0.0,(demhmin -DEM->data[r][c] ))+ std::pow( Q*  N /(sqrt(S->Drc + 0.001)*_dx),3.0/5.0);

                            double hnew = std::max(std::max(0.0,demhmin- dem)/hscale,sol);
                            HN->data[r][c] = H->Drc * (1.0-courant) + (courant) *hnew;//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                }
            }
        }
    }


    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;

    delete SX;
    delete SY;

    delete S;

    return H;
}


inline cTMap * AS_SimilaritySpread(cTMap * Val1, cTMap * Val2, cTMap * in_Prop, int iter_max, float wp, float wself, float coeff, bool w_original, bool only_larger)
{

    if(!(Val1->data.nr_rows() == Val2->data.nr_rows() && Val1->data.nr_cols() == Val2->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for Val2");
       throw -1;
    }
    if(!(Val1->data.nr_rows() == in_Prop->data.nr_rows() && Val1->data.nr_cols() == in_Prop->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for Prop");
       throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run similarity spread without iterations");
        throw 1;
    }

    cTMap * Prop = in_Prop->GetCopy();
    cTMap * PropN = in_Prop->GetCopy();


    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        float N = 0.05;
        double _dx = std::fabs(Val1->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    std::cout << "should break" <<std::endl;
                    stop = true;
                    do_stop = true;
                }

            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < Prop->data.nr_rows();r++)
            {
                for(int c = 0; c < Prop->data.nr_cols();c++)
                {
                    if(!pcr::isMV(Prop->data[r][c]))
                    {
                        float propc = Prop->Drc;
                        float prop;

                        if(w_original)
                        {
                            prop = in_Prop->Drc;
                        }else
                        {
                            prop = Prop->Drc;
                        }

                        bool outlet = false;
                        if(OUTORMV(Prop,r,c-1) || OUTORMV(Prop,r,c+1) || OUTORMV(Prop,r-1,c) || OUTORMV(Prop,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float propx1 = !OUTORMV(Prop,r,c-1)? Prop->data[r][c-1] : prop;
                        float propx2 = !OUTORMV(Prop,r,c+1)? Prop->data[r][c+1] : prop;
                        float propy1 = !OUTORMV(Prop,r-1,c)? Prop->data[r-1][c] : prop;
                        float propy2 = !OUTORMV(Prop,r+1,c)? Prop->data[r+1][c] : prop;

                        float val1 = Val1->Drc;
                        float val2 = Val2->Drc;

                        float val1x1 = !OUTORMV(Val1,r,c-1)? Val1->data[r][c-1] : val1;
                        float val1x2 = !OUTORMV(Val1,r,c+1)? Val1->data[r][c+1] : val1;
                        float val1y1 = !OUTORMV(Val1,r-1,c)? Val1->data[r-1][c] : val1;
                        float val1y2 = !OUTORMV(Val1,r+1,c)? Val1->data[r+1][c] : val1;

                        float val2x1 = !OUTORMV(Val2,r,c-1)? Val2->data[r][c-1] : val2;
                        float val2x2 = !OUTORMV(Val2,r,c+1)? Val2->data[r][c+1] : val2;
                        float val2y1 = !OUTORMV(Val2,r-1,c)? Val2->data[r-1][c] : val2;
                        float val2y2 = !OUTORMV(Val2,r+1,c)? Val2->data[r+1][c] : val2;

                        float ratio_c;

                        //if the center ratio is very good (close to 1), we give no room for the neighboring cells anymore
                        float ratio_x1;// = (1.0f - ratio_c) * std::pow(std::min(val2x1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x1)),wp);
                        float ratio_x2;// = (1.0f - ratio_c) * std::pow(std::min(val2x2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x2)),wp);
                        float ratio_y1;// = (1.0f - ratio_c) * std::pow(std::min(val2y1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y1)),wp);
                        float ratio_y2;// = (1.0f - ratio_c) * std::pow(std::min(val2y2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y2)),wp);

                        if(!only_larger)
                        {
                            ratio_c = std::pow(std::min(val2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2)),wp);

                            ratio_x1 = (1.0f - ratio_c) * (std::min(val2x1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x1)));
                            ratio_x2 = (1.0f - ratio_c) * (std::min(val2x2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x2)));
                            ratio_y1 = (1.0f - ratio_c) * (std::min(val2y1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y1)));
                            ratio_y2 = (1.0f - ratio_c) * (std::min(val2y2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y2)));

                        }else
                        {
                            ratio_c = std::fabs(val2) * std::pow(std::min(val2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2)),wp);

                            ratio_x1 = std::fabs(val2x1) * (std::min(val2x1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x1)));
                            ratio_x2 = std::fabs(val2x2) * (std::min(val2x2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2x2)));
                            ratio_y1 = std::fabs(val2y1) * (std::min(val2y1/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y1)));
                            ratio_y2 = std::fabs(val2y2) * (std::min(val2y2/std::max(1e-12f,val1),val1/std::max(1e-12f,val2y2)));

                        }

                        float wtotal =std::max(1e-12f,wself * ratio_c + ratio_x1 + ratio_x2 + ratio_y1 + ratio_y2);
                        ratio_c = std::pow(std::max(0.0f,ratio_c/wtotal),wp);
                        ratio_x1 = std::pow(std::max(0.0f,ratio_x1/wtotal),wp);
                        ratio_x2 = std::pow(std::max(0.0f,ratio_x2/wtotal),wp);
                        ratio_y1 = std::pow(std::max(0.0f,ratio_y1/wtotal),wp);
                        ratio_y2 = std::pow(std::max(0.0f,ratio_y2/wtotal),wp);

                        PropN->data[r][c] = (wself  * ratio_c * prop + ratio_x1 * propx1 + ratio_x2 * propx2 + ratio_y1 * propy1 + ratio_y2 * propy2)/(std::max(1e-12f,wself * ratio_c + ratio_x1 + ratio_x2 + ratio_y1 + ratio_y2));
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < Prop->data.nr_rows();r++)
            {
                for(int c = 0; c < Prop->data.nr_cols();c++)
                {
                    if(!pcr::isMV(Prop->data[r][c]))
                    {
                        Prop->data[r][c] = coeff * Prop->data[r][c] + coeff * PropN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete PropN;

    return Prop;


}

//this approacht to 2d accuflux doesnt seem to work well, you need some kind of physical process to get the drainage right
//the function AccuFluxDiffusive is used instead
inline cTMap * AS_AccuFlux2D(cTMap * DEM, cTMap * Source, int iter_max, float height_scale)
{

    if(!(DEM->data.nr_rows() == Source->data.nr_rows() && DEM->data.nr_cols() == Source->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps


    float courant = 0.25;

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");


    float _dx = std::fabs(DEM->cellSizeX());

    bool stop = false;

    #pragma omp parallel shared(stop)
    {
        int i = 0;

        while( true)
        {
            i++;

            if(i >= iter_max)
            {
                stop = true;
            }

            #pragma omp barrier
            if(stop)
            {
                break;
            }



            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+height_scale*H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+height_scale*H->data[r][c-1] : dem;
                        double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+height_scale*H->data[r][c+1] : dem;
                        double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+height_scale*H->data[r-1][c] : dem;
                        double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+height_scale*H->data[r+1][c] : dem;

                        double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        double h = height_scale*std::max(0.0f,H->data[r][c]);

                        double Shx1 = (demhx1 - (demh))/(_dx);
                        double Shx2 = ((demh)- (demhx2))/(_dx);
                        double Shy1 = (demhy1 - (demh))/(_dx);
                        double Shy2 = ((demh)- (demhy2))/(_dx);

                        double Sx1 = (demx1 - (dem))/(_dx);
                        double Sx2 = ((dem)- (demx2))/(_dx);
                        double Sy1 = (demy1 - (dem))/(_dx);
                        double Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        double flowwidth = _dx;

                        S->Drc = (std::sqrt(SX->Drc * SX->Drc + SY->Drc * SY->Drc + 1e-8));

                        double HL = h;//std::min(h,std::max(0.0,H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));

                        double q = H->data[r][c] * _dx * _dx;

                        double qx = (demhx1 < demhx2 ? -1.0:1.0) * std::fabs(std::cos(atan2(std::fabs(SY->Drc),std::fabs(SX->Drc)))) * q;
                        double qy = (demhy1 < demhy2 ? -1.0:1.0) * std::fabs(std::sin(atan2(std::fabs(SY->Drc),std::fabs(SX->Drc)))) * q;

                        QX1->Drc = qx < 0? qx :0.0;
                        QX2->Drc = qx > 0? qx :0.0;
                        QY1->Drc = qy < 0? qy :0.0;
                        QY2->Drc = qy > 0? qy :0.0;
                    }

                }
            }

            #pragma omp barrier


            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        double dem = DEM->Drc;

                                double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                                dem = dem + pit;

                                double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+height_scale*H->data[r][c-1] : dem;
                                double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+height_scale*H->data[r][c+1] : dem;
                                double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+height_scale*H->data[r-1][c] : dem;
                                double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+height_scale*H->data[r+1][c] : dem;

                                double demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+height_scale*H->data[r-1][c-1] : dem;
                                double demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+height_scale*H->data[r+1][c+1] : dem;
                                double demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+height_scale*H->data[r-1][c+1] : dem;
                                double demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+height_scale*H->data[r+1][c-1] : dem;

                                double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                double qx1 = std::max(0.0,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0));
                                double qx2 = std::max(0.0,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0));
                                double qy1 = std::max(0.0,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0));
                                double qy2 = std::max(0.0,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0));

                                double qoutx1 = std::max(0.0f,(std::fabs(QX2->data[r][c])));
                                double qoutx2 = std::max(0.0f,(std::fabs(QX1->data[r][c])));
                                double qouty1 = std::max(0.0f,(std::fabs(QY2->data[r][c])));
                                double qouty2 = std::max(0.0f,(std::fabs(QY1->data[r][c])));

                                //total net incoming discharge
                                double QIN = std::max(0.0, (qx1 + qx2 + qy1 + qy2) + Source->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                //double Q = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2) + 1.0);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                //solve for height, so that new discharge is incoming discharge
                                double sol = QIN /(_dx*_dx);

                                double hnew = sol;//std::max(std::max(0.0,demhmin- dem),sol);
                                HN->data[r][c] = H->Drc * (1.0-courant) + (courant) *hnew;//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                    }
                }
            }

            #pragma omp barrier

            #pragma ompfor collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    H->data[r][c] = HN->data[r][c];
                }
            }
        }
    }

    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;

    delete S;
    delete SX;
    delete SY;

    return H;
}


inline cTMap * AS_AccufluxSoil(cTMap * DEM, cTMap * Accuflux, cTMap * QS, float k, float power, int iter_max, float courant)
{
    if(!(DEM->data.nr_rows() == QS->data.nr_rows() && DEM->data.nr_cols() == QS->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == Accuflux->data.nr_rows() && DEM->data.nr_cols() == Accuflux->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps


    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    MaskedRaster<float> raster_data10(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SD = new cTMap(std::move(raster_data10),DEM->projection(),"");
    MaskedRaster<float> raster_data11(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SDN = new cTMap(std::move(raster_data11),DEM->projection(),"");

    double _dx = std::fabs(DEM->cellSizeX());
    double hnmax = 0.0;
    double sdmax = 0.0;
    //do the loop

    hnmax = 0.0;

    double accmax = 0.0;

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(Accuflux->data[r][c]))
            {
                Accuflux->data[r][c] = std::log(Accuflux->data[r][c]);
            }
        }
    }

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(Accuflux->data[r][c]))
            {
                hnmax = std::max(hnmax,(double)Accuflux->data[r][c]);

            }
        }
    }

    if(hnmax > 0.0)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(Accuflux->data[r][c]))
                {
                    H->data[r][c] = std::pow(Accuflux->data[r][c]/hnmax, power);

                }
            }
        }
    }



    //do the loop
    for(int i = 0; i < iter_max; i++)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demh =DEM->data[r][c]+H->data[r][c];
                    bool outlet = false;
                    if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                    {
                        outlet = 1.0;
                    }

                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                    dem = dem + pit;
                    demh = demh + pit;

                    double sd = std::max(0.0f,SD->data[r][c]);

                    double shx1 = !OUTORMV(DEM,r,c-1)? SD->data[r][c-1] : sd;
                    double shx2 = !OUTORMV(DEM,r,c+1)? SD->data[r][c+1] : sd;
                    double shy1 = !OUTORMV(DEM,r-1,c)? SD->data[r-1][c] : sd;
                    double shy2 = !OUTORMV(DEM,r+1,c)? SD->data[r+1][c] : sd;


                    double Sx1 = std::max(0.0,((dem)- (demx1))/(_dx));
                    double Sx2 = std::max(0.0,((dem)- (demx2))/(_dx));
                    double Sy1 = std::max(0.0,((dem)- (demy1))/(_dx));
                    double Sy2 = std::max(0.0,((dem)- (demy2))/(_dx));

                    double St =Sx1 + Sx2 + Sy1 + Sy2;

                    double flowwidth = _dx;
                    double sd3 = sd*sd*sd;
                    double qx1 = Sx1 * shx1 * shx1 * Sx1;
                    double qx2 = Sx2 * shx2 * shx2 * Sx2;
                    double qy1 = Sy1 * shy1 * shy1 * Sy1;
                    double qy2 = Sy2 * shy2 * shy2 * Sy2;

                    Sx1 = std::max(0.0,-((dem)- (demx1))/(_dx));
                    Sx2 = std::max(0.0,-((dem)- (demx2))/(_dx));
                    Sy1 = std::max(0.0,-((dem)- (demy1))/(_dx));
                    Sy2 = std::max(0.0,-((dem)- (demy2))/(_dx));

                    double qxo1 = Sx1 * shx1 * shx1 * Sx1;
                    double qxo2 = Sx2 * shx2 * shx2 * Sx2;
                    double qyo1 = Sy1 * shy1 * shy1 * Sy1;
                    double qyo2 = Sy2 * shy2 * shy2 * Sy2;

                    double wsum = std::max(0.01,(-qx1 + qxo1 - qy1 + qyo1 -qx2 + qxo2 - qy2 + qyo2));
                    H->data[r][c] = std::max(0.01f,H->data[r][c]);
                    SDN->data[r][c] = (1.0-courant) * SD->data[r][c] + courant * 1.0 * (std::max(0.01,std::log(std::max(1.0,_dx*_dx/(( k * wsum + QS->data[r][c] * H->data[r][c] ))))));
                }
            }
        }


        sdmax = 0.0;
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(SDN->data[r][c]))
                {

                    SD->data[r][c] = SDN->data[r][c];
                    sdmax = std::max(sdmax,(double) SDN->data[r][c]);
                }
            }
        }
    }

    if(sdmax > 0.0)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(SD->data[r][c]))
                {

                    SD->data[r][c] = sdmax - SDN->data[r][c];
                }else
                {
                    pcr::setMV(SD->data[r][c]);
                }
            }
        }
    }

    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;
    delete SDN;
    delete S;
    delete SX;
    delete SY;

    return SD;
}



inline cTMap * AS_SteadyStateSoil(cTMap * DEM, cTMap * Source, cTMap * QS, int iter_max)
{
    if(!(DEM->data.nr_rows() == QS->data.nr_rows() && DEM->data.nr_cols() == QS->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }
    if(!(DEM->data.nr_rows() == Source->data.nr_rows() && DEM->data.nr_cols() == Source->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for N");
       throw -1;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps


    float courant = 0.25;

    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    MaskedRaster<float> raster_data10(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SD = new cTMap(std::move(raster_data10),DEM->projection(),"");
    MaskedRaster<float> raster_data11(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SDN = new cTMap(std::move(raster_data11),DEM->projection(),"");

    double _dx = std::fabs(DEM->cellSizeX());
    double hnmax = 0.0;
    double sdmax = 0.0;
    //do the loop
    for(int i = 0; i < iter_max; i++)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demh =DEM->data[r][c]+H->data[r][c];
                    bool outlet = false;
                    if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                    {
                        outlet = 1.0;
                    }

                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                    dem = dem + pit;
                    demh = demh + pit;

                    double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+H->data[r][c-1] : dem;
                    double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+H->data[r][c+1] : dem;
                    double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+H->data[r-1][c] : dem;
                    double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+H->data[r+1][c] : dem;

                    double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                    double h = std::max(0.0f,H->data[r][c]);

                    double Shx1 = (demhx1 - (demh))/(_dx);
                    double Shx2 = ((demh)- (demhx2))/(_dx);
                    double Shy1 = (demhy1 - (demh))/(_dx);
                    double Shy2 = ((demh)- (demhy2))/(_dx);

                    double Sx1 = (demx1 - (dem))/(_dx);
                    double Sx2 = ((dem)- (demx2))/(_dx);
                    double Sy1 = (demy1 - (dem))/(_dx);
                    double Sy2 = ((dem)- (demy2))/(_dx);

                    if(demhx1 < demhx2)
                    {
                        SX->Drc = Shx1;
                    }else
                    {
                        SX->Drc = Shx2;
                    }

                    if(demhy1 < demhy2)
                    {
                        SY->Drc = Shy1;
                    }else
                    {
                        SY->Drc = Shy2;
                    }

                    double flowwidth = _dx;

                    S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                    double HL = h;//std::min(h,std::max(0.0,H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));

                    double q = h * _dx * _dx;

                    double qx = (demhx1 < demhx2 > 0? -1.0:1.0) * (std::fabs(SX->Drc)/S->Drc) * q;
                    double qy = (demhy1 < demhy2 > 0? -1.0:1.0) * (std::fabs(SY->Drc)/S->Drc) * q;

                    QX1->Drc = qx < 0? qx :0.0;
                    QX2->Drc = qx > 0? qx :0.0;
                    QY1->Drc = qy < 0? qy :0.0;
                    QY2->Drc = qy > 0? qy :0.0;
                }

            }
        }

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double dem = DEM->Drc;

                            double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                            double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                            double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                            double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                            double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                            dem = dem + pit;

                            double demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+H->data[r][c-1] : dem;
                            double demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+H->data[r][c+1] : dem;
                            double demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+H->data[r-1][c] : dem;
                            double demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+H->data[r+1][c] : dem;

                            double demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+H->data[r-1][c-1] : dem;
                            double demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+H->data[r+1][c+1] : dem;
                            double demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+H->data[r-1][c+1] : dem;
                            double demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+H->data[r+1][c-1] : dem;

                            double demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                            demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                            double qx1 = std::max(0.0,((!OUTORMV(DEM,r,c-1))? (std::fabs(QX2->data[r][c-1])) : 0.0));
                            double qx2 = std::max(0.0,((!OUTORMV(DEM,r,c+1))? (std::fabs(QX1->data[r][c+1])) : 0.0));
                            double qy1 = std::max(0.0,((!OUTORMV(DEM,r-1,c))? (std::fabs(QY2->data[r-1][c])) : 0.0));
                            double qy2 = std::max(0.0,((!OUTORMV(DEM,r+1,c))? (std::fabs(QY1->data[r+1][c])) : 0.0));

                            double qoutx1 = std::max(0.0f,(std::fabs(QX2->data[r][c])));
                            double qoutx2 = std::max(0.0f,(std::fabs(QX1->data[r][c])));
                            double qouty1 = std::max(0.0f,(std::fabs(QY2->data[r][c])));
                            double qouty2 = std::max(0.0f,(std::fabs(QY1->data[r][c])));

                            //total net incoming discharge
                            double QIN = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                            double Q = std::max(0.0,0.5 * (qx1 + qx2 + qy1 + qy2) +0.5 * (qoutx1 + qoutx2 + qouty1 + qouty2) + 1.0);// - qoutx1 - qoutx2 - qouty1 - qouty2

                            //solve for height, so that new discharge is incoming discharge
                            double sol = Q /(_dx*_dx);

                            double hnew = std::max(std::max(0.0,demhmin- dem),sol);
                            HN->data[r][c] = H->Drc * (1.0-courant) + (courant) *hnew;//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                }
            }
        }

        hnmax = 0.0;

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(HN->data[r][c]))
                {
                    H->data[r][c] = HN->data[r][c];
                    hnmax = std::max(hnmax,(double)HN->data[r][c]);

                }
            }
        }
    }

    if(hnmax > 0.0)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(H->data[r][c]))
                {
                    H->data[r][c] = std::sqrt(H->data[r][c]/hnmax);

                }
            }
        }
    }



    //do the loop
    for(int i = 0; i < iter_max; i++)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demh =DEM->data[r][c]+H->data[r][c];
                    bool outlet = false;
                    if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                    {
                        outlet = 1.0;
                    }

                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    double pit = std::min(std::max(0.0,demx1 - dem),std::min(std::max(0.0,demx2 - dem),std::min(std::max(0.0,demy1 - dem),std::max(0.0,demy2 - dem))));

                    dem = dem + pit;
                    demh = demh + pit;

                    double sd = std::max(0.0f,SD->data[r][c]);

                    double shx1 = !OUTORMV(DEM,r,c-1)? SD->data[r][c-1] : sd;
                    double shx2 = !OUTORMV(DEM,r,c+1)? SD->data[r][c+1] : sd;
                    double shy1 = !OUTORMV(DEM,r-1,c)? SD->data[r-1][c] : sd;
                    double shy2 = !OUTORMV(DEM,r+1,c)? SD->data[r+1][c] : sd;


                    double Sx1 = std::max(0.0,((dem)- (demx1))/(_dx));
                    double Sx2 = std::max(0.0,((dem)- (demx2))/(_dx));
                    double Sy1 = std::max(0.0,((dem)- (demy1))/(_dx));
                    double Sy2 = std::max(0.0,((dem)- (demy2))/(_dx));

                    double St =Sx1 + Sx2 + Sy1 + Sy2;

                    double flowwidth = _dx;
                    double sd3 = sd*sd*sd;
                    double qx1 = Sx1 * shx1 * shx1 * Sx1;
                    double qx2 = Sx2 * shx2 * shx2 * Sx2;
                    double qy1 = Sy1 * shy1 * shy1 * Sy1;
                    double qy2 = Sy2 * shy2 * shy2 * Sy2;

                    Sx1 = std::max(0.0,-((dem)- (demx1))/(_dx));
                    Sx2 = std::max(0.0,-((dem)- (demx2))/(_dx));
                    Sy1 = std::max(0.0,-((dem)- (demy1))/(_dx));
                    Sy2 = std::max(0.0,-((dem)- (demy2))/(_dx));

                    double qxo1 = Sx1 * shx1 * shx1 * Sx1;
                    double qxo2 = Sx2 * shx2 * shx2 * Sx2;
                    double qyo1 = Sy1 * shy1 * shy1 * Sy1;
                    double qyo2 = Sy2 * shy2 * shy2 * Sy2;

                    double wsum = -(-qx1 + qxo1 - qy1 + qyo1 -qx2 + qxo2 - qy2 + qyo2);
                    H->data[r][c] = std::max(0.01f,H->data[r][c]);
                    SDN->data[r][c] = 1.0 * (std::max(0.01,std::log(std::max(1.0,_dx*_dx/(( Source->data[r][c] * wsum  + QS->data[r][c] * _dx*_dx*_dx * H->data[r][c] ))))));
                }
            }
        }


        sdmax = 0.0;
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(SDN->data[r][c]))
                {

                    SD->data[r][c] = SDN->data[r][c];
                    sdmax = std::max(sdmax,(double) SDN->data[r][c]);
                }
            }
        }
    }

    if(sdmax > 0.0)
    {
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(SD->data[r][c]))
                {

                    SD->data[r][c] = sdmax - SDN->data[r][c];
                }
            }
        }
    }

    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;
    delete SDN;
    delete S;
    delete SX;
    delete SY;

    return SD;
}


inline cTMap * AS_TransientFlow(cTMap * DEM, cTMap * SD, cTMap * KSat, cTMap * Theta, cTMap * HGW, float dt, bool inflowlimit)
{


    if(DEM->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for DiffusiveMaxWave (DEM) can not be non-spatial");
        throw 1;
    }
    if(!(dt > 0.0))
    {
        LISEMS_ERROR("Timestep must be greater then 0.0");
        throw 1;
    }

    if(!(DEM->data.nr_rows() == SD->data.nr_rows() && DEM->data.nr_cols() == SD->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }
    if(!(DEM->data.nr_rows() == SD->data.nr_rows() && DEM->data.nr_cols() == SD->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }
    if(!(DEM->data.nr_rows() == KSat->data.nr_rows() && DEM->data.nr_cols() == KSat->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }
    if(!(DEM->data.nr_rows() == Theta->data.nr_rows() && DEM->data.nr_cols() == Theta->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }



    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),DEM->projection(),"");

    float dx = DEM->cellSize();
    float sl;
    float dem,dem_xl,dem_xr,dem_yl,dem_yr;
    float h, h_xl, h_xr, h_yl, h_yr;
    float sd, sd_xl, sd_xr, sd_yl, sd_yr;
    float theta, theta_xl, theta_xr, theta_yl, theta_yr;
    float hn, qmax_xr,qmax_xl,qmax_yl,qmax_yr,signx,signy,vx,vy,qx,qy,sx,sy;
    bool mv_xl,mv_xr,mv_yl,mv_yr;
    float qmaxx, qmaxy;

    float ksat_max = 0.0;

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                map->data[r][c] = (!(pcr::isMV(HGW->data[r][c])))? HGW->Drc * Theta->Drc : 0.0;
                ksat_max = std::max(KSat->data[r][c],ksat_max);
            }
        }
    }

    if(!(ksat_max > 0.0))
    {
        delete map;
        LISEMS_ERROR("KSat must be at least larger than 0.0 somewhere");
        throw 1;
    }


    cTMap * QX = map->GetCopy0();
    cTMap * QY= map->GetCopy0();

    //example : 10 meter gridd cell * 0.05 / (100 mm/h)  = 5 hours
    double dt_act = 0.05 * dx / ksat_max;

    double t = 0.0;

    while(t < dt)
    {
        double dt_actn = std::min(dt_act,dt-t);
        t += dt_actn;
        for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
        {
            for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    //reset variables
                    hn = 0.0;
                    qmax_xr = 0.0;
                    qmax_xl = 0.0;
                    qmax_yl = 0.0;
                    qmax_yr = 0.0;
                    signx = 0.0;
                    signy = 0.0;
                    vx = 0.0;
                    vy = 0.0;
                    qx = 0.0;
                    qy = 0.0;
                    sx = 0.0;
                    sy = 0.0;
                    mv_xl = false;
                    mv_xr = false;
                    mv_yl = false;
                    mv_yr = false;

                    dem =DEM->data[r][c];

                    //slope
                    if(c > 0)
                    {
                        dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                    }else {
                        dem_xl = dem;
                    }
                    if(c < DEM->nrCols()-1)
                    {
                        dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                    }else {
                        dem_xr = dem;
                    }
                    if(r > 0)
                    {
                        dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                    }else {
                        dem_yl = dem;
                    }
                    if(r < DEM->nrRows()-1)
                    {
                        dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                    }else {
                        dem_yr = dem;

                    }

                    h = map->Drc;

                    if(c > 0)
                    {
                        h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                    }else {
                        h_xl = h;
                    }
                    if(c < DEM->nrCols()-1)
                    {
                        h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                    }else {
                        h_xr = h;
                    }
                    if(r > 0)
                    {
                        h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                    }else {
                        h_yl = h;
                    }
                    if(r < DEM->nrRows()-1)
                    {
                        h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                    }else {
                        h_yr = h;
                    }

                    theta = Theta->Drc;

                    if(c > 0)
                    {
                        theta_xl = pcr::isMV(map->data[r][c-1])?theta:Theta->data[r][c-1];
                    }else {
                        theta_xl = h;
                    }
                    if(c < DEM->nrCols()-1)
                    {
                        theta_xr = pcr::isMV(map->data[r][c+1])?theta:Theta->data[r][c+1];
                    }else {
                        theta_xr = h;
                    }
                    if(r > 0)
                    {
                        theta_yl = pcr::isMV(map->data[r-1][c])?theta:Theta->data[r-1][c];
                    }else {
                        theta_yl = h;
                    }
                    if(r < DEM->nrRows()-1)
                    {
                        theta_yr = pcr::isMV(map->data[r+1][c])?theta:Theta->data[r+1][c];
                    }else {
                        theta_yr = h;
                    }

                    sd = SD->Drc;

                    if(c > 0)
                    {
                        sd_xl = pcr::isMV(map->data[r][c-1])?sd:SD->data[r][c-1];
                    }else {
                        sd_xl = h;
                    }
                    if(c < DEM->nrCols()-1)
                    {
                        sd_xr = pcr::isMV(map->data[r][c+1])?sd:SD->data[r][c+1];
                    }else {
                        sd_xr = h;
                    }
                    if(r > 0)
                    {
                        sd_yl = pcr::isMV(map->data[r-1][c])?sd:SD->data[r-1][c];
                    }else {
                        sd_yl = h;
                    }
                    if(r < DEM->nrRows()-1)
                    {
                        sd_yr = pcr::isMV(map->data[r+1][c])?sd:SD->data[r+1][c];
                    }else {
                        sd_yr = h;
                    }




                    sx = dem_xr-dem_xl;
                    sy = dem_yr-dem_yl;


                    //velocity
                    sl = std::sqrt(sx*sx + sy*sy);
                    vx = KSat->data[r][c] * sx/sl;
                    vy = KSat->data[r][c] *sy/sl;
                    signx = vx > 0.0? -1.0:1.0;
                    signy = vy > 0.0? -1.0:1.0;

                    //flux limiter

                    qmax_xl = std::max(0.0f,dem+h/theta - dem_xl - h_xl/theta_xl);
                    qmax_xr = std::max(0.0f,dem+h/theta - dem_xr - h_xr/theta_xr);
                    qmax_yl = std::max(0.0f,dem+h/theta - dem_yl - h_yl/theta_yl);
                    qmax_yr = std::max(0.0f,dem+h/theta - dem_yr - h_yr/theta_yr);

                    //discharge

                    qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                    qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                    qx = std::min(0.25*h,std::min((double)qmaxx,std::abs(0.25*h * vx * dt_actn/dx)));
                    qy = std::min(0.25*h,std::min((double)qmaxy,std::abs(0.25*h * vy * dt_actn/dx)));

                    if(inflowlimit)
                    {


                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                qx = std::min(qx,sd_xl*theta_xl - h_xl);
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                qx = std::min(qx,sd_xr*theta_xr - h_xr);
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                qy = std::min(qy,sd_yl*theta_yl - h_yl);
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {

                                qy = std::min(qy,sd_yr*theta_yr - h_yr);
                            }
                        }
                    }


                    //change height

                    QX->data[r][c] = signx*qx;
                    QY->data[r][c] = signy*qy;


                }
            }


        }

        for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
        {
            for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double h = map->data[r][c];
                    double qx = QX->data[r][c];
                    double qy = QY->data[r][c];

                    hn = h - std::fabs(qx) - std::fabs(qy);
                    map->data[r][c] = hn;

                    if(c > 0)
                    {
                        if(qx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                        {
                            map->data[r][c-1] = map->data[r][c-1]-qx;
                        }
                    }
                    if(c < DEM->nrCols() -1)
                    {
                        if(qx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                        {
                            map->data[r][c+1] = map->data[r][c+1]+qx;
                        }
                    }

                    if(r > 0)
                    {
                        if(qy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                        {
                            map->data[r-1][c] = map->data[r-1][c]-qy;
                        }
                    }
                    if(r < DEM->nrRows() -1)
                    {
                        if(qy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                        {
                            map->data[r+1][c] = map->data[r+1][c]+qy;
                        }
                    }

                }
            }
        }

        for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
        {
            for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    map->data[r][c] = std::max(0.0f,std::min(map->data[r][c],SD->data[r][c] * Theta->data[r][c]))/std::max(0.001f,Theta->data[r][c]);

                }
            }
        }
    }

    delete QX;
    delete QY;

    map->AS_IsSingleValue = false;
    return map;
}



inline cTMap * AS_MVMap(cTMap * M)
{

    cTMap * m2 = M->GetCopy0();
    m2->setAllMV();
    return m2;
}



inline cTMap * AS_Diffusion(cTMap * VAL, float coeff, float dt, int iter = 1)
{
    cTMap * DEM =VAL->GetCopy();
    cTMap * DEMN = DEM->GetCopy();

    for(int i = 0; i < iter; i++)
    {
        for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
        {
            for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
            {
                if(!pcr::isMV(DEMN->data[r][c]))
                {

                    double dem = DEM->Drc;
                    double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;


                    float ddx = demx1 - 2.0 * dem + demx2;
                    float ddy = demy1 - 2.0 * dem + demy2;

                    DEMN->data[r][c] = DEM->data[r][c] - coeff *(ddx + ddy);

                }

            }
        }

        for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
        {
            for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
            {
                DEM->data[r][c] = DEMN->data[r][c];
            }
        }

    }

    delete DEMN;
    return DEM;


}
#include "rastercommon.h"
// Note convergence restrictions: abs(x) < 1 and c not a negative integer or zero
#include "gsl/gsl_errno.h"
#include "gsl/gsl_sf_hyperg.h"
inline static double hypergeometric( double a, double b, double c, double x )
{


    //does not converge well
    /*if(std::fabs(x) >= 1.0001f || c < 1.0e-12)
   {
        return 1.0;
   }
   const double TOLERANCE = 1.0e-10;
   double term = a * b * x / c;
   double value = 1.0 + term;
   int n = 1;

   while ( abs( term ) > TOLERANCE )
   {
      a++, b++, c++, n++;
      term *= a * b * x / c / n;
      value += term;
      if(n > 50)
      {
          break;
      }
   }

   return value;*/

    gsl_set_error_handler_off();


    if(std::isfinite(a) && std::isfinite(b) && std::isfinite(c) && std::isfinite(x))
    {
        double ret = gsl_sf_hyperg_2F1(a,b,c,x);
        if(std::isfinite(ret))
        {
            return ret;
        }else
        {
            if(n_warnhypergeom < 5)
            {
                LISEMS_ERROR("Could not run hyper-geometric function with values " + QString::number(a) + " " + QString::number(b)+ " " + QString::number(c) + " " + QString::number(x));

                 n_warnhypergeom += 1;
                if(n_warnhypergeom == 5)
                {
                    LISEMS_ERROR("Stopping ouput for this error");

                }
            }
            return 1.0;
        }
    }else
    {
        if(n_warnhypergeom < 5)
        {
            LISEMS_ERROR("Could not run hyper-geometric function with infinite values " + QString::number(a) + " " + QString::number(b)+ " " + QString::number(c) + " " + QString::number(x));

            n_warnhypergeom += 1;
            if(n_warnhypergeom == 5)
            {
                LISEMS_ERROR("Stopping ouput for this error");

            }
        }
        return 1.0;
    }

}

#include "gsl/gsl_sf_gamma.h"


inline static cTMap * AS_SteadyStateCorrection(cTMap * b, cTMap * Trel)
{

    if(!(b->nrRows() == Trel->nrRows() && b->nrCols() == Trel->nrCols()))
    {
        LISEMS_ERROR("Steay state correction called with maps of different numbers of rows and columns");
        throw 1;
    }

    cTMap * ret =b->GetCopy();

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols();c++)
        {
            if(!pcr::isMV(b->data[r][c]) && !pcr::isMV(Trel->data[r][c]))
            {
                if(!(b->data[r][c] > 0.0 && Trel->data[r][c] >= 0.0 && Trel->data[r][c] <= 1.0))
                {

                    pcr::setMV(ret->data[r][c]);
                    continue;
                }
                double There = Trel->data[r][c];
                double bhere = b->data[r][c];


                double log2 =0.69314718055994530943;
                double bsolve =  log2/std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere));
                double hg2 = hypergeometric(1.0/(1.0+bhere),log2/std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)),1.0+1.0/(1.0+bhere),std::pow(There,(1.0+bhere)));
                double p1 =(1.0/log2)*(log2 - std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)));

                double final = p1 *
                        (
                            ((bhere+1) * log2 * std::pow(There,bhere+1) * hg2 / std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)))
                                    - ((log2 * std::pow(1.0 - std::pow(There,bhere+1.0),1.0 - bsolve)))/(log2 - std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)))
                         - (((bhere + 1) * log2 * std::pow(There,bhere) * gsl_sf_gamma(1.0 + 1.0/(bhere+1.0)) * gsl_sf_gamma(1 - log2/(std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)))))/(std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)) * gsl_sf_gamma(-log2/(std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere))) + 1.0/(bhere  +1.0) + 1.0)))
                         + (log2/(log2 - std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere))))

                            );


                //hypergeometric is unstable due to singularities at high values of b, and x-value of 1

                /*
                //solve for median balance with distribution


                    double dev2 = std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere));

                double hg1 = hypergeometric(1.0/(1.0+bhere),log2/std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)),1.0+1.0/(1.0+bhere),0.9999);
                    double hg2 = hypergeometric(1.0/(1.0+bhere),log2/std::log(1.0 - std::pow(1.0 - std::pow(2.0,-(1.0/2.0)/bhere),1.0 + bhere)),1.0+1.0/(1.0+bhere),std::pow(There,(1.0+bhere)));

                    //total probabilistic normalized integral of discharges?
                    double p1 = (((std::pow(1.0,1.0-bsolve)*log2)/dev1)
                                             - ((std::pow((1.0 - std::pow(There,1+bhere)),1.0-bsolve)*log2)/dev1));

                    double p2 = (((std::pow(1.0,1.0-bsolve)*log2)/dev1)) ;


                    double final = (p1 - ((1.0+bhere)*std::pow(There,bhere) * hg1 * log2)/dev2 + ((1.0+bhere)*std::pow(There,bhere) * There * hg2 * log2)/dev2)/p2;

                    std::cout << r << " " << c << " " << bsolve << " " << dev1 << " " << dev2 << " " << p1 << " " << p2 << " "  << hg1 << " " << hg2 << " " <<  bhere << " " << There << " " << final << std::endl;
                    */




                if(std::isfinite(final))
                {
                    ret->data[r][c] = std::min(1.0,std::max(0.0,final));
                }else
                {
                    //okey, so if we dont get the convergence with the gamma functions and hypergeometric 2F1 functions, this is generally because we are on the very far right of the distribution.
                    //simply set the value to 1.0 seems to work well enough
                    ret->data[r][c] = 1.0;
                }


            }else
            {
                pcr::setMV(ret->data[r][c]);
            }
        }
    }

    return ret;


    //solved using Mathematica
    //represents the fractional loss of discharge with power law paramter bhere and relative event duration There
    //
    /*(((((1 - 0^(1 + bhere))^(
               1 - Log[2]/Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)])
                Log[2])/(
              Log[2] -
               Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)]) - ((1 - There^(
                 1 + bhere))^(
               1 - Log[2]/Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)])
                Log[2])/(
              Log[2] -
               Log[1 - (1 - 2^(-(1/2)/bhere))^(
                 1 + bhere)])) + ((-(((1 + bhere) There^
                 bhere 1 Hypergeometric2F1[1/(1 + bhere), Log[2]/
                  Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)],
                  1 + 1/(1 + bhere), 1^(1 + bhere)] Log[2])/
                Log[1 - (1 - 2^(-(1/2)/bhere))^(
                  1 + bhere)])) - (-(((1 + bhere) There^
                 bhere There Hypergeometric2F1[1/(1 + bhere), Log[2]/
                  Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)],
                  1 + 1/(1 + bhere), There^(1 + bhere)] Log[2])/
                Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)]))))/(((1 - 0^(
               1 + bhere))^(
             1 - Log[2]/Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)]) Log[2])/(
            Log[2] -
             Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)]) - ((1 - 1^(
               1 + bhere))^(
             1 - Log[2]/Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)]) Log[2])/(
            Log[2] - Log[1 - (1 - 2^(-(1/2)/bhere))^(1 + bhere)])))*/

}




#endif // RASTERFLOW_H
