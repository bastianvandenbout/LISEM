#ifndef MULTITRIANGLE_H
#define MULTITRIANGLE_H

#include "defines.h"
#include "shape.h"
#include "triangle.h"
#include "QList"

class LISEM_API LSMMultiTriangle: public LSMShape
{

    QList<SPHTriangle*> m_List;

    BoundingBox m_BoundingBox;
public:
    inline LSMMultiTriangle() : LSMShape(LISEM_SHAPE_MULTITRIANGLE)
    {
    }

    inline void Add(SPHTriangle * p)
    {
        m_List.append(p);
        if(m_List.size() == 0)
        {
            m_BoundingBox = p->GetBoundingBox();
        }else {
            m_BoundingBox.MergeWith(p->GetBoundingBox());
        }
    }

    inline int GetTriangleCount()
    {
        return m_List.length();
    }

    inline SPHTriangle * GetTriangle(int i)
    {
        return m_List.at(i);
    }

    inline LSMShape * Copy() override
    {

        LSMMultiTriangle *mp = new LSMMultiTriangle();

        for(int j = 0; j < GetTriangleCount(); j++)
        {
            mp->Add((SPHTriangle *)GetTriangle(j)->Copy());
        }

        return mp;
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

        for(int j = 0; j < GetTriangleCount(); j++)
        {
            m_List.at(j)->Destroy();
        }
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


    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

};

#endif // MULTITRIANGLE_H
