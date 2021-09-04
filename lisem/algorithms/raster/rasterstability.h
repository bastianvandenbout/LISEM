#ifndef RASTERSTABILITY_H
#define RASTERSTABILITY_H

#include "geo/raster/map.h"
#include "random"
//infinite slope


inline static std::vector<cTMap*> AS_SlopeStabilityIS(cTMap * DEM, cTMap * soildepth, cTMap * coh, cTMap * ifa, cTMap * dens, cTMap * WH, float sfmax)
{
    cTMap * SF = DEM->GetCopy0();
    cTMap * FD = DEM->GetCopy0();

    float dx = DEM->cellSize();
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {

            if(!pcr::isMV(DEM->data[r][c]))
            {
                cTMap * Other = DEM;

                float x_11 = GetMapValue_OUTORMV3x3Av(Other,r-1,c-1);
                float x_12 = GetMapValue_OUTORMV3x3Av(Other,r-1,c);
                float x_13 = GetMapValue_OUTORMV3x3Av(Other,r-1,c+1);
                float x_21 = GetMapValue_OUTORMV3x3Av(Other,r,c-1);
                float x_22 = GetMapValue_OUTORMV3x3Av(Other,r,c);
                float x_23 = GetMapValue_OUTORMV3x3Av(Other,r,c+1);
                float x_31 = GetMapValue_OUTORMV3x3Av(Other,r+1,c-1);
                float x_32 = GetMapValue_OUTORMV3x3Av(Other,r+1,c);
                float x_33 = GetMapValue_OUTORMV3x3Av(Other,r+1,c+1);

                float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dx);

                float slope = std::sqrt(dzdx*dzdx + dzdy*dzdy);
                float sins = sin(atan(slope));
                float coss = cos(atan(slope));
                float tanphi =  tan(ifa->data[r][c]);
                float den = coh->data[r][c] + tanphi*(soildepth->data[r][c] * (dens->data[r][c]) - WH->data[r][c] * 1000.0f) * coss*coss;
                float div = (soildepth->data[r][c] * (dens->data[r][c]) + WH->data[r][c] * 1000.0f) * sins*coss;

                FD->data[r][c] = 0.0;
                if(div > 1e-12)
                {
                    SF->data[r][c] = den/div;

                    if(SF->data[r][c] < 1.0)
                    {

                        float fheight =  0.0f;

                        float swh = WH->data[r][c];
                        float sd = soildepth->data[r][c];

                        //get all relevant vvariables for calculation of stable depth
                        float wf = swh/sd;
                        float swf = swh/sd;
                        float wd = 1000.0f;
                        float hf = max(0.5f * sd,sd - slope * dx);
                        float cohf = coh->data[r][c];

                        float h0 = hf;
                        float hdx = 0.5f * dx;
                        float sde = dens->data[r][c];
                        float h1 = sd;
                        float h2 = sd;
                        float sc = 0.0f;

                        //inversion of safety factor equation (solved using trigonometric identies of squares in a sin/squares in a cosine)
                        //reduces to quadratic equation, of which two solutions can be found
                        try{


                                h1 = ((-cohf)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                                      cohf*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd -

                                      sqrt(-4.0f*dx*sde*swh*
                                         tanphi*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd) +
                                                pow(cohf*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                           cohf*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                         (2.0f*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd));
                                h2 = ((-cohf)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                                      cohf*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd +

                                      sqrt(-4.0f*dx*sde*swh*
                                         tanphi*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd) +
                                                pow(cohf*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                           cohf*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                         (2.0f*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd));

                                h1 = isnan(h1)? sd:h1;
                                h2 = isnan(h2)? sd:h2;

                                float hnew_select1 = max(h1,h2);
                                float hnew_select2 = min(h1,h2);

                                float hnew_select = hnew_select1;

                                float hnew = min(sd,max(sd * 0.5f,hnew_select));


                                fheight = min(sd,max(0.0f,sd - hnew));

                                FD->data[r][c] = fheight;


                        }catch( ...)
                        {
                            LISEMS_ERROR("Error when calculating potential failure depth");
                            throw 1;
                        }

                    }

                }else {
                    pcr::setMV(SF->data[r][c]);
                }
            }


        }
    }

    return {SF,FD};
}


