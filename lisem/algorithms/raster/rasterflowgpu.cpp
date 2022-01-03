#include "rasterflowgpu.h"


//Default OPENCL kernels for flow processes
QMutex CGLFlowKernelMutex;
QList<OpenCLProgram *> CGLGPUFlowKernels;
bool CGLHasGPUFlowKernels = false;

//one input
OpenCLProgram * CGLKernelWaterFlowDT1 = nullptr;
OpenCLProgram * CGLKernelWaterFlowDT2 = nullptr;
OpenCLProgram * CGLKernelWaterFlow2 = nullptr;
OpenCLProgram * CGLKernelMixtureFlowDT1 = nullptr;
OpenCLProgram * CGLKernelMixtureFlowDT2 = nullptr;
OpenCLProgram * CGLKernelMixtureFlow2 = nullptr;
