#ifndef MODEL_H
#define MODEL_H

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX
#endif



#include "defines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenCLUtil.h"
#include "cl.hpp"

#include <condition_variable>
#include <thread>
#include <chrono>

#include "openglclmanager.h"
#include "openclprogram.h"
#include "openglprogram.h"
#include "openglcldatabuffer.h"
#include "openglgeometry.h"
#include "openglcltexture.h"

#include "iogdal.h"
#include "geo/raster/map.h"
#include "timeseries.h"
#include "parameters/parametermanager.h"

#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QMessageBox>
#include <qmessagebox.h>
#include <QWaitCondition>
#include <thread>
#include "functional"

#include "rigidphysics/rigidworld.h"
#include "rigidphysics/rigidobject.h"

typedef struct MODELTOINTERFACE
{

    float t = 0;
    float h_avg = 0;
    int timesteps = 0;
    int step = 0;
    float dt = 0;

    float area = 0;
    float rain_total = 0;
    float infil_total = 0;
    float wh_total = 0;
    float surfstor_total = 0;
    float canstor_total = 0;
    float chwh_total = 0;
    float outflow_total = 0;
    float failure_total = 0;

    float failures_total = 0;
    float sh_total = 0;
    float outflows_total = 0;

    QList<float> rain;
    QList<int> outlets;
    QList<QList<float>*> Q;
    QList<QList<float>*> H;
    QList<QList<float>*> V;

    QList<QString> m_UIMapNames;
    int m_CurrentUIMap = 0;
    bool m_mapnameschanged = false;

    double val_min = 0;
    double val_max = 0;


} MODELTOINTERFACE;


typedef struct AS_MODELRESULT
{
    QString Dir_Output;
    QString Dir_Maps;
    QString Dir_Time;

    QString Name_HFFINAL;
    QString Name_VELFFINAL;
    QString Name_HFMAX;
    QString Name_VFMAX;
    QString Name_INFIL;
    QString Name_SURFACESTORAGE;
    QString Name_CANOPYSTORAGE;
    QString Name_CHHFFINAL;
    QString Name_CHVELFFINAL;
    QString Name_HSFINAL;
    QString Name_VELSFINAL;
    QString Name_HSMAX;
    QString Name_VSMAX;
    QString Name_CHHSFINAL;
    QString Name_CHVELSFINAL;
    QString Name_FAILURE;
    QString Name_SF;
    QString Name_SFC;
    QString Name_SFracMax;
    QString Name_SoilLoss;
    QString Name_SoilErode;
    QString Name_SoilDeposit;
    QString Name_SedimentLoad;
    QString Name_CHSedimentLoad;

    double SimulationStartTime;
    double SimulationDurationTime;
    double SimulationRealTime;
    double SimulationArea;
    double RainTotal;
    double InterceptionTotal;
    double InfiltrationTotal;
    double SlopeFailureFluidTotal;
    double OutflowFluidTotal;
    double InitialFluidTotal;
    double InitialSolidTotal;
    double SlopeFailureSolidTotal;
    double OutflowSolidTotal;
    double Outflow;
    double OutflowSed;
    double ErosionTotal;
    double DepositionTotal;

    double ErrorF;
    double ErrorS;

} AS_MODELRESULT;

#include "boost/any.hpp"


class LISEM_API LISEMModel
{

public:
    LISEMModel(OpenGLCLManager * in_OpenGLCLManager, ParameterManager * pm);

    OpenGLCLManager * m_OpenGLCLManager;
    ParameterManager * m_ParameterManager;

    QString m_KernelDir;

    void InitModel();
    void OnStep();
    void OnDraw();
    void DrawPlotDataToBuffer(GLuint framebuffer);
    void DrawPlotInterfaceToBuffer(GLuint framebuffer);

    void CopyToUI();
    void GetOutput(float t, float dt);
    void ReportOutputToDisk();
    void ReportOutputToDiskAct();
    void ReportOutputToInterface(float t, float dt);
    void ModelRunInitialize();
    void ModelRunLoadData();
    void ModelRunDeleteData();
    void ModelRunCompileCode();
    void CreateParticleBuffers(int size_req, bool init = true);
    void CreateParticleFromFailures();