inline static std::vector<cTMap*> AS_SlopeStabilityISS(cTMap * DEM, cTMap * soildepth, cTMap * coh, cTMap * ifa, cTMap * dens, cTMap * WH, cTMap * PGA, float sfmax)
{

    cTMap * SF = DEM->GetCopy0();
    cTMap * FD = DEM->GetCopy0();

    float dx = DEM->cellSize();
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                cTMap * Other = DEM;

                float x_11 = GetMapValue_OUTORMV3x3Av(Other,r-1,c-1);
                float x_12 = GetMapValue_OUTORMV3x3Av(Other,r-1,c);
                float x_13 = GetMapValue_OUTORMV3x3Av(Other,r-1,c+1);
                float x_21 = GetMapValue_OUTORMV3x3Av(Other,r,c-1);
                float x_22 = GetMapValue_OUTORMV3x3Av(Other,r,c);
                float x_23 = GetMapValue_OUTORMV3x3Av(Other,r,c+1);
                float x_31 = GetMapValue_OUTORMV3x3Av(Other,r+1,c-1);
                float x_32 = GetMapValue_OUTORMV3x3Av(Other,r+1,c);
                float x_33 = GetMapValue_OUTORMV3x3Av(Other,r+1,c+1);

                float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dx);

                float pga = PGA->data[r][c];
                float slope = std::sqrt(dzdx*dzdx + dzdy*dzdy);
                float sins = sin(atan(slope));
                float coss = cos(atan(slope));
                float tanphi =  tan(ifa->data[r][c]);
                float den = coh->data[r][c] -coss * sins*(pga/9.81)*
                        (
                            dens->data[r][c] *(soildepth->data[r][c] - WH->data[r][c]) + 1000.0f *(WH->data[r][c])
                        ) + tanphi*(soildepth->data[r][c] * (dens->data[r][c]) - WH->data[r][c] * 1000.0f) * coss*coss;
                float div =  coss*coss*(pga/9.81)*(dens->data[r][c] * soildepth->data[r][c] + 1000.0 *(WH->data[r][c])) + (soildepth->data[r][c] * (dens->data[r][c]) + WH->data[r][c] * 1000.0f) * sins*coss;

                FD->data[r][c] = 0.0;
                if(div > 1e-12)
                {
                    SF->data[r][c] = den/div;

                    if(SF->data[r][c] < 1.0)
                    {

                        float fheight =  0.0f;

                        float swh = WH->data[r][c];
                        float sd = soildepth->data[r][c];

                        //get all relevant vvariables for calculation of stable depth
                        float wf = swh/sd;
                        float swf = swh/sd;
                        float wd = 1000.0f;
                        float hf = max(0.5f * sd,sd - slope * dx);
                        float cohf = coh->data[r][c];

                        float h0 = hf;
                        float hdx = 0.5f * dx;
                        float sde = dens->data[r][c];

                        float sc = coh->data[r][c] + (pga/9.81) *(wd * wf + (1.0 - wf) *dens->data[r][c])*coss*sins *tanphi -  (pga/9.81) *(wd * wf + (1.0 - wf) *dens->data[r][c])*coss*coss;

                        //inversion of safety factor equation (solved using trigonometric identies of squares in a sin/squares in a cosine)
                        //reduces to quadratic equation, of which two solutions can be found

                            double h1 = soildepth->data[r][c];
                            double h2 = soildepth->data[r][c];

                            //if(!(SwitchSeismic || SwitchUpslopeForcing || SwitchDownslopeForcing))
                            {
                                //get all relevant vvariables for calculation of stable depth
                                double cif = cos(ifa->data[r][c]);
                                double sif = sin(ifa->data[r][c]);
                                double d = dx;
                                double dx2 = dx*dx;
                                double pp = 0.01f;
                                double pc = 0.01;;
                                double sd = dens->data[r][c];
                                double ws = 0.01f;

                                //threshold safety factor value
                                //include sf calibration (compensation factor to make sure that the initial state is stable)
                                double sf = 1.0;// / _SFCalibration->Drc;

                                //solution for stable depth at which safety factor equals a threshold value
                                double t1 = (2.0 *hf *pc *cif + d* pp* sf*cif + 2.0 *hf *sc *cif -
                                             d *hf *sd *sf * cif + 2.0 *hf *ws *cif -
                                             dx2 *sd *sif + dx2 *wd *wf *sif);
                                double t21 = (-2.0 *hf *pc *cif - d*pp*sf*cif - 2.0 *hf *sc *cif +
                                        d *hf *sd *sf*cif - 2.0 *hf *ws *cif + dx2 *sd *sif -
                                        dx2 *wd *wf *sif);
                                double t22 = (pc*cif + sc*cif - d*sf*sd*cif +
                                        ws*cif)*(dx2*pc*cif + hf*hf*pc*cif + d*hf*pp*sf*cif +
                                        dx2*sc*cif + hf*hf*sc*cif + dx2*ws*cif +
                                        hf*hf*ws*cif + dx2*pp*sif);

                                //in the end, it is nothin more than a really complex implementation of the abc-formula
                                // x1,2 = (-b +- Sqrt(b^2-4ac))/2a
                                //Two solutions for this formula, in our case: 1 positive, 1 negative, so we pick the positive value

                                double t2 = sqrt(t21*t21 - 4.0*t22);
                                double t3 = (2.0*(pc*cif + sc*cif - d*sd*sf*cif + ws*cif));

                                //positive and negative solution
                                h1 = (t1 + t2)/t3;
                                h2 = (t1 - t2)/t3;

                                h1 = std::isnan(h1)? 0.0:h1;
                                h2 = std::isnan(h2)? 0.0:h2;

                                //qDebug() << h1 << h2 << _DFForcing->Drc << _DFForcingUp->Drc;
                            }


                        float hnew_select1 = max(h1,h2);
                        float hnew_select2 = min(h1,h2);

                        float hnew_select = hnew_select2;

                        float hnew = min(sd,max(sd * 0.5f,hnew_select));


                        fheight = min(sd,max(0.0f,sd - hnew));

                        FD->data[r][c] = fheight;
                    }

                }else {
                    pcr::setMV(SF->data[r][c]);
                }
            }


        }
    }

    return {SF,FD};
}




