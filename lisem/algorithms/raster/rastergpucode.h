#ifndef RASTERGPUCODE_H
#define RASTERGPUCODE_H

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

//Default OPENCL functions for both arrays and maps
LISEM_API extern QMutex CGLKernelMutex;
LISEM_API extern QList<OpenCLProgram *> CGLGPUKernels;
LISEM_API extern bool CGLHasGPUKernels;

//one input
LISEM_API extern OpenCLProgram * CGLKernelSqrt;
LISEM_API extern OpenCLProgram * CGLKernelExp;
LISEM_API extern OpenCLProgram * CGLKernelLog;
LISEM_API extern OpenCLProgram * CGLKernelSin;
LISEM_API extern OpenCLProgram * CGLKernelAsin;
LISEM_API extern OpenCLProgram * CGLKernelCos;
LISEM_API extern OpenCLProgram * CGLKernelAcos;
LISEM_API extern OpenCLProgram * CGLKernelTan;
LISEM_API extern OpenCLProgram * CGLKernelAtan;
LISEM_API extern OpenCLProgram * CGLKernelSinh;
LISEM_API extern OpenCLProgram * CGLKernelAsinh;
LISEM_API extern OpenCLProgram * CGLKernelCosh;
LISEM_API extern OpenCLProgram * CGLKernelAcosh;
LISEM_API extern OpenCLProgram * CGLKernelTanh;
LISEM_API extern OpenCLProgram * CGLKernelAtanh;
LISEM_API extern OpenCLProgram * CGLKernelAbs;
LISEM_API extern OpenCLProgram * CGLKernelFloor;
LISEM_API extern OpenCLProgram * CGLKernelSmoothstep;
LISEM_API extern OpenCLProgram * CGLKernelDecimal;
LISEM_API extern OpenCLProgram * CGLKernelCbrt;
LISEM_API extern OpenCLProgram * CGLKernelRand;
LISEM_API extern OpenCLProgram * CGLKernelRound;

//two-inputs
LISEM_API extern OpenCLProgram * CGLKernelMin;
LISEM_API extern OpenCLProgram * CGLKernelMax;
LISEM_API extern OpenCLProgram * CGLKernelAdd;
LISEM_API extern OpenCLProgram * CGLKernelMod;
LISEM_API extern OpenCLProgram * CGLKernelSub;
LISEM_API extern OpenCLProgram * CGLKernelMul;
LISEM_API extern OpenCLProgram * CGLKernelDiv;
LISEM_API extern OpenCLProgram * CGLKernelLogN;
LISEM_API extern OpenCLProgram * CGLKernelPow;
LISEM_API extern OpenCLProgram * CGLKernelEq;
LISEM_API extern OpenCLProgram * CGLKernelGreater;
LISEM_API extern OpenCLProgram * CGLKernelSmaller;
LISEM_API extern OpenCLProgram * CGLKernelGreaterEq;
LISEM_API extern OpenCLProgram * CGLKernelSmallerEq;
LISEM_API extern OpenCLProgram * CGLKernelFdim;
LISEM_API extern OpenCLProgram * CGLKernelHypoth;
LISEM_API extern OpenCLProgram * CGLKernelAnd;
LISEM_API extern OpenCLProgram * CGLKernelOr;
LISEM_API extern OpenCLProgram * CGLKernelXOr;
LISEM_API extern OpenCLProgram * CGLKernelNot;
LISEM_API extern OpenCLProgram * CGLKernelSign;

LISEM_API extern OpenCLProgram * CGLKernelfMin;
LISEM_API extern OpenCLProgram * CGLKernelfMax;
LISEM_API extern OpenCLProgram * CGLKernelfAdd;
LISEM_API extern OpenCLProgram * CGLKernelfMod;
LISEM_API extern OpenCLProgram * CGLKernelfSub;
LISEM_API extern OpenCLProgram * CGLKernelfMul;
LISEM_API extern OpenCLProgram * CGLKernelfDiv;
LISEM_API extern OpenCLProgram * CGLKernelfLogN;
LISEM_API extern OpenCLProgram * CGLKernelfPow;
LISEM_API extern OpenCLProgram * CGLKernelfEq;
LISEM_API extern OpenCLProgram * CGLKernelfGreater;
LISEM_API extern OpenCLProgram * CGLKernelfSmaller;
LISEM_API extern OpenCLProgram * CGLKernelfGreaterEq;
LISEM_API extern OpenCLProgram * CGLKernelfSmallerEq;
LISEM_API extern OpenCLProgram * CGLKernelfFdim;
LISEM_API extern OpenCLProgram * CGLKernelfHypoth;
LISEM_API extern OpenCLProgram * CGLKernelfAnd;
LISEM_API extern OpenCLProgram * CGLKernelfOr;
LISEM_API extern OpenCLProgram * CGLKernelfXOr;

LISEM_API extern OpenCLProgram * CGLKernelfrMin;
LISEM_API extern OpenCLProgram * CGLKernelfrMax;
LISEM_API extern OpenCLProgram * CGLKernelfrAdd;
LISEM_API extern OpenCLProgram * CGLKernelfrMod;
LISEM_API extern OpenCLProgram * CGLKernelfrSub;
LISEM_API extern OpenCLProgram * CGLKernelfrMul;
LISEM_API extern OpenCLProgram * CGLKernelfrDiv;
LISEM_API extern OpenCLProgram * CGLKernelfrLogN;
LISEM_API extern OpenCLProgram * CGLKernelfrPow;
LISEM_API extern OpenCLProgram * CGLKernelfrEq;
LISEM_API extern OpenCLProgram * CGLKernelfrGreater;
LISEM_API extern OpenCLProgram * CGLKernelfrSmaller;
LISEM_API extern OpenCLProgram * CGLKernelfrGreaterEq;
LISEM_API extern OpenCLProgram * CGLKernelfrSmallerEq;
LISEM_API extern OpenCLProgram * CGLKernelfrFdim;
LISEM_API extern OpenCLProgram * CGLKernelfrHypoth;
LISEM_API extern OpenCLProgram * CGLKernelfrAnd;
LISEM_API extern OpenCLProgram * CGLKernelfrOr;
LISEM_API extern OpenCLProgram * CGLKernelfrXOr;


//one input
LISEM_API extern OpenCLProgram * CGLArrIKernelSqrt;
LISEM_API extern OpenCLProgram * CGLArrIKernelExp;
LISEM_API extern OpenCLProgram * CGLArrIKernelLog;
LISEM_API extern OpenCLProgram * CGLArrIKernelSin;
LISEM_API extern OpenCLProgram * CGLArrIKernelAsin;
LISEM_API extern OpenCLProgram * CGLArrIKernelCos;
LISEM_API extern OpenCLProgram * CGLArrIKernelAcos;
LISEM_API extern OpenCLProgram * CGLArrIKernelTan;
LISEM_API extern OpenCLProgram * CGLArrIKernelAtan;
LISEM_API extern OpenCLProgram * CGLArrIKernelSinh;
LISEM_API extern OpenCLProgram * CGLArrIKernelAsinh;
LISEM_API extern OpenCLProgram * CGLArrIKernelCosh;
LISEM_API extern OpenCLProgram * CGLArrIKernelAcosh;
LISEM_API extern OpenCLProgram * CGLArrIKernelTanh;
LISEM_API extern OpenCLProgram * CGLArrIKernelAtanh;
LISEM_API extern OpenCLProgram * CGLArrIKernelAbs;
LISEM_API extern OpenCLProgram * CGLArrIKernelFloor;
LISEM_API extern OpenCLProgram * CGLArrIKernelDecimal;
LISEM_API extern OpenCLProgram * CGLArrIKernelSmoothstep;
LISEM_API extern OpenCLProgram * CGLArrIKernelCbrt;
LISEM_API extern OpenCLProgram * CGLArrIKernelRand;
LISEM_API extern OpenCLProgram * CGLArrIKernelRound;

//two-inputs
LISEM_API extern OpenCLProgram * CGLArrIKernelMin;
LISEM_API extern OpenCLProgram * CGLArrIKernelMax;
LISEM_API extern OpenCLProgram * CGLArrIKernelAdd;
LISEM_API extern OpenCLProgram * CGLArrIKernelMod;
LISEM_API extern OpenCLProgram * CGLArrIKernelSub;
LISEM_API extern OpenCLProgram * CGLArrIKernelMul;
LISEM_API extern OpenCLProgram * CGLArrIKernelDiv;
LISEM_API extern OpenCLProgram * CGLArrIKernelLogN;
LISEM_API extern OpenCLProgram * CGLArrIKernelPow;
LISEM_API extern OpenCLProgram * CGLArrIKernelEq;
LISEM_API extern OpenCLProgram * CGLArrIKernelGreater;
LISEM_API extern OpenCLProgram * CGLArrIKernelSmaller;
LISEM_API extern OpenCLProgram * CGLArrIKernelGreaterEq;
LISEM_API extern OpenCLProgram * CGLArrIKernelSmallerEq;
LISEM_API extern OpenCLProgram * CGLArrIFdim;
LISEM_API extern OpenCLProgram * CGLArrIHypot;
LISEM_API extern OpenCLProgram * CGLArrIKernelAnd;
LISEM_API extern OpenCLProgram * CGLArrIKernelOr;
LISEM_API extern OpenCLProgram * CGLArrIKernelXOr;
LISEM_API extern OpenCLProgram * CGLArrIKernelNot;
LISEM_API extern OpenCLProgram * CGLArrIKernelSign;

