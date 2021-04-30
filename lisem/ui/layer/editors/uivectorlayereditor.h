#pragma once

#include "layer/uilayereditor.h"

#include "layer/uilayereditor.h"
#include "layer/geo/uivectorlayer.h"
#include "geo/shapes/shapefile.h"


class UIVectorLayer;


typedef struct VertexRef
{
    float * x;
    float * y;
    LSMShape * s;
    Feature * f;
} VertexRef;


class UIVectorLayerEditor : public UILayerEditor
{


private:

    UIVectorLayer * m_VectorLayer;

    ShapeFile * m_OriginalShapes;
    ShapeFile * m_Shapes;

    QList<Feature *> m_DialogRemovedFeatures;
    QList<Feature *> m_SelectedFeatures;
    QList<std::pair<LSMShape *,Feature *>> m_SelectedShapes;
    QList<std::tuple<double*,double*,LSMShape *,Feature *>> m_SelectedVertices;
    Feature * m_SelectedFeature = nullptr;
    LSMShape * m_SelectedShape = nullptr;
    LSMShape * m_SelectedShapeC = nullptr;

    double ** x1t = new (double*);
    double ** y1t = new (double*);
    double ** x2t = new (double*);
    double ** y2t = new (double*);

    bool m_StartedDragging = false;
    bool m_StoppedDragging = false;
    bool m_Dragging = false;
    LSMVector3 m_DragOrigin;
    LSMVector3 m_DragPrevious;
    LSMVector3 m_DragDelta;
    LSMVector3 m_DragEnd;
    bool m_HasSquare = false;
    bool m_HasCircle = false;
    QList<LSMVector2> m_PolygonPoints;
    LSMMultiPolygon *m_PolygonFill = nullptr;
    std::vector<float> Polygonpx;
    std::vector<float> Polygonpy;
    std::vector<float> m_PolygonFillX;
    std::vector<float> m_PolygonFillY;

    QList<int> m_EditsSimultaneityCount;
    QList<ShapeLayerChange> m_Edits;

    bool m_AskAttributes;

    LSMVector2 m_DragOffset = LSMVector2(0.0,0.0);
    LSMVector2 m_closestVertex;
    LSMVector2 m_closestEdgeLocation;

    bool m_HasDoneDelete = false;
    bool m_HasDoneEnter = false;
    bool m_HasDoneCtrlZ = false;
    bool m_HasDoneMouseClick = false;
    bool m_HasDoneRightMouseClick = false;
    bool m_HasDoneMouseDoubleClick = false;
    bool m_HasDoneEscape = false;
    bool m_AltPressed=  false;


    bool m_DoAlterAttributeDialog = false;
    Feature * m_DoAlterAttributeFeature = nullptr;
public:

    UIVectorLayerEditor(UILayer * rl);


