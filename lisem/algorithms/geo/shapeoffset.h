#ifndef SHAPEOFFSET_H
#define SHAPEOFFSET_H

#include "QList"
#include "geometry/shape.h"
#include "geo/shapes/shapefile.h"
#include "geometry/clipper.hpp"
#include "shapealgorithms.h"


#define LISEM_OFFSET_ROUND 0
#define LISEM_OFFSET_SQUARE 1
#define LISEM_OFFSET_MITER 2



static QList<LSMShape * >ShapeOffset(QList<LSMShape *> shapes,float offset, int offset_type);
static ShapeFile * ShapeFileOffset(ShapeLayer * sf1, float offset, int offset_type);

inline static ShapeFile *AS_VectorOffset(ShapeFile * f,float offset)
{
    return ShapeFileOffset(f->GetLayer(0),offset,LISEM_OFFSET_ROUND);
}

//returns a list of shapes that are given by the operator applied to shape1 and shape2. Thus result = s1 *s2 (with * some operator)
inline static ShapeFile * ShapeFileOffset(ShapeLayer * sf1, float offset, int offset_type)
{
    std::cout << "doing shape offset " << std::endl;

    if(sf1 == nullptr)
    {
        return new ShapeFile();
    }
    ShapeFile * ret_sf = new ShapeFile();
    ShapeLayer * ret_layer = new ShapeLayer();
    ret_sf->AddLayer(ret_layer);
    sf1 = sf1->Copy();

    //Brute force n2, add all intersections betwee sf1 and sf2
    for(int i = 0; i < sf1->GetFeatureCount(); i++)
    {
        //prepare input
        Feature * f1 = sf1->GetFeature(i);
        QList<LSMShape *> in_A;
        for(int j = 0; j < f1->GetShapeCount(); j++)
        {
            in_A.append(f1->GetShape(j));
        }

        QList<LSMShape *> ret_A = ShapeOffset(in_A,offset,offset_type);
        for(int j = 0; j < ret_A.length(); j++)
        {
            Feature * fnew = new Feature();
            fnew->AddShape(ret_A.at(j));
            ret_layer->AddFeature(fnew);
        }
    }
    sf1->Destroy();
    delete sf1;

    return ret_sf;
}

inline static QList<LSMShape * >ShapeOffset(QList<LSMShape *> shapes,float offset, int offset_type)
{

    //run clipperlib offset code

    BoundingBox extent;
    for(int i = 0; i < shapes.length(); i++)
    {
        LSMShape * s1 = shapes.at(i);
        if(i == 0)
        {
            extent = s1->GetBoundingBox();
        }else {
            extent.MergeWith(s1->GetBoundingBox());
        }
    }

    extent.Scale(3);

    if(extent.GetSizeX() <offset * 4)
    {
        extent.Set(extent.GetMinX() - offset * 1.5,extent.GetMaxX() + offset * 1.5,extent.GetMinY(),extent.GetMaxY());
    }
    if(extent.GetSizeY() <offset * 4)
    {
        extent.Set(extent.GetMinX(),extent.GetMaxX(),extent.GetMinY() - offset * 1.5,extent.GetMaxY() + offset * 1.5);
    }
    if(extent.GetSizeX() > extent.GetSizeY())
    {
        extent.Set(extent.GetMinX(),extent.GetMaxX(),extent.GetCenterY() - extent.GetSizeX() *0.5,extent.GetCenterY() +  extent.GetSizeX() *0.5);
    }


    ClipperLib::ClipperOffset C;

    //fill the paths

    int n = 0;

    //find out how many paths we need to allocate
    for(int i = 0; i < shapes.length(); i++)
    {
        LSMShape * s1 = shapes.at(i);
        int type1 = s1->GetType();

        if(type1 == LISEM_SHAPE_UNKNOWN)
        {
            continue;
        }else if(type1 == LISEM_SHAPE_MULTIPOINT)
        {
            if(offset > 0.0f)
            {
                n++;
                LSMMultiPoint *msp = (LSMMultiPoint*)s1;
                for(int j = 0; j < msp->GetPointCount(); j++)
                {
                    LSMPoint * point = msp->GetPoint(j);
                    ClipperLib::Paths Path_A(1);
                    PointToClipperPath(point,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                }
            }

        }else if(type1 == LISEM_SHAPE_MULTILINE)
        {
            if(offset > 0.0f)
            {
                n++;
                LSMMultiLine *msp = (LSMMultiLine*)s1;
                for(int j = 0; j < msp->GetLineCount(); j++)
                {
                    LSMLine * line = msp->GetLine(j);
                    if(line->GetVertexCount() > 1)
                    {
                        ClipperLib::Paths Path_A(1);
                        LineToClipperPath(line,&Path_A,extent,0);
                        C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                    }
                }
            }

        }else if(type1 == LISEM_SHAPE_MULTIPOLYGON)
        {
            LSMMultiPolygon *msp = (LSMMultiPolygon*)s1;
            for(int j = 0; j < msp->GetPolygonCount(); j++)
            {
                LSMPolygon * polygon = msp->GetPolygon(j);
                if(polygon->GetExtRing() != nullptr)
                {
                    n++;
                    ClipperLib::Paths Path_A(polygon->GetIntRingCount() + 1);
                    PolygonToClipperPath(polygon,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etClosedPolygon);
                }
            }
        }else if(type1 == LISEM_SHAPE_POINT)
        {
            if(offset > 0.0f)
            {
                n++;
                LSMPoint * point = (LSMPoint *) s1;
                ClipperLib::Paths Path_A(1);
                PointToClipperPath(point,&Path_A,extent,0);
                C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
            }

        }else if(type1 == LISEM_SHAPE_POLYGON)
        {
            LSMPolygon * polygon = (LSMPolygon *) s1;
            if(polygon->GetExtRing() != nullptr)
            {
                n++;
                ClipperLib::Paths Path_A(polygon->GetIntRingCount() + 1);
                PolygonToClipperPath(polygon,&Path_A,extent,0);
                C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etClosedPolygon);
            }
        }else if(type1 == LISEM_SHAPE_LINE)
        {
            if(offset > 0.0f)
            {
                n++;
                LSMLine * line = (LSMLine *) s1;
                if(line->GetVertexCount() > 1)
                {
                    ClipperLib::Paths Path_A(1);
                    LineToClipperPath(line,&Path_A,extent,0);
                    C.AddPaths(Path_A,ClipperLib::jtRound,ClipperLib::etOpenRound);
                }
            }
        }
    }

    //recalculate offset value

    double offsetn = 2.0*9e18*(offset/extent.GetSizeX());

    //ClipperLib::Paths Polygon1(n_path_1);
    ClipperLib::PolyTree Solution;
    C.ArcTolerance = offsetn * 0.01;
    QList<LSMShape*> ret_pol;


    if(n > 0)
    {
        std::cout << offsetn << " " << C.ArcTolerance << " " << extent.GetMinX() << " " << extent.GetMaxX() << " " << extent.GetMinY() << " " << extent.GetMaxY() << std::endl;
        std::cout << "execute offset " << std::endl;
        C.Execute(Solution,offsetn);
        std::cout << "end execute offset " << std::endl;

        for(int i = 0; i < Solution.Childs.size(); i++)
        {
            PolygonTreeToPolygonList(&ret_pol,Solution.Childs.at(i),extent,nullptr);
        }

    }


    return ret_pol;

}






#endif // SHAPEOFFSET_H
