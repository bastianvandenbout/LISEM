#ifndef POINT_H
#define POINT_H
#include "defines.h"
#include "shape.h"
#include "geometry/geometrybase.h"


class LISEM_API LSMPoint: public LSMShape
{
    double m_x = 0.0;
    double m_y = 0.0;
    double m_z = 0.0;
    double m_radius = 0.0;




public:
    inline LSMPoint() : LSMShape(LISEM_SHAPE_POINT)
    {
    }

    inline LSMPoint(double x, double y) : LSMShape(LISEM_SHAPE_POINT)
    {
        m_x = x;
        m_y = y;
    }


    inline void SetPos(double x, double y, double z = 0.0f)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    inline double GetX()
    {
        return m_x;
    }

    inline double GetY()
    {
        return m_y;
    }

    inline double GetZ()
    {
        return m_z;
    }


    inline LSMShape * Copy() override
    {
        LSMPoint * p = new LSMPoint();
        p->SetPos(GetX(),GetY(),GetZ());

        return p;
    }

    inline double GetRadius()
    {
        return m_radius;
    }

    bool Contains(LSMVector2 pos, double tolerence)
    {
        float xdifsq = (pos.x-m_x);
        float ydifsq = (pos.y-m_y);
        if(std::sqrt((xdifsq * xdifsq + ydifsq * ydifsq)) <= m_radius + tolerence)
        {
            return true;
        }
        return false;
    }



    inline LSMVector2 GetClosestEdgeLocation(LSMVector2 pos) override
    {
        return LSMVector2(m_x,m_y);
    }

    inline LSMVector2 GetClosestVertex(LSMVector2 pos) override
    {
        return LSMVector2(m_x,m_y);
    }


    inline LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        if(x1 != nullptr)
        {
            *x1 = &m_x;
        }
        if(x2 != nullptr)
        {
            *x2 = &m_x;
        }
        if(y1 != nullptr)
        {
            *y1 = &m_y;
        }
        if(y2 != nullptr)
        {
            *y2 = &m_y;
        }

        return LSMVector2(m_x,m_y);
    }

    inline LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
    {
        if(x1 != nullptr)
        {
            *x1 = &m_x;
        }

        if(y1 != nullptr)
        {
            *y1 = &m_y;
        }
        return LSMVector2(m_x,m_y);
    }


    virtual void MoveVertex(double *x, double *y, LSMVector2 d) override
    {
        m_x += d.X();
        m_y += d.Y();
    }

    inline void Move(LSMVector2 d) override
    {
        m_x += d.X();
        m_y += d.Y();
    }

    virtual int GetVertexCount() override
    {
        return 1;
    }
    virtual LSMVector2 GetVertex(int i) override
    {
        return LSMVector2(m_x,m_y);
    }
    virtual void GetVertexP(int i, double ** x , double **y) override
    {
        *x = &m_x;
        *y = &m_y;
    }
    virtual bool Overlaps(std::vector<double> & x,std::vector<double> & y) override
    {
        return Geometry_PolygonContaintsPoint(x,y,m_x,m_y);
    }

    virtual QList<LSMShape *> Split(std::vector<double> & x,std::vector<double> & y) override
    {
        QList<LSMShape *> list;
        list.append(new LSMPoint(m_x,m_y));
        return list;

    }

    virtual LSMVector2 GetCentroid()
    {
        return LSMVector2(m_x,m_y);
    }

    inline BoundingBox GetBoundingBox()
    {
        BoundingBox b;
        b.Set(m_x - m_radius,m_x + m_radius,m_y - m_radius,m_y + m_radius);

        return b;
    }

    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMPoint*    AS_Assign            (LSMPoint*);


};


//Angelscript related functionality

inline void LSMPoint::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMPoint::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMPoint* LSMPoint::AS_Assign(LSMPoint* sh)
{

    return this;
}

inline static LSMPoint * AS_PointFactory()
{
    return new LSMPoint();
}

#endif // POINT_H
