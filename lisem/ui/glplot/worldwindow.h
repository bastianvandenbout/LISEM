#ifndef WORLD2DWINDOW_H
#define WORLD2DWINDOW_H
#include "model.h"

#include "defines.h"
#include "openglclmanager.h"
#include "qcolor.h"
#include "model.h"
#include "geo/raster/map.h"
#include "openglcltexture.h"
#include "openglprogram.h"
#include "lsmio.h"
#include "lisemmath.h"
#include "QObject"
#include "openglcldatabuffer.h"
#include "color/colorf.h"
#include "layer/geo/uigeolayer.h"
#include "layer/geo/uirasterlayer.h"
#include "layer/geo/uivectorlayer.h"
#include "layer/geo/uipointcloudlayer.h"
#include "layer/geo/uimodelrasterlayer.h"
#include "layer/geo/uigeolayer.h"
#include "layer/ui/uimapedge.h"
#include "glplot/worldgltransforms.h"
#include "layer/geo/uiwebtilelayer.h"
#include "layer/geo/uimodelpointlayer.h"
#include "layer/uilayereditor.h"
#include "glplot/gl3dcamera.h"
#include "gl3dcameracontroller.h"
#include "gl2dcamera.h"
#include "gl2dcameracontroller.h"
#include "QTime"
#include "QList"
#include "layer/3d/gl3dskybox.h"
#include "layer/3d/gl3dterrain.h"
#include "openglmsaarendertarget.h"
#include "gl3delevationprovider.h"
#include "QList"
#include "layer/layerregistry.h"
#include "layer/post/fxaa.h"
#include "layer/post/dof.h"
#include "layer/post/ssao.h"
#include "layer/post/ssr.h"
#include "layer/uilight.h"
#include "layer/light/uidirectionallight.h"
#include "layer/post/postdebug.h"
#include "opengl3dobject.h"

typedef struct World2DMouseState
{
    float scrollold = 0.0f;
    float scroll = 0.0f;
    bool dragging = false;
    bool in = true;

    float Drag_x = 0.0f;
    float Drag_y = 0.0f;
    float PosOld_x = 0.0f;
    float PosOld_y = 0.0f;
    float PosDO_x = 0.0f;
    float PosDO_y = 0.0f;
    float PosDOO_x = 0.0f;
    float PosDOO_y = 0.0f;
    float Pos_x = 0.0f;
    float Pos_y = 0.0f;

    bool move_first = true;
    float Move_X = 0.0f;
    float Move_Y = 0.0f;

    float Scroll_x = 0.0f;
    float Scroll_y = 0.0f;
    float ScrollOld_x = 0.0f;
    float ScrollOld_y = 0.0f;

    bool movescroll_first = true;
    float MoveScroll_X = 0.0f;
    float MoveScroll_Y = 0.0f;

    QList<bool> KeyEventShift;
    QList<int> KeyEvents;
    QList<int> KeyAction;
    QList<int> KeyMods;

    QList<bool> MouseButtonEventsShift;
    QList<int> MouseButtonEvents;
    QList<int> MouseButtonKeyAction;

} World2DMouseState;


//probe the world to find values
typedef struct WorldGeoProbeResult
{
    QList<UILayer * > Layers;
    QList<QString> Values;

}  WorldGeoProbeResult;


class LISEM_API WorldWindow : public QObject, public GLListener
{
    Q_OBJECT;

public:


private:
    //new
    //layers

    QList<UILayer *> m_UILayerList;
    QList<UILight *> m_UILightList;

    UILayer * m_UIFocusLayer = nullptr;

    //internal signaling that
    bool m_UILayersChanged;
    bool m_CRSChanged =false;
    //
    GeoWindowState m_CurrentWindowState;
    GeoWindowState m_CurrentDrawWindowState;

    QMutex MouseStateMutex;
    World2DMouseState m_MouseState;

    //general draw settings:
    bool m_DrawUI = true;
    bool m_DrawLines = false;
    double m_DrawUIScale = 1.0;
    bool m_DrawLegends = true;
    bool m_Draw3D = false;
    bool m_DrawArrows = false;
    bool m_Draw3DGlobe = false;
    bool m_DrawShadows = false;
    bool m_DoSet3DViewFrom2DOnce =false;

