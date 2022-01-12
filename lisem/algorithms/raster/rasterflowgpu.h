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
#include "lsmio.h"
#include "OpenCLUtil.h"

//Default OPENCL kernels for flow processes
LISEM_API extern QMutex CGLFlowKernelMutex;
LISEM_API extern QList<OpenCLProgram *> CGLGPUFlowKernels;
LISEM_API extern bool CGLHasGPUFlowKernels;

//one input
LISEM_API extern OpenCLProgram * CGLKernelWaterFlowDT1;
LISEM_API extern OpenCLProgram * CGLKernelWaterFlowDT2;
LISEM_API extern OpenCLProgram * CGLKernelWaterFlow2;
LISEM_API extern OpenCLProgram * CGLKernelMixtureFlowDT1;
LISEM_API extern OpenCLProgram * CGLKernelMixtureFlowDT2;
LISEM_API extern OpenCLProgram * CGLKernelMixtureFlow2;


inline static void LoadDefaultGPUFlowModels()
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }


    CGLFlowKernelMutex.lock();

    LISEMS_STATUS("Compiling default GPU Flow Models (Only occurs on first usage)");

    QStringList filestrings =ReadFileAsText(QCoreApplication::applicationDirPath() + "/kernels/model_scriptflow.cl");
    std::string file;
    for(int i = 0; i < filestrings.length(); i++)
    {
        file = file + filestrings.at(i).toStdString();
        file = file + "\n";
    }

    CGLKernelWaterFlowDT1 = new OpenCLProgram();
    CGLKernelWaterFlowDT2 = new OpenCLProgram();
    CGLKernelWaterFlow2 = new OpenCLProgram();
    CGLKernelMixtureFlowDT1 = new OpenCLProgram();
    CGLKernelMixtureFlowDT2 = new OpenCLProgram();
    CGLKernelMixtureFlow2 = new OpenCLProgram();

    CGLKernelWaterFlowDT1->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"reduceflowdt1");
    CGLKernelWaterFlowDT2->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"reduceflowdt2");
    CGLKernelWaterFlow2->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"flow");
    CGLKernelMixtureFlowDT1->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"reducemixflowdt1");
    CGLKernelMixtureFlowDT2->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"reducemixflowdt2");
    CGLKernelMixtureFlow2->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,file,"flowsolid");

    CGLGPUFlowKernels.append(CGLKernelWaterFlowDT1);
    CGLGPUFlowKernels.append(CGLKernelWaterFlowDT2);
    CGLGPUFlowKernels.append(CGLKernelWaterFlow2 );
    CGLGPUFlowKernels.append(CGLKernelMixtureFlowDT1);
    CGLGPUFlowKernels.append(CGLKernelMixtureFlowDT2);
    CGLGPUFlowKernels.append(CGLKernelMixtureFlow2);

    CGLHasGPUFlowKernels = true;

    CGLFlowKernelMutex.unlock();

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

