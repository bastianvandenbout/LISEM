#include "rastergpucode.h"

//Default OPENCL functions for both arrays and maps
 QMutex CGLKernelMutex;
 QList<OpenCLProgram *> CGLGPUKernels;
 bool CGLHasGPUKernels = false;

//one input
 OpenCLProgram * CGLKernelSqrt= nullptr;
 OpenCLProgram * CGLKernelExp= nullptr;
 OpenCLProgram * CGLKernelLog= nullptr;
 OpenCLProgram * CGLKernelSin= nullptr;
 OpenCLProgram * CGLKernelAsin= nullptr;
 OpenCLProgram * CGLKernelCos= nullptr;
 OpenCLProgram * CGLKernelAcos= nullptr;
 OpenCLProgram * CGLKernelTan= nullptr;
 OpenCLProgram * CGLKernelAtan= nullptr;
 OpenCLProgram * CGLKernelSinh= nullptr;
 OpenCLProgram * CGLKernelAsinh= nullptr;
 OpenCLProgram * CGLKernelCosh= nullptr;
 OpenCLProgram * CGLKernelAcosh= nullptr;
 OpenCLProgram * CGLKernelTanh= nullptr;
 OpenCLProgram * CGLKernelAtanh= nullptr;
 OpenCLProgram * CGLKernelAbs= nullptr;
 OpenCLProgram * CGLKernelFloor= nullptr;
 OpenCLProgram * CGLKernelSmoothstep= nullptr;
 OpenCLProgram * CGLKernelDecimal= nullptr;
 OpenCLProgram * CGLKernelCbrt= nullptr;
 OpenCLProgram * CGLKernelRand= nullptr;
 OpenCLProgram * CGLKernelRound= nullptr;

//two-inputs
 OpenCLProgram * CGLKernelMin= nullptr;
 OpenCLProgram * CGLKernelMax= nullptr;
 OpenCLProgram * CGLKernelAdd= nullptr;
 OpenCLProgram * CGLKernelMod= nullptr;
 OpenCLProgram * CGLKernelSub= nullptr;
 OpenCLProgram * CGLKernelMul= nullptr;
 OpenCLProgram * CGLKernelDiv= nullptr;
 OpenCLProgram * CGLKernelLogN= nullptr;
 OpenCLProgram * CGLKernelPow= nullptr;
 OpenCLProgram * CGLKernelEq= nullptr;
 OpenCLProgram * CGLKernelGreater= nullptr;
 OpenCLProgram * CGLKernelSmaller= nullptr;
 OpenCLProgram * CGLKernelGreaterEq= nullptr;
 OpenCLProgram * CGLKernelSmallerEq= nullptr;
 OpenCLProgram * CGLKernelFdim= nullptr;
 OpenCLProgram * CGLKernelHypoth= nullptr;
 OpenCLProgram * CGLKernelAnd= nullptr;
 OpenCLProgram * CGLKernelOr= nullptr;
 OpenCLProgram * CGLKernelXOr= nullptr;
 OpenCLProgram * CGLKernelNot= nullptr;
 OpenCLProgram * CGLKernelSign= nullptr;

OpenCLProgram * CGLKernelfMin= nullptr;
OpenCLProgram * CGLKernelfMax= nullptr;
OpenCLProgram * CGLKernelfAdd= nullptr;
OpenCLProgram * CGLKernelfMod= nullptr;
OpenCLProgram * CGLKernelfSub= nullptr;
OpenCLProgram * CGLKernelfMul= nullptr;
OpenCLProgram * CGLKernelfDiv= nullptr;
OpenCLProgram * CGLKernelfLogN= nullptr;
OpenCLProgram * CGLKernelfPow= nullptr;
OpenCLProgram * CGLKernelfEq= nullptr;
OpenCLProgram * CGLKernelfGreater= nullptr;
OpenCLProgram * CGLKernelfSmaller= nullptr;
OpenCLProgram * CGLKernelfGreaterEq= nullptr;
OpenCLProgram * CGLKernelfSmallerEq= nullptr;
OpenCLProgram * CGLKernelfFdim= nullptr;
OpenCLProgram * CGLKernelfHypoth= nullptr;
OpenCLProgram * CGLKernelfAnd= nullptr;
OpenCLProgram * CGLKernelfOr= nullptr;
OpenCLProgram * CGLKernelfXOr= nullptr;

