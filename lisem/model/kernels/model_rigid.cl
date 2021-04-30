
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


#endif


kernel
void rigid(
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
                                __read_only image2d_t BlockingX,
                                __read_only image2d_t BlockingY,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image
                                )
{
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif


    ///////
    //Cell Location Calculations
    //Here we go from the invocation id to a row and column number for the cell and its neighbors
    //////

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

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    ///////
    //Actual model code
    //////

    float bx = read_imagef(BlockingX,sampler, int2(gx,gy)).x;
    float by = read_imagef(BlockingY,sampler, int2(gx,gy)).x;
    float ui = 0;
    if(ui_image == 30)
    {
            write_imagef(OUTPUT_UI, int2(gx,gy), (bx+by));

    }

#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}


kernel
void rigid2(
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
                                __read_only image2d_t BlockingX,
                                __read_only image2d_t BlockingY,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image
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

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
    
#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}