inline static std::vector<cTMap*> AS_SlopeStabilityIFM(cTMap * DEMORG, cTMap * soildepthorg, cTMap * coh, cTMap * ifa, cTMap * dens, cTMap * WH, float sfmax, int iter)
{

    cTMap * soildepth = soildepthorg->GetCopy();
    cTMap * DEM = DEMORG->GetCopy();
    cTMap * SF = DEM->GetCopy0();
    cTMap * FD = DEM->GetCopy0();
    cTMap * FDT = DEM->GetCopy0();

    for(int i = 0; i < iter; i++)
    {
        float dx = DEM->cellSize();
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    cTMap * Other = DEM;

                    float x_11 = GetMapValue_OUTORMV3x3Av(Other,r-1,c-1);
                    float x_12 = GetMapValue_OUTORMV3x3Av(Other,r-1,c);
                    float x_13 = GetMapValue_OUTORMV3x3Av(Other,r-1,c+1);
                    float x_21 = GetMapValue_OUTORMV3x3Av(Other,r,c-1);
                    float x_22 = GetMapValue_OUTORMV3x3Av(Other,r,c);
                    float x_23 = GetMapValue_OUTORMV3x3Av(Other,r,c+1);
                    float x_31 = GetMapValue_OUTORMV3x3Av(Other,r+1,c-1);
                    float x_32 = GetMapValue_OUTORMV3x3Av(Other,r+1,c);
                    float x_33 = GetMapValue_OUTORMV3x3Av(Other,r+1,c+1);

                    float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                    float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dx);

                    float slope = std::sqrt(dzdx*dzdx + dzdy*dzdy);
                    float sins = sin(atan(slope));
                    float coss = cos(atan(slope));
                    float tanphi =  tan(ifa->data[r][c]);
                    float den = coh->data[r][c] + tanphi*(soildepth->data[r][c] * (dens->data[r][c]) - WH->data[r][c] * 1000.0f) * coss*coss;
                    float div = (soildepth->data[r][c] * (dens->data[r][c]) + WH->data[r][c] * 1000.0f) * sins*coss;

                    FD->data[r][c] = 0.0;
                    if(div > 1e-12)
                    {
                        if(i == 0)
                        {
                            SF->data[r][c] = den/div;
                        }else
                        {

                            SF->data[r][c] = std::min(SF->data[r][c],den/div);
                        }

                        if(den/div < 1.0)
                        {

                            float fheight =  0.0f;

                            float swh = WH->data[r][c];
                            float sd = soildepth->data[r][c];

                            //get all relevant vvariables for calculation of stable depth
                            float wf = swh/sd;
                            float swf = swh/sd;
                            float wd = 1000.0f;
                            float hf = max(0.5f * sd,sd - slope * dx);
                            float cohf = coh->data[r][c];

                            float h0 = hf;
                            float hdx = 0.5f * dx;
                            float sde = dens->data[r][c];
                            float h1 = sd;
                            float h2 = sd;
                            float sc = 0.0f;

                            //inversion of safety factor equation (solved using trigonometric identies of squares in a sin/squares in a cosine)
                            //reduces to quadratic equation, of which two solutions can be found
                            {


                                    h1 = ((-cohf)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                                          cohf*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd -

                                          sqrt(-4.0f*dx*sde*swh*
                                             tanphi*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd) +
                                                    pow(cohf*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                               cohf*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                             (2.0f*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd));
                                    h2 = ((-cohf)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                                          cohf*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd +

                                          sqrt(-4.0f*dx*sde*swh*
                                             tanphi*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd) +
                                                    pow(cohf*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                               cohf*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                             (2.0f*((-cohf)*sde + sde*sfmax - cohf*swf*wd + sfmax*swf*wd));

                                    h1 = isnan(h1)? sd:h1;
                                    h2 = isnan(h2)? sd:h2;

                            }

                            float hnew_select1 = max(h1,h2);
                            float hnew_select2 = min(h1,h2);

                            float hnew_select = hnew_select1;

                            float hnew = min(sd,max(sd * 0.5f,hnew_select));


                            fheight = min(sd,max(0.0f,sd - hnew));

                            FD->data[r][c] = fheight;
                        }else
                        {

                            FD->data[r][c] = 0.0;
                        }

                    }else {
                        FD->data[r][c] = 0.0;
                    }
                }


            }
        }

        bool has_failure= false;

        //#pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    if(FD->data[r][c] > 0.05)
                    {
                        has_failure = true;

                        DEM->data[r][c] -= FD->data[r][c];
                        soildepth->data[r][c] -= FD->data[r][c];
                        FDT->data[r][c] += FD->data[r][c];
                    }
                }
            }
        }
        if(!has_failure)
        {
            break;
        }
    }

    return {SF,FDT};

}






