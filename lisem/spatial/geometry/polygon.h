#pragma once
#include "defines.h"
#include "QList"
#include "shape.h"
#include "ring.h"
#include "multipolygon.h"
#include "iostream"
#include "geometry/geometrybase.h"

class LSMMultiPolygon;

class LISEM_API LSMPolygon: public LSMShape
{

    Ring * m_Ext_Ring = nullptr;
    QList<Ring*> m_Int_Ring;

    BoundingBox m_BoundingBox;

public:
    inline LSMPolygon() : LSMShape(LISEM_SHAPE_POLYGON)
    {
    }

    inline LSMPolygon(double x1, double y1, double x2, double y2, double x3, double y3) : LSMShape(LISEM_SHAPE_POLYGON)
    {
        m_Ext_Ring = new Ring();
        m_Ext_Ring->AddVertex(x1,y1);
        m_Ext_Ring->AddVertex(x2,y2);
        m_Ext_Ring->AddVertex(x3,y3);

        m_BoundingBox.SetAs(LSMVector2(x1,y1));
        m_BoundingBox.MergeWith(LSMVector2(x2,y2));
        m_BoundingBox.MergeWith(LSMVector2(x3,y3));

    }

    inline int GetRingCount()
    {

        return 1 + m_Int_Ring.size();

    }

    inline Ring* GetRing(int i)
    {
        if(i == 0)
        {
            return m_Ext_Ring;
        }else {
        return m_Int_Ring.at(i-1);
        }

    }


    inline void SetExtRing(Ring * r)
    {
        m_Ext_Ring = r;

        if(r->GetVertexCount() > 0)
        {
            m_BoundingBox.SetAs(r->GetVertex(0));
            for(int i = 1; i < r->GetVertexCount(); i++)
            {
                m_BoundingBox.MergeWith(r->GetVertex(i));
            }
        }

    }


    inline void SetAsRegularNGon(LSMVector2 center, double radius, int n_points = 25)
    {

        Ring * r = new Ring();
        for(int i = 0; i < n_points; i++)
        {
            double thetai = float(i) * 2.0 * 3.14159/((float)(n_points));
            double thetaip = float(i+1) * 2.0 * 3.14159/((float)(n_points));

            double x_i = center.x + radius*sin(thetai);
            double y_i = center.y + radius*cos(thetai);

            double x_ip = center.x + radius*sin(thetaip);
            double y_ip = center.y + radius*cos(thetaip);

            r->AddVertex(x_i,y_i);
        }

        this->SetExtRing(r);
    }

    inline void SetAsSquare(LSMVector2 tl, LSMVector2 br)
    {
        Ring * r = new Ring();
        r->AddVertex(tl.x,tl.Y());
        r->AddVertex(tl.x,br.Y());
        r->AddVertex(br.x,br.Y());
        r->AddVertex(br.x,tl.Y());
        r->AddVertex(tl.x,tl.Y());
        this->SetExtRing(r);
    }

    inline void AddIntRing(Ring * r)
    {

        m_Int_Ring.append(r);
    }

    inline int GetIntRingCount()
    {
        return m_Int_Ring.length();

    }

    inline Ring * GetIntRing(int i)
    {
        return m_Int_Ring.at(i);
    }

    inline Ring * GetExtRing()
    {
        return m_Ext_Ring;
    }

    inline LSMShape * Copy() override
    {

        LSMPolygon * p = new LSMPolygon();

        if(m_Ext_Ring != nullptr)
        {
            p->SetExtRing(m_Ext_Ring->Copy());
        }

        for(int i = 0; i < GetIntRingCount(); i++)
        {
            if(GetIntRing(i) != nullptr)
            {
                p->AddIntRing(GetIntRing(i)->Copy());
            }
        }

        return p;
    }

