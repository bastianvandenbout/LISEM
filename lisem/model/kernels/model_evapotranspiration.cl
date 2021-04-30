#define GRAV 9.81

float minmod(float a, float b)
{
        float rec = 0.0f;
        if (a >= 0.0f && b >= 0.0f)
        {
        rec = min(a, b);
        }else if(a <= 0.0f && b <= 0.0f)
        {
        rec = max(a, b);
        }
        return rec;
}



kernel
void InfilOnly( int dim0,
                                int dim1,
                                float dx,
                                float dt,  
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __read_only image2d_t DEM,
                                __read_only image2d_t H,
                                __read_only image2d_t KSAT_T,
                                __read_only image2d_t GW_WF,
                                __read_only image2d_t F_InfilAct,
                                __write_only image2d_t Hn,
                                __write_only image2d_t GW_WFN,
                                __write_only image2d_t F_InfilPot,
                                __write_only image2d_t INFIL_CUM,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                float rain
                                )
{

    float tx = dt/dx;

    const float n = 0.1;

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    const int id_1d = (int)(get_global_id(0));

    const int idx = trunc((float)(id_1d)/(float)(dim1));
    const int idy = id_1d % dim1;

    const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
    const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

    int x = trunc(read_imagef(LOCX,sampler, (int2)(gx_x,gy_y)).x);
    int y = trunc(read_imagef(LOCY,sampler, (int2)(gx_x,gy_y)).x);

    const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
    const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    float h = read_imagef(H,sampler, (int2)(gx,gy)).x;
    float ksat = read_imagef(KSAT_T,sampler, (int2)(gx,gy)).x;
    float WFH = read_imagef(GW_WF,sampler, (int2)(gx,gy)).x;
    float actualinfiltration = read_imagef(F_InfilAct,sampler, (int2)(gx,gy)).x;

    h = h + rain;

    float infil = actualinfiltration;
    //exchange fluxes
    WFH = WFH + infil;
    h = max(0.0f,(float)(h));

    float ksat_wfcomp = ksat;
    float infil_pot = min((float)(h * 0.5),(float)(ksat * dt));


    write_imagef(Hn, (int2)(gx,gy), h);
    write_imagef(F_InfilPot, (int2)(gx,gy), infil_pot);
    write_imagef(GW_WFN, (int2)(gx,gy), WFH);
    write_imagef(INFIL_CUM, (int2)(gx,gy), 0);


    float ui = 0;
    if(ui_image == 2)
    {
            ui = WFH;
            write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
    }else if(ui_image == 4)
    {
            ui = infil *1000.0 *3600.0/dt ;//actualinfiltration;//actualinfiltration;
            write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
    }

}

kernel
void InfilOnly2( int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __write_only image2d_t H,
                                __write_only image2d_t GW_WF,
                                __read_only image2d_t Hn,
                                __read_only image2d_t GW_WFN
                                )
{

        //copy calculated output values back to original maps


        float tx = dt/dx;

        const float n = 0.1;

        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;


        const int id_1d = (int)(get_global_id(0));

        const int idx = trunc((float)(id_1d)/(float)(dim1));
        const int idy = id_1d % dim1;

        const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
        const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

        int x = trunc(read_imagef(LOCX,sampler, (int2)(gx_x,gy_y)).x);
        int y = trunc(read_imagef(LOCY,sampler, (int2)(gx_x,gy_y)).x);

        const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
        const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

        float hn = read_imagef(Hn,sampler, (int2)(gx,gy)).x;
        float wfh = read_imagef(GW_WFN,sampler, (int2)(gx,gy)).x;

        write_imagef(H, (int2)(gx,gy), hn);
        write_imagef(GW_WF, (int2)(gx,gy), wfh);


}