inline float GetDynamicWaveTimestep(AS_GPUMap * m, AS_GPUMap * HI,  AS_GPUMap * VXI, AS_GPUMap * VYI, float courant)
{
    if(m->m_GPUData == nullptr)
    {
        LISEMS_ERROR("GPU Map is not valid");
        throw 1;
    }
    //first create output buffer (size/1024 + 1)
    int size = m->m_GPUData->m_dims[0] * m->m_GPUData->m_dims[1];
    int size_workgroup = 256;
    int size_workgroupcount = 256 * 8; //each workgroup has 256 threads, but does 8 items
    int size_output = size / size_workgroupcount;
    int size_corr = size_workgroupcount * (size/size_workgroupcount);
    int remainder = size - size_output * size_workgroupcount;
    if(remainder > 0)
    {
        size_output += 1;
    }


    //create buffer

    int ret = CL_SUCCESS;
    cl_mem outbuf = clCreateBuffer(CGlobalGLCLManager->context(), CL_MEM_READ_WRITE, sizeof(int)*size_output, NULL, &ret);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not create output buffer" + QString::number(ret));
        throw 1;
    }

    cl_kernel kernel1 = CGLKernelWaterFlowDT1->m_kernel();
    cl_kernel kernel2 = CGLKernelWaterFlowDT2->m_kernel();
    const size_t globalWorkSize = size_corr/8;
    const size_t localWorkSize = size_workgroup;

    //two kernels: first one runs reduction per 1024 items
    //second kernel: runs a serial reduction on the remainder of the dataset (size % 1024)

    int sizex = m->m_GPUData->m_dims[0];
    int sizey = m->m_GPUData->m_dims[1];
    int outputoffset = (size_output -1);

    int inOffset = size_workgroupcount * (size/size_workgroupcount);
    ret = clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-1 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-2 "  + QString::number(ret));
        throw 1;
    }
    cl_mem obj1 =m->m_GPUData->m_texcl();
    cl_mem objh =HI->m_GPUData->m_texcl();
    cl_mem objvx =VXI->m_GPUData->m_texcl();
    cl_mem objvy =VYI->m_GPUData->m_texcl();

    float dx =std::fabs(m->dx);

    ret = clSetKernelArg(kernel1, 2, sizeof(float), (void *)&dx);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-3 " + QString::number(ret));
        throw 1;
    }

    ret = clSetKernelArg(kernel1, 3, sizeof(cl_mem), (void *)&obj1);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-4 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 4, sizeof(cl_mem), (void *)&objh);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-5 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 5, sizeof(cl_mem), (void *)&objvx);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-6 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 6, sizeof(cl_mem), (void *)&objvy);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-7 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 7, sizeof(cl_mem), (void *)&outbuf);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-8 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 8, sizeof(float), (void *)&courant);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-8 " + QString::number(ret));
        throw 1;
    }
    //then run the first kernel for (n-1) output buffer results

    size_t wgSize[3] = {256, 1, 1};
    size_t gSize[3] = {size_corr/8, 1, 1};


    ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Error while running kernel1 " + QString::number(ret));
        std::cout << "error is "<< ret << std::endl;
        throw 1;
    }

    //then run the second kernel for the last output buffer result

    if(remainder > 0)
    {
        clSetKernelArg(kernel2, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-1 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-2 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 2, sizeof(float), (void *)&dx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-3 " + QString::number(ret));
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 3, sizeof(cl_mem), (void *)&obj1);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-4 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 4, sizeof(cl_mem), (void *)&objh);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-5 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 5, sizeof(cl_mem), (void *)&objvx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-6 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 6, sizeof(cl_mem), (void *)&objvy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 7, sizeof(cl_mem), (void *)&outbuf);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-8 " + QString::number(ret));
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 8, sizeof(int), (void *)&size);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 9, sizeof(int), (void *)&inOffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-10 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 10, sizeof(int), (void *)&outputoffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-11 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 11, sizeof(float), (void *)&courant);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-11 " + QString::number(ret));
            throw 1;
        }


        ret = clEnqueueTask(CGlobalGLCLManager->q(),kernel2,0,NULL,NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel2 " + QString::number(ret));
            throw 1;
        }
    }


    //read output buffer
    float * results = (float*) malloc(sizeof(float)*size_output);
    ret = clEnqueueReadBuffer(CGlobalGLCLManager->q(), outbuf, CL_TRUE, 0, sizeof(int)*size_output, results, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not read buffer " + QString::number(ret));
        throw 1;
    }


    float result = 1e31;
    // finally do a reduction on the final output buffer outputs
    for(int i = 0; i < size_output; i++)
    {
        result = std::min(result,results[i]);
    }

    ret = clReleaseMemObject(outbuf);
    free(results);

    return result;

}