OpenCLProgram * CGLKernelfrMin= nullptr;
OpenCLProgram * CGLKernelfrMax= nullptr;
OpenCLProgram * CGLKernelfrAdd= nullptr;
OpenCLProgram * CGLKernelfrMod= nullptr;
OpenCLProgram * CGLKernelfrSub= nullptr;
OpenCLProgram * CGLKernelfrMul= nullptr;
OpenCLProgram * CGLKernelfrDiv= nullptr;
OpenCLProgram * CGLKernelfrLogN= nullptr;
OpenCLProgram * CGLKernelfrPow= nullptr;
OpenCLProgram * CGLKernelfrEq= nullptr;
OpenCLProgram * CGLKernelfrGreater= nullptr;
OpenCLProgram * CGLKernelfrSmaller= nullptr;
OpenCLProgram * CGLKernelfrGreaterEq= nullptr;
OpenCLProgram * CGLKernelfrSmallerEq= nullptr;
OpenCLProgram * CGLKernelfrFdim= nullptr;
OpenCLProgram * CGLKernelfrHypoth= nullptr;
OpenCLProgram * CGLKernelfrAnd= nullptr;
OpenCLProgram * CGLKernelfrOr= nullptr;
OpenCLProgram * CGLKernelfrXOr= nullptr;


//one input
 OpenCLProgram * CGLArrIKernelSqrt= nullptr;
 OpenCLProgram * CGLArrIKernelExp= nullptr;
 OpenCLProgram * CGLArrIKernelLog= nullptr;
 OpenCLProgram * CGLArrIKernelSin= nullptr;
 OpenCLProgram * CGLArrIKernelAsin= nullptr;
 OpenCLProgram * CGLArrIKernelCos= nullptr;
 OpenCLProgram * CGLArrIKernelAcos= nullptr;
 OpenCLProgram * CGLArrIKernelTan= nullptr;
 OpenCLProgram * CGLArrIKernelAtan= nullptr;
 OpenCLProgram * CGLArrIKernelSinh= nullptr;
 OpenCLProgram * CGLArrIKernelAsinh= nullptr;
 OpenCLProgram * CGLArrIKernelCosh= nullptr;
 OpenCLProgram * CGLArrIKernelAcosh= nullptr;
 OpenCLProgram * CGLArrIKernelTanh= nullptr;
 OpenCLProgram * CGLArrIKernelAtanh= nullptr;
 OpenCLProgram * CGLArrIKernelAbs= nullptr;
 OpenCLProgram * CGLArrIKernelFloor= nullptr;
 OpenCLProgram * CGLArrIKernelDecimal= nullptr;
 OpenCLProgram * CGLArrIKernelSmoothstep= nullptr;
 OpenCLProgram * CGLArrIKernelCbrt= nullptr;
 OpenCLProgram * CGLArrIKernelRand= nullptr;
 OpenCLProgram * CGLArrIKernelRound= nullptr;

//two-inputs
 OpenCLProgram * CGLArrIKernelMin= nullptr;
 OpenCLProgram * CGLArrIKernelMax= nullptr;
 OpenCLProgram * CGLArrIKernelAdd= nullptr;
 OpenCLProgram * CGLArrIKernelMod= nullptr;
 OpenCLProgram * CGLArrIKernelSub= nullptr;
 OpenCLProgram * CGLArrIKernelMul= nullptr;
 OpenCLProgram * CGLArrIKernelDiv= nullptr;
 OpenCLProgram * CGLArrIKernelLogN= nullptr;
 OpenCLProgram * CGLArrIKernelPow= nullptr;
 OpenCLProgram * CGLArrIKernelEq= nullptr;
 OpenCLProgram * CGLArrIKernelGreater= nullptr;
 OpenCLProgram * CGLArrIKernelSmaller= nullptr;
 OpenCLProgram * CGLArrIKernelGreaterEq= nullptr;
 OpenCLProgram * CGLArrIKernelSmallerEq= nullptr;
 OpenCLProgram * CGLArrIFdim= nullptr;
 OpenCLProgram * CGLArrIHypot= nullptr;
 OpenCLProgram * CGLArrIKernelAnd= nullptr;
 OpenCLProgram * CGLArrIKernelOr= nullptr;
 OpenCLProgram * CGLArrIKernelXOr= nullptr;
 OpenCLProgram * CGLArrIKernelNot= nullptr;
 OpenCLProgram * CGLArrIKernelSign= nullptr;

