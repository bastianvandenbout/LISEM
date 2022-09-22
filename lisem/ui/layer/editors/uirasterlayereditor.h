#pragma once

#include "layer/uilayereditor.h"
#include "layer/geo/uirasterstreamlayer.h"
#include "geometry/multipolygon.h"
#include "geometry/polygon.h"
#include "iogdal.h"

class UIStreamRasterLayer;



class UIRasterLayerEditor : public UILayerEditor
{


private:


    UIStreamRasterLayer * m_RasterLayer;
    QList<cTMap *> m_OriginalMap;
    QList<cTMap *> m_Map;
    cTMap * m_RMap = nullptr;
    cTMap * m_CMap = nullptr;
    QList<cTMap *> m_TMap;
    bool m_Dragging = false;
    LSMVector3 m_DragOrigin;
    bool m_HasSquare = false;
    QList<LSMVector2> m_PolygonPoints;
    LSMMultiPolygon *m_PolygonFill = nullptr;
    std::vector<float> Polygonpx;
    std::vector<float> Polygonpy;
    std::vector<float> m_PolygonFillX;
    std::vector<float> m_PolygonFillY;

    QList<RasterChangeList> m_Edits;
    QList<int> m_EditSimultaneity;

    bool m_HasDoneEnter = false;
    bool m_HasDoneCtrlZ = false;
    bool m_HasDoneCtrlF = false;
    bool m_HasDoneMouseClick = false;
    bool m_HasDoneMouseDoubleClick = false;
    bool m_HasDoneEscape = false;

public:

    UIRasterLayerEditor(UILayer * rl);



    void OnPrepare(OpenGLCLManager * m)
    {
        m_IsPrepared = true;
    }
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    inline void FillMapWithCurrentEdit(cTMap * m)
    {
        m_ScriptMutex.lock();

        for(int r = 0; r < m->nrRows(); r++)
        {
            for(int c = 0; c < m->nrCols(); c++)
            {
                //if(!pcr::isMV(m_Map->data[r][c]))
                {
                    //std::cout << m << " " << m_Map.size() << " " << m_Map.at(0) << " "<< r<< " " << c << std::endl;
                    //std::cout << "cal1 " << m->data[r][c] << std::endl;
                    //std::cout << "cal2 " << m_Map.at(0)->data[r][c] << std::endl;
                    //std::cout << "check " << std::endl;
                    m->data[r][c] = m_Map.at(0)->data[r][c];
                }

            }
        }
        m_ScriptMutex.unlock();
    }




    inline void OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions) override
    {
        m_Dragging = false;
        if(name.compare("Shape") == 0)
        {

            m_HasDoneEscape = false;
            m_HasDoneEnter = false;
            m_HasDoneCtrlZ = false;
            m_HasDoneCtrlF = false;
            m_HasDoneMouseClick = false;
            m_HasDoneMouseDoubleClick = false;
        }

        if(name== "Type")
        {
            if(value == "Drag")
            {
                std::cout << "reset values"<< std::endl;
                for(int r = 0; r < m_Map.at(0)->nrRows(); r++)
                {
                    for(int c = 0; c < m_Map.at(0)->nrCols(); c++)
                    {
                        m_RMap->data[r][c] = r;
                        m_CMap->data[r][c] = c;
                        for(int i = 0; i < m_Map.length(); i++)
                        {

                            m_TMap.at(i)->data[r][c] = m_Map.at(i)->data[r][c];
                        }
                    }
                }

            }else
            {

            }
        }

    }


    void OnDraw(OpenGLCLManager * m,GeoWindowState s);

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


    void OnFrame(float dt, GLFWwindow*w)
    {



    }

    inline void OnKeyPressed(int button, int action, int modifiers) override
    {

        if(modifiers & GLFW_MOD_CONTROL)
        {

            if(button == GLFW_KEY_Z && action == GLFW_PRESS)
            {
                m_HasDoneCtrlZ = true;
            }
        }

        if(modifiers & GLFW_MOD_CONTROL)
        {

            if(button == GLFW_KEY_F && action == GLFW_PRESS)
            {
                m_HasDoneCtrlF = true;
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

    }

    inline void OnGeoMousePressed(int button, int action,GeoWindowState s, LSMVector3 pos) override
    {

        QString edit_shapetype_string = GetParameterValue("Shape");

        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            if(m_Dragging == false)
            {
                m_Dragging = true;

                if(edit_shapetype_string.compare("Rectangle") == 0 )
                {
                    m_HasSquare = true;
                }
                m_DragOrigin= pos;

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
            }

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




            m_Dragging = false;
        }

        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {

            if(edit_shapetype_string.compare("Rectangle(drag)") == 0 )
            {
                m_HasSquare = false;
            }

            if(edit_shapetype_string.compare("Polygon") == 0 ||edit_shapetype_string.compare("Lines") == 0  )
            {
                if(m_PolygonPoints.length() > 0)
                {
                    m_PolygonPoints.removeLast();
                }
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


    inline void OnSave() override
    {

        //save raster using known file path

        bool saved = false;
        try
        {
            if(m_Map.length() == 1)
            {

                AS_SaveMapToFileAbsolute(m_Map.at(0),GetCurrentFilePath());
            }else
            {
                AS_SaveMapQListToFileAbsolute(m_Map,GetCurrentFilePath());

            }
            saved = true;
        }
        catch(...)
                {

            //handle error message

                }

        if(saved)
        {

            for(int r = 0; r < m_Map.at(0)->nrRows(); r++)
            {
                for(int c = 0; c < m_Map.at(0)->nrCols(); c++)
                {
                    for(int i = 0; i < m_Map.length(); i++)
                    {

                        m_OriginalMap.at(0)->Drc = m_Map.at(0)->Drc;
                    }
                }
            }

            m_IsChanged = false;
        }
    }

    inline void OnSaveAs(QString path) override
    {
        //save as using the provided path

        bool saved = false;
        try
        {
            if(m_Map.length() == 1)
            {
                AS_SaveMapToFileAbsolute(m_Map.at(0),path);
            }else
            {
                AS_SaveMapQListToFileAbsolute(m_Map,path);
            }
        }catch(...)
        {

            //handle error message


        }

        saved = true;

        if(saved)
        {
            m_Layer->SetFilePath(path);
            m_FilePath = m_Layer->GetFilePath();
            for(int r = 0; r < m_Map.at(0)->nrRows(); r++)
            {
                for(int c = 0; c < m_Map.at(0)->nrCols(); c++)
                {
                    for(int i = 0; i < m_Map.length(); i++)
                    {
                        m_OriginalMap.at(i)->Drc = m_Map.at(i)->Drc;
                    }
                }
            }

            m_IsChanged = false;
        }

    }

    inline MatrixTable * GetAttributes() override
    {
        MatrixTable * T = new MatrixTable();


        //allow editing of North, West,
        T->SetSize(4,2);

        T->SetValue(0,0,QString("North"));
        T->SetValue(1,0,QString("West"));
        T->SetValue(2,0,QString("CellSizeY"));
        T->SetValue(3,0,QString("CellSizeY"));

        T->SetValue(0,1,m_Map.at(0)->north());
        T->SetValue(1,1,m_Map.at(0)->west());
        T->SetValue(2,1,m_Map.at(0)->cellSizeX());
        T->SetValue(3,1,m_Map.at(0)->cellSizeY());

        return T;
    }

    inline void SetAttributes(MatrixTable * t) override
    {

        //see of we can get back values from the table, and if they are equal to the original values




        m_IsChanged = true;
    }


    inline void OnClose();

};
