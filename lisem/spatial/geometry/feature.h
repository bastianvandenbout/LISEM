#pragma once

#include "defines.h"
#include "shape.h"
#include "polygon.h"
#include "point.h"
#include "line.h"
#include "multiline.h"
#include "multipoint.h"
#include "multipolygon.h"
#include <iostream>
#include "error.h"
#include <QList>
#include "geo/shapes/glgeometrylist.h"
#include "ogr_geometry.h"
#include "functional"

class LISEM_API Feature
{
private:

    QList<LSMShape*> m_ShapeList;
    int m_index = -1;

public:

    inline Feature()
    {

    }
    inline Feature(const Feature & f)
    {
        this->CopyFrom(&f);
    }


    inline int GetIndex()
    {
        return m_index;
    }

    inline void RemoveShape(int i)
    {

        m_ShapeList.removeAt(i);
        GetBoundingBox();
    }

    inline void RemoveShape(LSMShape*s)
    {
        m_ShapeList.removeOne(s);
        GetBoundingBox();
    }

    inline void ReplaceShape(LSMShape*s, LSMShape * s2)
    {
        int i = m_ShapeList.indexOf(s);
        if( i > -1)
        {
            m_ShapeList.replace(i,s2);
            GetBoundingBox();
        }
    }



    inline int GetShapeCount()
    {
        return m_ShapeList.length();
    }

    inline void AddShape(LSMShape*s)
    {
        m_ShapeList.append(s);
    }
    inline LSMShape * GetShape(int i)
    {

        return m_ShapeList.at(i);
    }

