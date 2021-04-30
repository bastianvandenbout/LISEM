#ifndef GLGEOMETRYLIST_H
#define GLGEOMETRYLIST_H

#include <vector>
#include "geo/geocoordtransform.h"

class GLGeometryList
{
public:



    std::vector<float> VertexPoint_x;
    std::vector<float> VertexPoint_y;

    std::vector<float> PointPoint_x;
    std::vector<float> PointPoint_y;

    std::vector<float> Point_Triangles_x;
    std::vector<float> Point_Lines_x;
    std::vector<float> Point_Triangles_rx;
    std::vector<float> Point_Lines_rx;
    std::vector<float> Line_Lines_x;
    std::vector<float> Polygon_Triangles_x;
    std::vector<float> Polygon_Lines_x;
    std::vector<float> Point_Triangles_y;
    std::vector<float> Point_Lines_y;
    std::vector<float> Point_Triangles_ry;
    std::vector<float> Point_Lines_ry;
    std::vector<float> Line_Lines_y;
    std::vector<float> Polygon_Triangles_y;
    std::vector<float> Polygon_Lines_y;

    std::vector<int> Point_Lines_connect;
    std::vector<int> Line_Lines_connect;
    std::vector<int> Polygon_Lines_connect;

    std::vector<int> Point_Triangles_index;
    std::vector<int> Point_Lines_index;
    std::vector<int> Line_Lines_index;
    std::vector<int> Polygon_Triangles_index;
    std::vector<int> Polygon_Lines_index;
    std::vector<int> Vertex_Point_index;

    std::vector<float> Point_Triangles_attr;
    std::vector<float> Point_Lines_attr;
    std::vector<float> Line_Lines_attr;
    std::vector<float> Polygon_Triangles_attr;
    std::vector<float> Polygon_Lines_attr;
    std::vector<float> Vertex_Point_attr;

    inline void AddVertexPoint(int index, float x, float y)
    {
        Vertex_Point_index.push_back(index);
        Vertex_Point_attr.push_back(index);
        VertexPoint_x.push_back(x);
        VertexPoint_y.push_back(y);
    }

    inline void AddPoint_Point(float x1, float y1)
    {
        PointPoint_x.push_back(x1);
        PointPoint_y.push_back(y1);
    }

    inline void AddPoint_Line(int index, float x1, float y1, float x2, float y2, float xr, float yr, bool connect_to_previous = false)
    {
        for(int i = 0; i < 2; i++)
        {
            Point_Lines_index.push_back(index);
            Point_Lines_attr.push_back(index);
        }

        Point_Lines_x.push_back(x1);
        Point_Lines_y.push_back(y1);
        Point_Lines_x.push_back(x2);
        Point_Lines_y.push_back(y2);
        Point_Lines_rx.push_back(xr);
        Point_Lines_ry.push_back(yr);
        Point_Lines_rx.push_back(xr);
        Point_Lines_ry.push_back(yr);

        Point_Lines_connect.push_back(connect_to_previous?1:0);

    }
    inline void AddPoint_Triangle(int index, float x1, float y1, float x2, float y2, float x3, float y3,float xr, float yr)
    {
        for(int i = 0; i < 3; i++)
        {
            Point_Triangles_index.push_back(index);
            Point_Triangles_attr.push_back(index);
        }

        Point_Triangles_x.push_back(x1);
        Point_Triangles_y.push_back(y1);
        Point_Triangles_x.push_back(x2);
        Point_Triangles_y.push_back(y2);
        Point_Triangles_x.push_back(x3);
        Point_Triangles_y.push_back(y3);
        Point_Triangles_rx.push_back(xr);
        Point_Triangles_ry.push_back(yr);
        Point_Triangles_rx.push_back(xr);
        Point_Triangles_ry.push_back(yr);
        Point_Triangles_rx.push_back(xr);
        Point_Triangles_ry.push_back(yr);
    }


