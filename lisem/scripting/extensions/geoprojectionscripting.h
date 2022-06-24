#ifndef GEOPROJECTIONSCRIPTING_H
#define GEOPROJECTIONSCRIPTING_H


#include <angelscript.h>
#include "scriptmanager.h"
#include "geo/geoprojection.h"

inline void RegisterGeoProjectionToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("GeoProjection",0,asOBJ_REF);//Shapes
    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_ADDREF,"void f()",asMETHOD(GeoProjection,AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_RELEASE,"void f()",asMETHOD(GeoProjection,ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GeoProjection", "GeoProjection& opAssign(GeoProjection &in m)", asMETHOD(GeoProjection,Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("GeoProjection",asBEHAVE_FACTORY,"GeoProjection@ CSF0()",asFUNCTIONPR(GeoProjectionFactory,(),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromEPSG(int)", asFUNCTIONPR(GetGeoProjectionFromEPSG,(int),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromWKT(string)", asFUNCTIONPR(GetGeoProjectionFromWKT,(std::string),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionFromWGS84UTM(int,bool)", asFUNCTIONPR(GetGeoProjectionFromWGS84UTM,(int, bool),GeoProjection *),asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterGlobalFunction("GeoProjection &GeoProjectionGeneric()", asFUNCTIONPR(GetGeoProjectionGeneric,(),GeoProjection *),asCALL_CDECL); assert( r >= 0 );

}
#endif // GEOPROJECTIONSCRIPTING_H
