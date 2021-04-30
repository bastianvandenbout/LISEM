#pragma once

#include "defines.h"
#include "shape.h"
#include "iostream"
#include "geometry/geometrybase.h"
#include "geometry/shapealgebra.h"

class LISEM_API LSMLine: public LSMShape
{

    std::vector<double> m_vertices;

    BoundingBox m_BoundingBox;

public:
    inline LSMLine() : LSMShape(LISEM_SHAPE_LINE)
    {
    }

    inline void AddVertex(double x, double y)
    {
        m_vertices.push_back(x);
        m_vertices.push_back(y);

        if(m_vertices.size() == 1)
        {
            m_BoundingBox.SetAs(LSMVector2(x,y));
        }else {
            m_BoundingBox.MergeWith(LSMVector2(x,y));
        }
    }

    inline void SetAsRegularNGon(LSMVector2 center, double radius, int n_points = 25)
    {

        m_vertices.clear();
        for(int i = 0; i < n_points; i++)
        {
            double thetai = float(i) * 2.0 * 3.14159/((float)(n_points));
            double thetaip = float(i+1) * 2.0 * 3.14159/((float)(n_points));

            double x_i = center.x + radius*sin(thetai);
            double y_i = center.y + radius*cos(thetai);

            double x_ip = center.x + radius*sin(thetaip);
            double y_ip = center.y + radius*cos(thetaip);

            AddVertex(x_i,y_i);
        }

    }

    inline void SetAsSquare(LSMVector2 tl, LSMVector2 br)
    {
        m_vertices.clear();
        AddVertex(tl.x,tl.Y());
        AddVertex(tl.x,br.Y());
        AddVertex(br.x,br.Y());
        AddVertex(br.x,tl.Y());
        AddVertex(tl.x,tl.Y());
    }





    inline double* GetVertexData()
    {
        return m_vertices.data();
    }
    inline int GetVertexCount() override
    {
        return m_vertices.size()/2;
    }
    inline LSMVector2 GetVertex(int i)
    {
        return LSMVector2(m_vertices.at(i*2),m_vertices.at(i*2 +1));
    }

    virtual void GetVertexP(int i, double ** x , double **y)
    {
        if(i < m_vertices.size()/2)
        {
            *x = &m_vertices.at(i*2);
            *y= &m_vertices.at(i*2 +1);
        }else {
            *x = nullptr;
            *y= nullptr;
        }


    }


    inline void RemoveVertex(int i)
    {
        m_vertices.erase(m_vertices.begin() + i);
        m_vertices.erase(m_vertices.begin() + i);
    }

    //input is polygon (provided in non-closed form)
    virtual bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {
        int vcount = GetVertexCount();

        bool intersect = false;

        for(int i = 0; i < vcount; i++)
        {
            if(i != vcount - 1)
            {
                intersect = intersect || Geometry_PolygonIntersectsLine(x,y,GetVertex(i).x,GetVertex(i).y,GetVertex(i+1).x,GetVertex(i+1).y);
                intersect = intersect || Geometry_PolygonContaintsPoint(x,y,GetVertex(i).x,GetVertex(i).y);
            }else {
                intersect = intersect || Geometry_PolygonIntersectsLine(x,y,GetVertex(i).x,GetVertex(i).y,GetVertex(0).x,GetVertex(0).y);
                intersect = intersect || Geometry_PolygonContaintsPoint(x,y,GetVertex(i).x,GetVertex(i).y);
            }

            if(intersect)
            {
                return true;
            }
        }


        return false;
    }
    virtual QList<LSMShape *> Split(std::vector<double> & x,std::vector<double> & y)
    {
        QList<LSMShape *> list;

        //start with a single line segment
        //we keep adding points
        //when we meet an intersection point, we push the line so far to the list
        //and start a new one

        int vcount = GetVertexCount();

        bool intersect = false;

        LSMLine * l = new LSMLine();

        std::vector<std::pair<LSMVector2,double>> collides;
        for(int i = 0; i < vcount-1; i++)
        {
            collides.clear();
            LSMVector2 v = GetVertex(i);
            l->AddVertex(v.x,v.y);

            double x1,x2,y1,y2;
            double x3,x4,y3,y4;

            if(i != vcount - 1)
            {
                x1 = GetVertex(i).x;
                y1 = GetVertex(i).y;
                x2 = GetVertex(i+1).x;
                y2 = GetVertex(i+1).y;

                //not used here, no polygon
            }else {
                x1 = GetVertex(i).x;
                y1 = GetVertex(i).y;
                x2 = GetVertex(0).x;
                y2 = GetVertex(0).y;
            }

            for(int j =0; j < x.size(); j++)
            {
                if(j != x.size() - 1)
                {
                    x3 = x[j];
                    y3 = y[j];
                    x4 = x[j+1];
                    y4 = y[j+1];
                }else {
                    x3 = x[j];
                    y3 = y[j];
                    x4 = x[0];
                    y4 = y[0];
                }
                LSMVector2 collide;
                bool doescollide = Geometry_lineLine(x1,y1,x2,y2,x3,y3,x4,y4,collide);
                if(doescollide)
                {
                    collides.push_back(std::make_pair<LSMVector2,double>(LSMVector2(collide.x,collide.y),(double)(collide - v).length()));
                }
            }

            //sort collisions
            std::sort(collides.begin(), collides.end(),[](const std::pair<LSMVector2,double> &a,const std::pair<LSMVector2,double> &b)
            {
                return a.second < b.second;
            });

            //now we do our things
            for(int j = 0; j < collides.size(); j++)
            {
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
                list.append(l);
                l = new LSMLine();
                l->AddVertex(collides.at(j).first.x,collides.at(j).first.y);
            }
        }

        list.append(l);

        return list;
    }