//one input
LISEM_API extern OpenCLProgram * CGLArrFKernelSqrt;
LISEM_API extern OpenCLProgram * CGLArrFKernelExp;
LISEM_API extern OpenCLProgram * CGLArrFKernelLog;
LISEM_API extern OpenCLProgram * CGLArrFKernelSin;
LISEM_API extern OpenCLProgram * CGLArrFKernelAsin;
LISEM_API extern OpenCLProgram * CGLArrFKernelCos;
LISEM_API extern OpenCLProgram * CGLArrFKernelAcos;
LISEM_API extern OpenCLProgram * CGLArrFKernelTan;
LISEM_API extern OpenCLProgram * CGLArrFKernelAtan;
LISEM_API extern OpenCLProgram * CGLArrFKernelSinh;
LISEM_API extern OpenCLProgram * CGLArrFKernelAsinh;
LISEM_API extern OpenCLProgram * CGLArrFKernelCosh;
LISEM_API extern OpenCLProgram * CGLArrFKernelAcosh;
LISEM_API extern OpenCLProgram * CGLArrFKernelTanh;
LISEM_API extern OpenCLProgram * CGLArrFKernelAtanh;
LISEM_API extern OpenCLProgram * CGLArrFKernelAbs;
LISEM_API extern OpenCLProgram * CGLArrFKernelFloor;
LISEM_API extern OpenCLProgram * CGLArrFKernelDecimal;
LISEM_API extern OpenCLProgram * CGLArrFKernelSmoothstep;
LISEM_API extern OpenCLProgram * CGLArrFKernelCbrt;
LISEM_API extern OpenCLProgram * CGLArrFKernelRand;
LISEM_API extern OpenCLProgram * CGLArrFKernelRound;

//two-inputs
LISEM_API extern OpenCLProgram * CGLArrFKernelMin;
LISEM_API extern OpenCLProgram * CGLArrFKernelMax;
LISEM_API extern OpenCLProgram * CGLArrFKernelAdd;
LISEM_API extern OpenCLProgram * CGLArrFKernelMod;
LISEM_API extern OpenCLProgram * CGLArrFKernelSub;
LISEM_API extern OpenCLProgram * CGLArrFKernelMul;
LISEM_API extern OpenCLProgram * CGLArrFKernelDiv;
LISEM_API extern OpenCLProgram * CGLArrFKernelLogN;
LISEM_API extern OpenCLProgram * CGLArrFKernelPow;
LISEM_API extern OpenCLProgram * CGLArrFKernelEq;
LISEM_API extern OpenCLProgram * CGLArrFKernelGreater;
LISEM_API extern OpenCLProgram * CGLArrFKernelSmaller;
LISEM_API extern OpenCLProgram * CGLArrFKernelGreaterEq;
LISEM_API extern OpenCLProgram * CGLArrFKernelSmallerEq;
LISEM_API extern OpenCLProgram * CGLArrFFdim;
LISEM_API extern OpenCLProgram * CGLArrFHypot;
LISEM_API extern OpenCLProgram * CGLArrFKernelAnd;
LISEM_API extern OpenCLProgram * CGLArrFKernelOr;
LISEM_API extern OpenCLProgram * CGLArrFKernelXOr;
LISEM_API extern OpenCLProgram * CGLArrFKernelNot;
LISEM_API extern OpenCLProgram * CGLArrFKernelSign;



//one input
LISEM_API extern OpenCLProgram * CGLKernelRedMin;
LISEM_API extern OpenCLProgram * CGLKernelRedMax;
LISEM_API extern OpenCLProgram * CGLKernelRedTotal;
LISEM_API extern OpenCLProgram * CGLKernelRedSum;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedMin;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedMax;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedTotal;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedSum;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedMin;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedMax;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedTotal;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedSum;
LISEM_API extern OpenCLProgram * CGLKernelRedRemMin;
LISEM_API extern OpenCLProgram * CGLKernelRedRemMax;
LISEM_API extern OpenCLProgram * CGLKernelRedRemTotal;
LISEM_API extern OpenCLProgram * CGLKernelRedRemSum;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedRemMin;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedRemMax;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedRemTotal;
LISEM_API extern OpenCLProgram * CGLArrIKernelRedRemSum;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedRemMin;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedRemMax;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedRemTotal;
LISEM_API extern OpenCLProgram * CGLArrFKernelRedRemSum;

inline static OpenCLProgram * GetCLProgramFromBasicReductionExpression(QString expression, bool isint, bool is_array, QString initial)
{
    QString typs = isint? QString("int"):QString("float");

    QString code  = "";
    /*if(is_array)
    {
        code +="__kernel void sph_function(__global const " +typs + " *input, __global " +typs + " *output, const int inputSize, __local " +typs + "* reductionSums) {";
        code +="const int globalID = get_global_id(0); \n";
        code +="const int localID = get_local_id(0); \n";
        code +="const int localSize = get_local_size(0); \n";
        code +="const int workgroupID = globalID / localSize; \n";

        code +="reductionSums[localID] = input[globalID]; \n";

        code +="for(int offset = localSize / 2; offset > 0; offset /= 2) { \n";
        code +="barrier(CLK_LOCAL_MEM_FENCE); \n";
        code +="if(localID < offset) { \n";
        code +=typs + " val1 = reductionSums[localID]; \n";
        code +=typs + " val2 = reductionSums[localID + offset]; \n";
        code +="reductionSums[localID] = "+ expression+ "; \n";
        code +="} \n";
        code +="} \n";

        code +="if(localID == 0) {	\n";
        code +="output[workgroupID] = reductionSums[0]; \n";
        code +="} \n";
        code +="} \n";

    }else
    {
        code +="__kernel void sph_function(int dim0, int dim1 , __read_only image2d_t input, __global float *output, const int inputSize, __local float * reductionSums) {";
        code +="const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n";
        code +="const int globalID = get_global_id(0); \n";
        code +="const int localID = get_local_id(0); \n";
        code +="const int localSize = get_local_size(0); \n";
        code +="const int workgroupID = globalID / localSize; \n";

        code += "int2 pixelcoord = (int2) (min(dim0-(int)(1),max((int)(0),(int)(globalID % dim1))), min(dim1-(int)(1),max((int)(0),(int)(globalID/dim1))));\n";
        code += "float valt = read_imagef(image1, sampler, pixelcoord).x;\n";

        code +="reductionSums[localID] = valt; \n";

        code +="for(int offset = localSize / 2; offset > 0; offset /= 2) { \n";
        code +="barrier(CLK_LOCAL_MEM_FENCE); \n";
        code +="if(localID < offset) { \n";
        code +="float val1 = reductionSums[localID]; \n";
        code +="float val2 = reductionSums[localID + offset]; \n";
        code +="reductionSums[localID] = "+ expression+ "; \n";
        code +="} \n";
        code +="} \n";

        code +="if(localID == 0) {	\n";
        code +="output[workgroupID] = reductionSums[0]; \n";
        code +="} \n";
        code +="} \n";

    }*/
    //highly optimized cl reduction stuff for group-sizes of 256
    //https://github.com/gante/OpenCL-Parallel-Reduction/blob/master/code/CR_version_8/device/kernel_8.cl
    if(is_array)
    {

        code +="__kernel __attribute__((reqd_work_group_size(256, 1, 1))) void sph_function( __global "+ typs + " *A, __global "+ typs+ " *B){";

        //Get the indexes of the local item
        code +=" unsigned int tid = get_local_id(0);";
        code +=" unsigned int wid = get_group_id(0);";
        code +=" unsigned int dim = get_local_size(0);";
        code +=" unsigned int i = wid*dim*2 + tid;";
        code +=" unsigned int gridSize = get_global_size(0)*2;";

         //Declares the shared memory
        code +=" __local " + typs + " sdata[256];";
        code +=" sdata[tid] = "+ initial + ";";

         // 2^3 external sums, or 2^2 * 2
        code +="for(int aux = 0; aux<4; aux++) {";
        code +="    " + typs + " temp1 = A[i];";
        code +="    " + typs + " temp2 = A[i+dim];";
        if(!isint)
        {
            code +="    temp1 = isnormal(temp1)? temp1 : "+ initial + ";";
            code +="    temp2 = isnormal(temp2)? temp2 : "+ initial + ";";

        }
        code +="    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","temp1") + ";";
        code +="    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","temp2") + ";";
        code +="    i += gridSize;";
        code +="}";

         //syncs the threads (to ensure the local memory is properly loaded)
        code +=" barrier(CLK_LOCAL_MEM_FENCE);";

         // the parallel reduction itself (workgroup = 2^7 -> 7 times)
        code +=" if (tid < 128){";
        code +="     sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 128]") + ";";
        code +="     barrier(CLK_LOCAL_MEM_FENCE);";

        code +="     if (tid < 64){";
        code +="        sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 64]") + ";";
        code +="        barrier(CLK_LOCAL_MEM_FENCE);";


        code +="        if (tid < 32){";
        code +="            sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 32]") + ";";
        code +="            barrier(CLK_LOCAL_MEM_FENCE);";

                    //if (tid < 16){
        code +="                sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 16]") + ";";
        code +="                barrier(CLK_LOCAL_MEM_FENCE);";

                        //if (tid < 8){
        code +="                    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 8]") + ";";
        code +="                    barrier(CLK_LOCAL_MEM_FENCE);";

                            //if (tid < 4){
        code +="                        sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 4]") + ";";
        code +="                        barrier(CLK_LOCAL_MEM_FENCE);";

                                //if (tid < 2){
        code +="                            sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 2]") + ";";
        code +="                            barrier(CLK_LOCAL_MEM_FENCE);";

        code +="                            if (tid < 1){";
                                         // write result for this block to global mem
        code +="                                B[wid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 1]") + ";";
        code +="                            }";
                                //}
                            //}
                        //}
                    //}
        code +="        }";
        code +="    }";
        code +=" }";


        code +="};";



    }else
    {
//__attribute__((reqd_work_group_size(256, 1, 1)))

        code +="__kernel __attribute__((reqd_work_group_size(256, 1, 1))) void sph_function(int dim0, int dim1,   __read_only image2d_t A, __global float *B){\n";
        code +="const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n";

        //Get the indexes of the local item
        code +=" unsigned int tid = get_local_id(0);\n";
        code +=" unsigned int wid = get_group_id(0);\n";
        code +=" unsigned int dim = get_local_size(0);\n";
        code +=" unsigned int i = wid*dim*2 + tid;\n";
        code +=" unsigned int gridSize = get_global_size(0)*2;\n";

         //Declares the shared memory
        code +=" __local float sdata[256];\n";
        code +=" sdata[tid] = "+ initial + ";\n";

         // 2^3 external sums, or 2^2 * 2
        code +="for(int aux = 0; aux<4; aux++) {\n";
        code += "int2 pixelcoord1 = (int2) (min(dim0-(int)(1),max((int)(0),(int)((i) % dim1))), min(dim1-(int)(1),max((int)(0),(int)((i)/dim1))));\n";
        code += "float valt1 = read_imagef(A, sampler, pixelcoord1).x;\n";
        code += "int2 pixelcoord2 = (int2) (min(dim0-(int)(1),max((int)(0),(int)((i+dim) % dim1))), min(dim1-(int)(1),max((int)(0),(int)((i+dim)/dim1))));\n";
        code += "float valt2 = read_imagef(A, sampler, pixelcoord2).x;\n";

        code +="    valt1 = isnormal(valt1)? valt1 : "+ initial + ";";
        code +="    valt2 = isnormal(valt2)? valt2 : "+ initial + ";";

        code +="    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","valt1") + ";\n";
        code +="    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","valt2") + ";\n";
        code +="    i += gridSize;\n";
        code +="}\n";

         //syncs the threads (to ensure the local memory is properly loaded)
        code +=" barrier(CLK_LOCAL_MEM_FENCE);\n";

         // the parallel reduction itself (workgroup = 2^7 -> 7 times)
        code +=" if (tid < 128){\n";
        code +="     sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 128]") + ";\n";
        code +="     barrier(CLK_LOCAL_MEM_FENCE);";

        code +="     if (tid < 64){\n";
        code +="        sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 64]") + ";\n";
        code +="        barrier(CLK_LOCAL_MEM_FENCE);\n";


        code +="        if (tid < 32){";
        code +="            sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 32]") + ";";
        code +="            barrier(CLK_LOCAL_MEM_FENCE);";

                    //if (tid < 16){
        code +="                sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 16]") + ";";
        code +="                barrier(CLK_LOCAL_MEM_FENCE);";

                        //if (tid < 8){
        code +="                    sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 8]") + ";";
        code +="                    barrier(CLK_LOCAL_MEM_FENCE);";

                            //if (tid < 4){
        code +="                        sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 4]") + ";";
        code +="                        barrier(CLK_LOCAL_MEM_FENCE);";

                                //if (tid < 2){
        code +="                            sdata[tid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 2]") + ";";
        code +="                            barrier(CLK_LOCAL_MEM_FENCE);";

        code +="                            if (tid < 1){";
                                         // write result for this block to global mem
        code +="                                B[wid] = "+ QString(expression).replace("val1","sdata[tid]").replace("val2","sdata[tid + 1]") + ";";
        code +="                            }";
                                //}
                            //}
                        //}
                    //}
        code +="        }";
        code +="    }";
        code +=" }";


        code +="};";


    }


    OpenCLProgram * p = new OpenCLProgram();
    p->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,code.toStdString(),"sph_function");

    CGLGPUKernels.append(p);

    return p;

}