    ModelGeometry* m_ArrowModel;
    gl3dObject * m_ArrowActor;

    LSMVector3 m_SunDir = LSMVector3(0.0,0.4,1.0);//default solar position
    bool m_SunDrag = false;
    GL3DCamera * m_Camera3D;
    GL3DCameraController *m_Camera3DController;
    GL2DCamera * m_Camera2D;
    GL2DCameraController *m_Camera2DController;

    GL3DElevationProvider * m_ElevationProvider;

    QElapsedTimer m_time;
    qint64 m_timeold;
    double m_GLDT;
    double m_GLT = 0;
    bool dt_first = true;
    bool draw_first = true;

    int m_uidcount = 1;

    LSMVector3 m_Prev_GeoLoc;

    QList<OpenGLCLMSAARenderTarget *> m_2D3DRenderCTargets;
    QList<OpenGLCLMSAARenderTarget *> m_2D3DRenderDTargets;
    QList<float> m_2D3DRenderTargetScales;
    QList<BoundingBox> m_2D3DRenderTargetBoundingBox;
    QList<WorldGLTransformManager  *> m_2D3DTransformManager;

    OpenGLCLMSAARenderTarget * m_TDRenderTarget;

    OpenGLCLMSAARenderTarget * m_3DRenderTarget;
    OpenGLCLMSAARenderTarget * m_Post1RenderTarget;
    OpenGLCLMSAARenderTarget * m_Post2RenderTarget;
    OpenGLCLMSAARenderTarget * m_3DLayerRenderTargetC;
    OpenGLCLMSAARenderTarget * m_3DLayerRenderTargetD;


    cTMap * m_3DScreenPosX = nullptr;
    cTMap * m_3DScreenPosY = nullptr;
    cTMap * m_3DScreenPosZ = nullptr;

    UILayerEditor *m_LayerEditor = nullptr;

    QMutex m_FocusMutex;
    QList<LSMVector2> m_FocusLocations;
    bool m_DraggingFocusSquare = false;
    LSMVector2 m_FocusSquareStart;
    QList<BoundingBox> m_FocusSquare;

public:
    QMutex m_UILayerMutex;
    QMutex m_CRSMutex;
    //default drawing programs


    WorldGLTransformManager * m_TransformManager;
    OpenGLCLManager * m_OpenGLCLManager;
    LISEMModel * m_Model;
    QString m_KernelDir;

    OpenGLCLMSAARenderTarget * External3DRenderTarget = nullptr;
    OpenGLCLMSAARenderTarget * ExternalTarget = nullptr;
    OpenGLCLMSAARenderTarget * ExternalPost1RenderTarget = nullptr;
    OpenGLCLMSAARenderTarget * ExternalPost2RenderTarget = nullptr;


    int m_InternalCharBufferSize = 0;
    uchar *m_InternalCharBuffer = nullptr;


    GeoProjection m_CurrentProjection;

    inline WorldWindow(OpenGLCLManager * glclm) : QObject()
    {
        m_OpenGLCLManager = glclm;
        m_OpenGLCLManager->AddListener(this);


        m_ElevationProvider = new GL3DElevationProvider();

        m_TransformManager = new WorldGLTransformManager(glclm);
        AddUILayer(new UIMapEdgeLayer());
        //AddUILayer(new UISkyBoxLayer());
        AddUILayer(new UITerrainLayer());
        AddUILayer(new UIPostFXAALayer());
        AddUILayer(new UIPostDebugLayer());
        AddLight(new UIDirectionalLight());

        GeoProjection p;
        p.SetGeneric();
        SetCurrentProjection(p,true);


        m_Camera3D = new GL3DCamera();
        m_Camera3DController = new GL3DCameraController(m_Camera3D,m_ElevationProvider);
        m_Camera2D = new GL2DCamera();
        m_Camera2DController = new GL2DCameraController(m_Camera2D);

        RegisterLayerTypes();

        m_time.start();
        m_timeold = m_time.nsecsElapsed();
        m_GLDT = 0.0f;

        connect(this,&WorldWindow::int_emit_fildropcallback,this,&WorldWindow::int_emit_filedropcallbackslot,Qt::ConnectionType::QueuedConnection);

    }