    inline void Destroy() override
    {
        if(m_Ext_Ring != nullptr)
        {
            delete m_Ext_Ring;
        }

        for(int i = 0; i < GetIntRingCount(); i++)
        {
            if(GetIntRing(i) != nullptr)
            {
                delete GetIntRing(i);
            }
        }
        m_Int_Ring.clear();

    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline float direction(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
        return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
    }

    inline bool onSegment(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
        if (std::min(pi.x, pj.x) <= pk.x && pk.x <= std::max(pi.x, pj.x)
            && std::min(pi.y, pj.y) <= pk.y && pk.y <= std::max(pi.y, pj.y)) {
            return true;
        } else {
            return false;
        }
    }

    inline bool segmentIntersect(LSMVector2 p1, LSMVector2 p2, LSMVector2 p3, LSMVector2 p4) {
        float d1 = direction(p3, p4, p1);
        float d2 = direction(p3, p4, p2);
        float d3 = direction(p1, p2, p3);
        float d4 = direction(p1, p2, p4);

        if (((d1 > 0.0 && d2 <  0.0) || (d1 <  0.0 && d2 >  0.0)) && ((d3 >  0.0 && d4 <  0.0) || (d3 <  0.0 && d4 >  0.0))) {
            return true;
        } else if (d1 ==  0.0 && onSegment(p3, p4, p1)) {
            return true;
        } else if (d2 ==  0.0 && onSegment(p3, p4, p2)) {
            return true;
        } else if (d3 ==  0.0 && onSegment(p1, p2, p3)) {
            return true;
        } else if (d4 ==  0.0 && onSegment(p1, p2, p4)) {
            return true;
        } else {
            return false;
        }
    }

    virtual bool Contains(LSMVector2 point, double tolerence)
    {
        BoundingBox bbox = GetBoundingBox();

        //create a ray (segment) starting from the given point,
       //and to the point outside of polygon.
       LSMVector2 outside(bbox.GetMinX() - 1.0, bbox.GetMinY());
       int intersections = 0;
       //check intersections between the ray and every side of the polygon.
       if(m_Ext_Ring != nullptr)
       {
           //check for intersections with internal ring
           for (int i = 0; i < m_Ext_Ring->GetVertexCount() - 1; ++i) {
           if (segmentIntersect(point, outside,m_Ext_Ring->GetVertex(i), m_Ext_Ring->GetVertex(i + 1))) {
                   intersections++;
               }
           }
           //check the last line
           if (segmentIntersect(point, outside,m_Ext_Ring->GetVertex(m_Ext_Ring->GetVertexCount() - 1), m_Ext_Ring->GetVertex(0))) {
               intersections++;
           }

           //check for intersections with internal rings
           for(int j = 0; j < m_Int_Ring.length(); j++)
           {
               Ring * rint = m_Int_Ring.at(j);


               for (int i = 0; i < rint->GetVertexCount() - 1; ++i) {
               if (segmentIntersect(point, outside,rint->GetVertex(i), rint->GetVertex(i + 1))) {
                       intersections++;
                   }
               }
               //check the last line
               if (segmentIntersect(point, outside,rint->GetVertex(rint->GetVertexCount() - 1), rint->GetVertex(0))) {
                   intersections++;
               }

           }
           return (intersections % 2 != 0);
       }

        return false;
    }


    LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;

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

        if(m_Ext_Ring != nullptr)
        {
            double ** x1t = new (double*);
            double ** y1t = new (double*);
            double ** x2t = new (double*);
            double ** y2t = new (double*);

            double ** x1t2 = new (double*);
            double ** y1t2 = new (double*);
            double ** x2t2 = new (double*);
            double ** y2t2 = new (double*);

            LSMVector2 p = m_Ext_Ring->GetClosestEdgeLocationP(pos,x1t,y1t,x2t,y2t);

            mindist = LSMVector2(pos - p).length();
            closest = p;


            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                LSMVector2 p = rint->GetClosestEdgeLocationP(pos,x1t2,y1t2,x2t2,y2t2);
                float distance = LSMVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;

                    *x1t = *x1t2;*y1t = *y1t2;*x2t = *x2t2;*y2t = *y2t2;
                }
            }

