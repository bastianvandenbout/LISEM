
#pragma once

#include "geometry/vonoroi.h"
#include "geo/shapes/shapefile.h"

inline static ShapeFile * AS_ShapeToPoints(ShapeFile * in, bool centroids, float boxscale = 1.0)
{
    ShapeFile * ret = new ShapeFile();
    ret->CopyFrom(in);

    if(ret->GetLayerCount() == 0)
    {
        LISEMS_ERROR("No layers in Shapes");
        return ret;
    }
    ShapeLayer * l = ret->GetLayer(0);

    BoundingBox b = l->GetAndCalcBoundingBox();
    b.Scale(boxscale);
    jcv_rect rect;
    rect.min.x = b.GetMinX();
    rect.min.y = b.GetMinY();
    rect.max.x = b.GetMaxX();
    rect.max.y = b.GetMaxY();

    std::cout << "layer found, "  << l->GetFeatureCount()<< std::endl;
    if(!centroids)
    {
        for(int findex = 0; findex < l->GetFeatureCount(); findex++)
        {

            Feature * f = l->GetFeature(findex);

            std::vector<LSMShape * > shapes;
            for(int j2 = 0; j2 < f->GetShapeCount(); j2++)
            {
                LSMShape * m_Shape = f->GetShape(j2);
                int type = m_Shape->GetType();
                if(type == LISEM_SHAPE_POINT)
                {
                    _jcv_point p;
                    p.x= ((LSMPoint*)(m_Shape))->GetX();
                    p.y = ((LSMPoint*)(m_Shape))->GetY();
                    f->RemoveShape(m_Shape);
                    shapes.push_back(new LSMPoint(p.x,p.y));

                }else if(type == LISEM_SHAPE_LINE)
                {
                    LSMLine * l = (LSMLine *)(m_Shape);
                    int count = l->GetVertexCount();
                    double * data = l->GetVertexData();

                    f->RemoveShape(m_Shape);
                    for(int i = 0; i < count; i++)
                    {
                        _jcv_point p;
                        p.x= data[i*2];
                        p.y = data[i*2+1];
                        shapes.push_back(new LSMPoint(p.x,p.y));
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

                    f->RemoveShape(m_Shape);

                    double * data = r->GetVertexData();
                    int i = 0;
                    for(i = 0; i < count; i++)
                    {
                        _jcv_point p;
                        p.x= data[i*2];
                        p.y = data[i*2+1];
                        shapes.push_back(new LSMPoint(p.x,p.y));
                    }
                    for(int j = 0; j < p->GetIntRingCount(); j++)
                    {
                        Ring * ir = p->GetIntRing(j);
                        double * datai = ir->GetVertexData();
                        for(int k; k < ir->GetVertexCount(); k++)
                        {
                            _jcv_point p;
                            p.x= data[k*2];
                            p.y = data[k*2+1];
                            shapes.push_back(new LSMPoint(p.x,p.y));
                            i++;
                        }
                    }

                }else if(type == LISEM_SHAPE_MULTIPOINT)
                {
                    LSMMultiPoint * l = (LSMMultiPoint*)(m_Shape);
                    int pc = l->GetPointCount();


                    f->RemoveShape(m_Shape);

                    for(int i = 0; i < pc; i++)
                    {
                        _jcv_point p;
                        p.x= l->GetPoint(i)->GetX();
                        p.y = l->GetPoint(i)->GetY();
                        shapes.push_back(new LSMPoint(p.x,p.y));

                    }
                }else if(type == LISEM_SHAPE_MULTILINE)
                {

                    LSMMultiLine * ml = (LSMMultiLine*)(m_Shape);
                    int count = 0;
                    for(int i = 0; i < ml->GetLineCount(); i++)
                    {
                        count += ml->GetLine(i)->GetVertexCount();
                    }


                    f->RemoveShape(m_Shape);

                    int ir = 0;
                    for(int i = 0; i < ml->GetLineCount(); i++)
                    {
                        LSMLine * l = (LSMLine *)(ml->GetLine(i));
                        count = l->GetVertexCount();
                        double * data = l->GetVertexData();

                        for(int j = 0; j < count; j++)
                        {
                            _jcv_point p;
                            p.x= data[j*2];
                            p.y = data[j*2+1];
                            shapes.push_back(new LSMPoint(p.x,p.y));
                            ir ++;
                        }
                    }

                }else if(type == LISEM_SHAPE_MULTIPOLYGON)
                {

                    LSMMultiPolygon * ml = (LSMMultiPolygon*)(m_Shape);

                    f->RemoveShape(m_Shape);

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
                            _jcv_point p;
                            p.x= data[i*2];
                            p.y = data[i*2+1];
                            shapes.push_back(new LSMPoint(p.x,p.y));
                            ir ++;
                        }
                        for(int j = 0; j < p->GetIntRingCount(); j++)
                        {
                            Ring * inr = p->GetIntRing(j);
                            double * datai = inr->GetVertexData();
                            for(int k; k < inr->GetVertexCount(); k++)
                            {
                                _jcv_point p;
                                p.x= data[k*2];
                                p.y = data[k*2+1];
                                shapes.push_back(new LSMPoint(p.x,p.y));
                                ir ++;
                            }
                        }
                    }
                }
            }

            for(int k = 0; k < shapes.size(); k++)
            {
                f->AddShape(shapes.at(k));
            }
        }
    }else
    {
        for(int i = 0; i < l->GetFeatureCount(); i++)
        {
            Feature * f = l->GetFeature(i);

            std::vector<LSMShape * > shapes;

            for(int j = 0; j < f->GetShapeCount(); j++)
            {
                LSMShape * m_Shape = f->GetShape(j);
                f->RemoveShape(m_Shape);
                LSMVector2 centroid = m_Shape->GetCentroid();
                _jcv_point p;
                p.x= centroid.x;
                p.y=centroid.y;
                shapes.push_back(new LSMPoint(p.x,p.y));
            }


            for(int k = 0; k < shapes.size(); k++)
            {
                f->AddShape(shapes.at(k));
            }

        }
    }

    l->SetType(LISEM_SHAPE_POINT);
    return ret;
}