//one input
 OpenCLProgram * CGLArrFKernelSqrt= nullptr;
 OpenCLProgram * CGLArrFKernelExp= nullptr;
 OpenCLProgram * CGLArrFKernelLog= nullptr;
 OpenCLProgram * CGLArrFKernelSin= nullptr;
 OpenCLProgram * CGLArrFKernelAsin= nullptr;
 OpenCLProgram * CGLArrFKernelCos= nullptr;
 OpenCLProgram * CGLArrFKernelAcos= nullptr;
 OpenCLProgram * CGLArrFKernelTan= nullptr;
 OpenCLProgram * CGLArrFKernelAtan= nullptr;
 OpenCLProgram * CGLArrFKernelSinh= nullptr;
 OpenCLProgram * CGLArrFKernelAsinh= nullptr;
 OpenCLProgram * CGLArrFKernelCosh= nullptr;
 OpenCLProgram * CGLArrFKernelAcosh= nullptr;
 OpenCLProgram * CGLArrFKernelTanh= nullptr;
 OpenCLProgram * CGLArrFKernelAtanh= nullptr;
 OpenCLProgram * CGLArrFKernelAbs= nullptr;
 OpenCLProgram * CGLArrFKernelFloor= nullptr;
 OpenCLProgram * CGLArrFKernelDecimal= nullptr;
 OpenCLProgram * CGLArrFKernelSmoothstep= nullptr;
 OpenCLProgram * CGLArrFKernelCbrt= nullptr;
 OpenCLProgram * CGLArrFKernelRand= nullptr;
 OpenCLProgram * CGLArrFKernelRound= nullptr;

//two-inputs
 OpenCLProgram * CGLArrFKernelMin= nullptr;
 OpenCLProgram * CGLArrFKernelMax= nullptr;
 OpenCLProgram * CGLArrFKernelAdd= nullptr;
 OpenCLProgram * CGLArrFKernelMod= nullptr;
 OpenCLProgram * CGLArrFKernelSub= nullptr;
 OpenCLProgram * CGLArrFKernelMul= nullptr;
 OpenCLProgram * CGLArrFKernelDiv= nullptr;
 OpenCLProgram * CGLArrFKernelLogN= nullptr;
 OpenCLProgram * CGLArrFKernelPow= nullptr;
 OpenCLProgram * CGLArrFKernelEq= nullptr;
 OpenCLProgram * CGLArrFKernelGreater= nullptr;
 OpenCLProgram * CGLArrFKernelSmaller= nullptr;
 OpenCLProgram * CGLArrFKernelGreaterEq= nullptr;
 OpenCLProgram * CGLArrFKernelSmallerEq= nullptr;
 OpenCLProgram * CGLArrFFdim= nullptr;
 OpenCLProgram * CGLArrFHypot= nullptr;
 OpenCLProgram * CGLArrFKernelAnd= nullptr;
 OpenCLProgram * CGLArrFKernelOr= nullptr;
 OpenCLProgram * CGLArrFKernelXOr= nullptr;
 OpenCLProgram * CGLArrFKernelNot= nullptr;
 OpenCLProgram * CGLArrFKernelSign= nullptr;

 //two-inputs
 OpenCLProgram * CGLKernelRedMin= nullptr;
 OpenCLProgram * CGLKernelRedMax= nullptr;
 OpenCLProgram * CGLKernelRedTotal= nullptr;
 OpenCLProgram * CGLKernelRedSum= nullptr;
 OpenCLProgram * CGLArrIKernelRedMin= nullptr;
 OpenCLProgram * CGLArrIKernelRedMax= nullptr;
 OpenCLProgram * CGLArrIKernelRedTotal= nullptr;
 OpenCLProgram * CGLArrIKernelRedSum= nullptr;
 OpenCLProgram * CGLArrFKernelRedMin= nullptr;
 OpenCLProgram * CGLArrFKernelRedMax= nullptr;
 OpenCLProgram * CGLArrFKernelRedTotal= nullptr;
 OpenCLProgram * CGLArrFKernelRedSum= nullptr;
 OpenCLProgram * CGLKernelRedRemMin= nullptr;
 OpenCLProgram * CGLKernelRedRemMax= nullptr;
 OpenCLProgram * CGLKernelRedRemTotal= nullptr;
 OpenCLProgram * CGLKernelRedRemSum= nullptr;
 OpenCLProgram * CGLArrIKernelRedRemMin= nullptr;
 OpenCLProgram * CGLArrIKernelRedRemMax= nullptr;
 OpenCLProgram * CGLArrIKernelRedRemTotal= nullptr;
 OpenCLProgram * CGLArrIKernelRedRemSum= nullptr;
 OpenCLProgram * CGLArrFKernelRedRemMin= nullptr;
 OpenCLProgram * CGLArrFKernelRedRemMax= nullptr;
 OpenCLProgram * CGLArrFKernelRedRemTotal= nullptr;
 OpenCLProgram * CGLArrFKernelRedRemSum= nullptr;