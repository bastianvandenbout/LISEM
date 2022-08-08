#pragma once

#include <angelscript.h>
#include "geo/raster/map.h"
#include "raster/rastermath.h"
#include "raster/rasterconditional.h"
#include "raster/rasterspread.h"
#include "raster/rasterreduce.h"
#include "raster/rastertrigonometric.h"
#include "raster/rasterlimit.h"
#include "raster/rasternetwork.h"
#include "iogdal.h"
#include "scriptmanager.h"
#include "raster/rasterderivative.h"
#include "raster/rasterclassified.h"
#include "raster/rasterinterpolation.h"
#include "raster/rasteralgorithms.h"
#include "raster/rasterconvolve.h"
#include "geo/raster/rasteralgebra.h"
#include "raster/rasterconstructors.h"
#include "matrixtable.h"
#include "raster/rastertable.h"
#include "raster/rasterflow.h"
#include "raster/rastershading.h"
#include "raster/rasterartificialflow.h"
#include "raster/rasterstatistics.h"
#include "raster/rasterrandom.h"
#include "raster/resterboussinesq.h"
#include "raster/rasterfourier.h"
#include "raster/rasterkinematic.h"
#include "raster/rasterdeconvolve.h"
#include "raster/rasterwarp.h"
#include "lsmscriptengine.h"
#include "raster/rasterpainting.h"
#include "raster/rasterincompressibleflow.h"
#include "scriptarrayhelpers.h"
#include "stat/mlsupersampling.h"
#include "raster/rastermixtureflow.h"
#include "raster/rasterchannel.h"
#include "raster/rasterfiberbundle.h"
#include "raster/rasterdiffuse.h"
#include "raster/rasterdither.h"
#include "raster/rasterfastflow.h"
#include "raster/rasterlaplace.h"
#include "raster/rasterpotentialflow.h"
#include "raster/rasterfastwave.h"
#include "raster/rastersuperfastflow.h"
#include "omp.h"

inline cTMap* AS_AssignArray(cTMap * m, CScriptArray * array)
{
    QList<cTMap *> list = array->ToQList<cTMap*>();
    if(m->AS_writeonassign)
    {
        m->AS_writefunclist(list,m->AS_FileName);

    }else {

        if(list.length() == 1)
        {
            m->Assign(list.at(0));
        }else {
            LISEMS_ERROR("Can not assign list of maps (length " + QString::number(list.length()) + ") to a single map");
            throw 1;
        }
    }

    return m;

}

inline CScriptArray * AS_GetAll(cTMap*m)
{
    std::cout << "Get All " << m->AS_IsBandLoadable << " "<< m->AS_TotalNBands << std::endl;

    if(m->AS_IsBandLoadable)
    {
        QList<cTMap *> ret = m->AS_readallfunc(m->AS_BandFilePath);

        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        asIScriptEngine *engine = ctx->GetEngine();

        // TODO: This should only be done once
        // TODO: This assumes that CScriptArray was already registered
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);
        array->Resize(ret.length());
        for(int i = 0; i < ret.length(); i++)
        {
            array->SetValue(i,ret.at(i),false);
        }
        return array;


    }else {

        cTMap * ret = m->GetCopy();

        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        asIScriptEngine *engine = ctx->GetEngine();

        // TODO: This should only be done once
        // TODO: This assumes that CScriptArray was already registered
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);
        array->Resize(1);
        array->SetValue(0,ret,false);
        return array;


    }


}