    void SetKernelInput();
    void SetUIMaps();
    QList<ModelTexture *> m_ModelTextureList;
    ModelTexture * CreateModelTexture(int,int,float);
    ModelTexture * CreateModelTexture(cTMap *);
    void CreateMapBuffers();
    void CreateDataBuffers();
    void InitialCalculations();

    bool IsRunning();
    void Destroy();

    QThread * modelthread;

    bool m_UIMapsSet = false;
    bool m_Loaded = false;
    bool m_RequiresUIReset = false;
    bool m_DidUpdate = false;

    int m_NCells = 0;
    int m_NParticle = 0;
    int m_NParticlePadded = 0;

    float m_DT = 0.0;

    float hmax = -1e31;
    float hmin = 1e31;
    float havg = 0;
    float uimax = -1e31;
    float uimin = 1e31;
    float uiavg = 0;

    std::function<void(bool,bool)> m_CallBackTimeStep;
    bool m_CallBackTimeStepSet = false;

    inline void SetCallBackTimeStep(std::function<void(bool, bool)> f)
    {
        m_CallBackTimeStep = f;
        m_CallBackTimeStepSet = true;
    }

    ///////
    /// Start/Stop/Pause settings and functions
    ///
    //////

    QList<QString> m_Options;

    bool m_StartRequested = false;
    bool m_PauseRequested = false;
    bool m_StopRequested = false;

    bool m_Started = false;
    bool m_Paused = false;
    bool m_Initializing = false;

    QMutex PauseMutex;
    QWaitCondition PauseCondition;
    QMutex m_ModelStateMutex;
    QMutex m_GLCopyMutex;

    QMutex UILayerRemoveMutex;
    QWaitCondition UILayerRemoveCondition;

    QMutex FinishMutex;
    AS_MODELRESULT FinishResult;
    QWaitCondition FinishCondition;

    void RequestStart(QList<QString> options);
    void RequestPause();
    void RequestStop();

    ///////
    /// Options for the simulations
    ///
    //////

    QString m_Dir_App;
    QString m_Dir_Assets;
    QString m_Dir_Maps;
    QString m_Dir_Time;
    QString m_Dir_Res;

    bool m_MemorySave = false;
    bool m_SimulateSubSection = false;
    int m_SimulateSubSectionIndex = 1;

    bool m_DoInfiltration = false;
    bool m_DoDoubleLayer = false;
    bool m_DoHydrology = false;
    bool m_DoCustomInfiltration = false;
    bool m_DoEvapoTranspiration = false;
    bool m_DoChannel = false;
    bool m_DoGroundWater = false;
    bool m_DoErosion = false;
    bool m_DoEntrainment = false;
    bool m_DoSlopeStability = false;
    bool m_DoSlopeFailure = false;
    bool m_DoInitialWater = false;
    bool m_DoInitialSolids = false;
    bool m_DoParticleSolids = false;
    bool m_DoInitialStability = false;
    bool m_DoSeismic = false;
    bool m_DoImprovedIterative = false;
    bool m_DoRigidWorld = false;
    bool m_DoTree = false;

    bool m_DoCPU = false;

    bool m_DoOutputTiff = false;
    bool m_DoOutputOnlyHV = false;
    bool m_DoOutputTotals = false;
    bool m_DoOutputHydrographs = false;
    bool m_DoOutputTimeseries = false;
    int m_DoOutputTimeseriesEvery = 1;
    int m_DoOutputParticleTimeseriesEvery = 1;
    bool m_DoOutputTimeseriesH = false;
    bool m_DoOutputTimeseriesHS = false;
    bool m_DoOutputTimeseriesHT = false;
    bool m_DoOutputTimeseriesSFRAC = false;
    bool m_DoOutputTimeseriesV = false;
    bool m_DoOutputTimeseriesVS = false;
    bool m_DoOutputTimeseriesVT = false;
    bool m_DoOutputTimeseriesInfil = false;
    bool m_DoOutputTimeseriesGWH = false;
    bool m_DoOutputTimeseriesErosion  = false;
    bool m_DoOutputTimeseriesSediment  = false;