inline static std::vector<AS_GPUMap *> AS_DynamicWaveGPU(AS_GPUMap * DEM, AS_GPUMap * N, AS_GPUMap * HI,  AS_GPUMap * VXI, AS_GPUMap * VYI, float _dt, float courant )
{

    if(!HasDefaultGPUFlowModels())
    {
        LoadDefaultGPUFlowModels();
    }

    //Run the actual kernel,
    //set the arguments for input and output

    int size = DEM->m_GPUData->m_dims[0] *DEM->m_GPUData->m_dims[1];
    int sizex = DEM->m_GPUData->m_dims[0];
    int sizey = DEM->m_GPUData->m_dims[1];

    AS_GPUMap * HN = HI->GetCopy0();
    AS_GPUMap * VXN = VXI->GetCopy0();
    AS_GPUMap * VYN = VYI->GetCopy0();
    float t = 0;
    while(t + 1e-6 < _dt)
    {
        //get timestep kernel
        float timestep = GetDynamicWaveTimestep(DEM,HI,VXI,VYI,courant);
        std::cout << "timestep " << timestep << std::endl;
        timestep = std::max(1e-6f,std::min(std::min(_dt - t,timestep),_dt));
        float dx = std::fabs(DEM->dx);

        t += timestep;


        //get flow kernel
        cl_kernel kernel1 = CGLKernelWaterFlow2->m_kernel();

        cl_mem objdem =DEM->m_GPUData->m_texcl();
        cl_mem objn =N->m_GPUData->m_texcl();
        cl_mem objh =HI->m_GPUData->m_texcl();
        cl_mem objvx =VXI->m_GPUData->m_texcl();
        cl_mem objvy =VYI->m_GPUData->m_texcl();
        cl_mem objhn =HN->m_GPUData->m_texcl();
        cl_mem objvxn =VXN->m_GPUData->m_texcl();
        cl_mem objvyn =VYN->m_GPUData->m_texcl();


        //set arguments
        int ret =clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-1 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-2 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 2, sizeof(float), (void *)&dx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-3 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 3, sizeof(float), (void *)&timestep);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-4 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 4, sizeof(cl_mem), (void *)&objdem);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-5 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 5, sizeof(cl_mem), (void *)&objn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-6 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 6, sizeof(cl_mem), (void *)&objh);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 7, sizeof(cl_mem), (void *)&objvx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 8, sizeof(cl_mem), (void *)&objvy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 9, sizeof(cl_mem), (void *)&objhn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-10 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 10, sizeof(cl_mem), (void *)&objvxn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-11 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 11, sizeof(cl_mem), (void *)&objvyn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-12 " + QString::number(ret));
            throw 1;
        }




        //run flow kernel
        size_t localWorkSize=64;
        size_t globalWorkSize=size;
        globalWorkSize = 64 * divup(size, localWorkSize);

        ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel3 " + QString::number(ret));
            std::cout << "error is "<< ret << std::endl;
            throw 1;
        }

        //copy back
        HI->CopyFrom(HN);
        VXI->CopyFrom(VXN);
        VYI->CopyFrom(VYN);
    }


    //return new maps

    return {HN,VXN,VYN};
}