            if(x1 != nullptr)
            {
                *x1 = *x1t;
            }
            if(x2 != nullptr)
            {
                *x2 = *x2t;
            }
            if(y1 != nullptr)
            {
                *y1 = *y1t;
            }
            if(y2 != nullptr)
            {
                *y2 = *y2t;
            }
            delete x1t;delete y1t; delete x2t; delete y2t;
            delete x1t2;delete y1t2; delete x2t2; delete y2t2;

        }
        return closest;
    }

    LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }

        if(m_Ext_Ring != nullptr)
        {

            double ** x1t = new (double*);
            double ** y1t = new (double*);

            double ** x1t2 = new (double*);
            double ** y1t2 = new (double*);

            LSMVector2 p = m_Ext_Ring->GetClosestVertexP(pos,x1t,y1t);


            mindist = LSMVector2(pos - p).length();
            closest = p;

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                LSMVector2 p = rint->GetClosestVertexP(pos,x1t2,y1t2);
                float distance = LSMVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;

                    *x1t = *x1t2;*y1t = *y1t2;
                }
            }

            if(x1 != nullptr)
            {
                *x1 = *x1t;
            }
            if(y1 != nullptr)
            {
                *y1 = *y1t;
            }
            delete x1t;delete y1t;
            delete x1t2;delete y1t2;

        }

        return closest;
    }

    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;
        if(m_Ext_Ring != nullptr)
        {

            LSMVector2 p = m_Ext_Ring->GetClosestEdgeLocation(pos);

            mindist = LSMVector2(pos - p).length();
            closest = p;


            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                LSMVector2 p = rint->GetClosestEdgeLocation(pos);
                float distance = LSMVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;
                }
            }
        }
        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;
        if(m_Ext_Ring != nullptr)
        {
            LSMVector2 p = m_Ext_Ring->GetClosestVertex(pos);

            mindist = LSMVector2(pos - p).length();
            closest = p;

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                Ring * rint = m_Int_Ring.at(j);

                LSMVector2 p = rint->GetClosestVertex(pos);
                float distance = LSMVector2(pos - p).length();

                if(distance < mindist)
                {
                    closest = p;
                    mindist = distance;
                }
            }
        }

        return closest;
    }

    virtual void MoveVertex(double *x, double *y, LSMVector2 d) override
    {
        if(m_Ext_Ring != nullptr)
        {
            m_Ext_Ring->MoveVertex(x,y,d);
            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                m_Int_Ring.at(j)->MoveVertex(x,y,d);
            }
            Ring * r = m_Ext_Ring;
            if(r->GetVertexCount() > 0)
            {
                m_BoundingBox.SetAs(r->GetVertex(0));
                for(int i = 1; i < r->GetVertexCount(); i++)
                {
                    m_BoundingBox.MergeWith(r->GetVertex(i));
                }
            }
        }
    }

    inline void Move(LSMVector2 d) override
    {

        m_BoundingBox.Move(d);
        if(m_Ext_Ring != nullptr)
        {
            m_Ext_Ring->Move(d);
            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                m_Int_Ring.at(j)->Move(d);
            }

        }

    }


    inline int GetVertexCount()
    {
        int count = 0;
        if(m_Ext_Ring != nullptr)
        {
            count += m_Ext_Ring->GetVertexCount();

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                count += m_Int_Ring.at(j)->GetVertexCount();
            }
        }

        return count;
    }
    inline LSMVector2 GetVertex(int i)
    {
        int count = 0;
        if(m_Ext_Ring != nullptr)
        {
            if( i < m_Ext_Ring->GetVertexCount())
            {
                return m_Ext_Ring->GetVertex(i);
            }else {
                i = i -m_Ext_Ring->GetVertexCount();
            }

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                if( i < m_Int_Ring.at(j)->GetVertexCount())
                {
                    return m_Int_Ring.at(j)->GetVertex(i);
                }else {
                    i = i -m_Int_Ring.at(j)->GetVertexCount();
                }
            }
        }
        return LSMVector2(0.0f,0.0f);

    }
    inline void GetVertexP(int i, double ** x , double **y)
    {
        int count = 0;
        if(m_Ext_Ring != nullptr)
        {
            if( i < m_Ext_Ring->GetVertexCount())
            {

                m_Ext_Ring->GetVertexP(i,x,y);
                return;
            }else {
                i = i -m_Ext_Ring->GetVertexCount();
            }

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                if( i < m_Int_Ring.at(j)->GetVertexCount())
                {
                    m_Int_Ring.at(j)->GetVertexP(i,x,y);
                    return;
                }else {
                    i = i -m_Int_Ring.at(j)->GetVertexCount();
                }
            }
        }

        *x = nullptr;
        *y = nullptr;
    }

    inline  void RemoveVertex(int i)
    {
        if(m_Ext_Ring != nullptr)
        {
            if( i < m_Ext_Ring->GetVertexCount())
            {

                m_Ext_Ring->RemoveVertex(i);
                return;
            }else {
                i = i -m_Ext_Ring->GetVertexCount();
            }

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                if( i < m_Int_Ring.at(j)->GetVertexCount())
                {
                    m_Int_Ring.at(j)->RemoveVertex(i);
                    return;
                }else {
                    i = i -m_Int_Ring.at(j)->GetVertexCount();
                }
            }
        }

    }

    //input is polygon (provided in non-closed form)
    inline bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {
        bool intersect = false;
        int count = 0;
        if(m_Ext_Ring != nullptr)
        {
            {
                int vcount =  m_Ext_Ring->GetVertexCount();

                for(int i = 0; i < vcount; i++)
                {
                    if(i != vcount - 1)
                    {
                        intersect = intersect || Geometry_PolygonIntersectsLine(x,y,m_Ext_Ring->GetVertex(i).x,m_Ext_Ring->GetVertex(i).y,m_Ext_Ring->GetVertex(i+1).x,m_Ext_Ring->GetVertex(i+1).y);
                        intersect = intersect || Geometry_PolygonContaintsPoint(x,y,m_Ext_Ring->GetVertex(i).x,m_Ext_Ring->GetVertex(i).y);
                    }else {
                        intersect = intersect || Geometry_PolygonIntersectsLine(x,y,m_Ext_Ring->GetVertex(i).x,m_Ext_Ring->GetVertex(i).y,m_Ext_Ring->GetVertex(0).x,m_Ext_Ring->GetVertex(0).y);
                        intersect = intersect || Geometry_PolygonContaintsPoint(x,y,m_Ext_Ring->GetVertex(i).x,m_Ext_Ring->GetVertex(i).y);
                    }

                    if(intersect)
                    {
                        return true;
                    }
                }
            }

            //check for intersections with internal rings
            for(int j = 0; j < m_Int_Ring.length(); j++)
            {
                {
                    int vcount =  m_Int_Ring.at(j)->GetVertexCount();

                    for(int i = 0; i < vcount; i++)
                    {
                        if(i != vcount - 1)
                        {
                            intersect = intersect || Geometry_PolygonIntersectsLine(x,y,m_Int_Ring.at(j)->GetVertex(i).x,m_Int_Ring.at(j)->GetVertex(i).y,m_Int_Ring.at(j)->GetVertex(i+1).x,m_Int_Ring.at(j)->GetVertex(i+1).y);
                            intersect = intersect || Geometry_PolygonContaintsPoint(x,y,m_Int_Ring.at(j)->GetVertex(i).x,m_Int_Ring.at(j)->GetVertex(i).y);
                        }else {
                            intersect = intersect || Geometry_PolygonIntersectsLine(x,y,m_Int_Ring.at(j)->GetVertex(i).x,m_Int_Ring.at(j)->GetVertex(i).y,m_Int_Ring.at(j)->GetVertex(0).x,m_Int_Ring.at(j)->GetVertex(0).y);
                            intersect = intersect || Geometry_PolygonContaintsPoint(x,y,m_Int_Ring.at(j)->GetVertex(i).x,m_Int_Ring.at(j)->GetVertex(i).y);
                        }

                        if(intersect)
                        {
                            return true;
                        }
                    }
                }
            }
        }

        return false;

    }
    QList<LSMShape *> Split(std::vector<double> & x,std::vector<double> & y);




    inline double Length() override
    {
        if(m_Ext_Ring == nullptr)
        {
            return 0.0;
        }

        double length_total = 0.0;

        length_total += m_Ext_Ring->Length();

        for(int k = 0; k < GetIntRingCount(); k++)
        {
            Ring * r = GetIntRing(k);
            length_total += r->Length();

        }
        return length_total;
    }

    const LSMVector2 findCentroid(double* pts, size_t nPts){
        LSMVector2 off = {(float)pts[0],(float)pts[1]};
        float twicearea = 0;
        float x = 0;
        float y = 0;
        LSMVector2 p1, p2;
        float f;
        for (int i = 0, j = nPts - 1; i < nPts; j = i++) {
            p1 = LSMVector2((float)pts[2*i],(float)pts[2*i+1]);
            p2 = LSMVector2((float)pts[2*j],(float)pts[2*j+1]);
            f = (p1.x - off.x) * (p2.y - off.y) - (p2.x - off.x) * (p1.y - off.y);
            twicearea += f;
            x += (p1.x + p2.x - 2.0 * off.x) * f;
            y += (p1.y + p2.y - 2.0 * off.y) * f;
        }

        f = twicearea * 3;

        return LSMVector2(x / f + off.x, y / f + off.y);
    }

    inline LSMVector2 GetCentroid() override
    {
        if(m_Ext_Ring == nullptr)
        {
            return LSMVector2(0.0,0.0);
        }

        if(m_Ext_Ring->GetVertexCount() < 3)
        {
            return LSMVector2(0.0,0.0);
        }

        return findCentroid(m_Ext_Ring->GetVertexData(),m_Ext_Ring->GetVertexCount());

    }


    inline double Area() override
    {
        if(m_Ext_Ring == nullptr)
        {
            return 0.0;
        }

        double area_total = 0.0;
        {

            int points = m_Ext_Ring->GetVertexCount();
            double *data = m_Ext_Ring->GetVertexData();

              double  area=0. ;
              int     i, j=points-1  ;

              for (i=0; i<points; i++)
              {
                area+=(data[j*2]+data[i* 2])*(data[j*2 + 1]-data[i* 2 + 1]); j=i;
              }
              area_total += std::fabs(area*.5);
        }

        for(int k = 0; k < GetIntRingCount(); k++)
        {
            Ring * r = GetIntRing(k);
            {
                int points = r->GetVertexCount();
                double *data = r->GetVertexData();

                  double  area=0. ;
                  int     i, j=points-1  ;

                  for (i=0; i<points; i++)
                  {
                    area+=(data[j*2]+data[i* 2])*(data[j*2 + 1]-data[i* 2 + 1]); j=i;
                  }
                  area_total -= std::fabs(area*.5);
            }
        }

          return std::max(0.0,area_total);

    }

    LSMMultiPolygon * Triangulate();


    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMPolygon*    AS_Assign            (LSMPolygon*);


};


//Angelscript related functionality

inline void LSMPolygon::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMPolygon::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMPolygon* LSMPolygon::AS_Assign(LSMPolygon* sh)
{

    return this;
}

inline static LSMPolygon * AS_PolygonFactory()
{
    return new LSMPolygon();
}
