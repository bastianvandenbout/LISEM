#ifndef RASTERLAPLACE_H
#define RASTERLAPLACE_H


#include "geo/raster/map.h"
#include "rastercommon.h"



inline std::vector<cTMap *> AS_SolvePoissonAbsTotal(cTMap * Initial, cTMap * Forced, cTMap * f, int iter_max)
{
    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

    cTMap * Fxp = Initial->GetCopy0();
    cTMap * Fxn = Initial->GetCopy0();
    cTMap * Fyp = Initial->GetCopy0();
    cTMap * Fyn = Initial->GetCopy0();

    cTMap * FNxp = Initial->GetCopy0();
    cTMap * FNxn = Initial->GetCopy0();
    cTMap * FNyp = Initial->GetCopy0();
    cTMap * FNyn = Initial->GetCopy0();


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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }



                        valh = f->data[r][c] + (val_x1 + val_x2 + val_y1 + val_y2)/4.0;

                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < val->data.nr_rows();r++)
    {
        for(int c = 0; c < val->data.nr_cols();c++)
        {
            if(!pcr::isMV(val->data[r][c]))
            {

                float valh = val->data[r][c];

                //get values
                float val_x1 = valh;
                float val_x2 = valh;
                float val_y1 = valh;
                float val_y2 = valh;

                if(!OUTORMV(val,r,c-1))
                {
                    val_x1 = val->data[r][c-1];
                }
                if(!OUTORMV(val,r,c+1))
                {
                    val_x2 = val->data[r][c+1];
                }
                if(!OUTORMV(val,r-1,c))
                {
                    val_y1 = val->data[r-1][c];
                }
                if(!OUTORMV(val,r+1,c))
                {
                    val_y2 = val->data[r+1][c];
                }

                float ux1 = val_x1-valh;
                float ux2 = val_x2-valh;
                float uy1 = val_y1-valh;
                float uy2 = val_y2-valh;

                Fxp->data[r][c] = 0;
                Fxn->data[r][c] = 0;
                Fyp->data[r][c] = 0;
                Fyn->data[r][c] = 0;
            }
        }
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



            #pragma omp for collapse(2)
           for(int r = 0; r < val->data.nr_rows();r++)
            {
                for(int c = 0; c < val->data.nr_cols();c++)
                {
                    if(pcr::isMV(val->data[r][c]))
                    {
                    }else
                    {
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }



                        valh = f->data[r][c] + (val_x1 + val_x2 + val_y1 + val_y2)/4.0;


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



                    }
                }


        #pragma omp barrier

        }

        #pragma omp barrier



    }

    delete FNxp;
    delete FNxn;
    delete FNyp;
    delete FNyn;
    delete valn;

    return {val,Fxn,Fxp,Fyn,Fyp};


}


inline cTMap * AS_SolvePoissonW(cTMap * Initial, cTMap * Forced, cTMap * f, float wx1, float wx2, float wy1, float wy2,int iter_max)
{
    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }



                        valh = f->data[r][c] + (wx1*val_x1 + wx2*val_x2 + wy1*val_y1 + wy2*val_y2)/(std::max(1e-12f,wx1+wx2+wy1+wy2));

                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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