    inline Feature * Copy()
    {
        Feature * f = new Feature();

        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            f->AddShape(m_ShapeList.at(i)->Copy());
        }
        return f;
    }

    inline void CopyFrom(const Feature * f)
    {
        Destroy();
        for(int i = 0; i < f->m_ShapeList.length(); i++)
        {
            AddShape(f->m_ShapeList.at(i)->Copy());
        }
    }

    inline void Destroy()
    {
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            m_ShapeList.at(i)->Destroy();
            delete m_ShapeList.at(i);
        }
        m_ShapeList.clear();

    }

    inline BoundingBox GetBoundingBox()
    {
        BoundingBox b;
        if(m_ShapeList.length() > 0)
        {
            b = m_ShapeList.at(0)->GetBoundingBox();
            for(int i = 1; i < m_ShapeList.length(); i++)
            {
                b.MergeWith(m_ShapeList.at(i)->GetBoundingBox());


            }
        }

        return b;
    }

    inline bool Contains(LSMVector2 pos, double tolerence)
    {

        //check for all shapes
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            LSMShape * s = m_ShapeList.at(i);
            if(s->GetBoundingBox().Contains(pos.x,pos.y,tolerence))
            {
                if(s->Contains(pos, tolerence))
                {
                    return true;
                }
            }
        }

        return false;
    }

    inline void Move(LSMVector2 m)
    {
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            LSMShape * s = m_ShapeList.at(i);
            s->Move(m);
        }
    }

    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos, LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }

        for(int j = 0; j < m_ShapeList.length(); j++)
        {
            LSMVector2 p = m_ShapeList.at(j)->GetClosestEdgeLocation(pos);
            float distance = LSMVector2(pos - p).length();
            if(!std::isnan(p.x) && !std::isnan(p.y) && std::isfinite(p.x) && std::isfinite(p.y))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr)
                    {
                        *s_ret = m_ShapeList.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr)
                    {
                        *s_ret = m_ShapeList.at(j);
                    }
                }
            }

        }

        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos, LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }
        for(int j = 0; j < m_ShapeList.length(); j++)
        {
            LSMVector2 p = m_ShapeList.at(j)->GetClosestVertex(pos);
            float distance = LSMVector2(pos - p).length();
            if(j == 0)
            {
                mindist = distance;
                closest = p;
                if(s_ret != nullptr)
                {
                    *s_ret = m_ShapeList.at(j);
                }
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = p;
                if(s_ret != nullptr)
                {
                    *s_ret = m_ShapeList.at(j);
                }
            }
        }
        return closest;
    }



    inline bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {
        return GetOverlappingShapes(x,y).length() > 0;
    }

    inline QList<std::pair<LSMShape*,Feature*>> GetOverlappingShapes(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::pair<LSMShape*,Feature*>> l;
        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            LSMShape * s = m_ShapeList.at(i);
            if(s->Overlaps(x,y))
            {
                l.append(std::pair<LSMShape*,Feature*>(s,this));
            }
        }

        std::cout << "get overlapping shapes " << l.size() << std::endl;
        return l;
    }
    inline QList<std::tuple<double*,double*,LSMShape*,Feature*>> GetOverlappingVertices(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::tuple<double*,double*,LSMShape*,Feature*>> l;

        for(int i = 0; i < m_ShapeList.length(); i++)
        {
            LSMShape * s = m_ShapeList.at(i);
            int count = m_ShapeList.at(i)->GetVertexCount();
            for(int j = 0; j < count; j++)
            {
                LSMVector2 p = (s->GetVertex(j));
                if(Geometry_PolygonContaintsPoint(x,y,p.x,p.y))
                {
                    double *xv;
                    double *yv;

                    Feature * f = this;

                    s->GetVertexP(j,&xv,&yv);

                    std::cout << "got vertex " << xv << " " << yv << std::endl;

                    l.push_back(std::tuple<double*,double*,LSMShape*,Feature*>(xv,yv,s,f));
                }
            }
        }
        return l;
    }


    inline LSMVector2 GetCentroid()
    {
        LSMVector2 centroid;
        double area = 0.0;
        for(int k = 0; k < m_ShapeList.length(); k++)
        {
            LSMShape * s = m_ShapeList.at(k);
            area += 1.0;
            centroid = centroid + s->GetCentroid();
        }
        if(area > 0.0)
        {
            centroid = centroid/area;
        }
        return centroid;
    }

    inline double Area()
    {
        double area = 0.0;
        for(int k = 0; k < m_ShapeList.length(); k++)
        {
            LSMShape * s = m_ShapeList.at(k);
            area += s->Area();
        }

        return area;
    }

    inline double Length()
    {
        double length = 0.0;
        for(int k = 0; k < m_ShapeList.length(); k++)
        {
            LSMShape * s = m_ShapeList.at(k);
            length += s->Length();
        }

        return length;


    }


    inline void AddToGlGeometryList(GLGeometryList * gl_list,int index)
    {
        for(int k = 0; k < m_ShapeList.length(); k++)
        {
            LSMShape * s = m_ShapeList.at(k);

            AddShapeToGlGeometryList(gl_list,index,s);
        }
    }

    inline int GetType()
    {
        if(m_ShapeList.length() > 0)
        {
            return m_ShapeList.at(0)->GetType();

        }

        return LISEM_SHAPE_UNKNOWN;
    }

    static inline void AddShapeToGlGeometryList(GLGeometryList * gl_list,int index,LSMShape * s)
    {
        if(s == nullptr)
        {
            return;
        }

        int type = s->GetType();
        if(type == LISEM_SHAPE_UNKNOWN)
        {

        }else if(type == LISEM_SHAPE_POINT)
        {
            LSMPoint * sp = (LSMPoint*) s;

            float x = sp->GetX();
            float y = sp->GetY();

            gl_list->AddPoint_Point(x,y);
            gl_list->AddVertexPoint(index,x,y);

            for(int i = 0; i < 10 ; i++)
            {
                float thetai = float(i) * 2.0f * 3.14159f/10.0f;
                float thetaip = float(i+1) * 2.0f * 3.14159f/10.0f;

                float x_i = x + sin(thetai);
                float y_i = y + cos(thetai);

                float x_ip = x + sin(thetaip);
                float y_ip = y + cos(thetaip);

                bool connect = (i > 0);

                gl_list->AddPoint_Line(index,x_i,y_i,x_ip,y_ip,x,y, connect);
                gl_list->AddPoint_Triangle(index,x_i,y_i,x_ip,y_ip,x,y,x,y);
            }

        }else if(type == LISEM_SHAPE_LINE)
        {
            LSMLine * sp = (LSMLine*) s;

            int nvertex = sp->GetVertexCount();
            double * vertices = sp->GetVertexData();

            for(int i = 0; i < nvertex; i++)
            {
                gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
            }

            for(int i = 0; i < nvertex - 1; i++)
            {
                bool connect = (i > 0);

                gl_list->AddLine_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
            }

        }else if(type == LISEM_SHAPE_POLYGON)
        {
            LSMPolygon * sp = (LSMPolygon*) s;

            LSMMultiPolygon * mp = sp->Triangulate();

            Ring * ext_ring = sp->GetExtRing();
            if(ext_ring != nullptr){
                double * vertices = ext_ring->GetVertexData();
                int nvertex = ext_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = (i > 0);
                    if(i == nvertex-1)
                    {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                    }else {
                        gl_list->AddPolygon_Line(index,vertices[2*i],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                    }
                }
            }

            int int_ring_count = sp->GetIntRingCount();
            for(int j = 0; j < int_ring_count; j++)
            {
                Ring * int_ring = sp->GetIntRing(j);
                double * vertices = int_ring->GetVertexData();
                int nvertex = int_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = i > 0;

                    if(i == nvertex-1)
                    {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                    }else {
                        gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                    }
                }
            }


            if(mp != nullptr)
            {
                for(int j = 0; j < mp->GetPolygonCount(); j++)
                {
                    LSMPolygon * pj = mp->GetPolygon(j);



                    if(pj != nullptr)
                    {
                        Ring * ring = pj->GetExtRing();
                        if(ring != nullptr)
                        {
                            if(ring->GetVertexCount() == 3)
                            {
                                double * data = ring->GetVertexData();
                                gl_list->AddPolygon_Triangle(index,data[0],data[1],data[2],data[3],data[4],data[5]);
                            }
                        }
                    }
                }

                mp->Destroy();
                delete mp;

            }


        }else if(type == LISEM_SHAPE_MULTIPOINT)
        {

            LSMMultiPoint *msp = (LSMMultiPoint*)s;

            for(int j = 0; j < msp->GetPointCount(); j++)
            {
                LSMPoint *sp = msp->GetPoint(j);

                float x = sp->GetX();
                float y = sp->GetY();

                gl_list->AddPoint_Point(x,y);
                gl_list->AddVertexPoint(index,x,y);

                for(int i = 0; i < 10 ; i++)
                {
                    float thetai = float(i) * 2.0f * 3.14159f/10.0f;
                    float thetaip = float(i+1) * 2.0f * 3.14159f/10.0f;

                    float x_i = x + sin(thetai);
                    float y_i = y + cos(thetai);

                    float x_ip = x + sin(thetaip);
                    float y_ip = y + cos(thetaip);

                    bool connect = (i > 0);


                    gl_list->AddPoint_Line(index,x_i,y_i,x_ip,y_ip,x,y, connect);
                    gl_list->AddPoint_Triangle(index,x_i,y_i,x_ip,y_ip,x,y,x,y);
                }

            }

        }else if(type == LISEM_SHAPE_MULTILINE)
        {
            LSMMultiLine *msp = (LSMMultiLine*)s;

            for(int i = 0; i < msp->GetLineCount(); i++)
            {
                LSMLine *sp = msp->GetLine(i);

                int nvertex = sp->GetVertexCount();
                double * vertices = sp->GetVertexData();

                for(int i = 0; i < nvertex; i++)
                {
                    gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                }

                for(int i = 0; i < nvertex; i++)
                {
                    bool connect = i > 0;
                    gl_list->AddLine_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                }

            }

        }else if(type == LISEM_SHAPE_MULTIPOLYGON)
        {
            LSMMultiPolygon *msp = (LSMMultiPolygon*)s;

            for(int i = 0; i < msp->GetPolygonCount(); i++)
            {
                LSMPolygon *sp = msp->GetPolygon(i);

                LSMMultiPolygon * mp = sp->Triangulate();

                Ring * ext_ring = sp->GetExtRing();
                if(ext_ring != nullptr){
                    double * vertices = ext_ring->GetVertexData();
                    int nvertex = ext_ring->GetVertexCount();

                    for(int i = 0; i < nvertex; i++)
                    {
                        gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                    }

                    for(int i = 0; i < nvertex; i++)
                    {
                        bool connect = (i > 0);
                        if(i == nvertex-1)
                        {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1],connect);
                        }else {
                            gl_list->AddPolygon_Line(index,vertices[2*i],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                        }
                    }
                }

                int int_ring_count = sp->GetIntRingCount();
                for(int j = 0; j < int_ring_count; j++)
                {
                    Ring * int_ring = sp->GetIntRing(j);
                    double * vertices = int_ring->GetVertexData();
                    int nvertex = int_ring->GetVertexCount();

                    for(int i = 0; i < nvertex; i++)
                    {
                        gl_list->AddVertexPoint(index,vertices[2*i+0],vertices[2*i+1]);
                    }

                    for(int i = 0; i < nvertex; i++)
                    {
                        bool connect = (i > 0);
                        if(i == nvertex-1)
                        {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(0)],vertices[2*(0)+1], connect);
                        }else {
                            gl_list->AddPolygon_Line(index,vertices[2*i+0],vertices[2*i+1],vertices[2*(i+1)],vertices[2*(i+1)+1],connect);
                        }
                    }
                }


                if(mp != nullptr)
                {
                    for(int j = 0; j < mp->GetPolygonCount(); j++)
                    {
                        LSMPolygon * pj = mp->GetPolygon(j);



                        if(pj != nullptr)
                        {
                            Ring * ring = pj->GetExtRing();
                            if(ring != nullptr)
                            {
                                if(ring->GetVertexCount() == 3)
                                {
                                    double * data = ring->GetVertexData();
                                    gl_list->AddPolygon_Triangle(index,data[0],data[1],data[2],data[3],data[4],data[5]);
                                }
                            }
                        }
                    }

                    mp->Destroy();
                    delete mp;
                }


            }
        }
    }



    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        Feature*    AS_Assign            (Feature*);

        //AS_LSMShape * AS_GetShape(int );
        int AS_GetShapeCount();
        //void AS_AddShape(AS_LSMShape *);
        void AS_RemoveShape(int );

        LSMVector2 AS_GetCentroid();
        bool AS_Contains(LSMVector2 v);
        double AS_GetLength();
        double AS_GetArea();
        void AS_Move(LSMVector2 m);
        LSMVector2 AS_GetClosestVertex(LSMVector2 v);
        LSMVector2 AS_GetClosestEdgePoint(LSMVector2 v);
        BoundingBox AS_GetBoundingBox();

};