inline void RegisterMapToScriptEngine(LSMScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("Map",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()
    if(r < 0){LISEM_DEBUG("error in registering scripting: class Map");};

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C0()",asFUNCTIONPR(MapFactory,(),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C1(int rows, int columns)",asFUNCTIONPR(MapFactory,(int,int),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C2(int rows, int columns, float dx)",asFUNCTIONPR(MapFactory,(int,int,float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C3(int rows, int columns, float dx, float value)",asFUNCTIONPR(MapFactory,(int, int, float,float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C4(int rows, int columns, float dx, float value, float north, float west)",asFUNCTIONPR(MapFactory,(int,int,float,float,float,float),cTMap *),asCALL_CDECL);  assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C7(int rows, int columns, float dx, float dy,float value, float north, float west)",asFUNCTIONPR(MapFactory,(int,int,float,float,float,float,float),cTMap *),asCALL_CDECL);  assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C8(int rows, int columns, float dx, float dy,float value, float north, float west, string proj)",asFUNCTIONPR(MapFactory,(int,int,float,float,float,float,float, QString),cTMap *),asCALL_CDECL);  assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C5(int)",asFUNCTIONPR(MapFactory,(int),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C6(float)",asFUNCTIONPR(MapFactory,(float),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C6(Region r, int rows, int cols)",asFUNCTIONPR(MapFactory,(BoundingBox,int,int),cTMap *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map@ C6(Region r, int rows, int cols, GeoProjection &in p)",asFUNCTIONPR(MapFactory,(BoundingBox, int,int,GeoProjection*),cTMap *),asCALL_CDECL); assert( r >= 0 );
    //r = engine->RegisterObjectBehaviour("Map",asBEHAVE_FACTORY,"Map& C7(int rows, int columns, BoundingBox &in extent)",asFUNCTIONPR(MapFactory,(float),cTMap *),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_ADDREF,"void f()",asMETHOD(cTMap,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Map",asBEHAVE_RELEASE,"void f()",asMETHOD(cTMap,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );


    //register member functions
    r = engine->RegisterObjectMethod("Map", "int Rows()", asMETHOD(cTMap,Rows), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "int Cols()", asMETHOD(cTMap,Cols), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "double CellSizeX()", asMETHOD(cTMap,cellSizeX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "double CellSizeY()", asMETHOD(cTMap,cellSizeY), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Map", "float North()", asMETHOD(cTMap,North), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "float West()", asMETHOD(cTMap,West), asCALL_THISCALL); assert( r >= 0 );

    //referencing options
    r = engine->RegisterGlobalFunction("Map &SetCRS(const Map &in s1,GeoProjection &in p)", asFUNCTIONPR( AS_SetCRS,(cTMap*,GeoProjection*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("GeoProjection& GetCRS(const Map &in m)", asFUNCTIONPR( AS_GetCRS,(cTMap*),GeoProjection*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Region GetRegion(const Map &in m)", asFUNCTIONPR( AS_GetRegion,(cTMap*),BoundingBox),  asCALL_CDECL); assert( r >= 0 );



    //special operators

    r = engine->RegisterObjectMethod("Map", "Map @opIndex(int)", asMETHOD(cTMap,GetBand), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map &opAssign(array<Map> &in maps)", asFUNCTIONPR(AS_AssignArray,(cTMap*,CScriptArray*),cTMap *), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "array<Map> @AllBands()", asFUNCTIONPR(AS_GetAll,(cTMap*),CScriptArray*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Map", "float& opIndex(int, int)", asMETHOD(cTMap,Value), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAssign(const Map &in m)", asMETHODPR(cTMap,Assign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAssign(float v)", asMETHODPR(cTMap,Assign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "Map@ opNeg()", asMETHODPR(cTMap,OpNeg,(void),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //mathematical operators

    r = engine->RegisterObjectMethod("Map", "Map@ opAdd(const Map &in m)", asMETHODPR(cTMap,OpAdd,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMul(const Map &in m)", asMETHODPR(cTMap,OpMul,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opSub(const Map &in m)", asMETHODPR(cTMap,OpSub,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opDiv(const Map &in m)", asMETHODPR(cTMap,OpDiv,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opPow(const Map &in m)", asMETHODPR(cTMap,OpPow,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMod(const Map &in m)", asMETHODPR(cTMap,OpMod,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opAdd(float v)", asMETHODPR(cTMap,OpAdd,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMul(float v)", asMETHODPR(cTMap,OpMul,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opSub(float v)", asMETHODPR(cTMap,OpSub,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opDiv(float v)", asMETHODPR(cTMap,OpDiv,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opPow(float v)", asMETHODPR(cTMap,OpPow,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMod(float v)", asMETHODPR(cTMap,OpMod,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //assignment operators

    r = engine->RegisterObjectMethod("Map", "Map& opAddAssign(const Map &in m)", asMETHODPR(cTMap,OpAddAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMulAssign(const Map &in m)", asMETHODPR(cTMap,OpMulAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSubAssign(const Map &in m)", asMETHODPR(cTMap,OpSubAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDivAssign(const Map &in m)", asMETHODPR(cTMap,OpDivAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPowAssign(const Map &in m)", asMETHODPR(cTMap,OpPowAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opModAssign(const Map &in m)", asMETHODPR(cTMap,OpModAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "Map& opAddAssign(float v)", asMETHODPR(cTMap,OpAddAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opMulAssign(float v)", asMETHODPR(cTMap,OpMulAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opSubAssign(float v)", asMETHODPR(cTMap,OpSubAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opDivAssign(float v)", asMETHODPR(cTMap,OpDivAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opPowAssign(float v)", asMETHODPR(cTMap,OpPowAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opModAssign(float v)", asMETHODPR(cTMap,OpModAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //register comparison operators

    r = engine->RegisterObjectMethod("Map", "Map@ eq(const Map &in m)", asMETHODPR(cTMap,EqualTo,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ neq(const Map &in m)", asMETHODPR(cTMap,NotEqualTo,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShl(const Map &in m)", asMETHODPR(cTMap,SmallerThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShr(const Map &in m)", asMETHODPR(cTMap,LargerThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShlAssign(const Map &in m)", asMETHODPR(cTMap,SmallerEqualThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShrAssign(const Map &in m)", asMETHODPR(cTMap,LargerEqualThen,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ eq(float v)", asMETHODPR(cTMap,EqualTo,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ neq(float v)", asMETHODPR(cTMap,NotEqualTo,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShl(float v)", asMETHODPR(cTMap,SmallerThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShr(float v)", asMETHODPR(cTMap,LargerThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShlAssign(float v)", asMETHODPR(cTMap,SmallerEqualThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShrAssign(float v)", asMETHODPR(cTMap,LargerEqualThen,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //register logical operators
    r = engine->RegisterObjectMethod("Map", "Map@ opCom()", asMETHODPR(cTMap,Negate,(),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opAnd(const Map &in m)", asMETHODPR(cTMap,And,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opOr(const Map &in m)", asMETHODPR(cTMap,Or,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opXor(const Map &in m)", asMETHODPR(cTMap,Xor,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opAnd(float v)", asMETHODPR(cTMap,And,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opOr(float v)", asMETHODPR(cTMap,Or,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opXor(float v)", asMETHODPR(cTMap,Xor,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    //register logical assignment operators
    r = engine->RegisterObjectMethod("Map", "Map& opAndAssign(const Map &in m)", asMETHODPR(cTMap,AndAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOrAssign(const Map &in m)", asMETHODPR(cTMap,OrAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXorAssign(const Map &in m)", asMETHODPR(cTMap,XorAssign,(cTMap *),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opAndAssign(float v)", asMETHODPR(cTMap,AndAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opOrAssign(float v)", asMETHODPR(cTMap,OrAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opXorAssign(float v)", asMETHODPR(cTMap,XorAssign,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    //reverse mathematical operators
    r = engine->RegisterObjectMethod("Map", "Map@ opAdd_r(float v)", asMETHODPR(cTMap,OpAdd_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMul_r(float v)", asMETHODPR(cTMap,OpMul_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opSub_r(float v)", asMETHODPR(cTMap,OpSub_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opDiv_r(float v)", asMETHODPR(cTMap,OpDiv_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opPow_r(float v)", asMETHODPR(cTMap,OpPow_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opMod_r(float v)", asMETHODPR(cTMap,OpMod_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opAnd_r(float v)", asMETHODPR(cTMap,And_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opOr_r(float v)", asMETHODPR(cTMap,Or_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opXor_r(float v)", asMETHODPR(cTMap,Xor_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Map", "Map@ eq_r(float v)", asMETHODPR(cTMap,EqualTo_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ neq_r(float v)", asMETHODPR(cTMap,NotEqualTo_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShl_r(float v)", asMETHODPR(cTMap,SmallerThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map@ opShr_r(float v)", asMETHODPR(cTMap,LargerThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShlAssign_r(float v)", asMETHODPR(cTMap,SmallerEqualThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Map", "Map& opShrAssign_r(float v)", asMETHODPR(cTMap,LargerEqualThen_r,(float),cTMap*), asCALL_THISCALL); assert( r >= 0 );


    //register text conversion for debugging
    int TypeId = engine->GetTypeIdByDecl("Map");
    asITypeInfo * asi = engine->GetTypeInfoById(TypeId);
    engine->RegisterTypeDebugTextConversion(TypeId,[](void*par){return par == nullptr? "Map{null}": QString("Map{c:") + QString::number(((cTMap *)(par))->nrCols()) + ",r:" + QString::number(((cTMap *)(par))->nrRows()) +"}";});



}

#include "stack/threadpool.h"

static inline void AS_SetNumThreads(int i)
{
    if(!(i < 1024 && i > 0))
    {
        LISEMS_ERROR("Can not set the number of threads to be" + QString::number(i));
        throw 1;
    }
    try
    {
        pool.reset(i);
        omp_set_num_threads(i);

    }catch(...)
    {
        LISEMS_ERROR("Could not set the number of threads to be" + QString::number(i));
        throw 1;
    }
}

inline void RegisterMapMathToScriptEngine(asIScriptEngine *engine)
{

    //register mathematical functions for maps

    engine->RegisterGlobalFunction("void SetNumThreads(int i)", asFUNCTION( AS_SetNumThreads),  asCALL_CDECL); assert( r >= 0 );


    int r = engine->RegisterGlobalFunction("Map @sin(const Map &in s)", asFUNCTION( AS_Mapsin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @cos(const Map &in s)", asFUNCTION( AS_Mapcos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @tan(const Map &in s)", asFUNCTION( AS_Maptan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @sinh(const Map &in s)", asFUNCTION( AS_Mapsinh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @cosh(const Map &in s)", asFUNCTION( AS_Mapcosh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @tanh(const Map &in s)", asFUNCTION( AS_Maptanh),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @asin(const Map &in s)", asFUNCTION( AS_Mapasin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @acos(const Map &in s)", asFUNCTION( AS_Mapacos),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @atan(const Map &in s)", asFUNCTION( AS_Mapatan),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @atan2(const Map &in s1,const Map &in s2)", asFUNCTION( AS_Mapatan2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @abs(const Map &in s)", asFUNCTION( AS_Mapabs),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @max(const Map &in s1,const Map &in s2)", asFUNCTION( AS_Mapmax),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @min(const Map &in s1,const Map &in s2)", asFUNCTION( AS_Mapmin),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @exp(const Map &in s)", asFUNCTION( AS_Mapexp),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @pow(const Map &in s1,const Map &in s2)", asFUNCTION( AS_Mappow),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @sqrt(const Map &in s)", asFUNCTION( AS_Mapsqrt),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @log(const Map &in s)", asFUNCTION( AS_Maplog),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @log10(const Map &in s)", asFUNCTION( AS_Maplog10),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ceil(const Map &in s)", asFUNCTION( AS_Mapceil),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @floor(const Map &in s)", asFUNCTION( AS_Mapfloor),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @fraction(const Map &in s)", asFUNCTION( AS_Mapfraction),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("void as_mtest(int i)", asFUNCTION( AS_MapTest),  asCALL_CDECL); assert( r >= 0 );

    //register functions related to missing values

    r = engine->RegisterGlobalFunction("Map @Cover(const Map &in s,const Map &in s2)", asFUNCTIONPR( AS_Cover,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(const Map &in s,float s2)", asFUNCTIONPR( AS_Cover,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(float s,const Map &in s2)", asFUNCTIONPR( AS_Cover,(float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Cover(float s, float s2)", asFUNCTIONPR( AS_Cover,(float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SetMV(const Map &in s,const Map &in s2)", asFUNCTIONPR( AS_SetMV,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SetMV(const Map &in s,float s2)", asFUNCTIONPR( AS_SetMV,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @IsMV(const Map &in s)", asFUNCTIONPR( AS_IsMV,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float GetMVVal()", asFUNCTION( AS_GetMVVal),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float IsValMV(float s)", asFUNCTION( AS_IsValMV),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @MapEdge(const Map &in m)", asFUNCTION(AS_Edge), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @MapEdgeLeft(const Map &in m)", asFUNCTION(AS_EdgeLeft), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @MapEdgeRight(const Map &in m)", asFUNCTION(AS_EdgeRight), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @MapEdgeTop(const Map &in m)", asFUNCTION(AS_EdgeTop), asCALL_CDECL);
    r = engine->RegisterGlobalFunction("Map @MapEdgeBottom(const Map &in m)", asFUNCTION(AS_EdgeBottom), asCALL_CDECL);


    //register map averages (normal and reductive->meaning not a map but single float is returned)
    r = engine->RegisterGlobalFunction("Map @MapTotal(const Map &in s)", asFUNCTION( AS_MapTotal),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapTotalRed(const Map &in s)", asFUNCTION( AS_MapTotalRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapAverage(const Map &in s)", asFUNCTION( AS_MapAverage),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapAverageRed(const Map &in s)", asFUNCTION( AS_MapAverageRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapMinimum(const Map &in s)", asFUNCTION( AS_MapMinimum),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapMinimumRed(const Map &in s)", asFUNCTION( AS_MapMinimumRed),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapMaximum(const Map &in s)", asFUNCTION( AS_MapMaximum),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("float MapMaximumRed(const Map &in s)", asFUNCTION( AS_MapMaximumRed),  asCALL_CDECL); assert( r >= 0 );


    //register conditional functions
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,const Map &in m2,const Map &in m3)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,const Map &in m2,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,float,const Map &in m3)", asFUNCTIONPR( AS_MapIf,(cTMap *,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,const Map &in m2,const Map &in m3)", asFUNCTIONPR( AS_MapIf,(float,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float,const Map &in m3)", asFUNCTIONPR( AS_MapIf,(float,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,float,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float,float)", asFUNCTIONPR( AS_MapIf,(float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,const Map &in m2)", asFUNCTIONPR( AS_MapIf,(cTMap *,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float ,const Map &in m2)", asFUNCTIONPR( AS_MapIf,(float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(const Map &in m1,float)", asFUNCTIONPR( AS_MapIf,(cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @MapIf(float,float)", asFUNCTIONPR( AS_MapIf,(float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );




}

inline void RegisterMapAlgorithmsToScriptEngine(LSMScriptEngine *engine)
{

    //register algorithm for maps
    //spread algorithms
    int r = engine->RegisterGlobalFunction("Map @Spread(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_Spread,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadMT(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadMT,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadMD(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadMD,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadFlowField(const Map &in source,const Map &in fx_norm,const Map &in fy_norm, int iter_max = 0)", asFUNCTIONPR( AS_SpreadFlow,(cTMap*,cTMap*,cTMap *, int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadFlowFieldMD(const Map &in source,const Map &in fx_norm1,const Map &in fx_norm2,const Map &in fy_norm1,const Map &in fy_norm2, int iter_max = 0)", asFUNCTIONPR( AS_SpreadFlowMD,(cTMap*,cTMap*,cTMap*,cTMap *,cTMap *, int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadFlowFieldMD2(const Map &in source,const Map &in fx_norm1,const Map &in fx_norm2,const Map &in fy_norm1,const Map &in fy_norm2, int iter_max = 0)", asFUNCTIONPR( AS_SpreadFlowMD2,(cTMap*,cTMap*,cTMap*,cTMap *,cTMap *, int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadFlowFieldMDCP(const Map &in source,const Map &in fx_norm1,const Map &in fx_norm2,const Map &in fy_norm1,const Map &in fy_norm2, const Map &in S, const Map &in scale, float power, int iter_max = 0)", asFUNCTIONPR( AS_SpreadFlowMDCP,(cTMap*,cTMap*,cTMap*,cTMap *,cTMap *,cTMap *,cTMap * ,float, int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Depressions(const Map &in DEM,int iter_max = 0)", asFUNCTIONPR( AS_SpreadDepressionFind,(cTMap*,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @FlowSuperFast(const Map &in DEM, const Map &in N, const Map &in Rain, float duration)", asFUNCTIONPR( AS_FlowSuperFast,(cTMap*,cTMap *,cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @SpreadDirectionalAbsMax(const Map &in s1,const Map &in initialfriction,const Map &in frictionx, const Map &in  frictiony, float delta = 0.0001)", asFUNCTIONPR( AS_SpreadDirectionalAbsMax,(cTMap*,cTMap*,cTMap *,cTMap*, float ),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadDirectionalAbsMaxMD(const Map &in s1,const Map &in initialfriction,const Map &in frictionx1,const Map &in frictionx2, const Map &in  frictiony1,const Map &in  frictiony2, float delta = 0.0001)", asFUNCTIONPR( AS_SpreadDirectionalAbsMaxMD,(cTMap*,cTMap*,cTMap *,cTMap *, cTMap *,cTMap*, float ),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(const Map &in s1,float s2,const Map &in s3)", asFUNCTIONPR( AS_Spread,(cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(const Map &in s1,const Map &in s2,float s3)", asFUNCTIONPR( AS_Spread,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Spread(const Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_Spread,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(const Map &in s1,float s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(const Map &in s1,const Map &in s2,float s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadZone(const Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_SpreadZone,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @SlopeLength(const Map &in s1,const Map &in s2,const Map &in s3,bool two_way = true)", asFUNCTIONPR( AS_SlopeLength,(cTMap*,cTMap*,cTMap *,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SlopeLength(const Map &in s1,float s2,const Map &in s3,bool two_way = true)", asFUNCTIONPR( AS_SlopeLength,(cTMap*,float,cTMap *,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SlopeLength(const Map &in s1,const Map &in s2,float s3,bool two_way = true)", asFUNCTIONPR( AS_SlopeLength,(cTMap*,cTMap*,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SlopeLength(const Map &in s1,float s2,float s3,bool two_way = true)", asFUNCTIONPR( AS_SlopeLength,(cTMap*,float,float,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @ToLDD(const Map &in lddvals)", asFUNCTIONPR(  AS_ToLDD,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @SpreadNetwork(const Map &in s1,const Map &in ldd,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadLDD,(cTMap*,cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetwork(const Map &in s1,const Map &in ldd,float s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadLDD,(cTMap*,cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetwork(const Map &in s1,const Map &in ldd,const Map &in s2,float s3)", asFUNCTIONPR( AS_SpreadLDD,(cTMap*,cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetwork(const Map &in s1,const Map &in ldd,float s2,float s3)", asFUNCTIONPR( AS_SpreadLDD,(cTMap*,cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetworkZone(const Map &in s1,const Map &in ldd,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadLDDZone,(cTMap*,cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetworkZone(const Map &in s1,const Map &in ldd,float s2,const Map &in s3)", asFUNCTIONPR( AS_SpreadLDDZone,(cTMap*,cTMap*,float,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetworkZone(const Map &in s1,const Map &in ldd,const Map &in s2,float s3)", asFUNCTIONPR( AS_SpreadLDDZone,(cTMap*,cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SpreadNetworkZone(const Map &in s1,const Map &in ldd,float s2,float s3)", asFUNCTIONPR( AS_SpreadLDDZone,(cTMap*,cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @ViewShed(const Map &in s1,float x,float y, float height=0.0)", asFUNCTIONPR( AS_ViewShed,(cTMap*,float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ViewAngle(const Map &in s1,float x,float y, float height = 0.0)", asFUNCTIONPR( AS_ViewAngle,(cTMap*,float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ViewCriticalAngle(const Map &in s1,float x,float y, float height = 0.0)", asFUNCTIONPR( AS_ViewCriticalAngle,(cTMap*,float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //engine->RegisterGlobalFunction("Map &ViewShed(const Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_SlopeLength,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );


    //register raster derivative functions
    r = engine->RegisterGlobalFunction("Map @Slope(const Map &in s1)", asFUNCTION( AS_Slope),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Aspect(const Map &in s1)", asFUNCTION( AS_Aspect),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientX(const Map &in s1)", asFUNCTION( AS_SlopeX),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientX1(const Map &in s1)", asFUNCTION( AS_SlopeX1),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientX2(const Map &in s1)", asFUNCTION( AS_SlopeX2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientXX(const Map &in s1)", asFUNCTION( AS_SlopeDx2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientY(const Map &in s1)", asFUNCTION( AS_SlopeY),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientY1(const Map &in s1)", asFUNCTION( AS_SlopeY1),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientY2(const Map &in s1)", asFUNCTION( AS_SlopeY2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GradientYY(const Map &in s1)", asFUNCTION( AS_SlopeDy2),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @CurvPlanar(const Map &in s1)", asFUNCTION( AS_PlanarCurvature),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @CurvProfile(const Map &in s1)", asFUNCTION( AS_ProfileCurvature),  asCALL_CDECL); assert( r >= 0 );

    //register ldd related functions
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,const Map &in s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap *,cTMap *,cTMap *,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,float s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,float s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,const Map &in s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,float s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,float s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,float s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,float,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,const Map &in s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,float s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,const Map &in s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,float s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,const Map &in s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap*,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,float s2,const Map &in s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,float,cTMap*,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,float s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,float,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1,const Map &in s2,const Map &in s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*,cTMap*,cTMap*,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetwork(const Map &in s1, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetwork,(cTMap*, bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register ldd related functions
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,const Map &in s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,cTMap *,cTMap *,cTMap *,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,float s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,float,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,float s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,float,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,const Map &in s3,float s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,cTMap*,float,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,float s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,float,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,float s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,float,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,float s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,float,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,const Map &in s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,cTMap*,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,float s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,float,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,const Map &in s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,cTMap*,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,float s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,float,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,const Map &in s3,const Map &in s4,float s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,cTMap*,cTMap*,float,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,float s2,const Map &in s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,float,cTMap*,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,float s3,const Map &in s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,float,cTMap*,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1,const Map &in s2,const Map &in s3,float s4,const Map &in s5,bool lddin = false, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*,cTMap*,cTMap*,float,cTMap*,bool,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @DrainageNetworkDEM(const Map &in s1, bool use4 = false)", asFUNCTIONPR( AS_DrainageNetworkDEM,(cTMap*, bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );



    //register accuflux function

    r = engine->RegisterGlobalFunction("Map @Accuflux(const Map &in s1)", asFUNCTIONPR( AS_Accuflux,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Accuflux(const Map &in s1, float)", asFUNCTIONPR( AS_Accuflux,(cTMap*, float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Accuflux(const Map &in s1, const Map &in s2)", asFUNCTIONPR( AS_Accuflux,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @AccufluxLim(const Map &in s1, const Map &in s2)", asFUNCTIONPR( AS_AccufluxThreshold,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AccufluxLim(const Map &in s1, float precip, const Map &in s2)", asFUNCTIONPR( AS_AccufluxThreshold,(cTMap*, float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AccufluxLim(const Map &in s1, const Map &in precip, const Map &in s3)", asFUNCTIONPR( AS_AccufluxThreshold,(cTMap*,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );


    r = engine->RegisterGlobalFunction("Map @DownStream(const Map &in LDD, const Map &in value)", asFUNCTIONPR( AS_DownStream,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @UpStream(const Map &in LDD, const Map &in value)", asFUNCTIONPR( AS_UpStream,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @DrainageMonotonicDownstream(const Map &in ldd, const Map &in value)", asFUNCTIONPR( AS_DrainageMonotonicDownstream,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //Outlet and Catchments stuff

    r = engine->RegisterGlobalFunction("Map @Outlets(const Map &in s1)", asFUNCTIONPR( AS_Outlets,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Catchments(const Map &in s1)", asFUNCTIONPR( AS_Catchments,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Catchments(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_Catchments,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //streamorder
    r = engine->RegisterGlobalFunction("Map @StreamOrder(const Map &in s1)", asFUNCTIONPR( AS_StreamOrder,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //area reduction
    r = engine->RegisterGlobalFunction("Map @AreaArea(const Map &in s1)", asFUNCTIONPR( AS_MapAreaArea,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaAverage(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_MapAreaAverage,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaTotal(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_MapAreaTotal,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaMinimum(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_MapAreaMinimum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @AreaMaximum(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_MapAreaMaximum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    //inversedistance
    r = engine->RegisterGlobalFunction("Map @InverseDistance(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_InverseDistance,(cTMap*,cTMap*,cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @InverseDistance(const Map &in s1,const Map &in s2,float)", asFUNCTIONPR( AS_InverseDistance,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //coordinate related functions
    r = engine->RegisterGlobalFunction("Map @SpreadOne(const Map &in s1)", asFUNCTIONPR( AS_SpreadOne,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @ShiftMV(const Map &in s1,int,int)", asFUNCTIONPR( AS_ShiftMV,(cTMap*,int,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Shift(const Map &in s1,int,int,float)", asFUNCTIONPR( AS_Shift,(cTMap*,int,int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Shift(const Map &in s1,int,int)", asFUNCTIONPR( AS_Shift,(cTMap*,int,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @XIndex(const Map &in s1)", asFUNCTIONPR( AS_XIndex,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @YIndex(const Map &in s1)", asFUNCTIONPR( AS_YIndex,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @XCoord(const Map &in s1)", asFUNCTIONPR( AS_XCoord,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @YCoord(const Map &in s1)", asFUNCTIONPR( AS_YCoord,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Clone(const Map &in s1,float)", asFUNCTIONPR( AS_Clone,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    //register random number generator

    r = engine->RegisterGlobalFunction("Map @RasterDeconvolve(const Map &in s1,int iterations, double sigma)", asFUNCTIONPR( AS_RasterDeconvolve,(cTMap*,int,double),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //gaussian influence
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(const Map &in s1,const Map &in s2,const Map &in s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(const Map &in s1,float,const Map &in s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,float,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(const Map &in s1,const Map &in s2,float)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @GaussianDistribute(const Map &in s1,float s2,float s3)", asFUNCTIONPR( AS_GaussianInfluence,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register clump, uniqueID etc..
    r = engine->RegisterGlobalFunction("Map @Clump(const Map &in s1, float threshold = 0.0, bool adaptive = false)", asFUNCTIONPR( AS_Clump,(cTMap*, float, bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @UniqueCellID(const Map &in s1)", asFUNCTIONPR( AS_UniqueID,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    //register windowaverage, windowminimum, window ...


    r = engine->RegisterGlobalFunction("Map @WindowAverage(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowAverage,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowAverage(const Map &in s1,float)", asFUNCTIONPR( AS_WindowAverage,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowVariance(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowVariance,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowVariance(const Map &in s1,float)", asFUNCTIONPR( AS_WindowVariance,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowMajority(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowMajority,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMajority(const Map &in s1,float)", asFUNCTIONPR( AS_WindowMajority,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowDiversity(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowDiversity,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowDiversity(const Map &in s1,float)", asFUNCTIONPR( AS_WindowDiversity,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowTotal(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowTotal,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowTotal(const Map &in s1,float)", asFUNCTIONPR( AS_WindowTotal,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowMaximum(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowMaximum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMaximum(const Map &in s1,float)", asFUNCTIONPR( AS_WindowMaximum,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowMinimum(const Map &in s1,const Map &in s2)", asFUNCTIONPR( AS_WindowMinimum,(cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @WindowMinimum(const Map &in s1,float)", asFUNCTIONPR( AS_WindowMinimum,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @Erode(const Map &in s1,int)", asFUNCTIONPR( AS_Erode,(cTMap*,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @Dilute(const Map &in s1,int)", asFUNCTIONPR( AS_Dilute,(cTMap*,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @WindowPointShift(const Map &in mask,const Map &in points, float window)", asFUNCTIONPR( AS_RasterShiftToOther,(cTMap*,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @RasterCut(const Map &in val, int r0, int c0, int rows, int cols)", asFUNCTIONPR( AS_RasterSubSection,(cTMap*,int,int,int,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterCut(const Map &in val,Region r)", asFUNCTIONPR( AS_RasterSubSection,(cTMap*,BoundingBox),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterCut(const array<Map> &in val, int r0, int c0, int rows, int cols)", GetFuncConvert(AS_RasterSublSection)); assert( r >= 0 );
    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterCut(const array<Map> &in val,Region r)", GetFuncConvert(AS_RasterSublSection)); assert( r >= 0 );


    //table interactions

    r = engine->RegisterGlobalFunction("Table @RasterConfusionMatrix(const Map &in real, Map&model)", asFUNCTIONPR( AS_RasterConfusionTable,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterFromTableValues(const Table &in table,Map &RowValue, Map &ColumnValue)", asFUNCTIONPR( AS_RasterFromTableHeader,(MatrixTable *,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterFromTable(const Table &in table,const Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterFromTable,(MatrixTable *,cTMap*,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterFromTable(const Table &in table,Map &Row, int Column)", asFUNCTIONPR( AS_RasterFromTable,(MatrixTable *,cTMap*,int),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @RasterFromTable(const Table &in table,int Row, const Map &Column)", asFUNCTIONPR( AS_RasterFromTable,(MatrixTable *,int,cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterClasses(Map &m)", asFUNCTIONPR( AS_RasterTableClasses,(cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableAverage(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableAverage,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableMinimum(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableMinimum,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableTotal(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableTotal,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Table @RasterTableMaximum(Map &Row, Map &Column)", asFUNCTIONPR( AS_RasterTableMaximum,(cTMap*,cTMap*),MatrixTable *),  asCALL_CDECL); assert( r >= 0 );


    //flow algorithms
    r = engine->RegisterGlobalFunction("Map @FlowDiffusiveMax(const Map &in DEM,const Map &in H,int iter = 10,float courant = 0.1)", asFUNCTIONPR( AS_DiffusiveMaxWave,(cTMap*,cTMap*,int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @FlowDiffusiveMaxCG(const Map &in DEM,const Map &in H, const Map &in DEMSlope, float slopefactor, int iter = 10,float courant = 0.1)", asFUNCTIONPR( AS_DiffusiveMaxWaveCG,(cTMap*,cTMap*,cTMap*,float, int,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowDynamic(const Map &in DEM, const Map &in N, const Map &in H, const Map &in VX, const Map &in VY, float dt, float courant = 0.1, float dt_min = 0.0)",GetFuncConvert(AS_DynamicWave));
    r = engine->RegisterGlobalSTDFunction("Map @FlowDynamicSuspendedTransport(const Map &in S, const Map &in DEM, const Map &in N, const Map &in H, const Map &in VX, const Map &in VY, float dt, float courant = 0.1)",GetFuncConvert(AS_DynamicWaveSuspendedTransport));
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowDynamicRigid(const Map &in DEM, const Map &in N, const Map &in H, const Map &in VX, const Map &in VY, const Map &in BlockX,const Map &in BlockY, const Map &in BlockFx, const Map &in BlockFy, const Map &in Blockvelx, const Map &in Blockvely, const Map &in HCorrect, float dt, float courant = 0.1)",GetFuncConvert(AS_DynamicWaveRigid));
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowDebris(const Map &in DEM, const Map &in N, const Map &in H, const Map &in VX, const Map &in VY, const Map &in HS, const Map &in VXS, const Map&in VYS, const Map &in IFA, const Map &in RS, const Map&in D, float dragmult, float dt, float courant = 0.1)",GetFuncConvert(AS_DebrisWave));


    r = engine->RegisterGlobalSTDFunction("Map @EikonalSolve(const Map &in Mask, const Map &in C, int iter_max = -1)",GetFuncConvert(AS_EikonalSolve));

    r = engine->RegisterGlobalSTDFunction("Map @EnergySpread(const Map&in Source, const Map&in ux1, const Map&in ux2, const Map&in uy1, const Map&in uy2,int iter_max)",GetFuncConvert(AS_WaveEnergySpread));
    r = engine->RegisterGlobalSTDFunction("array<Map> @WaveEnergySteadyState(const Map &in E_Source, int iter_max, float fac_lat_intert = 0.25)",GetFuncConvert(AS_WaveEnergySteadyState));

    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowBoussinesq(const Map &in DEM, const Map &in N, const Map &in H, const Map &in VX, const Map &in VY, float dt, float courant = 0.1)", GetFuncConvert(AS_BoussinesqWave));
    r = engine->RegisterGlobalFunction("Map @FlowSteady(const Map &in DEM, const Map &in N, const Map &in Source, const Map &in Sink, int iter = 100, float courant = 0.1)", asFUNCTIONPR(    AS_SteadyStateWave,(cTMap *,cTMap *,cTMap *,cTMap *,int,float),cTMap *),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowTsunami(const Map &in DEM, const Map &in H, const Map &in U, const Map &in V, const Map & in N, float timestep = 0.1)", GetFuncConvert(AS_TsunamiFlow));
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowNavierStokes(const Map &in DENS, const Map &in U, const Map &in V, float visc, float diff, float timestep = 0.1, float courant = 0.25)", GetFuncConvert(AS_NavierStokesWave));
    r = engine->RegisterGlobalFunction("Map @FlowKinematic(const Map &in LDD, const Map &in Slope, const Map &in Width, const Map &in N, const Map &in H, float dt)", asFUNCTIONPR(    AS_FlowKinematic,(cTMap *,cTMap *,cTMap *,cTMap *,cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("Map @FlowKinematicSuspendedTransport(const Map &in LDD, const Map &in Slope, const Map &in Width, const Map &in N, const Map &in H, const Map&in S, float dt)", asFUNCTIONPR(    AS_FlowKinematicSubstance,(cTMap *,cTMap *,cTMap *,cTMap *,cTMap*,cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );;

    r = engine->RegisterGlobalSTDFunction("array<Map> @VoxelMaskDem(array<Map> &in state, float zmin, float zmax, const Map &in elevation)", GetFuncConvert( AS_VoxelMaskDem));
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowNavierStokes3D(array<Map> &in state, float zmin, float zmax, float visc, float diff, float timestep = 0.1, float courant = 0.25)",GetFuncConvert( AS_NavierStokesWave3D));

    r = engine->RegisterGlobalSTDFunction("Map @ SimilaritySpread(const Map&in val1, const Map&in val2, const Map&in prop, int iter_max = 100, float weightpower = 1.0, float wself = 4.0, float coeff = 0.5, bool w_original = true, bool only_larger = false)",GetFuncConvert(AS_SimilaritySpread));
    r = engine->RegisterGlobalSTDFunction("array<Map> @Accuflux2D(const Map &in DEM, const Map &in Source, const Map &in FlowSource, const Map &in initial,  const Map&in forced, int iter = 100, float courant = 0.2,float hscale =1.0, int iter_accu = -1, bool precondition = false, float prec_scale = 1.0, bool do_forced = false )", GetFuncConvert( AS_AccuFluxDiffusive));
    r = engine->RegisterGlobalSTDFunction("array<Map> @Accuflux2D2(const Map &in DEM, const Map &in Source, const Map &in FlowSource, const Map &in initial,  const Map&in forced, int iter = 100, float courant = 0.2,float hscale =1.0, int iter_accu = -1, bool precondition = false, float prec_scale = 1.0, bool do_forced = false )", GetFuncConvert( AS_AccuFluxDiffusive2));
    r = engine->RegisterGlobalSTDFunction("array<Map> @Accuflux2D3(const Map &in DEM, const Map &in Source, const Map &in FlowSource, const Map &in initial,  const Map&in forced, const Map&in coeff_initial, int iter = 100, float courant = 0.2,float hscale =1.0, int iter_accu = -1, bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float do_next = 0.0)", GetFuncConvert( AS_AccuFluxDiffusive3));
    r = engine->RegisterGlobalSTDFunction("array<Map> @Accuflux2D32(const Map &in DEM, const Map &in Source, const Map &in FlowSource, const Map &in initial,  const Map&in forced, const Map&in coeff_initial,  const Map &in pressure_fac, int iter = 100, float courant = 0.2,float hscale =1.0, int iter_accu = -1, bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float do_next = 0.0)", GetFuncConvert( AS_AccuFluxDiffusive32));
    r = engine->RegisterGlobalSTDFunction("array<Map> @SSAccumulate(const Map &in DEM, const Map &in UX, const Map &in UY, const Map &in Source,  int iter = 2500)", GetFuncConvert( AS_AccumulateFlowAcc));

    r = engine->RegisterGlobalFunction("Map @SteadyStateSoil(const Map &in DEM, const Map &in Source, const Map &in QS, int iter = 100)", asFUNCTIONPR(    AS_SteadyStateSoil,(cTMap *,cTMap *,cTMap*,int),cTMap *),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("Map @AccufluxSoil(const Map &in DEM, const Map &in Accuflux, const Map &in QS, float k, float power = 1.0, int iter = 500, float courant = 0.2)", asFUNCTIONPR(    AS_AccufluxSoil,(cTMap *,cTMap *,cTMap*,float,float,  int,float),cTMap *),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalFunction("Map @FlowTransient(const Map &in DEM, const Map &in HSoil, const Map &in KSAT, const Map &in Porosity, const Map &in gwh, float dt, bool inflowlimit = true)", asFUNCTIONPR(    AS_TransientFlow,(cTMap *,cTMap *,cTMap*,cTMap *, cTMap*,float,bool),cTMap *),  asCALL_CDECL); assert( r >= 0 );;
    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowIncompressible(const Map &in M, const Map &in U, const Map &in U, const Map &in P, const Map &in LS, const Map &in Block,const Map &in BlockU,const Map &in BlockV, float dt, float visc,float courant = 0.2, float beta0 = 1.7)", GetFuncConvert(AS_IncompressibleWave));
    r = engine->RegisterGlobalFunction("Map @MVMap(const Map &in M)", asFUNCTIONPR(    AS_MVMap,(cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SteadyStateCorrection(const Map &in b, const Map &in Trel)", asFUNCTIONPR(  AS_SteadyStateCorrection,(cTMap *, cTMap *),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Map @FlowErosionDeposition(const Map &in Sed, const Map &in H, const Map &in Ux, const Map&in Uy, const Map&in Coh, const Map& in d50, const Map&in Slope, float dt)", asFUNCTIONPR( AS_ErosionDeposition,(cTMap *, cTMap *,cTMap*,cTMap*,cTMap *,cTMap *,cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @SplashErosion(const Map &in Sed, const Map &in H, const Map &in Coh, const Map&in VegCover, const Map&in VegHeight, const Map& in Rain,float dt)", asFUNCTIONPR( AS_SplashErosion,(cTMap *, cTMap *,cTMap*,cTMap *,cTMap *,cTMap *,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalSTDFunction("Map @RasterizeLines(const Map &in mask, const array<vec2> &in lines)", GetFuncConvert(AS_RasterizeLines));


    r = engine->RegisterGlobalSTDFunction("Map @ConductiveDiffuse(const Map &in data, const Map &in conductivitydata, const Map &in sigma, int iter = 100, float dt = 0.1, float feedback_self_weight = 0.0)", GetFuncConvert( AS_ConductiveDiffuse));
    r = engine->RegisterGlobalSTDFunction("Map @ConductiveFeedbackDiffuse(const Map &in data, const Map &in conductivitydata, const Map &in sigma, const Map &in fbweight, int iter = 100, float dt = 0.1)", GetFuncConvert( AS_ConductiveFeedbackDiffuse));
    r = engine->RegisterGlobalSTDFunction("Map @Quantize(const Map &in data, int i)", GetFuncConvert( AS_Quantize));
    r = engine->RegisterGlobalSTDFunction("array<Map> @SpreadProperty(const Map &in H, const Map&in Prop, const Map&in DEM, const Map&in slopeAdd, float merge = 0.0, int iter = 100)", GetFuncConvert( AS_FlowPropertySpread));
    r = engine->RegisterGlobalSTDFunction("Map @Accumulate2D(const Map &in QX1, const Map&in QX2, const Map&in QY1, const Map&in QY2, const Map &in Source, int iter = 1000)", GetFuncConvert( AS_Accumulate2D));
    r = engine->RegisterGlobalSTDFunction("array<Map> @Accumulate2DT(const Map &in QX1, const Map&in QX2, const Map&in QY1, const Map&in QY2, const Map &in Source, int iter = 1000)", GetFuncConvert( AS_Accumulate2DT));
    r = engine->RegisterGlobalSTDFunction("Map @Accumulate2DTReach(const Map &in QX1, const Map&in QX2, const Map&in QY1, const Map&in QY2, const Map &in Source, const Map&in Reach, int iter = 1000)", GetFuncConvert( AS_Accumulate2DTReach));

    r = engine->RegisterGlobalSTDFunction("float GetSSRainfalMaxVol(array<float> &in time, array<float> &in rain)", GetFuncConvert( AS_GetSSRainfallMaxVol));
    r = engine->RegisterGlobalSTDFunction("float GetTotalRain(array<float> &in time, array<float> &in rain)", GetFuncConvert( AS_GetTotalRainfall));
    r = engine->RegisterGlobalSTDFunction("float GetSSDurationRain(array<float> &in time, array<float> &in rain, float rain)", GetFuncConvert( AS_GetSSDurationFromRainfall));
    r = engine->RegisterGlobalSTDFunction("Map @FloodFill(const Map &in DEM, const Map &in HInit, int iter_max = 1000, bool subtract_slope= false)", GetFuncConvert( AS_FloodFill));
    r = engine->RegisterGlobalSTDFunction("Map @FloodReconstruct(const Map &in DEM, const Map &in HSS, const Map &in Hinv, int iter_max, float c, float d, float e, float f)", GetFuncConvert( AS_FlowReconstruct));

    r = engine->RegisterGlobalSTDFunction("array<Map> @FlowFast(const Map &in DEM, const Map &in N, const Map &in Rain, float duration, float scale_initial = 1.0, float iter_scale = 1.0, int reconstruct = 0)", GetFuncConvert( AS_FastFlood));


    r = engine->RegisterGlobalSTDFunction("array<Field> @FlowIncompressible3D(array<Field> &in state, Field &in Block,Field &in BlockU,Field &in BlockV,Field &in BlockW, float dt, float visc = 1.0,float courant = 0.2, float beta0 = 1.7)", GetFuncConvert(  AS_IncompressibleFlow3D));

    engine->RegisterGlobalFunction("double HyperGeometric(double a, double b, double c, double x)", asFUNCTION(hypergeometric),  asCALL_CDECL);
    engine->RegisterGlobalFunction("Map @Diffusion(const Map&in Value, float coeff, float dt, int iter = 1)", asFUNCTION(AS_Diffusion),  asCALL_CDECL); assert( r >= 0 );

    r = engine->RegisterGlobalSTDFunction("Map @SolveLaplace(const Map &in Initial, const Map&in Forced, int iterations)", GetFuncConvert( AS_SolveLaplace));
    r = engine->RegisterGlobalSTDFunction("Map @SolvePoisson(const Map &in Initial, const Map&in Forced, const Map &in f, int iterations)", GetFuncConvert( AS_SolvePoisson));
    r = engine->RegisterGlobalSTDFunction("Map @SolvePoissonDifSpeed(const Map &in Initial, const Map&in Forced, const Map &in f, const Map &in vel, int iterations)", GetFuncConvert( AS_SolvePoissonDifSpeed));
    r = engine->RegisterGlobalSTDFunction("Map @SolvePoissonWeighted(const Map &in Initial, const Map&in Forced, const Map &in f, float wx1,float wx2, float wy1, float wy2,int iterations)", GetFuncConvert( AS_SolvePoissonW));
    r = engine->RegisterGlobalSTDFunction("array<Map> @SolvePoissonWeightedC(const Map &in Initial, const Map&in Forced, const Map &in f, float wx1,float wx2, float wy1, float wy2,int iterations)", GetFuncConvert( AS_SolvePoissonWC));
    r = engine->RegisterGlobalSTDFunction("array<Map> @SolvePoissonAbsTotal(const Map &in Initial, const Map&in Forced, const Map &in f, int iterations)", GetFuncConvert( AS_SolvePoissonAbsTotal));


    r = engine->RegisterGlobalFunction("Map @SuperResolutionSISR(const Map &in s1,float upscale = 2.0, int iterations  =1, int tilesize = 512, int patchSize = 4, int PatchOverlap = 3, double lambda = 1e-12, int neighborhoodSize = 200, double neighborhoodWeight = 1.0, bool wavelet = false)", asFUNCTIONPR( AS_SuperResolutionSISR,(cTMap*, float,int,int,int,int,double,int,double,bool),cTMap*),  asCALL_CDECL); assert( r >= 0 );


    //distance transform

    r = engine->RegisterGlobalFunction("Map @DistanceTransform(const Map &in s1)", asFUNCTIONPR( AS_DistanceTransform,(cTMap*),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("Map @BilateralFilter(const Map &in s1,int size, float sigma_space, float sigma_color)", asFUNCTIONPR( AS_BilateralFilter,(cTMap*,int, float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );



    //hillshading
    r = engine->RegisterGlobalFunction("Map @HillShade(const Map &in elevation, float scale = 1.0)", asFUNCTIONPR( AS_HillShade,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );


    //domain transforms
    r = engine->RegisterGlobalSTDFunction("array<Map> @RasterFourier(const Map &in value)",GetFuncConvert(AS_RasterFourier));
    r = engine->RegisterGlobalFunction("Map @RasterInverseFourier(const Map &in real, const Map &in complex)",asFUNCTIONPR( AS_RasterInverseFourier, (cTMap *,cTMap *),cTMap *),asCALL_CDECL);


    //Comparative correlation coefficients
    r = engine->RegisterGlobalFunction("double MapCohensKappa(const Map &in Observed, const Map &in Model)", asFUNCTIONPR( AS_RasterCohensKappa,(cTMap*,cTMap*),double),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double MapContinuousCohensKappa(const Map &in Observed, const Map &in Model, double threshold)", asFUNCTIONPR( AS_RasterContinuousCohensKappa,(cTMap*,cTMap*, double),double),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double MapAccuracy(const Map &in Observed, const Map &in Model)", asFUNCTIONPR( AS_RasterAccuracy,(cTMap*,cTMap*),double),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double MapPrecisionRecall(const Map &in Observed, const Map &in Model)", asFUNCTIONPR( AS_RasterPrecisionRecall,(cTMap*,cTMap*),double),  asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("double MapContinuousPrecisionRecall(const Map &in Observed, const Map &in Model, double threshold)", asFUNCTIONPR( AS_RasterContinuousPrecisionRecall,(cTMap*,cTMap*, double),double),  asCALL_CDECL); assert( r >= 0 );

    engine->RegisterGlobalFunction("float Random(float min = 0.0,float max = 1.0)",asFUNCTIONPR(RandomFloat,(float,float),float),asCALL_CDECL);
    engine->RegisterGlobalFunction("float RandomNormal(float mean = 0.0,float sigma = 1.0)",asFUNCTIONPR(RandomNormalFloat,(float,float),float),asCALL_CDECL);

    engine->RegisterGlobalFunction("Map @RandomMap(const Map &in mask, float min = 0.0, float max =1.0)", asFUNCTIONPR( AS_MapRandom,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @RandomNormalMap(const Map &in mask, float mean = 0.0, float sigma = 1.0)", asFUNCTIONPR( AS_MapRandomNormal,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    engine->RegisterGlobalFunction("Map @RandomAreaMap(const Map &in mask, float min = 0.0, float max =1.0)", asFUNCTIONPR( AS_MapAreaRandom,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @RandomNormalAreaMap(const Map &in mask, float mean = 0.0, float sigma = 1.0)", asFUNCTIONPR( AS_MapAreaRandomNormal,(cTMap*,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    engine->RegisterGlobalFunction("Map @RandomPerlinNoise(const Map &in mask, float wavelength)", asFUNCTIONPR( AS_PerlinNoise,(cTMap*,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @RandomGaborNoise(const Map &in mask, float dirx, float diry, float scalex, float scaley)", asFUNCTIONPR( AS_GaborNoise,(cTMap*,float,float,float,float),cTMap*),  asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("Map @RandomVoronoise(const Map &in mask, float scalex, float scaley, int pointn = 1)", asFUNCTIONPR( AS_Voronoise,(cTMap*,float,float, int),cTMap*),  asCALL_CDECL); assert( r >= 0 );

    engine->RegisterGlobalSTDFunction("array<Map> @ApplyMLModel(array<Map> &in image, string modelpath, int pathsize_x, int patchsize_y , array<int> &in inputbasis = {0,1,2,3},bool fill = false)",GetFuncConvert(ApplyMLBModel));

    engine->RegisterGlobalSTDFunction("array<Map> @GetClosestCells(const Map &in mask)",GetFuncConvert(AS_GetClosestCells));
    engine->RegisterGlobalSTDFunction("array<Map> @GetClosestCoordinate(const Map &in mask)",GetFuncConvert(AS_GetClosestCoordinate));
    engine->RegisterGlobalSTDFunction("Map @RasterMoveToCell(const Map &in data, const Map &in row, const Map&in col)",GetFuncConvert(AS_RasterMoveToCell));
    engine->RegisterGlobalSTDFunction("Map @RasterCellSample(const Map &in data, const Map &in row, const Map&in col)",GetFuncConvert(AS_RasterCellSample));
    engine->RegisterGlobalSTDFunction("Map @RasterCoordinateSample(const Map &in data, const Map&in x, const Map&in y)",GetFuncConvert(AS_RasterSample));

    engine->RegisterGlobalSTDFunction("array<Map> @FiberBundleModel(const Map &in LoadMax, const Map&in LoadMaxLatX, const Map&in LoadMaxLatY, const Map &in Damage_Base, const Map &in Damage_LatX, const Map &in Damage_LatY, const Map&in LoadX, const Map &in LoadY, const Map&in CapacityBase, const Map&in CapacityLatX, const Map&in CapacityLatY, const Map&in K_comp, const Map&in K_tens,const Map &in Alpha, int iterations = 25, float m_base = 2, float m_lat = 2, bool do_springblock = false)",GetFuncConvert(AS_LoadRedistribute));
}