    double m_ElasticModulus = 1000000000.0;
    double m_ShearModulus = 1000000000.0;
    double m_DragMult = 1.0;
    double m_InitialStabMargin= 0.005;
    double m_ReynolldsMult = 1.0;

    std::thread m_ThreadReport;

    QString GetOptionString(QString name);
    int GetOptionInt(QString name);
    double GetOptionDouble(QString name);



    ///////
    /// Options for the simulations
    ///
    //////
    ///
    bool m_BaseMapSet = false;
    cTMap * m_BaseMap = 0;
    bool m_MaskMapSet = false;
    cTMap * m_MaskMap = 0;
    int m_MaskRMin = -1;
    int m_MaskRMax = -1;
    int m_MaskCMin = -1;
    int m_MaskCMax = -1;

    int m_BaseWidth = 0;
    int m_BaseHeight = 0;
    float _dx = 0.0;
    float factor_particle = 5;
    QList<cTMap *> m_MapList;

    void SetMaskMap(QString path, int index);
    cTMap *GetMapSubSection(cTMap * m);
    cTMap *GetBaseMap(QString path);
    cTMap *GetMap(float value);
    cTMap *GetMap(QString path);
    cTMap *GetMapWithMultByName(QString Dir, QString name, double mu = 1.0);
    double GetMapMult(QString name);

    void DestroyMaps();

    QMutex m_ModelDataMutex;
    MODELTOINTERFACE m_InterfaceData;
    bool m_SetOutputArrays = false;
    QList<int> Outlets;
    QList<int> OutletsR;
    QList<int> OutletsC;

    int m_Step = 0;
    float m_Time = 0;

    int m_uimap =0;
    float m_uimin =0;
    float m_uimax =0;

    std::function<void(void)> m_CallBack_ModelToInterface;
    bool m_CallBack_ModelToInterface_Set = false;

    template<typename _Callable, typename... _Args>
    inline void SetModelToInterfaceCallback(_Callable&& __f, _Args&&... __args)
    {
        m_CallBack_ModelToInterface = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBack_ModelToInterface_Set = true;
    }

    std::function<void(bool)> m_CallBack_ModelStopped;
    bool m_CallBack_ModelStopped_Set = false;