    void SetDraw3D(bool d);
    void SetDraw3DGlobe(bool d);
    void SetUIDraw(bool d);
    void SetLinesDraw(bool d);
    void SetUIScale(double d);
    void SetShadowDraw(bool d);
    void SetLegendDraw(bool d);
    GeoProjection GetCurrentProjection();
    void SetCurrentProjection(GeoProjection p, bool forceupdate = false);
    GeoWindowState GetCurrentWindowState();

    QImage *DoExternalDraw(GeoWindowState s);
    uchar * DoExternalDrawToUChar(GeoWindowState s);

    void SignalClose();
    void SetModel( LISEMModel * m);


    void AddOceanLayer();
    void AddSkyBoxLayer();
    void AddCloudLayer();

    void AddLight(UILight * L);


    int AddUILayerAt(UILayer *ML, bool emitsignal = true, bool do_zoom = true, int i = 0);
    int AddUILayer(UILayer *ML, bool emitsignal = true, bool do_zoom = true);
    UILayer * GetUILayerFromName(QString name);
    UILayer * GetUILayerFromUID(int id);
    UILayer * GetUILayerFromID(int id);
    UILayer * GetUILayerFromFile(QString path);
    UILayer * GetUIStreamLayerFromFile(QString path);
    UILayer * GetUIDuoLayerFromFile(QList<QString> path);
    UILayer * GetUITimeSeriesLayerFromFile(QList<QString> path);
    UILayer * GetUILayerFromMap(cTMap * map, QString name = "", bool native = false);
    UILayer * GetUIVectorLayerFromFile(QString path);
    UILayer * GetUIFieldLayerFromFile(QString path);
    UILayer * GetUIPointCloudLayerFromFile(QString path);
    UILayer * GetUIObjectLayerFromFile(QString path);

    UILayer * GetUILayerFromShapeFile(ShapeFile * map, QString name = "", bool native = false);
    UILayer * GetGoogleLayer();
    UILayer * GetGoogleDEMLayer();
    void RemoveUILayer(UILayer *ML, bool emitsignal = true);
    void ReplaceUILayer(UILayer *ML, UILayer *ML2);
    void SetUILayerAs(UILayer *ML,  bool emitsignal = false);
    QList<UILayer *> GetUILayers();
    void SetLayerOrder(QList<UILayer *> newlist, bool emitsignal = false, bool is_locked = false);
    int GetUILayerCount();
    UILayer* GetUILayer(int i);
    BoundingBox AddNativeUILayers(int channel = 0, LISEMModel * m = nullptr);
    void RemoveNativeUILayers(int channel = 0);

    inline void SetFocusLayer(UILayer *fl)
    {
        m_UILayerMutex.lock();

        m_UIFocusLayer = fl;

        m_UILayerMutex.unlock();
    }
    //control of the display geolocation
    BoundingBox GetLook();
    void LookAt(UIGeoLayer* geolayer);
    void LookAt(BoundingBox3D b);
    void LookAt(BoundingBox b, bool auto_3d = false);
    void LookAtbb(BoundingBox b);


    inline void SetEditor(UILayerEditor *e)
    {
        m_UILayerMutex.lock();

        //set view callback
        e->AddViewCallBack(&WorldWindow::LookAtbb,this);

        m_LayerEditor = e;
        m_UILayerMutex.unlock();

        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_FocusLocations.clear();
        m_FocusMutex.unlock();

        emit OnFocusLocationChanged();
    }

    inline bool IsEditsUnsaved()
    {
        m_UILayerMutex.lock();
        bool ret = m_LayerEditor != nullptr;
        m_UILayerMutex.unlock();
        return ret;
    }
    inline void RemoveEditor()
    {
        m_LayerEditor = nullptr;
    }

    inline void RemoveAndDeleteEditor()
    {
        if(m_LayerEditor != nullptr)
        {
            delete m_LayerEditor;
        }
        m_LayerEditor = nullptr;
    }


    inline QList<LSMVector2> GetFocusLocations()
    {
        QList<LSMVector2> temp;
        m_FocusMutex.lock();

        for(int i = 0; i < m_FocusLocations.length(); i++)
        {
            temp.append(m_FocusLocations.at(i));
        }
        m_FocusMutex.unlock();

        return temp;
    }




