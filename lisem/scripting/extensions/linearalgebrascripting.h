#ifndef LINEARALGEBRASCRIPTING_H
#define LINEARALGEBRASCRIPTING_H

#include "lsmscriptengine.h"
#include "linear/lsm_vector.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix2x2.h"
#include "linear/lsm_matrix3x3.h"
#include "linear/lsm_matrix4x4.h"




inline void RegisterLinearAlgebraScripting(asIScriptEngine *engine)
{

    //vec2
    int r = 1;
    engine->RegisterObjectType("vec2", sizeof(LSMVector2), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<LSMVector2>()); assert( r >= 0 );
    engine->RegisterObjectProperty("vec2", "float x", asOFFSET(LSMVector2,x)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec2", "float y", asOFFSET(LSMVector2,y)); assert( r >= 0 );
    engine->RegisterObjectBehaviour("vec2", asBEHAVE_LIST_CONSTRUCT, "void f(int &in) {float,float}", asMETHODPR(LSMVector2,AS_FromList, (void*),void), asCALL_THISCALL);

    //vec3
    engine->RegisterObjectType("vec3", sizeof(LSMVector3), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLFLOATS| asGetTypeTraits<LSMVector3>()); assert( r >= 0 );
    engine->RegisterObjectBehaviour("vec3", asBEHAVE_LIST_CONSTRUCT, "void f(int &in) {float,float,float}", asMETHODPR(LSMVector3,AS_FromList, (void*),void), asCALL_THISCALL);


    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(LSMVector3,x)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(LSMVector3,y)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(LSMVector3,z)); assert( r >= 0 );

    engine->RegisterObjectMethod("vec3", "vec3 opAdd(vec3 m) const", asMETHODPR(LSMVector3,operator+,(const LSMVector3 &) const , LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    engine->RegisterObjectMethod("vec3", "vec3 opSub(vec3 m) const", asMETHODPR(LSMVector3,operator-,(const LSMVector3 &) const , LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    engine->RegisterObjectMethod("vec3", "vec3 opMul(float m) const", asMETHODPR(LSMVector3,operator*,(const float &) const , LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    engine->RegisterObjectMethod("vec3", "vec3 opDiv(float m) const", asMETHODPR(LSMVector3,operator/,(const float &) const , LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    engine->RegisterObjectMethod("vec3", "vec3 opMul_r(float m) const", asMETHODPR(LSMVector3,operator*,(const float &) const , LSMVector3), asCALL_THISCALL); assert( r >= 0 );

    //dot product
    //r = sm->m_Engine->RegisterGlobalFunction("Table @LoadTable(const string &in s)", asFUNCTION( AS_LoadTableFromFile), asCALL_CDECL); assert( r >= 0 );
    engine->RegisterGlobalFunction("vec3 cross(const vec3 &in v1, const vec3 &in v2)",asFUNCTION(LSMVector3::CrossProduct),asCALL_CDECL);
    //cross product
    engine->RegisterGlobalFunction("float dot(const vec3 &in v1,const vec3 &in v2)",asFUNCTION(LSMVector3::VDot),asCALL_CDECL);
    //normalization
    engine->RegisterGlobalFunction("vec3 normalize(const vec3 &in v1)",asFUNCTION(LSMVector3::VNormalize),asCALL_CDECL);
    //length
    engine->RegisterGlobalFunction("float length(const vec3 &in v1)",asFUNCTION(LSMVector3::VLength),asCALL_CDECL);
    //distance
    engine->RegisterGlobalFunction("float distance(const vec3 &in v1, const vec3 &in v2)",asFUNCTION(LSMVector3::VDistance),asCALL_CDECL);
    //reflect
    engine->RegisterGlobalFunction("float reflect(const vec3 &in v1, const vec3 &in v2)",asFUNCTION(LSMVector3::VReflect),asCALL_CDECL);
    //refract
    engine->RegisterGlobalFunction("float refract(const vec3 &in v1, const vec3 &in v2, float eta)",asFUNCTION(LSMVector3::VRefract),asCALL_CDECL);
    //

    //vec4
    engine->RegisterObjectType("vec4", sizeof(LSMVector4), asOBJ_VALUE | asOBJ_APP_CLASS_ALLFLOATS| asOBJ_POD| asGetTypeTraits<LSMVector4>()); assert( r >= 0 );
    engine->RegisterObjectProperty("vec4", "float x", asOFFSET(LSMVector4,x)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec4", "float y", asOFFSET(LSMVector4,y)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec4", "float z", asOFFSET(LSMVector4,z)); assert( r >= 0 );
    engine->RegisterObjectProperty("vec4", "float w", asOFFSET(LSMVector4,w)); assert( r >= 0 );
    //vecn


    //quaternion

    //helper functions


    //matrix2x2

    //matrix3x3

    //matrix4x4
    engine->RegisterObjectType("mat4x4", sizeof(LSMMatrix4x4), asOBJ_VALUE| asOBJ_APP_CLASS_ALLFLOATS| asOBJ_POD| asGetTypeTraits<LSMMatrix4x4>()); assert( r >= 0 );


    //matrixnxn



    //helper functions



}




#endif // LINEARALGEBRASCRIPTING_H
