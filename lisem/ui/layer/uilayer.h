#pragma once

#include "styles/sphstyle.h"
#include "openglclmanager.h"
#include "glplot/worldgltransforms.h"
#include "geowindowstate.h"
#include "matrixtable.h"
#include "QMutex"
#include "layer/uilayereditor.h"
#include "layerparameters.h"
#include "geo/shapes/shapefile.h"
#include "boundingbox.h"

class UILayerEditor;
//class LayerParameters;

typedef struct LayerInfo
{
    QList<QString> m_Names;
    QList<QString> m_Value;
    QList<bool> m_CanEdit;


    inline void Add(QString m, QString v, bool can_edit = false)
    {
        m_Names.append(m);
        m_Value.append(v);
        m_CanEdit.append(m_CanEdit);
    }

    inline void Add(LayerInfo li)
    {
        m_Names.append(li.m_Names);
        m_Value.append(li.m_Value);
    }

    inline MatrixTable * GetMatrixTable()
    {
        MatrixTable * t = new MatrixTable();
        t->SetSize(m_Names.length(),2);
        for(int i= 0; i < m_Names.length(); i++)
        {
            t->SetValue(i,0,m_Names.at(i).toStdString());
            t->SetValue(i,1,m_Value.at(i).toStdString());
        }

        return t;
    }
} LayerInfo;

typedef struct LayerProbeResult
{
    bool hit;
    QList<QString> AttrNames;
    QList<QString> AttrValues;

} LayerProbeResult;






class UILayer : public UIDistanceEstimator
{
    //private members
protected:
    int m_Type;
    int m_ID=-1;
    int m_UID=-1;
    LSMStyle m_Style;
    LayerParameters *m_Parameters;
    QMutex *m_StyleMutex;
    QString m_Name;

    QMutex m_ScriptRefMutex;
    int m_Scriptref = 0;


    QString m_File;
    bool m_IsFile;

    bool m_ShadowCaster = false;
    bool m_Exists = true;
    bool m_Draw = true;
    bool m_IsDynamic = true;
    bool m_IsGeo = false;
    bool m_IsNative = false;
    bool m_IsModel = false;
    bool m_IsPrepared = false;
    bool m_DoRemove =false;
    bool m_DoReplace =false;
    UILayer * m_DoReplaceLayer = nullptr;
    UILayer * m_ReplacedLayer = nullptr;
    bool m_IsUser = true;
    bool m_RequiresCRSGLTransform = false;
    bool m_IsCRSChanged = false;
    bool m_HasLegend = false;
    bool m_Editable = false;
    bool m_Editing = false;
    bool m_NonRemovable = false;
    bool m_DrawLegend = true;
    bool m_CouldBeDEM = false;
    bool m_DrawAsDEM = false;
    bool m_DrawAsHillShade = false;
    bool m_IsSaveAble = false;
    bool m_IsLayerSaveAble = false;
    bool m_HasStyle = false;
    bool m_HasParameters = false;
    bool m_IsPostProcessDrawer = false;
    bool m_IsPostPostProcessDrawer = false;
    bool m_is3D = false;
    bool m_IsMovable = false;
    bool m_IsScaleable = false;
    bool m_IsRotateable = false;

    bool m_IsTimeSeriesAble = false;
    bool m_IsSpectraAble = false;
    bool m_IsProfileAble = false;

    float m_x = 0;
    float m_y = 0;
    float m_sizex = 0;
    float m_sizey = 0;

    int m_Channel = 0;
    bool m_IsRequired = false;

    //public functions
public:

    //constructor

    inline UILayer()
    {
        m_StyleMutex = new QMutex();
        m_Parameters = new LayerParameters();
    }

    inline UILayer(QString name, bool file = false, QString filepath = "", bool dynamic = false)
    {
        m_StyleMutex = new QMutex();
        m_Parameters = new LayerParameters();
        Initialize(name,file,filepath,dynamic);
    }

    inline void Initialize(QString name, bool file = false, QString filepath = "", bool dynamic = false)
    {
        m_IsLayerSaveAble = true;
        m_Name = name;
        m_IsFile = file;
        m_File = filepath;
        m_IsDynamic = dynamic;
    }

