#ifndef MODELCPUCODE_H
#define MODELCPUCODE_H

#include "geo/raster/map.h"
#include "cmath"

typedef struct int2
{
    int x = 0;
    int y = 0;
    inline int2(int ix, int iy)
    {
        x = ix;
        y = iy;
    }
} int2;

typedef struct float1
{
    float x;
    inline float1(float ix)
    {
        x = ix;
    }
} float1;



static float min(float a, float b)
{
    return std::min(a,b);
}
static float max(float a, float b)
{
    return std::max(a,b);
}

static float minmod(float a, float b)
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

#define float3 LSMVector3
#define LISEM_KERNEL_CPUCODE
#define kernel inline static
#define __read_only
#define __write_only
#define image2d_t MaskedRaster<float>*
#define sampler_t int
#define CLK_NORMALIZED_COORDS_FALSE 0x00000001
#define CLK_ADDRESS_NONE 0x00000010
#define CLK_FILTER_NEAREST 0x00000100
#define get_global_id(x) (id_1d_cpu)
#define __constant const

#define GRAV 9.91f

const int ch_dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
const int ch_dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};


static inline void write_imagef(MaskedRaster<float> *m , int2 p, float v)
{

    (*m)[p.y][p.x] = v;
}

static inline float1 read_imagef(MaskedRaster<float> *m, int, int2 p)
{

    return (*m)[p.y][p.x];
}

#include "kernels/model_flow.cl"
#include "kernels/model_flow_solids.cl"
#include "kernels/model_flow_solidsparticle.cl"
#include "kernels/model_groundwater.cl"
#include "kernels/model_hydrology.cl"
#include "kernels/model_sediment.cl"
#include "kernels/model_slopestability.cl"
#include "kernels/sort.cl"
#include "kernels/indexstore.cl"
#include "kernels/particles.cl"
#include "kernels/particles_mpm.cl"
#include "kernels/model_rigid.cl"
/*#include "model_rotstab.cl"
#include "model_evapotranspiration.cl"*/



#endif // MODELCPUCODE_H
