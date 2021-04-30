#pragma once

#include "defines.h"
#include "line.h"
#include "QList"
#include "shape.h"

class LISEM_API LSMMultiLine: public LSMShape
{

    QList<LSMLine*> m_List;
   BoundingBox m_BoundingBox;
public:
    inline LSMMultiLine() : LSMShape(LISEM_SHAPE_MULTILINE)
    {
    }

    inline void Add(LSMLine * l)
    {
        m_List.append(l);
        if(m_List.size() == 1)
        {
            m_BoundingBox = l->GetBoundingBox();
        }else {
            m_BoundingBox.MergeWith(l->GetBoundingBox());
        }
    }


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }


    inline int GetLineCount()
    {
        return m_List.length();
    }
    inline LSMLine * GetLine(int i)
    {
        return m_List.at(i);
    }

    inline LSMShape * Copy() override
    {

        LSMMultiLine *mp = new LSMMultiLine();

        for(int j = 0; j < GetLineCount(); j++)
        {
            mp->Add((LSMLine*)GetLine(j)->Copy());
        }

        return mp;
    }

    virtual LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
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


    virtual LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
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
            if(j == 0)
            {
                mindist = distance;
            }else if(distance < mindist)
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
            if(j == 0)
            {
                mindist = distance;
                closest = p;
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = p;
            }
        }

        return closest;
    }

    inline void Destroy() override
    {

        for(int j = 0; j < GetLineCount(); j++)
        {
            m_List.at(j)->Destroy();
        }
        m_List.clear();
    }
    inline double Length() override
    {
        double l = 0.0;
        for(int j = 0; j < GetLineCount(); j++)
        {
            l += m_List.at(j)->Length();
        }

        return l;
    }

    inline LSMVector2 GetCentroid() override
    {
        LSMVector2 Centroid = LSMVector2(0.0,0.0);
        double area = 0.0;
        for(int j = 0; j < m_List.length(); j++)
        {
            double area_this = m_List.at(j)->Length();
            area += area_this;
            Centroid = Centroid + m_List.at(j)->GetCentroid()*area_this;
        }
        if(area > 0)
        {
            Centroid = Centroid/area;
        }
        return Centroid;
    }

    bool Contains(LSMVector2 pos, double tolerence)
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

    inline int GetVertexCount() override
    {
        int count = 0;
        for(int j = 0; j < m_List.size() ; j++)
        {
            count += m_List.at(j)->GetVertexCount();
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
    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMMultiLine*    AS_Assign            (LSMMultiLine*);


};


//Angelscript related functionality

inline void LSMMultiLine::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMMultiLine::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMMultiLine* LSMMultiLine::AS_Assign(LSMMultiLine* sh)
{

    return this;
}

inline static LSMMultiLine * AS_MultiLineFactory()
{
    return new LSMMultiLine();
}

