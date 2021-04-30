#include "multipolygon.h"
#include "shape.h"

LSMShape * LSMMultiPolygon::Copy()
{

    LSMMultiPolygon *mp = new LSMMultiPolygon();

    for(int j = 0; j < GetPolygonCount(); j++)
    {
        mp->Add((LSMPolygon*)GetPolygon(j)->Copy());
    }
    return mp;
}


void LSMMultiPolygon::Add(LSMPolygon * p)
{
    m_List.append(p);
    if(m_List.size() == 1)
    {
        m_BoundingBox = p->GetBoundingBox();
    }else {
        m_BoundingBox.MergeWith(p->GetBoundingBox());
    }
}


void LSMMultiPolygon::Destroy()
{

    for(int j = 0; j < GetPolygonCount(); j++)
    {
        m_List.at(j)->Destroy();
    }
}

bool LSMMultiPolygon::Contains(LSMVector2 pos, double tolerence)
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
LSMVector2 LSMMultiPolygon::GetClosestEdgeLocationP(LSMVector2 pos, double **x1, double **y1, double **x2, double **y2)
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

LSMVector2 LSMMultiPolygon::GetClosestVertexP(LSMVector2 pos, double **x1, double **y1)
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

void LSMMultiPolygon::MoveVertex(double *x, double *y, LSMVector2 d)
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

void LSMMultiPolygon::Move(LSMVector2 d)
{
    m_BoundingBox.Move(d);
    for(int j = 0; j < m_List.length(); j++)
    {
        m_List.at(j)->Move(d);
    }
}

double LSMMultiPolygon::Area()
{
    double area = 0.0;
    for(int j = 0; j < m_List.length(); j++)
    {
        area += m_List.at(j)->Area();
    }
    return area;
}

double LSMMultiPolygon::Length()
{
    double length = 0.0;
    for(int j = 0; j < m_List.length(); j++)
    {
        length += m_List.at(j)->Length();
    }
    return length;
}


LSMVector2 LSMMultiPolygon::GetCentroid()
{
    LSMVector2 Centroid = LSMVector2(0.0,0.0);
    double area = 0.0;
    for(int j = 0; j < m_List.length(); j++)
    {
        double area_this = m_List.at(j)->Area();
        area += area_this;
        Centroid = Centroid + m_List.at(j)->GetCentroid()*area_this;
    }
    if(area > 0)
    {
        Centroid = Centroid/area;
    }
    return Centroid;
}


LSMVector2 LSMMultiPolygon::GetClosestEdgeLocation(LSMVector2 pos)
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
            closest = p;
        }else if(distance < mindist)
        {
            mindist = distance;
            closest = p;
        }
    }

    return closest;
}

LSMVector2 LSMMultiPolygon::GetClosestVertex(LSMVector2 pos)
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

int LSMMultiPolygon::GetVertexCount()
{
    int count = 0;
    for(int i = 0; i < m_List.size() ; i++)
    {
        count += m_List.at(i)->GetVertexCount();
    }
    return count;
}
LSMVector2 LSMMultiPolygon::GetVertex(int i)
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

void LSMMultiPolygon::GetVertexP(int i, double ** x , double **y)
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


void LSMMultiPolygon::RemoveVertex(int i)
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