//elevation (first solution to quadratic euqation)
//of bottom of rotated scaled ellipsoid
//scale a,b and c in x,y and z direction.
//Rotation theta is clockwise around z-axis.
//Rotation theta2 is clockwise around x-axis
//if the solution is imaginary, the sampling point is outside of ellipsoid
/*
1 = 1/(2 (cost2^2/c2 +
       sint2^2/
        a2)) (-((2 x cost cost2 sint2)/
       a2) + (2 x cost cost2 sint2)/
     c2 + (2 y cost2 sint sint2)/
     a2 - (2 y cost2 sint sint2)/
     c2 - \[Sqrt](((2 x cost cost2 sint2)/
           a2 - (2 x cost cost2 sint2)/
           c2 - (2 y cost2 sint sint2)/
           a2 + (2 y cost2 sint sint2)/
           c2)^2 -
       4 (cost2^2/c2 +
          sint2^2/a2) (-1 + (y^2 cost^2)/
           b2 + (x^2 cost^2 cost2^2)/
           a2 + (2 x y cost sint)/
           b2 - (2 x y cost cost2^2 sint)/
           a2 + (x^2 sint^2)/
           b2 + (y^2 cost2^2 sint^2)/
           a2 + (x^2 cost^2 sint2^2)/
           c2 - (2 x y cost sint sint2^2)/
           c2 + (y^2 sint^2 sint2^2)/c2)))
*/

