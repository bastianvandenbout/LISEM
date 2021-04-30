
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


kernel
void GroundWaterFlow(
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
				__read_only image2d_t GW_WF,
				__read_only image2d_t GW_US,
				__read_only image2d_t GW_S,
				__read_only image2d_t THETAS,
				__read_only image2d_t THETAR,
				__read_only image2d_t SoilDepth,
				__read_only image2d_t KSAT_B,
                                __write_only image2d_t GW_SN
				)
{				

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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


	//ground water flow

		
        float ThetaS = read_imagef(THETAS,sampler, int2(gx,gy)).x;
        float Ksat = read_imagef(KSAT_B,sampler, int2(gx,gy)).x;
	
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
	
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;
	
        float h_gw = (read_imagef(GW_S,sampler, int2(gx,gy)).x);
        float h_gw_x1 = (read_imagef(GW_S,sampler, int2(gx_x1,gy)).x);
        float h_gw_x2 = (read_imagef(GW_S,sampler, int2(gx_x2,gy)).x);
        float h_gw_y1 = (read_imagef(GW_S,sampler, int2(gx,gy_y1)).x);
        float h_gw_y2 = (read_imagef(GW_S,sampler, int2(gx,gy_y2)).x);
	
        float sd = (read_imagef(SoilDepth,sampler, int2(gx,gy)).x);
        float sd_x1 = (read_imagef(SoilDepth,sampler, int2(gx_x1,gy)).x);
        float sd_x2 = (read_imagef(SoilDepth,sampler, int2(gx_x2,gy)).x);
        float sd_y1 = (read_imagef(SoilDepth,sampler, int2(gx,gy_y1)).x);
        float sd_y2 = (read_imagef(SoilDepth,sampler, int2(gx,gy_y2)).x);
	
        float h_wf = (read_imagef(GW_WF,sampler, int2(gx,gy)).x);
        float h_wf_x1 = (read_imagef(GW_WF,sampler, int2(gx_x1,gy)).x);
        float h_wf_x2 = (read_imagef(GW_WF,sampler, int2(gx_x2,gy)).x);
        float h_wf_y1 = (read_imagef(GW_WF,sampler, int2(gx,gy_y1)).x);
        float h_wf_y2 = (read_imagef(GW_WF,sampler, int2(gx,gy_y2)).x);
	
        float h_us = (read_imagef(GW_US,sampler, int2(gx,gy)).x);
        float h_us_x1 = (read_imagef(GW_US,sampler, int2(gx_x1,gy)).x);
        float h_us_x2 = (read_imagef(GW_US,sampler, int2(gx_x2,gy)).x);
        float h_us_y1 = (read_imagef(GW_US,sampler, int2(gx,gy_y1)).x);
        float h_us_y2 = (read_imagef(GW_US,sampler, int2(gx,gy_y2)).x);

        float ts = (read_imagef(THETAS,sampler, int2(gx,gy)).x);
        float ts_x1 = (read_imagef(THETAS,sampler, int2(gx_x1,gy)).x);
        float ts_x2 = (read_imagef(THETAS,sampler, int2(gx_x2,gy)).x);
        float ts_y1 = (read_imagef(THETAS,sampler, int2(gx,gy_y1)).x);
        float ts_y2 = (read_imagef(THETAS,sampler, int2(gx,gy_y2)).x);
	
	float sx_zh_x2 = min((float)(0.5),max((float)(-0.5),(float)((z_x2 + (h_gw_x2/ts_x2)-z - h_gw)/dx)));
	float sy_zh_y1 = min((float)(0.5),max((float)(-0.5),(float)((z + h_gw-z_y1 - (h_gw_y1/ts_y1))/dx)));
	float sx_zh_x1 = min((float)(0.5),max((float)(-0.5),(float)((z + h_gw-z_x1 - (h_gw_x1/ts_x1))/dx)));
	float sy_zh_y2 = min((float)(0.5),max((float)(-0.5),(float)((z_y2 + (h_gw_y2/ts_y2)-z - h_gw)/dx)));
	
	float room = max(0.0f,(float)(sd * ts - h_us - h_wf - h_gw));
	float room_x1 = max(0.0f,(float)(sd_x1 * ts_x1 - h_us_x1 - h_wf_x1 - h_gw_x1));
	float room_x2 = max(0.0f,(float)(sd_x2 * ts_x2 - h_us_x2 - h_wf_x2 - h_gw_x2));
	float room_y1 = max(0.0f,(float)(sd_y1 * ts_y1 - h_us_y1 - h_wf_y1 - h_gw_y1));
	float room_y2 = max(0.0f,(float)(sd_y2 * ts_y2 - h_us_y2 - h_wf_y2 - h_gw_y2));
	
	float sx_zh = 0.5f * (sx_zh_x1+sx_zh_x2);
	float sy_zh = 0.5f * (sy_zh_y1+sy_zh_y2);
	
	float q_x1 = dt * sx_zh_x1 * (h_gw_x1/ts_x1) * Ksat/dx;
        float q_x2 = dt * -sx_zh_x2 * (h_gw_x2/ts_x2) * Ksat/dx;
	float q_y1 = dt * sy_zh_y1 * (h_gw_y1/ts_y1) * Ksat/dx;
        float q_y2 = dt * -sy_zh_y2 * (h_gw_y2/ts_y2) * Ksat/dx;
	
	q_x1 = min((float)(0.25 * h_gw_x1),max(q_x1,(float)(-0.25 * h_gw)));
	q_x1 = min((float)(0.25 * room),max(q_x1,(float)(-0.25 * room_x1)));
        q_x2 = min((float)(0.25 * h_gw_x2),max(q_x2,(float)(-0.25 * h_gw)));
	q_x2 = min((float)(0.25 * room),max(q_x2,(float)(-0.25 * room_x2)));
	q_y1 = min((float)(0.25 * h_gw_y1),max(q_y1,(float)(-0.25 * h_gw)));
	q_y1 = min((float)(0.25 * room),max(q_y1,(float)(-0.25 * room_y1)));
	q_y2 = min((float)(0.25 * h_gw_y2),max(q_y2,(float)(-0.25 * h_gw)));
	q_y2 = min((float)(0.25 * room),max(q_y2,(float)(-0.25 * room_y2)));
	
	h_gw = h_gw + q_x1 + q_x2 + q_y1 + q_y2;
	
        write_imagef(GW_SN, int2(gx,gy), h_gw);


        #ifdef LISEM_KERNEL_CPUCODE
        }
        #endif
}
	
	
kernel
void GroundWaterFlow2(
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
				__write_only image2d_t GW_S,
				__read_only image2d_t GW_SN
				)
{			
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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


        float h_gw = (read_imagef(GW_SN,sampler, int2(gx,gy)).x);
	
        write_imagef(GW_S, int2(gx,gy), h_gw);

        #ifdef LISEM_KERNEL_CPUCODE
        }
        #endif

}
