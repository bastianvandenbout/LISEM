
#ifndef LISEM_KERNEL_CPUCODE
#define GRAV 9.81

__constant int ch_dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
__constant int ch_dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

#define nonkernel

#define int2(x,y) ((int2)((x),(y)))
#define float3(x,y,z) ((float3)((x),(y),(z)))
#else

#define nonkernel inline static
#endif

#ifndef LISEM_KERNEL_CPUCODE

nonkernel float minmod(float a, float b)
{
        float rec = 0;
        if (a >= 0 && b >= 0)
        {
        rec = min(a, b);
        }else if(a <= 0 && b <= 0)
        {
        rec = max(a, b);
        }
        return rec;
}

#endif


kernel
void evapotranspiration(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif

int dim0,
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
                                __write_only image2d_t EVAPO_CUM,
                                __write_only image2d_t CH_HN,
                                __write_only image2d_t OUTPUT_UI,
                                __read_only image2d_t TEMP,
                                __read_only image2d_t WIND,
                                __read_only image2d_t VAPR,
                                __read_only image2d_t RAD,
                                __read_only image2d_t NDVI,
                                __read_only image2d_t CROPF,
                                int ui_image,
                                float rain
                                )
{
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

        //sampler and coordinates

        float tx = dt/dx;

        const float n = 0.1;

        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

        const int id_1d = (int)(get_global_id(0));

        const int idx = trunc((float)(id_1d)/(float)(dim1));
        const int idy = id_1d % dim1;

        const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
        const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

        int x = trunc(read_imagef(LOCX,sampler, int2(gx_x,gy_y)).x);
        int y = trunc(read_imagef(LOCY,sampler, int2(gx_x,gy_y)).x);

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


        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float temp = read_imagef(TEMP,sampler, int2(gx,gy)).x;
        float wind = read_imagef(WIND,sampler, int2(gx,gy)).x;
        float vapr = read_imagef(VAPR,sampler, int2(gx,gy)).x;
        float ndvi = read_imagef(NDVI,sampler, int2(gx,gy)).x;
        float rad = read_imagef(RAD,sampler, int2(gx,gy)).x;
        float cropf = read_imagef(CROPF,sampler, int2(gx,gy)).x;
        float h = read_imagef(H,sampler, int2(gx,gy)).x;
        float store_canopy = read_imagef(SCANOPY,sampler, int2(gx,gy)).x;
        float store_surface = read_imagef(SSURFACE,sampler, int2(gx,gy)).x;
        float WFH = read_imagef(GW_WF,sampler, int2(gx,gy)).x;
        float actualinfiltration = read_imagef(F_InfilAct,sampler, int2(gx,gy)).x;
        float GWUH = read_imagef(GW_US,sampler, int2(gx,gy)).x;
        float GWH = read_imagef(GW_S,sampler, int2(gx,gy)).x;
        float ThetaS = min(0.9f,(float)(max(0.1f,(float)(read_imagef(THETAS,sampler, int2(gx,gy)).x))));
        float ThetaR = read_imagef(THETAR,sampler, int2(gx,gy)).x;
        float SD = read_imagef(SoilDepth,sampler, int2(gx,gy)).x;
        float KSattop = read_imagef(KSAT_T,sampler, int2(gx,gy)).x;
        float KSat = read_imagef(KSAT_B,sampler, int2(gx,gy)).x;
        float A = read_imagef(SWR_A,sampler, int2(gx,gy)).x;
        float B = read_imagef(SWR_B,sampler, int2(gx,gy)).x;
        float Smaxc = read_imagef(SMAX_CANOPY,sampler, int2(gx,gy)).x;
        float Smaxs = read_imagef(SMAX_SURFACE,sampler, int2(gx,gy)).x;
        float chm = read_imagef(CH_MASK,sampler, int2(gx,gy)).x;
        float chh = read_imagef(CH_H,sampler, int2(gx,gy)).x;
        float chwidth = read_imagef(CH_WIDTH,sampler, int2(gx,gy)).x;
        float chheight = read_imagef(CH_HEIGHT,sampler, int2(gx,gy)).x;
        //float chv = read_imagef(CH_V,sampler, int2(gx,gy)).x;


        float yterm = 0.000665f * 101.3f*pow((293.0f-0.0065f*z)/293.0f,5.26f);
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
        float et_act_left2 =  max(0.0f,et_act-GWH);

        //write output values to maps


        write_imagef(Hn, int2(gx,gy), h);
        write_imagef(SCANOPYN, int2(gx,gy), 0);
        write_imagef(SSURFACEN, int2(gx,gy), 0);
        write_imagef(GW_WFN, int2(gx,gy), WFH);
        write_imagef(GW_USN, int2(gx,gy), max(0.0f,GWUH -  et_act));
        write_imagef(GW_SN, int2(gx,gy), max(0.0f,GWH - et_act_left));
        write_imagef(EVAPO_CUM, int2(gx,gy), et_act - et_act_left2);


#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}

kernel
void evapotranspiration2(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif

int dim0,
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
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

        //copy calculated output values back to original maps


        float tx = dt/dx;

        const float n = 0.1;

        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;


        const int id_1d = (int)(get_global_id(0));

        const int idx = trunc((float)(id_1d)/(float)(dim1));
        const int idy = id_1d % dim1;

        const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
        const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

        int x = trunc(read_imagef(LOCX,sampler, int2(gx_x,gy_y)).x);
        int y = trunc(read_imagef(LOCY,sampler, int2(gx_x,gy_y)).x);

        const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
        const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

        float hn = read_imagef(Hn,sampler, int2(gx,gy)).x;
        float scan = read_imagef(SCANOPYN,sampler, int2(gx,gy)).x;
        float ssurf = read_imagef(SSURFACEN,sampler, int2(gx,gy)).x;
        float wfh = read_imagef(GW_WFN,sampler, int2(gx,gy)).x;
        float ush = read_imagef(GW_USN,sampler, int2(gx,gy)).x;
        float sh = read_imagef(GW_SN,sampler, int2(gx,gy)).x;

        write_imagef(H, int2(gx,gy), hn);
        write_imagef(SCANOPY, int2(gx,gy), scan);
        write_imagef(SSURFACE, int2(gx,gy), ssurf);
        write_imagef(GW_WF, int2(gx,gy), wfh);
        write_imagef(GW_US, int2(gx,gy), ush);
        write_imagef(GW_S, int2(gx,gy), sh);


#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}
