#ifndef BOUNDINGBOXSCRIPTING_H
#define BOUNDINGBOXSCRIPTING_H

#include "boundingbox.h"
#include "lsmscriptengine.h"
#include "geometry/point.h"

inline static void RegionFactory(void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) BoundingBox();

}

inline static void RegionFactory(double x1,double x2,double y1,double y2,void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) BoundingBox(x1,x2,y1,y2);

}


inline void RegisterGeoElementScripting(asIScriptEngine *engine)
{
    int r = engine->RegisterObjectType("Region", sizeof(BoundingBox), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLFLOATS|asGetTypeTraits<BoundingBox>()); assert( r >= 0 );

    // Register the behaviours
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(RegionFactory,(void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0(double,double,double,double)", asFUNCTIONPR(RegionFactory,(double,double,double,double,void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double XMin()", asMETHOD(BoundingBox,GetMinX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double XMax()", asMETHOD(BoundingBox,GetMaxX), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double YMin()", asMETHOD(BoundingBox,GetMinX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double YMax()", asMETHOD(BoundingBox,GetMaxX), asCALL_THISCALL); assert( r >= 0 );

}

inline static void PointFactory(void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) LSMPoint();

}

inline static void PointFactory(double x1,double x2,void * memory)
{
    // Initialize the pre-allocated memory by calling the
    // object constructor with the placement-new operator
    new(memory) LSMPoint(x1,x2);

}


inline void RegisterGeometryScripting(asIScriptEngine *engine)
{

    int r = engine->RegisterObjectType("Point", sizeof(LSMPoint), asOBJ_VALUE | asOBJ_POD| asGetTypeTraits<LSMPoint>()); assert( r >= 0 );

    // Register the behaviours
    r = engine->RegisterObjectBehaviour("Point", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(PointFactory,(void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Point", asBEHAVE_CONSTRUCT, "void CSF0(double,double)", asFUNCTIONPR(PointFactory,(double,double,void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Point", "double X()", asMETHOD(LSMPoint,GetX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Point", "double Y()", asMETHOD(LSMPoint,GetY), asCALL_THISCALL); assert( r >= 0 );

    //point

    //line

    //polygon

    //circle



    //algorithms



}



#endif // BOUNDINGBOXSCRIPTING_H