inline static float Sample_Ellipsoid_Z_Bottom(float x, float y, float a, float b, float c, float Theta, float Theta2)
{

    float sint = std::sin(Theta);
    float sint2 = std::sin(Theta2);
    float cost = std::cos(Theta2);
    float cost2 = std::cos(Theta2);
    float sint_2 = sint*sint;
    float sint2_2 = sint2*sint2;
    float cost_2 = cost*cost;
    float cost2_2 = cost2*cost2;
    float a2 = a*a;
    float b2 = b*b;
    float c2 = c*c;

    float bsquare = (2.0* x* cost* cost2* sint2)/
            a2
         - (2* x* cost* cost2* sint2)/
            c2
         - (2* y* cost2* sint* sint2)/
            a2
         + (2* y* cost2* sint* sint2)/
            c2;

    float ac4 = 4.0* (cost2_2/c2 + sint2_2/a2)*
            (-1.0 + (y*y *cost_2)/b2
             + (x*x *cost_2* cost2_2)/a2
             + (2.0* x *y* cost* sint)/b2
             - (2.0* x* y* cost* cost2_2* sint)/a2
             + (x*x *sint_2)/b2
             + (y*y *cost2_2* sint_2)/a2
             + (x*x *cost_2 *sint2_2)/c2
             - (2.0* x *y* cost* sint *sint2_2)/c2
             + (y*y *sint_2 *sint2_2)/c2)
                   ;

    if(bsquare * bsquare <  ac4 )
    {
        float mv;
        pcr::setMV(mv);
        return mv;
    }


    float asquare = -((2.0 *x*cost* cost2* sint2)/
                      a2)
                       + (2.0* x* cost* cost2* sint2)/
                    c2 +
                       (2.0 *y* cost2 *sint* sint2)/
                    a2 -
                       (2.0* y* cost2* sint* sint2)/
                    c2;

   float z1 = 1.0/(2.0 *(cost2_2/c2 +
           sint2_2/
            a2))
            *
            (
            asquare
            -
            std::sqrt(
            bsquare *bsquare
            -
          ac4
           )
         );

   /*float z2 = 1.0/(2.0 (cost2_2/c2 +
           sint2_2/
            a2))
            *
            (
            asquare
            +
            std::sqrt(
            bsquare *bsquare
            -
          ac4
           )
         );*/

   if(!std::isfinite(z1))
   {
       float mv;
       pcr::setMV(mv);
       return mv;
   }else {
       return z1;
   }


}


inline static void Sample_Slope(cTMap * DEM, float &s_x1, float &s_x2, float &s_y1, float &s_y2, int radius_pixels_x, int radius_pixels_y, int r, int c, float dx, float dy)
{

    if(pcr::isMV(DEM->data[r][c]))
    {
        return;
    }
    float z = DEM->data[r][c];

    if(r - radius_pixels_y > 0)
    {
        if(!pcr::isMV(DEM->data[r-radius_pixels_y][c]))
        {
            s_y1 = -(DEM->data[r-radius_pixels_y][c] - z)/(radius_pixels_y * dy);
        }
    }else if(r -1 > 0)
    {
        if(!pcr::isMV(DEM->data[r-1][c]))
        {
            s_y1 =- (DEM->data[r-1][c]-z)/dy;
        }
    }
    if(r + radius_pixels_y < DEM->nrRows())
    {
        if(!pcr::isMV(DEM->data[r+radius_pixels_y][c]))
        {
            s_y2 = -(z-DEM->data[r+radius_pixels_y][c])/(radius_pixels_y * dy);
        }
    }else if(r +1 < DEM->nrRows())
    {
        if(!pcr::isMV(DEM->data[r+1][c]))
        {
            s_y2 =-(z-DEM->data[r+1][c])/dy;
        }
    }
    if(c - radius_pixels_x > 0)
    {
        if(!pcr::isMV(DEM->data[r][c-radius_pixels_x]))
        {
            s_x1 = -(DEM->data[r][c-radius_pixels_x] - z)/(radius_pixels_x * dx);
        }
    }else if(c -1 > 0)
    {
        if(!pcr::isMV(DEM->data[r][c-1]))
        {
            s_x1 = -(DEM->data[r][c-1]-z)/dx;
        }
    }
    if(c + radius_pixels_x < DEM->nrCols())
    {
        if(!pcr::isMV(DEM->data[r][c+radius_pixels_x]))
        {
            s_x2 = -(z-DEM->data[r][c+radius_pixels_x])/(radius_pixels_x * dx);
        }
    }else if(c +1 < DEM->nrCols())
    {
        if(!pcr::isMV(DEM->data[r][c+1]))
        {
            s_x2 = -(z-DEM->data[r][c+1])/dx;
        }
    }



}