inline static OpenCLProgram * GetCLProgramFromBasicReductionRemExpression(QString expression, bool isint, bool is_array, QString initial)
{
    QString typs = isint? QString("int"):QString("float");

    QString code  = "";
    if(is_array)
    {
        code +="__kernel void sph_function(__global const " +typs + " *input, __global " +typs + " *output, const int inputSize, const int inOffset,const int outputOffset) {";
        code +="const int globalID = get_global_id(0); \n";
        code +="const int localID = get_local_id(0); \n";
        code +="const int localSize = get_local_size(0); \n";
        code +="const int workgroupID = globalID / localSize; \n";
        code += typs + " val1 = "+ initial + "; \n";
        code +="for(int i = inOffset; i < inputSize; i++) { \n";
        code += typs + " val2 = input[i]; \n";
        if(!isint)
        {
            code += "val2 = isnormal(val2)? val2 : " + initial + ";";
        }
        code +="val1 = "+ expression+ "; \n";
        code +="} \n";
        code +="output[outputOffset] = val1; \n";
        code +="} \n";

    }else
    {
        code +="__kernel void sph_function(int dim0, int dim1, __read_only image2d_t input, __global float *output, const int inputSize, const int inOffset,const int outputOffset) {";
        code +="const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n";
        code +="const int globalID = get_global_id(0); \n";
        code +="const int localID = get_local_id(0); \n";
        code +="const int localSize = get_local_size(0); \n";
        code +="const int workgroupID = globalID / localSize; \n";
        code +="float val1 = "+ initial + "; \n";
        code +="for(int i = inOffset; i < inputSize; i++) { \n";
        code += "int2 pixelcoord = (int2) (min(dim0-(int)(1),max((int)(0),(int)(i % dim1))), min(dim1-(int)(1),max((int)(0),(int)(i/dim1))));\n";
        code += "float val2 = read_imagef(input, sampler, pixelcoord).x;\n";
        //if(!isint)
        {
            code += "val2 = isnormal(val2)? val2 : " + initial + ";";
        }
        code +="val1 = "+ expression+ "; \n";
        code +="} \n";
        code +="output[outputOffset] = val1; \n";
        code +="} \n";
    }

    OpenCLProgram * p = new OpenCLProgram();
    p->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,code.toStdString(),"sph_function");

    CGLGPUKernels.append(p);

    return p;

}


inline static OpenCLProgram * GetCLProgramFromBasicExpression(QString expression, bool two_op, bool map, bool isint, bool second_is_nonarray = false, bool reverse_args = false)
{

    //set up the surrounding kernel function

    QString code  = "";
    if(map)
    {
        code += "kernel void sph_function( int dim0, int dim1, \n";
    }else
    {
        code += "kernel void sph_function( int dim0, \n";
    }
    if(map)
    {
        code += "__read_only image2d_t image1";
        if(two_op)
        {
            if(!second_is_nonarray)
            {
                code += ", __read_only image2d_t image2";
            }else
            {
                code += ", float image2";
            }
        }
        code += ", __write_only image2d_t imager";
    }else
    {
        if(isint)
        {
            code += "__global int* array1";

            if(two_op)
            {
                if(!second_is_nonarray)
                {

                    code += ",__global int* array2";
                }else
                {

                    code += ",int array2";
                }
            }
            code += ",__global int* arrayr";
        }else
        {
            code += "__global float* array1";

            if(two_op)
            {
                if(!second_is_nonarray)
                {
                    code += ",__global float* array2";
                }else
                {

                    code += ",float array2";
                }
            }
            code += ",__global float* arrayr";
        }
    }

    code += "){\n";

    code += "const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;\n";

    QString val1name = "val1";
    QString val2name = "val2";
    if(reverse_args)
    {
        val1name = "val2";
        val2name = "val1";
    }
    //if we are using maps, we have to define the read_only and write_only output
    if(map)
    {
        if(two_op)
        {
            //get first variable
            code += "int2 pixelcoord = (int2) (min(dim0-(int)(1),max((int)(0),(int)(get_global_id(0)))), min(dim1-(int)(1),max((int)(0),(int)(get_global_id(1)))));\n";
            code += "float " + val1name + " = read_imagef(image1, sampler, pixelcoord).x;\n";
            code += "float " + val2name + " = " + (second_is_nonarray? "image2;\n":"read_imagef(image2, sampler, pixelcoord).x;\n");
            code += "write_imagef(imager,pixelcoord," + expression + ");\n";
        }else
        {
            //get first variable
            code += "int2 pixelcoord = (int2) (min(dim0-(int)(1),max((int)(0),(int)(get_global_id(0)))), min(dim1-(int)(1),max((int)(0),(int)(get_global_id(1)))));\n";
            code += "float val1 = read_imagef(image1, sampler, pixelcoord).x;\n";
            code += "write_imagef(imager,pixelcoord," + expression + ");\n";
        }
    }
    //for vectors, just assign things directly
    if(!map)
    {
        if(isint)
        {
            if(two_op)
            {
                //get first variable
                code += "int pixelcoord = max((int)(0),min(dim0-(int)(1),(int)(get_global_id(0))));\n";
                code += "int " + val1name + " = array1[pixelcoord];\n";
                code += "int " + val2name + " = array2" + (second_is_nonarray? ";\n":"[pixelcoord];\n");
                code += "arrayr[pixelcoord] = "+ expression + ";\n";
            }else
            {
                //get first variable
                code += "int pixelcoord = max((int)(0),min(dim0-(int)(1),(int)(get_global_id(0))));\n";
                code += "int val1 = array1[pixelcoord];\n";
                code += "arrayr[pixelcoord] = "+ expression + ";\n";
            }
        }else
        {
            if(two_op)
            {
                //get first variable
                code += "int pixelcoord = max((int)(0),min(dim0-(int)(1),(int)(get_global_id(0))));\n";
                code += "float " + val1name + " = array1[pixelcoord];\n";
                code += "float " + val2name + " = array2" + (second_is_nonarray? ";\n":"[pixelcoord];\n");
                code += "arrayr[pixelcoord] = "+ expression + ";\n";
            }else
            {
                //get first variable
                code += "int pixelcoord = max((int)(0),min(dim0-(int)(1),(int)(get_global_id(0))));\n";
                code += "float val1 = array1[pixelcoord];\n";
                code += "arrayr[pixelcoord] = "+ expression + ";\n";
            }
        }
    }

    code += "}";

    OpenCLProgram * p = new OpenCLProgram();
    p->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,code.toStdString(),"sph_function");

    CGLGPUKernels.append(p);

    return p;
}