inline LSMVector2  Feature::AS_GetClosestVertex(LSMVector2 v)
{
    return this->GetClosestVertex(v);
}
inline LSMVector2  Feature::AS_GetClosestEdgePoint(LSMVector2 v)
{
    return this->GetClosestEdgeLocation(v);
}
inline BoundingBox  Feature::AS_GetBoundingBox()
{
    return this->GetBoundingBox();
}

inline void Feature::AS_Move(LSMVector2 m)
{
    this->Move(m);
}

inline double Feature::AS_GetLength()
{
    return this->Length();
}

inline double Feature::AS_GetArea()
{
    return this->Area();
}

inline bool Feature::AS_Contains(LSMVector2 v)
{
    return this->Contains(v,0.0);
}


inline LSMVector2 Feature::AS_GetCentroid()
{
    return this->GetCentroid();
}

/*inline void Feature::AS_AddShape(AS_LSMShape *s)
{
    this->AddShape(s->m_Shape->Copy());
}*/
inline int Feature::AS_GetShapeCount()
{
    return m_ShapeList.size();

}

inline void Feature::AS_RemoveShape(int i)
{
    if(i > -1 && i < m_ShapeList.size())
    {
        this->RemoveShape(i);

    }else {

        LISEMS_WARNING("Can remove shape from feature, index out of bounds");
    }
}

