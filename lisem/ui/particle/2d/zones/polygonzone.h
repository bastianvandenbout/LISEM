#ifndef POLYGONZONEH
#define POLYGONZONEH

#include "linear/lsm_vector3.h"
#include "linear/lsm_vector2.h"
#include "boundingbox.h"
#include "geometry/polygon.h"
#include "geometry/multipolygon.h"

#include "random/randomnumbers.h"

#include "particle/common/zone.h"

namespace LISEM
{

class PolygonZone : public Zone
{

    Ring * m_Ext_Ring = nullptr;
    QList<Ring*> m_Int_Ring;
    std::vector<LSMVector2> m_Points;
    BoundingBox m_BoundingBox;
    LSMMultiPolygon * m_Triangulation;
    double m_Area = 0.0;

public:

    inline PolygonZone(std::vector<LSMVector2> points)
    {
        m_Point = points;
        m_Ext_Ring = new Ring(points);

        if(r->GetVertexCount() > 0)
        {
            m_BoundingBox.SetAs(r->GetVertex(0));
            for(int i = 1; i < r->GetVertexCount(); i++)
            {
                m_BoundingBox.MergeWith(r->GetVertex(i));
            }
        }

        //set up this polygon in required structure


        LSMMultiPolygon * mp = new LSMMultiPolygon();

        std::vector<std::vector<std::array<double,2>>> polygon;
        std::vector<std::array<double,2>> polygonflat;
        std::vector<std::array<double,2>> ring_ext;

        double area_total = 0.0;
        if(m_Ext_Ring != nullptr)
        {
            int nvertex = m_Ext_Ring->GetVertexCount();
            double*vertexdata = m_Ext_Ring->GetVertexData();
            for(int i= 0; i < nvertex; i++)
            {
                ring_ext.push_back({vertexdata[i*2],vertexdata[i*2+1]});
                polygonflat.push_back({vertexdata[i*2],vertexdata[i*2+1]});
            }
            polygon.push_back(ring_ext);


            for(int i = 0; i < GetIntRingCount(); i++)
            {
                Ring * rint = GetIntRing(i);
                int nvertex = rint->GetVertexCount();
                double*vertexdata = rint->GetVertexData();
                std::vector<std::array<double,2>> ring_int;
                for(int i= 0; i < nvertex; i++)
                {
                    ring_int.push_back({vertexdata[i*2],vertexdata[i*2+1]});
                    polygonflat.push_back({vertexdata[i*2],vertexdata[i*2+1]});
                }

                polygon.push_back(ring_int);
            }
            //run the triangulation algorithm

            std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);

            //get back all the triangles

            for(int i = 0; i < (indices.size()+1)/3; i++)
            {
                double x1 = polygonflat.at(indices.at(i*3 + 0)).at(0);
                double x2 = polygonflat.at(indices.at(i*3 + 1)).at(0);
                double x3 = polygonflat.at(indices.at(i*3 + 2)).at(0);
                double y1 = polygonflat.at(indices.at(i*3 + 0)).at(1);
                double y2 = polygonflat.at(indices.at(i*3 + 1)).at(1);
                double y3 = polygonflat.at(indices.at(i*3 + 2)).at(1);

                LSMPolygon * pp = new LSMPolygon(x1,y1,x2,y2,x3,y3);
                area_total +=pp->Area();
                mp->Add(pp);
            }
        }

        m_Area = area_total;
        m_Triangulation = mp;

    }

    inline virtual LSMVector3 GetRandomPoint()
    {
        float triangle_random =  GetRandom(0.0,m_Area);
        int index = 0;
        double area_sofar = 0.0;
        for(int i =0; i < m_Triangulation->GetPolygonCount(); i++)
        {
            LSMPolygon * p = m_Triangulation->GetPolygon(i);
            area_sofar += p->Area();

            if(triangle_random < area_sofar)
            {
                index = i;
                break;
            }
        }

        LSMVector2 p1 = p->GetRing(0)->GetVertex(0);
        LSMVector2 p2 = p->GetRing(0)->GetVertex(1);
        LSMVector2 p3 = p->GetRing(0)->GetVertex(2);

        float randomu =  GetRandom(0.0,1.0);
        float randomv =  GetRandom(0.0,1.0);

        if(randomu + randomv > 1.0)
        {
            randomu = 1.0-randomu;
            randomv = 1.0-randomv;
        }

        LSMVector2 res= p1 + randomu * (p2-p1) + randomv * (p3-p1);

        return LSMVector3(res.x, 0.0,res.y);
    }
    inline virtual double GetZoneArea()
    {
        return 0.0;
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

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline virtual bool Contains(LSMVector3 xin)
    {
        LSMVector2 point = LSMVector2(xin.x,xin.z);

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

    inline virtual double DistanceTo(LSMVector3 x)
    {
        return (x - m_Point).length();
    }

};

}

#endif