inline static void LoadDefaultGPUModels()
{
    CGLKernelMutex.lock();

    LISEMS_STATUS("Compiling default GPU functions (Only occurs on first usage)");
    CGLKernelSqrt =  GetCLProgramFromBasicExpression("sqrt(val1)",false,true,false);
    CGLKernelExp =  GetCLProgramFromBasicExpression("exp(val1)",false,true,false);
    CGLKernelLog =  GetCLProgramFromBasicExpression("log(val1)",false,true,false);
    CGLKernelSin =  GetCLProgramFromBasicExpression("sin(val1)",false,true,false);
    CGLKernelAsin =  GetCLProgramFromBasicExpression("asin(val1)",false,true,false);
    CGLKernelCos =  GetCLProgramFromBasicExpression("cos(val1)",false,true,false);
    CGLKernelAcos =  GetCLProgramFromBasicExpression("acos(val1)",false,true,false);
    CGLKernelTan =  GetCLProgramFromBasicExpression("tan(val1)",false,true,false);
    CGLKernelAtan =  GetCLProgramFromBasicExpression("atan(val1)",false,true,false);
    CGLKernelSinh =  GetCLProgramFromBasicExpression("sinh(val1)",false,true,false);
    CGLKernelAsinh =  GetCLProgramFromBasicExpression("asinh(val1)",false,true,false);
    CGLKernelCosh =  GetCLProgramFromBasicExpression("cosh(val1)",false,true,false);
    CGLKernelAcosh =  GetCLProgramFromBasicExpression("acosh(val1)",false,true,false);
    CGLKernelTanh =  GetCLProgramFromBasicExpression("tanh(val1)",false,true,false);
    CGLKernelAtanh =  GetCLProgramFromBasicExpression("atanh(val1)",false,true,false);
    CGLKernelAbs =  GetCLProgramFromBasicExpression("(val1 > 0.0f? val1 : -val1)",false,true,false);
    CGLKernelFloor =  GetCLProgramFromBasicExpression("floor(val1)",false,true,false);
    CGLKernelDecimal =  GetCLProgramFromBasicExpression(" fmin( val1 - floor(val1), 0x1.fffffep-1f )",false,true,false);
    CGLKernelSmoothstep =  GetCLProgramFromBasicExpression("smoothstep(0.0f,1.0f,val1)",false,true,false);
    CGLKernelCbrt =  GetCLProgramFromBasicExpression("cbrt(val1)",false,true,false);
    CGLKernelRand =  GetCLProgramFromBasicExpression("(get_global_id(1) ^ (get_global_id(1) >> 19) ^ ((get_global_id(0) ^(UINT_MAX<<11)) ^((get_global_id(0) ^(UINT_MAX<<11))>>8)))/UINT_MAX",false,true,false);
    CGLKernelRound =  GetCLProgramFromBasicExpression("round(val1)",false,true,false);

    CGLKernelMin = GetCLProgramFromBasicExpression("min(val1,val2)",true,true,false,false,false);
    CGLKernelMax = GetCLProgramFromBasicExpression("max(val1,val2)",true,true,false,false,false);
    CGLKernelAdd = GetCLProgramFromBasicExpression("(val1 + val2)",true,true,false,false,false);
    CGLKernelMod = GetCLProgramFromBasicExpression("(fmod(val1,val2))",true,true,false,false,false);
    CGLKernelSub = GetCLProgramFromBasicExpression("(val1 - val2)",true,true,false,false,false);
    CGLKernelMul = GetCLProgramFromBasicExpression("(val1 * val2)",true,true,false,false,false);
    CGLKernelDiv = GetCLProgramFromBasicExpression("(val1 / val2)",true,true,false,false,false);
    CGLKernelLogN = GetCLProgramFromBasicExpression("log(val2)/log(val1)",true,true,false,false,false);
    CGLKernelPow = GetCLProgramFromBasicExpression("pow(val1,val2)",true,true,false,false,false);
    CGLKernelEq = GetCLProgramFromBasicExpression("(val1 == val2)? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelGreater = GetCLProgramFromBasicExpression("(val1 > val2)? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelSmaller = GetCLProgramFromBasicExpression("(val1 < val2)? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelGreaterEq = GetCLProgramFromBasicExpression("(val1 >= val2)? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelSmallerEq = GetCLProgramFromBasicExpression("(val1 <= val2)? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelFdim = GetCLProgramFromBasicExpression("fdim(val1,val2)",true,true,false,false,false);
    CGLKernelHypoth = GetCLProgramFromBasicExpression("hypot(val1,val2)",true,true,false,false,false);
    CGLKernelAnd = GetCLProgramFromBasicExpression("((val1 >= 0.5f) && (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelOr = GetCLProgramFromBasicExpression("((val1 >= 0.5f) || (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,false,false);
    CGLKernelXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,false);

    CGLKernelfMin = GetCLProgramFromBasicExpression("min(val1,val2)",true,true,false,true,false);
    CGLKernelfMax = GetCLProgramFromBasicExpression("max(val1,val2)",true,true,false,true,false);
    CGLKernelfAdd = GetCLProgramFromBasicExpression("(val1 + val2)",true,true,false,true,false);
    CGLKernelfMod = GetCLProgramFromBasicExpression("(fmod(val1,val2))",true,true,false,true,false);
    CGLKernelfSub = GetCLProgramFromBasicExpression("(val1 - val2)",true,true,false,true,false);
    CGLKernelfMul = GetCLProgramFromBasicExpression("(val1 * val2)",true,true,false,true,false);
    CGLKernelfDiv = GetCLProgramFromBasicExpression("(val1 / val2)",true,true,false,true,false);
    CGLKernelfLogN = GetCLProgramFromBasicExpression("log(val2)/log(val1)",true,true,false,true,false);
    CGLKernelfPow = GetCLProgramFromBasicExpression("pow(val1,val2)",true,true,false,true,false);
    CGLKernelfEq = GetCLProgramFromBasicExpression("(val1 == val2)? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfGreater = GetCLProgramFromBasicExpression("(val1 > val2)? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfSmaller = GetCLProgramFromBasicExpression("(val1 < val2)? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfGreaterEq = GetCLProgramFromBasicExpression("(val1 >= val2)? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfSmallerEq = GetCLProgramFromBasicExpression("(val1 <= val2)? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfFdim = GetCLProgramFromBasicExpression("fdim(val1,val2)",true,true,false,true,false);
    CGLKernelfHypoth = GetCLProgramFromBasicExpression("hypot(val1,val2)",true,true,false,true,false);
    CGLKernelfAnd = GetCLProgramFromBasicExpression("((val1 >= 0.5f) && (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfOr = GetCLProgramFromBasicExpression("((val1 >= 0.5f) || (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,true,false);
    CGLKernelfXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,false);


    CGLKernelfrMin = GetCLProgramFromBasicExpression("min(val1,val2)",true,true,false,true,true);
    CGLKernelfrMax = GetCLProgramFromBasicExpression("max(val1,val2)",true,true,false,true,true);
    CGLKernelfrAdd = GetCLProgramFromBasicExpression("(val1 + val2)",true,true,false,true,true);
    CGLKernelfrMod = GetCLProgramFromBasicExpression("(fmod(val1,val2))",true,true,false,true,true);
    CGLKernelfrSub = GetCLProgramFromBasicExpression("(val1 - val2)",true,true,false,true,true);
    CGLKernelfrMul = GetCLProgramFromBasicExpression("(val1 * val2)",true,true,false,true,true);
    CGLKernelfrDiv = GetCLProgramFromBasicExpression("(val1 / val2)",true,true,false,true,true);
    CGLKernelfrLogN = GetCLProgramFromBasicExpression("log(val2)/log(val1)",true,true,false,true,true);
    CGLKernelfrPow = GetCLProgramFromBasicExpression("pow(val1,val2)",true,true,false,true,true);
    CGLKernelfrEq = GetCLProgramFromBasicExpression("(val1 == val2)? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrGreater = GetCLProgramFromBasicExpression("(val1 > val2)? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrSmaller = GetCLProgramFromBasicExpression("(val1 < val2)? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrGreaterEq = GetCLProgramFromBasicExpression("(val1 >= val2)? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrSmallerEq = GetCLProgramFromBasicExpression("(val1 <= val2)? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrFdim = GetCLProgramFromBasicExpression("fdim(val1,val2)",true,true,false,true,true);
    CGLKernelfrHypoth = GetCLProgramFromBasicExpression("hypot(val1,val2)",true,true,false,true,true);
    CGLKernelfrAnd = GetCLProgramFromBasicExpression("((val1 >= 0.5f) && (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrOr = GetCLProgramFromBasicExpression("((val1 >= 0.5f) || (val2 >= 0.5f))? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,true);

    CGLKernelfrXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,true);
    CGLKernelfrXOr = GetCLProgramFromBasicExpression("(((val1 >= 0.5f) || (val2 >= 0.5f)) && !((val1 >= 0.5f) && (val2 >= 0.5f)))? 1.0f:0.0f",true,true,false,true,true);


    CGLKernelRedMin = GetCLProgramFromBasicReductionExpression(" (min(val1,val2)) ",false,false,"1e31f");
    CGLKernelRedMax = GetCLProgramFromBasicReductionExpression(" (max(val1,val2)) ",false,false,"-1e31f");
    CGLKernelRedTotal = GetCLProgramFromBasicReductionExpression(" (val1 - val2) ",false,false,"0.0");
    CGLKernelRedSum = GetCLProgramFromBasicReductionExpression(" (val1 + val2) ",false,false,"0.0");
    CGLArrIKernelRedMin = GetCLProgramFromBasicReductionExpression(" (min(val1,val2)) ",true,true,"2147483647");
    CGLArrIKernelRedMax = GetCLProgramFromBasicReductionExpression(" (max(val1,val2)) ",true,true,"-2147483648");
    CGLArrIKernelRedTotal = GetCLProgramFromBasicReductionExpression(" (val1 - val2) ",true,true, "0");
    CGLArrIKernelRedSum = GetCLProgramFromBasicReductionExpression(" (val1 + val2) ",true,true, "0");
    CGLArrFKernelRedMin = GetCLProgramFromBasicReductionExpression(" (min(val1,val2)) ",false,true,"1e31f");
    CGLArrFKernelRedMax = GetCLProgramFromBasicReductionExpression(" (max(val1,val2)) ",false,true,"-1e31f");
    CGLArrFKernelRedTotal = GetCLProgramFromBasicReductionExpression(" (val1 - val2) ",false,true,"0.0");
    CGLArrFKernelRedSum = GetCLProgramFromBasicReductionRemExpression(" (val1 + val2) ",false,true,"0.0");
    CGLKernelRedRemMin = GetCLProgramFromBasicReductionRemExpression(" (min(val1,val2)) ",false,false,"1e31f");
    CGLKernelRedRemMax = GetCLProgramFromBasicReductionRemExpression(" (max(val1,val2)) ",false,false,"-1e31f");
    CGLKernelRedRemTotal = GetCLProgramFromBasicReductionRemExpression(" (val1 - val2) ",false,false,"0.0");
    CGLKernelRedRemSum = GetCLProgramFromBasicReductionRemExpression(" (val1 + val2) ",false,false,"0.0");
    CGLArrIKernelRedRemMin = GetCLProgramFromBasicReductionRemExpression(" (min(val1,val2)) ",true,true,"2147483647");
    CGLArrIKernelRedRemMax = GetCLProgramFromBasicReductionRemExpression(" (max(val1,val2)) ",true,true,"-2147483648");
    CGLArrIKernelRedRemTotal = GetCLProgramFromBasicReductionRemExpression(" (val1 - val2) ",true,true, "0");
    CGLArrIKernelRedRemSum = GetCLProgramFromBasicReductionRemExpression(" (val1 + val2) ",true,true, "0");
    CGLArrFKernelRedRemMin = GetCLProgramFromBasicReductionRemExpression(" (min(val1,val2)) ",false,true,"1e31f");
    CGLArrFKernelRedRemMax = GetCLProgramFromBasicReductionRemExpression(" (max(val1,val2)) ",false,true,"-1e31f");
    CGLArrFKernelRedRemTotal = GetCLProgramFromBasicReductionRemExpression(" (val1 - val2) ",false,true,"0.0");
    CGLArrFKernelRedRemSum = GetCLProgramFromBasicReductionRemExpression(" (val1 + val2) ",false,true,"0.0");

    //ulong seed = randoms + globalID;
    //seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    //uint result = seed >> 16;
    CGLHasGPUKernels = true;

    CGLKernelMutex.unlock();
}

inline static void DestroyDefaultGPUModels()
{
    CGLKernelMutex.lock();
    for(int i = 0; i < CGLGPUKernels.length(); i++)
    {
        CGLGPUKernels.at(i)->Destroy();
        delete CGLGPUKernels.at(i);
    }
    CGLKernelMutex.unlock();
}

inline static bool HasDefaultGPUModels()
{
    CGLKernelMutex.lock();
    bool has = CGLHasGPUKernels;
    CGLKernelMutex.unlock();

    return has;
}





class AS_GPUFArray
{
private:

    OpenGLCLDataBuffer<float> *m_GPUData;

    AS_GPUFArray *    AS_Assign            (AS_GPUFArray*);
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();



};

class AS_GPUIArray
{
private:
    OpenGLCLDataBuffer<int> *m_GPUData;


    AS_GPUIArray *    AS_Assign            (AS_GPUIArray*);
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();



};

//Angelscript related functionality

inline void AS_GPUIArray::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void AS_GPUIArray::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {

        m_GPUData->DestroyBufferObject();
        delete this;

    }

}

inline AS_GPUIArray * AS_GPUIArray::AS_Assign(AS_GPUIArray * gpum)
{
    if(gpum->m_GPUData == nullptr)
    {
        LISEMS_ERROR("Can not use uninitialized GPU array");
        throw 1;
    }
    if(m_GPUData == nullptr)
    {



    }



    return this;
}

inline static AS_GPUIArray * GPUIArrayConstructor()
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    AS_GPUIArray * m = new AS_GPUIArray();


    return m;
}


class AS_GPUMap
{

private:


public:
    OpenCLTexture * m_GPUData = nullptr;

    double dx = 1.0;
    double dy = 1.0;
    GeoProjection projection;
    QString _projection;
    double ulx = 1.0;
    double uly = 1.0;

    inline void GeoPropsFromMap(AS_GPUMap * m)
    {
        dx =m->dx;
        dy =m->dy;
        projection =m->projection;
        _projection = m->_projection;
        ulx = m->ulx;
        uly = m->uly;
    }


    inline void GeoPropsFromMap(cTMap * m)
    {
        dx =m->cellSizeX();
        dy =m->cellSizeY();
        projection = m->GetProjection();
        _projection = m->projection();
        ulx = m->west();
        uly = m->north();
    }

    inline void GeoPropsToMap(cTMap * m)
    {
        m->SetProjection(projection);
        m->setcellSizeX(dx);
        m->setcellSizeY(dy);
        m->setprojectionstring(_projection);
        m->setnorth(uly);
        m->setwest(ulx);
    }

    AS_GPUMap * GetCopy0();
    void CopyFrom(AS_GPUMap * gpum);

    AS_GPUMap *    AS_Assign            (AS_GPUMap*);
    AS_GPUMap *    AS_Assign            (cTMap*);
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();

};

//Angelscript related functionality

inline void AS_GPUMap::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void AS_GPUMap::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        std::cout << "remove gpu map"<<std::endl;
        if(m_GPUData != nullptr)
        {
            std::cout << "destroy gpu data " << std::endl;
            m_GPUData->Destroy();
            delete m_GPUData;
            m_GPUData = nullptr;
        }
        delete this;

    }

}

inline AS_GPUMap * AS_GPUMap::AS_Assign(cTMap * m)
{
    std::cout << " new gpu map cpuassign" <<std::endl;
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    if(m_GPUData == nullptr)
    {
        m_GPUData = new OpenCLTexture();
        m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m->nrCols(),m->nrRows(),0.0);
    }else
    {
        if(!((m_GPUData->m_dims[0] == m->nrCols()) &&(m_GPUData->m_dims[1] == m->nrRows())))
        {
            m_GPUData->Destroy();
            delete m_GPUData;
            m_GPUData = nullptr;

            m_GPUData = new OpenCLTexture();
            m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m->nrCols(),m->nrRows(),0.0);
        }
    }


    GeoPropsFromMap(m);

    cl::CommandQueue &q = CGlobalGLCLManager->q;
    cl::Event ev;

    cl::size_t<3> src_origin;
    src_origin[0] = 0;
    src_origin[1] = 0;
    src_origin[2] = 0;

    cl::size_t<3> dst_origin ;
    dst_origin[0] = 0;
    dst_origin[1] = 0;
    dst_origin[2] = 0;

    cl::size_t<3> region ;
    region[0] = m_GPUData->m_dims[0];
    region[1] = m_GPUData->m_dims[1];
    region[2] = 1;

    try
    {

        cl::Event ev;
        CGlobalGLCLManager->q.enqueueWriteImage(m_GPUData->m_texcl,true,src_origin,region,0,0,&(m->data[0][0]),nullptr,&ev);

        ev.wait();
        cl_int ret = CGlobalGLCLManager->q.finish();

    }catch(cl::Error & e)
    {
        std::cout << "cl error " << e.err() << " " << e.what() << std::endl;
        LISEMS_ERROR("Error during GPU copy : " + QString::number(e.err()) + " : " + QString(e.what()));
        throw 1;
    }


    return this;
}

inline AS_GPUMap * AS_GPUMap::GetCopy0()
{

    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    AS_GPUMap * ret = new AS_GPUMap();

    if(ret->m_GPUData == nullptr)
    {

        std::cout << "new gpu map (copy)" << std::endl;
        ret->m_GPUData = new OpenCLTexture();
        ret->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m_GPUData->m_dims[0],m_GPUData->m_dims[1],0.0);
    }else
    {

        if(!((ret->m_GPUData->m_dims[0] == m_GPUData->m_dims[0]) &&(ret->m_GPUData->m_dims[1] == m_GPUData->m_dims[1])))
        {

            ret->m_GPUData->Destroy();
            delete ret->m_GPUData;
            ret->m_GPUData = nullptr;

            ret->m_GPUData = new OpenCLTexture();
            ret->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m_GPUData->m_dims[0],m_GPUData->m_dims[1],0.0);
        }
    }

    ret->GeoPropsFromMap(this);

    //no copy needed;

    return ret;
}

inline void AS_GPUMap::CopyFrom(AS_GPUMap * gpum)
{
    cl::CommandQueue &q = CGlobalGLCLManager->q;
    cl::Event ev;

    cl::size_t<3> src_origin;
    src_origin[0] = 0;
    src_origin[1] = 0;
    src_origin[2] = 0;

    cl::size_t<3> dst_origin ;
    dst_origin[0] = 0;
    dst_origin[1] = 0;
    dst_origin[2] = 0;

    cl::size_t<3> region ;
    region[0] = m_GPUData->m_dims[0];
    region[1] = m_GPUData->m_dims[1];
    region[2] = 1;


    try
    {

        q.enqueueCopyImage(
        gpum->m_GPUData->m_texcl,
        m_GPUData->m_texcl,
        src_origin,
        dst_origin,
        region,
        NULL,
        NULL);


        int res = q.finish();

    }catch(cl::Error & e)
    {
        std::cout << "cl error " << e.err() << " " << e.what() << std::endl;
        LISEMS_ERROR("Error during GPU copy : " + QString::number(e.err()) + " : " + QString(e.what()));
        throw 1;
    }



}

inline AS_GPUMap * AS_GPUMap::AS_Assign(AS_GPUMap * gpum)
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    if(gpum->m_GPUData == nullptr)
    {
        LISEMS_ERROR("Can not use uninitialized GPU map");
        throw 1;
    }
    if(m_GPUData == nullptr)
    {

        std::cout << "New gpu map (GPUAssign)" <<std::endl;

        m_GPUData = new OpenCLTexture();
        m_GPUData->Create2DRF32(CGlobalGLCLManager->context,gpum->m_GPUData->m_dims[0],gpum->m_GPUData->m_dims[1],0.0);
    }else
    {

        if(!((gpum->m_GPUData->m_dims[0] == m_GPUData->m_dims[0]) &&(gpum->m_GPUData->m_dims[1] == m_GPUData->m_dims[1])))
        {
            std::cout << "New gpu map (GPUAssign wrong size) " << gpum->m_GPUData->m_dims[0] << " " <<  m_GPUData->m_dims[0] << " " << gpum->m_GPUData->m_dims[1]<< " " <<  m_GPUData->m_dims[1] << std::endl;

            m_GPUData->Destroy();
            delete m_GPUData;
            m_GPUData = nullptr;

            m_GPUData = new OpenCLTexture();
            m_GPUData->Create2DRF32(CGlobalGLCLManager->context,gpum->m_GPUData->m_dims[0],gpum->m_GPUData->m_dims[1],0.0);
        }
    }

    GeoPropsFromMap(gpum);

    cl::CommandQueue &q = CGlobalGLCLManager->q;
    cl::Event ev;

    cl::size_t<3> src_origin;
    src_origin[0] = 0;
    src_origin[1] = 0;
    src_origin[2] = 0;

    cl::size_t<3> dst_origin ;
    dst_origin[0] = 0;
    dst_origin[1] = 0;
    dst_origin[2] = 0;

    cl::size_t<3> region ;
    region[0] = m_GPUData->m_dims[0];
    region[1] = m_GPUData->m_dims[1];
    region[2] = 1;


    try
    {

        q.enqueueCopyImage(
        gpum->m_GPUData->m_texcl,
        m_GPUData->m_texcl,
        src_origin,
        dst_origin,
        region,
        NULL,
        NULL);


        int res = q.finish();

    }catch(cl::Error & e)
    {
        std::cout << "cl error " << e.err() << " " << e.what() << std::endl;
        LISEMS_ERROR("Error during GPU copy : " + QString::number(e.err()) + " : " + QString(e.what()));
        throw 1;
    }


    return this;
}

inline static AS_GPUMap * GPUMapConstructor()
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    AS_GPUMap * m = new AS_GPUMap();
    return m;
}

