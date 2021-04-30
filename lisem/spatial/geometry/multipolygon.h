#pragma once

#include "defines.h"
#include "shape.h"
#include "polygon.h"
#include "QList"

class LSMPolygon;

class LISEM_API LSMMultiPolygon: public LSMShape
{

    QList<LSMPolygon*> m_List;


    BoundingBox m_BoundingBox;

public:
    inline LSMMultiPolygon() : LSMShape(LISEM_SHAPE_MULTIPOLYGON)
    {
    }

    void Add(LSMPolygon * p);

    inline int GetPolygonCount()
    {
        return m_List.length();
    }

    inline LSMPolygon * GetPolygon(int i)
    {
        return m_List.at(i);
    }

    LSMShape * Copy() override;

    LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override;

    LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override;


    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos) override;

    LSMVector2 GetClosestVertex(LSMVector2 pos) override;

    int GetVertexCount() override;
    LSMVector2 GetVertex(int i);
    void GetVertexP(int i, double ** x , double **y);
    void RemoveVertex(int i);
    void Destroy() override;

    bool Contains(LSMVector2 pos, double tolerence);

    virtual double Area() override;
    virtual double Length() override;
    virtual LSMVector2 GetCentroid() override;

    virtual void MoveVertex(double *x, double *y, LSMVector2 d) override;

    inline void Move(LSMVector2 d) override;

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }
    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMMultiPolygon*    AS_Assign            (LSMMultiPolygon*);


};


//Angelscript related functionality

inline void LSMMultiPolygon::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMMultiPolygon::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMMultiPolygon* LSMMultiPolygon::AS_Assign(LSMMultiPolygon* sh)
{

    return this;
}

inline static LSMMultiPolygon * AS_MultiPolygonFactory()
{
    return new LSMMultiPolygon();
}
