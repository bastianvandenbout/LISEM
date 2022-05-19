#ifndef RASTERSOIL_H
#define RASTERSOIL_H


#include "rastercommon.h"
#include "geo/raster/map.h"

//run evapotranspiration

/* float yterm = 0.000665f * 101.3f*pow((293.0f-0.0065f*z)/293.0f,5.26f);
        float T = temp;
        float u = wind;
        float ea = vapr;
        float height_u = 2.0f;
        float uh = 4.87f/(log(67.8f * height_u - 5.42f));
        float lighthours = 8.0f;
        float Rn = rad * (1.0f-0.23f) *(0.75f + (2.0e-5f)*z)/1000.0f;
        float Rl = max(0.0f,(4.903f*10e-9f) * pow(T+237.0f,4.0f)*(0.34f - (0.14f)*sqrt(ea))*(1.34f*(rad/1000.0f)/Rn - 0.35f))/1000.0f;
        //float LAI = ndvi * 4.0;
        float G = Rn *(0.4f * lighthours/24.0f + 1.8f *(1.0f -lighthours/24.0f))*exp(-0.5f*ndvi * 6.0f);
        float es = 0.6108f*exp(17.27f*T/(T+237.3f));

        float delta = 4098.0f *(0.6108f*exp((17.27f*T)/(T+237.3f)))/((T+237.3f)*(T+237.3f));

        float ET0 = (0.408f*delta *((Rn-Rl) - G) + (yterm *(900.0f)/(T+273.3f))*uh*(es-ea))/(delta + yterm*(1.0f + uh*34.0f));

        //to meters per second
        float etref = max(0.0f, ET0)/1000.0f;

        float GWTheta = max((float)(ThetaR), min((float)(ThetaS),(float)(GWUH/max((float)(0.01),(float)((SD - WFH/ThetaS - GWH/ThetaS))))));


        float et_act = min(GWH+GWUH,cropf * etref * GWTheta);

        float et_act_left = max(0.0f,et_act-GWUH);
        float et_act_left2 =  max(0.0f,et_act-GWH);*/

inline static cTMap * AS_ReferenceEvapotranspiration(cTMap * DEM, cTMap * temp, cTMap * wind, cTMap * vapr, cTMap * rad, cTMap * ndvi)
{
    cTMap * res = DEM->GetCopy0();

    CheckMapsSameSizeError("ReferenceEvapoTranspiration",{"DEM","temp","wind","vapr","rad","ndvi"},{DEM,temp,wind,vapr,rad,ndvi});

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float z = DEM->data[r][c];
                float yterm = 0.000665f * 101.3f*std::pow((293.0f-0.0065f*z)/293.0f,5.26f);
                    float T = temp->data[r][c];
                    float u = wind->data[r][c];
                    float ea = vapr->data[r][c];
                    float height_u = 2.0f;
                    float uh = 4.87f/(log(67.8f * height_u - 5.42f));
                    float lighthours = 8.0f;
                    float Rn = rad->data[r][c] * (1.0f-0.23f) *(0.75f + (2.0e-5f)*z)/1000.0f;
                    float Rl = std::max(0.0f,(4.903f*10e-9f) * std::pow(T+237.0f,4.0f)*(0.34f - (0.14f)*std::sqrt(ea))*(1.34f*(rad->data[r][c]/1000.0f)/Rn - 0.35f))/1000.0f;
                    //float LAI = ndvi * 4.0;
                    float G = Rn *(0.4f * lighthours/24.0f + 1.8f *(1.0f -lighthours/24.0f))*std::exp(-0.5f*ndvi->data[r][c] * 6.0f);
                    float es = 0.6108f*std::exp(17.27f*T/(T+237.3f));

                    float delta = 4098.0f *(0.6108f*std::exp((17.27f*T)/(T+237.3f)))/((T+237.3f)*(T+237.3f));

                    float ET0 = (0.408f*delta *((Rn-Rl) - G) + (yterm *(900.0f)/(T+273.3f))*uh*(es-ea))/(delta + yterm*(1.0f + uh*34.0f));

                    //to meters per second
                    float etref = std::max(0.0f, ET0)/1000.0f;
                    res->data[r][c] = etref;
            }
        }
        }


    return res;
}


