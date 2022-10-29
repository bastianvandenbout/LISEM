#ifndef RASTERFASTWAVE_H
#define RASTERFASTWAVE_H

#include "geo/raster/map.h"
#include "rastercommon.h"

#include "lisemmath.h"

// https://en.wikipedia.org/wiki/Eikonal_equation
inline float eikonal1d(float h, float v, float g)
{
    float hv = absMin(h, v);
    return hv + g * negSign(hv);
}

inline float eikonal2d(float h, float v, float g)
{
    float hv = h + v;
    float d = hv*hv - 2.0 * (h*h + v*v - g*g);
    if (d < g*g) return 1e30f;
    return 0.5 * (hv + sqrt(d) * negSign(hv));
}


inline cTMap * AS_EikonalSolve(cTMap * mask, cTMap * cw, int iter_max)
{
    if(iter_max <= 0)
    {
        iter_max = std::max(mask->nrRows(),mask->nrCols());
    }

    cTMap * res = mask->GetCopy0();
    cTMap * resN = mask->GetCopy0();

    bool stop =false;
    int i = 0;
    int i2 = 0;

    #pragma omp for collapse(2)
    for(int r = 0; r < res->data.nr_rows();r++)
    {
        for(int c = 0; c < res->data.nr_cols();c++)
        {

            if(!pcr::isMV(res->data[r][c]))
            {
                if(mask->data[r][c] >= 0.5f)
                {
                    res->data[r][c] = 0.0f;
                    resN->data[r][c] = 0.0f;
                }else
                {

                    res->data[r][c] = 1e30f;
                    resN->data[r][c] = 1e30f;
                }
            }

        }
    }

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(res->cellSizeX());
        double _dy = std::fabs(res->cellSizeY());




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
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < res->data.nr_rows();r++)
            {
                for(int c = 0; c < res->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(res->data[r][c]))
                    {
                        float dis = res->data[r][c];
                        float dis_x1 = OUTORMV(res,r,c-1)?dis:res->data[r][c-1];
                        float dis_x2 = OUTORMV(res,r,c+1)?dis:res->data[r][c+1];
                        float dis_y1 = OUTORMV(res,r-1,c)?dis:res->data[r-1][c];
                        float dis_y2 = OUTORMV(res,r+1,c)?dis:res->data[r+1][c];

                        float h = absMin(dis_x1,dis_x2);
                        float v = absMin(dis_y1,dis_y2);

                        float cur = dis;

                        float e1 = eikonal1d(h,v,cw->data[r][c]);
                        float e2 = eikonal2d(h,v,cw->data[r][c]);

                        float newv = absMin(e1,e2);
                        resN->data[r][c] = absMin(newv,cur);

                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < res->data.nr_rows();r++)
            {
                for(int c = 0; c < res->data.nr_cols();c++)
                {
                    if(!pcr::isMV(res->data[r][c]))
                    {
                       res->data[r][c] = resN->data[r][c];
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }

    float maxdist = 0.0;
    for(int r = 0; r < res->data.nr_rows();r++)
    {
        for(int c = 0; c < res->data.nr_cols();c++)
        {
            if(!pcr::isMV(res->data[r][c]))
            {

                if(res->data[r][c] < 0.9e30f)
                {
                    maxdist = std::max(maxdist,res->data[r][c]);
                }
            }
        }
    }

    #pragma omp for collapse(2)
    for(int r = 0; r < res->data.nr_rows();r++)
    {
        for(int c = 0; c < res->data.nr_cols();c++)
        {
            if(res->data[r][c] >= 1e30f)
            {
               res->data[r][c] = maxdist;
            }
        }
    }


    delete res;
    return resN;
}


inline cTMap * AS_WaveEnergySpread(cTMap * Source, cTMap * ux1, cTMap * ux2, cTMap * uy1, cTMap * uy2, int iter_max)
{
    cTMap * val = Source->GetCopy0();
    cTMap * valn = Source->GetCopy0();

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {

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
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));



            #pragma omp for collapse(2)
           for(int r = 0; r < val->data.nr_rows();r++)
            {
                for(int c = 0; c < val->data.nr_cols();c++)
                {
                    if(pcr::isMV(val->data[r][c]))
                    {
                    }else
                    {
                        float ux1h =  !OUTORMV(ux1,r,c)? ux1->data[r][c] : 0.0;
                        float ux2h =  !OUTORMV(ux1,r,c)? ux2->data[r][c] : 0.0;
                        float uy1h =  !OUTORMV(ux1,r,c)? uy1->data[r][c] : 0.0;
                        float uy2h =  !OUTORMV(ux1,r,c)? uy2->data[r][c] : 0.0;


                        float ux1_x1 =  !OUTORMV(ux1,r,c-1)? ux1->data[r][c-1] : 0.0;
                        float ux2_x1 =  !OUTORMV(ux1,r,c-1)? ux2->data[r][c-1] : 0.0;
                        float uy1_x1 =  !OUTORMV(ux1,r,c-1)? uy1->data[r][c-1] : 0.0;
                        float uy2_x1 =  !OUTORMV(ux1,r,c-1)? uy2->data[r][c-1] : 0.0;

                        float ux1_x2 =  !OUTORMV(ux1,r,c+1)? ux1->data[r][c+1] : 0.0;
                        float ux2_x2 =  !OUTORMV(ux1,r,c+1)? ux2->data[r][c+1] : 0.0;
                        float uy1_x2 =  !OUTORMV(ux1,r,c+1)? uy1->data[r][c+1] : 0.0;
                        float uy2_x2 =  !OUTORMV(ux1,r,c+1)? uy2->data[r][c+1] : 0.0;

                        float ux1_y1 =  !OUTORMV(ux1,r-1,c)? ux1->data[r-1][c] : 0.0;
                        float ux2_y1 =  !OUTORMV(ux1,r-1,c)? ux2->data[r-1][c] : 0.0;
                        float uy1_y1 =  !OUTORMV(ux1,r-1,c)? uy1->data[r-1][c] : 0.0;
                        float uy2_y1 =  !OUTORMV(ux1,r-1,c)? uy2->data[r-1][c] : 0.0;

                        float ux1_y2 =  !OUTORMV(ux1,r+1,c)? ux1->data[r+1][c] : 0.0;
                        float ux2_y2 =  !OUTORMV(ux1,r+1,c)? ux2->data[r+1][c] : 0.0;
                        float uy1_y2 =  !OUTORMV(ux1,r+1,c)? uy1->data[r+1][c] : 0.0;
                        float uy2_y2 =  !OUTORMV(ux1,r+1,c)? uy2->data[r+1][c] : 0.0;

                        float e_x1 = !OUTORMV(val,r,c-1)? val->data[r][c-1] : 0.0;
                        float e_x2 = !OUTORMV(val,r,c+1)? val->data[r][c+1] : 0.0;
                        float e_y1 = !OUTORMV(val,r-1,c)? val->data[r-1][c] : 0.0;
                        float e_y2 = !OUTORMV(val,r+1,c)? val->data[r+1][c] : 0.0;

                        float e_in_x1 = e_x1 * (ux2_x1)/(std::max(1e-12f,ux2_x1 + ux1_x1 +uy1_x1 + uy2_x1));
                        float e_in_x2 = e_x2 * (ux1_x2)/(std::max(1e-12f,ux2_x2 + ux1_x2 +uy1_x2 + uy2_x2));
                        float e_in_y1 = e_y1 * (uy2_y1)/(std::max(1e-12f,ux2_y1 + ux1_y1 +uy1_y1 + uy2_y1));
                        float e_in_y2 = e_y2 * (ux1_y2)/(std::max(1e-12f,ux2_y2 + ux1_y2 +uy1_y2 + uy2_y2));

                        float e_n = Source->data[r][c] + e_in_x1 + e_in_x2 + e_in_y1 + e_in_y2;

                        valn->data[r][c] = e_n;
                        //get new guess
                    }
                }
            }
            #pragma omp barrier

            #pragma omp for collapse(2)
                for(int r = 0; r < val->data.nr_rows();r++)
                {
                    for(int c = 0; c < val->data.nr_cols();c++)
                    {
                        val->data[r][c] = valn->data[r][c];
                    }
                }


        #pragma omp barrier

        }

        #pragma omp barrier



    }

    delete valn;

    return val;
}

