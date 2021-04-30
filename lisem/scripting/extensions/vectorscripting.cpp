#pragma once
#include "vectorscripting.h"
#include "geometry/shape.h"
#include "QString"
#include "geometry/line.h"
#include "geometry/point.h"
#include "geometry/polygon.h"
#include "geometry/multiline.h"
#include "geometry/multipoint.h"
#include "geometry/multipolygon.h"
#include "scriptarray.h"
#include "error.h"

void AS_LSMShape::AS_SetAsPoint(LSMVector2 v)
{
    Destroy();
    LSMPoint * p = new LSMPoint();
    p->SetPos(v.x,v.y,0.0);

    intern_InitializeFromShape(p);

}
void AS_LSMShape::AS_SetAsLine(std::vector<LSMVector2> v)
{
    Destroy();
    LSMLine * p = new LSMLine();

    std::vector<LSMVector2> ps = v;
    for(int i = 0; i < ps.size(); i++)
    {
        p->AddVertex(ps.at(i).x,ps.at(i).y);
    }
    intern_InitializeFromShape(p);
}
void AS_LSMShape::AS_SetAsLine(LSMVector2 v1, LSMVector2 v2)
{
    Destroy();
    LSMLine * p = new LSMLine();
    p->AddVertex(v1.x,v1.y);
    intern_InitializeFromShape(p);

}
void AS_LSMShape::AS_SetAsMultiPoint(std::vector<LSMVector2>v)
{
    Destroy();
    LSMMultiPoint * p = new LSMMultiPoint();

    std::vector<LSMVector2> ps = v;
    for(int i = 0; i < ps.size(); i++)
    {
        LSMPoint * pp = new LSMPoint();
        pp->SetPos(ps.at(i).x,ps.at(i).y);
        p->Add(pp);
    }
    intern_InitializeFromShape(p);

}
void AS_LSMShape::AS_SetAsMultiLine(std::vector<std::vector<LSMVector2>>v)
{
    Destroy();

    LSMMultiLine * p = new LSMMultiLine();

    for(int i = 0; i < v.size(); i++)
    {
        std::vector<LSMVector2> ps = v.at(i);

        LSMLine * pp = new LSMLine();
        for(int j = 0; j < ps.size(); j++)
        {
            pp->AddVertex(ps.at(j).x,ps.at(j).y);

        }
        p->Add(pp);
    }
    intern_InitializeFromShape(p);

}
void AS_LSMShape::AS_SetAsPolygon(std::vector<LSMVector2> v)
{
    Destroy();
    LSMPolygon * p = new LSMPolygon();

    std::vector<LSMVector2> ps = v;
    Ring * r= new Ring();
    for(int i = 0; i < ps.size(); i++)
    {
        r->AddVertex(ps.at(i).x,ps.at(i).y);
    }
    p->SetExtRing(r);
    intern_InitializeFromShape(p);
}
void AS_LSMShape::AS_SetAsPolygon(std::vector<LSMVector2> v, std::vector<LSMVector2>vin)
{
    Destroy();
    LSMPolygon * p = new LSMPolygon();

    std::vector<LSMVector2> ps = v;
    std::vector<LSMVector2> psi = vin;
    Ring * r= new Ring();

    for(int i = 0; i < ps.size(); i++)
    {
        r->AddVertex(ps.at(i).x,ps.at(i).y);
    }
    if(psi.size() > 0)
    {
        Ring * ri = new Ring();
        for(int i = 0; i < psi.size(); i++)
        {
            ri->AddVertex(psi.at(i).x,psi.at(i).y);
        }
        p->AddIntRing(ri);
    }

    p->SetExtRing(r);
    intern_InitializeFromShape(p);

}
void AS_LSMShape::AS_SetAsMultiPolygon(std::vector<std::vector<LSMVector2>> v)
{
    Destroy();

    LSMMultiPolygon * mp = new LSMMultiPolygon();

    for(int l = 0; l < v.size(); l++)
    {
        LSMPolygon * p = new LSMPolygon();

        std::vector<LSMVector2> ps = v.at(l);
        Ring * r= new Ring();
        for(int i = 0; i < ps.size(); i++)
        {
            r->AddVertex(ps.at(i).x,ps.at(i).y);
        }
        p->SetExtRing(r);
        mp->Add(p);
    }
    intern_InitializeFromShape(mp);

}
void AS_LSMShape::AS_SetAsMultiPolygon(std::vector<std::vector<LSMVector2>> v, std::vector<std::vector<LSMVector2>>vin)
{
    Destroy();

    LSMMultiPolygon * mp = new LSMMultiPolygon();

    if(!(v.size() == vin.size()))
    {
        LISEMS_ERROR("number of external and internal rings not equal");
        throw 1;
    }
    for(int l = 0; l < vin.size(); l++)
    {
        LSMPolygon * p = new LSMPolygon();

        std::vector<LSMVector2> ps = v.at(l);
        std::vector<LSMVector2> psi = vin.at(l);
        Ring * r= new Ring();
        for(int i = 0; i < ps.size(); i++)
        {
            r->AddVertex(ps.at(i).x,ps.at(i).y);
        }
        p->SetExtRing(r);

        if(psi.size() > 0)
        {
            Ring * ri = new Ring();
            for(int i = 0; i < psi.size(); i++)
            {
                ri->AddVertex(psi.at(i).x,psi.at(i).y);
            }
            p->AddIntRing(ri);
        }

        mp->Add(p);
    }
    intern_InitializeFromShape(mp);

}

