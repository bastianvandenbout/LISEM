#ifndef POINTCLOUDSCRIPTING_H
#define POINTCLOUDSCRIPTING_H

#include <angelscript.h>
#include "scriptmanager.h"
#include "geo/pointcloud/pointcloud.h"

inline void RegisterPointCloudToScriptEngine(asIScriptEngine *engine)
{

    //register object type
    int r = engine->RegisterObjectType("PointCloud",0,asOBJ_REF);//PointCloud
    if(r < 0){LISEM_DEBUG("error in registering scripting: class PointCloud");};

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("PointCloud",asBEHAVE_ADDREF,"void f()",asMETHOD(PointCloud,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("PointCloud",asBEHAVE_RELEASE,"void f()",asMETHOD(PointCloud,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("PointCloud", "PointCloud& opAssign(PointCloud &in m)", asMETHOD(PointCloud,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("PointCloud",asBEHAVE_FACTORY,"PointCloud@ CSF0()",asFUNCTIONPR(PointCloudFactory,(),PointCloud *),asCALL_CDECL); assert( r >= 0 );


}
#endif // POINTCLOUDSCRIPTING_H
