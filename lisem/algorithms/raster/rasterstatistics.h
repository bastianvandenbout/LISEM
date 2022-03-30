#ifndef RASTERSTATISTICS_H
#define RASTERSTATISTICS_H


#include <algorithm>
#include <QList>
#include "geo/raster/map.h"


inline double AS_RasterCohensKappa(cTMap * Inventory,cTMap * Model)
{
    if(Inventory->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterCohensKappa (Observed) can not be non-spatial");
        throw 1;
    }
    if(Model->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterCohensKappa (Model) can not be non-spatial");
        throw 1;
    }

    if(!(Inventory->data.nr_rows() ==  Model->data.nr_rows() && Inventory->data.nr_cols() == Model->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    double n_yy = 0.0;
    double n_yn = 0.0;
    double n_ny = 0.0;
    double n_nn = 0.0;

    for(int r = 0; r < Model->data.nr_rows();r++)
    {
        for(int c = 0; c < Model->data.nr_cols();c++)
        {
            if(pcr::isMV(Model->data[r][c]) || pcr::isMV(Inventory->data[r][c]))
            {

            }else {

                if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_nn += 1.0;

                }else if(Inventory->data[r][c] >= 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_yn += 1.0;

                }else if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  >= 0.5f)
                {
                    n_ny += 1.0;
                }else {

                    n_yy += 1.0;
                }

            }
        }
    }

    double total = n_nn + n_yn + n_ny + n_yy;
    if(total > 0.0)
    {
        double p_e = ((n_nn + n_ny)/total)* ((n_nn + n_yn)/total) + ((n_yy + n_ny)/total)* ((n_yy + n_yn)/total);
        double p_0 = ((n_yy + n_nn)/total);
        double cohens_kappa = (p_0 - p_e)/std::max(1e-10,1.0-p_e);
        return cohens_kappa;
    }else {
        return 0.0;
    }

}

