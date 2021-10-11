#ifndef RASTERFIBERBUNDLE_H
#define RASTERFIBERBUNDLE_H


#include "geo/raster/map.h"
#include <vector>
#include "linear/lsm_vector3.h"

#include "functions.h"


//integral of x*pdfWeibull(x,a,b) from 0 to x
//can get into extreme large and small values, need to use double!
float cdf_xweibull(double x, double a, double b)
{
    double x1 =  -x*(pow((x/b),a));
    if(x < 0.0001 && x > -0.0001)
    {
        x = 0.0001;
    }
    double x1a = (-x/b);
    double x1b = -(pow(x1a,a));
    double x2 = ((x1b));
    if(x2 < 0.0)
    {
        x2 = - x2;
    }
    double x5 = (-(a+1.0)/a);


    double x4 = pow(x2,x5);

    //std::cout << a << " " << b << " " << 1.0+1.0/a << " " << pow((x/b),a) << std::endl;
    double x3 = boost::math::tgamma(1.0+1.0/a,pow((x/b),a));

    return x1*x4*x3;
}

float cdf_xweibull(float x_start, float x_end, float a, float b)
{
    return cdf_xweibull(x_end,a,b)-cdf_xweibull(x_start,a,b);
}


//integral of pdfWeibull(x,a,b) from 0 to x
float cdf_weibull(float x, float a, float b)
{
    return 1.0 - pow(2.71828 ,-pow((x/b),a));
}

float cdf_weibull(float x_start, float x_end, float a, float b)
{
    return cdf_weibull(x_end,a,b)-cdf_weibull(x_start,a,b);
}

//pdfWeibull(x,a,b) at x
float pdf_weibull(float x, float a, float b)
{
    return ((a*(pow(2.71828,pow((-x/b),a)) * pow((x/b),(a-1.0))/b)));
}


double icdf_weibull(double y, double a, double b)
{

        return b*pow((-log(1.0-y)),(1.0/a));
}


inline LSMVector3 StressRedistribute(double stress_initial, double stress_done, double a, double b)
{
    float stresso = stress_initial;
    float stress_prev = stress_done;
    float stress = stress_initial;


    float fraction_break_max = 0.0;
    //typically needs only 1 or two iterations if stable, up to 10 if very close to threshold
    for(int i = 0; i < 50; i++)
    {
        float fractionsofar = cdf_weibull(0.0001,stress,a,b);

        fraction_break_max = std::max(fractionsofar,fraction_break_max);

        if(stress_prev > stress)
        {
            break;
        }

        if(fractionsofar > 0.999)
        {

            break;
        }

        float fraction = cdf_weibull(stress_prev,stress,a,b);
        if(fraction < 0.001)
        {
            break;
        }

        float stress_wav = cdf_xweibull(stress_prev,stress,a,b)/cdf_weibull(stress_prev,stress,a,b);


        stress_prev = stress;
        stress = stress + stress*fraction/(1.0- fractionsofar);

        if(!std::isfinite(stress_wav))
        {
            break;
        }
        if(stress - stress_prev < 0.001)
        {

            break;
        }
        if(stress < 0.0)
        {

            break;
        }
    }


    if(!std::isfinite(stress))
    {
        stress = stress_initial;
        fraction_break_max = 0.0;
    }

    return LSMVector3(stress,fraction_break_max,0.0);
}