    inline ~UILayer()
    {
        delete m_StyleMutex;
        delete m_Parameters;

    }

    inline void SetChannel(int c)
    {
        m_Channel = c;

    }

    inline int GetChannel()
    {
        return m_Channel;
    }

    inline bool IsRequired()
    {
        return m_IsRequired;
    }

    inline bool Is3D()
    {
        return m_is3D;
    }

    inline bool IsShadowCaster()
    {
        return m_ShadowCaster;
    }

    inline bool IsMovable()
    {
        return m_IsMovable;
    }

    inline bool IsScaleAble()
    {
        return m_IsScaleable;
    }
    inline bool IsRotateAble()
    {
        return m_IsRotateable;
    }

    inline virtual void Scale(LSMVector3 s)
    {


    }

    inline virtual void Move(LSMVector3 m)
    {


    }
    inline virtual void Rotate(LSMVector3 r)
    {


    }
    //virtual methods implemented by other classes

    virtual QString layertypeName() = 0;

    //fundamental functions related to drawing
    //called by the windowmanager when an update is needed
    //either due to user interaction with the display or
    //another external event (window opening/resizing../changing CRS)
    //possibly on every frame when IsDynamic and IsDraw return true
    virtual void OnPrepare(OpenGLCLManager * m,GeoWindowState s) = 0;
    virtual void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) = 0;

    inline virtual bool RequiresDraw(GeoWindowState s)
    {
        return false;
    }

    virtual void OnDraw(OpenGLCLManager * m,GeoWindowState s) = 0;
    virtual void OnDestroy(OpenGLCLManager * m) = 0;
    virtual void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) = 0;
    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm){};
    inline virtual bool IsPostProcess()
    {
        return m_IsPostProcessDrawer;
    }
    inline virtual bool IsPostPostProcess()
    {
        return m_IsPostPostProcessDrawer;
    }
    //called before OnDraw3D and OnDraw3DGeo, so that transparant layers can render their depth to a special surface
    //later, only the closest transparent surface is drawn
    inline virtual void OnDraw3DTransparancyDepthMap(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnPreDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnPostDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnPostDraw3DUI(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnDraw3DShadowDepth(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, OpenGLCLMSAARenderTarget * target, LSMMatrix4x4 perspectiveview, BoundingBox bb, int level)
    {}

    inline virtual void OnDraw3DTransparentLayer(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline virtual void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline virtual void OnDrawPostPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline virtual void OnDraw3DPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}
    inline virtual void OnDraw3DPostPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {}

    inline virtual void OnDrawLightingFraction(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
    }
    inline virtual void OnDrawLightingBlockingGeometry(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s, float posy_start = 0.0, float posy_end = 1.0)
    {}

    inline virtual void OnFrame(float dt, GLFWwindow*w)
    {

    }

    //

    //updates the on-screen X, Y, SizeX and SizeY values
    virtual void UpdatePositionInfo(OpenGLCLManager * m) = 0;

    inline virtual void OnMouseMove(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline virtual void OnMouseScroll(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline virtual void OnMousePressed(int button, int pressed)
    {

    }

    inline virtual void OnMouseDrag(LSMVector2 pos, LSMVector2 mov)
    {

    }


    inline virtual void OnMouseLeave(bool leave)
    {

    }

    inline virtual void OnKeyPressed(int key, int action, int modifier)
    {

    }

    inline virtual void OnGeoMousePressed(int button, int action,GeoWindowState s, LSMVector3 pos)
    {

    }

    inline virtual void OnGeoMouseMove(GeoWindowState s, LSMVector3 pos, LSMVector3 mov, bool drag)
    {

    }


    //The boundaries of the object on the window
    //these values can not be simply calculated without knowledge of the current state of the OpenGLCLManager (CRS/zoom/translation/window size)
    //therefore a call to UpdatePositionInfo() must be done before getting these
    inline float GetX()
    {
        return m_x;
    }
    inline float GetY()
    {
        return m_y;
    }
    inline float GetSizeX()
    {
        return m_sizex;
    }
    inline float GetSizeY()
    {
        return m_sizey;
    }

    //Editor Related Stuff
    inline virtual UILayerEditor* GetEditor()
    {
        return nullptr;

    }



    //setters and getters for most general options for a layer
    inline bool IsEditAble()
    {
        return m_Editable;
    }

    inline virtual bool IsSaveAble()
    {
        return m_IsSaveAble;
    }
    inline virtual bool IsLayerSaveAble()
    {
        return m_IsLayerSaveAble;
    }


    inline virtual void SaveLayer(QJsonObject * obj)
    {

    }

    inline virtual void RestoreLayer(QJsonObject * obj)
    {

    }

    inline virtual bool SaveLayerToFile(QString filepath)
    {
        return false;
    }

    inline QString GetSaveDirHint()
    {
        if(m_IsFile && m_File.length() > 4)
        {
            QFileInfo f(m_File);
            return f.dir().path();
        }
        return "";
    }

    inline QString GetSaveNameHint()
    {
        if(m_IsFile && m_File.length() > 4)
        {
            QFileInfo f(m_File);
            return f.completeBaseName();

            /*QString end = m_File.mid(m_File.length() - 4,4);
            if(end.startsWith("."))
            {
                return m_File.mid(0,m_File.length -4);
            }else
            {
                return m_File;
            }*/
        }

        return "";
    }

    inline QList<QString> GetSaveExtHint()
    {
        QList<QString> ret;

        if(m_IsFile && m_File.length() > 4)
        {

            QString end = m_File.mid(m_File.length() - 4,4);
            if(end.startsWith("."))
            {
                ret.append(end.remove(0,1));
            }
        }
        return ret;
    }

    inline LayerParameters * GetParameters()
    {
        return m_Parameters;
    }


    inline bool IsBeingEdited()
    {
        return m_Editing;
    }

    inline void SetBeingEdited(bool x)
    {
        m_Editing = x;
    }

    inline bool IsNonRemovable()
    {
        return m_NonRemovable;
    }

    inline bool IsCRSChanged()
    {
        return m_IsCRSChanged;
    }
    inline bool Exists()
    {
        return m_Exists;
    }
    inline bool IsDraw()
    {
        return m_Draw;
    }
    inline bool IsDynamic()
    {
        return m_IsDynamic;
    }
    inline bool IsFile()
    {
        return m_IsFile;
    }
    inline QString GetFilePath()
    {
        return m_File;
    }
    inline void SetFilePath(QString f)
    {
        m_File =f;
    }
    inline QString GetName()
    {
        return m_Name;
    }

    inline bool HasParameters()
    {
        return m_Parameters->GetParameterCount() > 0;
    }
    inline int GetID()
    {
        return m_ID;
    }
    inline int GetUID()
    {
        return m_UID;
    }
    inline void SetID(int id)
    {
        m_ID = id;
    }
    inline void SetUID(int id)
    {
        m_UID = id;
    }
    inline bool IsGeo()
    {
        return m_IsGeo;
    }

    inline bool IsTimeSeriesAble()
    {
        return m_IsTimeSeriesAble;
    }

    inline bool IsProfileAble()
    {
        return m_IsProfileAble;
    }

    inline bool IsSpectraAble()
    {
        return m_IsSpectraAble;
    }

    inline bool IsNative()
    {
        return m_IsNative;
    }

    inline void SetRemove()
    {
        m_DoRemove = true;
    }

    inline void SetReplace(UILayer * other)
    {
        m_DoReplace = true;
        m_DoReplaceLayer = other;
    }

    inline UILayer * GetReplaceLayer()
    {
        return m_DoReplaceLayer;
    }

    inline void SetReplacedLayer(UILayer * other)
    {
         m_ReplacedLayer = other;
    }

    inline UILayer * GetReplacedLayer()
    {
        return m_ReplacedLayer;
    }


    inline bool ShouldBeReplaced()
    {
        return m_DoReplace;
    }

    inline bool ShouldBeRemoved()
    {
        return m_DoRemove;
    }

    inline bool IsPrepared()
    {
       return m_IsPrepared;
    }

    inline void SetDraw(bool x)
    {
        m_Draw = x;
    }
    inline bool IsUser()
    {
        return m_IsUser;
    }
    inline bool IsModel()
    {
        return m_IsModel;
    }

    inline bool CouldBeDEM()
    {
        return m_CouldBeDEM;
    }

    inline bool HasLegend()
    {
        return m_HasLegend;
    }

    inline virtual float MinimumLegendHeight(OpenGLCLManager * m, GeoWindowState s)
    {
        //as fraction of the vertical screen resolution

        return 0.1; //10 percent
    }

    inline virtual float MaximumLegendHeight(OpenGLCLManager * m, GeoWindowState s)
    {
        //as fraction of the vertical screen resolution

        return 1.0; //full height
    }

    inline bool DrawLegend()
    {
        return m_HasLegend && m_DrawLegend;
    }

    inline void SetDrawLegend(bool x)
    {
        m_DrawLegend = x;
    }

    inline void SetDrawAsDEM(bool dem)
    {
        m_Style.m_IsDEM = dem;
    }

    inline void SetDrawAsHillShade(bool hs)
    {
        m_Style.m_IsHS = hs;
        m_DrawAsHillShade = hs;
    }
    inline bool IsDrawAsDEM()
    {
        return m_Style.m_IsDEM;
    }

    inline bool IsDrawAsHillShade()
    {
        return m_DrawAsHillShade;
    }

    inline virtual int GetMapCount()
    {

        return 0;
    }

    inline virtual QList<cTMap *> GetMaps()
    {

        return QList<cTMap *>();
    }

    inline virtual QList<ShapeFile*> GetShapeFiles()
    {
        return QList<ShapeFile *>();

    }

    //style related functions

    virtual void SetStyle(LSMStyle s, bool OverWriteAll = false)
    {

        int n_bands = m_Style.m_Bands;

        if(OverWriteAll)
        {
            LSMStyle s_old = m_Style;
            m_Style = s;
            s.m_Bands = s_old.m_Bands;
            s.m_TimeMax = s_old.m_TimeMax;
            s.m_TimeMin = s_old.m_TimeMin;

        }else
        {
             m_Style.CopyFrom(s);
        }
        m_Style.m_Bands = n_bands;

    }

    inline QMutex * GetStyleMutex()
    {
        return m_StyleMutex;
    }
    inline LSMStyle *GetStyleRef()
    {
        return &m_Style;
    }

    inline LSMStyle GetStyle()
    {
        return m_Style;
    }

    inline virtual QList<QString> GetAllAttributes()
    {
        return QList<QString>();
    }
    inline virtual QList<QString> GetNumberAttributes()
    {
        return QList<QString>();
    }

    inline virtual double GetMinimumValue(int i)
    {
        return 0.0;
    }

    inline virtual double GetAverageValue(int i)
    {
        return 0.0;
    }

    inline virtual double GetMaximumValue(int i)
    {
        return 0.0;
    }

    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        return p;
    }

    inline virtual LayerProbeResult Probe3D(LSMVector3 Pos, LSMVector3 Dir, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        return p;
    }


    inline virtual MatrixTable * GetTimeSeries(QList<LSMVector2> Pos, GeoProjection proj)
    {
        MatrixTable * m = new MatrixTable();
        return m;
    }

    inline virtual MatrixTable * GetProfile(QList<LSMVector2> Pos, GeoProjection proj)
    {
        MatrixTable * m = new MatrixTable();
        return m;
    }

    inline virtual MatrixTable * GetSpectra(QList<LSMVector2> Pos, GeoProjection proj)
    {
        MatrixTable * m = new MatrixTable();
        return m;
    }



    inline virtual LayerInfo GetInfo()
    {
        return LayerInfo();
    }

    inline int GetScriptRefs()
    {

        std::cout << "scriptref " << this <<  " " << m_Scriptref << std::endl;
        return m_Scriptref;
    }


    inline void IncreaseScriptRef()
    {
        m_ScriptRefMutex.lock();
        m_Scriptref ++;
        std::cout << "scriptref " << this <<  " " << m_Scriptref << std::endl;
        m_ScriptRefMutex.unlock();
    }

    inline void DecreaseScriptRef()
    {
        m_ScriptRefMutex.lock();
        m_Scriptref --;
        std::cout << "scriptref " << this <<  " " << m_Scriptref << std::endl;
        m_ScriptRefMutex.unlock();
    }

};