inline double AS_RasterContinuousCohensKappa(cTMap * Inventory,cTMap * Model, double threshold)
{
    if(Inventory->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterCohensKappa (Observed) can not be non-spatial");
        throw 1;
    }
    if(Model->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterCohensKappa (Model) can not be non-spatial");
        throw 1;
    }
    if(threshold < 1e-20)
    {
        LISEMS_ERROR("Input 3 for RasterContinuousCohensKappa (threshold) can not be zero");
        throw 1;
    }

    if(!(Inventory->data.nr_rows() ==  Model->data.nr_rows() && Inventory->data.nr_cols() == Model->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    double n_yy = 0.0;
    double n_yn = 0.0;
    double n_ny = 0.0;
    double n_nn = 0.0;

    for(int r = 0; r < Model->data.nr_rows();r++)
    {
        for(int c = 0; c < Model->data.nr_cols();c++)
        {
            if(pcr::isMV(Model->data[r][c]) || pcr::isMV(Inventory->data[r][c]))
            {

            }else {

                if(Inventory->data[r][c] < 0.5f && Model->data[r][c] < threshold)
                {
                    float error = 0.5 * Model->data[r][c]/threshold;
                    n_nn += 1.0 - error;
                    n_ny += error;

                }else if(Inventory->data[r][c] >= 0.5f && Model->data[r][c]  < threshold)
                {
                    float error = 1.0 -0.5 * Model->data[r][c]/threshold;
                    n_yn += error;
                    n_yy += 1.0-error;

                }else if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  >= threshold)
                {
                    float error = 0.5 + 0.5 * (1.0-exp(-(Model->data[r][c] - threshold)));
                    n_ny += error;
                    n_nn += 1.0 - error;
                }else {

                    n_yy += 1.0;
                }

            }
        }
    }

    double total = n_nn + n_yn + n_ny + n_yy;
    if(total > 0.0)
    {
        double p_e = ((n_nn + n_ny)/total)* ((n_nn + n_yn)/total) + ((n_yy + n_ny)/total)* ((n_yy + n_yn)/total);
        double p_0 = ((n_yy + n_nn)/total);
        double cohens_kappa = (p_0 - p_e)/std::max(1e-10,1.0-p_e);
        return cohens_kappa;
    }else {
        return 0.0;
    }
}


inline double AS_RasterAccuracy(cTMap * Inventory,cTMap * Model)
{
    if(Inventory->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterCohensKappa (Observed) can not be non-spatial");
        throw 1;
    }
    if(Model->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterCohensKappa (Model) can not be non-spatial");
        throw 1;
    }

    if(!(Inventory->data.nr_rows() ==  Model->data.nr_rows() && Inventory->data.nr_cols() == Model->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    double n_yy = 0.0;
    double n_yn = 0.0;
    double n_ny = 0.0;
    double n_nn = 0.0;

    for(int r = 0; r < Model->data.nr_rows();r++)
    {
        for(int c = 0; c < Model->data.nr_cols();c++)
        {
            if(pcr::isMV(Model->data[r][c]) || pcr::isMV(Inventory->data[r][c]))
            {

            }else {

                if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_nn += 1.0;

                }else if(Inventory->data[r][c] >= 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_yn += 1.0;

                }else if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  >= 0.5f)
                {
                    n_ny += 1.0;
                }else {

                    n_yy += 1.0;
                }

            }
        }
    }

    double total = n_nn + n_yn + n_ny + n_yy;
    if(total > 0.0)
    {
        return (n_nn + n_yy)/total;
    }else {
        return 0.0;
    }

}

inline double AS_RasterPrecisionRecall(cTMap * Inventory,cTMap * Model)
{
    if(Inventory->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterCohensKappa (Observed) can not be non-spatial");
        throw 1;
    }
    if(Model->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterCohensKappa (Model) can not be non-spatial");
        throw 1;
    }

    if(!(Inventory->data.nr_rows() ==  Model->data.nr_rows() && Inventory->data.nr_cols() == Model->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    double n_yy = 0.0;
    double n_yn = 0.0;
    double n_ny = 0.0;
    double n_nn = 0.0;

    for(int r = 0; r < Model->data.nr_rows();r++)
    {
        for(int c = 0; c < Model->data.nr_cols();c++)
        {
            if(pcr::isMV(Model->data[r][c]) || pcr::isMV(Inventory->data[r][c]))
            {

            }else {

                if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_nn += 1.0;

                }else if(Inventory->data[r][c] >= 0.5f && Model->data[r][c]  < 0.5f)
                {
                    n_yn += 1.0;

                }else if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  >= 0.5f)
                {
                    n_ny += 1.0;
                }else {

                    n_yy += 1.0;
                }

            }
        }
    }

    double P = n_yy/std::max(1e-12,n_ny + n_yy);
    double R = n_yy/std::max(1e-12,n_ny + n_yn);

    return 2.0 * P *R/std::max(1e-12,P+R);

}


inline double AS_RasterContinuousPrecisionRecall(cTMap * Inventory,cTMap * Model, double threshold)
{
    if(Inventory->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 1 for RasterCohensKappa (Observed) can not be non-spatial");
        throw 1;
    }
    if(Model->AS_IsSingleValue)
    {
        LISEMS_ERROR("Input 2 for RasterCohensKappa (Model) can not be non-spatial");
        throw 1;
    }
    if(threshold < 1e-20)
    {
        LISEMS_ERROR("Input 3 for RasterContinuousCohensKappa (threshold) can not be zero");
        throw 1;
    }

    if(!(Inventory->data.nr_rows() ==  Model->data.nr_rows() && Inventory->data.nr_cols() == Model->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }

    double n_yy = 0.0;
    double n_yn = 0.0;
    double n_ny = 0.0;
    double n_nn = 0.0;

    for(int r = 0; r < Model->data.nr_rows();r++)
    {
        for(int c = 0; c < Model->data.nr_cols();c++)
        {
            if(pcr::isMV(Model->data[r][c]) || pcr::isMV(Inventory->data[r][c]))
            {

            }else {

                if(Inventory->data[r][c] < 0.5f && Model->data[r][c] < threshold)
                {
                    float error = 0.5 * Model->data[r][c]/threshold;
                    n_nn += 1.0 - error;
                    n_ny += error;

                }else if(Inventory->data[r][c] >= 0.5f && Model->data[r][c]  < threshold)
                {
                    float error = 1.0 -0.5 * Model->data[r][c]/threshold;
                    n_yn += error;
                    n_yy += 1.0-error;

                }else if(Inventory->data[r][c] < 0.5f && Model->data[r][c]  >= threshold)
                {
                    float error = 0.5 + 0.5 * (1.0-exp(-(Model->data[r][c] - threshold)));
                    n_ny += error;
                    n_nn += 1.0 - error;
                }else {

                    n_yy += 1.0;
                }

            }
        }
    }

    double P = n_yy/std::max(1e-12,n_ny + n_yy);
    double R = n_yy/std::max(1e-12,n_ny + n_yn);

    return 2.0 * P *R/std::max(1e-12,P+R);

}

#endif // RASTERSTATISTICS_H