inline static void GPUMapDestructor(AS_GPUMap * m)
{

    if(m->m_GPUData != nullptr)
    {
        m->m_GPUData->Destroy();
        delete m->m_GPUData;
        m->m_GPUData = nullptr;
    }


}
inline static void AS_INT_GLSTART()
{
    CGlobalGLCLManager->m_GLOutputUIMutex.lock();
    CGlobalGLCLManager->m_GLMutex.lock();
    glfwMakeContextCurrent(CGlobalGLCLManager->window);
}

inline static void AS_INT_GLEND()
{
    glfwMakeContextCurrent(NULL);
    CGlobalGLCLManager->m_GLOutputUIMutex.unlock();
    CGlobalGLCLManager->m_GLMutex.unlock();
}

inline static AS_GPUMap * GPUMapConstructor(cTMap * m)
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }


    AS_GPUMap * mg = new AS_GPUMap();
    mg->m_GPUData = new OpenCLTexture();
    mg->m_GPUData->Create2DFromMap(CGlobalGLCLManager->context,&(m->data));


    mg->GeoPropsFromMap(m);

    return mg;
}
inline static AS_GPUMap * GPUMapConstructor(int rows, int cols, float val)
{
    AS_GPUMap * m = new AS_GPUMap();
    m->m_GPUData = new OpenCLTexture();
    int err = m->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,cols,rows,val);

    if(err > 0)
    {
        LISEMS_ERROR("Could not create GPU Map");
        throw 1;
    }
    return m;
}