int AS_LSMShape::GetSubItemCount()
{
    if(m_Shape == nullptr)
    {
        return 0;
    }

    if(!AS_IsMulti())
    {
        return 0;
    }else {

        int type = m_Shape->GetType();
        if(type == LISEM_SHAPE_MULTIPOINT)
        {
            LSMMultiPoint *msp = (LSMMultiPoint*)m_Shape;
            return msp->GetPointCount();

        }else if(type == LISEM_SHAPE_MULTILINE)
        {
            LSMMultiLine *msp = (LSMMultiLine*)m_Shape;
            return msp->GetLineCount();

        }else if(type == LISEM_SHAPE_MULTIPOLYGON)
        {
            LSMMultiPolygon *msp = (LSMMultiPolygon*)m_Shape;
            return msp->GetPolygonCount();

        }
    }

    return 0;

}
bool AS_LSMShape::AS_IsMulti()
{
    if(m_Shape == nullptr)
    {
        return false;
    }
    int type = m_Shape->GetType();
    if(type == LISEM_SHAPE_UNKNOWN)
    {
        return false;

    }else if(type == LISEM_SHAPE_POINT)
    {
        return false;
    }else if(type == LISEM_SHAPE_LINE)
    {
        return false;

    }else if(type == LISEM_SHAPE_POLYGON)
    {
        return false;

    }else if(type == LISEM_SHAPE_MULTIPOINT)
    {

       return true;

    }else if(type == LISEM_SHAPE_MULTILINE)
    {

       return true;
    }else if(type == LISEM_SHAPE_MULTIPOLYGON)
    {
        return true;
    }

    return false;


}
AS_LSMShape * AS_LSMShape::GetSubItem(int i)
{
    if(m_Shape == nullptr)
    {
        LISEMS_ERROR("Can not request subshape from empty shape type");
        throw 1;
    }

    if(!AS_IsMulti())
    {
        LISEMS_ERROR("Can not request subshape from non-multi shape type");
        throw 1;
    }else {

        int type = m_Shape->GetType();
        if(type == LISEM_SHAPE_MULTIPOINT)
        {
            LSMMultiPoint *msp = (LSMMultiPoint*)m_Shape;
            if(i < 0 || i > msp->GetPointCount() - 1)
            {
                LISEMS_ERROR("Can not get subsape, index out of bounds");
                throw 1;
            }
            return new AS_LSMShape(msp->GetPoint(i));

        }else if(type == LISEM_SHAPE_MULTILINE)
        {
            LSMMultiLine *msp = (LSMMultiLine*)m_Shape;
            if(i < 0 || i > msp->GetLineCount() - 1)
            {
                LISEMS_ERROR("Can not get subsape, index out of bounds");
                throw 1;
            }
            return new AS_LSMShape(msp->GetLine(i));

        }else if(type == LISEM_SHAPE_MULTIPOLYGON)
        {
            LSMMultiPolygon *msp = (LSMMultiPolygon*)m_Shape;
            if(i < 0 || i > msp->GetPolygonCount() - 1)
            {
                LISEMS_ERROR("Can not get subsape, index out of bounds");
                throw 1;
            }
            return new AS_LSMShape(msp->GetPolygon(i));

        }
    }

    return new AS_LSMShape();
}