inline std::vector<cTMap *> AS_SolvePoissonWC(cTMap * Initial, cTMap * Forced, cTMap * f, float wx1, float wx2, float wy1, float wy2,int iter_max)
{
    //check directionality of weights (N,S,E or W)


    //we need to add buffer lines to the map as we lose a row or column where-ever there is source data.
    //worst case scenario we need to calculate on a map which is twice as big

    //first we are going
    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }

                        float val_x11 = val_x1;
                        float val_x22 = val_x2;
                        float val_y11 = val_y1;
                        float val_y22 = val_y2;

                        if(!OUTORMV(val,r,c-2))
                        {
                            val_x11 = val->data[r][c-2];
                        }
                        if(!OUTORMV(val,r,c+2))
                        {
                            val_x22 = val->data[r][c+2];
                        }
                        if(!OUTORMV(val,r-2,c))
                        {
                            val_y11 = val->data[r-2][c];
                        }
                        if(!OUTORMV(val,r+2,c))
                        {
                            val_y22 = val->data[r+2][c];
                        }


                        if(wx1 < 0.5)
                        {
                            if(val_x1 > valh)
                            {
                                valh = 0.25*std::min(0.0f,-(val_x2 - valh )) + f->data[r][c] + (valh + val_x2 + val_y1 + val_y2)/(4.0f);
                            }else if(val_x2 < valh)
                            {
                                valh =  0.25*std::max(0.0f,(val_x22 - val_x2)) + f->data[r][c] + (val_x1 +valh+ val_y1 + val_y2 )/(4.0f);
                            }else
                            {
                                valh = f->data[r][c] + (val_x1 +val_x2 + val_y1 + val_y2 )/(4.0f);
                            }

                        }
                        if(wx2 < 0.5)
                        {
                            if(val_x2 > valh)
                            {
                                valh = 0.25*std::min(0.0f,-(val_x1 - valh)) +f->data[r][c] + (val_x1 + valh + val_y1 + val_y2)/(4.0f);
                            }else if(val_x1 < valh)
                            {
                                valh = 0.25*std::max(0.0f,(val_x11 - val_x1)) + f->data[r][c] + (valh +val_x2 + val_y1 + val_y2 )/(4.0f);
                            }else
                            {
                                valh = f->data[r][c] + (val_x1 +val_x2 + val_y1 + val_y2 )/(4.0f);
                            }

                        }
                        if(wy1 < 0.5)
                        {
                            if(val_y1 > valh)
                            {
                                valh = 0.25*std::min(0.0f,-(val_y2 - valh)) +f->data[r][c] + (val_x1 + val_x2 + valh + val_y2)/(4.0f);
                            }else if(val_y2 < valh)
                            {
                                valh = 0.25*std::max(0.0f,(val_y22 - val_y2)) + f->data[r][c] + (val_x1 +val_x2 + val_y1 + valh )/(4.0f);
                            }else
                            {
                                valh = f->data[r][c] + (val_x1 +val_x2 + val_y1 + val_y2 )/(4.0f);
                            }

                        }
                        if(wy2 < 0.5)
                        {
                            if(val_y2 > valh)
                            {
                                valh = 0.25*std::min(0.0f,-(val_y1 - valh)) +f->data[r][c] + (val_x1 + val_x2 + val_y1 + valh)/(4.0f);
                            }else if(val_y1 < valh)
                            {
                                valh = 0.25*std::max(0.0f,(val_y11 - val_y1)) + f->data[r][c] + (val_x1 +val_x2 + valh + val_y2 )/(4.0f);
                            }else
                            {
                                valh = f->data[r][c] + (val_x1 +val_x2 + val_y1 + val_y2 )/(4.0f);
                            }

                        }


                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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

    #pragma omp for collapse(2)
    for(int r = 0; r < val->data.nr_rows();r++)
    {
        for(int c = 0; c < val->data.nr_cols();c++)
        {

            if(pcr::isMV(val->data[r][c]))
            {
            }else
            {
                float valh = val->data[r][c];

                //get values
                float val_x1 = valh;
                float val_x2 = valh;
                float val_y1 = valh;
                float val_y2 = valh;

                if(!OUTORMV(val,r,c-1))
                {
                    val_x1 = val->data[r][c-1];
                }
                if(!OUTORMV(val,r,c+1))
                {
                    val_x2 = val->data[r][c+1];
                }
                if(!OUTORMV(val,r-1,c))
                {
                    val_y1 = val->data[r-1][c];
                }
                if(!OUTORMV(val,r+1,c))
                {
                    val_y2 = val->data[r+1][c];
                }

                float val_x11 = val_x1;
                float val_x22 = val_x2;
                float val_y11 = val_y1;
                float val_y22 = val_y2;

                if(!OUTORMV(val,r,c-2))
                {
                    val_x11 = val->data[r][c-2];
                }
                if(!OUTORMV(val,r,c+2))
                {
                    val_x22 = val->data[r][c+2];
                }
                if(!OUTORMV(val,r-2,c))
                {
                    val_y11 = val->data[r-2][c];
                }
                if(!OUTORMV(val,r+2,c))
                {
                    val_y22 = val->data[r+2][c];
                }


                if(wx1 < 0.5)
                {
                    if(val_x1 > valh)
                    {
                        valn->data[r][c] = val_x2 - valh;
                    }else
                    {
                        valn->data[r][c] = valh - val_x1;
                    }

                }
                if(wx2 < 0.5)
                {
                    if(val_x2 > valh)
                    {
                        valn->data[r][c] = val_x1 - valh;
                    }else
                    {
                        valn->data[r][c] = valh - val_x2;
                    }

                }
                if(wy1 < 0.5)
                {
                    if(val_y1 > valh)
                    {
                        valn->data[r][c] = val_y2 - valh;
                    }else
                    {
                        valn->data[r][c] = valh - val_y1;
                    }

                }
                if(wy2 < 0.5)
                {
                    if(val_y2 > valh)
                    {
                        valn->data[r][c] = val_y1 - valh;
                    }else
                    {
                        valn->data[r][c] = valh - val_y2;
                    }

                }
            }
        }
    }


    return {val,valn};


}

inline cTMap * AS_SolvePoisson(cTMap * Initial, cTMap * Forced, cTMap * f, int iter_max)
{
    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }



                        valh = f->data[r][c] + (val_x1 + val_x2 + val_y1 + val_y2)/4.0;

                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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

inline cTMap * AS_SolvePoissonDifSpeed(cTMap * Initial, cTMap * Forced, cTMap * f, cTMap * c, int iter_max)
{
    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }

                        //get flux ratios according to wave speed variation in space


                        valh = f->data[r][c] + (val_x1 + val_x2 + val_y1 + val_y2)/4.0;

                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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

inline cTMap * AS_SolveLaplace(cTMap * Initial, cTMap * Forced, int iter_max)
{

    cTMap * val = Initial->GetCopy();
    cTMap * valn = Initial->GetCopy();

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
                        float valh = val->data[r][c];

                        //get values
                        float val_x1 = valh;
                        float val_x2 = valh;
                        float val_y1 = valh;
                        float val_y2 = valh;

                        if(!OUTORMV(val,r,c-1))
                        {
                            val_x1 = val->data[r][c-1];
                        }
                        if(!OUTORMV(val,r,c+1))
                        {
                            val_x2 = val->data[r][c+1];
                        }
                        if(!OUTORMV(val,r-1,c))
                        {
                            val_y1 = val->data[r-1][c];
                        }
                        if(!OUTORMV(val,r+1,c))
                        {
                            val_y2 = val->data[r+1][c];
                        }



                        valh = (val_x1 + val_x2 + val_y1 + val_y2)/4.0;

                        if(!pcr::isMV(Forced->data[r][c]))
                        {
                            valh = Forced->data[r][c];
                        }
                        valn->data[r][c] = valh;
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



#endif // RASTERLAPLACE_H