inline static ShapeFile * AS_Vonoroi(ShapeFile * in, bool centroids)
{

    ShapeFile * ret = new ShapeFile();
    //get point count
    if(in->GetLayerCount() == 0)
    {
        return ret;
    }
    ShapeLayer * l = in->GetLayer(0);

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));

    std::vector<_jcv_point> pointdata;

    if(!centroids)
    {
        for(int findex = 0; findex < l->GetFeatureCount(); findex++)
        {
            Feature * f = l->GetFeature(findex);
            for(int j2 = 0; j2 < f->GetShapeCount(); j2++)
            {
                LSMShape * m_Shape = f->GetShape(j2);
                int type = m_Shape->GetType();
                if(type == LISEM_SHAPE_POINT)
                {
                    _jcv_point p;
                    p.x= ((LSMPoint*)(m_Shape))->GetX();
                    p.y = ((LSMPoint*)(m_Shape))->GetY();
                    pointdata.push_back(p);

                }else if(type == LISEM_SHAPE_LINE)
                {
                    LSMLine * l = (LSMLine *)(m_Shape);
                    int count = l->GetVertexCount();
                    double * data = l->GetVertexData();
                    for(int i = 0; i < count; i++)
                    {
                        _jcv_point p;
                        p.x= data[i*2];
                        p.y = data[i*2+1];
                        pointdata.push_back(p);
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
                        _jcv_point p;
                        p.x= data[i*2];
                        p.y = data[i*2+1];
                        pointdata.push_back(p);
                    }
                    for(int j = 0; j < p->GetIntRingCount(); j++)
                    {
                        Ring * ir = p->GetIntRing(j);
                        double * datai = ir->GetVertexData();
                        for(int k; k < ir->GetVertexCount(); k++)
                        {
                            _jcv_point p;
                            p.x= data[k*2];
                            p.y = data[k*2+1];
                            pointdata.push_back(p);
                            i++;
                        }
                    }

                }else if(type == LISEM_SHAPE_MULTIPOINT)
                {
                    LSMMultiPoint * l = (LSMMultiPoint*)(m_Shape);
                    int pc = l->GetPointCount();

                    for(int i = 0; i < pc; i++)
                    {
                        _jcv_point p;
                        p.x= l->GetPoint(i)->GetX();
                        p.y = l->GetPoint(i)->GetY();
                        pointdata.push_back(p);

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
                            _jcv_point p;
                            p.x= data[j*2];
                            p.y = data[j*2+1];
                            pointdata.push_back(p);
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
                            _jcv_point p;
                            p.x= data[i*2];
                            p.y = data[i*2+1];
                            pointdata.push_back(p);
                            ir ++;
                        }
                        for(int j = 0; j < p->GetIntRingCount(); j++)
                        {
                            Ring * inr = p->GetIntRing(j);
                            double * datai = inr->GetVertexData();
                            for(int k; k < inr->GetVertexCount(); k++)
                            {
                                _jcv_point p;
                                p.x= data[k*2];
                                p.y = data[k*2+1];
                                pointdata.push_back(p);
                                ir ++;
                            }
                        }

                    }


                }
            }
        }
    }else
    {
        for(int i = 0; i < l->GetFeatureCount(); i++)
        {
            Feature * f = l->GetFeature(i);
            for(int j = 0; j < f->GetShapeCount(); j++)
            {
                LSMShape * m_Shape = f->GetShape(j);
                LSMVector2 centroid = m_Shape->GetCentroid();
                _jcv_point p;

                if(!std::isnan(centroid.x) && !std::isnan(centroid.y))
                {
                    p.x = centroid.x;
                    p.y = centroid.y;
                    pointdata.push_back(p);
                }
            }
        }
    }