//ellipsoid sampling
//minimum
//probability
//depth of minimum
//ellipsoid properties of minimum pos1
//ellipsoid properties of minimum pos2
//ellipsoid properties of minimum pos3
//ellipsoid properties of minimum size1
//ellipsoid properties of minimum size2
//ellipsoid properties of minimum size3
//ellipsoid properties of minimum rot1
//ellipsoid properties of minimum rot2
inline static std::vector<cTMap*> AS_SlopeStabilityRES(cTMap * DEM, std::vector<cTMap*> soildepth, std::vector<cTMap*> coh, std::vector<cTMap*> ifa, std::vector<cTMap*> dens, std::vector<cTMap*> sat, cTMap * WH,
                                                 float sample_density, float h_min, float h_max, float size_min, float size_max, float size_lat_min, float size_lat_max, float size_vert_min, float size_vert_max,
                                                 float rot_min, float rot_max, float rot_lat_min, float rot_lat_max, float vol_min)
{

    cTMap * SF = DEM->GetCopy0();
    SF->setAllMV();
    cTMap * ProbabilityN = DEM->GetCopy0();
    cTMap * Probability = DEM->GetCopy0();
    cTMap * Depth = DEM->GetCopy0();
    cTMap * ellipsoid_x = DEM->GetCopy0();
    cTMap * ellipsoid_y = DEM->GetCopy0();
    cTMap * ellipsoid_z = DEM->GetCopy0();
    cTMap * ellipsoid_a = DEM->GetCopy0();
    cTMap * ellipsoid_b = DEM->GetCopy0();
    cTMap * ellipsoid_c = DEM->GetCopy0();
    cTMap * ellipsoid_t1 = DEM->GetCopy0();
    cTMap * ellipsoid_t2 = DEM->GetCopy0();

    cTMap * DEMBottom = DEM->GetCopy();
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                for(int  i = 0; i < soildepth.size(); i++)
                {
                    DEMBottom->data[r][c] -= soildepth.at(i)->data[r][c];
                }
            }

        }
    }

    std::vector<int> cols;
    std::vector<int> rows;
    int n_cells = 0;
    //total number of cells
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                cols.push_back(c);
                rows.push_back(r);
            }
        }
    }
    n_cells = cols.size();
    //random number generator, 32 bit mersenne twister
    std::random_device dev;
    std::mt19937 rng(dev());

    QMutex m;
    std::uniform_int_distribution<std::mt19937::result_type> intgen(0,n_cells-1); // distribution in range [1, 6]
    std::uniform_real_distribution<double> realgen(0.0,1.0);

    //total number of ellipsoids
    int cells_per_ellips = (0.5 * (size_max + size_max)) * (0.5 * (size_lat_max + size_lat_max))/(std::fabs(DEM->cellSizeX()) * std::fabs(DEM->cellSizeY()));
    cells_per_ellips = std::max(1,cells_per_ellips);
    int n_ellips = sample_density * n_cells/ cells_per_ellips;

    float dx = std::fabs(DEM->cellSizeX());
    float dy = std::fabs(DEM->cellSizeY());
    //just an assumption here
    //most efficient number of locks for the cell array is equal to the number of threads

     std::cout << "number of ellipsoids " << n_ellips << " " << sample_density << " " << n_cells << "  " << cells_per_ellips << std::endl;

    //for each ellipsoid
