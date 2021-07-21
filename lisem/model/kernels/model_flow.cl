
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

//int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
//int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};
	
#ifndef LISEM_KERNEL_CPUCODE




nonkernel float minmod(float a, float b)
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
#endif


nonkernel float maxmod(float x, float y)
{
    if(y < 0.0f)
    {
        return min(-x,y);
    }else
    {
        return max(x,y);
    }
}

nonkernel bool flow_to(int x,int y, int x2, int y2, int ldd)
{

	if(x + ch_dx[ldd] == x2 && y + ch_dy[ldd] == y2)
	{
		return true;
	}else
	{
		return false;
	}

}

nonkernel float fcabs(float f)
{
    if(f > 0.0)
    {
        return f;
    }else
    {
        return -f;
    }
}

nonkernel float3 F_HLL2FB(float h_L,float u_L,float v_L, float fb_L, float h_R,float u_R,float v_R, float fb_R)
{
    float3 ret;

    float grav_h_L = GRAV*h_L;
    float grav_h_R = GRAV*h_R;
    float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
    float sqrt_grav_h_R = sqrt((float)(grav_h_R));
    float q_R = fb_R * u_R*h_R;
    float q_L = fb_L * u_L*h_L;

    float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
    float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
    float tmp = 1.0/max((float)0.1,(float)(sc2-sc1));
    float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.)))*tmp;
    float t2 = 1.0 - t1;
    float t3_1 = fabs((float)(sc1));
    float t3_2 = fabs((float)(sc2));
    float t3 =(sc2* t3_1 - sc1* t3_2)*0.5*tmp;

    ret.x = t1*q_R + t2*q_L - t3*0.5*(fb_R + fb_L)*(h_R - h_L);
    ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5) + t2*(q_L*u_L + grav_h_L*h_L*0.5) - t3*0.5*(fb_R + fb_L)*(q_R - q_L);
    ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*0.5*(fb_R + fb_L)*(h_R*v_R - h_L*v_L);

    return ret;
}

nonkernel float3 F_HLL2(float h_L,float u_L,float v_L,float h_R,float u_R,float v_R)
{
		float3 ret;
		
        float grav_h_L = GRAV*h_L;
        float grav_h_R = GRAV*h_R;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R)); 
        float tmp = 1.0/max((float)0.1,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.)))*tmp;
        float t2 = 1.0 - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5*tmp;
		
        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5) + t2*(q_L*u_L + grav_h_L*h_L*0.5) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);
		
		return ret;
}


