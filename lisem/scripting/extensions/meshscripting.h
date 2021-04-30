#pragma once
#include "models/3dmodel.h"
#include "models/lsmmesh.h"





inline void RegisterMeshToScriptEngine(asIScriptEngine *engine)
{

    int r = engine->RegisterObjectType("Object",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Object",asBEHAVE_ADDREF,"void f()",asMETHOD(ModelGeometry,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Object",asBEHAVE_RELEASE,"void f()",asMETHOD(ModelGeometry,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Object", "Object& opAssign(Object &in m)", asMETHOD(ModelGeometry,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Object",asBEHAVE_FACTORY,"Object@ CSF0()",asFUNCTIONPR(AS_ModelGeometryFactory,(),ModelGeometry *),asCALL_CDECL); assert( r >= 0 );


    r = engine->RegisterObjectType("Mesh",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Mesh",asBEHAVE_ADDREF,"void f()",asMETHOD(LSMMesh,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Mesh",asBEHAVE_RELEASE,"void f()",asMETHOD(LSMMesh,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Mesh", "Mesh& opAssign(Mesh &in m)", asMETHOD(LSMMesh,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Mesh",asBEHAVE_FACTORY,"Mesh@ CSF0()",asFUNCTIONPR(AS_MeshFactory,(),LSMMesh *),asCALL_CDECL); assert( r >= 0 );




}