inline static std::vector<cTMap *>  AS_Evapotranspiration(cTMap * WFH, cTMap * Theta, cTMap * GWH, cTMap * SD, cTMap * ThetaS, cTMap * ReferenceET, cTMap * Kc, float dt)
{

    CheckMapsSameSizeError("ReferenceEvapoTranspiration",{"DEM","Theta","GWH","SD","ThetaS","ReferenceET","Kc"},{WFH,  Theta,  GWH,  SD, ThetaS, ReferenceET,  Kc});

    cTMap * WFHN = WFH->GetCopy0();
    cTMap * ThetaN = WFH->GetCopy0();
    cTMap * GWHN = WFH->GetCopy0();
    cTMap * ETReal = WFH->GetCopy0();


    #pragma omp parallel for collapse(2)
    for(int r = 0; r < WFH->data.nr_rows();r++)
    {
        for(int c = 0; c < WFH->data.nr_cols();c++)
        {
            if(!pcr::isMV(WFH->data[r][c]))
            {

                float sd = SD->data[r][c];
                float thetas = std::max(0.01f,ThetaS->data[r][c]);
                float w_wf = WFH->data[r][c];
                float w_us = std::min(thetas,Theta->data[r][c]) *std::max(0.0f,SD->data[r][c] - WFH->data[r][c]/thetas - GWH->data[r][c]/thetas);
                float w_s = GWH->data[r][c];

                float theta_rel = std::min(thetas,Theta->data[r][c])/thetas;

                //use crop factor to get actual evapotranspiration

                float et_real = ReferenceET->data[r][c] * Kc->data[r][c] * dt;

                //now get what we can from wetting front

                float et_wf = std::min(w_wf,et_real);

                et_real = et_real - et_wf;

                float et_us = std::min(w_us,et_real);

                et_real = et_real - et_us;

                float et_s = std::min(w_s,et_real * theta_rel);

                et_real = et_real - et_s;

                //then get what we can from unsaturated layer and saturated layer

                //scaling the evaporation from ground water by saturation in unsaturated zone (trick from R. van Beek, 2002)

                GWHN->data[r][c] = std::max(0.0f, std::min(sd * thetas,(w_s - et_s)));
                WFHN->data[r][c] = std::max(0.0f,std::min(sd * thetas - GWHN->data[r][c],(w_wf - et_wf)));
                ThetaN->data[r][c] =std::max(0.0f,std::min(thetas,std::min(sd * thetas - GWHN->data[r][c] - WFHN->data[r][c],(w_us - et_us))/(std::max(w_us,sd - GWHN->data[r][c]/thetas - WFHN->data[r][c]/thetas)));
                ETReal->data[r][c] = et_wf + et_us + et_s;
            }
        }
    }


    return {wfhn, ThetaN, GWHN,ETReal};
}

//run green and ampt with percolation to ground water
inline static std::vector<cTMap *> AS_GreenAndAmptPercolation(cTMap * WFH, cTMap * Theta, cTMap * GWH, cTMap * SD,cTMap * ksat,cTMap * ksatb, cTMap * A, cTMap * B,cTMap * ThetaS, float dt)
{


    //do infiltration
    cTMap * wfhn = WFH->GetCopy();
    cTMap * watern = water->GetCopy();
    cTMap * ThetaN = Theta->GetCopy0();
    cTMap * GWHN = GWN->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < WFH->data.nr_rows();r++)
    {
        for(int c = 0; c < WFH->data.nr_cols();c++)
        {
            if(!pcr::isMV(WFH->data[r][c]))
            {
                float sd = SD->data[r][c];
                float thetas = std::max(0.01f,ThetaS->data[r][c]);
                float a = A->data[r][c];
                float b = B->data[r][c];

                float lambda = 1.0/std::max(1e-12f,b);

                float w_wf = WFH->data[r][c];
                float w_us = std::min(thetas,Theta->data[r][c]) *std::max(0.0f,SD->data[r][c] - WFH->data[r][c]/thetas - GWH->data[r][c]/thetas);
                float w_s = GWH->data[r][c];
                float theta = Theta->data[r][c];

                float space_infil = std::max((float) (0.0),(float)(SD->data[r][c] - WFH->data[r][c]));
                float GA_PSI = psi->data[r][c];
                float KSattop = ksat->data[r][c];
                float KSatb = ksatb->data[r][c];

                float KSattop = ksat->data[r][c];
                float KSatb = ksatb->data[r][c];

                float KSattopus = ksat->data[r][c] * std::pow(std::max(0.0001f,std::min(1.0f,theta/thetas)),3.0+2.0/lambda);
                float KSatbus = ksatb->data[r][c] * std::pow(std::max(0.0001f,std::min(1.0f,theta/thetas)),3.0+2.0/lambda);


                float ksat_wfcomp = KSattop * (1.0f + (GA_PSI * std::max(0.00001f,(float)(ThetaS->data[r][c] - Theta->data[r][c])))/(std::max(0.01f,(float)(WFH->data[r][c]/std::max(0.00001f,(ThetaS->data[r][c] - Theta->data[r][c]))))));
                float infil_pot = std::min((float)(water->data[r][c] * 0.5),std::min((float)(space_infil * 0.5),(float)(ksat_wfcomp * dt)));

                //limit potential infiltration by available room
                float room = std::max(0.0f,thetas * sd - w_wf - w_us - w_s);
                infil_pot = std::min(room,infil_pot);

                watern->data[r][c] = water->data[r][c] - infil_pot;
                wfhn->data[r][c] =  WFH->data[r][c] + infil_pot;

                float ush = std::min(sd,std::max(0.0f,sd  - (w_s/thetas + wfhn->data[r][c]/thetas)));

                //darcy law
                float flux_wf_us = (wfhn->data[r][c]/(wfhn->data[r][c]/thetas + 0.5 * ush)) * (0.5 * (KSattop + KSattopus)) * dt;
                float flux_us_s  = ((theta * ush)/(0.5 * ush))* ((KSatbus)) * dt;

                wfhn->data[r][c] -= flux_wf_us;
                w_us += flux_wf_us - flux_us_s;
                w_s += flux_us_s;

                GWHN->data[r][c] = w_s;
                ThetaN->data[r][c] =std::max(0.0f,std::min(thetas,std::min(sd * thetas - GWHN->data[r][c] - wfhn->data[r][c],(w_us))/(std::max(w_us,sd - GWHN->data[r][c]/thetas - wfhn->data[r][c]/thetas))));





            }
        }
    }

    return {wfhn, ThetaN, GWHN,watern};

    //now percolate from bottom of wetting front to unsaturated zone, movement takes place to distribute the moving water over the entire unsaturated zone
    //finally move some of the unsaterated zone layer towads the groundwater layer

}