inline std::vector<cTMap *> AS_WaveEnergySteadyState(cTMap * E_Source, int iter_max, float side_fac = 0.25)
{

    double _dx = std::fabs(E_Source->cellSizeX());



    cTMap * Uxp = E_Source->GetCopy0();
    cTMap * Uxm = E_Source->GetCopy0();
    cTMap * Uyp = E_Source->GetCopy0();
    cTMap * Uym = E_Source->GetCopy0();


    cTMap * psi = E_Source->GetCopy0();



    cTMap * UNxp = E_Source->GetCopy0();
    cTMap * UNxm = E_Source->GetCopy0();
    cTMap * UNyp = E_Source->GetCopy0();
    cTMap * UNym = E_Source->GetCopy0();


    cTMap * psiN = E_Source->GetCopy0();


    cTMap * E = E_Source->GetCopy();
    cTMap * EN = E_Source->GetCopy();


    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {


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
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < E_Source->data.nr_rows();r++)
            {
                for(int c = 0; c < E_Source->data.nr_cols();c++)
                {
                    if(!pcr::isMV(E_Source->data[r][c]))
                    {

                        float valh = psi->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(psi,r,c-1))
                        {
                            val_x1 = psi->data[r][c-1];
                        }
                        if(!OUTORMV(psi,r,c+1))
                        {
                            val_x2 = psi->data[r][c+1];
                        }
                        if(!OUTORMV(psi,r-1,c))
                        {
                            val_y1 = psi->data[r-1][c];
                        }
                        if(!OUTORMV(psi,r+1,c))
                        {
                            val_y2 = psi->data[r+1][c];
                        }



                        valh = E_Source->data[r][c] + (val_x1 + val_x2 + val_y1 + val_y2)/4.0;

                        psiN->data[r][c] = valh;
                    }

                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < E_Source->data.nr_rows();r++)
            {
                for(int c = 0; c < E_Source->data.nr_cols();c++)
                {
                    if(!pcr::isMV(E_Source->data[r][c]))
                    {
                        psi->data[r][c] = psiN->data[r][c];
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }

    stop = false;

    i = 0;
    i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {


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
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            float dx = 1.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < E_Source->data.nr_rows();r++)
            {
                for(int c = 0; c < E_Source->data.nr_cols();c++)
                {
                    if(!pcr::isMV(E_Source->data[r][c]))
                    {

                        float psih = !OUTORMV(psi,r,c)? psi->data[r][c] : 0.0;
                        float psi_x1 = !OUTORMV(psi,r,c-1)? psi->data[r][c-1] : psih;
                        float psi_x11 = !OUTORMV(psi,r,c-2)? psi->data[r][c-2] : psih;
                        float psi_x22 = !OUTORMV(psi,r,c+2)? psi->data[r][c+2] : psih;
                        float psi_x2 = !OUTORMV(psi,r,c+1)? psi->data[r][c+1] : psih;
                        float psi_y1 = !OUTORMV(psi,r-1,c)? psi->data[r-1][c] : psih;
                        float psi_y11 = !OUTORMV(psi,r-2,c)? psi->data[r-2][c] : psih;
                        float psi_y2 = !OUTORMV(psi,r+1,c)? psi->data[r+1][c] : psih;
                        float psi_y22 = !OUTORMV(psi,r+2,c)? psi->data[r+2][c] : psih;

                        float psi_x1y1 = !OUTORMV(psi,r-1,c-1)? psi->data[r-1][c-1] : psih;
                        float psi_x1y2 = !OUTORMV(psi,r+1,c-1)? psi->data[r+1][c-1] : psih;
                        float psi_y1x1 = !OUTORMV(psi,r-1,c-1)? psi->data[r-1][c-1] : psih;
                        float psi_y1x2 = !OUTORMV(psi,r-1,c+1)? psi->data[r-1][c+1] : psih;

                        float ux_p = !OUTORMV(psi,r,c)? Uxp->data[r][c] : 0.0;
                        float ux_m = !OUTORMV(psi,r,c)? Uxm->data[r][c] : 0.0;

                        float uy_p = !OUTORMV(psi,r,c)? Uyp->data[r][c] : 0.0;
                        float uy_m = !OUTORMV(psi,r,c)? Uym->data[r][c] : 0.0;

                        float ux_p_x1 = !OUTORMV(psi,r,c-1)? Uxp->data[r][c-1] : ux_p;
                        float ux_m_x1 = !OUTORMV(psi,r,c-1)? Uxm->data[r][c-1] : ux_m;
                        float ux_p_x2 = !OUTORMV(psi,r,c+1)? Uxp->data[r][c+1] : ux_p;
                        float ux_m_x2 = !OUTORMV(psi,r,c+1)? Uxm->data[r][c+1] : ux_m;
                        float ux_p_y1 = !OUTORMV(psi,r-1,c)? Uxp->data[r-1][c] : ux_p;
                        float ux_m_y1 = !OUTORMV(psi,r-1,c)? Uxm->data[r-1][c] : ux_m;
                        float ux_p_y2 = !OUTORMV(psi,r+1,c)? Uxp->data[r+1][c] : ux_p;
                        float ux_m_y2 = !OUTORMV(psi,r+1,c)? Uxm->data[r+1][c] : ux_m;

                        float uy_p_x1 = !OUTORMV(psi,r,c-1)? Uyp->data[r][c-1] : uy_p;
                        float uy_m_x1 = !OUTORMV(psi,r,c-1)? Uym->data[r][c-1] : uy_m;
                        float uy_p_x2 = !OUTORMV(psi,r,c+1)? Uyp->data[r][c+1] : uy_p;
                        float uy_m_x2 = !OUTORMV(psi,r,c+1)? Uym->data[r][c+1] : uy_m;
                        float uy_p_y1 = !OUTORMV(psi,r-1,c)? Uyp->data[r-1][c] : uy_p;
                        float uy_m_y1 = !OUTORMV(psi,r-1,c)? Uym->data[r-1][c] : uy_m;
                        float uy_p_y2 = !OUTORMV(psi,r+1,c)? Uyp->data[r+1][c] : uy_p;
                        float uy_m_y2 = !OUTORMV(psi,r+1,c)? Uym->data[r+1][c] : uy_m;

                        float out_all_x1 = ux_p_x1 + ux_m_x1 + uy_p_x1 + uy_m_x1;
                        float out_all_x2 = ux_p_x2 + ux_m_x2 + uy_p_x2 + uy_m_x2;
                        float out_all_y1 = ux_p_y1 + ux_m_y1 + uy_p_y1 + uy_m_y1;
                        float out_all_y2 = ux_p_y2 + ux_m_y2 + uy_p_y2 + uy_m_y2;


                        //for both uxp, uxn, uyp, uyn we can get 4 different estimates from the actual equations,
                        //take all the estimates and average them in the end


                        float ux_psi = (psih - psi_x1)/dx;
                        float uy_psi = (psih - psi_y1)/dx;

                        //second derivative on staggered grid, location x-0.5
                        //(f(x-2) -f(x-1) - f(x) + f(x+1))/(2*dx*dx)
                        float dpsidx2 = (psi_x11 - psi_x1 -psih + psi_x2)/(2.0 * dx*dx);
                        float dpsidy2 = (psi_y11 - psi_y1 -psih + psi_y2)/(2.0 * dx*dx);
                        //second derivative x-y direction on staggered grid, location x-0.5
                        float dpsidxy = (psi_x1y1 - psi_y1 + psi_y2 - psi_x1y2)/(2.0 * dx*dx);
                        float dpsidyx = (psi_y1x1 - psi_x1 + psi_x2 - psi_y1x2)/(2.0 * dx*dx);

                        float duxpdx = (ux_p_x2 - ux_p_x1)/(2.0*dx);
                        float duxmdx = (ux_m_x2 - ux_m_x1)/(2.0*dx);

                        float duypdx = (uy_p_x2 - uy_p_x1)/(2.0*dx);
                        float duymdx = (uy_m_x2 - uy_m_x1)/(2.0*dx);

                        float duxpdy = (ux_p_y2 - ux_p_y1)/(2.0*dx);
                        float duxmdy = (ux_m_y2 - ux_m_y1)/(2.0*dx);

                        float duypdy = (uy_p_y2 - uy_p_y1)/(2.0*dx);
                        float duymdy = (uy_m_y2 - uy_m_y1)/(2.0*dx);

                        float uxp_n = (E_Source->data[r][c]  + ux_p_x1 *  (ux_p_x1/std::max(1e-12f,out_all_x1))  + ux_p_y1 *(uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_p_y2  *(uy_m_y2/std::max(1e-12f,out_all_y2)));
                        float uxn_n = (E_Source->data[r][c]  + ux_m_x2  *(ux_m_x2/std::max(1e-12f,out_all_x2)) + ux_m_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_m_y2 *  (uy_m_y2/std::max(1e-12f,out_all_y2)));

                        float uyp_n = (E_Source->data[r][c]  + uy_p_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + uy_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_p_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)));
                        float uyn_n = (E_Source->data[r][c]  + uy_m_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)) + uy_m_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_m_x2 *(ux_m_x2/std::max(1e-12f,out_all_x2)));



                         /*float uxp_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_p_x1  *ux_p_x1 *  (ux_p_x1/std::max(1e-12f,out_all_x1))  + ux_p_y1 *  uy_p_y1 *(uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_p_y2 * uy_m_y2 *(uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_p_x1/std::max(1e-12f,out_all_x1)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));
                        float uxn_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_m_x2  *ux_m_x2  *(ux_m_x2/std::max(1e-12f,out_all_x2)) + ux_m_y1 * uy_p_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_m_y2 *uy_m_y2 *  (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));

                        float uyp_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_p_y1  *uy_p_y1  * (uy_p_y1/std::max(1e-12f,out_all_y1)) + uy_p_x1 * ux_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_p_x2 * ux_m_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_p_y1/std::max(1e-12f,out_all_y1)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));
                        float uyn_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_m_y2  *uy_m_y2  * (uy_m_y2/std::max(1e-12f,out_all_y2)) + uy_m_x1 * ux_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_m_x2 *  ux_m_x2 *(ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));


                        float uxp_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1))  + ux_p_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_p_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + ux_p_x1 + uy_m_y2 + uy_p_y1);
                        float uxn_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_m_x2  * (ux_m_x2/std::max(1e-12f,out_all_x2)) + ux_m_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_m_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + ux_m_x2 + uy_m_y2 + uy_p_y1);

                        float uyp_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_p_y1  * (uy_p_y1/std::max(1e-12f,out_all_y1)) + uy_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_p_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + uy_p_y1 + ux_m_x2 + ux_p_x1);
                        float uyn_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_m_y2  * (uy_m_y2/std::max(1e-12f,out_all_y2)) + uy_m_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_m_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + uy_m_y2 + ux_m_x2 + ux_p_x1);

                        */


                        /*float uxp_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1))  + ux_p_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_p_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_p_x1/std::max(1e-12f,out_all_x1)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));
                        float uxn_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_m_x2  * (ux_m_x2/std::max(1e-12f,out_all_x2)) + ux_m_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_m_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));

                        float uyp_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_p_y1  * (uy_p_y1/std::max(1e-12f,out_all_y1)) + uy_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_p_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_p_y1/std::max(1e-12f,out_all_y1)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));
                        float uyn_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_m_y2  * (uy_m_y2/std::max(1e-12f,out_all_y2)) + uy_m_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_m_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));
                        */

                        /*float uxp_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1))  + ux_p_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_p_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_p_x1/std::max(1e-12f,out_all_x1)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));
                        float uxn_n = (E_Source->data[r][c] * E_Source->data[r][c] + ux_m_x2  * (ux_m_x2/std::max(1e-12f,out_all_x2)) + ux_m_y1 * (uy_p_y1/std::max(1e-12f,out_all_y1)) + ux_m_y2 * (uy_m_y2/std::max(1e-12f,out_all_y2)))/std::max(1e-12f,E_Source->data[r][c] + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (uy_p_y1/std::max(1e-12f,out_all_y1)));

                        float uyp_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_p_y1  * (uy_p_y1/std::max(1e-12f,out_all_y1)) + uy_p_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_p_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_p_y1/std::max(1e-12f,out_all_y1)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));
                        float uyn_n = (E_Source->data[r][c] * E_Source->data[r][c] + uy_m_y2  * (uy_m_y2/std::max(1e-12f,out_all_y2)) + uy_m_x1 * (ux_p_x1/std::max(1e-12f,out_all_x1)) + uy_m_x2 * (ux_m_x2/std::max(1e-12f,out_all_x2)))/std::max(1e-12f,E_Source->data[r][c] + (uy_m_y2/std::max(1e-12f,out_all_y2)) + (ux_m_x2/std::max(1e-12f,out_all_x2)) + (ux_p_x1/std::max(1e-12f,out_all_x2)));
                        */


                        /*float s_xpx1 = !OUTORMV(psi,r,c-1)? E_Source->data[r][c-1] : 0.0;
                        float s_xpx2 = !OUTORMV(psi,r,c)? E_Source->data[r][c] : 0.0;
                        float s_xmx1 = !OUTORMV(psi,r,c-1)? E_Source->data[r][c-1] : 0.0;
                        float s_xmx2 = !OUTORMV(psi,r,c)? E_Source->data[r][c] : 0.0;
                        float s_ypy1 = !OUTORMV(psi,r-1,c)? E_Source->data[r-1][c] : 0.0;
                        float s_ypy2 = !OUTORMV(psi,r,c)? E_Source->data[r][c] : 0.0;
                        float s_ymy1 = !OUTORMV(psi,r-1,c)? E_Source->data[r-1][c] : 0.0;
                        float s_ymy2 = !OUTORMV(psi,r,c)? E_Source->data[r][c] : 0.0;


                        float uxp_g1 = s_xpx1 + ux_m - (uy_p - uy_m)*(duxpdy - duxmdy)/absMax(1e-12f,duxpdx - duxmdx);
                        float uxp_g2 = s_xpx1 + ux_m - (uy_p - uy_m)*(duypdy - duymdy)/absMax(1e-12f,duypdx - duymdx);

                        float uxn_g1 = -s_xmx2 + ux_p + (uy_p - uy_m)*(duxpdy - duxmdy)/absMax(1e-12f,duxpdx - duxmdx);
                        float uxn_g2 = -s_xmx2 + ux_p + (uy_p - uy_m)*(duypdy - duymdy)/absMax(1e-12f,duypdx - duymdx);

                        float uyp_g1 = s_ypy1 + uy_m - (ux_p - ux_m)*(duxpdx - duxmdx)/absMax(1e-12f,duxpdy - duxmdy);
                        float uyp_g2 = s_ypy1 + uy_m - (ux_p - ux_m)*(duypdx - duymdx)/absMax(1e-12f,duypdy - duymdy);

                        float uyn_g1 = -s_ymy2 + uy_p + (ux_p - ux_m)*(duxpdx - duxmdx)/absMax(1e-12f,duxpdy - duxmdy);
                        float uyn_g2 = -s_ymy2 + uy_p + (ux_p - ux_m)*(duypdx - duymdx)/absMax(1e-12f,duypdy - duymdy);


                        float uxp_g1 = ux_m - (uy_p - uy_m)*( dpsidxy)/absMax(1e-12f,dpsidx2);
                        float uxp_g2 = ux_m - (uy_p - uy_m)*( dpsidy2)/absMax(1e-12f,dpsidyx);

                        float uxn_g1 = ux_p + (uy_p - uy_m)*( dpsidxy)/absMax(1e-12f,dpsidx2);
                        float uxn_g2 = ux_p + (uy_p - uy_m)*( dpsidy2)/absMax(1e-12f,dpsidyx);

                        float uyp_g1 = uy_m - (ux_p - ux_m)*( dpsidx2)/absMax(1e-12f,dpsidxy);
                        float uyp_g2 = uy_m - (ux_p - ux_m)*(  dpsidyx)/absMax(1e-12f,dpsidy2);

                        float uyn_g1 = uy_p + (ux_p - ux_m)*( dpsidx2)/absMax(1e-12f,dpsidxy);
                        float uyn_g2 = uy_p + (ux_p - ux_m)*(  dpsidyx)/absMax(1e-12f,dpsidy2);


                        float uxp_g3 = ux_psi + uxn_g1;
                        float uxp_g4 = ux_psi + uxn_g2;
                        float uxn_g3 = -ux_psi + uxp_g1;
                        float uxn_g4 = -ux_psi + uxp_g2;
                        float uyp_g3 = uy_psi + uyn_g1;
                        float uyp_g4 = uy_psi + uyn_g2;
                        float uyn_g3 = -uy_psi + uyp_g1;
                        float uyn_g4 = -uy_psi + uyp_g2;*/

                        /*
                        if(std::fabs(uxp_n) > 0.1 || std::fabs(uxn_n) > 0.1 || std::fabs(uyp_n) > 0.1 || std::fabs(uyn_n) > 0.1)
                        {
                            std::cout << "found "  << std::endl;
                            std::cout << uxp_n << " " << uxn_n << " " << uyp_n << " " << uyn_n << std::endl;
                            //std::cout << uxp_g1 << " " << uxp_g2 << " " << uxp_g3 << " " <<uxp_g4<< std::endl;
                            std::cout << ux_psi << " " << uy_psi << " " << std::endl;
                            std::cout << dpsidx2 << " " << dpsidy2 << " " << dpsidxy << " " <<dpsidyx <<  std::endl;
                            std::cout << duxpdx << " " << duxmdx << " " << duypdx << " " <<duypdx<< " " << duxpdy << " " << duxmdy << " " << duypdy << " " <<duypdy<< std::endl;
                            std::cout << (duxpdy - duxmdy) << " " << absMax(1e-12f,duxpdx - duxmdx) << " " << (duypdy - duymdy) << " " << absMax(1e-12f,duypdx - duymdx) << std::endl;

                        }*/

                        if(uxp_n < 0.0)
                        {
                            uxn_n += -uxp_n;
                            uxp_n = 0.0;
                        }
                        if(uxn_n < 0.0)
                        {
                            uxp_n += -uxn_n;
                            uxn_n = 0.0;
                        }
                        if(uyp_n < 0.0)
                        {
                            uyn_n += -uyp_n;
                            uyp_n = 0.0;
                        }
                        if(uyn_n < 0.0)
                        {
                            uyp_n += -uyn_n;
                            uyn_n = 0.0;
                        }

                        if(!OUTORMV(E,r,c-1))
                        {
                            }
                        if(!OUTORMV(E,r,c+1))
                        {
                            }
                        if(!OUTORMV(E,r-1,c))
                        {
                           }
                        if(!OUTORMV(E,r+1,c))
                        {
                            }


                        //get the y-velocity going to the current cell

                        UNxp->data[r][c] = uxp_n;
                        UNxm->data[r][c] = uxn_n;
                        UNyp->data[r][c] = uyp_n;
                        UNym->data[r][c] = uyn_n;
                    }

                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < E_Source->data.nr_rows();r++)
            {
                for(int c = 0; c < E_Source->data.nr_cols();c++)
                {
                    if(!pcr::isMV(E_Source->data[r][c]))
                    {

                        Uxp->data[r][c] = side_fac *UNxp->data[r][c] + (1.0-side_fac) * Uxp->data[r][c] ;
                        Uxm->data[r][c] = side_fac *UNxm->data[r][c] + (1.0-side_fac) * Uxm->data[r][c] ;
                        Uyp->data[r][c] = side_fac *UNyp->data[r][c] + (1.0-side_fac) * Uyp->data[r][c] ;
                        Uym->data[r][c] = side_fac *UNym->data[r][c] + (1.0-side_fac) * Uym->data[r][c] ;
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }

    delete Uxp;
    delete Uxm;
    delete Uyp;
    delete Uym;
    delete psiN;


    return {psi, UNxp, UNxm, UNyp, UNym};


}


#endif // RASTERFASTWAVE_H
