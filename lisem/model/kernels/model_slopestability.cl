
#define MAXCONC 848.0f

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


nonkernel float fmaxc(float a, float b)
{
    return a >b? a:b;
}


kernel
void SlopeStability(
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
				__read_only image2d_t Cohesion,
				__read_only image2d_t IFA,
				__read_only image2d_t Density,
				__read_only image2d_t GW_WF,
				__read_only image2d_t GW_US,
				__read_only image2d_t GW_S,
				__read_only image2d_t THETAS,
				__read_only image2d_t THETAR,
				__read_only image2d_t SoilDepth,
				__write_only image2d_t SafetyFactor,
				__write_only image2d_t SafetyFactorC,
				int first,
				__write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                float initialstabilitymargin
				)
{
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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
	
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;
		
        float sx_zh_x2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_x2-z)/dx))));
        float sy_zh_y1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_y1)/dx))));
        float sx_zh_x1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_x1)/dx))));
        float sy_zh_y2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_y2-z)/dx))));
	
	float sx_zh = max((float)((sx_zh_x1)),(float)((sx_zh_x2)));
	float sy_zh = max((float)((sy_zh_y1)),(float)((sy_zh_y2)));

        float Slope = fmaxc((float)(0.0001f),sqrt((float)((sx_zh*sx_zh)))+sqrt((float)((sy_zh*sy_zh))));
	
        float coh = read_imagef(Cohesion,sampler, int2(gx,gy)).x;
        float ifa = read_imagef(IFA,sampler, int2(gx,gy)).x;
        float d = read_imagef(Density,sampler, int2(gx,gy)).x;
        float wfh = read_imagef(GW_WF,sampler, int2(gx,gy)).x;
        float ush = read_imagef(GW_US,sampler, int2(gx,gy)).x;
        float sh = read_imagef(GW_S,sampler, int2(gx,gy)).x;
        float ts = read_imagef(THETAS,sampler, int2(gx,gy)).x;
        float sd = read_imagef(SoilDepth,sampler, int2(gx,gy)).x;
	float swh = wfh + ush + sh;
	
	float angle = atan(Slope);
        float cosa = cos(angle);
        float sina = sin(angle);
        float tanphi = tan(ifa);
	
        float forcingup = 0.0f;
        float forcing = 0.0f;
        float pga =  0.0f;
	
	float t1 = (forcingup + coh)
                    +
                    (
                        -cosa * sina*(pga/9.81f)*
                        (
                            d *(sd - swh) + 1000.0f *(swh)
                        )
                        +
                        cosa * cosa*
                        (

                               (
                                    d *(sd - swh) + 1000.0f *(swh)
                                )
                         )
                     )*tanphi;

        float t2 = max(0.0001f,(float)(forcing +
                (
                cosa*cosa*(pga/9.81f)*(d * sd + 1000.0f *(swh ))
                    +
                    ( d*sd + 1000.0f *(swh ))
                *sina*cosa
                )));
				
	float FoS = min((float)(t1/t2),1000.0f);
	
        float mul = 1.0f;

	if(first == 1)
	{
		float threshold = 1.0f;
                float margin = initialstabilitymargin;
			
		if(FoS < threshold * (1.0f + margin))
		{
		
                        float val = threshold *(1.0f + margin * (FoS/(threshold *(1.0f+margin)))*(FoS/(threshold *(1.0f+margin))))/max(0.01f,FoS);
                        val = max(0.0f,val);
                        write_imagef(SafetyFactorC, int2(gx,gy), val);
                        mul = val;
		}else
		{
                        write_imagef(SafetyFactorC, int2(gx,gy), 1.0f);

		}
	}
	
	float ui = 0;
	if(ui_image == 6)
	{
		ui = FoS;
		
                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
	}
				
	
        write_imagef(SafetyFactor, int2(gx,gy), FoS);


#ifdef LISEM_KERNEL_CPUCODE
}
#endif


}