inline float GetMixtureWaveTimestep(AS_GPUMap * m, AS_GPUMap * HI,  AS_GPUMap * VXI, AS_GPUMap * VYI, AS_GPUMap * HSI,  AS_GPUMap * VSXI, AS_GPUMap * VSYI,float courant)
{
    if(m->m_GPUData == nullptr)
    {
        LISEMS_ERROR("GPU Map is not valid");
        throw 1;
    }
    //first create output buffer (size/1024 + 1)
    int size = m->m_GPUData->m_dims[0] * m->m_GPUData->m_dims[1];
    int size_workgroup = 256;
    int size_workgroupcount = 256 * 8; //each workgroup has 256 threads, but does 8 items
    int size_output = size / size_workgroupcount;
    int size_corr = size_workgroupcount * (size/size_workgroupcount);
    int remainder = size - size_output * size_workgroupcount;
    if(remainder > 0)
    {
        size_output += 1;
    }


    //create buffer

    int ret = CL_SUCCESS;
    cl_mem outbuf = clCreateBuffer(CGlobalGLCLManager->context(), CL_MEM_READ_WRITE, sizeof(int)*size_output, NULL, &ret);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not create output buffer" + QString::number(ret));
        throw 1;
    }

    cl_kernel kernel1 = CGLKernelMixtureFlowDT1->m_kernel();
    cl_kernel kernel2 = CGLKernelMixtureFlowDT2->m_kernel();
    const size_t globalWorkSize = size_corr/8;
    const size_t localWorkSize = size_workgroup;

    //two kernels: first one runs reduction per 1024 items
    //second kernel: runs a serial reduction on the remainder of the dataset (size % 1024)

    int sizex = m->m_GPUData->m_dims[0];
    int sizey = m->m_GPUData->m_dims[1];
    int outputoffset = (size_output -1);

    std::cout << "size " << sizex << " " << sizey << " " << size_output << " " << size << std::endl;

    int inOffset = size_workgroupcount * (size/size_workgroupcount);
    ret = clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-1 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-2 "  + QString::number(ret));
        throw 1;
    }
    cl_mem obj1 =m->m_GPUData->m_texcl();
    cl_mem objh =HI->m_GPUData->m_texcl();
    cl_mem objvx =VXI->m_GPUData->m_texcl();
    cl_mem objvy =VYI->m_GPUData->m_texcl();
    cl_mem objhs =HSI->m_GPUData->m_texcl();
    cl_mem objvsx =VSXI->m_GPUData->m_texcl();
    cl_mem objvsy =VSYI->m_GPUData->m_texcl();

    float dx =std::fabs(m->dx);

    ret = clSetKernelArg(kernel1, 2, sizeof(float), (void *)&dx);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-3 " + QString::number(ret));
        throw 1;
    }

    ret = clSetKernelArg(kernel1, 3, sizeof(cl_mem), (void *)&obj1);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-4 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 4, sizeof(cl_mem), (void *)&objh);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-5 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 5, sizeof(cl_mem), (void *)&objvx);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-6 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 6, sizeof(cl_mem), (void *)&objvy);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-7 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 7, sizeof(cl_mem), (void *)&objhs);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-5 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 8, sizeof(cl_mem), (void *)&objvsx);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-6 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 9, sizeof(cl_mem), (void *)&objvsy);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-7 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 10, sizeof(cl_mem), (void *)&outbuf);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-8 " + QString::number(ret));
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 11, sizeof(float), (void *)&courant);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-8 " + QString::number(ret));
        throw 1;
    }
    //then run the first kernel for (n-1) output buffer results

    size_t wgSize[3] = {256, 1, 1};
    size_t gSize[3] = {size_corr/8, 1, 1};


    ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Error while running kernel1 " + QString::number(ret));
        std::cout << "error is "<< ret << std::endl;
        throw 1;
    }

    //then run the second kernel for the last output buffer result

    if(remainder > 0)
    {
        clSetKernelArg(kernel2, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-1 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-2 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 2, sizeof(float), (void *)&dx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-3 " + QString::number(ret));
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 3, sizeof(cl_mem), (void *)&obj1);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-4 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 4, sizeof(cl_mem), (void *)&objh);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-5 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 5, sizeof(cl_mem), (void *)&objvx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-6 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 6, sizeof(cl_mem), (void *)&objvy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 7, sizeof(cl_mem), (void *)&objhs);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-5 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 8, sizeof(cl_mem), (void *)&objvsx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-6 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 9, sizeof(cl_mem), (void *)&objvsy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 10, sizeof(cl_mem), (void *)&outbuf);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-8 " + QString::number(ret));
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 11, sizeof(int), (void *)&size);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 12, sizeof(int), (void *)&inOffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-10 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 13, sizeof(int), (void *)&outputoffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-11 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 14, sizeof(float), (void *)&courant);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-11 " + QString::number(ret));
            throw 1;
        }


        ret = clEnqueueTask(CGlobalGLCLManager->q(),kernel2,0,NULL,NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel2 " + QString::number(ret));
            throw 1;
        }
    }


    //read output buffer
    float * results = (float*) malloc(sizeof(float)*size_output);
    ret = clEnqueueReadBuffer(CGlobalGLCLManager->q(), outbuf, CL_TRUE, 0, sizeof(int)*size_output, results, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not read buffer " + QString::number(ret));
        throw 1;
    }


    float result = 1e31;
    // finally do a reduction on the final output buffer outputs
    for(int i = 0; i < size_output; i++)
    {
        result = std::min(result,results[i]);
    }

    ret = clReleaseMemObject(outbuf);
    free(results);

    return result;

}