inline static std::vector<cTMap *> AS_GreenAndAmpt(cTMap * WFH, cTMap * water, cTMap * SD, cTMap * ksat, cTMap * ThetaS, cTMap * Theta, cTMap * psi, float dt)
{
    cTMap * wfhn = WFH->GetCopy();
    cTMap * watern = water->GetCopy();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < WFH->data.nr_rows();r++)
    {
        for(int c = 0; c < WFH->data.nr_cols();c++)
        {
            if(!pcr::isMV(WFH->data[r][c]))
            {
                float space_infil = std::max((float) (0.0),(float)(SD->data[r][c] - WFH->data[r][c]));
                float GA_PSI = psi->data[r][c];
                float KSattop = ksat->data[r][c];
                float ksat_wfcomp = KSattop * (1.0f + (GA_PSI * std::max(0.00001f,(float)(ThetaS->data[r][c] - Theta->data[r][c])))/(std::max(0.01f,(float)(WFH->data[r][c]))));
                float infil_pot = std::min((float)(water->data[r][c] * 0.5),std::min((float)(space_infil * 0.5),(float)(ksat_wfcomp * dt)));

                watern->data[r][c] = water->data[r][c] - infil_pot;
                wfhn->data[r][c] =  WFH->data[r][c] + infil_pot/std::max(0.00001f,(ThetaS->data[r][c] - Theta->data[r][c]));
            }
        }
    }

    return {wfhn,watern};

}

