#ifndef FIELDSCRIPTING_H
#define FIELDSCRIPTING_H

#include "lsmscriptengine.h"
#include "error.h"
#include "geo/raster/field.h"
#include "scriptarrayhelpers.h"
#include "geo/raster/field.h"
#include "field/fieldtrigonometric.h"
#include "field/fieldmath.h"
#include "raster/rasterflow.h"

inline void RegisterFieldToScriptEngine(LSMScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Field",0,asOBJ_REF );// | asGetTypeTraits<Field>()
    if(r < 0){LISEM_DEBUG("error in registering scripting: class Field");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Field",asBEHAVE_FACTORY,"Field@ C0()",asFUNCTIONPR(FieldFactory,(),Field *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Field",asBEHAVE_FACTORY,"Field@ C0(int,int,int,float,float,float,float,float,float)",asFUNCTIONPR(FieldFactory2,(int,int,int,float,float,float,float,float,float),Field *),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Field",asBEHAVE_ADDREF,"void f()",asMETHOD(Field,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Field",asBEHAVE_RELEASE,"void f()",asMETHOD(Field,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Field", "Field& opAssign(const Field &in m)", asMETHODPR(Field,Assign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterGlobalSTDFunction("Field @FieldFromMapList(array<Map> &in levels, float z_start, float dz)", GetFuncConvert(FieldFromMapList));
    r = engine->RegisterGlobalSTDFunction("array<Map> @GetMapList(const Field &in field)", GetFuncConvert(MapListFromField));


    r = engine->RegisterObjectMethod("Field", "Field@ opNeg()", asMETHODPR(Field,OpNeg,(void),Field*), asCALL_THISCALL); assert( r >= 0 );

    //mathematical operators

    r = engine->RegisterObjectMethod("Field", "Field@ opAdd(const Field &in m)", asMETHODPR(Field,OpAdd,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMul(const Field &in m)", asMETHODPR(Field,OpMul,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opSub(const Field &in m)", asMETHODPR(Field,OpSub,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opDiv(const Field &in m)", asMETHODPR(Field,OpDiv,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opPow(const Field &in m)", asMETHODPR(Field,OpPow,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMod(const Field &in m)", asMETHODPR(Field,OpMod,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opAdd(float v)", asMETHODPR(Field,OpAdd,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMul(float v)", asMETHODPR(Field,OpMul,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opSub(float v)", asMETHODPR(Field,OpSub,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opDiv(float v)", asMETHODPR(Field,OpDiv,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opPow(float v)", asMETHODPR(Field,OpPow,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMod(float v)", asMETHODPR(Field,OpMod,(float),Field*), asCALL_THISCALL); assert( r >= 0 );

    //assignment operators

    r = engine->RegisterObjectMethod("Field", "Field& opAddAssign(const Field &in m)", asMETHODPR(Field,OpAddAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opMulAssign(const Field &in m)", asMETHODPR(Field,OpMulAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opSubAssign(const Field &in m)", asMETHODPR(Field,OpSubAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opDivAssign(const Field &in m)", asMETHODPR(Field,OpDivAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opPowAssign(const Field &in m)", asMETHODPR(Field,OpPowAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opModAssign(const Field &in m)", asMETHODPR(Field,OpModAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Field", "Field& opAddAssign(float v)", asMETHODPR(Field,OpAddAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opMulAssign(float v)", asMETHODPR(Field,OpMulAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opSubAssign(float v)", asMETHODPR(Field,OpSubAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opDivAssign(float v)", asMETHODPR(Field,OpDivAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opPowAssign(float v)", asMETHODPR(Field,OpPowAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opModAssign(float v)", asMETHODPR(Field,OpModAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );

    //register comparison operators

    r = engine->RegisterObjectMethod("Field", "Field@ eq(const Field &in m)", asMETHODPR(Field,EqualTo,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ neq(const Field &in m)", asMETHODPR(Field,NotEqualTo,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShl(const Field &in m)", asMETHODPR(Field,SmallerThen,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShr(const Field &in m)", asMETHODPR(Field,LargerThen,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShlAssign(const Field &in m)", asMETHODPR(Field,SmallerEqualThen,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShrAssign(const Field &in m)", asMETHODPR(Field,LargerEqualThen,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ eq(float v)", asMETHODPR(Field,EqualTo,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ neq(float v)", asMETHODPR(Field,NotEqualTo,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShl(float v)", asMETHODPR(Field,SmallerThen,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShr(float v)", asMETHODPR(Field,LargerThen,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShlAssign(float v)", asMETHODPR(Field,SmallerEqualThen,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShrAssign(float v)", asMETHODPR(Field,LargerEqualThen,(float),Field*), asCALL_THISCALL); assert( r >= 0 );

    //register logical operators
    r = engine->RegisterObjectMethod("Field", "Field@ opCom()", asMETHODPR(Field,Negate,(),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opAnd(const Field &in m)", asMETHODPR(Field,And,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opOr(const Field &in m)", asMETHODPR(Field,Or,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opXor(const Field &in m)", asMETHODPR(Field,Xor,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opAnd(float v)", asMETHODPR(Field,And,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opOr(float v)", asMETHODPR(Field,Or,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opXor(float v)", asMETHODPR(Field,Xor,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    //register logical assignment operators
    r = engine->RegisterObjectMethod("Field", "Field& opAndAssign(const Field &in m)", asMETHODPR(Field,AndAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opOrAssign(const Field &in m)", asMETHODPR(Field,OrAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opXorAssign(const Field &in m)", asMETHODPR(Field,XorAssign,(Field *),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opAndAssign(float v)", asMETHODPR(Field,AndAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opOrAssign(float v)", asMETHODPR(Field,OrAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opXorAssign(float v)", asMETHODPR(Field,XorAssign,(float),Field*), asCALL_THISCALL); assert( r >= 0 );

    //reverse mathematical operators
    r = engine->RegisterObjectMethod("Field", "Field@ opAdd_r(float v)", asMETHODPR(Field,OpAdd_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMul_r(float v)", asMETHODPR(Field,OpMul_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opSub_r(float v)", asMETHODPR(Field,OpSub_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opDiv_r(float v)", asMETHODPR(Field,OpDiv_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opPow_r(float v)", asMETHODPR(Field,OpPow_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opMod_r(float v)", asMETHODPR(Field,OpMod_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opAnd_r(float v)", asMETHODPR(Field,And_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opOr_r(float v)", asMETHODPR(Field,Or_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opXor_r(float v)", asMETHODPR(Field,Xor_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Field", "Field@ eq_r(float v)", asMETHODPR(Field,EqualTo_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ neq_r(float v)", asMETHODPR(Field,NotEqualTo_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShl_r(float v)", asMETHODPR(Field,SmallerThen_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field@ opShr_r(float v)", asMETHODPR(Field,LargerThen_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opShlAssign_r(float v)", asMETHODPR(Field,SmallerEqualThen_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Field", "Field& opShrAssign_r(float v)", asMETHODPR(Field,LargerEqualThen_r,(float),Field*), asCALL_THISCALL); assert( r >= 0 );




    //register text conversion for debugging
    int TypeId = engine->GetTypeIdByDecl("Field");
    asITypeInfo * asi = engine->GetTypeInfoById(TypeId);
    engine->RegisterTypeDebugTextConversion(TypeId,[](void*par){return par == nullptr? "Field{null}": QString("Field{c:") + QString::number(((Field *)(par))->nrCols()) + ",r:" + QString::number(((Field *)(par))->nrRows())+ ",l:" + QString::number(((Field *)(par))->nrLevels()) +"}";});


    r = engine->RegisterGlobalFunction("Field @sin(const Field &in s)", asFUNCTION( AS_Fieldsin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @cos(const Field &in s)", asFUNCTION( AS_Fieldcos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @tan(const Field &in s)", asFUNCTION( AS_Fieldtan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @sinh(const Field &in s)", asFUNCTION( AS_Fieldsinh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @cosh(const Field &in s)", asFUNCTION( AS_Fieldcosh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @tanh(const Field &in s)", asFUNCTION( AS_Fieldtanh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @asin(const Field &in s)", asFUNCTION( AS_Fieldasin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @acos(const Field &in s)", asFUNCTION( AS_Fieldacos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @atan(const Field &in s)", asFUNCTION( AS_Fieldatan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @atan2(const Field &in s1,const Field &in s2)", asFUNCTION( AS_Fieldatan2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @abs(const Field &in s)", asFUNCTION( AS_Fieldabs),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @max(const Field &in s1,const Field &in s2)", asFUNCTION( AS_Fieldmax),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @max(float s1,const Field &in s2)", asFUNCTION( AS_Fieldmax1),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @max(const Field &in s1,float s2)", asFUNCTION( AS_Fieldmax2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @min(const Field &in s1,const Field &in s2)", asFUNCTION( AS_Fieldmin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @min(float s1,const Field &in s2)", asFUNCTION( AS_Fieldmin1),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @min(const Field &in s1,float s2)", asFUNCTION( AS_Fieldmin2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @exp(const Field &in s)", asFUNCTION( AS_Fieldexp),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @pow(const Field &in s1,const Field &in s2)", asFUNCTION( AS_Fieldpow),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @pow(const Field &in s1,float s2)", asFUNCTION( AS_Fieldpow1),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @pow(float s1,const Field &in s2)", asFUNCTION( AS_Fieldpow2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @sqrt(const Field &in s)", asFUNCTION( AS_Fieldsqrt),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @log(const Field &in s)", asFUNCTION( AS_Fieldlog),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @log10(const Field &in s)", asFUNCTION( AS_Fieldlog10),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @ceil(const Field &in s)", asFUNCTION( AS_Fieldceil),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @floor(const Field &in s)", asFUNCTION( AS_Fieldfloor),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @fraction(const Field &in s)", asFUNCTION( AS_Fieldfraction),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Field @SetMV(const Field &in s)", asFUNCTION( AS_Fieldsetmv),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Field @IsMV(const Field &in s)", asFUNCTION( AS_Fieldismv),  asCALL_CDECL); assert( r >= 0 );


    r = engine->RegisterGlobalSTDFunction("Field @FieldMaskDem(Field &in f, const Map &in elevation, float value)", GetFuncConvert( AS_FieldMaskDem));


}



#endif // FIELDSCRIPTING_H