    inline void AddLine_Line(int index, float x1, float y1, float x2, float y2, bool connect_to_previous = false)
    {

        for(int i = 0; i < 2; i++)
        {
            Line_Lines_index.push_back(index);
            Line_Lines_attr.push_back(index);
        }

        Line_Lines_x.push_back(x1);
        Line_Lines_y.push_back(y1);
        Line_Lines_x.push_back(x2);
        Line_Lines_y.push_back(y2);

        Line_Lines_connect.push_back(connect_to_previous?1:0);

    }
    inline void AddPolygon_Line(int index, float x1, float y1, float x2, float y2, bool connect_to_previous = false)
    {
        for(int i = 0; i < 2; i++)
        {
            Polygon_Lines_index.push_back(index);
            Polygon_Lines_attr.push_back(index);
        }

        Polygon_Lines_x.push_back(x1);
        Polygon_Lines_y.push_back(y1);
        Polygon_Lines_x.push_back(x2);
        Polygon_Lines_y.push_back(y2);

        Polygon_Lines_connect.push_back(connect_to_previous?1:0);
    }
    inline void AddPolygon_Triangle(int index, float x1, float y1, float x2, float y2, float x3, float y3)
    {
        for(int i = 0; i < 3; i++)
        {
            Polygon_Triangles_index.push_back(index);
            Polygon_Triangles_attr.push_back(index);
        }

        Polygon_Triangles_x.push_back(x1);
        Polygon_Triangles_y.push_back(y1);
        Polygon_Triangles_x.push_back(x2);
        Polygon_Triangles_y.push_back(y2);
        Polygon_Triangles_x.push_back(x3);
        Polygon_Triangles_y.push_back(y3);



    }

    inline GLGeometryList * CopyGeomOnly()
    {
        GLGeometryList * ret = new GLGeometryList();


        ret->VertexPoint_x = VertexPoint_x;
        ret->VertexPoint_y = VertexPoint_y;

        ret->PointPoint_x = PointPoint_x;
        ret->PointPoint_y = PointPoint_y;

        ret->Point_Triangles_x = Point_Triangles_x;
        ret->Point_Lines_x = Point_Lines_x;
        ret->Point_Triangles_rx = Point_Triangles_rx;
        ret->Point_Lines_rx = Point_Lines_rx;
        ret->Line_Lines_x = Line_Lines_x;
        ret->Polygon_Triangles_x = Polygon_Triangles_x;
        ret->Polygon_Lines_x = Polygon_Lines_x;
        ret->Point_Triangles_y = Point_Triangles_y;
        ret->Point_Lines_y = Point_Lines_y;
        ret->Point_Triangles_ry = Point_Triangles_ry;
        ret->Point_Lines_ry = Point_Lines_ry;
        ret->Line_Lines_y = Line_Lines_y;
        ret->Polygon_Triangles_y = Polygon_Triangles_y;
        ret->Polygon_Lines_y = Polygon_Lines_y;
        ret->Point_Lines_connect = Point_Lines_connect;
        ret->Line_Lines_connect = Line_Lines_connect;
        ret->Polygon_Lines_connect = Polygon_Lines_connect;

        ret->Point_Triangles_index =Point_Triangles_index;
        ret->Point_Lines_index = Point_Lines_index;
        ret->Line_Lines_index = Line_Lines_index;
        ret->Polygon_Triangles_index = Polygon_Triangles_index;
        ret->Polygon_Lines_index = Polygon_Lines_index;
        ret->Vertex_Point_index = Vertex_Point_index;

        ret->Point_Triangles_attr = Point_Triangles_attr;
        ret->Point_Lines_attr = Point_Lines_attr;
        ret->Line_Lines_attr = Line_Lines_attr;
        ret->Polygon_Triangles_attr = Polygon_Triangles_attr;
        ret->Polygon_Lines_attr = Polygon_Lines_attr;
        ret->Vertex_Point_attr = Vertex_Point_attr;

        return ret;
    }