kernel
void hydrology( int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __read_only image2d_t DEM,
                                __read_only image2d_t H,
                                __read_only image2d_t SCANOPY,
                                __read_only image2d_t SSURFACE,
                                __read_only image2d_t GW_WF,
                                __read_only image2d_t F_InfilAct,
                                __read_only image2d_t GW_US,
                                __read_only image2d_t GW_S,
                                __read_only image2d_t THETAS,
                                __read_only image2d_t THETAR,
                                __read_only image2d_t SoilDepth,
                                __read_only image2d_t KSAT_T,
                                __read_only image2d_t KSAT_B,
                                __read_only image2d_t SWR_A,
                                __read_only image2d_t SWR_B,
                                __read_only image2d_t SMAX_CANOPY,
                                __read_only image2d_t SMAX_SURFACE,
                                __read_only image2d_t CH_MASK,
                                __read_only image2d_t CH_WIDTH,
                                __read_only image2d_t CH_HEIGHT,
                                __read_only image2d_t CH_H,
                                __write_only image2d_t Hn,
                                __write_only image2d_t SCANOPYN,
                                __write_only image2d_t SSURFACEN,
                                __write_only image2d_t F_InfilPot,
                                __write_only image2d_t GW_WFN,
                                __write_only image2d_t GW_USN,
                                __write_only image2d_t GW_SN,
                                __write_only image2d_t INFIL_CUM,
                                __write_only image2d_t CH_HN,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                float rain
                                )
{

        //sampler and coordinates

        float tx = dt/dx;

        const float n = 0.1;

        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

        const int id_1d = (int)(get_global_id(0));

        const int idx = trunc((float)(id_1d)/(float)(dim1));
        const int idy = id_1d % dim1;

        const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
        const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

        int x = trunc(read_imagef(LOCX,sampler, (int2)(gx_x,gy_y)).x);
        int y = trunc(read_imagef(LOCY,sampler, (int2)(gx_x,gy_y)).x);

        const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
        const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

        const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
        const int gy_x1 = gy;
        const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
        const int gy_x2 = gy;
        const int gx_y1 = gx;
        const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
        const int gx_y2 = gx;
        const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));


        //sample input map values


        float h = read_imagef(H,sampler, (int2)(gx,gy)).x;
        float store_canopy = read_imagef(SCANOPY,sampler, (int2)(gx,gy)).x;
        float store_surface = read_imagef(SSURFACE,sampler, (int2)(gx,gy)).x;
        float WFH = read_imagef(GW_WF,sampler, (int2)(gx,gy)).x;
        float actualinfiltration = read_imagef(F_InfilAct,sampler, (int2)(gx,gy)).x;
        float GWUH = read_imagef(GW_US,sampler, (int2)(gx,gy)).x;
        float GWH = read_imagef(GW_S,sampler, (int2)(gx,gy)).x;
        float ThetaS = min(0.9f,(float)(max(0.1f,(float)(read_imagef(THETAS,sampler, (int2)(gx,gy)).x))));
        float ThetaR = read_imagef(THETAR,sampler, (int2)(gx,gy)).x;
        float SD = read_imagef(SoilDepth,sampler, (int2)(gx,gy)).x;
        float KSattop = read_imagef(KSAT_T,sampler, (int2)(gx,gy)).x;
        float KSat = read_imagef(KSAT_B,sampler, (int2)(gx,gy)).x;
        float A = read_imagef(SWR_A,sampler, (int2)(gx,gy)).x;
        float B = read_imagef(SWR_B,sampler, (int2)(gx,gy)).x;
        float Smaxc = read_imagef(SMAX_CANOPY,sampler, (int2)(gx,gy)).x;
        float Smaxs = read_imagef(SMAX_SURFACE,sampler, (int2)(gx,gy)).x;
        float chm = read_imagef(CH_MASK,sampler, (int2)(gx,gy)).x;
        float chh = read_imagef(CH_H,sampler, (int2)(gx,gy)).x;
        float chwidth = read_imagef(CH_WIDTH,sampler, (int2)(gx,gy)).x;
        float chheight = read_imagef(CH_HEIGHT,sampler, (int2)(gx,gy)).x;
        //float chv = read_imagef(CH_V,sampler, (int2)(gx,gy)).x;

        h = h + store_canopy;

        float GWTheta = max((float)(ThetaR), min((float)(ThetaS),(float)(GWUH/max((float)(0.01),(float)((SD - WFH/ThetaS - GWH/ThetaS))))));

        float infil = actualinfiltration;
        //exchange fluxes
        WFH = WFH + infil;
        h = max(0.0f,(float)(h));
        //exchange fluxes
        float depth_add = infil/max(0.001f,(ThetaS - GWTheta));
        float flux_add =depth_add * GWTheta;
        WFH =WFH + flux_add;
        GWUH = GWUH - flux_add;

        GWTheta = max((float)(ThetaR), min((float)(ThetaS),(float)(GWUH/max((float)(0.01),(float)((SD - WFH/ThetaS - GWH/ThetaS))))));

        h = h + rain;

        //calculate infiltration
        float space_infil = max((float) (0.0),(float)(ThetaS * SD - WFH - GWH - GWUH));
        float GA_PSI = min((float)(1500.0),max((float)(10.0),(float)(A*pow((float)(GWTheta),(float)(-B))/100.0)))/(1000.0 * 9.81);
        float ksat_wfcomp = KSattop * (1.0f + (GA_PSI * max(0.0f,(float)(ThetaS - GWTheta)))/(max(0.01f,(float)(WFH/ThetaS))));
        float infil_pot = min((float)(h * 0.5),min((float)(space_infil * 0.5),(float)(ksat_wfcomp * dt)));


        //calculate effective ground water saturation in unsaturated layer
        GWTheta = max((float)(ThetaR), min((float)(ThetaS),(float)(GWUH/max((float)(0.01),(float)((SD - WFH/ThetaS - GWH/ThetaS))))));


        //percolation


        //calculate percolation rates
        //average of top unsaturated infiltration rate and wetting front compensated infiltration rate
        float KUnSat_perc1 = 0.5f *(KSattop * pow((float)(GWTheta/ThetaS),(float)(3.0 + 2.0*B)) + ksat_wfcomp);
        float KUnSat_perc2 = KSat * pow((float)(GWTheta/ThetaS),(float)(3.0 + 2.0*B));

        //depth of unsaturated layer
        float US_Depth = max(0.01f,(SD - WFH/ThetaS - GWH/ThetaS));
        //available water for percolation, at least residual water content must be left
        float Perc_available = max(0.0f,(float)(GWUH - max(0.01f,(float)((SD - WFH/ThetaS - GWH/ThetaS))) * ThetaR));

        //actual percolation from wetting front to unsaturated
        float perc1 = max(0.0f,min((float)(WFH * 0.25),(float)(KUnSat_perc1 * dt)));

        //actual percolation from unsaturated layer to saturated layer
        float perc2 = max(0.0f,min((float)(Perc_available * 0.5),(float)(KUnSat_perc1 * dt)));

        //move the actual water fluxes
        WFH = max(0.0f,(float)(WFH - perc1));
        GWUH = max(0.0f,(float)(GWUH - perc2 + perc1));
        GWH = max(0.0f,(float)(GWH + perc2));

        //calculate new water content
        GWTheta = max(ThetaR, min(ThetaS,GWUH/max(0.01f,(float)(SD - WFH/ThetaS - GWH/ThetaS))));

        //combine percolation for later output
        float PERC = perc1 + perc2;


        float ui = 0;
        if(ui_image == 1)
        {
                ui = h;
                write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
        }else if(ui_image == 2)
        {
                ui = WFH;
                write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
        }else if(ui_image == 3)
        {
                ui = GWH;
                write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
        }else if(ui_image == 4)
        {
                ui = infil *1000.0 *3600.0/dt ;//actualinfiltration;//actualinfiltration;
                write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
        }else if(ui_image == 5)
        {
                ui = chh;
                write_imagef(OUTPUT_UI, (int2)(gx,gy), ui);
        }

        //write output values to maps


        write_imagef(Hn, (int2)(gx,gy), h);
        write_imagef(SCANOPYN, (int2)(gx,gy), 0);
        write_imagef(SSURFACEN, (int2)(gx,gy), 0);
        write_imagef(F_InfilPot, (int2)(gx,gy), infil_pot);
        write_imagef(GW_WFN, (int2)(gx,gy), WFH);
        write_imagef(GW_USN, (int2)(gx,gy), GWUH);
        write_imagef(GW_SN, (int2)(gx,gy), GWH);
        write_imagef(INFIL_CUM, (int2)(gx,gy), 0);
}

