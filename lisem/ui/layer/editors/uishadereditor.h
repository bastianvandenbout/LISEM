#ifndef UISHADEREDITOR_H
#define UISHADEREDITOR_H

#include "layer/uilayereditor.h"
#include "layer/geo/uishaderlayer.h"
#include "iogdal.h"


class UIShaderLayer;



class UIShaderLayerEditor : public UILayerEditor
{

public:
    UIShaderLayer* m_ShaderLayer = nullptr;


    UIShaderLayerEditor(UILayer * l);


    void OnUpdate(std::vector<QString> shaders,std::vector<std::vector<QString>> files,std::vector<std::vector<cTMap *>> maps);
    void OnRun();
    void OnPause();
    void OnCoordinates(BoundingBox b, bool abs, bool is_2d,GeoProjection p);


    bool m_CallBackCompileErrorSet = false;
    std::function<void(std::vector<int>,std::vector<std::vector<int>>,std::vector<std::vector<QString>>)> m_CallBackCompileError;
    template<typename _Callable1, typename... _Args1>
    inline void SetCompileErrorCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackCompileErrorSet = true;
        m_CallBackCompileError = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    }

    inline void OnCompilerError(std::vector<int> p, std::vector<std::vector<int>> l,std::vector<std::vector<QString>> m)
    {
        if(m_CallBackCompileErrorSet)
        {
            m_CallBackCompileError(p,l,m);
        }
    }

    inline virtual void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    inline virtual void OnDraw(OpenGLCLManager * m,GeoWindowState s)
    {

    }
    inline virtual void OnDestroy(OpenGLCLManager * m)
    {

    }
    inline virtual void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }

};
#endif // UISHADEREDITOR_H