    void OnPrepare(OpenGLCLManager * m)
    {
        UILayerEditor::OnPrepare(m);
        m_IsPrepared = true;
    }
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        UILayerEditor::OnCRSChanged(m,s,tm);
    }

    inline LSMShape * GetSelectedShape()
    {
        return m_SelectedShape;
    }
    inline LSMShape * GetSelectedShapeC()
    {
        return m_SelectedShape;
    }





    inline Feature * GetSelectedFeature()
    {
        return m_SelectedFeature;
    }

    inline void ClearSelected()
    {
        m_DragOffset = LSMVector2(0.0,0.0);

        m_SelectedFeatures.clear();
        m_SelectedShapes.clear();
        m_SelectedVertices.clear();
        m_SelectedFeature = nullptr;
        m_SelectedShape = nullptr;
        if( m_SelectedShapeC != nullptr)
        {
            delete m_SelectedShapeC;
            m_SelectedShapeC = nullptr;
        }
    }
    inline void SetSelected(Feature*f, LSMShape * s)
    {
        ClearSelected();

        m_SelectedFeature = f;
        m_SelectedShape = s;
        if( m_SelectedShapeC != nullptr)
        {
            delete m_SelectedShapeC;
            m_SelectedShapeC = nullptr;
        }
        if(s!= nullptr)
        {
            m_SelectedShapeC = s->Copy();
        }else
        {
            m_SelectedShapeC = nullptr;
        }
    }
    inline bool IsNothingSelected()
    {
        if(m_SelectedFeatures.length() > 0)
        {
            return false;
        }
        if(m_SelectedShapes.length() > 0)
        {
            return false;
        }
        if(m_SelectedVertices.length() > 0)
        {
            return false;
        }
        if(m_SelectedFeature != nullptr || m_SelectedShape != nullptr)
        {
            return false;
        }


        return true;
    }
    inline void ClearCurrentEditShape()
    {
        m_HasSquare = false;
        m_HasCircle = false;
        m_PolygonPoints.clear();
        if(m_PolygonFill != nullptr)
        {
            m_PolygonFill->Destroy();
            delete m_PolygonFill;
            m_PolygonFill = nullptr;
        }
        Polygonpx.clear();
        Polygonpy.clear();
        m_PolygonFillX.clear();
        m_PolygonFillY.clear();
    }

    inline void OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions) override
    {
        m_Dragging = false;
        if(name.compare("Selection") == 0 || name.compare("Shape") == 0 || name.compare("Type") == 0)
        {

            m_HasDoneEscape = false;
            m_HasDoneDelete = false;
            m_HasDoneEnter = false;
            m_HasDoneCtrlZ = false;
            m_HasDoneMouseClick = false;
            m_HasDoneMouseDoubleClick = false;
            m_StartedDragging = false;

            if(!(name.compare("Type") == 0 && value == "Move"))
            {

                ClearSelected();
                ClearCurrentEditShape();
            }
        }

        if(name.compare("Ask For Attributes") == 0)
        {
            m_AskAttributes = value.toInt();
        }

        if(name.compare("Topological Editing") == 0)
        {

        }
        if(name.compare("Topological Distance") == 0)
        {

        }
        if(name.compare("Value") == 0)
        {

        }
        if(name.compare("Circle Points") == 0)
        {

        }
    }

    void OnDraw(OpenGLCLManager * m, GeoWindowState s );

    void OnDestroy(OpenGLCLManager * m)
    {

    }
    void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);

    void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDraw3DGeoPost(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline void OnKeyPressed(int button, int action, int modifiers) override
    {

        if((button == GLFW_KEY_LEFT_ALT || button == GLFW_KEY_RIGHT_ALT ) && action == GLFW_PRESS)
        {
            m_AltPressed = true;
        }

        if((button == GLFW_KEY_LEFT_ALT || button == GLFW_KEY_RIGHT_ALT ) && action == GLFW_RELEASE)
        {
            m_AltPressed = false;
        }

        if(modifiers & GLFW_MOD_CONTROL)
        {

            if(button == GLFW_KEY_Z && action == GLFW_PRESS)
            {
                m_HasDoneCtrlZ = true;
            }
        }

        if(button == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            m_HasDoneEscape = true;
        }

        if(button == GLFW_KEY_ENTER && action == GLFW_PRESS)
        {
            m_HasDoneEnter= true;
        }

        if(button == GLFW_KEY_DELETE && action == GLFW_PRESS)
        {
            m_HasDoneDelete= true;
        }
    }


    inline void OnGeoMousePressed(int button, int action,GeoWindowState s, LSMVector3 pos) override
    {

        QString edit_type_string = GetParameterValue("Type");
        QString edit_shapetype_string = GetParameterValue("Shape");
        QString edit_selecttype_string = GetParameterValue("Selection");

        if(m_AltPressed)
        {
            std::cout << "altpressed " << std::endl;
            edit_type_string = "Select (alt)";
            /*if(!(edit_selecttype_string == "Shape" || edit_selecttype_string == "Feature"))
            {
                edit_selecttype_string = "Shape";
            }*/
        }

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {

            m_DragOffset = LSMVector2(0.0,0.0);

            if(((edit_type_string=="Select (alt)") && (edit_selecttype_string=="Shape") || (edit_selecttype_string=="Feature")) || (edit_type_string=="Attributes") ||  ((edit_type_string=="Move") && IsNothingSelected()))
            {

                {
                    LSMVector2 locn = m_Transformer->Transform(LSMVector2(s.MouseGeoPosX,s.MouseGeoPosZ));

                    double tolerence = 0.02 *std::min(s.width,s.height);
                    QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

                    if(features.length() > 0)
                    {

                        //allways pick first feature
                        //for(int j = 0; j < features.size(); j++)
                        {

                            if(edit_type_string.compare("Edit Attributes") == 0)
                            {
                                m_DoAlterAttributeDialog = true;
                                m_DoAlterAttributeFeature = features.at(0);
                            }

                            for(int i = 0; i < features.at(0)->GetShapeCount(); i++)
                            {
                                LSMShape * s = features.at(0)->GetShape(i);
                                if(s->Contains(locn,tolerence))
                                {
                                    m_SelectedFeature = features.at(0);
                                    m_SelectedShape = s;
                                    if( m_SelectedShapeC != nullptr)
                                    {
                                        delete m_SelectedShapeC;
                                        m_SelectedShapeC = nullptr;
                                    }
                                    if(s!= nullptr)
                                    {
                                        m_SelectedShapeC = s->Copy();
                                    }else
                                    {
                                        m_SelectedShapeC = nullptr;
                                    }
                                    break;
                                }
                            }
                        }
                    }else {

                        ClearSelected();

                    }
                }
            }

            if(m_Dragging == false)
            {
                m_StartedDragging = true;
                m_Dragging = true;

                m_DragOrigin= pos;

                if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string == "Shape")  || (edit_selecttype_string == "Feature")))  || edit_type_string == "Add/Remove Shape" )
                {
                    if(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 )
                    {
                        m_HasCircle = true;
                    }
                }
                if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string == "Shape")  || (edit_selecttype_string == "Feature")))  || (edit_type_string == "Add/Remove Shape") )
                {
                    if(edit_shapetype_string.compare("Rectangle") == 0 ||edit_shapetype_string.compare("Rectangle (Outline)") == 0 )
                    {
                        m_HasSquare = true;
                    }
                }

            }
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {

            m_HasDoneMouseClick = true;
            if( action == GLFW_RELEASE ){
                    static auto before = std::chrono::system_clock::now();
                    auto now = std::chrono::system_clock::now();
                    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();
                    before = now;
                    if(diff_ms>10 && diff_ms<200){

                        m_HasDoneMouseDoubleClick = true;

                    }
                }
            if(m_Dragging == true)
            {
                m_Dragging = false;
                m_StoppedDragging = true;
                m_DragEnd = pos;
            }


            if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string == "Shape")  || (edit_selecttype_string == "Feature")))  || edit_type_string.compare("Add/Remove Shape") == 0)
            {
                if(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 )
                {
                    m_HasCircle = true;
                }
            }
            if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string == "Shape")  || (edit_selecttype_string == "Feature")))  || (edit_type_string == "Add/Remove Shape") )
            {
                if(edit_shapetype_string.compare("Rectangle") == 0 ||edit_shapetype_string.compare("Rectangle (Outline)") == 0 )
                {
                    m_HasSquare = true;
                }
            }


            if(m_StoppedDragging)
            {

                if((edit_type_string.compare("Select (alt)") == 0) &&(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 ))
                {
                    LSMVector3 endpos = m_DragEnd;

                    float tlX = m_DragOrigin.x;
                    float tlY = m_DragOrigin.z;

                    float brX = endpos.x;
                    float brY = endpos.z;

                    std::vector<double> ppx;
                    std::vector<double> ppy;

                    LSMVector2 br = m_Transformer->Transform(LSMVector2(endpos.x,endpos.z));
                    LSMVector2 tl = m_Transformer->Transform(LSMVector2(m_DragOrigin.x,m_DragOrigin.z));

                    float r = sqrt((tl.y - br.y)*(tl.y - br.y) + (tl.x - br.x)*(tl.x - br.x));


                    LSMPolygon * snew = new LSMPolygon();
                    snew->SetAsRegularNGon(tl,r);

                    Ring* ring = snew->GetRing(0);
                    if(ring != nullptr)
                    {
                        for(int k = 0; k < ring->GetVertexCount(); k++)
                        {

                            ppx.push_back(ring->GetVertex(k).x);
                            ppy.push_back(ring->GetVertex(k).y);
                        }
                    }



                    delete snew;

                    if(edit_selecttype_string.compare("Vertices") == 0)
                    {
                        QList<std::tuple<double*,double*,LSMShape *,Feature *>> list =  m_Shapes->GetOverlappingVertices(ppx,ppy);
                        for(int i = 0; i < list.size(); i++)
                        {
                            double * xpos = std::get<0>(list.at(i));
                            double * ypos = std::get<1>(list.at(i));

                            if(xpos == nullptr || ypos == nullptr)
                            {
                                std::cout << "invalid vertex " << std::endl;
                            }else
                            {
                                bool has = false;
                                for(int j = 0; j < m_SelectedVertices.length(); j++)
                                {

                                    if(m_SelectedVertices.at(j) == list.at(i))
                                    {
                                        has = true;
                                        break;
                                    }
                                }
                                if(!has)
                                {
                                    m_SelectedVertices.append(list.at(i));
                                }
                            }


                        }
                    }
                    if(edit_selecttype_string.compare("Shapes") == 0)
                    {
                        QList<std::pair<LSMShape *,Feature *>> list =  m_Shapes->GetOverlappingShapes(ppx,ppy);

                        for(int i = 0; i < list.size(); i++)
                        {
                            bool has = false;
                            for(int j = 0; j < m_SelectedShapes.length(); j++)
                            {
                                if(m_SelectedShapes.at(j) == list.at(i))
                                {
                                    has = true;
                                    break;
                                }
                            }
                            if(!has)
                            {
                                m_SelectedShapes.append(list.at(i));
                            }
                        }

                    }
                    if(edit_selecttype_string.compare("Features") == 0)
                    {

                        QList<Feature *> list = m_Shapes->GetOverlappingFeatures(ppx,ppy);

                        for(int i = 0; i < list.size(); i++)
                        {
                            bool has = false;
                            for(int j = 0; j < m_SelectedFeatures.length(); j++)
                            {
                                if(m_SelectedFeatures.at(j) == list.at(i))
                                {
                                    has = true;
                                    break;
                                }
                            }
                            if(!has)
                            {
                                m_SelectedFeatures.append(list.at(i));
                            }
                        }
                    }

                    m_HasCircle = false;




                }

                if((edit_type_string.compare("Select (alt)") == 0) &&(edit_shapetype_string.compare("Rectangle") == 0 || edit_shapetype_string.compare("Rectangle (Outline)") == 0))
                {
                    LSMVector3 endpos = m_DragEnd;

                    float tlX = m_DragOrigin.x;
                    float tlY = m_DragOrigin.z;

                    float brX = endpos.x;
                    float brY = endpos.z;

                    std::vector<double> ppx;
                    std::vector<double> ppy;
                    LSMVector2 point1 = m_Transformer->Transform(LSMVector2(tlX,tlY));
                    LSMVector2 point2 = m_Transformer->Transform(LSMVector2(brX,tlY));
                    LSMVector2 point3 = m_Transformer->Transform(LSMVector2(brX,brY));
                    LSMVector2 point4 = m_Transformer->Transform(LSMVector2(tlX,brY));

                    ppx.push_back(point1.x);
                    ppy.push_back(point1.y);
                    ppx.push_back(point2.x);
                    ppy.push_back(point2.y);
                    ppx.push_back(point3.x);
                    ppy.push_back(point3.y);
                    ppx.push_back(point4.x);
                    ppy.push_back(point4.y);

                    if(edit_selecttype_string.compare("Vertices") == 0)
                    {
                        QList<std::tuple<double*,double*,LSMShape *,Feature *>> list =  m_Shapes->GetOverlappingVertices(ppx,ppy);
                        for(int i = 0; i < list.size(); i++)
                        {
                            double * xpos = std::get<0>(list.at(i));
                            double * ypos = std::get<1>(list.at(i));

                            if(xpos == nullptr || ypos == nullptr)
                            {
                                std::cout << "invalid vertex " << std::endl;
                            }else
                            {
                                bool has = false;
                                for(int j = 0; j < m_SelectedVertices.length(); j++)
                                {

                                    if(m_SelectedVertices.at(j) == list.at(i))
                                    {
                                        has = true;
                                        break;
                                    }
                                }
                                if(!has)
                                {
                                    m_SelectedVertices.append(list.at(i));
                                }
                            }


                        }
                    }
                    if(edit_selecttype_string.compare("Shapes") == 0)
                    {
                        QList<std::pair<LSMShape *,Feature *>> list =  m_Shapes->GetOverlappingShapes(ppx,ppy);

                        for(int i = 0; i < list.size(); i++)
                        {
                            bool has = false;
                            for(int j = 0; j < m_SelectedShapes.length(); j++)
                            {
                                if(m_SelectedShapes.at(j) == list.at(i))
                                {
                                    has = true;
                                    break;
                                }
                            }
                            if(!has)
                            {
                                m_SelectedShapes.append(list.at(i));
                            }
                        }

                    }
                    if(edit_selecttype_string.compare("Features") == 0)
                    {

                        QList<Feature *> list = m_Shapes->GetOverlappingFeatures(ppx,ppy);

                        for(int i = 0; i < list.size(); i++)
                        {
                            bool has = false;
                            for(int j = 0; j < m_SelectedFeatures.length(); j++)
                            {
                                if(m_SelectedFeatures.at(j) == list.at(i))
                                {
                                    has = true;
                                    break;
                                }
                            }
                            if(!has)
                            {
                                m_SelectedFeatures.append(list.at(i));
                            }
                        }
                    }

                    m_HasSquare = false;
                }
            }

            if(m_HasDoneMouseDoubleClick)
            {

                if((edit_type_string.compare("Select (alt)")) == 0)
                {
                    if(edit_shapetype_string.compare("Polygon") == 0 || edit_shapetype_string.compare("Lines") == 0)
                    {

                        if(m_PolygonPoints.length() > 2)
                        {

                            LSMPolygon * p = new LSMPolygon();
                            Ring * r = new Ring();
                            for(int i = 0; i < m_PolygonPoints.length(); i++)
                            {
                                r->AddVertex(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y);
                            }
                            p->SetExtRing(r);

                            std::vector<double> ppx;
                            std::vector<double> ppy;
                            for(int i = 0; i < m_PolygonPoints.length(); i++)
                            {
                                LSMVector2 point = m_Transformer->Transform(LSMVector2(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y));

                                ppx.push_back(point.x);
                                ppy.push_back(point.y);
                            }

                            if(edit_selecttype_string.compare("Vertices") == 0)
                            {
                                QList<std::tuple<double*,double*,LSMShape *,Feature *>> list = m_Shapes->GetOverlappingVertices(ppx,ppy);
                                std::cout <<" selected vertices" << list.size() << std::endl;

                                for(int i = 0; i < list.size(); i++)
                                {
                                    double * xpos = std::get<0>(list.at(i));
                                    double * ypos = std::get<1>(list.at(i));

                                    std::cout << xpos << " " << ypos << std::endl;
                                    if(xpos == nullptr || ypos == nullptr)
                                    {
                                        std::cout << "invalid vertex " << std::endl;
                                    }else
                                    {
                                        bool has = false;
                                        for(int j = 0; j < m_SelectedVertices.length(); j++)
                                        {
                                            if(m_SelectedVertices.at(j) == list.at(i))
                                            {
                                                has = true;
                                                break;
                                            }
                                        }
                                        if(!has)
                                        {
                                            m_SelectedVertices.append(list.at(i));
                                        }
                                    }
                                }
                            }
                            if(edit_selecttype_string.compare("Shapes") == 0)
                            {
                                QList<std::pair<LSMShape *,Feature *>> list =  m_Shapes->GetOverlappingShapes(ppx,ppy);
                                std::cout <<" selected shapes" << list.size() << std::endl;
                                for(int i = 0; i < list.size(); i++)
                                {
                                    bool has = false;
                                    for(int j = 0; j < m_SelectedShapes.length(); j++)
                                    {
                                        if(m_SelectedShapes.at(j) == list.at(i))
                                        {
                                            has = true;
                                            break;
                                        }
                                    }
                                    if(!has)
                                    {
                                        m_SelectedShapes.append(list.at(i));
                                    }
                                }

                            }
                            if(edit_selecttype_string.compare("Features") == 0)
                            {

                                QList<Feature *> list = m_Shapes->GetOverlappingFeatures(ppx,ppy);

                                std::cout <<" selected features " << list.size() << std::endl;
                                for(int i = 0; i < list.size(); i++)
                                {
                                    bool has = false;
                                    for(int j = 0; j < m_SelectedFeatures.length(); j++)
                                    {
                                        if(m_SelectedFeatures.at(j) == list.at(i))
                                        {
                                            has = true;
                                            break;
                                        }
                                    }
                                    if(!has)
                                    {
                                        m_SelectedFeatures.append(list.at(i));
                                    }
                                }
                            }

                            m_PolygonPoints.clear();
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                            Polygonpx.clear();
                            Polygonpy.clear();

                            if(m_PolygonFill != nullptr)
                            {
                                m_PolygonFill->Destroy();
                                delete m_PolygonFill;
                                m_PolygonFill = nullptr;
                            }

                        }else if( ((edit_selecttype_string == "Shapes")  || (edit_selecttype_string == "Features")))
                        {
                            LSMVector2 locn = m_Transformer->Transform(LSMVector2(s.MouseGeoPosX,s.MouseGeoPosZ));

                            double tolerence = 0.02 *std::min(s.width,s.height);
                            QList<Feature *> features = m_Shapes->GetFeaturesAt(locn,tolerence);

                            if(features.length() > 0)
                            {

                                //allways pick first feature
                                //for(int j = 0; j < features.size(); j++)
                                {
                                    for(int i = 0; i < features.at(0)->GetShapeCount(); i++)
                                    {
                                        LSMShape * s = features.at(0)->GetShape(i);
                                        if(s->Contains(locn,tolerence))
                                        {
                                            if((edit_selecttype_string == "Features") && !m_SelectedFeatures.contains(features.at(0)))
                                            {
                                                m_SelectedFeatures.append(features.at(0));
                                            }else
                                            {
                                                bool contains = false;
                                                for(int j = 0; j < m_SelectedShapes.size(); j++)
                                                {
                                                    if(m_SelectedShapes.at(j).first == s)
                                                    {
                                                        contains = true;
                                                        break;
                                                    }
                                                }
                                                if(!contains)
                                                {
                                                    m_SelectedShapes.append(std::pair<LSMShape*, Feature*>(s,features.at(0)));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if(((edit_type_string == "Select (alt)") && !((edit_selecttype_string=="Shape") || (edit_selecttype_string == "Feature")) )  || (edit_type_string=="Add/Remove Shape")  || (edit_type_string=="Split") )
                {
                    if(edit_shapetype_string.compare("Polygon") == 0 || edit_shapetype_string.compare("Lines") == 0)
                    {
                        m_PolygonPoints.append(LSMVector2(pos.x,pos.z));


                        if(m_PolygonPoints.length() > 1)
                        {

                            Polygonpx.clear();
                            Polygonpy.clear();

                            for(int i = 0; i < m_PolygonPoints.length(); i++)
                            {
                                Polygonpx.push_back(m_PolygonPoints.at(i).x);
                                Polygonpy.push_back(m_PolygonPoints.at(i).y);
                            }
                        }else
                        {
                            Polygonpx.clear();
                            Polygonpy.clear();
                        }

                        if(m_PolygonPoints.length() > 2)
                        {
                            LSMPolygon * p = new LSMPolygon();
                            Ring * r = new Ring();
                            for(int i = 0; i < m_PolygonPoints.length(); i++)
                            {
                                r->AddVertex(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y);
                            }
                            p->SetExtRing(r);
                            if(m_PolygonFill != nullptr)
                            {
                                m_PolygonFill->Destroy();
                                delete m_PolygonFill;
                                m_PolygonFill = nullptr;
                            }
                            m_PolygonFill = p->Triangulate();
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                            for(int i = 0; i < m_PolygonFill->GetPolygonCount(); i++)
                            {
                                LSMPolygon * p = m_PolygonFill->GetPolygon(i);
                                if(p->GetExtRing()->GetVertexCount() > 2)
                                {
                                    double * data = p->GetExtRing()->GetVertexData();
                                    m_PolygonFillX.push_back(data[0]);
                                    m_PolygonFillY.push_back(data[1]);
                                    m_PolygonFillX.push_back(data[2]);
                                    m_PolygonFillY.push_back(data[3]);
                                    m_PolygonFillX.push_back(data[4]);
                                    m_PolygonFillY.push_back(data[5]);
                                }
                            }
                        }else
                        {
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                        }
                    }
                }
            }


        }

        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {

            if(((edit_type_string=="Select (alt)")  || ((edit_type_string=="Move"))))
            {
                ClearSelected();

                m_PolygonPoints.clear();
                m_PolygonFillX.clear();
                m_PolygonFillY.clear();
                Polygonpx.clear();
                Polygonpy.clear();

                if(m_PolygonFill != nullptr)
                {
                    m_PolygonFill->Destroy();
                    delete m_PolygonFill;
                    m_PolygonFill = nullptr;
                }

            }

            if(edit_type_string.compare("Add/Remove Shape") == 0)
            {

                if(edit_shapetype_string.compare("Polygon") == 0 || edit_shapetype_string.compare("Lines") == 0)
                {

                    if(m_PolygonPoints.length() > 1)
                    {
                        m_PolygonPoints.removeLast();
                        if(m_PolygonPoints.length() > 2)
                        {
                            LSMPolygon * p = new LSMPolygon();
                            Ring * r = new Ring();
                            for(int i = 0; i < m_PolygonPoints.length(); i++)
                            {
                                r->AddVertex(m_PolygonPoints.at(i).x,m_PolygonPoints.at(i).y);
                            }
                            p->SetExtRing(r);
                            if(m_PolygonFill != nullptr)
                            {
                                m_PolygonFill->Destroy();
                                delete m_PolygonFill;
                                m_PolygonFill = nullptr;
                            }
                            m_PolygonFill = p->Triangulate();
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                            for(int i = 0; i < m_PolygonFill->GetPolygonCount(); i++)
                            {
                                LSMPolygon * p = m_PolygonFill->GetPolygon(i);
                                if(p->GetExtRing()->GetVertexCount() > 2)
                                {
                                    double * data = p->GetExtRing()->GetVertexData();
                                    m_PolygonFillX.push_back(data[0]);
                                    m_PolygonFillY.push_back(data[1]);
                                    m_PolygonFillX.push_back(data[2]);
                                    m_PolygonFillY.push_back(data[3]);
                                    m_PolygonFillX.push_back(data[4]);
                                    m_PolygonFillY.push_back(data[5]);
                                }
                            }
                        }else
                        {
                            m_PolygonFillX.clear();
                            m_PolygonFillY.clear();
                        }

                    }else
                    {
                        m_PolygonPoints.clear();
                        m_PolygonFillX.clear();
                        m_PolygonFillY.clear();
                        Polygonpx.clear();
                        Polygonpy.clear();

                        if(m_PolygonFill != nullptr)
                        {
                            m_PolygonFill->Destroy();
                            delete m_PolygonFill;
                            m_PolygonFill = nullptr;
                        }
                    }
                }
            }

            if(edit_type_string.compare("Add/Remove Shape") == 0)
            {
                if(edit_shapetype_string.compare("Circle (Outline)") == 0 ||edit_shapetype_string.compare("Circle") == 0 )
                {
                    m_HasCircle = false;
                }
            }
            if( (edit_type_string == "Add/Remove Shape") )
            {
                if(edit_shapetype_string.compare("Rectangle") == 0 ||edit_shapetype_string.compare("Rectangle (Outline)") == 0 )
                {
                    m_HasSquare = false;
                }
            }



            m_HasDoneRightMouseClick = true;

        }
    }


    void OnFrame(float dt, GLFWwindow*w)
    {

    }

    void OnSave() override;

    void OnSaveAs(QString path) override;

    inline void OnClose();

    inline void OnAttributeEditStopped()
    {
        m_DialogRemovedFeatures.clear();
    }

    inline MatrixTable * GetAttributes() override
    {
        std::cout <<"get attributes " << std::endl;
        if(m_Shapes != nullptr)
        {
            if(m_Shapes->GetLayerCount() > 0)
            {
                return m_Shapes->GetLayer(0)->GetAttributeTable();
            }
        }

        return nullptr;
    }

    void SetAttributes(MatrixTable * t) override;

    inline QList<QString> GetAttributeRowCallbacks() override
    {

        QList<QString> ret;
        ret.append("View");
        ret.append("Remove");
        ret.append("Select");

        return ret;

    }

    inline void OnAttributeRowCallBack(QString action, QList<int> rows, QList<QString> rowtitles) override
    {
        //get list of relevant features
        if(m_Shapes != nullptr)
        {
            if(m_Shapes->GetLayerCount() > 0)
            {

                ShapeLayer * l = m_Shapes->GetLayer(0);
                QList<Feature *> features;

                for(int i = 0; i < rowtitles.length(); i++)
                {
                    bool ok = true;
                    int index = rowtitles.at(i).toInt(&ok)-1;
                    std::cout << "selected feature " << i << " " << index << " " << ok << " " << rowtitles.at(i).toStdString() <<  std::endl;
                    if(ok)
                    {
                        if(index > -1 && index < l->GetFeatureCount())
                        {
                            features.append(l->GetFeature(index));
                        }
                    }
                }

                if(action == "View")
                {
                    BoundingBox b;
                    bool set = false;

                    for(int i = 0; i <features.length(); i++)
                    {
                        if(features.at(i)->GetShapeCount() > 0)
                        {
                            if(!set)
                            {
                                b =features.at(i)->GetBoundingBox();
                                set = true;
                            }else
                            {
                                b.MergeWith(features.at(i)->GetBoundingBox());
                            }
                        }
                    }
                    if(m_CallBackViewSet)
                    {
                        m_CallBackView(b);
                    }

                }else if(action == "Remove")
                {


                    for(int i = 0; i <features.length(); i++)
                    {
                        if(!m_DialogRemovedFeatures.contains(features.at(i)))
                        {
                            m_DialogRemovedFeatures.append(features.at(i));
                        }
                    }

                }else if(action == "Select")
                {
                    ClearSelected();

                    for(int i = 0; i <features.length(); i++)
                    {
                        m_SelectedFeatures.append(features.at(i));
                    }



                }
            }
        }

    }


    UILayerEditor* GetEditor();



    int m_DialogReturn = 0;
    MatrixTable * m_DialogResult = nullptr;
    MatrixTable * DoTableDialog(OpenGLCLManager * m, MatrixTable * t);

    inline int GetResultingMatrixTableCode()
    {
        return m_DialogReturn;
    }

};