    //input listener functions as called by OpenGLCLManager
    //here we also call all the ui functions for the layers:
    //inline void OnMouseMove(LSMVector2 pos, LSMVector2 mov)
    //inline void OnMouseScroll(LSMVector2 pos, LSMVector2 mov)
    //inline void OnMousePressed(int button, int pressed)
    //inline void OnMouseLeave(bool leave)
    //inline void OnKeyPressed(int key, bool pressed)
    //inline void OnGeoMousePressed(int button, GeoWindowState s, LSMVector2 pos)
    //inline void OnGeoMouseMove(GeoWindowState s, LSMVector2 pos, LSMVector2 mov)

    //furthermore, we have to call the ui functions for the currently active editor

    void OnMouseScroll(double scrollx ,double scrolly) override;
    void OnMouseMove(double posx,double posy) override;
    void OnMouseInOut(bool enter) override;
    void OnMouseKey( int key, int action, int mods) override;
    void OnFrameBufferSize(int w,int h) override;
    void OnFileDrop(QString) override;
    void OnKey(int key, int action, int mods) override;




    std::function<bool(QString, int)> m_FileCallBack;
    bool m_FileCallBackSet = false;

    template<typename _Callable, typename... _Args>
    inline void SetCallBackFileOpened(_Callable&& __f, _Args&&... __args)
    {
        //now we store this function pointer and call it later when a file is openened
        m_FileCallBack = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1,std::placeholders::_2);
        m_FileCallBackSet = true;
    }

    void InputToLayers();
    /////
    /// \brief UpdateCurrentWindowState
    /// This function calculates, based on the MouseState
    /// the current window state (zoom, extent, location etc..)
    ///
    /// \return true if the user has altered the windowstate
    ///
    bool UpdateCurrentWindowState();
    bool UpdateCamera();
    bool UpdateTransformers();

    WorldGeoProbeResult ProbeAll();
    WorldGeoProbeResult ProbeLayer(UILayer * l);

    void Draw();


    void Draw3DArrows(GeoWindowState s, bool external = false);
    void Arrow3DRayCast();
    void Draw2DArrows(GeoWindowState s, bool external = false);
    void Arrow2DRayCast();

    void DrawToFrameBuffer2D(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawToFrameBuffer2DElevation(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void Draw2DPlotDataToBuffer(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawToFrameBuffer3D(GeoWindowState S, bool external);
    void DrawGlobeToBuffer(GeoWindowState S);
    void Draw3DPlotDataToBuffer(GeoWindowState S);
    void DrawBaseLayers3D(GeoWindowState S);
    void DrawColoredTerrain3D(GeoWindowState *S);
    void Draw3DObjects(GeoWindowState S);
    void Draw3DPost(GeoWindowState s);
    void Draw3DUI(GeoWindowState s);
    void DrawGeoLayers2DColor(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawGeoLayers2DElevation(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    void DrawUILayers(GeoWindowState S, WorldGLTransformManager * glt = nullptr);
    bool DrawPostProcess(GeoWindowState &s, WorldGLTransformManager * glt = nullptr);
    void CreateShadowMaps(GeoWindowState s, bool external);
    void CreateLightBuffer(GeoWindowState s, bool external);
    void DeferredLightPass(GeoWindowState s, bool external);

    //serialization

    UILayerTypeRegistry * m_LayerRegistry;

    void RegisterLayerTypes();
    void SaveTo(QJsonObject*);
    void LoadFrom(QJsonObject*, bool delete_old = true);

    GeoWindowState ExternalDrawGeoWindowState(int scr_w, int scr_h, BoundingBox b, bool ui, bool legends, bool no_async = true, float scale_mult = 1.0);

public slots:
    inline void int_emit_filedropcallbackslot(QString x)
    {
        if(m_FileCallBackSet)
        {
            m_FileCallBack(x,LISEM_FILE_TYPE_UNKNOWN);
        }
    }

signals:
    void OnMapsChanged();
    void OnFocusLocationChanged();

    void int_emit_fildropcallback(QString x);


};




#endif // WORLD2DWINDOW_H
