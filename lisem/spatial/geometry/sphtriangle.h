#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "defines.h"
#include "QList"
#include "shape.h"

class LISEM_API SPHTriangle: public LSMShape
{

private:

    LSMVector2 m_P1;
    LSMVector2 m_P2;
    LSMVector2 m_P3;


    BoundingBox m_BoundingBox;

public:

    inline SPHTriangle() : LSMShape(LISEM_SHAPE_TRIANGLE)
    {

    }

    inline SPHTriangle(LSMVector2 p1, LSMVector2 p2, LSMVector2 p3) : LSMShape(LISEM_SHAPE_TRIANGLE)
    {
        Set(p1,p2,p3);
    }

    inline SPHTriangle(float x1, float y1, float x2, float y2, float x3, float y3) : LSMShape(LISEM_SHAPE_TRIANGLE)
    {
        Set(LSMVector2(x1,y1),LSMVector2(x2,y2),LSMVector2(x3,y3));
    }


    inline void Set(LSMVector2 p1, LSMVector2 p2, LSMVector2 p3)
    {
        m_P1 = p1;
        m_P2 = p2;
        m_P3 = p3;


        m_BoundingBox.SetAs(p1);
        m_BoundingBox.MergeWith(p2);
        m_BoundingBox.MergeWith(p3);
    }

    inline void Set(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        Set(LSMVector2(x1,y1),LSMVector2(x2,y2),LSMVector2(x3,y3));
    }



    inline LSMShape * Copy() override
    {

        SPHTriangle * p = new SPHTriangle();
        p->Set(m_P1,m_P2,m_P3);
        return p;
    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    float sign (LSMVector2 p1, LSMVector2 p2, LSMVector2 p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle (LSMVector2 pt, LSMVector2 v1, LSMVector2 v2, LSMVector2 v3)
    {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    virtual bool Contains(LSMVector2 pos, double tolerence)
    {
        return PointInTriangle(pos,m_P1,m_P2,m_P3);
    }


};

#endif // TRIANGLE_H
