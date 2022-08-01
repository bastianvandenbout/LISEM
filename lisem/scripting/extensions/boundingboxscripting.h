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

inline static BoundingBox ScaleBoundingBox(BoundingBox b, float sx, float sy)
{
    BoundingBox res = b;
    res.ScaleX(sx);
    res.ScaleY(sy);

    return res;
}

inline static BoundingBox Move(BoundingBox b, float mx, float my)
{

    BoundingBox res = b;

    res.Move(LSMVector2(mx,my));

    return res;
}

BoundingBox MoveTo(BoundingBox b, LSMVector2 center)
{

    BoundingBox res = b;
    res.Move(LSMVector2(center.x - b.GetCenterX(),center.y - b.GetCenterY()));

    return res;
}

inline static BoundingBox LimitWithin(BoundingBox act, BoundingBox limit)
{
    BoundingBox bb = act;
    //check if size is smaller then required scale

    double size_min = std::min(bb.GetSizeX(),bb.GetSizeY());

    double req_min = 0.0;

    if(size_min < req_min)
    {
        bb.Scale(req_min/std::max(1e-12,size_min));
    }

    if(bb.GetSizeX() > limit.GetSizeX())
    {
        bb.Scale(limit.GetSizeX()/std::max(1e-12,bb.GetSizeX()));
    }

    if(bb.GetSizeY() > limit.GetSizeY())
    {
        bb.Scale(limit.GetSizeY()/std::max(1e-12,bb.GetSizeY()));
    }


    //if it is outside the bounds, move it in

    double move_x1 = std::max(0.0,limit.GetMinX() - bb.GetMinX());
    double move_x2 = std::min(0.0,limit.GetMaxX() - bb.GetMaxX());
    double move_y1 = std::max(0.0,limit.GetMinY() - bb.GetMinY());
    double move_y2 = std::min(0.0,limit.GetMaxY() - bb.GetMaxY());

    bb.Move(LSMVector2(move_x1 + move_x2,move_y1 + move_y2));

    return bb;
}


inline void RegisterGeoElementScripting(asIScriptEngine *engine)
{
    int r = engine->RegisterObjectType("Region", sizeof(BoundingBox), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLFLOATS|asGetTypeTraits<BoundingBox>()); assert( r >= 0 );

    // Register the behaviours
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(RegionFactory,(void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Region", asBEHAVE_CONSTRUCT, "void CSF0(double,double,double,double)", asFUNCTIONPR(RegionFactory,(double,double,double,double,void*),void), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double XMin()", asMETHOD(BoundingBox,GetMinX), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double XMax()", asMETHOD(BoundingBox,GetMaxX), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Region", "double YMin()", asMETHOD(BoundingBox,GetMinY), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Region", "double YMax()", asMETHOD(BoundingBox,GetMaxY), asCALL_THISCALL); assert( r >= 0 );


    //some helper functions

     engine->RegisterGlobalFunction("Region RegionScale(Region r, float sx, float sy)", asFUNCTIONPR( ScaleBoundingBox,(BoundingBox, float, float),BoundingBox),  asCALL_CDECL); assert( r >= 0 );
     engine->RegisterGlobalFunction("Region RegionMove(Region r, float mx, float my)", asFUNCTIONPR( Move,(BoundingBox, float, float),BoundingBox),  asCALL_CDECL); assert( r >= 0 );
     engine->RegisterGlobalFunction("Region RegionMoveTo(Region r, vec2 center)", asFUNCTIONPR( MoveTo,(BoundingBox,  LSMVector2),BoundingBox),  asCALL_CDECL); assert( r >= 0 );
     engine->RegisterGlobalFunction("Region RegionLimit(Region act, Region limit)",  asFUNCTIONPR(  LimitWithin,(BoundingBox, BoundingBox),BoundingBox),  asCALL_CDECL); assert( r >= 0 );



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
