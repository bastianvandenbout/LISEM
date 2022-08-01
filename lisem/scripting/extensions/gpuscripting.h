#ifndef GPUSCRIPTING_H
#define GPUSCRIPTING_H

#include "raster/rastergpucode.h"
#include "lsmscriptengine.h"
#include "raster/rasterflowgpu.h"




#include "scriptarrayhelpers.h"



template<>
class asbindc_convert<std::vector<AS_GPUMap*>,CScriptArray*>
{

public:

    inline static CScriptArray* asbind_convert(std::vector<AS_GPUMap*> x)
    {
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        asIScriptEngine *engine = ctx->GetEngine();

        // TODO: This should only be done once
        // TODO: This assumes that CScriptArray was already registered
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<GPUMap>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);
        array->Resize(x.size());
        for(int i = 0; i < x.size(); i++)
        {
            array->SetValue(i,x.at(i),false);
        }

        return array;
    }
};




inline void RegisterGPUToScriptEngine(LSMScriptEngine *engine)
{


    int r = engine->RegisterObjectType("GPUMap",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()
    if(r < 0){LISEM_DEBUG("error in registering scripting: class GPUMap");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("GPUMap",asBEHAVE_FACTORY,"GPUMap@ C0()",asFUNCTIONPR(GPUMapConstructor,(),AS_GPUMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GPUMap",asBEHAVE_FACTORY,"GPUMap@ C1(int rows, int columns, float val)",asFUNCTIONPR(GPUMapConstructor,(int,int,float),AS_GPUMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GPUMap",asBEHAVE_FACTORY,"GPUMap@ C2(Map & in copy)",asFUNCTIONPR(GPUMapConstructor,(cTMap *),AS_GPUMap *),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("GPUMap",asBEHAVE_ADDREF,"void f()",asMETHOD(AS_GPUMap,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GPUMap",asBEHAVE_RELEASE,"void f()",asMETHOD(AS_GPUMap,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAssign(GPUMap &in m)", asMETHODPR(AS_GPUMap,AS_Assign,(AS_GPUMap *),AS_GPUMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAssign(Map &in m)", asMETHODPR(AS_GPUMap,AS_Assign,(cTMap *),AS_GPUMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("GPUMap @ToGPU(Map &in s)", asFUNCTION( CPUToGPU),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ToCPU(GPUMap &in s)", asFUNCTION( GPUToCPU),  asCALL_CDECL); assert( r >= 0 );

    //mono-operators
    r = engine->RegisterGlobalFunction("GPUMap @sqrt(GPUMap &in s)", asFUNCTION( GPUSqrt),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @exp(GPUMap &in s)", asFUNCTION( GPUExp),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @log(GPUMap &in s)", asFUNCTION( GPULog),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @sin(GPUMap &in s)", asFUNCTION( GPUSin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @asin(GPUMap &in s)", asFUNCTION( GPUAsin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @cos(GPUMap &in s)", asFUNCTION( GPUCos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @acos(GPUMap &in s)", asFUNCTION( GPUAcos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @tan(GPUMap &in s)", asFUNCTION( GPUTan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @atan(GPUMap &in s)", asFUNCTION( GPUATan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @sinh(GPUMap &in s)", asFUNCTION( GPUSinh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @asinh(GPUMap &in s)", asFUNCTION( GPUAsinh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @cosh(GPUMap &in s)", asFUNCTION( GPUCosh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @acosh(GPUMap &in s)", asFUNCTION( GPUAcosh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @tanh(GPUMap &in s)", asFUNCTION( GPUTanh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @atanh(GPUMap &in s)", asFUNCTION( GPUATanh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @abs(GPUMap &in s)", asFUNCTION( GPUAbs),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @floor(GPUMap &in s)", asFUNCTION( GPUFloor),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @fraction(GPUMap &in s)", asFUNCTION( GPUDecimal),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @smoothstep(GPUMap &in s)", asFUNCTION( GPUSmoothStep),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @cbrt(GPUMap &in s)", asFUNCTION( GPUCbrt),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @rand(GPUMap &in s)", asFUNCTION( GPURand),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @round(GPUMap &in s)", asFUNCTION( GPURound),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("GPUMap @min(GPUMap &in s,GPUMap &in s2)", asFUNCTION( GPUMin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUMap @max(GPUMap &in s,GPUMap &in s2)", asFUNCTION( GPUMax),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("GPUMap @sign(GPUMap &in m)",asFUNCTION(GPUSign),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @fdim(GPUMap &in m,GPUMap &in m2)",asFUNCTION(GPUFdim),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @fdim(GPUMap &in m,float m2)",asFUNCTION(GPUfFdim),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @fdim(float m,GPUMap &in m2)",asFUNCTION(GPUfrFdim),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @hypot(GPUMap &in m,GPUMap &in m2)",asFUNCTION(GPUHypoth),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @hypot(GPUMap &in m,float m2)",asFUNCTION(GPUfHypoth),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @hypot(float m,GPUMap &in m2)",asFUNCTION(GPUfrHypoth),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @logn(GPUMap &in m,GPUMap &in m2)",asFUNCTION(GPULogN),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @logn(GPUMap &in m,float m2)",asFUNCTION(GPUfLogN),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @logn(float m,GPUMap &in m2)",asFUNCTION(GPUfrLogN),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @pow(GPUMap &in m,GPUMap &in m2)",asFUNCTION(GPUPow),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @pow(GPUMap &in m,float m2)",asFUNCTION(GPUfPow),asCALL_CDECL);
    engine->RegisterGlobalFunction("GPUMap @pow(float m,GPUMap &in m2)",asFUNCTION(GPUfrPow),asCALL_CDECL);

    //dual operators on map/map
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAdd(GPUMap &in m)",asFUNCTION(GPUAdd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMod(GPUMap &in m)",asFUNCTION(GPUMod),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opSub(GPUMap &in m)",asFUNCTION(GPUSub),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMul(GPUMap &in m)",asFUNCTION(GPUMul),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opDiv(GPUMap &in m)",asFUNCTION(GPUDiv),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opPow(GPUMap &in m)",asFUNCTION(GPUPow),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @eq(GPUMap &in m)",asFUNCTION(GPUEq),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShl(GPUMap &in m)",asFUNCTION(GPUSmaller),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShr(GPUMap &in m)",asFUNCTION(GPUGreater),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAnd(GPUMap &in m)",asFUNCTION(GPUAnd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opOr(GPUMap &in m)",asFUNCTION(GPUOr),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opXor(GPUMap &in m)",asFUNCTION(GPUXOr),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opCom()",asFUNCTION(GPUNot),asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAdd(float v)",asFUNCTION(GPUfAdd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMod(float v)",asFUNCTION(GPUfMod),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opSub(float v)",asFUNCTION(GPUfSub),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMul(float v)",asFUNCTION(GPUfMul),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opDiv(float v)",asFUNCTION(GPUfDiv),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opPow(float v)",asFUNCTION(GPUfPow),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @eq(float v)",asFUNCTION(GPUfEq),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShl(float v)",asFUNCTION(GPUfGreater),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShr(float v)",asFUNCTION(GPUfSmaller),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAnd(float v)",asFUNCTION(GPUfAnd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opOr(float v)",asFUNCTION(GPUfOr),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opXor(float v)",asFUNCTION(GPUfXOr),asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAdd_r(float v)",asFUNCTION(GPUfrAdd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMod_r(float v)",asFUNCTION(GPUfrMod),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opSub_r(float v)",asFUNCTION(GPUfrSub),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opMul_r(float v)",asFUNCTION(GPUfrMul),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opDiv_r(float v)",asFUNCTION(GPUfrDiv),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opPow_r(float v)",asFUNCTION(GPUfrPow),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShl_r(float v)",asFUNCTION(GPUfrSmaller),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opShr_r(float v)",asFUNCTION(GPUfrGreater),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opAnd_r(float v)",asFUNCTION(GPUfrAnd),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opOr_r(float v)",asFUNCTION(GPUfrOr),asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod("GPUMap", "GPUMap @opXor_r(float v)",asFUNCTION(GPUfrXOr),asCALL_CDECL_OBJFIRST);

    //combined assignment operators
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opShlAssign(GPUMap &in m)", asFUNCTION(GPUSmallerAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opShrAssign(GPUMap &in m)", asFUNCTION(GPUGreaterAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opShlAssign(float v)", asFUNCTION(GPUfSmallerAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opShrAssign(float v)", asFUNCTION(GPUfGreaterAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAndAssign(GPUMap &in m)", asFUNCTION(GPUAndAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opOrAssign(GPUMap &in m)", asFUNCTION(GPUOrAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opXorAssign(GPUMap &in m)", asFUNCTION(GPUXOrAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAndAssign(float v)", asFUNCTION(GPUfAndAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opOrAssign(float v)", asFUNCTION(GPUfOrAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opXorAssign(float v)", asFUNCTION(GPUfXOrAssign),asCALL_CDECL_OBJFIRST);

    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAddAssign(GPUMap &in m)", asFUNCTION(GPUAddAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opMulAssign(GPUMap &in m)", asFUNCTION(GPUMulAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opSubAssign(GPUMap &in m)", asFUNCTION(GPUSubAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opDivAssign(GPUMap &in m)", asFUNCTION(GPUDivAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opPowAssign(GPUMap &in m)", asFUNCTION(GPUPowAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opModAssign(GPUMap &in m)", asFUNCTION(GPUModAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opAddAssign(float v)", asFUNCTION(GPUfAddAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opMulAssign(float v)", asFUNCTION(GPUfMulAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opSubAssign(float v)", asFUNCTION(GPUfSubAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opDivAssign(float v)", asFUNCTION(GPUfDivAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opPowAssign(float v)", asFUNCTION(GPUfPowAssign),asCALL_CDECL_OBJFIRST);
    r = engine->RegisterObjectMethod("GPUMap", "GPUMap& opModAssign(float v)", asFUNCTION(GPUfModAssign),asCALL_CDECL_OBJFIRST);


    //dual operators on map/float


    //dual operators on float/map


    r = engine->RegisterObjectType("GPUScript",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()
    if(r < 0){LISEM_DEBUG("error in registering scripting: class GPUScript");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("GPUScript",asBEHAVE_FACTORY,"GPUScript@ C0()",asFUNCTIONPR(GPUScriptConstructor,(),AS_GPUScript *),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("GPUScript",asBEHAVE_ADDREF,"void f()",asMETHOD(AS_GPUScript,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GPUScript",asBEHAVE_RELEASE,"void f()",asMETHOD(AS_GPUScript,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GPUScript", "GPUScript& opAssign(GPUScript &in m)", asMETHODPR(AS_GPUScript,AS_Assign,(AS_GPUScript *),AS_GPUScript*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("GPUScript @GPUScriptFromFile(string &in script, string &in kernelname)", asFUNCTION( AS_GPUScriptFromFile),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUScript @GPUScriptFromFileAbsPath(string &in script, string &in kernelname)", asFUNCTION( AS_GPUScriptFromFileAbsPath),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GPUScript @GPUScriptFromExpression(string &in script, string &in kernelname)", asFUNCTION( AS_GPUScriptFromExpression),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("float MapMinimumRed(const GPUMap &in m)", asFUNCTION( AS_MapMinimumRedGPU),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapMaximumRed(const GPUMap &in m)", asFUNCTION( AS_MapMaximumRedGPU),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapTotalRed(const GPUMap &in m)", asFUNCTION( AS_MapTotalRedGPU),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalSTDFunction("array<GPUMap> @FlowDynamic(const GPUMap &in DEM, const GPUMap &in N, const GPUMap &in H, const GPUMap &in VX, const GPUMap &in VY, float dt, float courant = 0.15, float dt_min = 0.0)",GetFuncConvert( AS_DynamicWaveGPU),  asCALL_CDECL);
    r = engine->RegisterGlobalSTDFunction("array<GPUMap> @FlowDebris(const GPUMap &in DEM, const GPUMap &in N, const GPUMap &in H, const GPUMap &in VX, const GPUMap &in VY,const GPUMap &in HS, const GPUMap &in VSX, const GPUMap &in VSY, const GPUMap &in IFA, const GPUMap &in ROCKSIZE, const GPUMap &in DENSITY,float dt, float courant = 0.15, float drag_mult = 1.0)", GetFuncConvert(  AS_MixtureWaveGPU),  asCALL_CDECL);

}


#endif // GPUSCRIPTING_H