inline static AS_GPUMap * CPUToGPU(cTMap * m)
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    AS_GPUMap * mg = new AS_GPUMap();
    mg->m_GPUData = new OpenCLTexture();
    mg->GeoPropsFromMap(m);
    int err = mg->m_GPUData->Create2DFromMap(CGlobalGLCLManager->context,&(m->data));
    if(err > 0)
    {
        LISEMS_ERROR("Could not create GPU Map");
        throw 1;
    }
    return mg;
}


inline static cTMap * GPUToCPU(AS_GPUMap *gpum)
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }
    if(gpum->m_GPUData == nullptr)
    {
        LISEMS_ERROR("Uninitialized gpu map can not be used");
        throw 1;
    }
    cTMap * m;
    MaskedRaster<float> data = MaskedRaster<float>(gpum->m_GPUData->m_dims[1],gpum->m_GPUData->m_dims[0],0.0,0.0,1.0,1.0);
    m = new cTMap(std::move(data),"","",false);

    gpum->GeoPropsToMap(m);

    cl::size_t<3> src_origin;
    src_origin[0] = 0;
    src_origin[1] = 0;
    src_origin[2] = 0;

    cl::size_t<3> dst_origin ;
    dst_origin[0] = 0;
    dst_origin[1] = 0;
    dst_origin[2] = 0;

    cl::size_t<3> region ;
    region[0] = gpum->m_GPUData->m_dims[0];
    region[1] = gpum->m_GPUData->m_dims[1];
    region[2] = 1;

    cl::Event ev;
    CGlobalGLCLManager->q.enqueueReadImage(gpum->m_GPUData->m_texcl,true,src_origin,region,0,0,&(m->data[0][0]),nullptr,&ev);

    ev.wait();
    cl_int ret = CGlobalGLCLManager->q.finish();

    return m;
}

inline static AS_GPUMap * ApplyGPUKernelSingleMap(OpenCLProgram * p,AS_GPUMap * m)
{
    AS_GPUMap * r = new AS_GPUMap();
    r->m_GPUData = new OpenCLTexture();
    r->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1],0.0);

    //run kernel
    ModelTexture t1;
    t1.m_IsCPU = false;
    t1.m_IsCLOnly = true;
    t1.m_TextureGPUCL = m->m_GPUData;

    ModelTexture t2;
    t2.m_IsCPU = false;
    t2.m_IsCLOnly = true;
    t2.m_TextureGPUCL = r->m_GPUData;

    p->PlaceArgument(0,(int)(m->m_GPUData->m_dims[0]));
    p->PlaceArgument(1,(int)(m->m_GPUData->m_dims[1]));

    p->PlaceArgument(2,&t1);
    p->PlaceArgument(3,&t2);

    p->SetRunDims(m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1]);
    p->Run(CGlobalGLCLManager->context,CGlobalGLCLManager->q);

    r->GeoPropsFromMap(m);
    return r;

}

inline static AS_GPUMap * ApplyGPUKernelDoubleMap(OpenCLProgram * p,AS_GPUMap * m,AS_GPUMap * m2, bool assign_back = false)
{
    if(m->m_GPUData == nullptr || m2->m_GPUData == nullptr)
    {
        LISEMS_ERROR("Can not do map calculation with uninitialized map");
        throw 1;
    }
    if(!((m->m_GPUData->m_dims[0] == m2->m_GPUData->m_dims[0])&&(m->m_GPUData->m_dims[1] == m2->m_GPUData->m_dims[1])))
    {
        LISEMS_ERROR("GPUMaps must be of identical size");
        throw 1;
    }
    AS_GPUMap * r = new AS_GPUMap();
    r->m_GPUData = new OpenCLTexture();
    r->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1],0.0);

    //run kernel
    ModelTexture t1;
    t1.m_IsCPU = false;
    t1.m_IsCLOnly = true;
    t1.m_TextureGPUCL = m->m_GPUData;

    ModelTexture t2;
    t2.m_IsCPU = false;
    t2.m_IsCLOnly = true;
    t2.m_TextureGPUCL = m2->m_GPUData;

    ModelTexture t3;
    t3.m_IsCPU = false;
    t3.m_IsCLOnly = true;
    t3.m_TextureGPUCL = r->m_GPUData;


    p->PlaceArgument(0,(int)(m->m_GPUData->m_dims[0]));
    p->PlaceArgument(1,(int)(m->m_GPUData->m_dims[1]));

    p->PlaceArgument(2,&t1);
    p->PlaceArgument(3,&t2);
    p->PlaceArgument(4,&t3);

    p->SetRunDims(m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1]);
    p->Run(CGlobalGLCLManager->context,CGlobalGLCLManager->q);

    if(assign_back)
    {
        m->AS_Assign(r);

        r->m_GPUData->Destroy();
        delete r->m_GPUData;
        delete r;
        return m;
    }else
    {
        r->GeoPropsFromMap(m);

        return r;
    }


}