    inline void CopyGeomOnlyFrom(GLGeometryList * other)
    {

        VertexPoint_x = other->VertexPoint_x;
        VertexPoint_y = other->VertexPoint_y;

        PointPoint_x = other->PointPoint_x;
        PointPoint_y = other->PointPoint_y;

        Point_Triangles_x = other->Point_Triangles_x;
        Point_Lines_x = other->Point_Lines_x;
        Point_Triangles_rx = other->Point_Triangles_rx;
        Point_Lines_rx = other->Point_Lines_rx;
        Line_Lines_x = other->Line_Lines_x;
        Polygon_Triangles_x = other->Polygon_Triangles_x;
        Polygon_Lines_x = other->Polygon_Lines_x;
        Point_Triangles_y = other->Point_Triangles_y;
        Point_Lines_y = other->Point_Lines_y;
        Point_Triangles_ry = other->Point_Triangles_ry;
        Point_Lines_ry = other->Point_Lines_ry;
        Line_Lines_y = other->Line_Lines_y;
        Polygon_Triangles_y = other->Polygon_Triangles_y;
        Polygon_Lines_y = other->Polygon_Lines_y;
        Point_Lines_connect = other->Point_Lines_connect;
        Line_Lines_connect = other->Line_Lines_connect;
        Polygon_Lines_connect = other->Polygon_Lines_connect;

        Point_Triangles_index =other->Point_Triangles_index;
        Point_Lines_index = other->Point_Lines_index;
        Line_Lines_index = other->Line_Lines_index;
        Polygon_Triangles_index = other->Polygon_Triangles_index;
        Polygon_Lines_index = other->Polygon_Lines_index;
        Vertex_Point_index = other->Vertex_Point_index;

        Point_Triangles_attr = other->Point_Triangles_attr;
        Point_Lines_attr = other->Point_Lines_attr;
        Line_Lines_attr = other->Line_Lines_attr;
        Polygon_Triangles_attr = other->Polygon_Triangles_attr;
        Polygon_Lines_attr = other->Polygon_Lines_attr;
        Vertex_Point_attr = other->Vertex_Point_attr;
    }

    inline void ApplyOffset( LSMVector2 offset)
    {
        GeoCoordTransformer *t = GeoCoordTransformer::GetCoordTransformer(offset);

        if(!t->IsGeneric())
        {
            t->Transform(PointPoint_x.size(),PointPoint_x.data(),PointPoint_y.data());
            t->Transform(VertexPoint_x.size(),VertexPoint_x.data(),VertexPoint_y.data());
            t->Transform(Point_Triangles_x.size(),Point_Triangles_x.data(),Point_Triangles_y.data());
            t->Transform(Point_Lines_x.size(),Point_Lines_x.data(),Point_Lines_y.data());
            t->Transform(Point_Triangles_rx.size(),Point_Triangles_rx.data(),Point_Triangles_ry.data());
            t->Transform(Point_Lines_rx.size(),Point_Lines_rx.data(),Point_Lines_ry.data());
            t->Transform(Line_Lines_x.size(),Line_Lines_x.data(),Line_Lines_y.data());
            t->Transform(Polygon_Lines_x.size(),Polygon_Lines_x.data(),Polygon_Lines_y.data());
            t->Transform(Polygon_Triangles_x.size(),Polygon_Triangles_x.data(),Polygon_Triangles_y.data());
        }
        delete t;
    }