std::vector<LSMVector2> AS_LSMShape::AS_GetVertices()
{


    std::vector<LSMVector2> array;
    if(m_Shape == nullptr)
    {
        return array;
    }



    int type = m_Shape->GetType();
    if(type == LISEM_SHAPE_POINT)
    {
        array.push_back(LSMVector2(((LSMPoint*)(m_Shape))->GetX(),((LSMPoint*)(m_Shape))->GetY()));

    }else if(type == LISEM_SHAPE_LINE)
    {
        LSMLine * l = (LSMLine *)(m_Shape);
        int count = l->GetVertexCount();
        double * data = l->GetVertexData();
        for(int i = 0; i < count; i++)
        {

            array.push_back(LSMVector2(data[i*2],data[i*2+1]));
        }

    }else if(type == LISEM_SHAPE_POLYGON)
    {
        LSMPolygon * p = (LSMPolygon*)(m_Shape);
        Ring * r = p->GetExtRing();
        int count = r->GetVertexCount();
        for(int i = 0;i < p->GetIntRingCount(); i++)
        {
            Ring * ir = p->GetIntRing(i);
            count += ir->GetVertexCount();
        }

        double * data = r->GetVertexData();
        int i = 0;
        for(i = 0; i < count; i++)
        {
            array.push_back(LSMVector2(data[i*2],data[i*2+1]));
        }
        for(int j = 0; j < p->GetIntRingCount(); j++)
        {
            Ring * ir = p->GetIntRing(j);
            double * datai = ir->GetVertexData();
            for(int k; k < ir->GetVertexCount(); k++)
            {
                array.push_back(LSMVector2(datai[k*2],datai[k*2+1]));
                i++;
            }
        }

    }else if(type == LISEM_SHAPE_MULTIPOINT)
    {
        LSMMultiPoint * l = (LSMMultiPoint*)(m_Shape);
        int pc = l->GetPointCount();
        for(int i = 0; i < pc; i++)
        {
            array.push_back(LSMVector2(l->GetPoint(i)->GetX(),l->GetPoint(i)->GetY()));
        }
    }else if(type == LISEM_SHAPE_MULTILINE)
    {

        LSMMultiLine * ml = (LSMMultiLine*)(m_Shape);
        int count = 0;
        for(int i = 0; i < ml->GetLineCount(); i++)
        {
            count += ml->GetLine(i)->GetVertexCount();
        }

        int ir = 0;
        for(int i = 0; i < ml->GetLineCount(); i++)
        {
            LSMLine * l = (LSMLine *)(ml->GetLine(i));
            count = l->GetVertexCount();
            double * data = l->GetVertexData();

            for(int j = 0; j < count; j++)
            {
                array.push_back(LSMVector2(data[j*2],data[j*2+1]));
                ir ++;
            }
        }

    }else if(type == LISEM_SHAPE_MULTIPOLYGON)
    {

        LSMMultiPolygon * ml = (LSMMultiPolygon*)(m_Shape);
        int count = 0;
        for(int o = 0; o < ml->GetPolygonCount(); o++)
        {
            LSMPolygon * p = (LSMPolygon*)(ml->GetPolygon(o));
            Ring * r = p->GetExtRing();

            count += r->GetVertexCount();
            for(int i = 0;i < p->GetIntRingCount(); i++)
            {
                Ring * ir = p->GetIntRing(i);
                count += ir->GetVertexCount();
            }

        }

        int ir = 0;
        for(int o = 0; o < ml->GetPolygonCount(); o++)
        {
            LSMPolygon * p = (LSMPolygon*)((ml->GetPolygon(o)));
            Ring * r = p->GetExtRing();

            double * data = r->GetVertexData();

            for(int i = 0; i < count; i++)
            {
                array.push_back(LSMVector2(data[i*2],data[i*2+1]));
                ir ++;
            }
            for(int j = 0; j < p->GetIntRingCount(); j++)
            {
                Ring * inr = p->GetIntRing(j);
                double * datai = inr->GetVertexData();
                for(int k; k < inr->GetVertexCount(); k++)
                {
                    array.push_back(LSMVector2(datai[k*2],datai[k*2+1]));
                    ir ++;
                }
            }

        }
    }
    return array;
}
AS_LSMShape * AS_LSMShape::AS_GetOuterRing()
{
    if(m_Shape != nullptr)
    {
        if(m_Shape->GetType() == LISEM_SHAPE_LINE)
        {
            return new AS_LSMShape(m_Shape->Copy());
        }else if(m_Shape->GetType() == LISEM_SHAPE_POLYGON)
        {
            LSMPolygon * p = (LSMPolygon*)(m_Shape);
            Ring * r = p->GetExtRing();
            if(r != nullptr)
            {
                LSMPolygon * retp = new LSMPolygon();
                retp->SetExtRing(r->Copy());
                return new AS_LSMShape(retp);
            }
        }
    }
    return new AS_LSMShape();
}
AS_LSMShape * AS_LSMShape::AS_GetInnerRing()
{
    if(m_Shape != nullptr)
    {

    }

    return new AS_LSMShape();

}
double AS_LSMShape::AS_GetRadius()
{

    if(m_Shape != nullptr)
    {

        if(m_Shape->GetType() == LISEM_SHAPE_POINT)
        {
            LSMPoint * p = (LSMPoint*)(m_Shape);
            return p->GetRadius();
        }
    }

    return 0.0;
}
bool AS_LSMShape::AS_IsEmpty()
{
    return m_Shape == nullptr;
}
LSMVector2 AS_LSMShape::AS_GetCentroid()
{
    if(m_Shape != nullptr)
    {
        return m_Shape->GetCentroid();
    }
    return LSMVector2(0.0,0.0);

}
bool AS_LSMShape::AS_Contains(LSMVector2 v)
{
    if(m_Shape != nullptr)
    {
        return m_Shape->Contains(v,0);
    }
    return false;

}
double AS_LSMShape::AS_GetLength()
{
    if(m_Shape != nullptr)
    {
        return m_Shape->Length();
    }
    return 0.0;
}
double AS_LSMShape::AS_GetArea()
{
    if(m_Shape != nullptr)
    {
        return m_Shape->Area();
    }
    return 0.0;

}
void AS_LSMShape::AS_Move(LSMVector2 m)
{
    if(m_Shape != nullptr)
    {
        return m_Shape->Move(m);
    }
    return;


}
LSMVector2 AS_LSMShape::AS_GetClosestVertex(LSMVector2 v)
{
    if(m_Shape == nullptr)
    {
        return v;
    }

    return m_Shape->GetClosestVertex(v);

}
LSMVector2 AS_LSMShape::AS_GetClosestEdgePoint(LSMVector2 v)
{
    if(m_Shape == nullptr)
    {
        return v;
    }

    return m_Shape->GetClosestEdgeLocation(v);

}
BoundingBox AS_LSMShape::AS_GetBoundingBox()
{
    if(m_Shape != nullptr)
    {
        return m_Shape->GetBoundingBox();
    }else {
        return BoundingBox();
    }
}


QString AS_LSMShape::AS_GetShapeType()
{
    if(m_Shape == nullptr)
    {
        return "none";
    }
    int type = m_Shape->GetType();
    if(type == LISEM_SHAPE_UNKNOWN)
    {
        return "none";

    }else if(type == LISEM_SHAPE_POINT)
    {
        return "point";
    }else if(type == LISEM_SHAPE_LINE)
    {
        return "line";

    }else if(type == LISEM_SHAPE_POLYGON)
    {
        return "polygon";

    }else if(type == LISEM_SHAPE_MULTIPOINT)
    {

       return "multipoint";

    }else if(type == LISEM_SHAPE_MULTILINE)
    {

       return "multiline";
    }else if(type == LISEM_SHAPE_MULTIPOLYGON)
    {
        return "multipolygon";
    }

    return "none";


}
