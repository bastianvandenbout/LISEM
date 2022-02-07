#ifndef RASTERDIFFUSE_H
#define RASTERDIFFUSE_H


#include "error.h"
#include "geo/raster/map.h"

inline cTMap * AS_ConductiveDiffuse(cTMap * dataI, cTMap * conductivitydata, cTMap * sigma, int iter_max, float timestep, float feed_self_weight)
{

    if(!(dataI->data.nr_rows() == conductivitydata->data.nr_rows() && dataI->data.nr_cols() == conductivitydata->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for sigma");
       throw -1;
    }
    if(!(dataI->data.nr_rows() == sigma->data.nr_rows() && dataI->data.nr_cols() == sigma->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for Prop");
       throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run similarity spread without iterations");
        throw 1;
    }

    cTMap * data= dataI->GetCopy();
    cTMap * dataN= dataI->GetCopy();


    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        float N = 0.05;
        double _dx = std::fabs(data->cellSizeX());




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
            for(int r = 0; r < data->data.nr_rows();r++)
            {
                for(int c = 0; c < data->data.nr_cols();c++)
                {
                    if(!pcr::isMV(data->data[r][c]))
                    {
                        float prop = data->Drc;
                        float propx1 = !OUTORMV(data,r,c-1)? data->data[r][c-1] : prop;
                        float propx2 = !OUTORMV(data,r,c+1)? data->data[r][c+1] : prop;
                        float propy1 = !OUTORMV(data,r-1,c)? data->data[r-1][c] : prop;
                        float propy2 = !OUTORMV(data,r+1,c)? data->data[r+1][c] : prop;

                        float val1 = conductivitydata->Drc;
                        float val2 = sigma->Drc;

                        float val1x1 = !OUTORMV(conductivitydata,r,c-1)? conductivitydata->data[r][c-1] : val1;
                        float val1x2 = !OUTORMV(conductivitydata,r,c+1)? conductivitydata->data[r][c+1] : val1;
                        float val1y1 = !OUTORMV(conductivitydata,r-1,c)? conductivitydata->data[r-1][c] : val1;
                        float val1y2 = !OUTORMV(conductivitydata,r+1,c)? conductivitydata->data[r+1][c] : val1;

                        float val2x1 = !OUTORMV(sigma,r,c-1)? sigma->data[r][c-1] : val2;
                        float val2x2 = !OUTORMV(sigma,r,c+1)? sigma->data[r][c+1] : val2;
                        float val2y1 = !OUTORMV(sigma,r-1,c)? sigma->data[r-1][c] : val2;
                        float val2y2 = !OUTORMV(sigma,r+1,c)? sigma->data[r+1][c] : val2;

                        float sigmax1 = std::sqrt(val2 * val2x1);
                        float sigmax2 = std::sqrt(val2 * val2x2);
                        float sigmay1 = std::sqrt(val2 * val2y1);
                        float sigmay2 = std::sqrt(val2 * val2y2);

                        float cond_x1 = std::exp(-(val1x1-val1)*(val1x1-val1)/(sigmax1*sigmax1));
                        float cond_x2 = std::exp(-(val1x2-val1)*(val1x2-val1)/(sigmax2*sigmax2));
                        float cond_y1 = std::exp(-(val1y1-val1)*(val1y1-val1)/(sigmay1*sigmay1));
                        float cond_y2 = std::exp(-(val1y2-val1)*(val1y2-val1)/(sigmay2*sigmay2));

                        float w_self = std::max(0.0,1.0 - timestep * (cond_x1 + cond_x2 + cond_y1 + cond_y2));
                        float datan  = w_self * prop + timestep * (cond_x1 * propx1 + cond_x2 * propx2 + cond_y1 * propy1 + cond_y2 * propy2);
                        if(std::isnormal(datan))
                        {
                            dataN->data[r][c] = feed_self_weight * data->data[r][c] + (1.0- feed_self_weight) * datan;
                        }else
                        {
                            dataN->data[r][c] = data->data[r][c];
                        }
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < data->data.nr_rows();r++)
            {
                for(int c = 0; c < data->data.nr_cols();c++)
                {
                    if(!pcr::isMV(data->data[r][c]))
                    {
                        data->data[r][c] =dataN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete dataN;

    return data;


}


inline cTMap * AS_ConductiveFeedbackDiffuse(cTMap * dataI, cTMap * conductivitydata, cTMap * sigma, cTMap * fbweight, int iter_max, float timestep)
{

    if(!(dataI->data.nr_rows() == conductivitydata->data.nr_rows() && dataI->data.nr_cols() == conductivitydata->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for data");
       throw -1;
    }
    if(!(dataI->data.nr_rows() == sigma->data.nr_rows() && dataI->data.nr_cols() == sigma->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for conductivitydatea");
       throw -1;
    }
    if(!(dataI->data.nr_rows() == fbweight->data.nr_rows() && dataI->data.nr_cols() == fbweight->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for fbweight");
       throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run similarity spread without iterations");
        throw 1;
    }

    cTMap * data= dataI->GetCopy();
    cTMap * dataN= dataI->GetCopy();


    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        float N = 0.05;
        double _dx = std::fabs(data->cellSizeX());




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
            for(int r = 0; r < data->data.nr_rows();r++)
            {
                for(int c = 0; c < data->data.nr_cols();c++)
                {
                    if(!pcr::isMV(data->data[r][c]))
                    {
                        float prop = data->Drc;
                        float propx1 = !OUTORMV(data,r,c-1)? data->data[r][c-1] : prop;
                        float propx2 = !OUTORMV(data,r,c+1)? data->data[r][c+1] : prop;
                        float propy1 = !OUTORMV(data,r-1,c)? data->data[r-1][c] : prop;
                        float propy2 = !OUTORMV(data,r+1,c)? data->data[r+1][c] : prop;

                        float val1 = conductivitydata->Drc;
                        float val2 = sigma->Drc;

                        float val1x1 = !OUTORMV(conductivitydata,r,c-1)? conductivitydata->data[r][c-1] : val1;
                        float val1x2 = !OUTORMV(conductivitydata,r,c+1)? conductivitydata->data[r][c+1] : val1;
                        float val1y1 = !OUTORMV(conductivitydata,r-1,c)? conductivitydata->data[r-1][c] : val1;
                        float val1y2 = !OUTORMV(conductivitydata,r+1,c)? conductivitydata->data[r+1][c] : val1;

                        float val2x1 = !OUTORMV(sigma,r,c-1)? sigma->data[r][c-1] : val2;
                        float val2x2 = !OUTORMV(sigma,r,c+1)? sigma->data[r][c+1] : val2;
                        float val2y1 = !OUTORMV(sigma,r-1,c)? sigma->data[r-1][c] : val2;
                        float val2y2 = !OUTORMV(sigma,r+1,c)? sigma->data[r+1][c] : val2;

                        float sigmax1 = std::sqrt(val2 * val2x1);
                        float sigmax2 = std::sqrt(val2 * val2x2);
                        float sigmay1 = std::sqrt(val2 * val2y1);
                        float sigmay2 = std::sqrt(val2 * val2y2);

                        float cond_x1 = std::exp(-(val1x1-val1)*(val1x1-val1)/(sigmax1*sigmax1));
                        float cond_x2 = std::exp(-(val1x2-val1)*(val1x2-val1)/(sigmax2*sigmax2));
                        float cond_y1 = std::exp(-(val1y1-val1)*(val1y1-val1)/(sigmay1*sigmay1));
                        float cond_y2 = std::exp(-(val1y2-val1)*(val1y2-val1)/(sigmay2*sigmay2));

                        float w_self = std::max(0.0,1.0 - timestep * (cond_x1 + cond_x2 + cond_y1 + cond_y2));
                        float datan  = w_self * prop + timestep * (cond_x1 * propx1 + cond_x2 * propx2 + cond_y1 * propy1 + cond_y2 * propy2);
                        if(std::isnormal(datan))
                        {
                            dataN->data[r][c] = fbweight->data[r][c] * data->data[r][c] + (1.0- fbweight->data[r][c]) * datan;
                        }else
                        {
                            dataN->data[r][c] = data->data[r][c];
                        }
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < data->data.nr_rows();r++)
            {
                for(int c = 0; c < data->data.nr_cols();c++)
                {
                    if(!pcr::isMV(data->data[r][c]))
                    {
                        data->data[r][c] =dataN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete dataN;

    return data;


}



#endif // RASTERDIFFUSE_H