inline static AS_GPUMap * ApplyGPUKernelDoubleMap(OpenCLProgram * p,AS_GPUMap * m,float m2, bool assign_back = false)
{
    AS_GPUMap * r = new AS_GPUMap();
    r->m_GPUData = new OpenCLTexture();
    r->m_GPUData->Create2DRF32(CGlobalGLCLManager->context,m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1],0.0);

    //run kernel
    ModelTexture t1;
    t1.m_IsCPU = false;
    t1.m_IsCLOnly = true;
    t1.m_TextureGPUCL = m->m_GPUData;

    ModelTexture t3;
    t3.m_IsCPU = false;
    t3.m_IsCLOnly = true;
    t3.m_TextureGPUCL = r->m_GPUData;


    p->PlaceArgument(0,(int)(m->m_GPUData->m_dims[0]));
    p->PlaceArgument(1,(int)(m->m_GPUData->m_dims[1]));

    p->PlaceArgument(2,&t1);
    p->PlaceArgument(3,(float) m2);
    p->PlaceArgument(4,&t3);

    p->SetRunDims(m->m_GPUData->m_dims[0],m->m_GPUData->m_dims[1]);
    p->Run(CGlobalGLCLManager->context,CGlobalGLCLManager->q);

    if(assign_back)
    {
        m->AS_Assign(r);

        r->m_GPUData->Destroy();
        delete r->m_GPUData;
        delete r;
        return m;
    }else
    {
        r->GeoPropsFromMap(m);
        return r;
    }

}


inline static AS_GPUMap * GPUSqrt(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelSqrt,m);
}
inline static AS_GPUMap * GPUExp(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelExp,m);
}
inline static AS_GPUMap * GPULog(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelLog,m);
}
inline static AS_GPUMap * GPUSin(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelSin,m);
}
inline static AS_GPUMap * GPUAsin(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAsin,m);
}
inline static AS_GPUMap * GPUCos(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelCos,m);
}
inline static AS_GPUMap * GPUAcos(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAcos,m);
}
inline static AS_GPUMap * GPUTan(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelTan,m);
}
inline static AS_GPUMap * GPUATan(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAtan,m);
}
inline static AS_GPUMap * GPUSinh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelSinh,m);
}
inline static AS_GPUMap * GPUAsinh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAsinh,m);
}
inline static AS_GPUMap * GPUCosh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelCosh,m);
}
inline static AS_GPUMap * GPUAcosh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAcosh,m);
}
inline static AS_GPUMap * GPUTanh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelTanh,m);
}
inline static AS_GPUMap * GPUATanh(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAtanh,m);
}
inline static AS_GPUMap * GPUAbs(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelAbs,m);
}
inline static AS_GPUMap * GPUFloor(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelFloor,m);
}
inline static AS_GPUMap * GPUDecimal(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelDecimal,m);
}
inline static AS_GPUMap * GPUSmoothStep(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelSmoothstep,m);
}
inline static AS_GPUMap * GPUCbrt(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelCbrt,m);
}
inline static AS_GPUMap * GPURand(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelRand,m);
}
inline static AS_GPUMap * GPURound(AS_GPUMap * m)
{
    return ApplyGPUKernelSingleMap(CGLKernelRound,m);
}


inline static AS_GPUMap * GPUMin(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMin,m,m2);
}
inline static AS_GPUMap * GPUMax(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMax,m,m2);
}
inline static AS_GPUMap * GPUAdd(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelAdd,m,m2);
}
inline static AS_GPUMap * GPUMod(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMod,m,m2);
}
inline static AS_GPUMap * GPUSub(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSub,m,m2);
}
inline static AS_GPUMap * GPUMul(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMul,m,m2);
}
inline static AS_GPUMap * GPUDiv(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelDiv,m,m2);
}
inline static AS_GPUMap * GPULogN(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelLogN,m,m2);
}
inline static AS_GPUMap * GPUPow(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelPow,m,m2);
}
inline static AS_GPUMap * GPUEq(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelEq,m,m2);
}
inline static AS_GPUMap * GPUGreater(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelGreater,m,m2);
}
inline static AS_GPUMap * GPUSmaller(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSmaller,m,m2);
}
inline static AS_GPUMap * GPUGreaterEq(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelGreaterEq,m,m2);
}
inline static AS_GPUMap * GPUSmallerEq(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSmallerEq,m,m2);
}
inline static AS_GPUMap * GPUFdim(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelFdim,m,m2);
}
inline static AS_GPUMap * GPUHypoth(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelHypoth,m,m2);
}
inline static AS_GPUMap * GPUAnd(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelAnd,m,m2);
}
inline static AS_GPUMap * GPUOr(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelOr,m,m2);
}
inline static AS_GPUMap * GPUXOr(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelXOr,m,m2);
}
inline static AS_GPUMap * GPUNot(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelNot,m,m2);
}
inline static AS_GPUMap * GPUSign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSign,m,m2);
}


inline static AS_GPUMap * GPUfMin(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMin,m,m2);
}
inline static AS_GPUMap * GPUfMax(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMax,m,m2);
}
inline static AS_GPUMap * GPUfAdd(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfAdd,m,m2);
}
inline static AS_GPUMap * GPUfMod(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMod,m,m2);
}
inline static AS_GPUMap * GPUfSub(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfSub,m,m2);
}
inline static AS_GPUMap * GPUfMul(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMul,m,m2);
}
inline static AS_GPUMap * GPUfDiv(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfDiv,m,m2);
}
inline static AS_GPUMap * GPUfLogN(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfLogN,m,m2);
}
inline static AS_GPUMap * GPUfPow(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfPow,m,m2);
}
inline static AS_GPUMap * GPUfEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfEq,m,m2);
}
inline static AS_GPUMap * GPUfGreater(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfGreater,m,m2);
}
inline static AS_GPUMap * GPUfSmaller(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfSmaller,m,m2);
}
inline static AS_GPUMap * GPUfGreaterEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfGreaterEq,m,m2);
}
inline static AS_GPUMap * GPUfSmallerEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfSmallerEq,m,m2);
}
inline static AS_GPUMap * GPUfFdim(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfFdim,m,m2);
}
inline static AS_GPUMap * GPUfHypoth(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfHypoth,m,m2);
}
inline static AS_GPUMap * GPUfAnd(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfAnd,m,m2);
}
inline static AS_GPUMap * GPUfOr(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfOr,m,m2);
}
inline static AS_GPUMap * GPUfXOr(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfXOr,m,m2);
}

inline static AS_GPUMap * GPUfrMin(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrMin,m,m2);
}
inline static AS_GPUMap * GPUfrMax(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrMax,m,m2);
}
inline static AS_GPUMap * GPUfrAdd(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrAdd,m,m2);
}
inline static AS_GPUMap * GPUfrMod(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrMod,m,m2);
}
inline static AS_GPUMap * GPUfrSub(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrSub,m,m2);
}
inline static AS_GPUMap * GPUfrMul(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrMul,m,m2);
}
inline static AS_GPUMap * GPUfrDiv(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrDiv,m,m2);
}
inline static AS_GPUMap * GPUfrLogN(float m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfLogN,m2,m);
}
inline static AS_GPUMap * GPUfrPow(float m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfPow,m2,m);
}
inline static AS_GPUMap * GPUfrEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrEq,m,m2);
}
inline static AS_GPUMap * GPUfrGreater(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrGreater,m,m2);
}
inline static AS_GPUMap * GPUfrSmaller(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrSmaller,m,m2);
}
inline static AS_GPUMap * GPUfrGreaterEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrGreaterEq,m,m2);
}
inline static AS_GPUMap * GPUfrSmallerEq(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrSmallerEq,m,m2);
}
inline static AS_GPUMap * GPUfrFdim(float m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfFdim,m2,m);
}
inline static AS_GPUMap * GPUfrHypoth(float m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfHypoth,m2,m);
}
inline static AS_GPUMap * GPUfrAnd(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrAnd,m,m2);
}
inline static AS_GPUMap * GPUfrOr(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrOr,m,m2);
}
inline static AS_GPUMap * GPUfrXOr(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfrXOr,m,m2);
}




inline static AS_GPUMap * GPUfGreaterAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfGreater,m,m2,true);
}
inline static AS_GPUMap * GPUfSmallerAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfSmaller,m,m2,true);
}
inline static AS_GPUMap * GPUfAndAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfAnd,m,m2,true);
}
inline static AS_GPUMap * GPUfOrAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfOr,m,m2,true);
}
inline static AS_GPUMap * GPUfXOrAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfXOr,m,m2,true);
}

inline static AS_GPUMap * GPUGreaterAssign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelGreater,m,m2,true);
}
inline static AS_GPUMap * GPUSmallerAssign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSmaller,m,m2,true);
}
inline static AS_GPUMap * GPUAndAssign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelAnd,m,m2,true);
}
inline static AS_GPUMap * GPUOrAssign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelOr,m,m2,true);
}
inline static AS_GPUMap * GPUXOrAssign(AS_GPUMap * m,AS_GPUMap * m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelXOr,m,m2,true);
}




inline static AS_GPUMap * GPUAddAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelAdd,m,m2,true);
}
inline static AS_GPUMap * GPUModAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMod,m,m2,true);
}
inline static AS_GPUMap * GPUSubAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelSub,m,m2,true);
}
inline static AS_GPUMap * GPUMulAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelMul,m,m2,true);
}
inline static AS_GPUMap * GPUDivAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelDiv,m,m2,true);
}
inline static AS_GPUMap * GPUPowAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelPow,m,m2,true);
}


inline static AS_GPUMap * GPUfAddAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfAdd,m,m2,true);
}
inline static AS_GPUMap * GPUfModAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMod,m,m2,true);
}
inline static AS_GPUMap * GPUfSubAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfSub,m,m2,true);
}
inline static AS_GPUMap * GPUfMulAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfMul,m,m2,true);
}
inline static AS_GPUMap * GPUfDivAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfDiv,m,m2,true);
}
inline static AS_GPUMap * GPUfPowAssign(AS_GPUMap * m,float m2)
{
    return ApplyGPUKernelDoubleMap(CGLKernelfPow,m,m2,true);
}

class AS_GPUScript
{
private:
    OpenCLProgram * program;
    bool is_valid = false;


