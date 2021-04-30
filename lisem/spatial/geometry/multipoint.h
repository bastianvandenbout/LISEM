#pragma once

#include "point.h"
#include <QList>
#include "shape.h"

class LSMMultiPoint: public LSMShape
{
    QList<LSMPoint *> m_List;

    BoundingBox m_BoundingBox;
public:
    inline LSMMultiPoint() : LSMShape(LISEM_SHAPE_MULTIPOINT)
    {
    }

    inline void Add(LSMPoint * p)
    {
        m_List.append(p);
        if(m_List.size() == 1)
        {
            m_BoundingBox = p->GetBoundingBox();
        }else {
            m_BoundingBox.MergeWith(p->GetBoundingBox());
        }
    }

    inline int GetPointCount()
    {
        return m_List.size();
    }
    inline LSMPoint * GetPoint(int i)
    {
        return m_List.at(i);
    }

    inline int GetVertexCount() override
    {
        int count = 0;
        for(int i = 0; i < m_List.size() ; i++)
        {
            count += m_List.at(i)->GetVertexCount();
        }
        return count;
    }
    inline LSMVector2 GetVertex(int i)
    {
        for(int j = 0; j < m_List.size() ; j++)
        {
            if(i < m_List.at(j)->GetVertexCount() && m_List.at(j)->GetVertexCount() > 0)
            {
                return m_List.at(j)->GetVertex(i);
            }else {
                i -= m_List.at(j)->GetVertexCount();
            }
        }
        return LSMVector2(0.0,0.0);
    }

    inline void GetVertexP(int i, double ** x , double **y)
    {
        for(int j = 0; j < m_List.size() ; j++)
        {
            if(i < m_List.at(j)->GetVertexCount() && m_List.at(j)->GetVertexCount() > 0)
            {
                m_List.at(j)->GetVertexP(i,x,y);
                return;
            }else {
                i -= m_List.at(j)->GetVertexCount();
            }
        }

        *x = nullptr;
        *y= nullptr;

    }


    inline void RemoveVertex(int i)
    {
        for(int j = 0; j < m_List.size() ; j++)
        {
            if(i < m_List.at(j)->GetVertexCount() && m_List.at(j)->GetVertexCount() > 0)
            {
                if(m_List.at(j)->GetVertexCount() == 1)
                {
                    m_List.removeAt(j);
                    return;
                }else {
                    m_List.at(j)->RemoveVertex(i);
                    return;
                }

            }else {
                i -= m_List.at(j)->GetVertexCount();
            }
        }
    }

    inline LSMShape * Copy() override
    {

        LSMMultiPoint *mp = new LSMMultiPoint();

        for(int j = 0; j < GetPointCount(); j++)
        {
            mp->Add((LSMPoint*)GetPoint(j)->Copy());
        }

        return mp;
    }

    inline void Destroy() override
    {

        for(int j = 0; j < GetPointCount(); j++)
        {
            m_List.at(j)->Destroy();
        }
    }

    virtual LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 , double **y1 , double **x2 , double **y2) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(x2 != nullptr)
        {
            *x2 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }
        if(y2 != nullptr)
        {
            *y2 = nullptr;
        }

        double ** x1t = new (double*);
        double ** y1t = new (double*);
        double ** x2t = new (double*);
        double ** y2t = new (double*);


        for(int j = 0; j < m_List.length(); j++)
        {
            LSMVector2 p = m_List.at(j)->GetClosestEdgeLocationP(pos,x1t,y1t,x2t,y2t);
            float distance = LSMVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;
                *x1 = *x1t;*y1 = *y1t;*x2 = *x2t;*y2 = *y2t;

            }
        }
        delete x1t;delete y1t; delete x2t; delete y2t;

        return closest;
    }

    virtual LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 , double **y1) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }
        double ** x1t = new (double*);
        double ** y1t = new (double*);

        for(int j = 0; j < m_List.length(); j++)
        {
            LSMVector2 p = m_List.at(j)->GetClosestVertexP(pos,x1t,y1t);
            float distance = LSMVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;
                *x1 = *x1t;*y1 = *y1t;
            }
        }
        delete x1t;delete y1t;

        return closest;
    }

    virtual void MoveVertex(double *x, double *y, LSMVector2 d) override
    {
        for(int j = 0; j < m_List.length(); j++)
        {
            m_List.at(j)->MoveVertex(x,y,d);
            if(j == 0)
            {
                m_BoundingBox = m_List.at(j)->GetBoundingBox();
            }else {
                m_BoundingBox.MergeWith(m_List.at(j)->GetBoundingBox());
            }
        }

    }

    inline void Move(LSMVector2 d) override
    {
        m_BoundingBox.Move(d);
        for(int j = 0; j < m_List.length(); j++)
        {
            m_List.at(j)->Move(d);
        }
    }


    virtual LSMVector2 GetClosestEdgeLocation(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        for(int j = 0; j < m_List.length(); j++)
        {
            LSMVector2 p = m_List.at(j)->GetClosestEdgeLocation(pos);
            float distance = LSMVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;
            }
        }

        return closest;
    }

    virtual LSMVector2 GetClosestVertex(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        for(int j = 0; j < m_List.length(); j++)
        {
            LSMVector2 p = m_List.at(j)->GetClosestVertex(pos);
            float distance = LSMVector2(pos - p).length();
            if(j == 0 || distance < mindist)
            {
                mindist = distance;
                closest = p;

            }
        }

        return closest;
    }

    inline LSMVector2 GetCentroid() override
    {
        LSMVector2 Centroid = LSMVector2(0.0,0.0);
        double area = 0.0;
        for(int j = 0; j < m_List.length(); j++)
        {
            double area_this = 1.0;
            area += area_this;
            Centroid = Centroid + m_List.at(j)->GetCentroid()*area_this;
        }
        if(area > 0)
        {
            Centroid = Centroid/area;
        }
        return Centroid;
    }


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    virtual bool Contains(LSMVector2 pos, double tolerence)
    {

        for(int i = 0; i < m_List.length(); i++)
        {
            if(m_List.at(i)->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(m_List.at(i)->Contains(pos,tolerence))
                {
                    return true;
                }
            }
        }
        return false;
    }
    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMMultiPoint*    AS_Assign            (LSMMultiPoint*);


};


//Angelscript related functionality

inline void LSMMultiPoint::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMMultiPoint::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMMultiPoint* LSMMultiPoint::AS_Assign(LSMMultiPoint* sh)
{

    return this;
}

inline static LSMMultiPoint * AS_MultiPointFactory()
{
    return new LSMMultiPoint();
}