    inline void Transform( GeoCoordTransformer * t)
    {
        if(!t->IsGeneric())
        {
            t->Transform(PointPoint_x.size(),PointPoint_x.data(),PointPoint_y.data());
            t->Transform(VertexPoint_x.size(),VertexPoint_x.data(),VertexPoint_y.data());
            t->Transform(Point_Triangles_x.size(),Point_Triangles_x.data(),Point_Triangles_y.data());
            t->Transform(Point_Lines_x.size(),Point_Lines_x.data(),Point_Lines_y.data());
            t->Transform(Point_Triangles_rx.size(),Point_Triangles_rx.data(),Point_Triangles_ry.data());
            t->Transform(Point_Lines_rx.size(),Point_Lines_rx.data(),Point_Lines_ry.data());
            t->Transform(Line_Lines_x.size(),Line_Lines_x.data(),Line_Lines_y.data());
            t->Transform(Polygon_Lines_x.size(),Polygon_Lines_x.data(),Polygon_Lines_y.data());
            t->Transform(Polygon_Triangles_x.size(),Polygon_Triangles_x.data(),Polygon_Triangles_y.data());
        }
    }

    inline void TransformToScreen(BoundingBox screen_bb, float scr_width, float scr_height)
    {

        //float tlX = state.scr_width * (m_PolygonFillX.at(i) - state.tlx)/state.width;
        //float tlY = (state.scr_height * (m_PolygonFillY.at(i)- state.tly)/state.height) ;

        for(int i = 0; i < VertexPoint_x.size(); i++)
        {
            VertexPoint_x.at(i) = scr_width * (VertexPoint_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < VertexPoint_y.size(); i++)
        {
            VertexPoint_y.at(i) = scr_height * (VertexPoint_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }

        for(int i = 0; i < PointPoint_x.size(); i++)
        {
            PointPoint_x.at(i) = scr_width * (PointPoint_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < PointPoint_y.size(); i++)
        {
            PointPoint_y.at(i) = scr_height * (PointPoint_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }

        for(int i = 0; i < Point_Triangles_x.size(); i++)
        {
            Point_Triangles_x.at(i) = scr_width * (Point_Triangles_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Point_Triangles_y.size(); i++)
        {
            Point_Triangles_y.at(i) = scr_height * (Point_Triangles_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }


        for(int i = 0; i < Point_Lines_x.size(); i++)
        {
            Point_Lines_x.at(i) = scr_width * (Point_Lines_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Point_Lines_y.size(); i++)
        {
            Point_Lines_y.at(i) = scr_height * (Point_Lines_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }


        for(int i = 0; i < Point_Triangles_rx.size(); i++)
        {
            Point_Triangles_rx.at(i) = scr_width * (Point_Triangles_rx.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Point_Triangles_ry.size(); i++)
        {
            Point_Triangles_ry.at(i) = scr_height * (Point_Triangles_ry.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }

        for(int i = 0; i < Point_Lines_rx.size(); i++)
        {
            Point_Lines_rx.at(i) = scr_width * (Point_Lines_rx.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Point_Lines_ry.size(); i++)
        {
            Point_Lines_ry.at(i) = scr_height * (Point_Lines_ry.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }



        for(int i = 0; i < Line_Lines_x.size(); i++)
        {
            Line_Lines_x.at(i) = scr_width * (Line_Lines_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Line_Lines_y.size(); i++)
        {
            Line_Lines_y.at(i) = scr_height * (Line_Lines_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }


        for(int i = 0; i < Polygon_Lines_x.size(); i++)
        {
            Polygon_Lines_x.at(i) = scr_width * (Polygon_Lines_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Polygon_Lines_y.size(); i++)
        {
            Polygon_Lines_y.at(i) = scr_height * (Polygon_Lines_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }


        for(int i = 0; i < Polygon_Triangles_x.size(); i++)
        {
            Polygon_Triangles_x.at(i) = scr_width * (Polygon_Triangles_x.at(i) - screen_bb.GetMinX())/screen_bb.GetSizeX();
        }
        for(int i = 0; i < Polygon_Triangles_y.size(); i++)
        {
            Polygon_Triangles_y.at(i) = scr_height * (Polygon_Triangles_y.at(i) - screen_bb.GetMinY())/screen_bb.GetSizeY();
        }

    }
};


#endif // GLGEOMETRYLIST_H