#pragma omp parallel for collapse(1)
    for(int i = 0; i < n_ellips; i++)
    {
        //get parameters

        double h = h_min + (h_max -h_min)*realgen(rng);
        double size = size_min + (size_max -size_min)*realgen(rng);
        double size_lat = size_lat_min + (size_lat_max -size_lat_min)*realgen(rng);
        double size_vert = size_vert_min + (size_vert_max -size_vert_min)*realgen(rng);
        double rot = rot_min + (rot_max -rot_min)*realgen(rng);
        double rot_lat = rot_lat_min + (rot_lat_max -rot_lat_min)*realgen(rng);
        int cellid = intgen(rng);

        double rand_radius = 0.5 + 0.5 * realgen(rng) * size;


        //get center cell location
        int r = rows[cellid];
        int c = cols[cellid];

        //place randomly within cell
        //float x = DEM->west() + DEM->cellSizeX() * ((float)(r));
        //float y = DEM->north() + DEM->cellSizeY() * ((float)(c));

        float x_offset = realgen(rng) *dx;
        float y_offset = realgen(rng) *dy;

        //get extent based on size

        int rand_radius_pixels_x = rand_radius/dx;
        int rand_radius_pixels_y = rand_radius/dy;

        int radius_pixels_x = std::max(size,size_lat)/dx;
        int radius_pixels_y = std::max(size,size_lat)/dy;

        //calculate slope over the distance of the ellipsoid

        float s_x1 = 0.0;
        float s_x2 = 0.0;
        float s_y1 = 0.0;
        float s_y2 = 0.0;


        Sample_Slope(DEM,s_x1,s_x2,s_y1,s_y2,radius_pixels_x,radius_pixels_y,r,c,dx,dy);

        float sx = (s_x1 + s_x2)*0.5;
        float sy = (s_y1 + s_y2)*0.5;


        float aspect = atan2(sy,sx); //angle in radians, anti-clockwise relative to positive x-acis
        float angle = atan(std::sqrt(sx*sx+sy*sy)); // angle in radians, compared to flat

        int rmin = std::max(0,r - radius_pixels_y);
        int rmax = std::min(DEM->nrRows(),r + radius_pixels_y);
        int cmin = std::max(0,c - radius_pixels_x);
        int cmax = std::min(DEM->nrCols(),c + radius_pixels_x);

        float elev = DEM->data[r][c];

        float den_total = 0.0;
        float div_total = 0.0;
        float volume = 0.0;

        for(int r2 = rmin; r2 < rmax; r2++)
        {
            for(int c2 = cmin; c2 < cmax; c2++)
            {
                if(!(r2 >-1 && r2 < DEM->nrRows() && c2 > -1 && c2 < DEM->nrCols()))
                {
                    continue;
                }
                if(pcr::isMV(DEM->data[r2][c2]))
                {
                    continue;
                }
                //get elevation (if any)
                float x = (c2-c)*dx - x_offset;
                float y = (r2-r)*dy - y_offset;

                float z = elev + h + Sample_Ellipsoid_Z_Bottom(x,y,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);
                float z_x1 = elev + h + Sample_Ellipsoid_Z_Bottom(x- 1e-6,y,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);
                float z_x2 = elev + h + Sample_Ellipsoid_Z_Bottom(x+ 1e-6,y,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);
                float z_y1 = elev + h + Sample_Ellipsoid_Z_Bottom(x,y-1e-6,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);
                float z_y2 = elev + h + Sample_Ellipsoid_Z_Bottom(x,y+1e-6,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);


                if(!pcr::isMV(z))
                {
                    float sz_x1 = !pcr::isMV(z_x1)? ((z-z_x1)/dx):0.0;
                    float sz_x2 = !pcr::isMV(z_x2)? ((z_x2-z)/dx):0.0;
                    float sz_y1 = !pcr::isMV(z_y1)? ((z-z_y1)/dy):0.0;
                    float sz_y2 = !pcr::isMV(z_y2)? ((z_y2-z)/dy):0.0;
                    float szx = (sz_x1 + sz_x2)*0.5;
                    float szy = (sz_y1 + sz_y2)*0.5;


                    //calculate slope over the distance of the ellipsoid
                    float s2_x1 = 0.0;
                    float s2_x2 = 0.0;
                    float s2_y1 = 0.0;
                    float s2_y2 = 0.0;

                    Sample_Slope(DEM,s_x1,s_x2,s_y1,s_y2,radius_pixels_x,radius_pixels_y,r,c,dx,dy);

                    float s2x = (s2_x1 + s2_x2)*0.5;
                    float s2y = (s2_y1 + s2_y2)*0.5;
                    float Slope2 = std::sqrt(s2x*s2x+s2y*s2y);
                    float Area = (dx*dy) /std::cos(atan(Slope2));
                    float Slopealphax = cos(aspect) * s2x;
                    float Slopealphay = -sin(aspect) * s2y;
                    float Slopealpha = std::sqrt(Slopealphax*Slopealphax +Slopealphay*Slopealphay);

                    //check the properties at this elevation
                    if(z < DEMBottom->data[r][c])
                    {
                        z =std::max(DEMBottom->data[r][c],z);
                        szx = s2x;
                        szy = s2y;
                    }

                    LSMVector3 slip = LSMVector3(szx > 0.0? -1.0:1.0,szy > 0.0? -1.0:1.0,std::sqrt(szx*szx+szy*szy));
                    LSMVector3 seep = LSMVector3(s2x > 0.0? -1.0:1.0,s2y > 0.0? -1.0:1.0,std::sqrt(s2x*s2x+s2y*s2y));

                    float angle_seep_slip = acos(slip.Normalize().dot(seep.Normalize()));

                    float betac = atan(std::sqrt(szx*szx+szy*szy));
                    float betam = atan(Slopealpha);
                    float coh_this = 0.0;
                    float ifa_this = 0.0;
                    float weight = 0.0;
                    float wz = WH->data[r2][c2] + DEMBottom->data[r2][c2];
                    float wz_z = std::max(0.0f,wz - z);

                    float seep_d = 0.0;
                    float seep_c = 0.0;

                    float z_this = DEMBottom->data[r2][c2];
                    float z_prev = z_this;
                    bool first = true;
                    for(int l = 0; l < soildepth.size(); l++)
                    {
                        float z_this_next = z_this + soildepth.at(l)->data[r2][c2];
                        if(z < z_this_next)
                        {
                            float height = std::min(z_this_next-z_this,z_this_next - z);
                            if(first)
                            {
                                coh_this = coh.at(l)->data[r2][c2];
                                ifa_this = ifa.at(l)->data[r2][c2];
                            }
                            if(height > 0.0)
                            {
                                float sat_this = sat.at(l)->data[r2][c2];
                                float dens_this = dens.at(l)->data[r2][c2];
                                weight += dens_this * height * dx*dy;
                                weight += sat_this * height * dx*dy * 1000.0;
                                volume += height * dx * dy;
                            }
                        }

                        z_prev = z_this;
                        z_this = z_this_next;
                    }
                    weight -= wz_z * 1000.0 * dx * dy;
                    weight = std::max(weight,0.0f);
                    seep_c = 1000.0 * wz_z * sin(angle_seep_slip);
                    seep_d = 1000.0 * wz_z * cos(angle_seep_slip);

                    den_total += (coh_this * Area + tan(ifa_this) * weight * cos(betac) + seep_c) * cos(betam);
                    div_total += cos(betam)*(weight * sin(betam) + seep_d);
                }
            }
        }

        if(div_total > 0.0 && volume > vol_min)
        {
            float FoS = den_total/div_total;

            //finally, loop again over all cells, and inform them of our found data
            for(int r2 = rmin; r2 < rmax; r2++)
            {
                for(int c2 = cmin; c2 < cmax; c2++)
                {
                    if(!(r2 >-1 && r2 < DEM->nrRows() && c2 > -1 && c2 < DEM->nrCols()))
                    {
                        continue;
                    }
                    if(pcr::isMV(DEM->data[r2][c2]))
                    {
                        continue;
                    }
                    //get elevation (if any)
                    float x = (c2-c)*dx - x_offset;
                    float y = (r2-r)*dy - y_offset;

                    float zbot = Sample_Ellipsoid_Z_Bottom(x,y,size,size_lat,size_vert,-aspect+rot_lat,angle + rot);
                    if(!pcr::isMV(zbot))
                    {

                            float z = elev + h + zbot;
                            z =std::max(DEMBottom->data[r][c],z);

                            float height = DEM->data[r2][c2] - z;

                            if(!pcr::isMV(z) && height > 0.0f)
                            {
                                m.lock();
                                bool do_write = false;

                                if(pcr::isMV(SF->data[r2][c2]))
                                {
                                    do_write = true;
                                    SF->data[r2][c2] = FoS;

                                    Probability->data[r][c] += 1.0;

                                }else if(SF->data[r2][c2] > FoS)
                                {
                                    do_write = true;
                                    SF->data[r2][c2] = FoS;

                                    Probability->data[r2][c2] += 1.0;
                                }
                                ProbabilityN->data[r2][c2] += 1.0;

                                if(do_write)
                                {
                                    Depth->data[r2][c2] = height;
                                    ellipsoid_y->data[r2][c2] = DEM->north() + r * DEM->cellSizeY() + y_offset;
                                    ellipsoid_x->data[r2][c2] = DEM->west() + r * DEM->cellSizeX() + x_offset;
                                    ellipsoid_z->data[r2][c2] = elev + h;
                                    ellipsoid_a->data[r2][c2] = size;
                                    ellipsoid_b->data[r2][c2] = size_lat;
                                    ellipsoid_c->data[r2][c2] = size_vert;
                                    ellipsoid_t1->data[r2][c2] = rot_lat;
                                    ellipsoid_t2->data[r2][c2] = rot;
                                }
                                m.unlock();
                            }
                    }
                }
            }

        }

    }

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(ProbabilityN->data[r][c] > 0.0)
            {
                Probability->data[r][c] = Probability->data[r][c]/ProbabilityN->data[r][c];
            }
        }
    }

    delete ProbabilityN;


    return {SF,Depth,Probability,ellipsoid_x,ellipsoid_y,ellipsoid_z,ellipsoid_a,ellipsoid_b,ellipsoid_c,ellipsoid_t1,ellipsoid_t2};

}

//fos
//depth
inline static cTMap * AS_SlopeStabilityEllipsoid(cTMap * DEM, std::vector<cTMap*>soildepth, std::vector<cTMap*>coh, std::vector<cTMap*>ifa, std::vector<cTMap*> dens, cTMap * WH,
                                                 float pos1, float pos2, float pos3, float size1, float size2, float rot1, float rot2)
{




    return nullptr;
}








#endif // RASTERSTABILITY_H
