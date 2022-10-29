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


public:
    UIRigidWorldLayerEditor(UILayer * rl);


    void OnPrepare(OpenGLCLManager *m);
    void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);
    void OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions) override;
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