/*inline AS_LSMShape * Feature::AS_GetShape(int i)
{
    if(i > -1 && i < m_ShapeList.size())
    {
        AS_LSMShape * s = new AS_LSMShape();
        s->intern_InitializeFromShape(this->GetShape(i));
        return s;

    }else {

        LSMS_WARNING("Can not get shape from feature, index out of bounds");
        return new AS_LSMShape();
    }
}*/


//Angelscript related functionality

inline void Feature::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void Feature::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline Feature* Feature::AS_Assign(Feature* sh)
{

    this->CopyFrom(sh);

    return this;
}

inline static Feature * AS_FeatureFactory()
{
    return new Feature();
}



inline LSMShape * FromGDALShape(OGRGeometry * g)
{
    //the gdal formats we recognize for now

    //wkbPoint = 1,           /**< 0-dimensional geometric object, standard WKB */
    //wkbLineString = 2,      /**< 1-dimensional geometric object with linear
    //                         *   interpolation between Points, standard WKB */
    //wkbPolygon = 3,         /**< planar 2-dimensional geometric object defined
    //                         *   by 1 exterior boundary and 0 or more interior
    //                         *   boundaries, standard WKB */
    //wkbMultiPoint = 4,      /**< GeometryCollection of Points, standard WKB */
    //wkbMultiLineString = 5, /**< GeometryCollection of LineStrings, standard WKB */
    //wkbMultiPolygon = 6,    /**< GeometryCollection of Polygons, standard WKB */

    LSMShape * s;

    OGRwkbGeometryType type = g->getGeometryType();
    OGRwkbGeometryType isotype = g->getIsoGeometryType();
    OGRPoint ptTemp;

    if ( wkbFlatten (type) == wkbPoint )
    {
        LSMPoint * p = new LSMPoint();
        OGRPoint *poPoint = g->toPoint();
        p->SetPos(poPoint->getX(),poPoint->getY(),poPoint->getZ());

        s = p;
    }else if ( wkbFlatten (type) == wkbLineString )
    {
        LSMLine * p = new LSMLine();

        OGRLineString * ls = g->toLineString();
        int NumberOfVertices = ls->getNumPoints();
        for ( int k = 0; k < NumberOfVertices; k++ )
        {
           ls->getPoint(k,&ptTemp);
            p->AddVertex(ptTemp.getX(),ptTemp.getY());
        }
        s = p;
    }else if(wkbFlatten(type) == wkbPolygon )
    {
        LSMPolygon * p = new LSMPolygon();

        OGRPolygon *po = g->toPolygon();

        OGRLinearRing *ext_ring = po->getExteriorRing();
        if(ext_ring != NULL)
        {
            int exterior_ring_point_count = ext_ring->getNumPoints();
            int interior_ring_count = po->getNumInteriorRings();

            Ring * r = new Ring();
            for(int k = 0; k <  exterior_ring_point_count; k++)
            {
                ext_ring->getPoint(k,&ptTemp);
                r->AddVertex(ptTemp.getX(),ptTemp.getY());
            }
            p->SetExtRing(r);

        }
        int numintrings = po->getNumInteriorRings();
        for(int i =0; i < numintrings; i++)
        {
            OGRLinearRing *int_ring = po->getInteriorRing(i);
            if(int_ring != NULL)
            {
                int interior_ring_point_count = int_ring->getNumPoints();
                Ring * r = new Ring();
                for(int k = 0; k <  interior_ring_point_count; k++)
                {
                    int_ring->getPoint(k,&ptTemp);
                    r->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->AddIntRing(r);

            }
        }

        s = p;
    }else if ( wkbFlatten (type) == wkbMultiPoint )
    {
        LSMMultiPoint * p = new LSMMultiPoint();

        OGRMultiPoint *poMultipoint = g->toMultiPoint();
        int NumberOfGeometries = poMultipoint->getNumGeometries();

        for ( int j = 0; j < NumberOfGeometries; j++ )
        {
           OGRPoint *poPoint = poMultipoint ->getGeometryRef(j)->toPoint();
           if(poPoint != NULL)
           {
               LSMPoint * point = new LSMPoint();
               point->SetPos(poPoint->getX(),poPoint->getY(),poPoint->getZ());
               p->Add(point);
           }
        }

        s = p;
    }else if ( wkbFlatten (type) == wkbMultiLineString )
    {
        LSMMultiLine * p = new LSMMultiLine();
        OGRMultiLineString *mls = g->toMultiLineString();
        int NumberOfGeometries = mls->getNumGeometries();
        for ( int j = 0; j < NumberOfGeometries; j++ )
        {
            LSMLine * pl = new LSMLine();
            OGRLineString * ls = mls->getGeometryRef(j)->toLineString();
            if(ls != NULL)
            {
                int NumberOfVertices = ls->getNumPoints();
                for ( int k = 0; k < NumberOfVertices; k++ )
                {
                   ls->getPoint(k,&ptTemp);
                    pl->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->Add(pl);
            }
        }
        s = p;

    }else if(wkbFlatten(type) == wkbMultiPolygon )
    {
        LSMMultiPolygon * mp = new LSMMultiPolygon();

        OGRMultiPolygon *mpo = g->toMultiPolygon();
        int po_count = mpo->getNumGeometries();

        for(int l = 0; l < po_count;l++)
        {
            LSMPolygon * p = new LSMPolygon();

            OGRPolygon *po = mpo->getGeometryRef(l)->toPolygon();

            OGRLinearRing *ext_ring = po->getExteriorRing();
            if(ext_ring != NULL)
            {
                int exterior_ring_point_count = ext_ring->getNumPoints();

                Ring * r = new Ring();
                for(int k = 0; k <  exterior_ring_point_count; k++)
                {
                    ext_ring->getPoint(k,&ptTemp);
                    r->AddVertex(ptTemp.getX(),ptTemp.getY());
                }
                p->SetExtRing(r);

            }
            int numintrings = po->getNumInteriorRings();
            for(int i =0; i < numintrings; i++)
            {
                OGRLinearRing *int_ring = po->getInteriorRing(i);
                if(int_ring != NULL)
                {
                    int interior_ring_point_count = int_ring->getNumPoints();
                    Ring * r = new Ring();
                    for(int k = 0; k <  interior_ring_point_count; k++)
                    {
                        int_ring->getPoint(k,&ptTemp);
                        r->AddVertex(ptTemp.getX(),ptTemp.getY());
                    }
                    p->AddIntRing(r);
                }
            }
            mp->Add(p);
        }
        s = mp;
    }

    return s;
}

inline OGRGeometry * ToGDALShape(LSMShape *s)
{

    //#define LISEM_SHAPE_UNKNOWN 0
    //#define LISEM_SHAPE_POINT 1
    //#define LISEM_SHAPE_LINE 2
    //#define LISEM_SHAPE_POLYGON 3
    //#define LISEM_SHAPE_MULTIPOINT 4
    //#define LISEM_SHAPE_MULTILINE 5
    //#define LISEM_SHAPE_MULTIPOLYGON 6

    int type = s->GetType();
    if(type == LISEM_SHAPE_UNKNOWN)
    {

    }else if(type == LISEM_SHAPE_POINT)
    {
        LSMPoint * sp = (LSMPoint*) s;

        OGRPoint *pt = new OGRPoint();

        pt->setX( sp->GetX());
        pt->setY( sp->GetY() );
        return pt;

    }else if(type == LISEM_SHAPE_LINE)
    {
        LSMLine * sp = (LSMLine*) s;

        OGRLineString *pt = new OGRLineString();

        int nvertex = sp->GetVertexCount();
        double * vertices = sp->GetVertexData();

        for(int i = 0; i < nvertex; i++)
        {
            pt->addPoint(vertices[2*i+0],vertices[2*i+1]);
        }
        return pt;

    }else if(type == LISEM_SHAPE_POLYGON)
    {
        LSMPolygon * sp = (LSMPolygon*) s;
        OGRPolygon *pt = new OGRPolygon();

        OGRLinearRing * ogr_ext_ring = new OGRLinearRing();
        OGRLinearRing * ogr_ext_ring2 = new OGRLinearRing();
        Ring * ext_ring = sp->GetExtRing();
        if(ext_ring != nullptr){
            double * vertices = ext_ring->GetVertexData();
            int nvertex = ext_ring->GetVertexCount();

            for(int i = 0; i < nvertex; i++)
            {
                ogr_ext_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
            ogr_ext_ring->closeRings();
        }
        if(!ogr_ext_ring->IsValid())
        {
            std::cout <<"invalid geometry writing "<< std::endl;
            ogr_ext_ring2 = (OGRLinearRing *)ogr_ext_ring->MakeValid();
            if(ogr_ext_ring2 != nullptr)
            {
                delete ogr_ext_ring;
                ogr_ext_ring = ogr_ext_ring2;
            }else {
                std::cout << "validated geom nul " <<std::endl;
            }
        }
        pt->addRing(ogr_ext_ring);

        int int_ring_count = sp->GetIntRingCount();
        for(int j = 0; j < int_ring_count; j++)
        {
            OGRLinearRing * ogr_int_ring = new OGRLinearRing();
            OGRLinearRing * ogr_int_ring2 = new OGRLinearRing();

            Ring * int_ring = sp->GetIntRing(j);
            double * vertices = int_ring->GetVertexData();
            int nvertex = int_ring->GetVertexCount();

            for(int i = 0; i < nvertex; i++)
            {
                ogr_int_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
            ogr_int_ring->closeRings();
            if(!ogr_int_ring->IsValid())
            {
                std::cout <<"invalid geometry writing "<< std::endl;
                ogr_int_ring2 = (OGRLinearRing *)ogr_int_ring->MakeValid();
                if(ogr_int_ring2 != nullptr)
                {
                    delete ogr_ext_ring;
                    ogr_int_ring = ogr_int_ring2;
                }else {
                    std::cout << "validated geom nul " <<std::endl;
                }
            }
            pt->addRing(ogr_int_ring);
        }

        return pt;

    }else if(type == LISEM_SHAPE_MULTIPOINT)
    {

        OGRMultiPoint *mp = new OGRMultiPoint();
        LSMMultiPoint *msp = (LSMMultiPoint*)s;

        for(int i = 0; i < msp->GetPointCount(); i++)
        {
            LSMPoint *sp = msp->GetPoint(i);
            OGRPoint *pt = new OGRPoint();

            pt->setX( sp->GetX());
            pt->setY( sp->GetY());
            mp->addGeometry(pt);
        }
        return mp;

    }else if(type == LISEM_SHAPE_MULTILINE)
    {

        OGRMultiLineString *mp = new OGRMultiLineString();
        LSMMultiLine *msp = (LSMMultiLine*)s;

        for(int i = 0; i < msp->GetLineCount(); i++)
        {
            LSMLine *sp = msp->GetLine(i);
            OGRLineString *pt = new OGRLineString();

            int nvertex = sp->GetVertexCount();
            double * vertices = sp->GetVertexData();

            for(int i = 0; i < nvertex; i++)
            {
                pt->addPoint(vertices[2*i+0],vertices[2*i+1]);
            }
            mp->addGeometry(pt);

        }
        return mp;

    }else if(type == LISEM_SHAPE_MULTIPOLYGON)
    {
        OGRMultiPolygon *mp = new OGRMultiPolygon();
        LSMMultiPolygon *msp = (LSMMultiPolygon*)s;

        for(int i = 0; i < msp->GetPolygonCount(); i++)
        {
            LSMPolygon *sp = msp->GetPolygon(i);
            OGRPolygon *pt = new OGRPolygon();

            OGRLinearRing * ogr_ext_ring = new OGRLinearRing();
            OGRLinearRing * ogr_ext_ring2 = new OGRLinearRing();
            Ring * ext_ring = sp->GetExtRing();
            if(ext_ring != nullptr){
                double * vertices = ext_ring->GetVertexData();
                int nvertex = ext_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    ogr_ext_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
                }
            }
            if(!ogr_ext_ring->IsValid())
            {
                std::cout <<"invalid geometry writing "<< std::endl;
                ogr_ext_ring2 = (OGRLinearRing *)ogr_ext_ring->MakeValid();
                if(ogr_ext_ring2 != nullptr)
                {
                    delete ogr_ext_ring;
                    ogr_ext_ring = ogr_ext_ring2;
                }else {
                    std::cout << "validated geom nul " <<std::endl;
                }
            }
            ogr_ext_ring->closeRings();
            pt->addRing(ogr_ext_ring);

            int int_ring_count = sp->GetIntRingCount();
            for(int j = 0; j < int_ring_count; j++)
            {
                OGRLinearRing * ogr_int_ring = new OGRLinearRing();
                OGRLinearRing * ogr_int_ring2 = new OGRLinearRing();
                Ring * int_ring = sp->GetIntRing(j);
                double * vertices = int_ring->GetVertexData();
                int nvertex = int_ring->GetVertexCount();

                for(int i = 0; i < nvertex; i++)
                {
                    ogr_int_ring->addPoint(vertices[2*i+0],vertices[2*i+1]);
                }
                ogr_int_ring->closeRings();
                if(!ogr_int_ring->IsValid())
                {
                    std::cout <<"invalid geometry writing "<< std::endl;
                    ogr_int_ring2 = (OGRLinearRing *)ogr_int_ring->MakeValid();
                    if(ogr_int_ring2 != nullptr)
                    {
                        delete ogr_ext_ring;
                        ogr_int_ring = ogr_int_ring2;
                    }else {
                        std::cout << "validated geom nul " <<std::endl;
                    }
                }
                pt->addRing(ogr_int_ring);
            }

            mp->addGeometry(pt);

        }
        return mp;
    }

    return nullptr;

}


inline Feature * FromGDALFeature(OGRFeature *d)
{
    Feature * f = new Feature();

    int gcount = d->GetGeomFieldCount();
    for(int k = 0; k < gcount; k++)
    {

        OGRGeometry *poGeometry = d->GetGeomFieldRef(k);
        if( poGeometry != NULL )
        {
            if(poGeometry->IsValid())
            {
                LSMShape* s = FromGDALShape(poGeometry);

                f->AddShape(s);
            }
        }
    }
    return f;
}

inline OGRFeature * ToGDALFeature(OGRFeature * to,Feature * f)
{
    for(int i = 0; i < f->GetShapeCount();i++)
    {

        OGRGeometry * g = ToGDALShape(f->GetShape(i));
        if(g == nullptr)
        {
            LISEM_ERROR("Could not create gdal geometry from shape");
            continue;
        }
        if(!g->IsValid())
        {
            g = g->MakeValid();
        }

        to->SetGeometryDirectly(g);

        //so far we only do one shape per feature
        //this is sufficient for most types of shapefiles
        break;
    }
    return to;
}



