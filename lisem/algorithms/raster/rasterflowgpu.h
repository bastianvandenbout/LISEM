#ifndef RASTERFLOWGPU_H
#define RASTERFLOWGPU_H


#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX
#endif

#define __CL_ENABLE_EXCEPTIONS

#include "error.h"
#include "openclprogram.h"
#include "geo/raster/map.h"
//#include "scriptarray.h"
#include "qmutex.h"
#include "openglclmanager.h"
#include "defines.h"
#include "rastergpucode.h"

//Default OPENCL kernels for flow processes
LISEM_API extern QMutex CGLFlowKernelMutex;
LISEM_API extern QList<OpenCLProgram *> CGLGPUFlowKernels;
LISEM_API extern bool CGLHasGPUFlowKernels;

//one input
LISEM_API extern OpenCLProgram * CGLKernelWaterFlow1;
LISEM_API extern OpenCLProgram * CGLKernelWaterFlow2;
LISEM_API extern OpenCLProgram * CGLKernelMixtureFlow1;
LISEM_API extern OpenCLProgram * CGLKernelMixtureFlow2;


inline static void LoadDefaultGPUFlowModels()
{
    CGLKernelMutex.lock();

    LISEMS_STATUS("Compiling default GPU Flow Models (Only occurs on first usage)");

    CGLKernelWaterFlow1;
    CGLKernelWaterFlow2;
    CGLKernelMixtureFlow1;
    CGLKernelMixtureFlow2;

    CGLHasGPUKernels = true;

    CGLKernelMutex.unlock();

}

inline static void DestroyDefaultGPUFlowModels()
{
    CGLFlowKernelMutex.lock();
    for(int i = 0; i < CGLGPUFlowKernels.length(); i++)
    {
        CGLGPUFlowKernels.at(i)->Destroy();
        delete CGLGPUFlowKernels.at(i);
    }
    CGLKernelMutex.unlock();
}

inline static bool HasDefaultGPUFlowModels()
{
    CGLFlowKernelMutex.lock();
    bool has = CGLHasGPUFlowKernels;
    CGLFlowKernelMutex.unlock();

    return has;
}


inline static std::vector<AS_GPUMap *> DynamicWaveGPU(AS_GPUMap * DEM, AS_GPUMap * N, AS_GPUMap * HI,  AS_GPUMap * VXI, AS_GPUMap * VYI, float _dt, float courant )
{




}


#endif // RASTERFLOWGPU_H