inline static std::vector<AS_GPUMap *> AS_MixtureWaveGPU(AS_GPUMap * DEM, AS_GPUMap * N, AS_GPUMap * HI,  AS_GPUMap * VXI, AS_GPUMap * VYI,AS_GPUMap * HSI,  AS_GPUMap * VSXI, AS_GPUMap * VSYI,AS_GPUMap * IFA, AS_GPUMap * ROCKSIZE, AS_GPUMap * DENSITY, float _dt, float courant, float dragmult )
{

    if(!HasDefaultGPUFlowModels())
    {
        LoadDefaultGPUFlowModels();
    }

    //Run the actual kernel,
    //set the arguments for input and output


    int size = DEM->m_GPUData->m_dims[0] *DEM->m_GPUData->m_dims[1];
    int sizex = DEM->m_GPUData->m_dims[0];
    int sizey = DEM->m_GPUData->m_dims[1];


    AS_GPUMap * HN = HI->GetCopy0();
    AS_GPUMap * VXN = HI->GetCopy0();
    AS_GPUMap * VYN = HI->GetCopy0();
    AS_GPUMap * HSN = HI->GetCopy0();
    AS_GPUMap * VSXN = HI->GetCopy0();
    AS_GPUMap * VSYN = HI->GetCopy0();
    AS_GPUMap * IFAN = HI->GetCopy0();
    AS_GPUMap * ROCKSIZEN = HI->GetCopy0();
    AS_GPUMap * DENSITYN = HI->GetCopy0();


    float t = 0;
    while(t + 1e-6 < _dt)
    {
        //get timestep kernel
        float timestep = GetMixtureWaveTimestep(DEM,HI,VXI,VYI,HSI,VSXI,VSYI,courant);
        std::cout << "timestep " << timestep << std::endl;
        timestep = std::max(1e-6f,std::min(std::min(_dt - t,timestep),_dt));
        float dx = std::fabs(DEM->dx);

        t += timestep;

        //get flow kernel
        cl_kernel kernel1 = CGLKernelMixtureFlow2->m_kernel();

        cl_mem objdem =DEM->m_GPUData->m_texcl();
        cl_mem objn =N->m_GPUData->m_texcl();
        cl_mem objh =HI->m_GPUData->m_texcl();
        cl_mem objvx =VXI->m_GPUData->m_texcl();
        cl_mem objvy =VYI->m_GPUData->m_texcl();
        cl_mem objhn =HN->m_GPUData->m_texcl();
        cl_mem objvxn =VXN->m_GPUData->m_texcl();
        cl_mem objvyn =VYN->m_GPUData->m_texcl();
        cl_mem objhs =HSI->m_GPUData->m_texcl();
        cl_mem objvsx =VSXI->m_GPUData->m_texcl();
        cl_mem objvsy =VSYI->m_GPUData->m_texcl();
        cl_mem objhsn =HSN->m_GPUData->m_texcl();
        cl_mem objvsxn =VSXN->m_GPUData->m_texcl();
        cl_mem objvsyn =VSYN->m_GPUData->m_texcl();
        cl_mem objifa =IFA->m_GPUData->m_texcl();
        cl_mem objrs =ROCKSIZE->m_GPUData->m_texcl();
        cl_mem objd =DENSITY->m_GPUData->m_texcl();
        cl_mem objifan =IFAN->m_GPUData->m_texcl();
        cl_mem objrsn =ROCKSIZEN->m_GPUData->m_texcl();
        cl_mem objdn =DENSITYN->m_GPUData->m_texcl();

        //set arguments
        int ret =clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-1 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-2 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 2, sizeof(float), (void *)&dx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-3 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 3, sizeof(float), (void *)&timestep);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-4 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 4, sizeof(cl_mem), (void *)&objdem);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-5 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 5, sizeof(cl_mem), (void *)&objn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-6 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 6, sizeof(cl_mem), (void *)&objh);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 7, sizeof(cl_mem), (void *)&objvx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 8, sizeof(cl_mem), (void *)&objvy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 9, sizeof(cl_mem), (void *)&objhs);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 10, sizeof(cl_mem), (void *)&objvsx);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 11, sizeof(cl_mem), (void *)&objvsy);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 12, sizeof(cl_mem), (void *)&objifa);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 13, sizeof(cl_mem), (void *)&objrs);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 14, sizeof(cl_mem), (void *)&objd);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 15, sizeof(cl_mem), (void *)&objhn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-10 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 16, sizeof(cl_mem), (void *)&objvxn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-11 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 17, sizeof(cl_mem), (void *)&objvyn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-12 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 18, sizeof(cl_mem), (void *)&objhsn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 19, sizeof(cl_mem), (void *)&objvsxn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 20, sizeof(cl_mem), (void *)&objvsyn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 21, sizeof(cl_mem), (void *)&objifan);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-7 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 22, sizeof(cl_mem), (void *)&objrsn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-8 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 23, sizeof(cl_mem), (void *)&objdn);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 24, sizeof(float), (void *)&dragmult);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel3-9 " + QString::number(ret));
            throw 1;
        }


        //run flow kernel
        size_t localWorkSize=16;
        size_t globalWorkSize=size;
        globalWorkSize = 16 * divup(size, localWorkSize);

        ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel3 " + QString::number(ret));
            std::cout << "error is "<< ret << std::endl;
            throw 1;
        }

        //copy back
        HI->CopyFrom(HN);
        VXI->CopyFrom(VXN);
        VYI->CopyFrom(VYN);
        HSI->CopyFrom(HSN);
        VSXI->CopyFrom(VSXN);
        VSYI->CopyFrom(VSYN);
        IFA->CopyFrom(IFAN);
        ROCKSIZE->CopyFrom(ROCKSIZEN);
        DENSITY->CopyFrom(DENSITYN);
    }


    //return new maps

    return {HN,VXN,VYN,HSN,VSXN,VSYN,IFAN,ROCKSIZEN, DENSITYN};
}



#endif // RASTERFLOWGPU_H
