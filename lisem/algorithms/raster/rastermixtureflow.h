#ifndef RASTERMIXTUREFLOW_H
#define RASTERMIXTUREFLOW_H

#include "linear/lsm_vector3.h"
#include "geo/raster/map.h"
#include "rasterflow.h"
#include <vector>
#define GRAV 9.81

static LSMVector3 ASF_HLL2FS(float h_L,float ho_L,float u_L,float v_L,float h_R,float ho_R,float u_R,float v_R,float sifa,float srocksize,float sdensity,float dragmult)
{
    LSMVector3 retnw;


    float c = std::max(std::fabs(u_L),std::fabs(u_R));
    float cd = c*0.5f;
    float q_R = u_R*h_R;
    float q_L = u_L*h_L;
    retnw.x = (q_L+q_R)*0.5f-cd*(h_R-h_L);
    retnw.y = ((u_L*q_L)+(u_R*q_R))*0.5f-cd*(q_R-q_L);
    retnw.z = (q_L*v_L+q_R*v_R)*0.5f-cd*(h_R*v_R-h_L*v_L);



    LSMVector3 ret;

        //force ratio to approximate the influence of the wave-velocity in riemann solution
        float ff_dev_L = std::max(0.001f,h_L/std::max(0.0001f,(h_L + ho_L)));
        float sf_dev_L = std::max(0.001f,ho_L/std::max(0.0001f,(h_L + ho_L)));
        float dc_L = dragmult * sf_dev_L * ff_dev_L * (1.0/(std::max(0.0001f,srocksize)))* std::max(0.0f,sdensity-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float fac_dc_L = std::max(0.0f,1.0f - (4.0f *(1.0f-std::min(1.0f,std::max(std::max(0.0f,(ff_dev_L)),(exp(-(dc_L /ff_dev_L)) + ((sf_dev_L *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        float ff_dev_R = std::max(0.001f,h_R/std::max(0.0001f,(h_R + ho_R)));
        float sf_dev_R = std::max(0.001f,ho_R/std::max(0.0001f,(h_R + ho_R)));
        float dc_R = dragmult * sf_dev_R * ff_dev_R * (1.0f/(std::max(0.0001f,srocksize)))* std::max(0.0f,sdensity-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float fac_dc_R = std::max(0.0f,1.0f - (4.0f *(1.0f-std::min(1.0f,std::max(std::max(0.0f,(ff_dev_R)),(exp(-(dc_R /ff_dev_R)) + ((sf_dev_R *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        float dc = (dc_L * h_L + dc_R * h_R)/std::max(0.0001f,(h_L + h_R));
        float ff_tot = std::max(0.001f,std::max(ho_L,ho_R)/std::max(0.0001f,std::max(h_L + ho_L,h_R + ho_R)));

        ff_tot = std::max(0.0f,std::min(1.0f, (ff_tot -0.2f) * 10.0f));

        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(std::min(1.0f,std::max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(std::min(1.0f,std::max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L)) * ff_tot;// * fac_dc_L;  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R)) * ff_tot;// * fac_dc_R ;
        q_R = u_R*h_R;
        q_L = u_L*h_L;



        float sc1 = std::min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = std::max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
        float tmp = 1.0f/std::max((float)0.1f,(float)(sc2-sc1));
        float t1 = (std::min((float)(sc2),(float)(0.)) - std::min((float)(sc1),(float)(0.f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;

        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);

        //ret.x = ret.x * (1.0f- ff_tot) + ff_tot*retnw.x;
        //ret.y = ret.y * (1.0f- ff_tot) + ff_tot*retnw.y;
        //ret.z = ret.z * (1.0f- ff_tot) + ff_tot*retnw.z;
                return ret;
}



static inline LSMVector3 ASF_HLL2SF(float h_L,float ho_L,float u_L,float v_L,float h_R,float ho_R,float u_R,float v_R,float sifa,float srocksize,float sdensity,float dragmult)
{
        LSMVector3 retnw;


                float c = std::max(fabs(u_L),fabs(u_R));
        float cd = c*0.5f;
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;
        retnw.x = (q_L+q_R)*0.5f-cd*(h_R-h_L);
        retnw.y = ((u_L*q_L)+(u_R*q_R))*0.5f-cd*(q_R-q_L);
        retnw.z = (q_L*v_L+q_R*v_R)*0.5f-cd*(h_R*v_R-h_L*v_L);



        LSMVector3 ret;

        //h_L = std::max(0.0001f,h_L);
        //h_R = std::max(0.0001f,h_R);


        //force ratio to approximate the influence of the wave-velocity in riemann solution
        float ff_dev_L = std::max(0.001f,ho_L/std::max(0.0001f,(h_L + ho_L)));
        float sf_dev_L = std::max(0.001f,h_L/std::max(0.0001f,(h_L + ho_L)));
        float dc_L = dragmult * sf_dev_L * ff_dev_L * (1.0/(std::max(0.0001f,srocksize)))* std::max(0.0f,sdensity-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float fac_dc_L = std::max(0.0f,1.0f - (4.0f *(1.0f-std::min(1.0f,std::max(std::max(0.0f,(ff_dev_L)),(exp(-(dc_L /ff_dev_L)) + ((sf_dev_L *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        float ff_dev_R = std::max(0.001f,ho_R/std::max(0.0001f,(h_R + ho_R)));
        float sf_dev_R = std::max(0.001f,h_R/std::max(0.0001f,(h_R + ho_R)));
        float dc_R = dragmult * sf_dev_R * ff_dev_R * (1.0f/(std::max(0.0001f,srocksize)))* std::max(0.0f,sdensity-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float fac_dc_R = std::max(0.0f,1.0f - (4.0f *(1.0f-std::min(1.0f,std::max(std::max(0.0f,(ff_dev_R)),(exp(-(dc_R /ff_dev_R)) + ((sf_dev_R *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));


        float dc = (dc_L * h_L + dc_R * h_R)/std::max(0.0001f,(h_L + h_R));
        float ff_tot = std::max(0.001f,std::max(h_L,h_R)/std::max(0.0001f,std::max(h_L + ho_L,h_R + ho_R)));

        ff_tot = std::max(0.0f,std::min(1.0f,(ff_tot -0.2f) * 10.0f));


        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(std::min(1.0f,std::max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(std::min(1.0f,std::max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L)) * ff_tot;// * fac_dc_L;  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R)) * ff_tot;// * fac_dc_R ;
        q_R = u_R*h_R;
        q_L = u_L*h_L;

        float sc1 = std::min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = std::max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
        float tmp = 1.0f/std::max((float)0.1f,(float)(sc2-sc1));
        float t1 = (std::min((float)(sc2),(float)(0.)) - std::min((float)(sc1),(float)(0.f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;

        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);


        //ret.x = ret.x * (1.0f- ff_tot) + ff_tot*retnw.x;
        //ret.y = ret.y * (1.0f- ff_tot) + ff_tot*retnw.y;
        //ret.z = ret.z * (1.0f- ff_tot) + ff_tot*retnw.z;

        //switch betweeen non-shock capturing gudanov and shock-capturing hll solver based on solid content
        return ret;
}



static inline float AS_GetVNFX(float v,float hn, float dt, float dx, float n,
                           float z, float z_x1, float z_x2, float z_y1, float z_y2,
                           float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
                           float us, float us_x1, float us_x2, float us_y1, float us_y2,
                           float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
                           float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
                           float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
                           float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
                           float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
                           float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
                           float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
                           float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{

        float vn = v;
        float vnold = v;

        float hsurf = z +hs + hf;
        float hsurfs = z +hs;
        float sx_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 + hf_x2-hsurf)/dx)));
        float sy_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - hf_y1)/dx)));
        float sx_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - hf_x1)/dx)));
        float sy_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 + hf_y2-hsurf)/dx)));

        float sx_zh = std::min(1.0f,std::max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
        float sy_zh = std::min(1.0f,std::max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));


        if(hs < 0.1f || (hs < 0.1f && hs_x1 < 0.01f && hs_x2 < 0.01f && hs_y1 < 0.01f && hs_y2 < 0.01f))
        {
                float acc_x = -0.5 * GRAV * sx_zh;

                vn = vn + acc_x * dt;

        }else
        {

                float dx2 =(dx*dx);

                float h_dev = std::max(0.001f,hs + hf);
                float ff_dev = std::max(0.001f,hf/std::max(0.001f,(hf + hs)));
                float sf_dev = std::max(0.001f,hs/std::max(0.001f,(hf + hs)));
                float ff = std::max(0.0f,hf/std::max(0.001f,(hf + hs)));
                float sf = std::max(0.0f,hs/std::max(0.001f,(hf + hs)));

                float ddemhdx = minmod((z_x2 + hs_x2 + hf_x2 - z + hs + hf)/dx,(z + hs + hf - z_x1 - hs_x1 - hf_x1)/dx);

                float ddfudxx = (uf_x2 - 2.0f * uf + uf_x1)/(dx2);
                float ddfvdxy = (vf_x1y1 + vf_x2y2 - vf_x1y2 - vf_x2y1)/(4.0f*dx2);
                float ddfudyy = (uf_y2 - 2.0f * uf + uf_y1)/(dx2);

                float sf_x1 = hs_x1/std::max(0.001f,(hf_x1 + hs_x1));
                float sf_x2 = hs_x2/std::max(0.001f,(hf_x2 + hs_x2));
                float sf_y1 = hs_y1/std::max(0.001f,(hf_y1 + hs_y1));
                float sf_y2 = hs_y2/std::max(0.001f,(hf_y2 + hs_y2));

                float dsfdx = 0.5f * (sf_x2 - sf_x1)/dx;
                float dsfdy = 0.5f * (hs_y2/std::max(0.001f,(hf_y2 + hs_y2)) - hs_y1/std::max(0.001f,(hf_y1 + hs_y1)))/dx;

                float dfudx = 0.5f*(uf_x2 - uf_x1)/dx;
                float dsudx = 0.5f*(us_x2 - us_x1)/dx;
                float dfvdy = 0.5f*(vf_y2 - vf_y1)/dx;
                float dsvdy = 0.5f*(vs_y2 - vs_y1)/dx;
                float dfudy = 0.5f*(uf_y2 - uf_y1)/dx;
                float dsudy = 0.5f*(us_y2 - us_y1)/dx;

                float ddsfdxx = (sf_x2 - 2.0f * sf_dev + sf_x1)/(dx2);
                float ddsfdxy = 0.0f;
                float ddsfdyy = (sf_y2 - 2.0f * sf_dev + sf_y1)/(dx2);

                float UF_Aspect = 1.0f;
                float UF_Ksi = 3.0f;
                float UF_Chi = 3.0f;
                float UF_j = 2.0f;
                float gamma = 1000.0f/density;
                float UT = 0.1f;
                float visc = std::min(100000.0f,std::max(1.0f,1.0f * exp(20.0f * std::max(0.0f,std::min(1.0f,sf_dev)))));
                float Nr = 10000.0f;
                float Nra = 10000.0f;
                float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
                float G = pow(ff_dev,2.5f);
                float P = sf_dev;
                float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
                float dc = dragmult * sf_dev * ff_dev * (1.0/(std::max(0.0001f,rocksize)))* std::max(0.0f,density-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
                float aspect = 1.0;


        float acc_x =
                                (-GRAV * sx_zh
                                                -UF_Aspect
                                                *(
                                                        -(1.0f/(ff_dev * Nr))*(
                                                                2.0f*ddfudxx + ddfvdxy + ddfudyy
                                                                )
                                                        +(1.0f/(ff_dev * Nra))*(
                                                                2.0f *(dsfdx*(dfudx - dsudx) + ddsfdxx*(uf - us))
                                                                +(dsfdx*(dfvdy - dsvdy) + ddsfdxy*(vf - vs))
                                                                +(dsfdy*(dfudy - dsudy) + ddsfdyy*(uf - us))
                                                                + UF_Chi * uf/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev)
                                                                )
                                                        -(UF_Ksi*sf_dev*(uf - us)/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev))
                                                        )
                                                );

                vn = vn + acc_x * dt;

                //drag
                float vpow = pow((vn - us) * (vn - us) + (vf - vs) * (vf - vs),UF_j - 1.0f);
                float lfacu = std::max(0.0f,dt * dc);
                float fac_dc = std::min(1.0f,std::max(0.0f,(exp(-(dc * vpow/ff_dev)) + ((sf_dev *density)/(1000.0f))*tan(ifa))/tan(ifa)));
                float v_balance = us;// (1.0f - fac_dc) *(ff * vn + sf * us) + (fac_dc) * us;
                vn = v_balance + (vn - v_balance) * exp(-lfacu);
        }

        {
            float vsq = sqrt((float)(uf * uf + vf * vf));
            float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
            float nsq = nsq1*vsq*dt;
            vn = (float)((vn/(1.0f+nsq)));
        }

        float threshold = 0.01 * dx;
        if(hn < threshold)
        {
            float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vn -vnold)/std::max(0.0001f,dt);

            float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * std::max(0.001f,sqrt(sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

            vn = kinfac * v_kin + vn*(1.0f-kinfac);

        }



        return vn;
}
static inline float AS_GetVNFY(float v,float hn, float dt, float dx, float n,
                           float z, float z_x1, float z_x2, float z_y1, float z_y2,
                           float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
                           float us, float us_x1, float us_x2, float us_y1, float us_y2,
                           float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
                           float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
                           float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
                           float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
                           float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
                           float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
                           float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
                           float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{
        float vn = v;
        float vnold = v;
        float hsurf = z +hs + hf;
        float hsurfs = z +hs;
        float sx_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 + hf_x2-hsurf)/dx)));
        float sy_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - hf_y1)/dx)));
        float sx_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - hf_x1)/dx)));
        float sy_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 + hf_y2-hsurf)/dx)));

        float sx_zh = std::min(1.0f,std::max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
        float sy_zh = std::min(1.0f,std::max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));

        if(hs < 0.1f || (hs < 0.1f && hs_x1 < 0.01f && hs_x2 < 0.01f && hs_y1 < 0.01f && hs_y2 < 0.01f))
        {
                float acc_y = -0.5 * GRAV * sy_zh;

                vn = vn + acc_y * dt;

        }else
        {

                float dx2 =(dx*dx);

                float h_dev = std::max(0.001f,hs + hf);
                float ff_dev = std::max(0.001f,hf/std::max(0.001f,(hf + hs)));
                float sf_dev = std::max(0.001f,hs/std::max(0.001f,(hf + hs)));
                float ff = std::max(0.0f,hf/std::max(0.001f,(hf + hs)));
                float sf = std::max(0.0f,hs/std::max(0.001f,(hf + hs)));

                float ddfvdxx = (vf_x2 - 2.0f * vf + vf_x1)/(dx2);
                float ddfudxy = (uf_x1y1 + uf_x2y2 - uf_x1y2 - uf_x2y1)/(4.0f*dx2);
                float ddfvdyy = (vf_y2 - 2.0f * vf + vf_y1)/(dx2);

                float sf_x1 = hs_x1/std::max(0.001f,(hf_x1 + hs_x1));
                float sf_x2 = hs_x2/std::max(0.001f,(hf_x2 + hs_x2));
                float sf_y1 = hs_y1/std::max(0.001f,(hf_y1 + hs_y1));
                float sf_y2 = hs_y2/std::max(0.001f,(hf_y2 + hs_y2));

                float dsfdx = 0.5f * (sf_x2 - sf_x1)/dx;
                float dsfdy = 0.5f * (hs_y2/std::max(0.001f,(hf_y2 + hs_y2)) - hs_y1/std::max(0.001f,(hf_y1 + hs_y1)))/dx;

                float dfvdy = 0.5f*(vf_y2 - vf_y1)/dx;
                float dsvdy = 0.5f*(vs_y2 - vs_y1)/dx;
                float dfvdx = 0.5f*(vf_x2 - vf_x1)/dx;
                float dsvdx = 0.5f*(vs_x2 - vs_x1)/dx;
                float dfudx = 0.5f*(uf_x2 - uf_x1)/dx;
                float dsudx = 0.5f*(us_x2 - us_x1)/dx;

                float ddsfdxx = (sf_x2 - 2.0f * sf_dev + sf_x1)/(dx2);
                float ddsfdxy = 0.0f;
                float ddsfdyy = (sf_y2 - 2.0f * sf_dev + sf_y1)/(dx2);

                float UF_Aspect = 1.0f;
                float UF_Ksi = 3.0f;
                float UF_Chi = 3.0f;
                float UF_j = 2.0f;
                float gamma = 1000.0f/density;
                float UT = 0.1f;
                float visc = std::min(100000.0f,std::max(1.0f,1.0f * exp(20.0f * std::max(0.0f,std::min(1.0f,sf_dev)))));
                float Nr = 10000.0f;
                float Nra = 10000.0f;
                float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
                float G = pow(ff_dev,2.5f);
                float P = sf_dev;
                float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
                float dc = dragmult * sf_dev * ff_dev * (1.0/(std::max(0.0001f,rocksize)))* std::max(0.0f,density-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
                float aspect = 1.0;


        float acc_y =
                                (-GRAV * sy_zh
                                                -UF_Aspect
                                                *(
                                                        -(1.0f/(ff_dev * Nr))*(
                            2.0f*ddfvdyy + ddfudxy + ddfvdxx
                            )
                        +(1.0f/(ff_dev * Nra))*(
                            2.0f *(dsfdy*(dfvdy - dsvdy) + ddsfdyy*(vf - vs))
                            +(dsfdy*(dfudx - dsudx) + ddsfdxy*(uf - us))
                            +(dsfdx*(dfvdx - dsvdx) + ddsfdxx*(vf - vs))
                            + UF_Chi * vf/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev)
                            )
                        -(UF_Ksi*sf*(vf - vs)/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev))
                        )
                    );

                vn = vn + acc_y * dt;

                //drag
                float vpow = pow((vn - vs) * (vn - vs) + (uf - us) * (uf - us),UF_j - 1.0f);
                float lfacv = std::max(0.0f,dt * dc);
                float fac_dc = std::min(1.0f,std::max(0.0f,(exp(-(dc * vpow/ff_dev)) + ((sf_dev *density)/(1000.0f))*tan(ifa))/tan(ifa)));
                float v_balance = vs;//(1.0f - fac_dc) *(ff * vn + sf * vs) + (fac_dc) * vs;
                vn = v_balance + (vn - v_balance) * exp(-lfacv);
        }

        {

            float vsq = sqrt((float)(uf * uf + vf * vf));
            float nsq1 = (0.001f+n)*(0.001f+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0f/3.0f)));
            float nsq = nsq1*vsq*dt;
            vn = (float)((vn/(1.0f+nsq)));
        }

        float threshold = 0.01f * dx;
        if(hn < threshold)
        {
            float kinfac = std::max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vn -vnold)/std::max(0.0001f,dt);

            float v_kin = (sy_zh>0.0f?1.0f:-1.0f) * hn * sqrt(hn) * std::max(0.001f,sqrt(sy_zh>0.0f?sy_zh:-sy_zh))/(0.001f+n);

            vn = kinfac * v_kin + vn*(1.0f-kinfac);

        }


        return vn;
}

static inline float AS_GetVNSX(float v,float hn, float dt, float dx, float n,
                           float z, float z_x1, float z_x2, float z_y1, float z_y2,
                           float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
                           float us, float us_x1, float us_x2, float us_y1, float us_y2,
                           float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
                           float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
                           float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
                           float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
                           float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
                           float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
                           float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
                           float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{

        if(hs < 0.0001f)
        {
                return 0.0f;
        }

        if(ifa > 1.5f)
        {
            ifa = 1.5f;
        }

        float vn = v;

        float dx2 =(dx*dx);


        float hsurf = z +hs + hf;
        float hsurfs = z +hs;

        float sxs_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
        float sys_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
        float sxs_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
        float sys_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));

        float sxs_zh = std::min(1.0f,std::max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
        float sys_zh = std::min(1.0f,std::max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));

        float h_dev = std::max(0.001f,hs + hf);
        float ff_dev = std::max(0.001f,hf/std::max(0.001f,(hf + hs)));
        float sf_dev = std::max(0.001f,hs/std::max(0.001f,(hf + hs)));
        float ff = std::max(0.0f,hf/std::max(0.001f,(hf + hs)));
        float sf = std::max(0.0f,hs/std::max(0.001f,(hf + hs)));

        float ddemhdx = minmod(z_x2 + hs_x2 - z + hs,z + hs - z_x1 - hs_x1)/dx;
        float SlopeX = minmod(z_x2 - z ,z- z_x1)/dx;
        float dhdx = minmod(hs_x2 - hs ,hs- hs_x1)/dx;


        float UF_Aspect = 1.0f;
        float UF_Ksi = 3.0f;
        float UF_Chi = 3.0f;
        float UF_j = 2.0f;
        float gamma = 1000.0f/density;
        float UT = 0.1f;
        float visc = std::min(100000.0f,std::max(1.0f,1.0f * exp(20.0f * std::max(0.0f,std::min(1.0f,sf_dev)))));
        float Nr = 10000.0f;
        float Nra = 10000.0f;
        float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
        float G = pow(ff_dev,2.5f);
        float P = sf_dev;
        float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
        float dc = dragmult * sf_dev * ff_dev *  (1.0f/(std::max(0.0001f,rocksize)))*std::max(0.0f,density-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float pbf = -GRAV;
        float pbs = (1.0f-gamma) * pbf;

    float acc_x =(-GRAV * sxs_zh + UF_Aspect*pbs*( dhdx)
                                        +UF_Aspect * gamma * pbf * (SlopeX + dhdx)
                                         );

        vn = vn + acc_x * dt;

    //drag
    float vpow = pow((vn - uf) * (vn - uf) + (vf - vs) * (vf - vs),UF_j - 1.0f);
    float lfacv = std::max(0.0f,dt * dc);
    float v_balance = ff * uf + sf * vn;
    vn = v_balance + (vn - v_balance) * exp(-lfacv);

    float vsq = sqrt((float)(us * us + vs * vs));
    float nsq1 = 0.1f * (0.001f+n)*(0.001f+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0f/3.0f)));
    float nsq = nsq1*vsq*dt;
    vn = (float)((vn/(1.0f+nsq)));

    float vn_ifa = vn > 0.0f? std::max(0.0f,(float)(vn - std::max(0.0f,(sf - ff * (1000.0f/density))) * dt *GRAV * tan(ifa))) : std::min(0.0f,(float)(vn + std::max(0.0f,(sf - ff * (1000.0f/density))) * dt * GRAV * tan(ifa)));
    float fac_dc = std::min(1.0f,std::max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
    vn = vn_ifa * fac_dc + (1.0f-fac_dc) * vn_ifa;

    return vn;
}

static inline float AS_GetVNSY(float v,float hn, float dt, float dx, float n,
                           float z, float z_x1, float z_x2, float z_y1, float z_y2,
                           float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
                           float us, float us_x1, float us_x2, float us_y1, float us_y2,
                           float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
                           float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
                           float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
                           float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
                           float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
                           float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
                           float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
                           float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{
        if(hs < 0.0001f)
        {
                return 0.0f;
        }
        if(ifa > 1.5f)
        {
            ifa = 1.5f;
        }
        float hsurf = z +hs + hf;
        float hsurfs = z +hs;

        float sxs_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
        float sys_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
        float sxs_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
        float sys_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));

        float sxs_zh = std::min(1.0f,std::max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
        float sys_zh = std::min(1.0f,std::max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));

        float vn = v;

        float dx2 =(dx*dx);

        float h_dev = std::max(0.001f,hs + hf);
        float ff_dev = std::max(0.001f,hf/std::max(0.001f,(hf + hs)));
        float sf_dev = std::max(0.001f,hs/std::max(0.001f,(hf + hs)));
        float ff = std::max(0.0f,hf/std::max(0.001f,(hf + hs)));
        float sf = std::max(0.0f,hs/std::max(0.001f,(hf + hs)));

        float SlopeY = minmod(z_y2 - z ,z- z_y1)/dx;
        float dhdy = minmod(hs_y2 - hs ,hs- hs_y1)/dx;


        float UF_Aspect = 1.0f;
        float UF_Ksi = 3.0f;
        float UF_Chi = 3.0f;
        float UF_j = 2.0f;
        float gamma = 1000.0f/density;
        float UT = 0.1f;
        float visc = std::min(100000.0f,std::max(1.0f,1.0f * exp(20.0f * std::max(0.0f,std::min(1.0f,sf_dev)))));
        float Nr = 10000.0f;
        float Nra = 10000.0f;
        float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
        float G = pow(ff_dev,2.5f);
        float P = sf_dev;
        float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
        float dc = dragmult * sf_dev * ff_dev * (1.0/(std::max(0.0001f,rocksize)))* std::max(0.0f,density-1000.0f)/std::max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/std::max(1.0f,density)))/den : 0.0f;
        float pbf = -GRAV;
        float pbs = (1.0-gamma) * pbf;

    float acc_y =(-GRAV * sys_zh + UF_Aspect*pbs*( dhdy)
                                        +UF_Aspect * gamma * pbf * (SlopeY + dhdy)
                                         );

        vn = vn + acc_y * dt;

        //drag
        float vpow = pow((vn - vf) * (vn - vf) + (uf - us) * (uf - us),UF_j - 1.0f);
        float lfacv = std::max(0.0f,dt * dc);
        float v_balance = ff * vf  + sf * vn;
        vn = v_balance + (vn - v_balance) * exp(-lfacv);

        float vsq = sqrt((float)(us * us + vs * vs));
        float nsq1 = (0.001+n)*(0.001+n)*GRAV/std::max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
        float nsq = nsq1*vsq*dt;
        vn = (float)((vn/(1.0f+nsq)));

        float vn_ifa = vn > 0? std::max(0.0f,(float)(vn - std::max(0.0f,(sf - ff * (1000.0f/density))) * dt *GRAV * tan(ifa))) : std::min(0.0f,(float)(vn + std::max(0.0f,(sf - ff * (1000.0f/density))) * dt * GRAV * tan(ifa)));
        float fac_dc = std::min(1.0f,std::max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
        vn = vn_ifa * fac_dc + (1.0f-fac_dc) * vn_ifa;
        return vn;
}

static inline float flow_pudasainidt(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY, cTMap * HS,cTMap * VXS,cTMap * VYS)
{

    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();
    float dx = DEM->cellSize();

    float dt_min = 1e6;

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {

            int x = r;
            int y = c;

            const int gy = std::min(dim1-(int)(1),std::max((int)(0),(int)(x)));
            const int gx = std::min(dim0-(int)(1),std::max((int)(0),(int)(y)));

            float vmax = 100.0;

            float vx = std::min(vmax,std::max(-vmax,VX->data[gy][gx]));
            float vy = std::min(vmax,std::max(-vmax,VY->data[gy][gx]));
            float vxs = std::min(vmax,std::max(-vmax,VXS->data[gy][gx]));
            float vys = std::min(vmax,std::max(-vmax,VYS->data[gy][gx]));

            float dt_req = 0.25f *dx/( std::min(100.0f,std::max(0.01f,(sqrt(vx*vx + vy * vy)))));
            float dt_reqs = 0.25f *dx/( std::min(100.0f,std::max(0.01f,(sqrt(vxs*vxs + vys * vys)))));
            dt_min = std::min(dt_min,std::min(dt_reqs,dt_req));

        }
    }

    return dt_min;
}




static inline void flow_pudasaini(cTMap * DEM,cTMap * N,cTMap * H,cTMap * VX,cTMap * VY,cTMap * HS,cTMap * VXS,cTMap * VYS, cTMap * IFA, cTMap * RS, cTMap * D, cTMap * HN,cTMap * VXN,cTMap * VYN, cTMap * HSN,cTMap * VXSN,cTMap * VYSN, cTMap * IFAN, cTMap * RSN, cTMap * DN,cTMap * QX1 = nullptr, cTMap * QX2 = nullptr,cTMap * QY1 = nullptr,cTMap * QY2 = nullptr, float dt = 0.1, float dragmult = 1.0)
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

                float vmax = 0.1f * dx/dt;

                float h = std::max(0.0f,H->data[gy][gx]);
                float h_x1 = std::max(0.0f,H->data[gy][gx_x1]);
                float h_x2 = std::max(0.0f,H->data[gy][gx_x2]);
                float h_y1 = std::max(0.0f,H->data[gy_y1][gx]);
                float h_y2 = std::max(0.0f,H->data[gy_y2][gx]);

                float hs = std::max(0.0f,HS->data[gy][gx]);
                float hs_x1 = std::max(0.0f,HS->data[gy][gx_x1]);
                float hs_x2 = std::max(0.0f,HS->data[gy][gx_x2]);
                float hs_y1 = std::max(0.0f,HS->data[gy_y1][gx]);
                float hs_y2 = std::max(0.0f,HS->data[gy_y2][gx]);

                float sifa = std::min(3.0f,std::max(0.00001f,IFA->data[gy][gx]));
                float srocksize = std::min(100.0f,std::max(0.00001f,RS->data[gy][gx]));
                float sdensity = std::min(3000.0f,std::max(1000.00f,D->data[gy][gx]));

                float sifa_x1 = IFA->data[gy][gx_x1];
                float sifa_x2 = IFA->data[gy][gx_x2];
                float sifa_y1 = IFA->data[gy_y1][gx];
                float sifa_y2 = IFA->data[gy_y2][gx];
                float srocksize_x1 = RS->data[gy][gx_x1];
                float srocksize_x2 = RS->data[gy][gx_x2];
                float srocksize_y1 = RS->data[gy_y1][gx];
                float srocksize_y2 = RS->data[gy_y2][gx];
                float sdensity_x1 = D->data[gy][gx_x1];
                float sdensity_x2 = D->data[gy][gx_x2];
                float sdensity_y1 = D->data[gy_y1][gx];
                float sdensity_y2 = D->data[gy_y2][gx];

                float vsx = std::min(vmax,std::max(-vmax,VXS->data[gy][gx]));
                float vsy = std::min(vmax,std::max(-vmax,VYS->data[gy][gx]));

                float vsx_x1 = std::min(vmax,std::max(-vmax,VXS->data[gy][gx_x1]));
                float vsy_x1 = std::min(vmax,std::max(-vmax,VYS->data[gy][gx_x1]));
                float vsx_x2 = std::min(vmax,std::max(-vmax,VXS->data[gy][gx_x2]));
                float vsy_x2 = std::min(vmax,std::max(-vmax,VYS->data[gy][gx_x2]));
                float vsx_y1 = std::min(vmax,std::max(-vmax,VXS->data[gy_y1][gx]));
                float vsy_y1 = std::min(vmax,std::max(-vmax,VYS->data[gy_y1][gx]));
                float vsx_y2 = std::min(vmax,std::max(-vmax,VXS->data[gy_y2][gx]));
                float vsy_y2 = std::min(vmax,std::max(-vmax,VYS->data[gy_y2][gx]));

                float vsx_x1y1 = std::min(vmax,std::max(-vmax,VXS->data[gy_y1][gx_x1]));
                float vsy_x1y1 = std::min(vmax,std::max(-vmax,VYS->data[gy_y1][gx_x1]));
                float vsx_x2y2 = std::min(vmax,std::max(-vmax,VXS->data[gy_y2][gx_x2]));
                float vsy_x2y2 = std::min(vmax,std::max(-vmax,VYS->data[gy_y2][gx_x2]));
                float vsx_x1y2 = std::min(vmax,std::max(-vmax,VXS->data[gy_y2][gx_x1]));
                float vsy_x1y2 = std::min(vmax,std::max(-vmax,VYS->data[gy_y2][gx_x1]));
                float vsx_x2y1 = std::min(vmax,std::max(-vmax,VXS->data[gy_y1][gx_x2]));
                float vsy_x2y1 = std::min(vmax,std::max(-vmax,VYS->data[gy_y1][gx_x2]));

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

                float vx_x1y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx_x1]));
                float vy_x1y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx_x1]));
                float vx_x2y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx_x2]));
                float vy_x2y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx_x2]));
                float vx_x1y2 = std::min(vmax,std::max(-vmax,VX->data[gy_y2][gx_x1]));
                float vy_x1y2 = std::min(vmax,std::max(-vmax,VY->data[gy_y2][gx_x1]));
                float vx_x2y1 = std::min(vmax,std::max(-vmax,VX->data[gy_y1][gx_x2]));
                float vy_x2y1 = std::min(vmax,std::max(-vmax,VY->data[gy_y1][gx_x2]));

                float hsurf = z +hs + h;
                float hsurfs = z +hs;
                float sx_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 + h_x2-hsurf)/dx)));
                float sy_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - h_y1)/dx)));
                float sx_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - h_x1)/dx)));
                float sy_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 + h_y2-hsurf)/dx)));

                float sx_zh = std::min(1.0f,std::max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
                float sy_zh = std::min(1.0f,std::max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));

                float sxs_zh_x2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
                float sys_zh_y1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
                float sxs_zh_x1 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
                float sys_zh_y2 = std::min((float)(0.5f),std::max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));

                float sxs_zh = std::min(1.0f,std::max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
                float sys_zh = std::min(1.0f,std::max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));

                float C = 0.1f;
                float err = 0.0000f;


                LSMVector3 hlls_x1 = z_x1 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2SF(std::max(0.0f,hs_x1 -std::max(0.0f,z - z_x1)),std::max(0.0f,h_x1 -std::max(0.0f,z - z_x1)),vsx_x1,vsy_x1,std::max(0.0f,hs -std::max(0.0f,z_x1-z)) ,std::max(0.0f,h -std::max(0.0f,z_x1-z)),vsx,vsy,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hlls_x2 = z_x2 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2SF(std::max(0.0f,hs -std::max(0.0f,z_x2 - z)),std::max(0.0f,h -std::max(0.0f,z_x2 - z)),vsx,vsy,std::max(0.0f,hs_x2 -std::max(0.0f,z - z_x2)),std::max(0.0f,h_x2 -std::max(0.0f,z - z_x2)),vsx_x2,vsy_x2,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hlls_y1 = z_y1 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2SF(std::max(0.0f,hs_y1 -std::max(0.0f,z - z_y1)),std::max(0.0f,h_y1 -std::max(0.0f,z - z_y1)),vsy_y1,vsx_y1,std::max(0.0f,hs -std::max(0.0f,z_y1 - z)),std::max(0.0f,h -std::max(0.0f,z_y1 - z)),vsy,vsx,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hlls_y2 = z_y2 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2SF(std::max(0.0f,hs -std::max(0.0f,z_y2 - z)),std::max(0.0f,h -std::max(0.0f,z_y2 - z)),vsy,vsx,std::max(0.0f,hs_y2 -std::max(0.0f,z - z_y2)),std::max(0.0f,h_y2 -std::max(0.0f,z - z_y2)),vsy_y2,vsx_y2,sifa, srocksize, sdensity, dragmult);

                //float3 hlls_x1 = z_x1 < -1000? LSMVector3(0.0f,0.0f,0.0f):F_HLL2SF(hs_x1,h_x1,vsx_x1,vsy_x1,hs,h,vsx,vsy);
                //float3 hlls_x2 = z_x2 < -1000? LSMVector3(0.0f,0.0f,0.0f):F_HLL2SF(hs,h,vsx,vsy,hs_x2,h_x2,vsx_x2,vsy_x2);
                //float3 hlls_y1 = z_y1 < -1000? LSMVector3(0.0f,0.0f,0.0f):F_HLL2SF(hs_y1,h_y1,vsy_y1,vsx_y1,hs,h,vsy,vsx);
                //float3 hlls_y2 = z_y2 < -1000? LSMVector3(0.0f,0.0f,0.0f):F_HLL2SF(hs,h,vsy,vsx,hs_y2,h_y2,vsy_y2,vsx_y2);
                float fluxs_x1 = z_x1 < -99995? std::max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):std::max(-std::max(err,hs) * C,std::min(std::max(err,hs_x1) * C,(float)(+tx*(hlls_x1.x))));
                float fluxs_x2 = z_x2 < -99995? std::max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):std::max(-std::max(err,hs) * C,std::min(std::max(err,hs_x2) * C,(float)(-tx*(hlls_x2.x))));
                float fluxs_y1 = z_y1 < -99995? std::max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):std::max(-std::max(err,hs) * C,std::min(std::max(err,hs_y1) * C,(float)(+tx*(hlls_y1.x))));
                float fluxs_y2 = z_y2 < -99995? std::max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):std::max(-std::max(err,hs) * C,std::min(std::max(err,hs_y2) * C,(float)(-tx*(hlls_y2.x))));

                float hsold = hs;
                float hsn = ((std::max(0.00f,(float)(hs + fluxs_x1 + fluxs_x2 + fluxs_y1 + fluxs_y2))));
                float a_sx = 0.5f * GRAV *sx_zh;
                float a_sy = 0.5f * GRAV *sy_zh;
                float qsxn = hs * vsx - tx*(hlls_x2.y - hlls_x1.y) - tx*(hlls_y2.z - hlls_y1.z);
                float qsyn = hs * vsy - tx*(hlls_x2.z - hlls_x1.z) - tx*(hlls_y2.y - hlls_y1.y);
                float vsxn = (float)((qsxn))/std::max(0.01f,(float)(hsn));
                float vsyn = (float)((qsyn))/std::max(0.01f,(float)(hsn));

                vsxn = AS_GetVNSX(vsxn, hsn,dt, dx, n,
                                   z, z_x1, z_x2, z_y1, z_y2,
                                   hs, hs_x1, hs_x2, hs_y1, hs_y2,
                                   vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                                   vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                                   vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                                   vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                                   h, h_x1, h_x2, h_y1, h_y2,
                                   vx, vx_x1, vx_x2, vx_y1, vx_y2,
                                   vy, vy_x1, vy_x2, vy_y1, vy_y2,
                                   vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                                   vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                                   sifa, srocksize, sdensity, dragmult);

                vsyn = AS_GetVNSY(vsyn,hsn, dt, dx, n,
                                   z, z_x1, z_x2, z_y1, z_y2,
                                   hs, hs_x1, hs_x2, hs_y1, hs_y2,
                                   vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                                   vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                                   vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                                   vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                                   h, h_x1, h_x2, h_y1, h_y2,
                                   vx, vx_x1, vx_x2, vx_y1, vx_y2,
                                   vy, vy_x1, vy_x2, vy_y1, vy_y2,
                                   vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                                   vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                                   sifa, srocksize, sdensity, dragmult);

                float ff_dev_dc = std::max(0.001f,h/std::max(0.001f,(h + hs)));
                float sf_dev_dc = std::max(0.001f,hs/std::max(0.001f,(h + hs)));

                float dc = dragmult * ff_dev_dc * sf_dev_dc * (1.0f/(std::max(0.0001f,srocksize)))* std::max(0.0f,sdensity-1000.0f)/std::max(0.1f,1000.0f);

                LSMVector3 hll_x1 = z_x1 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2FS(std::max(0.0f,h_x1 -std::max(0.0f,z - z_x1)),std::max(0.0f,hs_x1 -std::max(0.0f,z - z_x1)),vx_x1,vy_x1,std::max(0.0f,h -std::max(0.0f,z_x1-z)) ,std::max(0.0f,hs -std::max(0.0f,z_x1-z)),vx,vy,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hll_x2 = z_x2 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2FS(std::max(0.0f,h -std::max(0.0f,z_x2 - z)),std::max(0.0f,hs -std::max(0.0f,z_x2 - z)),vx,vy,std::max(0.0f,h_x2 -std::max(0.0f,z - z_x2)),std::max(0.0f,hs_x2 -std::max(0.0f,z - z_x2)),vx_x2,vy_x2,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hll_y1 = z_y1 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2FS(std::max(0.0f,h_y1 -std::max(0.0f,z - z_y1)),std::max(0.0f,hs_y1 -std::max(0.0f,z - z_y1)),vy_y1,vx_y1,std::max(0.0f,h -std::max(0.0f,z_y1 - z)),std::max(0.0f,hs -std::max(0.0f,z_y1 - z)),vy,vx,sifa, srocksize, sdensity, dragmult);
                LSMVector3 hll_y2 = z_y2 < -99995? LSMVector3(0.0f,0.0f,0.0f):ASF_HLL2FS(std::max(0.0f,h -std::max(0.0f,z_y2 - z)),std::max(0.0f,hs -std::max(0.0f,z_y2 - z)),vy,vx,std::max(0.0f,h_y2 -std::max(0.0f,z - z_y2)),std::max(0.0f,hs_y2 -std::max(0.0f,z - z_y2)),vy_y2,vx_y2,sifa, srocksize, sdensity, dragmult);
                float flux_x1 = z_x1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):std::max(-std::max(err,h) * C,std::min(std::max(err,h_x1) * C,(float)(+tx*(hll_x1.x))));
                float flux_x2 = z_x2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):std::max(-std::max(err,h) * C,std::min(std::max(err,h_x2) * C,(float)(-tx*(hll_x2.x))));
                float flux_y1 = z_y1 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):std::max(-std::max(err,h) * C,std::min(std::max(err,h_y1) * C,(float)(+tx*(hll_y1.x))));
                float flux_y2 = z_y2 < -99995? std::max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):std::max(-std::max(err,h) * C,std::min(std::max(err,h_y2) * C,(float)(-tx*(hll_y2.x))));



                float hold = h;
                float hn = ((std::max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));
                float a_fx = 0.5f * GRAV *sx_zh;
                float a_fy = 0.5f * GRAV *sy_zh;
                float qxn = h * vx - tx*(hll_x2.y - hll_x1.y) - tx*(hll_y2.z - hll_y1.z);
                float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y);
                float vxn = (float)((qxn))/std::max(0.01f,(float)(hn));
                float vyn = (float)((qyn))/std::max(0.01f,(float)(hn));

                vxn = AS_GetVNFX(vxn,hn, dt, dx, n,
                                   z, z_x1, z_x2, z_y1, z_y2,
                                   hs, hs_x1, hs_x2, hs_y1, hs_y2,
                                   vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                                   vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                                   vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                                   vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                                   h, h_x1, h_x2, h_y1, h_y2,
                                   vx, vx_x1, vx_x2, vx_y1, vx_y2,
                                   vy, vy_x1, vy_x2, vy_y1, vy_y2,
                                   vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                                   vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                                   sifa, srocksize, sdensity, dragmult);

                vyn = AS_GetVNFY(vyn,hn, dt, dx, n,
                                   z, z_x1, z_x2, z_y1, z_y2,
                                   hs, hs_x1, hs_x2, hs_y1, hs_y2,
                                   vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                                   vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                                   vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                                   vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                                   h, h_x1, h_x2, h_y1, h_y2,
                                   vx, vx_x1, vx_x2, vx_y1, vx_y2,
                                   vy, vy_x1, vy_x2, vy_y1, vy_y2,
                                   vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                                   vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                                   sifa, srocksize, sdensity, dragmult);


                int edges = ((z_x1 < -1000)?1:0) +((z_x2 < -1000)?1:0)+((z_y1 < -1000)?1:0)+((z_y2 < -1000)?1:0);

                bool edge = (z_x1 < -1000 || z_x2 < -1000 || z_y1 < -1000 || z_y2 < -1000);


                HSN->data[gy][gx] = edges > 2? 0.0f:isnan(hsn)? 0.0f:hsn;
                VXSN->data[gy][gx] = edges > 2? 0.0f:isnan(vsxn)? 0.0f:vsxn;
                VYSN->data[gy][gx] = edges > 2? 0.0f:isnan(vsyn)? 0.0f:vsyn;
                HN->data[gy][gx] = edges > 2? 0.0f:isnan(hn)? 0.0f:hn;
                VXN->data[gy][gx] = edges > 2? 0.0f:isnan(vxn)? 0.0f:vxn;
                VYN->data[gy][gx] = edges > 2? 0.0f:isnan(vyn)? 0.0f:vyn;

                float hsleft = hs + std::min(0.0f,fluxs_x1) +std::min(0.0f,fluxs_x2) + std::min(0.0f,fluxs_y1) + std::min(0.0f,fluxs_y2);
                float sifan = (sifa * hsleft + std::max(0.0f,fluxs_x1) * sifa_x1 + std::max(0.0f,fluxs_x2) * sifa_x2 + std::max(0.0f,fluxs_y1) * sifa_y1 + std::max(0.0f,fluxs_y2) * sifa_y2)/(std::max(0.0001f,hsn));
                float srocksizen = (srocksize * hsleft + std::max(0.0f,fluxs_x1) * srocksize_x1 + std::max(0.0f,fluxs_x2) * srocksize_x2 + std::max(0.0f,fluxs_y1) * srocksize_y1 + std::max(0.0f,fluxs_y2) * srocksize_y2)/(std::max(0.0001f,hsn));
                float sdensityn = (sdensity * hsleft + std::max(0.0f,fluxs_x1) * sdensity_x1 + std::max(0.0f,fluxs_x2) * sdensity_x2 + std::max(0.0f,fluxs_y1) * sdensity_y1 + std::max(0.0f,fluxs_y2) * sdensity_y2)/(std::max(0.0001f,hsn));

                IFAN->data[r][c] = isnan(sifan)? 0.3f : sifan;
                RSN->data[r][c] = isnan(srocksizen)? 0.1f : srocksizen;
                DN->data[r][c] = isnan(sdensityn)? 2000.0f : sdensityn;


            }
        }
    }
    return;
}