kernel
void SlopeFailure(
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
				__read_only image2d_t Cohesion,
				__read_only image2d_t IFA,
				__read_only image2d_t Density,
				__read_only image2d_t GW_WF,
				__read_only image2d_t GW_US,
				__read_only image2d_t GW_S,
				__read_only image2d_t THETAS,
				__read_only image2d_t THETAR,
				__read_only image2d_t SoilDepth,
				__read_only image2d_t H,
				__read_only image2d_t HS,
				__read_only image2d_t SIfa,
				__read_only image2d_t SDensity,
				__read_only image2d_t SRockSize,
				__read_only image2d_t SafetyFactorC,
				__write_only image2d_t FailureDepth,
				__write_only image2d_t SoilDepthN,
				__write_only image2d_t HN,
				__write_only image2d_t HSN,
				__write_only image2d_t SIfaN,
				__write_only image2d_t SDensityN,
				__write_only image2d_t SRockSizeN,
				__write_only image2d_t GW_WFN,
				__write_only image2d_t GW_USN,
				__write_only image2d_t GW_SN,		
                                __write_only image2d_t DEMN,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                int do_initialstability
				)
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif


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
	
        float sfc = read_imagef(SafetyFactorC,sampler, int2(gx,gy)).x;
    
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;
		
        float sx_zh_x2 = (min((float)(10.0),max((float)(-10.0),(float)((z_x2-z)/dx))));
        float sy_zh_y1 = (min((float)(10.0),max((float)(-10.0),(float)((z-z_y1)/dx))));
        float sx_zh_x1 = (min((float)(10.0),max((float)(-10.0),(float)((z-z_x1)/dx))));
        float sy_zh_y2 = (min((float)(10.0),max((float)(-10.0),(float)((z_y2-z)/dx))));
	
	float sx_zh = max((float)((sx_zh_x1)),(float)((sx_zh_x2)));
	float sy_zh = max((float)((sy_zh_y1)),(float)((sy_zh_y2)));

        float Slope = fmaxc(0.0001,sqrt((float)((sx_zh*sx_zh)))+sqrt((float)((sy_zh*sy_zh))));
	
        float coh = read_imagef(Cohesion,sampler, int2(gx,gy)).x;
        float ifa = read_imagef(IFA,sampler, int2(gx,gy)).x;
        float sde = read_imagef(Density,sampler, int2(gx,gy)).x;
        float wfh = read_imagef(GW_WF,sampler, int2(gx,gy)).x;
        float ush = read_imagef(GW_US,sampler, int2(gx,gy)).x;
        float sh = read_imagef(GW_S,sampler, int2(gx,gy)).x;
        float ts = read_imagef(THETAS,sampler, int2(gx,gy)).x;
        float sd = read_imagef(SoilDepth,sampler, int2(gx,gy)).x;
	float swh = wfh + ush + sh;
	
	float angle = atan(Slope);
        float cosa = cos(angle);
        float sina = sin(angle);
        float tanphi = tan(ifa);
	
        float forcingup =  0.0f;
        float forcing =  0.0f;
        float pga =  0.0f;
	
	float t1 = (forcingup + coh)
                    +
                    (
                        -cosa * sina*(pga/9.81f)*
                        (
                            sde *(sd - swh) + 1000.0f *(swh)
                        )
                        +
                        cosa * cosa*
                        (

                               (
                                    sde *(sd - swh) + 1000.0f *(swh)
                                )
                         )
                     )*tanphi;

        float t2 = max(0.0001f,(float)(forcing +
                (
                cosa*cosa*(pga/9.81f)*(sde * sd + 1000.0f *(swh ))
                    +
                    ( sde*sd + 1000.0f *(swh ))
                *sina*cosa
                )));
				
	float FoS = min((float)(t1/t2),1000.0f);


        float fheight =  0.0f;
        float FoScr = (do_initialstability == 1? (1.0f/sfc):1.0f);
        if(FoS < FoScr && FoS > 0.0)
	{
		
		//get all relevant vvariables for calculation of stable depth
		float wf = swh/sd;
                float swf = swh/sd;
                float wd = 1000.0f;
                float hf = max(0.5f * sd,sd - Slope * dx);

                float sfmax = 1.2f / max(0.001f,sfc);


		float h0 = hf;
                float hdx = 0.5f * dx;

		float h1 = sd;
		float h2 = sd;
                float sc = 0.0f;
		{
                        //get all relevant variables for calculation of stable depth

                        //float cif = cos(ifa);
                        //float sif = sin(ifa);
                        //float d = dx;
                        //float dx2 = dx*dx;
                        //wf = swh/sd;
                        //wd = 1000.0;
                        //float pp = 0.01;
                        //float pc = 0.01;
                        //float ws = 0.01;
                        //float forcing = 0.01;
                        //float forcingup = 0.01;
                        //float pga = 0.01;
                        //sc = coh - forcing + forcingup -  (pga/9.81) *(wd * wf + (1.0 - wf) *sde)*cosa*sina *tan(ifa) -  (pga/9.81) *(wd * wf + (1.0 - wf) *sde)*cosa*cosa;

			//threshold safety factor value
			//include sf calibration (compensation factor to make sure that the initial state is stable)
                        //sf = 1.0;// / _SFCalibration->Drc;

                        //solution for stable depth at which safety factor equals a threshold value
                        //float t1 = (2.0 *hf *pc *cif + d* pp* sf*cif + 2.0 *hf *sc *cif -
                        //			 d *hf *sd *sf * cif + 2.0 *hf *ws *cif -
                        //			 dx2 *sd *sif + dx2 *wd *wf *sif);
                        //float t21 = (-2.0 *hf *pc *cif - d*pp*sf*cif - 2.0 *hf *sc *cif +
                        //		d *hf *sd *sf*cif - 2.0 *hf *ws *cif + dx2 *sd *sif -
                        //		dx2 *wd *wf *sif);
                        //float t22 = (pc*cif + sc*cif - d*sf*sd*cif +
                        //		ws*cif)*(dx2*pc*cif + hf*hf*pc*cif + d*hf*pp*sf*cif +
                        //		dx2*sc*cif + hf*hf*sc*cif + dx2*ws*cif +
                        //		hf*hf*ws*cif + dx2*pp*sif);

                        //in the end, it is nothin more than a implementation of the abc-formula with a lot of variables in there
			// x1,2 = (-b +- Sqrt(b^2-4ac))/2a
			//Two solutions for this formula, in our case: 1 positive, 1 negative, so we pick the positive value

                        //float t2 = sqrt(t21*t21 - 4.0*t22);
                        //float t3 = (2.0*(pc*cif + sc*cif - d*sd*sf*cif + ws*cif));

                        //positive and negative solution
                        //h1 = (t1 + t2)/t3;
                        //h2 = (t1 - t2)/t3;

                        //h1 = isnan(h1)? (isnan(h2)?0:h2):h1;
                        //h2 = isnan(h2)? (isnan(h1)?0:h1):h2;
//Mathematica code:
//Solve[sfmax == (coh) +
//Power[Cos[ArcTan[(h - h0)/dx]], 2]*((sde*(h - swh) + wd*(swf*h)))*
// tanphi/((sde*h + wd*(swf*h))*Sin[ArcTan[(h - h0)/dx]]*
//    Cos[ArcTan[(h - h0)/dx]]), h]

                        h1 = ((-coh)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                              coh*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd -

                              sqrt(-4.0f*dx*sde*swh*
                                 tanphi*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd) +
                                        pow(coh*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                   coh*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                 (2.0f*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd));
                        h2 = ((-coh)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                              coh*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd +

                              sqrt(-4.0f*dx*sde*swh*
                                 tanphi*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd) +
                                        pow(coh*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                   coh*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                 (2.0f*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd));

                        h1 = isnan(h1)? sd:h1;
                        h2 = isnan(h2)? sd:h2;

		}

		float hnew_select1 = max(h1,h2);
		float hnew_select2 = min(h1,h2);

                float hnew_select = hnew_select1;

                float hnew = min(sd,max(sd * 0.5f,hnew_select));


                fheight = min(sd,max(0.0f,sd - hnew));
	}

        float ui = 0;
        if(ui_image == 6)
        {
                ui = FoS / FoScr;

                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
        }
	if(ui_image == 7)
	{
                ui = fheight;
                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
	}
	
        write_imagef(FailureDepth, int2(gx,gy), fheight);
	
	
	//calculate new flow heights, flow properties, and soil water contents
	
	
        if(fheight > 0.05f && sd > 0.05f)
	{	
		float frac_fail = min(1.0f,max(0.0f,fheight/sd));
		float frac_left = 1.0f- frac_fail;
		
                float fluidh = read_imagef(H,sampler, int2(gx,gy)).x;;
                float solidh = read_imagef(HS,sampler, int2(gx,gy)).x;
                float sdensity = read_imagef(SDensity,sampler, int2(gx,gy)).x;
                float srocksize = read_imagef(SRockSize,sampler, int2(gx,gy)).x;
                float sifa = read_imagef(SIfa,sampler, int2(gx,gy)).x;
	
		float soildepth_n = sd * frac_left;
		float gw_wfh_n = wfh * frac_left;
		float gw_ush_n = ush * frac_left;
		float gw_sh_n = sh * frac_left;
		
                float fluid_add = max(0.0f,min((1.0f-ts)*sd,wfh * frac_fail+ ush * frac_fail + sh * frac_fail));
                float solid_add = min(1.0f,max(0.0f,1.0f-ts)) *sd * frac_fail;
		
		float solidh_n = solidh + solid_add;
		float fluidh_n = fluidh + fluid_add;
		float sdensity_n = (solidh * sdensity + solid_add*sde)/max(0.01f,solidh_n);
		float srocksize_n = 0.1;
		float sifa_n = (solidh * sifa + solid_add*ifa)/max(0.01f,solidh_n);;
		
                write_imagef(SoilDepthN, int2(gx,gy), soildepth_n);
                write_imagef(HN, int2(gx,gy), fluidh_n);
                write_imagef(HSN, int2(gx,gy), solidh_n);
                write_imagef(SIfaN, int2(gx,gy), sifa_n);
                write_imagef(SDensityN, int2(gx,gy), sdensity_n);
                write_imagef(SRockSizeN, int2(gx,gy), srocksize_n);
                write_imagef(GW_WFN, int2(gx,gy), gw_wfh_n);
                write_imagef(GW_USN, int2(gx,gy), gw_ush_n);
                write_imagef(GW_SN, int2(gx,gy), gw_sh_n);
                write_imagef(DEMN, int2(gx,gy), z - fheight);
	}
	

#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}



kernel
void SlopeStabilityExtended(
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
                                __read_only image2d_t Cohesion,
                                __read_only image2d_t IFA,
                                __read_only image2d_t Density,
                                __read_only image2d_t GW_WF,
                                __read_only image2d_t GW_US,
                                __read_only image2d_t GW_S,
                                __read_only image2d_t THETAS,
                                __read_only image2d_t THETAR,
                                __read_only image2d_t SoilDepth,
                                __read_only image2d_t PGA,
                                __read_only image2d_t ForcingX,
                                __read_only image2d_t ForcingY,
                                __write_only image2d_t SafetyFactor,
                                __write_only image2d_t SafetyFactorC,
                                int first,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                float initialstabilitymargin
                                )
{


#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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

        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

        float sx_zh_x2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_x2-z)/dx))));
        float sy_zh_y1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_y1)/dx))));
        float sx_zh_x1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_x1)/dx))));
        float sy_zh_y2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_y2-z)/dx))));

        float sx_zh = max((float)((sx_zh_x1)),(float)((sx_zh_x2)));
        float sy_zh = max((float)((sy_zh_y1)),(float)((sy_zh_y2)));

        float Slope = fmaxc((float)(0.0001f),sqrt((float)((sx_zh*sx_zh)))+sqrt((float)((sy_zh*sy_zh))));

        float coh = read_imagef(Cohesion,sampler, int2(gx,gy)).x;
        float ifa = read_imagef(IFA,sampler, int2(gx,gy)).x;
        float d = read_imagef(Density,sampler, int2(gx,gy)).x;
        float wfh = read_imagef(GW_WF,sampler, int2(gx,gy)).x;
        float ush = read_imagef(GW_US,sampler, int2(gx,gy)).x;
        float sh = read_imagef(GW_S,sampler, int2(gx,gy)).x;
        float ts = read_imagef(THETAS,sampler, int2(gx,gy)).x;
        float sd = read_imagef(SoilDepth,sampler, int2(gx,gy)).x;
        float swh = wfh + ush + sh;

        float angle = atan(Slope);
        float cosa = cos(angle);
        float sina = sin(angle);
        float tanphi = tan(ifa);

        float forcingup = 0;
        float forcing = 0;
        float pga = read_imagef(PGA,sampler, int2(gx,gy)).x;;

        float t1 = (forcingup + coh)
                    +
                    (
                        -0.0f * cosa * sina*(pga/9.81f)*
                        (
                            d *(sd - swh) + 1000.0f *(swh)
                        )
                        +
                        cosa * cosa*
                        (

                               (
                                    d *(sd - swh) + 1000.0f *(swh)
                                )
                         )
                     )*tanphi;

        float t2 = max(0.0001f,(float)(forcing +
                (
                sina*cosa*(pga/9.81f)*(d * sd + 1000.0f *(swh ))
                    +
                    ( d*sd + 1000.0f *(swh ))
                *sina*cosa
                )));

        float FoS = min((float)(t1/t2),1000.0f);

        float mul = 1.0f;

        if(first == 1)
        {
                float threshold = 1.0f;
                float margin = initialstabilitymargin;

                if(FoS < threshold * (1.0f + margin))
                {

                        float val = threshold *(1.0f + margin * (FoS/(threshold *(1.0f+margin)))*(FoS/(threshold *(1.0f+margin))))/max(0.01f,FoS);
                        val = max(0.0f,val);
                        write_imagef(SafetyFactorC, int2(gx,gy), val);
                        mul = val;
                }else
                {
                        write_imagef(SafetyFactorC, int2(gx,gy), 1.0f);

                }
        }

        float ui = 0;
        if(ui_image == 6)
        {
                ui = FoS;

                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
        }


        write_imagef(SafetyFactor, int2(gx,gy), FoS);


