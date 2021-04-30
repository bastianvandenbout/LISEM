#ifndef VECTORSCRIPTING_H
#define VECTORSCRIPTING_H

#include <angelscript.h>
#include "scriptmanager.h"
#include "geo/shapes/shapefile.h"
#include "geo/shapeoffset.h"
#include "geometry/feature.h"
#include "geometry/shape.h"
#include "extensions/scriptarray.h"

class AS_LSMShape
{



     //Angelscript related functions
     public:

    inline AS_LSMShape()
    {

    }

    inline AS_LSMShape(LSMShape * s,bool copy = true)
    {
        this->intern_InitializeFromShape(s,copy);

    }
    inline AS_LSMShape(const AS_LSMShape & s)
    {
         m_Shape = s.m_Shape->Copy();
    }

    LSMShape * m_Shape = nullptr;

         int            AS_refcount          = 1;
         void           AS_AddRef            ();
         void           AS_ReleaseRef        ();
         AS_LSMShape * AS_Assign            (AS_LSMShape*);

         inline void intern_InitializeFromShape(LSMShape * s, bool copy = true)
         {
             if(copy)
             {
                 m_Shape = s->Copy();
             }else {
                 m_Shape = s;
                }
         }

         void AS_SetAsPoint(LSMVector2 v);
         void AS_SetAsMultiPoint(std::vector<LSMVector2>);
         void AS_SetAsLine(LSMVector2 v1, LSMVector2 v2);
         void AS_SetAsLine(std::vector<LSMVector2>);
         void AS_SetAsMultiLine(std::vector<std::vector<LSMVector2> >);
         void AS_SetAsPolygon(std::vector<LSMVector2> ext);
         void AS_SetAsPolygon(std::vector<LSMVector2> ext, std::vector<LSMVector2> in);
         void AS_SetAsMultiPolygon(std::vector<std::vector<LSMVector2> > exts);
         void AS_SetAsMultiPolygon(std::vector<std::vector<LSMVector2>> exts, std::vector<std::vector<LSMVector2> > in);
         QString AS_GetShapeType();
         bool AS_IsMulti();
         bool AS_IsEmpty();
         int GetSubItemCount();
         AS_LSMShape * GetSubItem(int i);

         std::vector<LSMVector2>  AS_GetVertices();
         AS_LSMShape * AS_GetOuterRing();
         AS_LSMShape * AS_GetInnerRing();
         double AS_GetRadius();
         LSMVector2 AS_GetCentroid();
         bool AS_Contains(LSMVector2 v);
         double AS_GetLength();
         double AS_GetArea();
         void AS_Move(LSMVector2 m);
         LSMVector2 AS_GetClosestVertex(LSMVector2 v);
         LSMVector2 AS_GetClosestEdgePoint(LSMVector2 v);
         BoundingBox AS_GetBoundingBox();



    inline void Destroy()
    {
        if(m_Shape != nullptr)
        {
            delete m_Shape;
            m_Shape = nullptr;
        }
    }
};

//Angelscript related functionality

inline void AS_LSMShape::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void AS_LSMShape::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline AS_LSMShape* AS_LSMShape::AS_Assign(AS_LSMShape* sh)
{
    Destroy();
    intern_InitializeFromShape(sh->m_Shape->Copy());
    return this;
}


inline static AS_LSMShape * AS_ShapeFactory()
{
    return new AS_LSMShape();
}


#include "geo/shapealgorithms.h"
#include "geo/shapes/shapefile.h"

inline static ShapeFile * AS_And(ShapeFile * thisl, ShapeFile * other)
{
    ShapeLayer * l1 = thisl->GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,LISEM_SHAPE_INTERSECT);
}

inline static ShapeFile * AS_Or(ShapeFile * thisl,ShapeFile * other)
{
    ShapeLayer * l1 = thisl->GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,LISEM_SHAPE_UNION);
}

inline static ShapeFile * AS_Xor(ShapeFile * thisl,ShapeFile * other)
{
    ShapeLayer * l1 = thisl->GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,LISEM_SHAPE_XOR);
}

inline static ShapeFile * AS_Add(ShapeFile * thisl,ShapeFile * other)
{
    ShapeLayer * l1 = thisl->GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,LISEM_SHAPE_ADD);
}

