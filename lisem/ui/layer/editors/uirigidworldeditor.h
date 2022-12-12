#ifndef UIRIGIDWORLDEDITOR_H
#define UIRIGIDWORLDEDITOR_H


#include "layer/uilayereditor.h"
#include "layer/geo/uirigidworldlayer.h"

class UIRigidWorldLayer;

class UIRigidWorldLayerEditor : public UILayerEditor
{


private:

    UIRigidWorldLayer * m_RigidWorldLayer = nullptr;
    UIRigidWorldLayer * m_RigidWorldLayerOriginal = nullptr;

    ModelGeometry* m_CurrentOptionalModel;
    gl3dObject * m_CurrentOptionalActor;

    QMutex m_ScriptMutex;

    bool m_StartedDragging = false;
    bool m_Dragging = false;
    bool m_HasDoneDelete = false;
    bool m_HasDoneEnter = false;
    bool m_HasDoneCtrlZ = false;
    bool m_HasDoneMouseClick = false;
    bool m_HasDoneRightMouseClick = false;
    bool m_HasDoneMouseDoubleClick = false;
    bool m_HasDoneEscape = false;
    bool m_AltPressed=  false;
public:
    UIRigidWorldLayerEditor(UILayer * rl);


    void OnPrepare(OpenGLCLManager *m);
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);
    void OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions)
    {
        m_Dragging = false;
        if(name.compare("Shape") == 0 || name.compare("Type") == 0)
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

                //ClearSelected();
                //ClearCurrentEditShape();
            }
        }


    }
    void OnDraw(OpenGLCLManager * m,GeoWindowState s);
    void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);
    void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);

    inline void OnDestroy(OpenGLCLManager * m)
    {

    }

    inline void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline void OnDraw3DGeoPost(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline void OnFrame(float dt, GLFWwindow*w)
    {

    }

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

    inline void OnMouseMove(LSMVector2 pos, LSMVector2 mov) override
    {

    }

    inline void OnMouseScroll(LSMVector2 pos, LSMVector2 mov) override
    {

    }

    void OnMousePressed(int button, int pressed) override;

    void OnMouse3DPressed(int button, int pressed, GeoWindowState s,LSMVector2 ppos, LSMVector3 Pos, LSMVector3 Dir) override;
    inline void OnMouseLeave(bool leave) override
    {

    }

    inline void OnGeoMousePressed(int button, int action,GeoWindowState s, LSMVector3 pos) override
    {


    }


    inline void OnSave() override
    {


    }

    inline void OnSaveAs(QString path) override
    {

    }

    inline MatrixTable * GetAttributes() override
    {
        MatrixTable * T = new MatrixTable();

        return T;
    }


    inline void SetAttributes(MatrixTable * t) override
    {

        //see of we can get back values from the table, and if they are equal to the original values




        m_IsChanged = true;
    }

    inline void OnClose()
    {

    }

};



#endif // UIRIGIDWORLDEDITOR_H