#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}


kernel
void SlopeFailureExtended(
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
                                __read_only image2d_t Cohesion,
                                __read_only image2d_t IFA,
                                __read_only image2d_t Density,
                                __read_only image2d_t GW_WF,
                                __read_only image2d_t GW_US,
                                __read_only image2d_t GW_S,
                                __read_only image2d_t THETAS,
                                __read_only image2d_t THETAR,
                                __read_only image2d_t SoilDepth,
                                __read_only image2d_t H,
                                __read_only image2d_t HS,
                                __read_only image2d_t SIfa,
                                __read_only image2d_t SDensity,
                                __read_only image2d_t SRockSize,
                                __read_only image2d_t SafetyFactorC,
                                __read_only image2d_t PGA,
                                __read_only image2d_t ForcingX,
                                __read_only image2d_t ForcingY,
                                __write_only image2d_t FailureDepth,
                                __write_only image2d_t SoilDepthN,
                                __write_only image2d_t HN,
                                __write_only image2d_t HSN,
                                __write_only image2d_t SIfaN,
                                __write_only image2d_t SDensityN,
                                __write_only image2d_t SRockSizeN,
                                __write_only image2d_t GW_WFN,
                                __write_only image2d_t GW_USN,
                                __write_only image2d_t GW_SN,
                                __write_only image2d_t DEMN,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                int do_initialstability
                                )
{


#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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

        float sfc = read_imagef(SafetyFactorC,sampler, int2(gx,gy)).x;

        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

        float sx_zh_x2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_x2-z)/dx))));
        float sy_zh_y1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_y1)/dx))));
        float sx_zh_x1 = (min((float)(10.0f),max((float)(-10.0f),(float)((z-z_x1)/dx))));
        float sy_zh_y2 = (min((float)(10.0f),max((float)(-10.0f),(float)((z_y2-z)/dx))));

        float sx_zh = max((float)((sx_zh_x1)),(float)((sx_zh_x2)));
        float sy_zh = max((float)((sy_zh_y1)),(float)((sy_zh_y2)));

        float Slope = fmaxc(0.0001f,sqrt((float)((sx_zh*sx_zh)))+sqrt((float)((sy_zh*sy_zh))));

        float coh = read_imagef(Cohesion,sampler, int2(gx,gy)).x;
        float ifa = read_imagef(IFA,sampler, int2(gx,gy)).x;
        float sde = read_imagef(Density,sampler, int2(gx,gy)).x;
        float wfh = read_imagef(GW_WF,sampler, int2(gx,gy)).x;
        float ush = read_imagef(GW_US,sampler, int2(gx,gy)).x;
        float sh = read_imagef(GW_S,sampler, int2(gx,gy)).x;
        float ts = read_imagef(THETAS,sampler, int2(gx,gy)).x;
        float sd = read_imagef(SoilDepth,sampler, int2(gx,gy)).x;
        float swh = wfh + ush + sh;

        float angle = atan(Slope);
        float cosa = cos(angle);
        float sina = sin(angle);
        float tanphi = tan(ifa);

        float forcingup = 0.0f;
        float forcing = 0.0f;
        float pga = read_imagef(PGA,sampler, int2(gx,gy)).x;;

        float t1 = (forcingup + coh)
                    +
                    (
                        -0.0f*cosa * sina*(pga/9.81f)*
                        (
                            sde *(sd - swh) + 1000.0f *(swh)
                        )
                        +
                        cosa * cosa*
                        (

                               (
                                    sde *(sd - swh) + 1000.0f *(swh)
                                )
                         )
                     )*tanphi;

        float t2 = max(0.0001f,(float)(forcing +
                (
                cosa*sina*(pga/9.81f)*(sde * sd + 1000.0f *(swh ))
                    +
                    ( sde*sd + 1000.0f *(swh ))
                *sina*cosa
                )));

        float FoS = min((float)(t1/t2),1000.0f);


        float fheight = 0;
        float FoScr = (do_initialstability == 1? (1.0f/sfc):1.0f);
        if(FoS < FoScr && FoS > 0.0f)
        {
                coh = coh -cosa * sina*(pga/9.81f)*(sde *(sd - swh) + 1000.0f *(swh)) * tanphi;

                //get all relevant vvariables for calculation of stable depth
                float wf = swh/sd;
                float swf = swh/sd;
                float wd = 1000.0f;
                float hf = max(0.5f * sd,sd - Slope * dx);

                float sfmax = 1.2f / max(0.001f,sfc);


                float h0 = hf;
                float hdx = 0.5f * dx;

                float h1 = sd;
                float h2 = sd;
                float sc = 0.0f;
                {
                        h1 = ((-coh)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                              coh*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd -

                              sqrt(-4.0f*dx*sde*swh*
                                 tanphi*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd) +
                                        pow(coh*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                   coh*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                 (2.0f*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd));
                        h2 = ((-coh)*h0*sde + h0*sde*sfmax + dx*sde*tanphi -
                              coh*h0*swf*wd + h0*sfmax*swf*wd + dx*swf*tanphi*wd +

                              sqrt(-4.0f*dx*sde*swh*
                                 tanphi*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd) +
                                        pow(coh*h0*sde - h0*sde*sfmax - dx*sde*tanphi +
                                   coh*h0*swf*wd - h0*sfmax*swf*wd - dx*swf*tanphi*wd,2.0f)))/
                                 (2.0f*((-coh)*sde + sde*sfmax - coh*swf*wd + sfmax*swf*wd));

                        h1 = isnan(h1)? sd:h1;
                        h2 = isnan(h2)? sd:h2;

                }

                float hnew_select1 = max(h1,h2);
                float hnew_select2 = min(h1,h2);

                float hnew_select = hnew_select1;

                float hnew = min(sd,max(sd * 0.5f,hnew_select));


                fheight = min(sd,max(0.0f,sd - hnew));
        }

        float ui = 0;
        if(ui_image == 6)
        {
                ui = FoS / FoScr;

                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
        }
        if(ui_image == 7)
        {
                ui = fheight;
                write_imagef(OUTPUT_UI, int2(gx,gy), ui);
        }

        write_imagef(FailureDepth, int2(gx,gy), fheight);


        //calculate new flow heights, flow properties, and soil water contents


        if(fheight > 0.05f && sd > 0.05f)
        {
                float frac_fail = min(1.0f,max(0.0f,fheight/sd));
                float frac_left = 1.0f- frac_fail;

                float fluidh = read_imagef(H,sampler, int2(gx,gy)).x;;
                float solidh = read_imagef(HS,sampler, int2(gx,gy)).x;
                float sdensity = read_imagef(SDensity,sampler, int2(gx,gy)).x;
                float srocksize = read_imagef(SRockSize,sampler, int2(gx,gy)).x;
                float sifa = read_imagef(SIfa,sampler, int2(gx,gy)).x;

                float soildepth_n = sd * frac_left;
                float gw_wfh_n = wfh * frac_left;
                float gw_ush_n = ush * frac_left;
                float gw_sh_n = sh * frac_left;

                float fluid_add = max(0.0f,min((1.0f-ts)*sd,wfh * frac_fail+ ush * frac_fail + sh * frac_fail));
                float solid_add = min(1.0f,max(0.0f,1.0f-ts)) * sd *  frac_fail;

                float solidh_n = solidh + solid_add;
                float fluidh_n = fluidh + fluid_add;
                float sdensity_n = (solidh * sdensity + solid_add*sde)/max(0.01f,solidh_n);
                float srocksize_n = 0.1f;
                float sifa_n = (solidh * sifa + solid_add*ifa)/max(0.01f,solidh_n);;

                write_imagef(SoilDepthN, int2(gx,gy), soildepth_n);
                write_imagef(HN, int2(gx,gy), fluidh_n);
                write_imagef(HSN, int2(gx,gy), solidh_n);
                write_imagef(SIfaN, int2(gx,gy), sifa_n);
                write_imagef(SDensityN, int2(gx,gy), sdensity_n);
                write_imagef(SRockSizeN, int2(gx,gy), srocksize_n);
                write_imagef(GW_WFN, int2(gx,gy), gw_wfh_n);
                write_imagef(GW_USN, int2(gx,gy), gw_ush_n);
                write_imagef(GW_SN, int2(gx,gy), gw_sh_n);
                write_imagef(DEMN, int2(gx,gy), z - fheight);
        }