    public:
    inline AS_GPUScript()
    {

    }

    inline bool LoadFromExpression(QString expression, QString kernelname)
    {
        program = new OpenCLProgram();
        try
        {
            if(program->LoadProgramString(CGlobalGLCLManager->context,CGlobalGLCLManager->m_device,expression.toStdString(),kernelname.toStdString()) == 0)
            {
                return false;
            }
        }catch(std::exception e)
        {
            return false;
        }catch( ... )
        {
            return false;
        }

        is_valid = true;
        return true;
    }

    inline void SetArgumentMap()
    {

    }
    inline void SetArgumentArrayI()
    {

    }

    inline void SetArgumentArrayF()
    {

    }

    inline void SetArgumentInt()
    {

    }
    inline void SetArgumentFloat()
    {

    }


    inline bool Run()
    {
         return false;
    }

    QString        AS_FileName = "";
    AS_GPUScript * AS_Assign            (AS_GPUScript*);
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();



};

//Angelscript related functionality

inline void AS_GPUScript::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void AS_GPUScript::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {


        delete this;

    }

}

inline AS_GPUScript * AS_GPUScript::AS_Assign(AS_GPUScript * gpum)
{



    return this;
}


#include "lsmio.h"


inline static AS_GPUScript * AS_GPUScriptFromFile(QString file, QString kernel)
{
    AS_GPUScript * ret = new AS_GPUScript();

    if(!FileExists(AS_DIR+file))
    {
        LISEMS_ERROR("File could not be found: " + AS_DIR + file);
        throw 1;
    }
    if(!ret->LoadFromExpression(GetFileString(AS_DIR + file),kernel))
    {
        LISEMS_ERROR("GPU Script could not be compiled: " + AS_DIR + file);
        throw 1;
    }
    return ret;
}

inline static AS_GPUScript * AS_GPUScriptFromFileAbsPath(QString file, QString kernel)
{
    AS_GPUScript * ret = new AS_GPUScript();
    if(!FileExists(AS_DIR+file))
    {
        LISEMS_ERROR("File could not be found: " + file);
        throw 1;
    }
    if(!ret->LoadFromExpression(GetFileString(file),kernel))
    {
        LISEMS_ERROR("GPU Script could not be compiled: " + file);
        throw 1;
    }

    return ret;
}

inline static AS_GPUScript * AS_GPUScriptFromExpression(QString expression, QString kernel)
{
    AS_GPUScript * ret = new AS_GPUScript();

    ret->LoadFromExpression(expression,kernel);


    return ret;

}

inline static AS_GPUScript * GPUScriptConstructor()
{
    if(!HasDefaultGPUModels())
    {
        LoadDefaultGPUModels();
    }

    AS_GPUScript * m = new AS_GPUScript();
    return m;
}


inline static float ApplyReductionToMap(AS_GPUMap * m, OpenCLProgram * red, OpenCLProgram * redrem, std::function<float(float,float)> reduction, float val_start = 0.0)
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
        LISEMS_ERROR("Could not create output buffer");
        throw 1;
    }

    cl_kernel kernel1 = red->m_kernel();
    cl_kernel kernel2 = redrem->m_kernel();
    const size_t globalWorkSize = size_corr/8;
    const size_t localWorkSize = size_workgroup;

    //two kernels: first one runs reduction per 1024 items
    //second kernel: runs a serial reduction on the remainder of the dataset (size % 1024)

    int sizex = m->m_GPUData->m_dims[0];
    int sizey = m->m_GPUData->m_dims[1];
    int outputoffset = (size_output -1);
    int inOffset = size_workgroupcount * (size/size_workgroupcount);
    clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-1 ");
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-2 ");
        throw 1;
    }
    cl_mem obj1 =m->m_GPUData->m_texcl();

    ret = clSetKernelArg(kernel1, 2, sizeof(cl_mem), (void *)&obj1);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-3 ");
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 3, sizeof(cl_mem), (void *)&outbuf);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-4 ");
        throw 1;
    }
    //then run the first kernel for (n-1) output buffer results

    size_t wgSize[3] = {256, 1, 1};
    size_t gSize[3] = {size_corr/8, 1, 1};


    ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Error while running kernel1 ");
        std::cout << "error is "<< ret << std::endl;
        throw 1;
    }

    //then run the second kernel for the last output buffer result

    if(remainder > 0)
    {
        clSetKernelArg(kernel2, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-1 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-2 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 2, sizeof(cl_mem), (void *)&obj1);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-3 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 3, sizeof(cl_mem), (void *)&outbuf);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-4 ");
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 4, sizeof(int), (void *)&size);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-5 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 5, sizeof(int), (void *)&inOffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-6 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 6, sizeof(int), (void *)&outputoffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel2-7 ");
            throw 1;
        }


        ret = clEnqueueTask(CGlobalGLCLManager->q(),kernel2,0,NULL,NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel2 ");
            std::cout << "error is "<< ret << "  " << outputoffset << " " << inOffset << " " << size << " " << std::endl;
            throw 1;
        }
    }


    //read output buffer
    float * results = (float*) malloc(sizeof(float)*size_output);
    ret = clEnqueueReadBuffer(CGlobalGLCLManager->q(), outbuf, CL_TRUE, 0, sizeof(int)*size_output, results, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not read buffer ");
        throw 1;
    }


    float result = val_start;
    // finally do a reduction on the final output buffer outputs
    for(int i = 0; i < size_output; i++)
    {
        result = reduction(result,results[i]);
    }

    ret = clReleaseMemObject(outbuf);
    free(results);

    return result;

}

inline static float ApplyReductionToFArray(AS_GPUFArray * m, OpenCLProgram * red, OpenCLProgram * redrem, std::function<float(float,float)> reduction, float val_start = 0.0)
{
    /*if(m->m_GPUData == nullptr)
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
        LISEMS_ERROR("Could not create output buffer");
        throw 1;
    }

    cl_kernel kernel1 = red->m_kernel();
    cl_kernel kernel2 = redrem->m_kernel();
    const size_t globalWorkSize = size;
    const size_t localWorkSize = size_workgroup;

    //two kernels: first one runs reduction per 1024 items
    //second kernel: runs a serial reduction on the remainder of the dataset (size % 1024)

    int sizex = m->m_GPUData->m_dims[0];
    int sizey = m->m_GPUData->m_dims[1];
    int outputoffset = (size_output -1);
    int inOffset = size_workgroupcount * (size/size_workgroupcount);
    clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-1 ");
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-2 ");
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 2, sizeof(int), (void *)&m->m_GPUData->m_texcl);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-3 ");
        throw 1;
    }
    ret = clSetKernelArg(kernel1, 3, sizeof(cl_mem), outbuf);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not set argument for kernel1-4 ");
        throw 1;
    }
    //then run the first kernel for (n-1) output buffer results

    size_t wgSize[3] = {256, 1, 1};
    size_t gSize[3] = {size_corr/8, 1, 1};


    ret = clEnqueueNDRangeKernel(CGlobalGLCLManager->q(), kernel1, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Error while running kernel1 ");
        throw 1;
    }

    //then run the second kernel for the last output buffer result

    if(remainder > 0)
    {
        clSetKernelArg(kernel1, 0, sizeof(int), (void *)&sizex);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-1 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 1, sizeof(int), (void *)&sizey);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-2 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 2, sizeof(int), (void *)&m->m_GPUData->m_texcl);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-3 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel1, 3, sizeof(cl_mem), outbuf);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-4 ");
            throw 1;
        }

        ret = clSetKernelArg(kernel2, 4, sizeof(int), (void *)&size);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-3 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 5, sizeof(int), (void *)&inOffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-3 ");
            throw 1;
        }
        ret = clSetKernelArg(kernel2, 6, sizeof(int), (void *)&outputoffset);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Could not set argument for kernel1-3 ");
            throw 1;
        }


        ret = clEnqueueTask(CGlobalGLCLManager->q(),kernel2,0,NULL,NULL);
        if(ret != CL_SUCCESS )
        {
            LISEMS_ERROR("Error while running kernel2 ");
            throw 1;
        }
    }


    //read output buffer
    float * results = (float*) malloc(sizeof(float)*size_output);
    ret = clEnqueueReadBuffer(CGlobalGLCLManager->q(), outbuf, CL_TRUE, 0, sizeof(int)*size_output, results, 0, NULL, NULL);
    if(ret != CL_SUCCESS )
    {
        LISEMS_ERROR("Could not read buffer ");
        throw 1;
    }


    float result = val_start;
    // finally do a reduction on the final output buffer outputs
    for(int i = 0; i < size_output; i++)
    {
        result = reduction(result,results[i]);
    }

    ret = clReleaseMemObject(outbuf);
    free(results);

    return result;*/

    return 0.0;

}


inline static float AS_MapMinimumRedGPU(AS_GPUMap * m)
{
    return ApplyReductionToMap(m, CGLKernelRedMin,CGLKernelRedRemMin, std::function<float(float,float)>([](float a,float b){return std::min(a,b);}),1e31);
}
inline static float AS_MapMaximumRedGPU(AS_GPUMap * m)
{
    return ApplyReductionToMap(m, CGLKernelRedMax,CGLKernelRedRemMax, std::function<float(float,float)>([](float a,float b){return std::max(a,b);}),-1e31);
}
inline static float AS_MapTotalRedGPU(AS_GPUMap * m)
{
    return ApplyReductionToMap(m, CGLKernelRedSum,CGLKernelRedRemSum, std::function<float(float,float)>([](float a,float b){return  a +b;}),0.0);
}
/*inline static CScriptArray * AS_RunGPUModel(AS_GPUModel *, CScriptArray * input)
{

    //load input maps to GPU

    //prepare potential output maps


    //run model


    //download output

    //delete GPU textures


    //return output


    return nullptr;
}*/




#endif // RASTERGPUCODE_H
