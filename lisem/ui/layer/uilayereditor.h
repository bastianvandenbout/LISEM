#pragma once

#include "layer/uilayer.h"
#include "QString"
#include "raster/rasterpainting.h"
#include "layer/geo/uigeolayer.h"
#include "geo/shapeediting.h"
#include "functional"

#define UI_PARAMETER_TYPE_GROUP -1
#define UI_PARAMETER_TYPE_LABEL 0
#define UI_PARAMETER_TYPE_FLOAT 1
#define UI_PARAMETER_TYPE_DOUBLE 2
#define UI_PARAMETER_TYPE_BOOL 3
#define UI_PARAMETER_TYPE_FILE 4
#define UI_PARAMETER_TYPE_STRING 5
#define UI_PARAMETER_TYPE_DIR 6
#define UI_PARAMETER_TYPE_RADIO 7
#define UI_PARAMETER_TYPE_COLOR 8
#define UI_PARAMETER_TYPE_BUTTON 9
#define UI_PARAMETER_TYPE_STYLE 10

typedef struct UIParameter
{
    int Type;
    QString DefaultValue;
    QString Value;
    QString Name;
    QString Description;
    double min = 0.0;
    double max = 0.0;

}UIParameter;

class UIGeoLayer;
class UILayer;

class UILayerEditor
{

protected:


    UILayer * m_Layer;
    UIGeoLayer * m_GLayer;

    bool m_IsChangedSinceSave;

    QMutex m_ScriptMutex;
    bool m_IsScript = false;
    bool m_HasChangedSinceLastScriptCheck = false;


    GeoCoordTransformer * m_Transformer = nullptr;
    GeoCoordTransformer * m_Transformerinv = nullptr;

    QList<ShapeLayerChange> m_Edits;

    QList<UIParameter> m_ParameterList;

    bool m_DoRemove = false;
    bool m_IsPrepared = false;
    bool m_Exists = false;
    bool m_Draw = true;
    bool m_IsChanged = false;
    bool m_IsGeo = false;

    QString m_FilePath;

public:
    UILayerEditor(UILayer * ml);

    inline UILayer * GetLayer()
    {
        return m_Layer;
    }
    inline bool IsPrepared()
    {
       return m_IsPrepared;
    }

    inline bool IsChangedSinceLastScriptCheck()
    {
        bool value;
        m_ScriptMutex.lock();
        value = m_HasChangedSinceLastScriptCheck;
        m_HasChangedSinceLastScriptCheck = false;
        m_ScriptMutex.unlock();
        return value;
    }

    inline virtual void FillMapWithCurrentEdit(cTMap * m)
    {
    }



    bool m_CallBackViewSet = false;
    std::function<void(BoundingBox)> m_CallBackView;

    template<typename _Callable1, typename... _Args1>
    inline void AddViewCallBack( _Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackViewSet = true;
        m_CallBackView = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }


    inline void SetDraw(bool x)
    {
        m_Draw = x;
    }
    inline void SetRemove()
    {
        m_DoRemove = true;
    }
    inline bool Exists()
    {
        return m_Exists;
    }
    inline bool IsDraw()
    {
        return m_Draw;
    }
    inline bool ShouldBeRemoved()
    {
        return m_DoRemove;
    }

    inline void SetResponsive(bool r)
    {

    }

    inline virtual QList<QString> GetAttributeRowCallbacks()
    {


        return {};
    }

    inline virtual void OnAttributeRowCallBack(QString action, QList<int> rows, QList<QString> rowt)
    {

    }

    inline virtual MatrixTable * GetAttributes()
    {
        return nullptr;
    }

    inline virtual void SetAttributes(MatrixTable * t)
    {

    }



    inline void AddParameter(int type, QString name, QString description, QString value, double min = 0.0, double max = 0.0)
    {
        UIParameter p;
        p.Type = type;
        p.Name = name;
        p.Description = description;
        p.Value = value;
        p.DefaultValue = value;
        p.max = max;
        p.min = min;

        m_ParameterList.append(p);
    }

    inline QList<UIParameter> GetParameters()
    {
        return m_ParameterList;
    }

    inline double GetParameterValueDouble(QString name)
    {
        for(int i = 0;i < m_ParameterList.length(); i++)
        {
            if(m_ParameterList.at(i).Name.compare(name) == 0)
            {
                return m_ParameterList.at(i).Value.toDouble();
            }
        }

        return 0.0;
    }
    inline QString GetParameterValue(QString name)
    {
        for(int i = 0;i < m_ParameterList.length(); i++)
        {
            if(m_ParameterList.at(i).Name.compare(name) == 0)
            {
                return m_ParameterList.at(i).Value;
            }
        }

        return "";
    }
    inline virtual void OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions)
    {

    }

    inline void ChangeParameter(QString name, QString value, std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions)
    {
        for(int i = 0;i < m_ParameterList.length(); i++)
        {
            UIParameter p = m_ParameterList.at(i);
            if(p.Name.compare(name) == 0)
            {
                p.Value = value;
                m_ParameterList.replace(i,p);
                break;
            }
        }

        OnParameterChanged(name,value,fset,foptions);
    }

    virtual void OnPrepare(OpenGLCLManager * m)
    {

    }
    virtual void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm);

    virtual void OnDraw(OpenGLCLManager * m,GeoWindowState s) = 0;
    virtual void OnDestroy(OpenGLCLManager * m) = 0;
    virtual void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) = 0;
    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    };

    inline virtual void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }
    inline virtual void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }
    inline virtual void OnDraw3DGeoPost(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }
    inline virtual void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }


    inline virtual void OnFrame(float dt, GLFWwindow*w)
    {

    }

    inline virtual void OnMouseDrag(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline virtual void OnMouseMove(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline virtual void OnMouseScroll(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline virtual void OnMousePressed(int button, int pressed)
    {

    }

    inline virtual void OnMouseLeave(bool leave)
    {

    }

    inline virtual void OnKeyPressed(int key, int action, int modifier)
    {
    }

    inline virtual void OnGeoMousePressed(int button, int action, GeoWindowState s, LSMVector3 pos)
    {

    }

    inline virtual void OnGeoMouseMove(GeoWindowState s, LSMVector3 pos, LSMVector3 mov, bool drag)
    {

    }

    inline virtual void OnAttributeEditStopped()
    {

    }

    inline virtual QString GetCurrentFilePath()
    {
        return m_FilePath;
    }

    inline virtual bool IsChanged()
    {
        return m_IsChanged;
    }

    inline virtual void OnSave()
    {


        m_IsChanged = false;
    }

    inline virtual void OnSaveAs(QString path)
    {

        m_IsChanged = false;
    }


    inline virtual void OnClose()
    {


    }



};