#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}

kernel
void SlopeFailure2(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif
 int dim0,
				int dim1, 
				float dx,
				float dt,
				__read_only image2d_t LOCX,
				__read_only image2d_t LOCY,
				__read_only image2d_t FailureDepth,
				__write_only image2d_t GW_WF,
				__write_only image2d_t GW_US,
				__write_only image2d_t GW_S,
				__write_only image2d_t SoilDepth,
				__write_only image2d_t H,
				__write_only image2d_t HS,
				__write_only image2d_t SIfa,
				__write_only image2d_t SDensity,
				__write_only image2d_t SRockSize,
				__read_only image2d_t SoilDepthN,
				__read_only image2d_t HN,
				__read_only image2d_t HSN,
				__read_only image2d_t SIfaN,
				__read_only image2d_t SDensityN,
				__read_only image2d_t SRockSizeN,
				__read_only image2d_t GW_WFN,
				__read_only image2d_t GW_USN,
                                __read_only image2d_t GW_SN,
                                __read_only image2d_t FailureDepthCum,
                                __write_only image2d_t FailureDepthCumN,
                                int is_first,
                                __read_only image2d_t DEMN,
                                __write_only image2d_t DEM
				)
{


#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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
	
        float fd = read_imagef(FailureDepth,sampler, int2(gx,gy)).x;
        float fdcum = read_imagef(FailureDepthCum,sampler, int2(gx,gy)).x;

        if(is_first == 1)
        {
            fdcum = 0.0f;
        }

	if(fd > 0)
	{
                float sd = read_imagef(SoilDepthN,sampler, int2(gx,gy)).x;
                float h = read_imagef(HN,sampler, int2(gx,gy)).x;
                float hs = read_imagef(HSN,sampler, int2(gx,gy)).x;
                float sifa = read_imagef(SIfaN,sampler, int2(gx,gy)).x;
                float sde = read_imagef(SDensityN,sampler, int2(gx,gy)).x;
                float srs = read_imagef(SRockSizeN,sampler, int2(gx,gy)).x;
                float wfh = read_imagef(GW_WFN,sampler, int2(gx,gy)).x;
                float ush = read_imagef(GW_USN,sampler, int2(gx,gy)).x;
                float sh = read_imagef(GW_SN,sampler, int2(gx,gy)).x;
		
                write_imagef(SoilDepth, int2(gx,gy), sd);
                write_imagef(H, int2(gx,gy), h);
                write_imagef(HS, int2(gx,gy), hs);
                write_imagef(SIfa, int2(gx,gy), sifa);
                write_imagef(SDensity, int2(gx,gy),sde);
                write_imagef(SRockSize, int2(gx,gy), srs);
                write_imagef(GW_WF, int2(gx,gy), wfh);
                write_imagef(GW_US, int2(gx,gy), ush);
                write_imagef(GW_S, int2(gx,gy), sh);

                float dem = read_imagef(DEMN,sampler, int2(gx,gy)).x;
                write_imagef(DEM, int2(gx,gy), dem);

                write_imagef(FailureDepthCumN, int2(gx,gy), fdcum + fd);
        }else
        {
            write_imagef(FailureDepthCumN, int2(gx,gy), fdcum);
        }

#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}

kernel
void SlopeFailureP2(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif
 int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __read_only image2d_t FailureDepth,
                                __write_only image2d_t GW_WF,
                                __write_only image2d_t GW_US,
                                __write_only image2d_t GW_S,
                                __write_only image2d_t SoilDepth,
                                __write_only image2d_t H,
                                __write_only image2d_t HS,
                                __write_only image2d_t SIfa,
                                __write_only image2d_t SDensity,
                                __write_only image2d_t SRockSize,
                                __read_only image2d_t SoilDepthN,
                                __read_only image2d_t HN,
                                __read_only image2d_t HSN,
                                __read_only image2d_t SIfaN,
                                __read_only image2d_t SDensityN,
                                __read_only image2d_t SRockSizeN,
                                __read_only image2d_t GW_WFN,
                                __read_only image2d_t GW_USN,
                                __read_only image2d_t GW_SN,
                                __read_only image2d_t FailureDepthCum,
                                __write_only image2d_t FailureDepthCumN,
                                int is_first,
                                __read_only image2d_t DEMN,
                                __write_only image2d_t DEM
                                )
{


#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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

        float fd = read_imagef(FailureDepth,sampler, int2(gx,gy)).x;
        float fdcum = read_imagef(FailureDepthCum,sampler, int2(gx,gy)).x;

        if(is_first == 1)
        {
            fdcum = 0.0f;
        }

        if(fd > 0)
        {

                float sd = read_imagef(SoilDepthN,sampler, int2(gx,gy)).x;
                float h = read_imagef(HN,sampler, int2(gx,gy)).x;
                float hs = read_imagef(HSN,sampler, int2(gx,gy)).x;
                float sifa = read_imagef(SIfaN,sampler, int2(gx,gy)).x;
                float sde = read_imagef(SDensityN,sampler, int2(gx,gy)).x;
                float srs = read_imagef(SRockSizeN,sampler, int2(gx,gy)).x;
                float wfh = read_imagef(GW_WFN,sampler, int2(gx,gy)).x;
                float ush = read_imagef(GW_USN,sampler, int2(gx,gy)).x;
                float sh = read_imagef(GW_SN,sampler, int2(gx,gy)).x;

                write_imagef(SoilDepth, int2(gx,gy), sd);
                write_imagef(H, int2(gx,gy), h);
                //write_imagef(HS, int2(gx,gy), hs);
                write_imagef(SIfa, int2(gx,gy), sifa);
                write_imagef(SDensity, int2(gx,gy),sde);
                write_imagef(SRockSize, int2(gx,gy), srs);
                write_imagef(GW_WF, int2(gx,gy), wfh);
                write_imagef(GW_US, int2(gx,gy), ush);
                write_imagef(GW_S, int2(gx,gy), sh);

                float dem = read_imagef(DEMN,sampler, int2(gx,gy)).x;
                write_imagef(DEM, int2(gx,gy), dem);

                write_imagef(FailureDepthCumN, int2(gx,gy), fdcum + fd);
        }else
        {
            write_imagef(FailureDepthCumN, int2(gx,gy), fdcum);
        }


#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}


kernel
void SubSurfaceForce1( int dim0, 
				int dim1, 
				float dx,
				float dt,
				__read_only image2d_t ForceCapacity,
				__read_only image2d_t ForceDemandX,
				__read_only image2d_t ForceDemandY,
				__write_only image2d_t ForceDemandXN,
				__write_only image2d_t ForceDemandYN)
{




}

kernel
void SubSurfaceForce2( int dim0, 
				int dim1, 
				float dx,
				float dt,
				__read_only image2d_t ForceCapacity,
				__write_only image2d_t ForceDemandX,
				__write_only image2d_t ForceDemandY,
				__read_only image2d_t ForceDemandXN,
				__read_only image2d_t ForceDemandYN)
{




}