inline std::vector<cTMap*> AS_DebrisWave(cTMap * DEM,cTMap * N,cTMap * H, cTMap * VX, cTMap * VY, cTMap * HS, cTMap * VXS, cTMap * VYS, cTMap * IFA, cTMap * RS, cTMap * D,  float dragmult, float _dt, float courant)
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
    cTMap * HSN = HS->GetCopy();
    cTMap * VXSN = VXS->GetCopy();
    cTMap * VYSN = VYS->GetCopy();
    cTMap * IFAN = IFA->GetCopy();
    cTMap * RSN = RS->GetCopy();
    cTMap * DN = D->GetCopy();

    double t = 0;
    double t_end = _dt;


    int iter = 0;
    while(t < t_end)
    {

        //first update flow heights
        double dt = flow_pudasainidt(DEM,N,H,VX,VY,HS,VXS,VYS);

        dt = std::min(t_end - t,std::max(dt,1e-6));

        //then update fluxes
        flow_pudasaini(DEM,N,H,VX,VY,HS,VXS,VYS,IFA,RS,D,HN,VXN,VYN,HSN,VXSN,VYSN,IFAN,RSN,DN,nullptr,nullptr,nullptr,nullptr,dt, dragmult);

        t = t+dt;
        iter ++;

        for(int r = 0; r < H->nrRows(); r++)
        {
            for(int c = 0; c < H->nrCols(); c++)
            {
                H->Drc = HN->Drc;
                VX->Drc = VXN->Drc;
                VY->Drc = VYN->Drc;
                HS->Drc = HSN->Drc;
                VXS->Drc = VXSN->Drc;
                VYS->Drc = VYSN->Drc;
                IFA->Drc = IFAN->Drc;
                RS->Drc = RSN->Drc;
                D->Drc = DN->Drc;


            }
        }
    }



    return {HN,VXN,VYN, HSN,VXSN,VYSN,IFAN,RSN,DN};
}


#endif // RASTERMIXTUREFLOW_H