    /*inline void getLine(double x1, double y1, double x2, double y2, double &a, double &b, double &c)
    {
        // (x- p1X) / (p2X - p1X) = (y - p1Y) / (p2Y - p1Y)
        a = y1 - y2;
        b = x2 - x1;
        c = x1 * y2 - x2 * y1;
    }

    inline double LineDist(double pct1X, double pct1Y, double pct2X, double pct2Y, double pct3X, double pct3Y)
    {
        double a, b, c;
        getLine(pct2X, pct2Y, pct3X, pct3Y, a, b, c);
        return abs(a * pct1X + b * pct1Y + c) / sqrt(a * a + b * b);
    }*/

    inline double LineDist(double pct1X, double pct1Y, double pct2X, double pct2Y, double pct3X, double pct3Y)
    {
        double retx =1e31;
        double rety =1e31;

        GetClosestPointOnLine(pct2X,pct2Y,pct3X,pct3Y,pct1X,pct1Y, &retx, &rety);

        if(std::isnan(retx) || std::isnan(rety))
        {
            retx =1e30;
            rety =1e30;
        }

        return (LSMVector2(retx,rety) - LSMVector2(pct1X,pct1Y)).length();


    }

    inline double GetDistanceToPoint(LSMVector2 pos)
    {
        double mindist = 1e31;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double x1 = m_vertices[i +0];
            double x2 = m_vertices[i +2];
            double y1 = m_vertices[i +1];
            double y2 = m_vertices[i +3];

            mindist = std::min((double)(LineDist(pos.x,pos.y,x1,y1,x2,y2)),mindist);
        }
        return mindist;
    }

    inline double Length() override
    {
        double length = 0.0;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double lx2 = m_vertices[i +2];
            double ly1 = m_vertices[i +1];
            double ly2 = m_vertices[i +3];

            length += std::sqrt((lx2-lx1) *(lx2-lx1) + (ly2-ly1) *(ly2-ly1));
        }
        return length;
    }


    inline LSMVector2 GetCentroid() override
    {
        double length = 0.0;
        LSMVector2 centroid;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double lx2 = m_vertices[i +2];
            double ly1 = m_vertices[i +1];
            double ly2 = m_vertices[i +3];

            LSMVector2 centroid_this = LSMVector2(lx1,ly1)*0.5 + LSMVector2(lx2,ly2)*0.5;

            length += std::sqrt((lx2-lx1) *(lx2-lx1) + (ly2-ly1) *(ly2-ly1));

            centroid = centroid + centroid_this * length;
        }
        if(length > 0)
        {
            centroid = centroid/length;
        }
        return centroid;
    }


    LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr) override
    {
        LSMVector2 closest = LSMVector2(1e30,1e30);
        double mindist = 1e30;

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

        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double lx2 = m_vertices[i +2];
            double ly1 = m_vertices[i +1];
            double ly2 = m_vertices[i +3];

            double retx =lx1;
            double rety =ly1;

            GetClosestPointOnLine(lx1,ly1,lx2,ly2,pos.x,pos.y, &retx, &rety);

            float distance = LSMVector2(pos - LSMVector2(retx,rety)).length();

                if(distance < mindist)
                {
                    if(x1 != nullptr)
                    {
                        *x1 = &m_vertices[i +0];
                    }
                    if(x2 != nullptr)
                    {
                        *x2 = &m_vertices[i +2];
                    }
                    if(y1 != nullptr)
                    {
                        *y1 = &m_vertices[i +1];
                    }
                    if(y2 != nullptr)
                    {
                        *y2 = &m_vertices[i +3];
                    }
                    closest =LSMVector2(retx,rety);
                    mindist = distance;
                }

        }

        return closest;
    }

    LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr) override
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

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            double lx1 = m_vertices[i +0];
            double ly1 = m_vertices[i +1];
            float distance = LSMVector2(pos - LSMVector2(lx1,ly1)).length();
            if(i == 0 || distance < mindist)
            {
                mindist = distance;
                closest = LSMVector2(lx1,ly1);
                if(x1 != nullptr)
                {
                    *x1 = &m_vertices[i +0];
                }
                if(y1 != nullptr)
                {
                    *y1 = &m_vertices[i +1];
                }

            }
        }

        return closest;
    }


    virtual void MoveVertex(double *x, double *y, LSMVector2 d) override
    {
        m_BoundingBox.Set(0,0,0,0);
        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {

            if(&m_vertices[i +0] == x)
            {
                m_vertices[i +0] += d.x;
            }
            if(&m_vertices[i +1] == y)
            {
                m_vertices[i +1] += d.y;
            }
            if(i == 0)
            {
                m_BoundingBox.SetAs(LSMVector2(m_vertices[i +0],m_vertices[i +1]));
            }else {
                m_BoundingBox.MergeWith(LSMVector2(m_vertices[i +0],m_vertices[i +1]));
            }
        }
    }

    inline void Move(LSMVector2 d) override
    {
        m_BoundingBox.Move(d);

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            m_vertices[i +0] += d.x;
            m_vertices[i +1] += d.y;
        }
    }

    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e30,1e30);
        double mindist = 1e30;
        for(int i = 0; i < m_vertices.size() - 2; i += 2)
        {
            double x1 = m_vertices[i +0];
            double x2 = m_vertices[i +2];
            double y1 = m_vertices[i +1];
            double y2 = m_vertices[i +3];

            double retx =x1;
            double rety =y1;

            GetClosestPointOnLine(x1,y1,x2,y2,pos.x,pos.y, &retx, &rety);

            float distance = LSMVector2(pos - LSMVector2(retx,rety)).length();

                if(distance < mindist)
                {
                    closest =LSMVector2(retx,rety);
                    mindist = distance;
                }

        }

        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos) override
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        for(int i = 0; i < ((int)m_vertices.size())-1; i += 2)
        {
            double x1 = m_vertices[i +0];
            double y1 = m_vertices[i +1];
            float distance = LSMVector2(pos - LSMVector2(x1,y1)).length();
            if(i == 0)
            {
                mindist = distance;
                closest = LSMVector2(x1,y1);
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = LSMVector2(x1,y1);
            }
        }

        return closest;
    }


    bool Contains(LSMVector2 pos, double tolerence) override
    {
        LSMVector2 hit = GetClosestEdgeLocation(pos);
        float distance = (hit - pos).length();


        if(distance <= tolerence)
        {
            return true;
        }else {
            return false;
        }
    }


    inline LSMShape * Copy() override
    {
        LSMLine * l = new LSMLine();
        l->m_vertices = m_vertices;
        l->m_BoundingBox = m_BoundingBox;
        return l;

    }

    inline BoundingBox GetBoundingBox() override
    {
        return m_BoundingBox;
    }


    //Angelscript related functions
    public:

        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMLine*    AS_Assign            (LSMLine*);


};


//Angelscript related functionality

inline void LSMLine::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMLine::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        Destroy();
        delete this;
    }

}

inline LSMLine* LSMLine::AS_Assign(LSMLine* sh)
{

    return this;
}

inline static LSMLine * AS_LineFactory()
{
    return new LSMLine();
}