std::cout << "do output" << std::endl;


    //get all the points

    //generate diagram
    jcv_diagram_generate(pointdata.size(),pointdata.data(),nullptr,&diagram);

    std::cout << pointdata.size() << std::endl;
    std::cout << diagram.numsites << std::endl;

    ShapeLayer* lay = new ShapeLayer();
    lay->AddAttribute("ID",ATTRIBUTE_TYPE_INT64);

    //get back the polyons
    const jcv_site* sites = jcv_diagram_get_sites( &diagram );
    for( int i = 0; i < diagram.numsites; ++i )
    {
        const jcv_site* site = &sites[i];


        LSMPolygon * p = new LSMPolygon();
        Ring* r = new Ring();

        const jcv_graphedge* e = site->edges;
        bool first = true;
        while( e )
        {

            if(first)
            {
                first = false;
                r->AddVertex(e->pos[0].x,e->pos[0].y);
            }
            r->AddVertex(e->pos[1].x,e->pos[1].y);
            e = e->next;
        }

        std::cout << "pre "<< r->GetVertexCount() << std::endl;
        //r->CloseIfNot();
        r->RemoveSelfIntersection();
        std::cout << "post "<<  r->GetVertexCount() << std::endl;
        if(r->Is_ClockWise())
        {
            r->Reverse();

        }

        p->SetExtRing(r);
        Feature*f = new Feature();
        f->AddShape(p);
        int fid = lay->AddFeature(f);
        lay->SetFeatureAttribute(fid,QString("ID"),QString::number(site->index));
    }

    std::cout << "created layer " << std::endl;

    ret->AddLayer(lay);

    std::cout << lay->GetFeatureCount() << std::endl;

    //return result
    jcv_diagram_free( &diagram );

    return ret;
}