inline static cTMap * AS_SaxtonKSat(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {

        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = Ksat1 * (1.0f/3600000.0f);
    }

    return ret;
}
inline static cTMap * AS_SaxtonPorosity(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {

        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = thetast;
    }

    return ret;
}
inline static cTMap * AS_SaxtonSuction(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL,  cTMap * Saturation,float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {
        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;
        float psi1 = std::max(10.0f,(float) aA* std::pow((FC1 + Saturation->data[r][c] * (thetast - FC1)),-bB));

        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = psi1;
    }

    return ret;
}
inline static cTMap * AS_SaxtonFieldCapacity(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {

        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = FC1;
    }

    return ret;
}
inline static cTMap * AS_SaxtonA(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {

        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = A;
    }

    return ret;
}
inline static cTMap * AS_SaxtonB(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {
        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,1000.0f * GRAVEL->data[r][c]));

        float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
        float M1500adj =M1500+0.14*M1500-0.02;
        float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
        float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
        float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
        float PM33adj = PM33+(0.636*PM33-0.107);
        float SatPM33 = M33adj + PM33adj;
        float SatSadj = -0.097*S+0.043;
        float SadjSat = SatPM33  + SatSadj;
        float Dens_om = (1-SadjSat)*2.65;
        float Dens_comp = Dens_om * densityfactor;
        float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
        float M33comp = M33adj - 0.2*PORE_comp;
        float thetast = 1.0-(Dens_comp/2.65);
        float PoreMcomp = thetast-M33comp;
        float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
        float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

        float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

        float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
        float WP1 = M1500adj;
        float FC1 = M33adj;
        float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

        float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
        float aA = exp(std::log(33.0f) + bB*std::log(FC1));

        float B = bB;
        float A = aA;

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
        float thetai1 = (FC1 + 0.7 * (thetast - FC1));

        ret->data[r][c] = B;
    }

    return ret;
}
inline static cTMap * AS_MedianGrainSize(cTMap * SAND, cTMap * CLAY)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {
        float sand = std::min(1.0f,std::max(0.0f,SAND->data[r][c]));
        float clay = std::min(1.0f - sand,std::max(0.0f,CLAY->data[r][c]));
        float silt = (1.0 - sand - clay);

        silt += std::max(0.0f,(1.0f - sand - clay - silt));

        float wsand = sand/0.035;
        float wclay = clay/0.001;
        float wsilt = silt/0.0035;

        float wtotal = wsand + wclay + wsilt;
        wsand = wsand/wtotal;
        wclay = wclay/wtotal;
        wsilt = wsilt/wtotal;

        ret->data[r][c] = 0.01*(0.5 * wsand +  0.0035 * wsilt +  0.001 * wclay);
    }

    return ret;
}


inline static cTMap * AS_TextureClass(cTMap * SAND, cTMap * CLAY)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {
        float sand = std::min(1.0f,std::max(0.0f,SAND->data[r][c]));
        float clay = std::min(1.0f - sand,std::max(0.0f,CLAY->data[r][c]));
        float silt = (1.0 - sand - clay);

        silt += std::max(0.0f,(1.0f - sand - clay - silt));

        sand = sand * 100.0;
        clay = clay * 100.0;
        silt = silt * 100.0;

        if((silt + 1.5*clay) < 15)
        {

            ret->data[r][c] = 0;// 'Sand';

        }
        else if((silt + 1.5*clay >= 15) && (silt + 2*clay < 30))
        {
            ret->data[r][c] = 1;// 'Loamy Sand';

        }
        else if((clay >= 7 && clay < 20) && (sand > 52) && ((silt + 2*clay) >= 30) || (clay < 7 && silt < 50 && (silt+2*clay)>=30))
        {
            ret->data[r][c] = 2;// 'Sandy Loam';

        }
        else if((clay >= 7 && clay < 27) && (silt >= 28 && silt < 50) && (sand <= 52))
        {
            ret->data[r][c] = 3;// 'Loam';
        }
        else if((silt >= 50 && (clay >= 12 && clay < 27)) || ((silt >= 50 && silt < 80) && clay < 12))
        {
            ret->data[r][c] = 4;// 'Silt Loam';
        }
        else if(silt >= 80 && clay < 12)
        {
            ret->data[r][c] = 5;// 'Silt';
        }
        else if((clay >= 20 && clay < 35) && (silt < 28) && (sand > 45))
        {
            ret->data[r][c] = 6;// 'Sandy Clay Loam';
        }
        else if((clay >= 27 && clay < 40) && (sand > 20 && sand <= 45))
        {
            ret->data[r][c] = 7;// 'Clay Loam';
        }
        else if((clay >= 27 && clay < 40) && (sand  <= 20))
        {
            ret->data[r][c] = 8;// 'Silty Clay Loam';
        }
        else if(clay >= 35 && sand > 45)
        {
            ret->data[r][c] = 9;// 'Sandy Clay';
        }
        else if(clay >= 40 && silt >= 40)
        {
            ret->data[r][c] = 10;// 'Silty Clay';
        }
        else if(clay >= 40 && sand <= 45 && silt < 40)
        {
            ret->data[r][c] = 11;// 'Clay';
        }
        else
        {
            ret->data[r][c] = -1;//'Undefined';
        }
    }

    return ret;
}

