#ifndef RASTERSOIL_H
#define RASTERSOIL_H





#include "geo/raster/map.h"



inline static cTMap * AS_SaxtonKSat(cTMap * SAND, cTMap * CLAY, cTMap * ORGANIC, cTMap * GRAVEL, float densityfactor = 1.0)
{
    cTMap * ret = SAND->GetCopy0();

    FOR_ROW_COL_MV(SAND)
    {

        float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
        float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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

        float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + Saturation->data[r][c] * (thetast - FC1)),-bB));
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
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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
        float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

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