std::vector<cTMap*> AS_LoadRedistribute(cTMap * inLoadMax, cTMap * inLoadMaxX, cTMap * inLoadMaxY,  cTMap * Broken_Base, cTMap * Broken_LatX, cTMap * Broken_LatY, cTMap * inLoadX, cTMap * inLoadY, cTMap * CapacityBase, cTMap * CapacityX, cTMap * CapacityY, cTMap * K_comp, cTMap * K_tensile, cTMap * alpha, int iterations, float m_base, float m_lat, bool do_springblock)
{

    //this is an implementation of the semi-analytical fiber bundle model with weibull-based fiber distributions, and an elastic-block slope model with distinct compressive and tensile strength properties.
    //This one is for you Yuanjing!

    cTMap * LoadX = inLoadX->GetCopy();
    cTMap * LoadY = inLoadY->GetCopy();
    cTMap * LoadTotal = LoadX->GetCopy0();
    cTMap * LoadTotalX = LoadX->GetCopy0();
    cTMap * LoadTotalY = LoadX->GetCopy0();
    cTMap * LoadTotalSX = LoadX->GetCopy0();
    cTMap * LoadTotalSY = LoadX->GetCopy0();
    cTMap * LoadTransferX = LoadX->GetCopy0();
    cTMap * LoadTransferY = LoadX->GetCopy0();
    cTMap * LoadTransferX1 = LoadX->GetCopy0();
    cTMap * LoadTransferX2 = LoadX->GetCopy0();
    cTMap * LoadTransferY1 = LoadX->GetCopy0();
    cTMap * LoadTransferY2 = LoadX->GetCopy0();
    cTMap * LoadExcessX = LoadX->GetCopy0();
    cTMap * LoadExcessY = LoadY->GetCopy0();
    cTMap * LoadMax = inLoadMax->GetCopy();
    cTMap * LoadMaxX = inLoadMaxX->GetCopy();
    cTMap * LoadMaxY = inLoadMaxY->GetCopy();

    cTMap * Frac_Broken_Base =Broken_Base->GetCopy();
    cTMap * Frac_Broken_LatX =Broken_LatX->GetCopy();
    cTMap * Frac_Broken_LatY =Broken_LatY->GetCopy();


    for(int i = 0; i < iterations; i++)
    {
        std::cout << "iteration " << i << std::endl;

        bool has_excess = false;


        for(int r = 0; r < LoadTotal->nrRows(); r++)
        {
            for(int c = 0; c < LoadTotal->nrCols(); c++)
            {
                if(!pcr::isMV(LoadX->data[r][c]))
                {

                    //get total load
                    //LoadTotal->data[r][c] = std::sqrt(LoadX->data[r][c] *LoadX->data[r][c] + LoadY->data[r][c] *LoadY->data[r][c]);
                    LoadTotal->data[r][c] = std::sqrt((LoadX->data[r][c] + LoadTotalX->data[r][c])*(LoadX->data[r][c] + LoadTotalX->data[r][c]) + (LoadY->data[r][c]+LoadTotalY->data[r][c])*(LoadY->data[r][c]+LoadTotalY->data[r][c]));

                    if(Frac_Broken_Base->data[r][c] > 0.99 || CapacityBase->data[r][c]  < 0.01 )
                    {
                        if(do_springblock)
                        {
                            float load_excess = LoadTotal->data[r][c];

                            float ratio_x = LoadX->data[r][c]/std::max(1e-6f,LoadTotal->data[r][c]);
                            float ratio_y = LoadY->data[r][c]/std::max(1e-6f,LoadTotal->data[r][c]);


                            LoadExcessX->data[r][c] = ratio_x * load_excess;
                            LoadExcessY->data[r][c] = ratio_y * load_excess;

                            LoadTotalSX->data[r][c] = ratio_x * load_excess;
                            LoadTotalSY->data[r][c] = ratio_y * load_excess;
                        }

                    }else
                    {
                        double broken = std::max(0.0f,std::min(1.0f,Frac_Broken_Base->data[r][c]));
                        double stress_to_exceed = icdf_weibull(broken,m_base,CapacityBase->data[r][c]);

                        double stress_test = LoadTotal->data[r][c] + LoadTotal->data[r][c] * broken /(std::max(0.001,1.0-broken));

                        LSMVector3 stress_redist;
                        if(stress_to_exceed < stress_test)
                        {
                            //check if this exceeds maximum
                            stress_redist = StressRedistribute(stress_test,stress_to_exceed,m_base,CapacityBase->data[r][c]);

                        }else
                        {
                            stress_redist = LSMVector3(stress_to_exceed,broken,0.0);
                        }

                        std::cout << "stress " << r << " " << c << " " << stress_test << " " << stress_redist.x << " " << stress_redist.y << " " << CapacityBase->data[r][c] << std::endl;

                        bool fail = false;
                        if(stress_redist.y > 0.99)
                        {
                            fail = true;
                            stress_redist.z = 1.0;
                        }
                        //LSMVector3(stress,fraction_break_max,0.0);

                        Frac_Broken_Base->data[r][c] = std::max(Frac_Broken_Base->data[r][c],stress_redist.y);
                        LoadMax->data[r][c] = std::max(LoadMax->data[r][c],LoadTotal->data[r][c] );


                        //get load excess
                        float load_excess = fail? LoadTotal->data[r][c] : 0.0;

                        if(fail)
                        {
                            has_excess = false;
                        }

                        //get excess load x and y components
                        float ratio_x = LoadX->data[r][c]/std::max(1e-6f,LoadTotal->data[r][c]);
                        float ratio_y = LoadY->data[r][c]/std::max(1e-6f,LoadTotal->data[r][c]);

                        if(do_springblock)
                        {
                            LoadExcessX->data[r][c] = ratio_x * load_excess;
                            LoadExcessY->data[r][c] = ratio_y * load_excess;

                            LoadTotalSX->data[r][c] = ratio_x * load_excess;
                            LoadTotalSY->data[r][c] = ratio_y * load_excess;
                        }
                    }
                }
            }
        }

        std::cout << "done base fibers" << std::endl;


        if(!has_excess)
        {
            std::cout << "no failures, done" << std::endl;

            break;
        }

        //get transfer load
        for(int r = 0; r < LoadTotal->nrRows(); r++)
        {
            for(int c = 0; c < LoadTotal->nrCols(); c++)
            {
                if(!pcr::isMV(LoadX->data[r][c]))
                {

                    //get k constants

                    int r1 = r;
                    int r2 = r;
                    int c1 = c;
                    int c2 = c;
                    if(r > 0)
                    {
                        r1 = r-1;
                    }
                    if(r < LoadTotal->nrRows()-1)
                    {
                        r2 = r+1;
                    }
                    if(c> 0)
                    {
                        c1 = c-1;
                    }
                    if(c < LoadTotal->nrCols()-1)
                    {
                        c2 = c+1;
                    }
                    float kt = K_tensile->data[r][c];
                    float kc = K_comp->data[r][c];

                    float kt_x1 = 0.5 *( kt + K_tensile->data[r][c1]);
                    float kt_x2 = 0.5 *( kt + K_tensile->data[r][c2]);
                    float kt_y1 = 0.5 *( kt + K_tensile->data[r1][c]);
                    float kt_y2 = 0.5 *( kt + K_tensile->data[r2][c]);
                    float kc_x1 = 0.5 *( kc + K_comp->data[r][c1]);
                    float kc_x2 = 0.5 *( kc + K_comp->data[r][c2]);
                    float kc_y1 = 0.5 *( kc +  K_comp->data[r1][c]);
                    float kc_y2 = 0.5 *( kc +  K_comp->data[r2][c]);

                    //transfer loads
                    double lex = LoadExcessX->data[r][c];
                    double ley = LoadExcessY->data[r][c];

                    bool fail_x =  Frac_Broken_LatX->data[r][c] > 0.99;
                    bool fail_y =  Frac_Broken_LatY->data[r][c] > 0.99;

                    bool fail_x_x1 =  Frac_Broken_LatX->data[r2][c] > 0.99;
                    bool fail_y_y1 =  Frac_Broken_LatY->data[r2][c] > 0.99;

                    bool fail_x_x2=  Frac_Broken_LatX->data[r][c1] > 0.99;
                    bool fail_y_y2 =  Frac_Broken_LatY->data[r][c2] > 0.99;

                    //if the cell itsefl has failed, we only do compressive forces downslope.
                    if(!fail_x)
                    {
                        if(!fail_x_x1)
                        {
                            LoadTransferX1->data[r][c] = lex < 0.0? -lex * kc_x1/(kc_x1 + kt_x2): lex * kt_x1/(kt_x1 + kc_x2);
                        }
                        if(!fail_x_x2)
                        {
                            LoadTransferX2->data[r][c] = lex > 0.0? lex * kc_x2/(kt_x1 + kc_x2): -lex * kt_x2/(kt_x1 + kc_x2);
                        }

                    }else
                    {
                        if(!fail_x_x1)
                        {
                            LoadTransferX1->data[r][c] = lex < 0.0? -lex : 0.0;
                        }
                        if(!fail_x_x2)
                        {
                            LoadTransferX2->data[r][c] = lex > 0.0? lex : 0.0;
                        }
                    }
                    if(!fail_y)
                    {
                        if(!fail_y_y1)
                        {
                            LoadTransferY1->data[r][c] = ley < 0.0? -ley * kc_y1/(kc_y1 + kt_y2): ley * kt_y1/(kt_y1 + kc_y2);
                        }
                        if(!fail_y_y2)
                        {
                            LoadTransferY2->data[r][c] = ley > 0.0? ley * kc_y2/(kt_y1 + kc_y2): -ley * kt_y2/(kt_y1 + kc_y2);
                        }

                    }else
                    {
                        if(!fail_y_y1)
                        {
                            LoadTransferY1->data[r][c] = ley < 0.0? -ley : 0.0;
                        }
                        if(!fail_y_y2)
                        {
                            LoadTransferY2->data[r][c] = ley > 0.0? ley : 0.0;
                        }
                    }

                    lex = LoadTransferX->data[r][c];
                    ley = LoadTransferY->data[r][c];

                    //only if the cell laterals itself has not failed, do we transfer current transfer forces
                    if(!fail_x)
                    {
                        LoadTransferX1->data[r][c] = lex < 0.0? -lex : 0.0;
                        LoadTransferX2->data[r][c] = lex > 0.0? lex : 0.0;
                        LoadTransferX->data[r][c] = 0.0;
                        LoadTransferY->data[r][c] = 0.0;
                    }

                    if(!fail_x)
                    {
                        LoadTransferY1->data[r][c] += ley < 0.0? -lex : 0.0;
                        LoadTransferY2->data[r][c] += ley > 0.0? lex : 0.0;
                        LoadTransferX->data[r][c] = 0.0;
                        LoadTransferY->data[r][c] = 0.0;
                    }


                    //if the other connection failed, just reflect the tension back onto the cell
                    if(fail_x_x1)
                    {
                        LoadTransferX->data[r][c] += LoadTransferX1->data[r][c];
                        LoadTransferX1->data[r][c] = 0.0;
                    }
                    if(fail_x_x2)
                    {
                        LoadTransferX->data[r][c] += LoadTransferX2->data[r][c];
                        LoadTransferX2->data[r][c] = 0.0;
                    }

                    if(fail_y_y1)
                    {
                        LoadTransferY->data[r][c] += LoadTransferY1->data[r][c];
                        LoadTransferY1->data[r][c] = 0.0;
                    }
                    if(fail_y_y2)
                    {
                        LoadTransferY->data[r][c] += LoadTransferY2->data[r][c];
                        LoadTransferY2->data[r][c] = 0.0;
                    }

                }
            }
        }

        //check failure of lateral bonds

        //get transfer load

        for(int r = 0; r < LoadTotal->nrRows(); r++)
        {
            for(int c = 0; c < LoadTotal->nrCols(); c++)
            {
                if(!pcr::isMV(LoadX->data[r][c]))
                {

                    //check breaking and failure of lateral fibers


                    //add lateral load (not present in first step)
                    double lat_load_x = 0.0;//LoadTransferX1->data[r][c];
                    double lat_load_y = 0.0;//LoadTransferX1->data[r][c];

                    int r1 = r;
                    int r2 = r;
                    int c1 = c;
                    int c2 = c;
                    if(r > 0)
                    {
                        r1 = r-1;
                        lat_load_y += LoadTransferY2->data[r1][c];

                    }
                    if(r < LoadTotal->nrRows()-1)
                    {
                        r2 = r+1;
                        lat_load_y -= LoadTransferY1->data[r2][c];
                    }
                    if(c> 0)
                    {
                        c1 = c-1;
                        lat_load_x += LoadTransferX2->data[r][c1];
                    }
                    if(c < LoadTotal->nrCols()-1)
                    {
                        c2 = c+1;
                        lat_load_x -= LoadTransferX1->data[r][c2];
                    }

                    LoadTransferX->data[r][c] += lat_load_x;
                    LoadTransferY->data[r][c] += lat_load_y;

                    //get total load
                    LoadTotalX->data[r][c] += lat_load_x;
                    LoadTotalY->data[r][c] += lat_load_y;

                    double LoadTotalXA = std::fabs(LoadTotalSX->data[r][c] + LoadTotalX->data[r][c]);
                    double LoadTotalYA = std::fabs(LoadTotalSY->data[r][c] + LoadTotalY->data[r][c]);

                    bool fail_x = false;
                    bool fail_y = false;

                    //x-lat
                    {

                        double broken = std::max(0.0f,std::min(1.0f,Frac_Broken_LatX->data[r][c]));
                        double stress_to_exceed = icdf_weibull(broken,m_lat,CapacityX->data[r][c]);

                        double stress_test = LoadTotalXA + LoadTotalXA * broken /(std::max(0.001,1.0-broken));

                        LSMVector3 stress_redist;
                        if(stress_to_exceed < stress_test)
                        {
                            //check if this exceeds maximum
                            stress_redist = StressRedistribute(stress_test,stress_to_exceed,m_lat,CapacityX->data[r][c]);

                        }else
                        {
                            stress_redist = LSMVector3(stress_to_exceed,broken,0.0);

                        }

                        bool fail = false;
                        if(stress_redist.y > 0.99)
                        {
                            fail = true;
                            stress_redist.z = 1.0;
                            fail_x = true;
                        }
                        //LSMVector3(stress,fraction_break_max,0.0);

                        Frac_Broken_LatX->data[r][c] = std::max(Frac_Broken_LatX->data[r][c],stress_redist.y);
                        LoadMaxX->data[r][c] = std::max(LoadMaxX->data[r][c],((float)(LoadTotalXA)));


                    }



                    //y-lat
                    {

                        double broken = std::max(0.0f,std::min(1.0f,Frac_Broken_LatY->data[r][c]));
                        double stress_to_exceed = icdf_weibull(broken,m_lat,CapacityY->data[r][c]);

                        double stress_test = LoadTotalYA + LoadTotalYA * broken /(std::max(0.001,1.0-broken));

                        LSMVector3 stress_redist;
                        if(stress_to_exceed < stress_test)
                        {
                            //check if this exceeds maximum
                            stress_redist = StressRedistribute(stress_test,stress_to_exceed,m_lat,CapacityY->data[r][c]);

                        }else
                        {
                            stress_redist = LSMVector3(stress_to_exceed,broken,0.0);

                        }

                        bool fail = false;
                        if(stress_redist.y > 0.99)
                        {
                            fail = true;
                            stress_redist.z = 1.0;
                            fail_y = true;
                        }
                        //LSMVector3(stress,fraction_break_max,0.0);

                        Frac_Broken_LatY->data[r][c] = std::max(Frac_Broken_LatY->data[r][c],stress_redist.y);
                        LoadMaxY->data[r][c] = std::max(LoadMaxY->data[r][c],((float)(LoadTotalYA) ));


                    }

                    //reset excess load
                    LoadExcessX->data[r][c] = 0.0;
                    LoadExcessY->data[r][c] = 0.0;
                }
            }
        }


    }

    //return:
    return {LoadMax,LoadMaxX,LoadMaxY,Frac_Broken_Base,Frac_Broken_LatX,Frac_Broken_LatY};
}


std::vector<cTMap*> AS_FiberWeibullFail(cTMap * LoadX, cTMap * LoadY, cTMap * CapacityX, cTMap * CapacityY)
{


    return {};
}


#endif // RASTERFIBERBUNDLE_H