//0->sand
//1->Loamy Sand
//2->Sandy Loam
//3->Loam
//4->Silt Loam
//5->Silt
//6->Sandy Clay Loam
//7->Clay Loam
//8->Silty Clay Loam
//9->Sandy Clay
//10-> Silty Clay
//11->Clay

//from the source code for:
//https://www.nrcs.usda.gov/wps/portal/nrcs/detail/soils/survey/?cid=nrcs142p2_054167
/*function getTexture(sand, clay, silt, detailsc)
{
    sand = parseFloat(sand);
    clay = parseFloat(clay);
    silt = parseFloat(silt);

    if((silt + 1.5*clay) < 15)
    {

        if (detailsc)
        {
            getSand(document.info.vcs.value, document.info.cs.value, document.info.ms.value, document.info.fs.value, document.info.vfs.value);
        }
        else
        {
            document.info.texture.value = 'Sand';
        }
    }
    else if((silt + 1.5*clay >= 15) && (silt + 2*clay < 30))
    {
        if(detailsc)
        {
            getLoamSand(document.info.vcs.value, document.info.cs.value, document.info.ms.value, document.info.fs.value, document.info.vfs.value);
        }
        else
        {
            document.info.texture.value = 'Loamy Sand';
        }
    }
    else if((clay >= 7 && clay < 20) && (sand > 52) && ((silt + 2*clay) >= 30) || (clay < 7 && silt < 50 && (silt+2*clay)>=30))
    {
        if (detailsc)
        {
            getSandLoam(document.info.vcs.value, document.info.cs.value, document.info.ms.value, document.info.fs.value, document.info.vfs.value);
        }
        else
        {
            document.info.texture.value = 'Sandy Loam';
        }
    }
    else if((clay >= 7 && clay < 27) && (silt >= 28 && silt < 50) && (sand <= 52))
    {
        document.info.texture.value = 'Loam';
    }
    else if((silt >= 50 && (clay >= 12 && clay < 27)) || ((silt >= 50 && silt < 80) && clay < 12))
    {
        document.info.texture.value = 'Silt Loam';
    }
    else if(silt >= 80 && clay < 12)
    {
        document.info.texture.value = 'Silt';
    }
    else if((clay >= 20 && clay < 35) && (silt < 28) && (sand > 45))
    {
        document.info.texture.value = 'Sandy Clay Loam';
    }
    else if((clay >= 27 && clay < 40) && (sand > 20 && sand <= 45))
    {
        document.info.texture.value = 'Clay Loam';
    }
    else if((clay >= 27 && clay < 40) && (sand  <= 20))
    {
        document.info.texture.value = 'Silty Clay Loam';
    }
    else if(clay >= 35 && sand > 45)
    {
        document.info.texture.value = 'Sandy Clay';
    }
    else if(clay >= 40 && silt >= 40)
    {
        document.info.texture.value = 'Silty Clay';
    }
    else if(clay >= 40 && sand <= 45 && silt < 40)
    {
        document.info.texture.value = 'Clay';
    }
    else
    {
        document.info.texture.value = 'Undefined';
    }
}

function getSand(vcs, cs, ms, fs, vfs)
{
    vcs = parseFloat(vcs);
    cs = parseFloat(cs);
    ms = parseFloat(ms);
    fs = parseFloat(fs);
    vfs = parseFloat(vfs);

    vcscs = parseFloat(vcs) + parseFloat(cs);
    vcscsms = parseFloat(vcs) + parseFloat(cs) + parseFloat(ms);
    fsvfs = parseFloat(fs) + parseFloat(vfs);

    if(vcscs >= 25 && ms < 50 && fs < 50 && vfs < 50)
    {
        document.info.texture.value = 'Coarse Sand';
    }
    else if((vcscsms >= 25 && vcscs < 25 && fs < 50 && vfs < 50) || (ms >= 50 && vcscs >= 25))
    {
        document.info.texture.value = 'Sand';
    }
    else if((fs >= 50 && fs > vfs) || (vcscsms < 25 && vfs < 50))
    {
        document.info.texture.value = 'Fine Sand';
    }
    else if(vfs >= 50)
    {
        document.info.texture.value = 'Very Fine Sand';
    }
    else
    {
        document.info.texture.value = 'Error';
    }
}

function getLoamSand(vcs, cs, ms, fs, vfs)
{
    vcs = parseFloat(vcs);
    cs = parseFloat(cs);
    ms = parseFloat(ms);
    fs = parseFloat(fs);
    vfs = parseFloat(vfs);

    vcscs = parseFloat(vcs) + parseFloat(cs);
    vcscsms = parseFloat(vcs) + parseFloat(cs) + parseFloat(ms);
    fsvfs = parseFloat(fs) + parseFloat(vfs);

    if(vcscs >= 25 && ms < 50 && fs < 50 && vfs < 50)
    {
        document.info.texture.value = 'Loamy Coarse Sand';
    }
    else if(vcscsms >= 25 && vcscs < 25 && fs < 50 && vfs < 50)
    {
        document.info.texture.value = 'Loamy Sand';
    }
    else if(ms >= 50 && vcscs >= 25)
    {
        document.info.texture.value = 'Loamy Sand';
    }
    else if(fs >= 50)
    {
        document.info.texture.value = 'Loamy Fine Sand';
    }
    else if(vcscsms < 25 && vfs < 50)
    {
        document.info.texture.value = 'Loamy Fine Sand';
    }
    else if(vfs >= 50)
    {
        document.info.texture.value = 'Loamy Very Fine Sand';
    }
    else
    {
        document.info.texture.value = 'Error';
    }

}

function getSandLoam(vcs, cs, ms, fs, vfs)
{
    vcs = parseFloat(vcs);
    cs = parseFloat(cs);
    ms = parseFloat(ms);
    fs = parseFloat(fs);
    vfs = parseFloat(vfs);

    vcscs = parseFloat(vcs) + parseFloat(cs);
    vcscsms = parseFloat(vcs) + parseFloat(cs) + parseFloat(ms);
    fsvfs = parseFloat(fs) + parseFloat(vfs);

    if(vcscs >= 25 && ms < 50 && fs < 50 && vfs < 50)
    {
        document.info.texture.value = 'Coarse Sandy Loam';
    }
    else if(vcscsms >= 30 && vfs >= 30 && vfs < 50)
    {
        document.info.texture.value = 'Coarse Sandy Loam';
    }
    else if(vcscsms >= 30 && vcscs < 25 && fs < 30 && vfs < 30)
    {
        document.info.texture.value = 'Sandy Loam';
    }
    else if(vcscsms <= 15 && fs < 30 && vfs < 30 && fsvfs < 40)
    {
        document.info.texture.value = 'Sandy Loam';
    }
    else if(vcscs >= 25 && ms >= 50)
    {
        document.info.texture.value = 'Sandy Loam';
    }
    else if(fs >= 30 && vfs < 30 && vcscs < 25)
    {
        document.info.texture.value = 'Fine Sandy Loam';
    }
    else if(vcscsms >= 15 && vcscsms < 30 && vcscs < 25)
    {
        document.info.texture.value = 'Fine Sandy Loam';
    }
    else if(fsvfs >= 40 && fs >= vfs && vcscsms <= 15)
    {
        document.info.texture.value = 'Fine Sandy Loam';
    }
    else if(vcscs >= 25 && fs >= 50)
    {
        document.info.texture.value = 'Fine Sandy Loam';
    }
    else if(vfs >= 30 && vcscsms < 15 && vfs > fs)
    {
        document.info.texture.value = 'Very Fine Sandy Loam';
    }
    else if(fsvfs >= 40 && vfs > fs && vcscsms < 15)
    {
        document.info.texture.value = 'Very Fine Sandy Loam';
    }
    else if(vcscs >= 25 && vfs >= 50)
    {
        document.info.texture.value = 'Very Fine Sandy Loam';
    }
    else if(vcscsms >= 30 && vfs >= 50)
    {
        document.info.texture.value = 'Very Fine Sandy Loam';
    }
    else
    {
        document.info.texture.value = 'Error';
    }
}*/


#endif // RASTERSOIL_H