kernel
void flow(
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
				__read_only image2d_t Vx,
				__read_only image2d_t Vy,
				__read_only image2d_t InfilPot,
				__read_only image2d_t InfilCum,
				__read_only image2d_t CH_MASK,
				__read_only image2d_t CH_WIDTH,
				__read_only image2d_t CH_HEIGHT,
				__read_only image2d_t CH_H,
				__read_only image2d_t CH_V,
				__write_only image2d_t Hn,
				__write_only image2d_t Vxn,
				__write_only image2d_t Vyn,
				__write_only image2d_t InfilAct,
				__write_only image2d_t CH_HN,
				__write_only image2d_t CH_VN,
                __write_only image2d_t OUTPUT_UI,
                int ui_image,
                float dt_hydro,
                __read_only image2d_t QFOUT,
                __write_only image2d_t QFOUTN,
                __write_only image2d_t DTREQ,
                __write_only image2d_t QFX1,
                __write_only image2d_t QFX2,
                __write_only image2d_t QFY1,
                __write_only image2d_t QFY2,
                __write_only image2d_t QFCHX1,
                __write_only image2d_t QFCHX2,
                __write_only image2d_t QFCHIN,
                __write_only image2d_t RAIN,
                __read_only image2d_t MANNING,
                __read_only image2d_t BlockingX,
                __read_only image2d_t BlockingY,
                __read_only image2d_t BlockingFX,
                __read_only image2d_t BlockingFY,
                __read_only image2d_t BlockingVelX,
                __read_only image2d_t BlockingVelY,
                __read_only image2d_t BlockingLDD,
                __read_only image2d_t HCorrect




				)
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

	float tx = dt/dx;

	
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
	
        //const int gx = min(dim0-(int)(1),max((int)(0),(int)(get_global_id(0))));
        //const int gy = min(dim1-(int)(1),max((int)(0),(int)(get_global_id(1))));
	
	const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
        const int gy_x1 = gy;
	const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
        const int gy_x2 = gy;
	const int gx_y1 = gx;
        const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
	const int gx_y2 = gx;
        const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

        const int gx_x11 = min(dim0-(int)(1),max((int)(0),(int)(gx - 2 )));
            const int gy_x11 = gy;
        const int gx_x22 = min(dim0-(int)(1),max((int)(0),(int)(gx + 2)));
            const int gy_x22 = gy;
        const int gx_y11 = gx;
            const int gy_y11 = min(dim1-(int)(1),max((int)(0),(int)(gy - 2)));
        const int gx_y22 = gx;
            const int gy_y22 = min(dim1-(int)(1),max((int)(0),(int)(gy + 2)));

	float infilpot = (dt/dt_hydro)*read_imagef(InfilPot,sampler, int2(gx,gy)).x;
   	float infilcum = read_imagef(InfilCum,sampler, int2(gx,gy)).x;
	
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float man_n = read_imagef(MANNING,sampler, int2(gx,gy)).x;
        float n = man_n + 0.001f;
        if(z > -1000)
        {

            float vmax = 0.25 * dx/dt;


            float rain = read_imagef(RAIN,sampler, int2(gx_x1,gy)).x;

            float qfout = read_imagef(QFOUT,sampler, int2(gx,gy)).x;

            float blockx = read_imagef(BlockingX,sampler, int2(gx,gy)).x;
            float blocky = read_imagef(BlockingY,sampler, int2(gx,gy)).x;

            float blockfx = read_imagef(BlockingFX,sampler, int2(gx,gy)).x;
            float blockfy = read_imagef(BlockingFY,sampler, int2(gx,gy)).x;

            float blockvelx = min(vmax,max(-vmax,read_imagef(BlockingVelX,sampler, int2(gx,gy)).x));
            float blockvely = min(vmax,max(-vmax,read_imagef(BlockingVelY,sampler, int2(gx,gy)).x));

            float blockx_x1 = read_imagef(BlockingX,sampler, int2(gx_x1,gy)).x;
            float blockx_x2 = read_imagef(BlockingX,sampler, int2(gx_x2,gy)).x;
            float blocky_y1 = read_imagef(BlockingY,sampler, int2(gx,gy_y1)).x;
            float blocky_y2 = read_imagef(BlockingY,sampler, int2(gx,gy_y2)).x;

            float blockvelx_x1 = min(vmax,max(-vmax,read_imagef(BlockingVelX,sampler, int2(gx_x1,gy)).x));
            float blockvelx_x2 = min(vmax,max(-vmax,read_imagef(BlockingVelX,sampler, int2(gx_x2,gy)).x));
            float blockvely_y1 = min(vmax,max(-vmax,read_imagef(BlockingVelY,sampler, int2(gx,gy_y1)).x));
            float blockvely_y2 = min(vmax,max(-vmax,read_imagef(BlockingVelY,sampler, int2(gx,gy_y2)).x));

            float blockvely_x1 = min(vmax,max(-vmax,read_imagef(BlockingVelY,sampler, int2(gx_x1,gy)).x));
            float blockvely_x2 = min(vmax,max(-vmax,read_imagef(BlockingVelY,sampler, int2(gx_x2,gy)).x));
            float blockvelx_y1 = min(vmax,max(-vmax,read_imagef(BlockingVelX,sampler, int2(gx,gy_y1)).x));
            float blockvelx_y2 = min(vmax,max(-vmax,read_imagef(BlockingVelX,sampler, int2(gx,gy_y2)).x));

            float capture_x = max(0.0f,min(1.0f,blockx));//max(0.0f,min(1.0f,blockx - (1.0f-blockx)));
            float capture_y = max(0.0f,min(1.0f,blocky));//max(0.0f,min(1.0f,blocky - (1.0f-blocky)));

            float capture_x1 = max(0.0f,min(1.0f,blockx_x1));//max(0.0f,min(1.0f,blockx_x1 - (1.0f-blockx_x1)));
            float capture_y1 = max(0.0f,min(1.0f,blocky_y1));//max(0.0f,min(1.0f,blocky_y1 - (1.0f-blocky_y1)));
            float capture_x2 = max(0.0f,min(1.0f,blockx_x2));//max(0.0f,min(1.0f,blockx_x2 - (1.0f-blockx_x2)));
            float capture_y2 = max(0.0f,min(1.0f,blocky_y2));//max(0.0f,min(1.0f,blocky_y2 - (1.0f-blocky_y2)));
            float zhc = read_imagef(HCorrect,sampler, int2(gx,gy)).x;

            z = z + zhc;

            float zhc_x1 = read_imagef(HCorrect,sampler, int2(gx_x1,gy)).x;
            float zhc_x2 = read_imagef(HCorrect,sampler, int2(gx_x2,gy)).x;
            float zhc_y1 = read_imagef(HCorrect,sampler, int2(gx,gy_y1)).x;
            float zhc_y2 = read_imagef(HCorrect,sampler, int2(gx,gy_y2)).x;

            float z_x1 = zhc_x1 + read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
            float z_x2 = zhc_x2 + read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
            float z_y1 = zhc_y1 + read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
            float z_y2 = zhc_y2 + read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

            if(gx + 1 > dim0-1)
            {
                z_x2 = -99999;
            }
            if(gx - 1 < 0)
            {
                z_x1 = -99999;
            }
            if(gy + 1 > dim1-1)
            {
                z_y2 = -99999;
            }
            if(gy - 1 < 0)
            {
                z_y1 = -99999;
            }

            float zc_x1 = z_x1 < -1000? z : z_x1;
            float zc_x2 = z_x2 < -1000? z : z_x2;
            float zc_y1 = z_y1 < -1000? z : z_y1;
            float zc_y2 = z_y2 < -1000? z : z_y2;


            float h = max(0.0f,read_imagef(H,sampler, int2(gx,gy)).x);
            float h_x1 = max(0.0f,read_imagef(H,sampler, int2(gx_x1,gy)).x);
            float h_x2 = max(0.0f,read_imagef(H,sampler, int2(gx_x2,gy)).x);
            float h_y1 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y1)).x);
            float h_y2 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y2)).x);

            float h_x11 = max(0.0f,read_imagef(H,sampler, int2(gx_x11,gy)).x);
            float h_x22 = max(0.0f,read_imagef(H,sampler, int2(gx_x22,gy)).x);
            float h_y11 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y11)).x);
            float h_y22 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y22)).x);


            float zcc_x1 =max(z,zc_x1);
            float zcc_x2 =max(z,zc_x2);
            float zcc_y1 =max(z,zc_y1);
            float zcc_y2 =max(z,zc_y2);

            float vx = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy)).x));
            float vy = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy)).x));

            float vx_x1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x1,gy)).x));
            float vy_x1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x1,gy)).x));
            float vx_x2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x2,gy)).x));
            float vy_x2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x2,gy)).x));
            float vx_y1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy_y1)).x));
            float vy_y1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy_y1)).x));
            float vx_y2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy_y2)).x));
            float vy_y2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy_y2)).x));

            float factor_flowx1b = h_x1 < h_x2 ? 1.0-blockx : 1.0;
            float factor_flowy1b = h_y1 < h_y2 ? 1.0-blocky : 1.0;
            float factor_flowx2b = h_x1 < h_x2 ? 1.0 :  1.0-blockx;
            float factor_flowy2b = h_y1 < h_y2 ? 1.0 : 1.0-blocky;

            float factor_flowx1bt = h_x11 < h ? 1.0 : 1.0-blockx_x1;
            float factor_flowy1bt = h_y11 < h ? 1.0 : 1.0-blocky_y1;
            float factor_flowx2bt = h < h_x22 ? 1.0-blockx_x2 : 1.0;
            float factor_flowy2bt = h < h_y22 ? 1.0-blocky_y2 : 1.0;

            float factor_flowx1f = 1.0-min(1.0f,max(0.0f,z-z_x1)/max(1e-6f,h_x1));
            float factor_flowy1f = 1.0-min(1.0f,max(0.0f,z-z_y1)/max(1e-6f,h_y1));
            float factor_flowx2f = 1.0-min(1.0f,max(0.0f,z-z_x2)/max(1e-6f,h_x2));
            float factor_flowy2f = 1.0-min(1.0f,max(0.0f,z-z_y2)/max(1e-6f,h_y2));

            float factor_flowx1t = 1.0-min(1.0f,max(0.0f,z_x1-z)/max(1e-6f,h));
            float factor_flowy1t = 1.0-min(1.0f,max(0.0f,z_y1-z)/max(1e-6f,h));
            float factor_flowx2t = 1.0-min(1.0f,max(0.0f,z_x2-z)/max(1e-6f,h));
            float factor_flowy2t = 1.0-min(1.0f,max(0.0f,z_y2-z)/max(1e-6f,h));

            float sx_zh_x2 = min((float)(0.5),max((float)(-0.5),(float)((zc_x2 + h_x2-z - h)/dx)));
            float sy_zh_y1 = min((float)(0.5),max((float)(-0.5),(float)((z + h-zc_y1 - h_y1)/dx)));
            float sx_zh_x1 = min((float)(0.5),max((float)(-0.5),(float)((z + h-zc_x1 - h_x1)/dx)));
            float sy_zh_y2 = min((float)(0.5),max((float)(-0.5),(float)((zc_y2 + h_y2-z - h)/dx)));

            float sx_zh = min(1.0f,max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
            float sy_zh = min(1.0f,max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));

            float sx_z_x2 = min((float)(0.5),max((float)(-0.5),(float)((zc_x2 -z)/dx)));
            float sy_z_y1 = min((float)(0.5),max((float)(-0.5),(float)((z -zc_y1 )/dx)));
            float sx_z_x1 = min((float)(0.5),max((float)(-0.5),(float)((z -zc_x1 )/dx)));
            float sy_z_y2 = min((float)(0.5),max((float)(-0.5),(float)((zc_y2 -z )/dx)));

            float sx_z = min(1.0f,max(-1.0f,minmod(sx_z_x1,sx_z_x2)));
            float sy_z = min(1.0f,max(-1.0f,minmod(sy_z_y1,sy_z_y2)));

            //float3 hll_x1 = z_x1 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowx1bt *h_x1 -max(0.0f,z - z_x1)),vx_x1,vy_x1,max(0.0f,factor_flowx1b *h -max(0.0f,z_x1-z)),vx,vy);
            //float3 hll_x2 = z_x2 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowx2b *h -max(0.0f,z_x2 - z)),vx,vy,max(0.0f,factor_flowx2bt *h_x2 -max(0.0f,z - z_x2)),vx_x2,vy_x2);
            //float3 hll_y1 = z_y1 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowy1bt *h_y1 -max(0.0f,z - z_y1)),vy_y1,vx_y1,max(0.0f,factor_flowy1b *h -max(0.0f,z_y1 - z)),vy,vx);
            //float3 hll_y2 = z_y2 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowy2b *h -max(0.0f,z_y2 - z)),vy,vx,max(0.0f,factor_flowy2bt *h_y2 -max(0.0f,z - z_y2)),vy_y2,vx_y2);


            float vx_adaptl = blockvelx < 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_x * fcabs(blockvelx) /dx))*vx + min(0.5f,dt * capture_x *fcabs(blockvelx) /dx) * blockvelx : vx;
            float vy_adaptl = blockvely < 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_y *fcabs(blockvely) /dx))*vy + min(0.5f,dt * capture_y *fcabs(blockvely) /dx) * blockvely : vy;
            float vx_adaptr = blockvelx > 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_x * fcabs(blockvelx) /dx))*vx + min(0.5f,dt * capture_x *fcabs(blockvelx) /dx) * blockvelx : vx;
            float vy_adaptr = blockvely > 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_y *fcabs(blockvely) /dx))*vy + min(0.5f,dt * capture_y *fcabs(blockvely) /dx) * blockvely : vy;
            float vx_adapt_x1 = blockvelx_x1 > 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_x1 * fcabs(blockvelx_x1) /dx))*vx_x1 + min(0.5f,dt * capture_x1 *fcabs(blockvelx_x1) /dx) * blockvelx_x1 : vx_x1;
            float vy_adapt_y1 = blockvely_y1 > 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_y1 *fcabs(blockvely_y1) /dx))*vy_y1 + min(0.5f,dt * capture_y1 *fcabs(blockvely_y1) /dx) * blockvely_y1 : vy_y1;
            float vx_adapt_x2 = blockvelx_x2 < 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_x2 * fcabs(blockvelx_x2) /dx))*vx_x2 + min(0.5f,dt * capture_x2 *fcabs(blockvelx_x2) /dx) * blockvelx_x2 : vx_x2;
            float vy_adapt_y2 = blockvely_y2 < 0.0? max(0.5f,1.0f-min(0.5f,dt * capture_y2 *fcabs(blockvely_y2) /dx))*vy_y2 + min(0.5f,dt * capture_y2 *fcabs(blockvely_y2) /dx) * blockvely_y2 : vy_y2;

            //float3 hll_x1 = z_x1 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowx1bt *h_x1 -max(0.0f,z - z_x1)),vx_x1,vy_x1,max(0.0f,factor_flowx1b *h -max(0.0f,z_x1-z)),vx,vy);
            //float3 hll_x2 = z_x2 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowx2b *h -max(0.0f,z_x2 - z)),vx,vy,max(0.0f,factor_flowx2bt *h_x2 -max(0.0f,z - z_x2)),vx_x2,vy_x2);
            //float3 hll_y1 = z_y1 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowy1bt *h_y1 -max(0.0f,z - z_y1)),vy_y1,vx_y1,max(0.0f,factor_flowy1b *h -max(0.0f,z_y1 - z)),vy,vx);
            //float3 hll_y2 = z_y2 < -1000? float3(0.0,0.0,0.0):F_HLL2(max(0.0f,factor_flowy2b *h -max(0.0f,z_y2 - z)),vy,vx,max(0.0f,factor_flowy2bt *h_y2 -max(0.0f,z - z_y2)),vy_y2,vx_y2);

            float fb_x1 = max(0.0f,min(factor_flowx1bt,factor_flowx1b));
            float fb_x2 = max(0.0f,min(factor_flowx2bt,factor_flowx2b));
            float fb_y1 = max(0.0f,min(factor_flowy1bt,factor_flowy1b));
            float fb_y2 = max(0.0f,min(factor_flowy2bt,factor_flowy2b));
            float h_corr_x1 = max(0.0f,(h_x1 -max(0.0f,z - z_x1)));
            float h_corr_x1b = max(0.0f,(h -max(0.0f,z_x1-z)));
            float h_corr_x2 = max(0.0f,(h -max(0.0f,z_x2 - z)));
            float h_corr_x2b = max(0.0f,(h_x2 -max(0.0f,z - z_x2)));
            float h_corr_y1 = max(0.0f,(h_y1 -max(0.0f,z - z_y1)));
            float h_corr_y1b = max(0.0f,(h -max(0.0f,z_y1 - z)));
            float h_corr_y2 = max(0.0f,(h -max(0.0f,z_y2 - z)));
            float h_corr_y2b = max(0.0f,(h_y2 -max(0.0f,z - z_y2)));
            float3 hll_x1 = z_x1 < -1000? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_x1,vx_adapt_x1,vy_x1,fb_x1, h_corr_x1b,vx_adaptl,vy, fb_x1);
            float3 hll_x2 = z_x2 < -1000? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_x2,vx_adaptr,vy,fb_x2,h_corr_x2b,vx_adapt_x2,vy_x2,fb_x2);
            float3 hll_y1 = z_y1 < -1000? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_y1,vy_adapt_y1,vx_y1,fb_y1,h_corr_y1b,vy_adaptl,vx,fb_y1);
            float3 hll_y2 = z_y2 < -1000? float3(0.0,0.0,0.0):F_HLL2FB(h_corr_y2,vy_adaptr,vx,fb_y2,h_corr_y2b,vy_adapt_y2,vx_y2,fb_y2);


            float C = 0.25f;

            //float flux_x1 = z_x1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):+tx*(hll_x1.x);//max(-h * C,min((float)(+tx*(hll_x1.x)),h_x1 * C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            //float flux_x2 = z_x2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):-tx*(hll_x2.x);//max(-h_x2 * C,min((float)(-tx*(hll_x2.x)),h * C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            //float flux_y1 = z_y1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):+tx*(hll_y1.x);//max(-h *  C,min((float)(+tx*(hll_y1.x)),h_y1 * C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            //float flux_y2 = z_y2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):-tx*(hll_y2.x);//max(-h_y2 * C,min((float)(-tx*(hll_y2.x)),h  * C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))


            float fluxo_x1 = +tx*(hll_x1.x) + (blockvelx < 0? h * dt * capture_x * blockvelx/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvelx_x1/dx : 0.0);
            float fluxo_x2 = -tx*(hll_x2.x) + (blockvelx > 0? -h * dt *capture_x * blockvelx/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvelx_x2/dx : 0.0);
            float fluxo_y1 = +tx*(hll_y1.x) + (blockvely < 0? h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvely_y1/dx : 0.0);
            float fluxo_y2 = -tx*(hll_y2.x) + (blockvely > 0? -h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvely_y2/dx : 0.0);

            float flux_x1 = z_x1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowx1t * C,min((float)(fluxo_x1),h_x1 *factor_flowx1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            float flux_x2 = z_x2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowx2t * C,min((float)(fluxo_x2),h_x2 *factor_flowx2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            float flux_y1 = z_y1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowy1t * C,min((float)(fluxo_y1),h_y1 *factor_flowy1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
            float flux_y2 = z_y2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowy2t * C,min((float)(fluxo_y2),h_y2 *factor_flowy2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))

            float fluxor_x1 = 1.0;//min(1.0f,max(-1.0f,flux_x1/maxmod(1e-4f,fluxo_x1)));
            float fluxor_x2 = 1.0;// min(1.0f,max(-1.0f,flux_x2/maxmod(1e-4f,fluxo_x2)));
            float fluxor_y1 = 1.0;// min(1.0f,max(-1.0f,flux_y1/maxmod(1e-4f,fluxo_y1)));
            float fluxor_y2 = 1.0;// min(1.0f,max(-1.0f,flux_y2/maxmod(1e-4f,fluxo_y2)));



            qfout = qfout +z_x1 < -1000? flux_x1 : 0.0;
            qfout = qfout +z_x2 < -1000? flux_x2 : 0.0;
            qfout = qfout +z_y1 < -1000? flux_y1 : 0.0;
            qfout = qfout +z_y2 < -1000? flux_y2 : 0.0;

            write_imagef(QFX1, int2(gx,gy), flux_x1 *(dx*dx));
            write_imagef(QFX2, int2(gx,gy), flux_x2 *(dx*dx));
            write_imagef(QFY1, int2(gx,gy), flux_y1 *(dx*dx));
            write_imagef(QFY2, int2(gx,gy), flux_y2 *(dx*dx));

            int edges = ((z_x1 < -1000)?1:0) +((z_x2 < -1000)?1:0)+((z_y1 < -1000)?1:0)+((z_y2 < -1000)?1:0);

            bool edge = (z_x1 < -1000 || z_x2 < -1000 || z_y1 < -1000 || z_y2 < -1000);
            float hold = h;
            float hn = ((max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));

            //float qxn = h * vx - 0.5 * GRAV *hn*sx_z * dt- tx*(hll_x2.y - hll_x1.y) - tx*(hll_y2.z - hll_y1.z);
            //float qyn = h * vy - 0.5 * GRAV *hn*sy_z * dt- tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y);

            //
            //

            float r_x = (h_x2 -h)/max(1e-5f,(h-h_x1));
            float r_y = (h_y2 -h)/max(1e-5f,(h-h_y1));
            r_x = min(r_x,1.0f/r_x);
            r_y = min(r_y,1.0f/r_y);
            float r = 1.0-max(0.0f,min(1.0f,min(r_x,r_y)));


            float f_centre_x = 0.5 * GRAV*(h)*(((h - h_corr_x2) - (h - h_corr_x1)) > 0.0? 1.0:-1.0)*fcabs((h - h_corr_x2) - (h - h_corr_x1));
            float f_centre_y = 0.5 * GRAV*(h)*(((h - h_corr_y2) - (h - h_corr_y1)) > 0.0? 1.0:-1.0)*fcabs((h - h_corr_y2) - (h - h_corr_y1));

            //float f_centre_x = 0.5 * GRAV*(h)*((min(z + h,zcc_x2)- min(z + h,zcc_x1 )) > 0.0? 1.0:-1.0)*fcabs(min(z + h,zcc_x2)- min(z + h,zcc_x1));
            //float f_centre_y = 0.5 * GRAV*(h)*((min(z + h,zcc_y2)- min(z + h,zcc_y1 )) > 0.0? 1.0:-1.0)*fcabs(min(z + h,zcc_y2)- min(z + h,zcc_y1));


            float fluxmc_x1x = (blockvelx < 0? h * dt *capture_x * blockvelx* blockvelx/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvelx_x1* blockvelx_x1/dx : 0.0);
            float fluxmc_x2x = (blockvelx > 0? -h * dt *capture_x * blockvelx* blockvelx/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvelx_x2 *blockvelx_x2/dx : 0.0);
            float fluxmc_y1x = (blockvely < 0? h * dt *capture_y * blockvely* blockvelx/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvelx_y1* blockvely_y1/dx : 0.0);
            float fluxmc_y2x = (blockvely > 0? -h * dt *capture_y * blockvely* blockvelx/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvelx_y2* blockvely_y2/dx : 0.0);

            float fluxmc_x1y = (blockvelx < 0? h * dt *capture_x * blockvelx * blockvely/dx : 0.0) + (blockvelx_x1 > 0? h_x1 * dt *capture_x1 * blockvely_x1* blockvely_y1/dx : 0.0);
            float fluxmc_x2y = (blockvelx > 0? -h * dt *capture_x * blockvelx * blockvely/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvely_x2* blockvely_y2/dx : 0.0);
            float fluxmc_y1y = (blockvely < 0? h * dt *capture_y * blockvely* blockvely/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvely_y1* blockvely_y1/dx : 0.0);
            float fluxmc_y2y = (blockvely > 0? -h * dt *capture_y * blockvely* blockvely/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvely_y2* blockvely_y2/dx : 0.0);

            float qxn = h * vx - tx*(fluxor_x2 *hll_x2.y - fluxor_x1 *hll_x1.y + f_centre_x)  - tx*(fluxor_y2*hll_y2.z - fluxor_y1*hll_y1.z);// + fluxmc_x1x + fluxmc_x2x + fluxmc_y1x + fluxmc_y2x ;//- 0.5 * GRAV *hn*sx_z * dt
            float qyn = h * vy - tx*(fluxor_x2 *hll_x2.z - fluxor_x1 *hll_x1.z) - tx*(fluxor_y2 *hll_y2.y - fluxor_y1 *hll_y1.y + f_centre_y);// + fluxmc_x1y + fluxmc_x2y + fluxmc_y1y + fluxmc_y2y ;//- 0.5 * GRAV *hn*sy_z * dt

            float vsq = sqrt((float)(vx * vx + vy * vy));
            float nsq1 = (0.001+n)*(0.001+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
            float nsq = nsq1*vsq*dt;

            float vxn = max(0.5,(1.0 -  dt * capture_x * fcabs(blockvelx)/dx)) *(float)((qxn/(1.0f+nsq)))/max(0.01f,(float)(hn)) + min(0.5,1.0 * dt * capture_x *blockvelx* fcabs(blockvelx)/dx) + blockfx * dt/(max(0.01f * dx,hn)*dx*dx * 1000.0);
            float vyn = max(0.5,(1.0 -  dt * capture_y * fcabs(blockvely)/dx)) *(float)((qyn/(1.0f+nsq)))/max(0.01f,(float)(hn)) + min(0.5,1.0 * dt * capture_y *blockvely* fcabs(blockvely)/dx) + blockfy * dt/(max(0.01f * dx,hn)*dx*dx * 1000.0);

            //float vxn = (float)((qxn/(1.0f+nsq)))/max(0.01f,(float)(hn)) + blockfx * dt/(max(0.01f * dx,hn)*dx*dx * 1000.0);
            //float vyn = (float)((qyn/(1.0f+nsq)))/max(0.01f,(float)(hn)) + blockfy * dt/(max(0.01f * dx,hn)*dx*dx * 1000.0);


            float threshold = 0.01 * dx;
            if(hn < threshold)
            {
                float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
                float acc_eff = (vxn -vx)/max(0.0001f,dt);

                float v_kin = (sx_zh>0.0?1.0:-1.0) * hn * sqrt(hn) * max(0.001f,sqrt(sx_zh>0.0?sx_zh:-sx_zh))/(0.001f+n);

                vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

            }

            if(hn < threshold)
            {
                float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
                float acc_eff = (vyn -vy)/max(0.0001f,dt);

                float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * max(0.001f,sqrt(sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

                vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

            }

            float infil_act = min((float)(infilpot),(float)(hn));

            hn = hn - infil_act + rain * dt_hydro/max(1e-10f,dt);

            hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
            vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
            vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;

            vxn = min(vmax,max(-vmax,vxn));
            vyn = min(vmax,max(-vmax,vyn));


            float dt_req = 0.25f *dx/( min(100.0f,max(0.01f,(sqrt(vxn*vxn + vyn * vyn)))));

            float flux_chx1 = 0.0f;
            float flux_chx2 = 0.0f;

            //if channel is present, calculate channel interactions and channel flow
            int chmask = round(read_imagef(CH_MASK,sampler, int2(gx,gy)).x);
            if(chmask > 0 && chmask < 10)
            {
                    //channel properties
                    float ch_h = read_imagef(CH_H,sampler, int2(gx,gy)).x;
                    float ch_width  = read_imagef(CH_WIDTH,sampler, int2(gx,gy)).x;
                    float ch_height = read_imagef(CH_HEIGHT,sampler, int2(gx,gy)).x;
                    float ch_v = read_imagef(CH_V,sampler, int2(gx,gy)).x;
                    float ch_vol = ch_h * ch_width * dx;

                    //channel mask dubbles as ldd
                    int gx_next = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[chmask])));
                    int gy_next = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[chmask])));

                    float chn_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;
                    float chn_h = read_imagef(CH_H,sampler, int2(gx_next,gy_next)).x;
                    float chn_width = read_imagef(CH_WIDTH,sampler, int2(gx_next,gy_next)).x;
                    float chn_height = read_imagef(CH_HEIGHT,sampler, int2(gx_next,gy_next)).x;
                    float chn_v = read_imagef(CH_V,sampler, int2(gx_next,gy_next)).x;
                    float chn_vol = chn_h * chn_width * dx;

                    float chhn = ch_h;
                    float chvn = ch_v;

                    float ch_vadd = 0.0f;
                    float ch_vaddw = 0.5f;
                    //flow out to next cell
                    if(chmask == 5)
                    {
                            float ch_slope = (ch_h)/dx;
                            float ch_q = max(0.0f,dt * ch_h * ch_width * sqrt((float)(0.001+ch_h))/(dx * (0.05f)));
                            ch_q = max(0.0f,min(0.25f * ch_vol,ch_q));
                            chhn = chhn - 0.5 * ch_q/(ch_width * dx);

                            flux_chx2 = flux_chx2 + 0.5 * ch_q;

                            ch_vadd = ch_vadd + dt * 0.5 * GRAV * max(-1.0f,min(1.0f,(float)(ch_slope)));

                            qfout = qfout + 0.5 * ch_q;
                    }else
                    {
                            //float ch_q = dt*0.5*(ch_h+chn_h)*0.5*(ch_width+chn_width)*0.5*(ch_v+chn_v);
                            float3 hll_x1 = F_HLL2(ch_h,ch_v,0,chn_h,chn_v,0);
                            float ch_q = (dt/dx)*(max(ch_width,chn_width)/dx)*((dx * 0.5*(chn_width +ch_width)) *hll_x1.x);
                            ch_q = min(0.25f * ch_vol,ch_q);
                            ch_q = max(-0.25f * chn_vol,ch_q);
                            ch_q = ch_q * 0.5;


                            float ch_slope = (z + ch_h - chn_z - chn_h)/dx;
                            ch_vadd = ch_vadd + dt * 0.5 * GRAV * max(-1.0f,min(1.0f,(float)(ch_slope)));
                            if(ch_q < 0)
                            {
                                    float new_ch_vol = chhn*(ch_width*dx);
                                    chvn = (chvn * new_ch_vol - chn_v *(ch_q))/max(0.01f,new_ch_vol - ch_q);
                            }

                            chhn = chhn - ch_q/(ch_width * dx);
                            flux_chx2 = flux_chx2 + ch_q;
                    }

                    for(int i = 1; i < 10; i++)
                    {
                            if(i == 5)
                            {
                                    continue;
                            }
                            int gx_prev = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[i])));
                            int gy_prev = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[i])));

                            int lddp = round(read_imagef(CH_MASK,sampler, int2(gx_prev,gy_prev)).x);
                            float chp_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;

                            if(lddp > 0 && lddp < 10 && lddp != 5 && chp_z > -1000)
                            {

                                    if(flow_to(gx_prev,gy_prev,gx,gy,lddp))
                                    {

                                            float chp_h = read_imagef(CH_H,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_width = read_imagef(CH_WIDTH,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_height = read_imagef(CH_HEIGHT,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_v = read_imagef(CH_V,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_vol = chp_h * chp_width * dx;

                                            //flow in from previous cell
                                            //float chp_q = dt*0.5*(chp_h+ch_h)*0.5*(ch_width+chp_width)*0.5*(ch_v +chp_v);
                                            float3 hll_x2 = F_HLL2(chp_h,chp_v,0,ch_h,ch_v,0);
                                            float chp_q = (dt/dx)*(max(ch_width,chp_width)/dx)*((dx * 0.5*(chp_width +ch_width)) *hll_x2.x);
                                            chp_q = min(0.25f * chp_vol,chp_q);
                                            chp_q = max(-0.25f * ch_vol,chp_q);
                                            chp_q = chp_q * 0.5;

                                            float chp_slope = (chp_z + chp_h- z - ch_h)/dx;
                                            if(chp_q > 0)
                                            {

                                                    float new_ch_vol = chhn*(ch_width*dx);
                                                    chvn = (chvn * new_ch_vol + chp_v *(chp_q))/max(0.01f,new_ch_vol +chp_q);
                                            }
                                            ch_vadd = ch_vadd + dt * 0.5 * GRAV * max(-1.0f,min(1.0f,(float)(chp_slope)));
                                            ch_vaddw = ch_vaddw + 0.5f * chp_width/ch_width;

                                            chhn = chhn +chp_q/(ch_width * dx);

                                            flux_chx1 = flux_chx1 + chp_q;
                                    }
                            }
                    }
                    if(ch_vaddw > 1)
                    {
                            ch_vadd = ch_vadd/ch_vaddw;
                    }

                    chhn = min(99.0f,max(0.0f,(float)(chhn)));

                    chvn = chvn + ch_vadd;
                    float chvsq = sqrt((float)(chvn * chvn));
                    float chnsq1 = (0.1)*(0.1)*GRAV/pow((float)(chhn),(float)(4.0/3.0));
                    float chnsq = chnsq1*chvsq*dt;
                    chvn = min(25.0f,max(-25.0f,(float)(chvn/(1.0f+chnsq))));


                    //channel inflow and outflow
                    if(chhn < ch_height)
                    {
                            float vol_room = (ch_height - chhn)*ch_width;
                            float v_chin = hn * sqrt(hn) * sqrt(hn/dx)/(0.1f * 0.1f);
                            float flux_in = max(0.0f,min((float)(vol_room), hn *(dx*dx) *min(0.05f,max(0.0f,(float)( dt * v_chin/max(1.0f,(float)(dx - ch_width)))))));

                            chhn = chhn + flux_in/(dx * ch_width);
                            hn = hn - flux_in/(dx * dx);
                            write_imagef(QFCHIN, int2(gx,gy),- flux_in);
                    }else if(chhn > ch_height)
                    {
                            float vol_tomuch = max(0.0f,(chhn - ch_height)*ch_width);
                            chhn = ch_height;
                            hn = hn + vol_tomuch/(dx * dx);
                            write_imagef(QFCHIN, int2(gx,gy), vol_tomuch);
                    }else
                    {

                        write_imagef(QFCHIN, int2(gx,gy), 0.0);
                    }



                    write_imagef(CH_HN, int2(gx,gy), chhn);
                    write_imagef(CH_VN, int2(gx,gy), chvn);

                    dt_req = 0.25f *min(dt_req,dx/( min(100.0f,max(0.01f,(sqrt(chvn*chvn))))));


            }else
            {
                    write_imagef(CH_HN, int2(gx,gy), 0.0f);
                    write_imagef(CH_VN, int2(gx,gy), 0.0f);
            }

            write_imagef(QFCHX1, int2(gx,gy), flux_chx1);
            write_imagef(QFCHX2, int2(gx,gy), flux_chx2);

            write_imagef(InfilAct, int2(gx,gy), infilcum + infil_act);
            write_imagef(Hn, int2(gx,gy), hn);
            write_imagef(Vxn, int2(gx,gy), vxn);
            write_imagef(Vyn, int2(gx,gy), vyn);

            write_imagef(QFOUTN, int2(gx,gy),qfout);

            write_imagef(DTREQ, int2(gx,gy),dt_req);


            float ui = 0;
            if(ui_image == 1)
            {
                    ui = dt_req;//fcabs(capture_x * fcabs(blockvelx)/dx) + fcabs(capture_y * fcabs(blockvely)/dx);
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 14)
            {
                    ui = sqrt(vxn * vxn + vyn * vyn);
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 15)
            {
                    ui =read_imagef(CH_V,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 5)
            {
                    ui = read_imagef(CH_H,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }
      }
#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}

kernel
void flow2(
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
				__write_only image2d_t H,
				__write_only image2d_t Vx,
				__write_only image2d_t Vy,
				__write_only image2d_t InfilCum,
				__write_only image2d_t CH_H,
				__write_only image2d_t CH_V,
				__read_only image2d_t Hn,
				__read_only image2d_t Vxn,
				__read_only image2d_t Vyn,
				__read_only image2d_t InfilAct,
				__read_only image2d_t CH_HN,
                                __read_only image2d_t CH_VN,
                                __read_only image2d_t HFMAX,
                                __read_only image2d_t VFMAX,
                                __write_only image2d_t HFMAXN,
                                __write_only image2d_t VFMAXN
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
	
        float h = max(0.0f,(float)(read_imagef(Hn,sampler, int2(gx,gy)).x));
	float infil_cum = read_imagef(InfilAct,sampler, int2(gx,gy)).x;
	float vx = read_imagef(Vxn,sampler, int2(gx,gy)).x;
	float vy = read_imagef(Vyn,sampler, int2(gx,gy)).x;

	float chh = read_imagef(CH_HN,sampler, int2(gx,gy)).x;
	float chv = read_imagef(CH_VN,sampler, int2(gx,gy)).x;



	write_imagef(InfilCum, int2(gx,gy), infil_cum);
	write_imagef(H, int2(gx,gy), h);
	write_imagef(Vx, int2(gx,gy), vx);
	write_imagef(Vy, int2(gx,gy), vy);
	write_imagef(CH_H, int2(gx,gy), chh);
	write_imagef(CH_V, int2(gx,gy), chv);


        float hfmax = read_imagef(HFMAX,sampler, int2(gx,gy)).x;
        float vfmax = read_imagef(VFMAX,sampler, int2(gx,gy)).x;
        float vf =(vx*vx + vy*vy > 0)? sqrt(vx*vx + vy*vy) : 0.0f;
        write_imagef(HFMAXN, int2(gx,gy), max(h,hfmax));
        write_imagef(VFMAXN, int2(gx,gy), max(vf,vfmax));
#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}