inline static ShapeFile * AS_Sub(ShapeFile * thisl,ShapeFile * other)
{
    ShapeLayer * l1 = thisl->GetLayer(0);
    ShapeLayer * l2 = other->GetLayer(0);
    return ShapeLayerAlgebra(l1,l2,LISEM_SHAPE_DIFFERENCE);
}


inline void RegisterVectorToScriptEngine(asIScriptEngine *engine)
{




    //register object type
    int r =

    r = engine->RegisterObjectType("Shape",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Shape",asBEHAVE_ADDREF,"void f()",asMETHOD(AS_LSMShape,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Shape",asBEHAVE_RELEASE,"void f()",asMETHOD(AS_LSMShape,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "Shape& opAssign(Shape &in m)", asMETHOD(AS_LSMShape,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Shape",asBEHAVE_FACTORY,"Shape@ CSF0()",asFUNCTIONPR(AS_ShapeFactory,(),AS_LSMShape *),asCALL_CDECL); assert( r >= 0 );


    r = engine->RegisterObjectType("Feature",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Feature",asBEHAVE_ADDREF,"void f()",asMETHOD(Feature,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Feature",asBEHAVE_RELEASE,"void f()",asMETHOD(Feature,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "Feature& opAssign(Feature &in m)", asMETHOD(Feature,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Feature",asBEHAVE_FACTORY,"Feature@ CSF0()",asFUNCTIONPR(AS_FeatureFactory,(),Feature *),asCALL_CDECL); assert( r >= 0 );



    r = engine->RegisterObjectType("Shapes",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_ADDREF,"void f()",asMETHOD(ShapeFile,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_RELEASE,"void f()",asMETHOD(ShapeFile,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes& opAssign(Shapes &in m)", asMETHOD(ShapeFile,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("Shapes",asBEHAVE_FACTORY,"Shapes@ CSF0()",asFUNCTIONPR(ShapesFactory,(),ShapeFile *),asCALL_CDECL); assert( r >= 0 );



    r = engine->RegisterObjectType("ShapeLayer",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = engine->RegisterObjectBehaviour("ShapeLayer",asBEHAVE_ADDREF,"void f()",asMETHOD(ShapeLayer,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("ShapeLayer",asBEHAVE_RELEASE,"void f()",asMETHOD(ShapeLayer,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "ShapeLayer& opAssign(ShapeLayer &in m)", asMETHOD(ShapeLayer,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = engine->RegisterObjectBehaviour("ShapeLayer",asBEHAVE_FACTORY,"ShapeLayer@ CSF0()",asFUNCTIONPR(AS_ShapeLayerFactory,(),ShapeLayer *),asCALL_CDECL); assert( r >= 0 );




    //register binary operators
    r = engine->RegisterObjectMethod("Shapes", "Shapes@ opAnd(const Shapes &in s)", asFUNCTIONPR(AS_And,(ShapeFile *,ShapeFile *),ShapeFile*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes@ opOr(const Shapes &in s)", asFUNCTIONPR(AS_Or,(ShapeFile *,ShapeFile *),ShapeFile*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes@ opXor(const Shapes &in s)", asFUNCTIONPR(AS_Xor,(ShapeFile *,ShapeFile *),ShapeFile*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes@ opSub(const Shapes &in s)", asFUNCTIONPR(AS_Sub,(ShapeFile *,ShapeFile *),ShapeFile*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Shapes@ opAdd(const Shapes &in s)", asFUNCTIONPR(AS_Add,(ShapeFile *,ShapeFile *),ShapeFile*), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

    r = engine->RegisterGlobalFunction("Shapes@ VectorOffset(const Shapes &in s, float offset)", asFUNCTIONPR(AS_VectorOffset,(ShapeFile *,float),ShapeFile*), asCALL_CDECL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "Table@ GetTable()", asMETHODPR(ShapeFile,AS_GetTable,(),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Shapes", "Table& opIndex(string name)", asMETHODPR(ShapeFile,AS_GetTable,(QString),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );



    //all of the vector-object related methods
    r = engine->RegisterObjectMethod("Shapes", "void SetAttribute(string name, int index, string value)", asMETHODPR(ShapeFile,AS_SetAttributeValue,(QString,int,QString),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "ShapeLayer @GetLayer(int i)", asMETHODPR(ShapeFile,AS_GetLayer,(int),ShapeLayer *), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shapes", "int GetLayerCount()", asMETHODPR(ShapeFile,AS_GetLayerCount,(),int), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("ShapeLayer", "Feature@ GetFeature(int i)", asMETHODPR(ShapeLayer,AS_GetFeature,(int),Feature*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "int GetFeatureCount()", asMETHODPR(ShapeLayer,AS_GetFeatureCount,(),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "int GetShapeCount()", asMETHODPR(ShapeLayer,AS_GetShapeCount,(),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "int GetAtrributeCount()", asMETHODPR(ShapeLayer,AS_GetAtrributeCount,(),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "Table @GetAttributeTable()", asMETHODPR(ShapeLayer,AS_GetAttributes,(),MatrixTable*), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("ShapeLayer", "void AddFeature(Feature &f)", asMETHODPR(ShapeLayer,AS_AddFeature,(Feature *),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void RemoveFeature(int)", asMETHODPR(ShapeLayer,AS_RemoveFeature,(int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "string GetAttributeName(int)", asMETHODPR(ShapeLayer,AS_GetAttributeName,(int),QString), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "bool IsAttributeNumber(int)", asMETHODPR(ShapeLayer,AS_IsAttributeNumber,(int),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "bool IsAttributeText(int)", asMETHODPR(ShapeLayer, AS_IsAttributeText,(int),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void SetAttributeAsNumber(int)", asMETHODPR(ShapeLayer,AS_SetAttributeAsNumber,(int),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void SetAttributeAsText(int)", asMETHODPR(ShapeLayer,AS_SetAttributeAsText,(int),void), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("ShapeLayer", "bool SetFeatureAttribute(int f, string attributename, string val)", asMETHODPR(ShapeLayer,AS_SetFeatureAttribute,(int, QString, QString),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "bool SetFeatureAttribute(int f, string attributename, double val)", asMETHODPR(ShapeLayer, AS_SetFeatureAttribute,(int, QString, double),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void AddAttributeNumber(string name, double default_val = 0.0)", asMETHODPR(ShapeLayer,AS_AddAttributeNumber,(QString, double),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void AddAttributeText(string name, string default_val = \"\")", asMETHODPR(ShapeLayer,AS_AddAttributeString,(QString, QString),void), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("ShapeLayer", "vec2 GetCentroid()", asMETHODPR(ShapeLayer,AS_GetCentroid,(),LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "bool Contains(vec2 pos)", asMETHODPR(ShapeLayer,AS_Contains,(LSMVector2 pos),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "double GetLength()", asMETHODPR(ShapeLayer,AS_GetLength,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "double GetArea()", asMETHODPR(ShapeLayer,AS_GetArea,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "void Move(vec2 m)", asMETHODPR(ShapeLayer,AS_Move,(LSMVector2), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "vec2 GetClosestVertex(vec2 pos)", asMETHODPR(ShapeLayer,AS_GetClosestVertex,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "vec2 GetClosestEdgePoint(vec2 pos)", asMETHODPR(ShapeLayer,AS_GetClosestEdgePoint,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "Region GetBoundingBox()", asMETHODPR(ShapeLayer,AS_GetBoundingBox,(), BoundingBox), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ShapeLayer", "string GetAttributeValueAt(string attribute, vec2 pos)", asMETHODPR(ShapeLayer,AS_GetAttributeValueAt,( QString, LSMVector2), QString), asCALL_THISCALL); assert( r >= 0 );



    //shape

    r = engine->RegisterObjectMethod("Shape", "vec2 GetCentroid()", asMETHODPR(AS_LSMShape,AS_GetCentroid,(),LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "bool Contains(vec2 pos)", asMETHODPR(AS_LSMShape,AS_Contains,(LSMVector2 pos),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "double GetLength()", asMETHODPR(AS_LSMShape,AS_GetLength,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "double GetArea()", asMETHODPR(AS_LSMShape,AS_GetArea,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void Move(vec2 m)", asMETHODPR(AS_LSMShape,AS_Move,(LSMVector2), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "vec2 GetClosestVertex(vec2 pos)", asMETHODPR(AS_LSMShape,AS_GetClosestVertex,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "vec2 GetClosestEdgePoint(vec2 pos)", asMETHODPR(AS_LSMShape,AS_GetClosestEdgePoint,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "Region GetBoundingBox()", asMETHODPR(AS_LSMShape,AS_GetBoundingBox,(), BoundingBox), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Feature", "vec2 GetCentroid()", asMETHODPR(Feature,AS_GetCentroid,(),LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "bool Contains(vec2 pos)", asMETHODPR(Feature,AS_Contains,(LSMVector2 pos),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "double GetLength()", asMETHODPR(Feature,AS_GetLength,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "double GetArea()", asMETHODPR(Feature,AS_GetArea,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "void Move(vec2 m)", asMETHODPR(Feature,AS_Move,(LSMVector2), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "vec2 GetClosestVertex(vec2 pos)", asMETHODPR(Feature,AS_GetClosestVertex,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "vec2 GetClosestEdgePoint(vec2 pos)", asMETHODPR(Feature,AS_GetClosestEdgePoint,(LSMVector2), LSMVector2), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "Region GetBoundingBox()", asMETHODPR(Feature,AS_GetBoundingBox,(), BoundingBox), asCALL_THISCALL); assert( r >= 0 );

    //r = engine->RegisterObjectMethod("Feature", "Shape @GetShape(int index)", asMETHODPR(Feature,AS_GetShape,(int),AS_LSMShape *), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "int GetShapeCount()", asMETHODPR(Feature,AS_GetShapeCount,(), int), asCALL_THISCALL); assert( r >= 0 );
    //r = engine->RegisterObjectMethod("Feature", "void AddShape(Shape &in s)", asMETHODPR(Feature,AS_AddShape,(AS_LSMShape *), void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Feature", "void RemoveShape(int)", asMETHODPR(Feature, AS_RemoveShape,(int), void), asCALL_THISCALL); assert( r >= 0 );


    r = engine->RegisterObjectMethod("Shape", "void SetPoint(vec2 pos)", asMETHODPR(AS_LSMShape,AS_SetAsPoint,(LSMVector2),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetMultiPoint(array<vec2> &in pos)", asMETHODPR(AS_LSMShape,AS_SetAsMultiPoint,(std::vector<LSMVector2>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetLine(vec2 pos1, vec2 pos2)", asMETHODPR(AS_LSMShape,AS_SetAsLine,(LSMVector2, LSMVector2),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetLine(array<vec2> &in pos)", asMETHODPR(AS_LSMShape,AS_SetAsLine,(std::vector<LSMVector2>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetMultiLine(array<array<vec2>> &in pos)", asMETHODPR(AS_LSMShape,AS_SetAsMultiLine,(std::vector<std::vector<LSMVector2>>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetPolygon(array<vec2> &in pos)", asMETHODPR(AS_LSMShape,AS_SetAsPolygon,(std::vector<LSMVector2>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetPolygon(array<vec2> &in pos, array<vec2> &in hole)", asMETHODPR(AS_LSMShape,AS_SetAsPolygon,(std::vector<LSMVector2>,std::vector<LSMVector2>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetMultiPolygon(array<array<vec2>> &in pos)", asMETHODPR(AS_LSMShape,AS_SetAsMultiPolygon,(std::vector<std::vector<LSMVector2>>),void), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "void SetMultiPolygon(array<array<vec2>> &in pos, array<array<vec2>> &in hole)", asMETHODPR(AS_LSMShape,AS_SetAsMultiPolygon,(std::vector<std::vector<LSMVector2>>,std::vector<std::vector<LSMVector2>>),void), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Shape", "string GetShapeType()", asMETHODPR(AS_LSMShape,AS_GetShapeType,(),QString), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "bool IsMulti()", asMETHODPR(AS_LSMShape,AS_IsMulti,(),bool), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "int GetSubShapeCount()", asMETHODPR(AS_LSMShape,GetSubItemCount,(),int), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "Shape@ GetSubShape(int index)", asMETHODPR(AS_LSMShape,GetSubItem,(int),AS_LSMShape*), asCALL_THISCALL); assert( r >= 0 );

    r = engine->RegisterObjectMethod("Shape", "double GetRadius()", asMETHODPR(AS_LSMShape,AS_GetRadius,(),double), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "array<vec2> @GetVertices()", asMETHODPR(AS_LSMShape,AS_GetVertices,(),std::vector<LSMVector2>), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "Shape @GetInnerRing()", asMETHODPR(AS_LSMShape,AS_GetInnerRing,(),AS_LSMShape*), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Shape", "Shape @GetOuterRing()", asMETHODPR(AS_LSMShape,AS_GetOuterRing,(),AS_LSMShape*), asCALL_THISCALL); assert( r >= 0 );


    //feature



}

#endif // VECTORSCRIPTING_H