kernel
void hydrology2( int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __write_only image2d_t H,
                                __write_only image2d_t SCANOPY,
                                __write_only image2d_t SSURFACE,
                                __write_only image2d_t GW_WF,
                                __write_only image2d_t GW_US,
                                __write_only image2d_t GW_S,
                                __write_only image2d_t CH_H,
                                __read_only image2d_t Hn,
                                __read_only image2d_t SCANOPYN,
                                __read_only image2d_t SSURFACEN,
                                __read_only image2d_t GW_WFN,
                                __read_only image2d_t GW_USN,
                                __read_only image2d_t GW_SN,
                                __read_only image2d_t CH_HN
                                )
{

        //copy calculated output values back to original maps


        float tx = dt/dx;

        const float n = 0.1;

        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;


        const int id_1d = (int)(get_global_id(0));

        const int idx = trunc((float)(id_1d)/(float)(dim1));
        const int idy = id_1d % dim1;

        const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
        const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

        int x = trunc(read_imagef(LOCX,sampler, (int2)(gx_x,gy_y)).x);
        int y = trunc(read_imagef(LOCY,sampler, (int2)(gx_x,gy_y)).x);

        const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
        const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

        float hn = read_imagef(Hn,sampler, (int2)(gx,gy)).x;
        float scan = read_imagef(SCANOPYN,sampler, (int2)(gx,gy)).x;
        float ssurf = read_imagef(SSURFACEN,sampler, (int2)(gx,gy)).x;
        float wfh = read_imagef(GW_WFN,sampler, (int2)(gx,gy)).x;
        float ush = read_imagef(GW_USN,sampler, (int2)(gx,gy)).x;
        float sh = read_imagef(GW_SN,sampler, (int2)(gx,gy)).x;

        write_imagef(H, (int2)(gx,gy), hn);
        write_imagef(SCANOPY, (int2)(gx,gy), scan);
        write_imagef(SSURFACE, (int2)(gx,gy), ssurf);
        write_imagef(GW_WF, (int2)(gx,gy), wfh);
        write_imagef(GW_US, (int2)(gx,gy), ush);
        write_imagef(GW_S, (int2)(gx,gy), sh);


}