    template<typename _Callable, typename... _Args>
    inline void SetModelStoppedCallback(_Callable&& __f, _Args&&... __args)
    {
        m_CallBack_ModelStopped = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1);
        m_CallBack_ModelStopped_Set = true;
    }

    std::function<void(void)> m_CallBack_ModelPaused;
    bool m_CallBack_ModelPaused_Set = false;

    template<typename _Callable, typename... _Args>
    inline void SetModelPausedCallback(_Callable&& __f, _Args&&... __args)
    {
        m_CallBack_ModelPaused = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBack_ModelPaused_Set = true;
    }


    void run();
    void DoModelRun();


    //void SetScriptFunctions(ScriptManager * sm);


    //rigid body physics stuff
    RigidPhysicsWorld * m_RigidWorld = nullptr;


    //opencl particle buffers
    OpenGLCLDataBuffer<int> * m_DataIndex = NULL;
    OpenGLCLDataBuffer<int> * m_DataIndexC = NULL;
    OpenGLCLDataBuffer<float> * m_DataPos = NULL;
    OpenGLCLDataBuffer<float> * m_DataVel = NULL;
    OpenGLCLDataBuffer<float> * m_DataStressTensor = NULL;
    OpenGLCLDataBuffer<float> * m_DataDispTensor = NULL;
    OpenGLCLDataBuffer<float> * m_DataFluidProps = NULL;
    OpenGLCLDataBuffer<float> * m_DataProps = NULL;
    OpenGLCLDataBuffer<float> * m_DataPosN = NULL;
    OpenGLCLDataBuffer<float> * m_DataVelN = NULL;
    OpenGLCLDataBuffer<float> * m_DataStressTensorN = NULL;
    OpenGLCLDataBuffer<float> * m_DataPropsPhysical = NULL;
    OpenGLCLDataBuffer<float> * m_DataUI = NULL;

    OpenCLProgram * m_CLProgram_Particles = NULL;
    OpenCLProgram * m_CLProgram_Particles2 = NULL;
    OpenCLProgram * m_CLProgram_ParticlesToMaps = NULL;
    OpenCLProgram * m_CLProgram_ParticlesFromMaps = NULL;
    OpenCLProgram * m_CLProgram_Flow = NULL;
    OpenCLProgram * m_CLProgram_Flow2 = NULL;
    OpenCLProgram * m_CLProgram_Infiltration = NULL;
    OpenCLProgram * m_CLProgram_Infiltration2 = NULL;
    OpenCLProgram * m_CLProgram_Hydrology = NULL;
    OpenCLProgram * m_CLProgram_Hydrology2 = NULL;
    OpenCLProgram * m_CLProgram_Evapotranspiration = NULL;
    OpenCLProgram * m_CLProgram_Evapotranspiration2 = NULL;
    OpenCLProgram * m_CLProgram_Erosion = NULL;
    OpenCLProgram * m_CLProgram_Erosion2 = NULL;
    OpenCLProgram * m_CLProgram_Rigid = NULL;
    OpenCLProgram * m_CLProgram_Rigid2 = NULL;
    OpenCLProgram * m_CLProgram_GroundWaterFlow = NULL;
    OpenCLProgram * m_CLProgram_GroundWaterFlow2 = NULL;
    OpenCLProgram * m_CLProgram_SlopeStability = NULL;
    OpenCLProgram * m_CLProgram_SlopeStabilityE = NULL;
    OpenCLProgram * m_CLProgram_SlopeFailure = NULL;
    OpenCLProgram * m_CLProgram_SlopeFailureE = NULL;
    OpenCLProgram * m_CLProgram_SlopeFailure2 = NULL;
    OpenCLProgram * m_CLProgram_SlopeFailureP2 = NULL;
    OpenCLProgram * m_CLProgram_SubSurfaceForce1 = NULL;
    OpenCLProgram * m_CLProgram_SubSurfaceForce2 = NULL;
    OpenCLProgram * m_CLProgram_FlowSolids = NULL;
    OpenCLProgram * m_CLProgram_FlowSolids2 = NULL;
    OpenCLProgram * m_CLProgram_FlowSolidsParticle = NULL;
    OpenCLProgram * m_CLProgram_FlowSolidsParticle2 = NULL;
    OpenCLProgram * m_CLProgram_Sort = NULL;
    OpenCLProgram * m_CLProgram_StoreIndices = NULL;




    QList<QString> m_UIMapNames;
    QList<int> m_UIMapNums;

    cTMap * DEM;
    cTMap * DEMM;
    cTMap * LOC_R;
    cTMap * LOC_C;
    cTMap * SLOPE;
    cTMap * HILLSHADE;
    cTMap * N;

    cTMap * KSATTOPSIMPLE;
    cTMap * CLAY;
    cTMap * SAND;
    cTMap * GRAVEL;
    cTMap * ORGANIC;
    cTMap * DENSITY;
    cTMap * ROAD;
    cTMap * BUILDING;
    cTMap * THETAS;
    cTMap * THETAR;
    cTMap * SD;
    cTMap * KSAT_T;

    cTMap * KSAT_B;
    cTMap * SWR_A;
    cTMap * SWR_B;
    cTMap * SMAX_CANOPY;
    cTMap * SMAX_SURFACE;
    cTMap * GW_S;
    cTMap * GW_US;

    cTMap * CH_M;
    cTMap * CH_WIDTH;
    cTMap * CH_HEIGHT;

    cTMap * COHESION;
    cTMap * IFA;
    cTMap * DENSITY_BOTTOM;
    cTMap * ROCKSIZE;

    cTMap * COHESION_TOP;

    cTMap * INITIALHF;
    cTMap * INITIALHFPART;
    cTMap * INITIALHS;

    cTMap * INITIALIFA;
    cTMap * INITIALRS;
    cTMap * INITIALD;
    cTMap * INITIALCOH;
    cTMap * DTREQ;

    cTMap * VEGCOVER;
    cTMap * VEGH;
    cTMap * HARDCOVER;
    cTMap * D50;
    cTMap * D90;
    cTMap * COH;
    cTMap * CHCOH;
    cTMap * Y;
    cTMap * MD;
    cTMap * MDN;
    cTMap * MIFA;
    cTMap * MROCKSIZE;
    cTMap * MDENSITY;
    /*cTMap * SED;
    cTMap * CHSED;
    cTMap * DETTOT;
    cTMap * DEPTOT;
    cTMap * ENTRAINMENTTOT;
    cTMap * SEDN;
    cTMap * CHSEDN;
    cTMap * DETTOTN;
    cTMap * DEPTOTN;
    cTMap * ENTRAINMENTTOTN;
    cTMap * QSEDOUT;
    cTMap * QSEDOUTN;*/

    //timeseries
    cTMap * RAIN;
    cTMap * TEMP;
    cTMap * WIND;
    cTMap * RAD;
    cTMap * VAPR;
    cTMap * NDVI;

    cTMap * OUTLETS;

    //output simulation results
    cTMap * SLOPEFAILURETEMP;
    cTMap * SF;
    cTMap * SFMIN;
    cTMap * SFC;
    cTMap * m_BlockX;
    cTMap * m_BlockY;
    cTMap * SLOPEFAILURE;
    cTMap * SLOPEFAILURETOTAL;
    cTMap * HF;
    cTMap * HS;
    cTMap * HT;
    cTMap * VT;
    cTMap * Q;
    cTMap * QCH;
    cTMap * INFIL;
    cTMap * INFILTOTAL;
    cTMap * SURFACESTORAGE;
    cTMap * CANOPYSTORAGE;
    cTMap * EVAPOTRANSPIRATION;
    cTMap * CROPF;
    cTMap * VFX;
    cTMap * VFY;
    cTMap * VSX;
    cTMap * VSY;
    cTMap * CHHF;
    cTMap * CHVFX;
    cTMap * CHHS;
    cTMap * CHVSX;
    cTMap * PGA;

    cTMap * SED;
    cTMap * CHSED;
    cTMap * SLTOT;
    cTMap * DEPTOT;
    cTMap * DETTOT;

    cTMap * FluxFTotal;
    cTMap * FluxSTotal;
    cTMap * FluxFPeakMomentum;
    cTMap * FluxSPeakMomentum;
    cTMap * PeakMomentum;

    //compound or derived output maps
    cTMap * VELF;
    cTMap * VELS;
    cTMap * SFrac;
    cTMap * HFMax;
    cTMap * HSMax;
    cTMap * HTMax;
    cTMap * VFMax;
    cTMap * VSMax;
    cTMap * VTMax;
    cTMap * SFracMax;
    cTMap * FLOWTIMING;
    cTMap * FLOWDURATION;
    cTMap * GWOUT;
    cTMap * QFOUT;
    cTMap * QSOUT;




    ModelTexture * T_LOCR;
    ModelTexture * T_LOCC;
    ModelTexture * T_DEM;
    OpenGLCLTexture * TUI_DEM;
    ModelTexture * T_N;
    OpenGLCLTexture * TUI_HILLSHADE;
    OpenGLCLTexture * TUI_HS;
    ModelTexture * T_H;
    OpenGLCLTexture * TUI_H;
    OpenGLCLTexture * TUI_UX;
    OpenGLCLTexture * TUI_UY;
    ModelTexture * T_HALL;
    OpenGLCLTexture * TUI_HALL;
    ModelTexture * T_VX;
    ModelTexture * T_VY;
    OpenGLCLTexture * TUI_VX;
    OpenGLCLTexture * TUI_VY;
    ModelTexture * T_HN;
    ModelTexture * T_VXN;
    ModelTexture * T_VYN;
    ModelTexture * T_CHWIDTH;
    ModelTexture * T_CHHEIGHT;
    ModelTexture * T_CHMASK;
    ModelTexture * T_CHH;
    ModelTexture * T_CHVX;
    ModelTexture * T_CHHN;
    ModelTexture * T_CHVXN;

    ModelTexture * T_RAIN;
    ModelTexture * T_TEMP;
    ModelTexture * T_WIND;
    ModelTexture * T_VAPR;
    ModelTexture * T_NDVI;
    ModelTexture * T_RAD;

    ModelTexture * T_CROPF;

    ModelTexture * T_DTREQ;




    ModelTexture * T_QFX1;
    ModelTexture * T_QFX2;
    ModelTexture * T_QFY1;
    ModelTexture * T_QFY2;
    ModelTexture * T_QFCHX1;
    ModelTexture * T_QFCHX2;
    ModelTexture * T_QFCHIN;

    ModelTexture * T_QFOUT;
    ModelTexture * T_QSOUT;
    ModelTexture * T_QGWOUT;
    ModelTexture * T_QFOUTN;
    ModelTexture * T_QSOUTN;
    ModelTexture * T_QGWOUTN;

    ModelTexture * T_HFMax;
    ModelTexture * T_HSMax;
    ModelTexture * T_VFMax;
    ModelTexture * T_VSMax;
    ModelTexture * T_HFMaxN;
    ModelTexture * T_HSMaxN;
    ModelTexture * T_VFMaxN;
    ModelTexture * T_VSMaxN;


    ModelTexture * T_HS;
    ModelTexture * T_VSX;
    ModelTexture * T_VSY;
    ModelTexture * T_HSN;
    ModelTexture * T_VSXN;
    ModelTexture * T_VSYN;
    ModelTexture * T_SDensity;
    ModelTexture * T_SIfa;
    ModelTexture * T_SCohesion;
    ModelTexture * T_SCohesionFrac;
    ModelTexture * T_SFLUIDH;
    ModelTexture * T_SFLUIDHADD;
    ModelTexture * T_SCohesionFracOld;
    ModelTexture * T_SCohesionFFrac;
    ModelTexture * T_SCohesionFFracN;
    ModelTexture * T_SRockSize;
    ModelTexture * T_SDensityN;
    ModelTexture * T_SIfaN;
    ModelTexture * T_SRockSizeN;
    ModelTexture * T_CHHS;
    ModelTexture * T_CHVS;
    ModelTexture * T_CHHSN;
    ModelTexture * T_CHVSN;
    ModelTexture * T_CHSDensity;
    ModelTexture * T_CHSIfa;
    ModelTexture * T_CHSRockSize;
    ModelTexture * T_CHSDensityN;
    ModelTexture * T_CHSIfaN;
    ModelTexture * T_CHSRockSizeN;

    cTMap * MUI;
    cTMap * MUIH;
    cTMap * MUIUX;
    cTMap * MUIUY;
    cTMap * MUIHALL;
    cTMap * MUIHS;
    ModelTexture * T_UI;
    OpenGLCLTexture * TUI_UI;

    ModelTexture * T_SCANOPY;
    ModelTexture * T_SSURFACE;
    ModelTexture * T_INFIL;
    ModelTexture * T_F_INFILACT;
    ModelTexture * T_KSATTOPSIMPLE;
    ModelTexture * T_F_INFILFLOWCUM;
    ModelTexture * T_EVAPOCUM;
    ModelTexture * T_BUILDING;
    ModelTexture * T_ROAD;
    ModelTexture * T_GW_US;
    ModelTexture * T_GW_S;
    ModelTexture * T_THETAS;
    ModelTexture * T_THETAR;
    ModelTexture * T_SD;
    ModelTexture * T_DEMN;
    ModelTexture * T_SDN;
    ModelTexture * T_KSAT_T;
    ModelTexture * T_KSAT_B;
    ModelTexture * T_SWR_A;
    ModelTexture * T_SWR_B;
    ModelTexture * T_SMAX_CANOPY;
    ModelTexture * T_SMAX_SURFACE;
    ModelTexture * T_SCANOPYN;
    ModelTexture * T_SSURFACEN;
    ModelTexture * T_F_INFILPOT;
    ModelTexture * T_GW_WFN;
    ModelTexture * T_GW_USN;
    ModelTexture * T_GW_SN;
    ModelTexture * T_COHESION;
    ModelTexture * T_IFA;
    ModelTexture * T_DENSITY_BOTTOM;
    ModelTexture * T_ROCKSIZE;
    ModelTexture * T_SF;
    ModelTexture * T_SFH;
    ModelTexture * T_SFHCUM;
    ModelTexture * T_SFHCUMN;
    ModelTexture * T_SFC;
    ModelTexture * T_PGA;
    ModelTexture * T_SFFX;
    ModelTexture * T_SFFY;
    ModelTexture * T_SFFXN;
    ModelTexture * T_SFFYN;


    ModelTexture * T_HARDCOVER;
    ModelTexture * T_VEGCOVER;
    ModelTexture * T_VEGH;
    ModelTexture * T_D50;
    ModelTexture * T_D90;
    ModelTexture * T_COH;
    ModelTexture * T_AGGRSTAB;
    ModelTexture * T_CHCOH;
    ModelTexture * T_Y;
    ModelTexture * T_MD;
    ModelTexture * T_MDN;
    ModelTexture * T_MIFA;
    ModelTexture * T_MROCKSIZE;
    ModelTexture * T_MDENSITY;
    ModelTexture * T_SED;
    ModelTexture * T_CHSED;
    ModelTexture * T_DETTOT;
    ModelTexture * T_DEPTOT;
    ModelTexture * T_ENTRAINMENTTOT;
    ModelTexture * T_SEDN;
    ModelTexture * T_CHSEDN;
    ModelTexture * T_DETTOTN;
    ModelTexture * T_DEPTOTN;
    ModelTexture * T_ENTRAINMENTTOTN;
    ModelTexture * T_ENTRAINMENT;
    ModelTexture * T_ENTRAINMENTN;
    ModelTexture * T_ENTRAINMENTCH;
    ModelTexture * T_ENTRAINMENTCHN;

    ModelTexture * T_BlockingFX;
    ModelTexture * T_BlockingFY;
    ModelTexture * T_BlockingX;
    ModelTexture * T_BlockingY;
    ModelTexture * T_BlockingVelX;
    ModelTexture * T_BlockingVelY;
    ModelTexture * T_BlockingLDD;
    ModelTexture * T_HCorrect;

    ModelTexture * T_QSEDOUT;
    ModelTexture * T_QSEDOUTN;

    ModelTexture * T_P_Index;


    TimeSeries m_Rain;
    TimeSeries m_Temp;
    TimeSeries m_Rad;
    TimeSeries m_Wind;
    TimeSeries m_Vapr;
    TimeSeries m_NDVI;




    std::thread m_StarterThread;
    inline void start()
    {
        std::cout << "start "<< std::endl;
        m_StarterThread = std::thread(&(LISEMModel::ModelStarterThread), this);
        m_ReportThread = std::thread(&(LISEMModel::ModelReportThread), this);

    }

    std::mutex m_StarterQuitCondition;
    bool m_StarterQuitRequested = false;
    std::mutex m_StarterStartCondition;
    bool m_StarterStartRequested = false;
    std::condition_variable m_StarterStartRequestedCondition;
    inline void ModelStarterThread()
    {
        while(true)
        {
            std::cout << "try wait "<< std::endl;
            std::unique_lock<std::mutex> lock(m_StarterStartCondition);
            std::cout << "try wait_2 " << std::endl;
            m_StarterStartRequestedCondition.wait(lock);

             std::cout << "test quit"<< std::endl;
             m_StarterQuitCondition.lock();
             if(m_StarterQuitRequested)
             {
                return;
             }
             m_StarterQuitCondition.unlock();

             std::cout << "run"<< std::endl;
             if(m_StarterStartRequested)
             {
                 lock.unlock();
                 m_StarterStartRequested = false;
                 this->run();
             }
        }
    }



    std::thread m_ReportThread;

    std::mutex m_ReportFinish;
    std::condition_variable m_ReportFinishCondition;
    std::mutex m_ReportQuitCondition;
    bool m_ReportQuitRequested = false;
    std::mutex m_ReportStartCondition;
    bool m_ReportStartRequested = false;
    std::condition_variable m_ReportStartRequestedCondition;
    void ModelReportThread();

};

LISEM_API extern LISEMModel * CLISEMModel;
inline void SetGlobalModel(LISEMModel*m)
{
    CLISEMModel = m;
}
inline LISEMModel * GetGLobalModel()
{
    return CLISEMModel;
}

#endif // MODEL_H
