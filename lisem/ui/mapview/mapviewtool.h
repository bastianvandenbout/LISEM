#ifndef MAPVIEWTOOL_H
#define MAPVIEWTOOL_H


#define MAX_MAP_MEM_MB 200

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QString>
#include "error.h"
#include "matrixtable.h"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "scripting/codeeditor.h"
#include "QLineEdit"
#include "error.h"
#include "QTimer"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include "model.h"
#include <QListView>
#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QImage>
#include <QPixmap>
#include <QSlider>
#include "lsmio.h"
#include <QToolButton>
#include "layerwidget.h"
#include "glplot/worldwindow.h"
#include <QFileDialog>
#include "widgets/crsselectionwidget.h"
#include "QTimer"
#include "widgets/labeledwidget.h"
#include "videoexporter.h"
#include "widgets/switchbutton.h"
#include "widgets/labeledwidget.h"
#include "widgets/spoilerwidget.h"
#include "widgets/mapcreatedialog.h"
#include "widgets/vectorcreatedialog.h"
#include "site.h"
#include "resourcemanager.h"
#include "extensionprovider.h"
#include "geo/raster/field.h"
#include "rigidphysics/rigidworld.h"
#include "layer/geo/uifieldlayer.h"
#include "layer/geo/uirigidworldlayer.h"
#include <functional>
#include "layer/ui/uitext.h"
#include "layer/ui/uiimage.h"
#include "layer/ui/uiarrow.h"
#include "layer/geo/uishaderlayer.h"
#include "layer/geo/uiparticleeffectlayer.h"
#include "linear/lsm_vector4.h"
#include "particle/2d/zones/boxzone.h"
#include "particle/2d/zones/pointzone.h"
#include "particle/2d/zones/disczone.h"
#include "particle/2d/zones/discsectionzone.h"
#include "particle/2d/initializers/initcolor.h"
#include "particle/2d/initializers/initposition.h"
#include "particle/2d/initializers/initsize.h"
#include "particle/2d/initializers/initvelocity.h"
#include "particle/2d/initializers/initrotation.h"
#include "particle/2d/initializers/initrotationalvelocity.h"
#include "particle/2d/initializers/initscalevel.h"
#include "particle/2d/behaviors/move.h"
#include "particle/2d/behaviors/boundbybox.h"
#include "particle/2d/renderers/particleimagerenderer.h"
#include "particle/2d/renderers/particlepointrenderer.h"
#include "particle/common/instantiator.h"

class MapViewTool;

extern MapViewTool * CMapViewToolManager;

inline static void InitMapViewTool(MapViewTool * m)
{
    CMapViewToolManager = m;
}

inline static MapViewTool * GetMapViewTool()
{
    return CMapViewToolManager;
}




class ASZone2D
{
private:

    LISEM::Zone * m_Zone = nullptr;
public:

    inline void intern_InitializeFromZone(LISEM::Zone * s, bool copy = true)
    {
        if(copy)
        {
            m_Zone = s->GetCopy();
        }else {
            m_Zone = s;
           }
    }

    inline LISEM::Zone GetZone()
    {
        if(m_Zone!= nullptr)
        {
            return LISEM::Zone(*m_Zone);
        }else
        {
            return LISEM::Zone();
        }
    }

    inline void SetAsBoxZone(BoundingBox b)
    {

        m_Zone = new LISEM::BoxZone(b.ULC(), b.Size());
    }

    inline void SetAsDiscSectionZone()
    {

    }

    inline void SetAsPointZone(LSMVector2 p)
    {
        m_Zone = new LISEM::PointZone(p);

    }

    inline void SetAsPolygonZone(std::vector<LSMVector2> points)
    {

    }

    inline void SetAsDiscZone(LSMVector2 p, float radius, float radius_inner = 0.0)
    {

        m_Zone = new LISEM::DiscZone(p,radius,radius_inner);
    }

    inline void SetAsRasterZone()
    {

    }

    inline void Destroy()
    {

    }

    int            AS_refcount          = 1;
    void           AS_AddRef            ()
    {
        AS_refcount = AS_refcount + 1;

    }
    void           AS_ReleaseRef        ()
    {
        AS_refcount = AS_refcount - 1;
        if(AS_refcount == 0)
        {
            Destroy();
            delete this;
        }

    }


    ASZone2D  * AS_Assign            (ASZone2D* sh)
    {
        Destroy();
        intern_InitializeFromZone(sh->m_Zone->GetCopy());
        return this;
    }


};
inline static ASZone2D * AS_ZoneFactory()
{
    return new ASZone2D();
}

inline static ASZone2D * AS_GetPointZone(LSMVector2 p)
{

    ASZone2D * res =  new ASZone2D();
    res->SetAsPointZone(p);
    return res;
}
inline static ASZone2D * AS_GetBoxZone(BoundingBox b)
{
    ASZone2D * res =  new ASZone2D();
    res->SetAsBoxZone(b);
    return res;
}
inline static ASZone2D * AS_GetDiscZone(LSMVector2 p, float radius, float radius_inner)
{
    ASZone2D * res =  new ASZone2D();
    res->SetAsDiscZone(p,radius,radius_inner);
    return res;
}
inline static ASZone2D * AS_GetDiscZoneGaussian(LSMVector2 p, float radius)
{
 return new ASZone2D();
}
inline static ASZone2D * AS_GetDiscSectorZone(LSMVector2 p, float radius, float radius_inner, float angle_min, float angle_max)
{
 return new ASZone2D();
}
inline static ASZone2D * AS_GetPolygonZone(std::vector<LSMVector2> points)
{
 return new ASZone2D();
}
inline static ASZone2D * AS_GetRasterZone(cTMap * map)
{
 return new ASZone2D();
}
inline static ASZone2D * AS_GetMultiZone(std::vector<ASZone2D*> zones)
{

    return new ASZone2D();
}

class ASEmitter
{

    LISEM::Emitter *m_Emitter;


public:

    inline ASEmitter()
    {

        m_Emitter = new LISEM::Emitter();
    }

    inline LISEM::Emitter GetEmitter()
    {
        return LISEM::Emitter(*m_Emitter);
    }


    inline void AddInitiator_Position(ASZone2D * z, float weight = 1.0)
    {
        m_Emitter->AddInitializer(LISEM::InitializerPosition(z->GetZone()),weight);

    }
    inline void AddInitiator_Size(ASZone2D * z, float weight = 1.0)
    {
        m_Emitter->AddInitializer(LISEM::InitializerScale(z->GetZone()),weight);

    }
    inline void AddInitiator_Rotation(ASZone2D *z,float weight = 1.0)
    {

        m_Emitter->AddInitializer(LISEM::InitializerRotation(z->GetZone()),weight);
    }

    inline void AddInitiator_RotationalVelocity(ASZone2D *z,float weight = 1.0)
    {
        m_Emitter->AddInitializer(LISEM::InitializerRotationVelocity(z->GetZone()),weight);
    }
    inline void AddInitiator_Velocity(ASZone2D * z, float weight = 1.0)
    {
        m_Emitter->AddInitializer(LISEM::InitializerVelocity(z->GetZone()),weight);

    }
    inline void AddInitiator_Color(ASZone2D * z, float alphamin, float alphamax,float weight = 1.0)
    {
        m_Emitter->AddInitializer(LISEM::InitializerColor(z->GetZone(), alphamin, alphamax),weight);

    }
    /*inline void AddInitiator_Color(ASZone2D * z, float weight = 1.0)
    {

    }*/

    inline void AddInstantiator_Burst(int count)
    {
        m_Emitter->AddInstantiator(LISEM::BurstInstantiator(count));
    }

    inline void AddInstantiator_Stream(float rate)
    {

        m_Emitter->AddInstantiator(LISEM::StreamInstantiator(rate));
    }

    inline void AddInstantiator_NoisyStream(float rate, float noise_amp,float noise_period)
    {
        m_Emitter->AddInstantiator(LISEM::NoisyStreamInstantiator(rate,noise_amp, noise_period));

    }

    inline void AddInstantiator_DecliningNoisyStream(float rate, float noise_amp, float noise_period,float rate_decline)
    {

        m_Emitter->AddInstantiator(LISEM::DecliningNoisyStreamInstantiator(rate,noise_amp,noise_period,rate_decline));
    }


    inline void AddBehavior_Move()
    {
        m_Emitter->AddBehavior(LISEM::Move());

    }

    inline void AddBehavior_MinimumDistance(float distance_min)
    {

    }

    inline void AddBehavior_ApproachNeighbors()
    {


    }

    inline void AddBehavior_BoundingBox(BoundingBox Region)
    {

        m_Emitter->AddBehavior(LISEM::BoundedByBox(Region));

    }

    inline void AddBehavior_SpeedLimit()
    {


    }

    inline void AddBehavior_RotateToDirection()
    {

    }

    inline void AddBehavior_MatchVelocity()
    {



    }


    inline void SetRenderer_Image(std::vector<QString> filenames, std::vector<float> probs, int fillstyle)
    {

        m_Emitter->SetRenderer(new LISEM::ParticleImageRenderer(filenames,probs,fillstyle));


    }

    inline void SetRenderer_Image(QString filename, int fillstyle)
    {

        m_Emitter->SetRenderer(new LISEM::ParticleImageRenderer({filename},{1.0},fillstyle));


    }

    inline void AddRenderer_Particle()
    {

    }

    inline void Destroy()
    {


    }

    int            AS_refcount          = 1;
    void           AS_AddRef            ()
    {
        AS_refcount = AS_refcount + 1;

    }
    void           AS_ReleaseRef        ()
    {
        AS_refcount = AS_refcount - 1;
        if(AS_refcount == 0)
        {
            Destroy();
            delete this;
        }

    }


    ASEmitter  * AS_Assign            (ASEmitter* sh)
    {
        Destroy();
        m_Emitter = new LISEM::Emitter(*(sh->m_Emitter));
        return this;
    }


};

inline void CSetEmitterImageRenderer(ASEmitter * e, std::vector<QString> files, std::vector<float> probs, int fillstyle)
{

    e->SetRenderer_Image(files,probs,fillstyle);
}

inline static ASEmitter * AS_ParticleEffectFactory()
{
    return new ASEmitter();
}





struct ASUILayer
{
    int m_UniqueID = -1;
    std::function<void(void)> m_CallBackIncrease;
    std::function<void(void)> m_CallBackDecrease;
public:

    inline ASUILayer()
    {

    }
    inline ~ASUILayer()
    {
        if(m_CallBackDecrease)
        {
            m_CallBackDecrease();
        }

    }

    inline void SetUID(int id, std::function<void(void)> fincr, std::function<void(void)> fdecr)
    {
        m_UniqueID = id;

        m_CallBackIncrease = fincr;
        m_CallBackDecrease = fdecr;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease();
        }
    }

    inline int GetUID()
    {
        return m_UniqueID;
    }

    inline ASUILayer * Assign(ASUILayer * l)
    {
        std::cout << "assign ui layer "<< std::endl;
        m_UniqueID = l->m_UniqueID;
        m_CallBackIncrease = l->m_CallBackIncrease;
        m_CallBackDecrease = l->m_CallBackDecrease;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease();
        }

        return this;
    }
};
static inline void AS_ASUILayerC0(void * mem)
{

    new(mem)  ASUILayer();
}
static inline void AS_ASUILayerD0(ASUILayer * mem)
{
    std::cout << "destruct uilayer " << std::endl;

    mem->~ASUILayer();
}




struct ASUIPlot
{
    int m_UniqueID = -1;
    std::function<void( ASUIPlot*)> m_CallBackIncrease;
    std::function<void( ASUIPlot*)> m_CallBackDecrease;
public:

    inline ASUIPlot()
    {

    }
    inline ~ASUIPlot()
    {
        if(m_CallBackDecrease)
        {
            m_CallBackDecrease(this);
        }

    }

    inline void SetUID(int id, std::function<void( ASUIPlot *)> fincr, std::function<void( ASUIPlot*)> fdecr)
    {
        std::cout << "set id " << id << std::endl;
        m_UniqueID = id;

        m_CallBackIncrease = fincr;
        m_CallBackDecrease = fdecr;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease(this);
        }
    }

    inline int GetUID()
    {
        return m_UniqueID;
    }

    inline ASUIPlot * Assign(ASUIPlot * l)
    {
        std::cout << "assign ui layer "<< std::endl;
        m_UniqueID = l->m_UniqueID;
        m_CallBackIncrease = l->m_CallBackIncrease;
        m_CallBackDecrease = l->m_CallBackDecrease;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease(this);
        }

        return this;
    }
};
static inline void AS_ASUIPlotC0(void * mem)
{

    new(mem)  ASUIPlot();
}
static inline void AS_ASUIPlotD0(ASUIPlot * mem)
{
    std::cout << "destruct uilayer " << std::endl;

    mem->~ASUIPlot();
}







struct ASUIPlotLayer
{
    int m_UniqueID = -1;
    std::function<void(ASUIPlotLayer*)> m_CallBackIncrease;
    std::function<void(ASUIPlotLayer*)> m_CallBackDecrease;
public:

    inline ASUIPlotLayer()
    {

    }
    inline ~ASUIPlotLayer()
    {
        if(m_CallBackDecrease)
        {
            m_CallBackDecrease(this);
        }

    }

    inline void SetUID(int id, std::function<void(ASUIPlotLayer*)> fincr, std::function<void(ASUIPlotLayer*)> fdecr)
    {
        std::cout << "set ui plot layer id " << id << std::endl;
        m_UniqueID = id;

        m_CallBackIncrease = fincr;
        m_CallBackDecrease = fdecr;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease(this);
        }
    }

    inline int GetUID()
    {
        return m_UniqueID;
    }

    inline ASUIPlotLayer * Assign(ASUIPlotLayer * l)
    {
        std::cout << "assign ui layer "<< std::endl;
        m_UniqueID = l->m_UniqueID;
        m_CallBackIncrease = l->m_CallBackIncrease;
        m_CallBackDecrease = l->m_CallBackDecrease;

        if(m_CallBackIncrease)
        {
            m_CallBackIncrease(this);
        }

        return this;
    }
};
static inline void AS_ASUIPlotLayerC0(void * mem)
{

    new(mem)  ASUIPlotLayer();
}
static inline void AS_ASUIPlotLayerD0(ASUIPlotLayer * mem)
{
    std::cout << "destruct uilayer " << std::endl;

    mem->~ASUIPlotLayer();
}



class ListWidgetE : public QListWidget
{
    Q_OBJECT

public:

    inline ListWidgetE(QWidget * parent = 0) : QListWidget(parent)
    {

    }

    inline void dropEvent(QDropEvent * event) override
    {
        QListWidget::dropEvent(event);

        emit OnItemDragDrop();
    }

signals:
    inline void OnItemDragDrop();

};


class myEventFilter: public QObject {
  Q_OBJECT
public:
    WorldWindow * m_WorldWindow;

  myEventFilter(WorldWindow * WorldWindow) {
       m_WorldWindow = WorldWindow;
  }
  ~myEventFilter() {
  }
protected:
  bool eventFilter(QObject* object, QEvent* event) {

      if(object != nullptr)
      {
          //std::cout << "event " << event->type() << " " << object->inherits("QSplitter")  <<  object->metaObject()->className() << std::endl;
          //object->dumpObjectInfo();
      }else
      {
          //std::cout << "event " << event->type() << "no object " << std::endl;
      }
    if(event->type() == QEvent::MouseMove
             || event->type() == QEvent::TouchBegin
             || event->type() == QEvent::TouchUpdate
             || event->type() == QEvent::TouchEnd
             || event->type() == QEvent::GrabMouse
             || event->type() == QEvent::UngrabMouse
             || event->type() == QEvent::GrabKeyboard
             || event->type() == QEvent::UngrabKeyboard
             || event->type() == QEvent::GraphicsSceneMouseMove
             || event->type() == QEvent::GraphicsSceneMousePress
             || event->type() == QEvent::GraphicsSceneMouseRelease
             || event->type() == QEvent::GraphicsSceneMouseDoubleClick
             || event->type() == QEvent::GraphicsSceneContextMenu
             || event->type() == QEvent::GraphicsSceneHoverEnter
             || event->type() == QEvent::GraphicsSceneHoverMove
             || event->type() == QEvent::GraphicsSceneHoverLeave
             || event->type() == QEvent::GraphicsSceneHelp
             || event->type() == QEvent::GraphicsSceneDragEnter
             || event->type() == QEvent::GraphicsSceneDragMove
             || event->type() == QEvent::GraphicsSceneDragLeave
             || event->type() == QEvent::GraphicsSceneDrop
             || event->type() == QEvent::GraphicsSceneWheel
             || event->type() == QEvent::MouseButtonPress
             || event->type() == QEvent::MouseButtonRelease
             || event->type() == QEvent::MouseButtonDblClick
             || event->type() == QEvent::MouseMove
             || event->type() == QEvent::KeyPress
             || event->type() == QEvent::KeyRelease)
    {

        m_WorldWindow->SetRedrawNeeded();
        return false; // make it unhandled and sent to other filters.
    } else
    {
      return false;
    }
  }
};


class MapViewTool : public QWidget
{
        Q_OBJECT;

public:
    WorldWindow * m_WorldWindow;
    QComboBox * m_map_combo;

    LISEMModel * m_Model;
    MODELTOINTERFACE * m_ModelData;

    QIcon * NewIcon;
    QToolButton * NewButton;

    ListWidgetE *m_Layers;

    QTabWidget * m_ParentTabWidget;

    QLabel *CRSLabel;

    QTimer m_VideoTimer;
    QToolButton *m_RecordButton;
    QToolButton *m_StopButton;
    QToolButton *m_DelButton;
    QDoubleSpinBox *m_FPSBox;
    QDoubleSpinBox *m_RecordFPSBox;
    QSpinBox *m_WidthBox;
    QSpinBox *m_HeightBox;
    QLineEdit *m_RecordFile;
    VideoCapture * m_VideoCapture;


    QToolButton *m_RecordImButton;
    QSpinBox *m_WidthImBox;
    QSpinBox *m_HeightImBox;
    QLineEdit *m_RecordImFile;
    SwitchButton * m_3DButton;
    SwitchButton * m_3DGlobeButton;

    ColorButton * m_BackGroundColor;

    QTimer *m_AnimateTimer;

    inline MapViewTool(  LISEMModel * m , MODELTOINTERFACE *minterface ,WorldWindow * w,QTabWidget * tabwidget, QWidget *parent = 0): QWidget( parent)
    {

        SPHResourceManager *src = GetResourceManager();


        InitMapViewTool(this);

        m_ParentTabWidget = tabwidget;

        QString m_Dir = GetSite();

        m_WorldWindow = w;
        m_Model = m;
        m_ModelData = minterface;


        QCoreApplication::instance()->installEventFilter(new myEventFilter(m_WorldWindow));


        QVBoxLayout *gblayout_map  = new QVBoxLayout;
        gblayout_map->setMargin(0);
        gblayout_map->setSpacing(0);
        this->setLayout(gblayout_map);

        m_map_combo = new QComboBox(this);
        m_map_combo->addItem("Default");

        connect(m_map_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnMapIndexChanged(int)));

        QWidget * hw1 = new QWidget();
        QHBoxLayout *templayout1 = new QHBoxLayout(hw1);
        hw1->setLayout(templayout1);
        templayout1->setSpacing(0);
        templayout1->setMargin(0);

        QWidget * hw2 = new QWidget();
        QHBoxLayout *templayout2 = new QHBoxLayout(hw2);
        hw2->setLayout(templayout2);
        templayout2->setSpacing(0);
        templayout2->setMargin(0);


        QWidget * TitleBar = new QWidget();
        QHBoxLayout * TitleLayout = new QHBoxLayout();
        TitleBar->setLayout(TitleLayout);

        //QLabel * m_BrowseTreeTitle = new QLabel("Map Layers",this);
        //m_BrowseTreeTitle->setFont(QFont( "Helvetica", 16 ));
        //m_BrowseTreeTitle->setMargin(2);

        m_3DButton = new SwitchButton();
        m_3DGlobeButton = new SwitchButton();

        m_3DButton->setValue(false);
        m_3DGlobeButton->setValue(false);

        connect(m_3DButton,SIGNAL(valueChanged(bool)),this,SLOT(On3DToggled(bool)));
        connect(m_3DGlobeButton,SIGNAL(valueChanged(bool)),this,SLOT(On3DGlobeToggled(bool)));

        QIcon iconR;
        iconR.addFile((m_Dir + LISEM_FOLDER_ASSETS + "addraster.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconV;
        iconV.addFile((m_Dir + LISEM_FOLDER_ASSETS + "addvector.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconW;
        iconW.addFile((m_Dir + LISEM_FOLDER_ASSETS + "addweb.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconEV;
        iconEV.addFile((m_Dir + LISEM_FOLDER_ASSETS + "addenvironment.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconPC;
        iconPC.addFile((m_Dir + LISEM_FOLDER_ASSETS + "addpointcloud.png"), QSize(), QIcon::Normal, QIcon::Off);

        QIcon *iconFL= src->GetIcon(src->GetDefaultIconName(LISEM_ICON_ADDFIELD));

        QIcon iconLoad;
        iconLoad.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

        QIcon iconSave;
        iconSave.addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);


        QToolButton *loadButton = new QToolButton();
        loadButton->setIcon(iconLoad);
        loadButton->setIconSize(QSize(22,22));
        loadButton->resize(22,22);
        loadButton->setEnabled(true);


        QAction * loadAct = new QAction(tr("Load"), this);
        loadAct->setStatusTip(tr("Load project"));

        QAction * newAct = new QAction(tr("New"), this);
        newAct->setStatusTip(tr("Create new empty project"));


        loadButton->setPopupMode(QToolButton::MenuButtonPopup);
        loadButton->addAction(loadAct);
        loadButton->addAction(newAct);

        connect(loadAct,SIGNAL(triggered(bool)), this, SLOT(LoadPressed()));
        connect(newAct,SIGNAL(triggered(bool)), this, SLOT(NewPressed()));


        QToolButton *saveButton = new QToolButton();
        saveButton->setIcon(iconSave);
        saveButton->setIconSize(QSize(22,22));
        saveButton->resize(22,22);
        saveButton->setEnabled(true);


        QAction * saveAct = new QAction(tr("Save"), this);
        saveAct->setStatusTip(tr("Save project to disk"));

        QAction * saveasAct = new QAction(tr("Save As"), this);
        saveasAct->setStatusTip(tr("Save project to disk as..."));


        saveButton->setPopupMode(QToolButton::MenuButtonPopup);
        saveButton->addAction(saveAct);
        saveButton->addAction(saveasAct);

        connect(saveAct,SIGNAL(triggered(bool)), this, SLOT(SavePressed()));
        connect(saveasAct,SIGNAL(triggered(bool)), this, SLOT(SaveAsPressed()));


        QToolButton *addButton = new QToolButton();
        addButton->setIcon(iconR);
        addButton->setIconSize(QSize(22,22));
        addButton->resize(22,22);
        addButton->setEnabled(true);

        QAction * RMAAct = new QAction(tr("Add"), this);
        RMAAct->setShortcuts(QKeySequence::New);
        RMAAct->setStatusTip(tr("Add a raster to the display: 1 file-> single map, 2 files with 1 band-> duo map, otherwise timeseries; Added in memory based on total size"));

        QAction * RMAct = new QAction(tr("Add in Memory"), this);
        RMAct->setStatusTip(tr("Add a raster to the display, and load the full data into the GPU memory (fast, for small files < 100mb)"));

        QAction * RSAct = new QAction(tr("Stream from Disk"), this);
        RSAct->setStatusTip(tr("Add a raster to the display, allocate a temporary buffer and stream raster data from the disk (slow, for large files > 500 mb"));

        QAction * RTSAct = new QAction(tr("Timeseries"), this);
        RTSAct->setStatusTip(tr("Add a raster timeseries"));

        QAction * RDAct = new QAction(tr("Duo"), this);
        RDAct->setStatusTip(tr("Add a raster with dual-parameter visualization"));

        QAction * RNAct = new QAction(tr("New"), this);
        RNAct->setStatusTip(tr("Create new raster"));


        connect(RMAAct,SIGNAL(triggered(bool)), this, SLOT(OnAddPressed()));
        connect(RMAct,SIGNAL(triggered(bool)), this, SLOT(OnAddRasterMemoryPressed()));
        connect(RSAct,SIGNAL(triggered(bool)), this, SLOT(OnAddRasterStreamPressed()));
        connect(RTSAct,SIGNAL(triggered(bool)), this, SLOT(OnAddRasterTimeSeriesPressed()));
        connect(RDAct,SIGNAL(triggered(bool)), this, SLOT(OnAddRasterDuoPressed()));
        connect(RNAct,SIGNAL(triggered(bool)), this, SLOT(OnAddRasterNewPressed()));

        addButton->setPopupMode(QToolButton::MenuButtonPopup);
        addButton->addAction(RMAAct);
        addButton->addAction(RMAct);
        addButton->addAction(RSAct);
        addButton->addAction(RTSAct);
        addButton->addAction(RDAct);
        addButton->addAction(RNAct);

        QToolButton *addVButton = new QToolButton();
        addVButton->setIcon(iconV);
        addVButton->setIconSize(QSize(22,22));
        addVButton->resize(22,22);
        addVButton->setEnabled(true);


        QAction * VAAct = new QAction(tr("Add"), this);
        VAAct->setStatusTip(tr("Add a vector file to the display"));

        QAction * VNAct = new QAction(tr("New"), this);
        VNAct->setStatusTip(tr("Create new vector file"));


        addVButton->setPopupMode(QToolButton::MenuButtonPopup);
        addVButton->addAction(VAAct);
        addVButton->addAction(VNAct);


        connect(VAAct,SIGNAL(triggered(bool)), this, SLOT(OnAddVPressed()));
        connect(VNAct,SIGNAL(triggered(bool)), this, SLOT(OnAddVNewPressed()));


        QToolButton *addFLButton = new QToolButton();
        addFLButton->setIcon(*iconFL);
        addFLButton->setIconSize(QSize(22,22));
        addFLButton->resize(22,22);
        addFLButton->setEnabled(true);


        QAction * FLAAct = new QAction(tr("Add"), this);
        FLAAct->setStatusTip(tr("Add a field to the display"));

        QAction * FLNAct = new QAction(tr("New"), this);
        FLNAct->setStatusTip(tr("Create new field"));


        addFLButton->setPopupMode(QToolButton::MenuButtonPopup);
        addFLButton->addAction(FLAAct);
        addFLButton->addAction(FLNAct);


        connect(FLAAct,SIGNAL(triggered(bool)), this, SLOT(OnAddFLPressed()));
        connect(FLNAct,SIGNAL(triggered(bool)), this, SLOT(OnAddFLNewPressed()));



        QToolButton *addEVButton = new QToolButton();
        addEVButton->setIcon(iconEV);
        addEVButton->setIconSize(QSize(22,22));
        addEVButton->resize(22,22);
        addEVButton->setEnabled(true);

        addEVButton->setPopupMode(QToolButton::MenuButtonPopup);

        QAction * etAct = new QAction(tr("Text"), this);
        etAct->setStatusTip(tr("Add a layer which displays text"));

        QAction * eiAct = new QAction(tr("Image"), this);
        eiAct->setStatusTip(tr("Add a layer with a simpe image"));

        QAction * eaAct = new QAction(tr("Arrow"), this);
        eaAct->setStatusTip(tr("Add a layer with an arrow"));

        QAction * eshaderAct = new QAction(tr("Shader"), this);
        eshaderAct->setStatusTip(tr("Add a custom shader layer"));

        QAction * eoAct = new QAction(tr("Ocean"), this);
        eoAct->setStatusTip(tr("Ocean layer at elevation 0.0"));

        QAction * esAct = new QAction(tr("Sky"), this);
        esAct->setStatusTip(tr("Sky layer"));

        QAction * eobAct = new QAction(tr("Object"), this);
        eobAct->setStatusTip(tr("Object from textured model"));

        QAction * eclAct = new QAction(tr("Clouds"), this);
        eclAct->setStatusTip(tr("Add Volumetric Cloud Layer"));

        QAction * textAct = new QAction(tr("Text"), this);
        textAct->setStatusTip(tr("Add text Layer"));

        QAction * arrowAct = new QAction(tr("Arrow"), this);
        arrowAct->setStatusTip(tr("Add arrow Layer"));

        connect(etAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVTextPressed()));
        connect(eiAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVImagePressed()));
        connect(eaAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVArrowPressed()));

        connect(eoAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVOceanPressed()));
        connect(esAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVSkyPressed()));

        connect(eshaderAct,SIGNAL(triggered(bool)), this, SLOT(OnAddShaderPressed()));
        connect(eobAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVObjectPressed()));
        connect(eclAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVCloudPressed()));
        connect(textAct,SIGNAL(triggered(bool)), this, SLOT(OnAddTextPressed()));
        connect(arrowAct,SIGNAL(triggered(bool)), this, SLOT(OnAddArrowPressed()));
        connect(textAct,SIGNAL(triggered(bool)), this, SLOT(OnAddTextPressed()));

        addEVButton->addAction(eoAct);
        addEVButton->addAction(esAct);
        addEVButton->addAction(eshaderAct);
        addEVButton->addAction(eiAct);
        addEVButton->addAction(eobAct);
        addEVButton->addAction(eclAct);
        addEVButton->addAction(textAct);
        addEVButton->addAction(arrowAct);

        QToolButton *addPCButton = new QToolButton();
        addPCButton->setIcon(iconPC);
        addPCButton->setIconSize(QSize(22,22));
        addPCButton->resize(22,22);
        addPCButton->setEnabled(true);

        QToolButton *addWebButton = new QToolButton();
        addWebButton->setIcon(iconW);
        addWebButton->setIconSize(QSize(22,22));
        addWebButton->resize(22,22);
        addWebButton->setEnabled(true);

        QAction * dAct = new QAction(tr("Sattelite"), this);
        dAct->setShortcuts(QKeySequence::New);
        dAct->setStatusTip(tr("Google Sattelite Layer"));

        QAction * ttAct = new QAction(tr("Elevation"), this);
        ttAct->setShortcuts(QKeySequence::New);
        ttAct->setStatusTip(tr("Google Elevation Layer"));

        connect(dAct,SIGNAL(triggered(bool)), this, SLOT(OnAddWebPressed()));
        connect(ttAct,SIGNAL(triggered(bool)), this, SLOT(OnAddWebDEMPressed()));

        addWebButton->setPopupMode(QToolButton::MenuButtonPopup);
        addWebButton->addAction(dAct);
        addWebButton->addAction(ttAct);

        QIcon iconCRS;
        iconCRS.addFile((m_Dir + LISEM_FOLDER_ASSETS + "globe.png"), QSize(), QIcon::Normal, QIcon::Off);

        QToolButton *CRSButton = new QToolButton();
        CRSButton->setIcon(iconCRS);
        CRSButton->setIconSize(QSize(22,22));
        CRSButton->resize(22,22);
        CRSButton->setEnabled(true);

        CRSLabel = new QLabel("CRS: Generic");

        QSpacerItem * spacer = new QSpacerItem(5000,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem * spacer2 = new QSpacerItem(5000,20,QSizePolicy::Expanding, QSizePolicy::Minimum);

        TitleLayout->addWidget(loadButton);
        TitleLayout->addWidget(saveButton);
        TitleLayout->addItem(spacer);
        TitleLayout->addWidget(new QLabeledWidget("3D",m_3DButton));
        TitleLayout->addWidget(new QLabeledWidget("Globe",m_3DGlobeButton));
        TitleLayout->addItem(spacer2);
        TitleLayout->addWidget(addButton);
        TitleLayout->addWidget(addVButton);
        TitleLayout->addWidget(addPCButton);
        TitleLayout->addWidget(addFLButton);
        TitleLayout->addWidget(addWebButton);
        TitleLayout->addWidget(addEVButton);
        TitleLayout->addWidget(CRSButton);
        TitleLayout->addWidget(CRSLabel);

        m_Layers = new ListWidgetE();
        m_Layers->setMinimumSize(QSize(500,250));
        m_Layers->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_Layers->update();
        m_Layers->setDragDropMode(QAbstractItemView::InternalMove);
        m_Layers->setDefaultDropAction(Qt::MoveAction);



        gblayout_map->addWidget(hw1);
        gblayout_map->addWidget(hw2);
        gblayout_map->addWidget(TitleBar);
        gblayout_map->addWidget(m_Layers);



        connect(addButton,SIGNAL(clicked(bool)), this, SLOT(OnAddPressed()));
        connect(addVButton,SIGNAL(clicked(bool)), this, SLOT(OnAddVPressed()));
        connect(addWebButton,SIGNAL(clicked(bool)), this, SLOT(OnAddWebPressed()));
        connect(addPCButton,SIGNAL(clicked(bool)), this, SLOT(OnAddPCPressed()));
        connect(CRSButton,SIGNAL(clicked(bool)), this, SLOT(OnCRSPressed()));

        connect(m_Layers,SIGNAL(OnItemDragDrop()),this,SLOT(OnListOrderChanged()),Qt::ConnectionType::QueuedConnection);
        connect(m_Layers,SIGNAL(itemSelectionChanged()),this,SLOT(OnSelectedLayerChanged()),Qt::ConnectionType::QueuedConnection);
        connect(m_WorldWindow,SIGNAL(OnMapsChanged()),this,SLOT(OnLayerDataChanged()),Qt::ConnectionType::QueuedConnection);
        connect(m_WorldWindow,SIGNAL(OnFocusLocationChanged()),this,SLOT(OnFocusLocationChanged()),Qt::ConnectionType::QueuedConnection);

        //load icons
        QIcon iconrec;
        iconrec.addFile((m_Dir + LISEM_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconstop;
        iconstop.addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon iconbin;
        iconbin.addFile((m_Dir + LISEM_FOLDER_ASSETS + "bin.png"), QSize(), QIcon::Normal, QIcon::Off);


        //drawing options

        Spoiler *m_DrawingOptionsBox = new Spoiler("Drawing Options");

        QWidget *m_DrawingOptionsWidget = new QWidget();
        QVBoxLayout * m_DrawingOptionsVLayout = new QVBoxLayout();

        QHBoxLayout *m_DrawingOptionsLayout = new QHBoxLayout();




        QCheckBox * m_CheckBoxUI = new QCheckBox("Draw UI");
        QCheckBox * m_CheckBoxLegend = new QCheckBox("Draw Legends");
        QCheckBox * m_CheckBoxShadow = new QCheckBox("Draw Shadows");
        QCheckBox * m_CheckBoxLighting = new QCheckBox("Light Rendering");
        QCheckBox * m_CheckBoxLines = new QCheckBox("Draw Coordinate Lines");
        m_BackGroundColor = new ColorButton("BackGround Color");
        m_BackGroundColor->SetColor(m_WorldWindow->GetBackGroundColor());

        QDoubleSpinBox *m_UIScale = new QDoubleSpinBox();
        m_CheckBoxUI->setChecked(true);
        m_CheckBoxLegend->setChecked(true);
        m_UIScale->setValue(1.0);
        m_UIScale->setMinimum(0.0001);
        m_UIScale->setMaximum(1000.0);
        m_UIScale->setSingleStep(0.05);

        connect(m_CheckBoxLines,SIGNAL(toggled(bool)),this,SLOT(OnLinesToggled(bool)));
        connect(m_CheckBoxUI,SIGNAL(toggled(bool)),this,SLOT(OnUIToggled(bool)));
        connect(m_CheckBoxLegend,SIGNAL(toggled(bool)),this,SLOT(OnLegendToggled(bool)));
        connect(m_CheckBoxShadow,SIGNAL(toggled(bool)),this,SLOT(OnShadowToggled(bool)));
        connect(m_CheckBoxLighting,SIGNAL(toggled(bool)),this,SLOT(OnLightingToggled(bool)));
        connect(m_UIScale,SIGNAL(valueChanged(double)),this,SLOT(onUIScaleChanged(double)));
        connect(m_BackGroundColor,SIGNAL(OnColorChanged()),this,SLOT(onBackGroundColorChanged()));


        m_DrawingOptionsLayout->addWidget(m_CheckBoxUI);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxLegend);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxShadow);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxLighting);
        m_DrawingOptionsLayout->addWidget(m_BackGroundColor);


        m_DrawingOptionsLayout->addWidget(new QLabeledWidget("UI Scale Multiplier",m_UIScale));
        m_DrawingOptionsLayout->setSpacing(0);
        m_DrawingOptionsLayout->setMargin(0);


        //m_DrawingOptionsLayout->setSizeConstraint(QLayout::SetMaximumSize);
        m_DrawingOptionsWidget->setLayout(m_DrawingOptionsLayout);
        m_DrawingOptionsVLayout->addWidget(new QWidgetVDuo(m_DrawingOptionsWidget,new QWidgetHDuo(new QLabel("User Map from Model"),m_map_combo)));

        m_DrawingOptionsBox->setContentLayout(* m_DrawingOptionsVLayout);

        //image render options

        Spoiler *m_RenderOptionsBox = new Spoiler("Render Image");
        QHBoxLayout *m_RenderOptionsLayout = new QHBoxLayout();

        m_RecordImButton = new QToolButton();
        m_RecordImButton->setIcon(iconrec);
        m_WidthImBox = new QSpinBox();
        m_WidthImBox->setMinimum(16);
        m_WidthImBox->setMaximum(10000);
        m_WidthImBox->setValue(1920);
        m_HeightImBox = new QSpinBox();
        m_HeightImBox->setMinimum(16);
        m_HeightImBox->setMaximum(10000);
        m_HeightImBox->setValue(1080);

        m_RecordImFile = new QLineEdit();
        m_RecordImFile->setText(GetSite()+"/image.png");

        connect(m_RecordImButton,SIGNAL(pressed()),this,SLOT(OnImageRecord()));

        m_RenderOptionsLayout->addWidget(m_RecordImButton);
        m_RenderOptionsLayout->addWidget(new QLabeledWidget("Width",m_WidthImBox));
        m_RenderOptionsLayout->addWidget(new QLabeledWidget("Height",m_HeightImBox));
        m_RenderOptionsLayout->addWidget(m_RecordImFile);

        m_RenderOptionsLayout->setSpacing(0);
        m_RenderOptionsLayout->setMargin(0);


        //m_RenderOptionsLayout->setSizeConstraint(QLayout::SetMaximumSize);
        m_RenderOptionsBox->setContentLayout(*m_RenderOptionsLayout);

        //recording options

        Spoiler*m_RecordingOptionsBox = new Spoiler("Recording Options");
        QHBoxLayout *m_RecordingOptionsLayout = new QHBoxLayout();


        m_RecordButton = new QToolButton();
        m_RecordButton->setIcon(iconrec);
        m_StopButton = new QToolButton();
        m_StopButton->setIcon(iconstop);
        m_StopButton->setEnabled(false);
        m_DelButton = new QToolButton();
        m_DelButton->setIcon(iconbin);
        m_DelButton->setEnabled(false);
        m_FPSBox = new QDoubleSpinBox();
        m_FPSBox->setValue(1);
        m_FPSBox->setMinimum(1.0/60.0);
        m_FPSBox->setMaximum(30);
        m_FPSBox->setDecimals(3);
        m_RecordFPSBox = new QDoubleSpinBox();
        m_RecordFPSBox->setValue(10);
        m_RecordFPSBox->setMinimum(1);
        m_RecordFPSBox->setMaximum(60);
        m_RecordFPSBox->setDecimals(3);
        m_WidthBox = new QSpinBox();
        m_WidthBox->setMinimum(16);
        m_WidthBox->setMaximum(3840);
        m_WidthBox->setValue(1920);
        m_HeightBox = new QSpinBox();
        m_HeightBox->setMinimum(16);
        m_HeightBox->setMaximum(2160);
        m_HeightBox->setValue(1080);


        m_RecordFile = new QLineEdit();
        m_RecordFile->setText(GetSite()+"/recording.mp4");

        connect(m_RecordButton,SIGNAL(pressed()),this,SLOT(OnVideoRecord()));
        connect(m_StopButton,SIGNAL(pressed()),this,SLOT(OnVideoStop()));
        connect(m_DelButton,SIGNAL(pressed()),this,SLOT(OnVideoDel()));
        connect(&m_VideoTimer, SIGNAL(timeout()),this,SLOT(GetVideoFrame()));

        m_RecordingOptionsLayout->addWidget(m_RecordButton);
        m_RecordingOptionsLayout->addWidget(m_StopButton);
        m_RecordingOptionsLayout->addWidget(m_DelButton);
        m_RecordingOptionsLayout->addWidget(new QLabeledWidget("Frametime",m_FPSBox));
        m_RecordingOptionsLayout->addWidget(new QLabeledWidget("PlayFPS",m_RecordFPSBox));
        m_RecordingOptionsLayout->addWidget(new QLabeledWidget("Width",m_WidthBox));
        m_RecordingOptionsLayout->addWidget(new QLabeledWidget("Height",m_HeightBox));
        m_RecordingOptionsLayout->addWidget(m_RecordFile);
        m_RecordingOptionsLayout->setSpacing(0);
        m_RecordingOptionsLayout->setMargin(0);

        //m_RecordingOptionsLayout->setSizeConstraint(QLayout::SetMaximumSize);
        m_RecordingOptionsBox->setContentLayout(*m_RecordingOptionsLayout);



        gblayout_map->addWidget(m_DrawingOptionsBox);
        gblayout_map->addWidget(m_RenderOptionsBox);
        gblayout_map->addWidget(m_RecordingOptionsBox);

        LoadCRSTable(GetSite());


        m_AnimateTimer = new QTimer(this);
        connect(m_AnimateTimer, SIGNAL(timeout()), this, SLOT(AnimationUpdate()));
        m_AnimateTimer->start(10);

    }



    void AddLayerNative(QString Layer)
    {



    }

    void AddLayerFromFile(QString path)
    {

        UILayer *ml2 = m_WorldWindow->GetUILayerFromFile(path);

        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }



    void AddLayerFromFileAutoStream(QString path)
    {
        int size = 0;
        QFile myFile(path);
        if (myFile.open(QIODevice::ReadOnly)){
            size = myFile.size();  //when file does open.
            myFile.close();
        }
        if(size/(1024*1024) < MAX_MAP_MEM_MB)
        {

            UILayer *ml2 = m_WorldWindow->GetUILayerFromFile(path);

            if(ml2 != nullptr)
            {
                m_WorldWindow->AddUILayer(ml2,true);
            }
        }else
        {
            UILayer *ml2 = m_WorldWindow->GetUIStreamLayerFromFile(path);

            if(ml2 != nullptr)
            {
                m_WorldWindow->AddUILayer(ml2,true);
            }
        }
    }

    void AddVectorLayerFromFile(QString path)
    {
        UILayer *ml2 = m_WorldWindow->GetUIVectorLayerFromFile(path);
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }
    }

    void AddFieldLayerFromFile(QString path)
    {
        UILayer *ml2 = m_WorldWindow->GetUIFieldLayerFromFile(path);
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }
    }

    void AddStreamLayerFromFile(QString path)
    {

        UILayer *ml2 = m_WorldWindow->GetUIStreamLayerFromFile(path);


        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }

    void AddDuoLayerFromFile(QList<QString> path)
    {

        UILayer *ml2 = m_WorldWindow->GetUIDuoLayerFromFile(path);


        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }

    void AddTimeSeriesLayerFromFile(QList<QString> path)
    {

        UILayer *ml2 = m_WorldWindow->GetUITimeSeriesLayerFromFile(path);


        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }

    void AddObjectLayerFromFile(QString path)
    {
        UILayer *ml2 = m_WorldWindow->GetUIObjectLayerFromFile(path);
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }

    void AddPointCloudLayerFromFile(QString path)
    {

        UILayer *ml2 = m_WorldWindow->GetUIPointCloudLayerFromFile(path);
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }

    }
    void AddGoogleLayer()
    {
        UILayer * ml2 = m_WorldWindow->GetGoogleLayer();
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }
    }

    void AddGoogleDEMLayer()
    {
        UILayer * ml2 = m_WorldWindow->GetGoogleDEMLayer();
        if(ml2 != nullptr)
        {
            m_WorldWindow->AddUILayer(ml2,true);
        }
    }



    void RemoveLayer()
    {




    }

    inline ASUILayer AddLayerFromScript(cTMap * m, QString name, bool can_remove)
    {
        ASUILayer lay;

        if(m != nullptr)
        {
                RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{m->GetCopy()}}),false,true);

                std::cout << "rdp editable " << rdp->IsEditable() <<  " " << rdp->IsMemoryMap() << std::endl;
                if(rdp->Exists())
                {
                    UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,name,m->AS_FileName.size() == 0? false : true,m->AS_FileName,false);
                    if(rdp->GetBandCount() > 1)
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
                    }else
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
                    }
                    m_WorldWindow->AddUILayer(ret,true);


                    lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret) );

                    return lay;

                }else {
                    return lay;
                }
        }

        return lay;
    }


    inline ASUILayer AddVelLayerFromScript(cTMap * m, cTMap * m2,QString name, bool can_remove)
    {
        ASUILayer lay;

        if(m != nullptr && m2 != nullptr)
        {

            QList<QList<cTMap *>> lm;
            QList<cTMap*> lm1;
            lm1.append(m->GetCopy());
            lm1.append(m2->GetCopy());
            lm.append(lm1);

            QList<QList<OpenGLCLTexture *>> lt;
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(new RasterDataProvider(lm,true,true),name,m->AS_FileName.size() == 0? false : true,m->AS_FileName,false);
            ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_DUORASTER_VEL));

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret) );

        }

        return lay;
    }

    inline void ReplaceVelLayerFromScript(ASUILayer id, cTMap * mx, cTMap * my)
    {
        if(mx != nullptr && my!= nullptr&& id.GetUID()  > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID() );
            if(l != nullptr)
            {

                if(l->layertypeName() == "RasterStreamLayer")
                {
                    UIStreamRasterLayer * lrs =  (UIStreamRasterLayer *) l;
                    if(lrs->IsLayerDirectReplaceable({{mx,my}}))
                    {
                        m_WorldWindow->m_UILayerMutex.lock();
                        std::cout << "do direct replace " << std::endl;
                        lrs->DirectReplace({{mx->GetCopy(),my->GetCopy()}});

                        m_WorldWindow->SetRedrawNeeded();

                        m_WorldWindow->m_UILayerMutex.unlock();
                        return;
                    }
                }

                RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{mx->GetCopy(),my->GetCopy()}}),false,true);


                if(rdp->Exists())
                {
                    QString name =l->GetName();
                    UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,name,false,"",!false);
                    if(rdp->GetBandCount() > 1)
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_DUORASTER_VEL),true);
                    }else
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
                    }

                    m_WorldWindow->ReplaceUILayer(l,ret);
                }else
                {

                    LISEMS_WARNING("Could not create raster layer to replace layer id " + QString::number(id.GetUID()));
                }

            }else
            {

                LISEMS_WARNING("Could not replace layer with id " + QString::number(id.GetUID()));
            }
        }
    }



    inline ASUILayer AddLayerFromScript(ShapeFile * m, QString name, bool can_remove)
    {
        ASUILayer lay;

        if(m != nullptr)
        {
            ShapeFile * s = new ShapeFile();
            s->CopyFrom(m);
            UIVectorLayer *HS = new UIVectorLayer(s,QString(name),m->AS_FileName.size() == 0? false : true,m->AS_FileName,!can_remove);
            HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_VECTORUI),true);
            m_WorldWindow->AddUILayer(HS,true);

            lay.SetUID(HS->GetUID(),std::bind(&UILayer::IncreaseScriptRef,HS),std::bind(&UILayer::DecreaseScriptRef,HS) );
            return lay;
        }

        return lay;
    }

    inline ASUILayer AddLayerFromScript(Field * m, QString name, bool can_remove)
    {
        ASUILayer lay;

        if(m != nullptr)
        {
            Field* s = m->GetCopy();
            UIFieldLayer *HS = new UIFieldLayer(s,QString(name),m->AS_FileName.size() == 0? false : true,m->AS_FileName,!can_remove);
            HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
            m_WorldWindow->AddUILayer(HS,true);

            lay.SetUID(HS->GetUID(),std::bind(&UILayer::IncreaseScriptRef,HS),std::bind(&UILayer::DecreaseScriptRef,HS) );
            return lay;
        }

        return lay;
    }

    inline ASUILayer AddLayerFromScript(RigidPhysicsWorld * m, QString name, bool can_remove)
    {
        ASUILayer lay;
        m->AS_AddRef();

        if(m != nullptr)
        {
            UIRigidWorldLayer *UIP = new UIRigidWorldLayer(m,QString(name),"",!can_remove);

            m_WorldWindow->AddUILayer(UIP,true);

            lay.SetUID(UIP->GetUID(),std::bind(&UILayer::IncreaseScriptRef,UIP),std::bind(&UILayer::DecreaseScriptRef,UIP) );

            return lay;
        }

        return lay;
    }

    inline ASUILayer AddLayerFromScript(ASEmitter *m, float time_scale, bool autoremove)
    {
        ASUILayer lay;


        if(m != nullptr)
        {
            UIParticleEffectLayer * uip = new UIParticleEffectLayer();
            uip->SetEmitter(m->GetEmitter());
            uip->SetAutoRemove(autoremove);
            uip->SetTimeScale(time_scale);
            lay.SetUID(uip->GetUID(),std::bind(&UILayer::IncreaseScriptRef,uip),std::bind(&UILayer::DecreaseScriptRef,uip) );

            return lay;
        }

    return lay;
    }



    inline void RemoveLayerFromScript(ASUILayer id)
    {
        if(id.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
            if(l != nullptr)
            {
                m_WorldWindow->RemoveUILayer(l,false);
            }
        }
    }

    inline void ReplaceLayerFromScript(ASUILayer id, cTMap * m)
    {
        if(m != nullptr && id.GetUID()  > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID() );
            if(l != nullptr)
            {

                if(l->layertypeName() == "RasterStreamLayer")
                {
                    UIStreamRasterLayer * lrs =  (UIStreamRasterLayer *) l;
                    if(lrs->IsLayerDirectReplaceable({{m}}))
                    {
                        m_WorldWindow->m_UILayerMutex.lock();
                        std::cout << "do direct replace " << std::endl;
                        lrs->DirectReplace({{m->GetCopy()}});

                        m_WorldWindow->SetRedrawNeeded();

                        m_WorldWindow->m_UILayerMutex.unlock();
                        return;
                    }
                }

                RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{m->GetCopy()}}),false,true);


                if(rdp->Exists())
                {
                    QString name =l->GetName();
                    UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,name,m->AS_FileName.size() == 0? false : true,m->AS_FileName,!false);
                    if(rdp->GetBandCount() > 1)
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
                    }else
                    {
                        ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
                    }

                    m_WorldWindow->ReplaceUILayer(l,ret);
                }else
                {

                    LISEMS_WARNING("Could not create raster layer to replace layer id " + QString::number(id.GetUID()));
                }

            }else
            {

                LISEMS_WARNING("Could not replace layer with id " + QString::number(id.GetUID()));
            }
        }
    }



    inline void ReplaceLayerFromScript(ASUILayer id, Field * m)
    {
        if(m != nullptr && id.GetUID()  > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID() );
            if(l != nullptr)
            {

                if(l->layertypeName() == "UIFieldLayer")
                {
                    UIFieldLayer * lrs =  (UIFieldLayer *) l;
                    if(lrs->IsLayerDirectReplaceable(m))
                    {
                        m_WorldWindow->m_UILayerMutex.lock();
                        std::cout << "do direct replace " << std::endl;
                        lrs->DirectReplace(m->GetCopy());

                        m_WorldWindow->SetRedrawNeeded();

                        m_WorldWindow->m_UILayerMutex.unlock();
                        return;
                    }
                }

                Field* s = m->GetCopy();

                QString name =l->GetName();
                UIFieldLayer *HS = new UIFieldLayer(s,QString(name),m->AS_FileName.size() == 0? false : true,m->AS_FileName,!false);
                HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
                m_WorldWindow->AddUILayer(HS,true);


                m_WorldWindow->ReplaceUILayer(l,HS);

            }else
            {

                LISEMS_WARNING("Could not replace layer with id " + QString::number(id.GetUID()));
            }
        }
    }

    inline void ReplaceLayerFromScript(ASUILayer id, ShapeFile * m)
    {
        if(m != nullptr && id.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
            if(l != nullptr)
            {
                ShapeFile * s = new ShapeFile();
                s->CopyFrom(m);
                UIVectorLayer *HS = new UIVectorLayer(s,QString(l->GetName()),m->AS_FileName.size() == 0? false : true,m->AS_FileName,l->IsNative());
                HS->SetStyle(l->GetStyle(),true);

                m_WorldWindow->ReplaceUILayer(l,HS);
            }
        }
    }

    inline ASUILayer GetLayerMap(QString name)
    {
        ASUILayer lay;
        QString nameq = name;
        if(nameq.isEmpty())
        {

            UILayer *l = m_WorldWindow->GetUILayerFromName(name);
            if(l != nullptr)
            {
                //if(l->GetMapCount()>0)
                {
                    lay.SetUID(l->GetUID(),std::bind(&UILayer::IncreaseScriptRef,l),std::bind(&UILayer::DecreaseScriptRef,l) );
                    return lay;
                }
            }
        }

        return lay;

    }


    inline cTMap * ScriptGetLayerMap(ASUILayer id)
    {
        if(id.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
            if(l != nullptr)
            {
                QList<cTMap *> list = l->GetMaps();
                if(list.length() >0)
                {
                    return list.at(0);
                }
            }

        }
        return new cTMap();
    }

    inline ShapeFile * ScriptGetLayerShapeFile(ASUILayer id)
    {
        if(id.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromID(id.GetUID());
            if(l != nullptr)
            {
                QList<ShapeFile *> list = l->GetShapeFiles();
                if(list.length() >0)
                {
                    return list.at(0);
                }
            }

        }
        return new ShapeFile();
    }




    int m_UPlotIDCount = 0;
    int m_UPlotLayerIDCount = 0;


    QMutex m_PlotMutex;

    std::vector<TablePlotter *> m_Plots;
    std::vector<int> m_PlotsIds;

    std::vector<int> m_PlotsToCreate;
    std::vector<QString> m_PlotsToCreateS;
    std::vector<int> m_PlotLayersToCreate;
    std::vector<int> m_PlotLayersToCreateP;
    std::vector<QString> m_PlotLayersToCreateS;
    std::vector<MatrixTable*> m_PlotLayersToCreateT;
    std::vector<int> m_PlotLayersToReplace;
    std::vector<MatrixTable*> m_PlotLayersToReplaceT;
    std::vector<int> m_PlotsToStyle;
    std::vector<LSMVector3> m_PlotsToStyleColor;



    inline void IncreaseScriptRefPlot(ASUIPlot* p)
    {

    }
    inline void DecreaseScriptRefPlot(ASUIPlot* p)
    {

    }
    inline void IncreaseScriptRefPlotLayer(ASUIPlotLayer* p)
    {

    }
    inline void DecreaseScriptRefPlotLayer(ASUIPlotLayer* p)
    {

    }

    inline ASUIPlot CreatePlot(QString name)
    {

        ASUIPlot lay;
        lay.SetUID(m_UPlotIDCount,std::bind(&MapViewTool::IncreaseScriptRefPlot,this,std::placeholders::_1),std::bind(&MapViewTool::DecreaseScriptRefPlot,this,std::placeholders::_1) );

        m_PlotMutex.lock();

        m_PlotsToCreate.push_back(m_UPlotIDCount);
        m_PlotsToCreateS.push_back(name);

        m_UPlotIDCount = m_UPlotIDCount +1;
        m_PlotMutex.unlock();


        emit int_on_create_plot();

        return lay;


    }

    inline ASUIPlotLayer AddPlotLayer(ASUIPlot p, MatrixTable * data, QString name)
    {
        ASUIPlotLayer lay;
        lay.SetUID(m_UPlotLayerIDCount,std::bind(&MapViewTool::IncreaseScriptRefPlotLayer,this,std::placeholders::_1),std::bind(&MapViewTool::DecreaseScriptRefPlotLayer,this,std::placeholders::_1) );

        m_PlotMutex.lock();

        m_PlotLayersToCreate.push_back(m_UPlotLayerIDCount);
        m_PlotLayersToCreateS.push_back(name);
        m_PlotLayersToCreateP.push_back(p.GetUID());
        m_PlotLayersToCreateT.push_back(data->Copy());
        m_UPlotLayerIDCount = m_UPlotLayerIDCount +1;

        m_PlotMutex.unlock();


        emit int_on_add_plot();

        return lay;

    }

    inline void ReplacePlotLayer(ASUIPlotLayer l, MatrixTable * data)
    {
        m_PlotMutex.lock();

        m_PlotLayersToReplace.push_back(l.GetUID());
        m_PlotLayersToReplaceT.push_back(data->Copy());


        m_PlotMutex.unlock();


        emit int_on_replace_plot();
    }

    inline void SetPlotLayerStyle(ASUIPlotLayer l, LSMVector3 color)
    {



        emit int_on_style_plot();
    }

    inline bool AS_IsViewLayerEdited(ASUILayer idl)
    {
        if(idl.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(idl.GetUID());
            if(l != nullptr)
            {
                if(l->IsBeingEdited())
                {
                    UILayerEditor * editor = m_WorldWindow->GetLayerEditor();
                    return editor->IsChangedSinceLastScriptCheck();
                }
            }
        }

        return false;
    }

    inline cTMap * AS_GetEditedMap(ASUILayer idl)
    {
        if(idl.GetUID() > -1)
        {
            UILayer * l = m_WorldWindow->GetUILayerFromUID(idl.GetUID());
            if(l != nullptr)
            {
                QList<cTMap * > maps = l->GetMaps();
                if(maps.size() > 0)
                {
                    if(l->IsBeingEdited())
                    {
                        cTMap * ret = maps.at(0)->GetCopy();

                        UILayerEditor * editor = m_WorldWindow->GetLayerEditor();

                        editor->FillMapWithCurrentEdit(ret);
                        return ret;
                    }
                }

            }
        }

        LISEMS_ERROR("Layer is not being edited");
        throw 1;
    }


    inline QString rf(QString path)
    {
        path = AS_DIR + path;

        QString res;

        QFile fin(path);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {
            return  res;
        }

        while (!fin.atEnd())
        {
            QString S = fin.readLine().trimmed();
            res.append("\n");
            res.append(S);
        }

        return res;
    }

    inline void AddArrowLayer()
    {


    }

    inline bool IsMouseInScreen()
    {
         return m_WorldWindow->GetCurrentMouseState().in;
    }

    inline bool IsMouseButtonPressed(QString name)
    {
        if(name.compare("Left",Qt::CaseInsensitive) == 0)
        {
            return m_WorldWindow->GetCurrentMouseState().Button_Left_Pressed;
        }
        if(name.compare("Right",Qt::CaseInsensitive) == 0)
        {
            return m_WorldWindow->GetCurrentMouseState().Button_Right_Pressed;
        }
        if(name.compare("Middle",Qt::CaseInsensitive) == 0)
        {
            return m_WorldWindow->GetCurrentMouseState().Button_Middle_Pressed;
        }

        return false;
    }

    inline bool IsKeyPressed(QString name)
    {
        //all glfw key codes with names
        std::vector<QString> keynames = {
            "Space",
                        "Apostrophe",
                        "Comma",
                        "Minus",
                        "Period",
                        "Slash",
                        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                        "Semicolon","Equal",
                        "A","B","C","D","E","F","G","H", "i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
                        "LBracket","Backslash","RBracket","Grave","WOrld1","World2",
                        "Escape","enter","tab","backspace","insert","delete","right","left","down","up",
                        "page up","page down","home","end","capslock","scrollock","numlock","printscreen","pause",
                        "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24","F25",
                        "KP 0", "KP 1", "KP 2", "KP 3", "KP 4", "KP 5", "KP 6", "KP 7", "KP 8","KP 9",
                        "KP Decimal","KP divide","KP multiply","kp subtract","kp add","kp enter","kp equal",
                        "LShift","LControl","LAlt","LSuper","RShift","RControl","RAlt","RSuper","Menu"


        };
        std::vector<int> keyindices =
        {
            32,39,44,45,46,47,48,49,50,51,52,53,54,55,56,57,59,61,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
                        80,81,82,83,84,85,86,87,88,89,90,91,92,93,96,161,162,256,257,258,259,260,261,262,263,264,265,266,267,268,269,280,291,282,283,284,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,
                        309,310,311,312,313,314,320,321,322,323,324,325,326,327,328,329,330,331,332,333,
                        334,335,336,340,341,342,343,344,345,346,347,348

        };


        for(int i = 0; i < keynames.size(); i++)
        {
            if(keynames.at(i).compare(name,Qt::CaseInsensitive) == 0)
            {

                return glfwGetKey(m_WorldWindow->m_OpenGLCLManager->window,keyindices.at(i)) ==GLFW_PRESS;
            }
        }

        return false;

    }

    inline LSMVector2 MousePosition()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowState();

        return LSMVector2(s.MousePosX/s.scr_width,s.MousePosY/s.scr_height);
    }

    inline LSMVector3 MouseGeoPosition()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowStateSafe();

        return LSMVector3(s.MouseGeoPosX,s.MouseGeoPosY,s.MouseGeoPosZ);
    }

    inline LSMVector3 CameraPosition()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowStateSafe();
        return s.Camera3D->GetPosition();
    }

    inline LSMVector3 ViewDirection()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowStateSafe();
        return s.Camera3D->GetViewDir();
    }

    inline BoundingBox Camera2DView()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowStateSafe();
        return BoundingBox(s.tlx,s.brx,s.tly,s.bry);
    }
    inline void SetCamera2DView(BoundingBox b)
    {
        m_WorldWindow->LookAtbbSafe(b);
    }
    inline void SetCamera3DView(LSMVector3 pos, LSMVector3 viewdir)
    {
        m_WorldWindow->Set3DViewSafe(pos,viewdir);
    }


    inline bool IsCamera3D()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowState();
        return s.is_3d;
    }

    inline void SetUI(bool edge, bool legends)
    {
        m_WorldWindow->SetLegendDraw(legends);
        m_WorldWindow->SetUIDraw(edge);
    }

    inline void SetUISize(float scale)
    {
        m_WorldWindow->SetUIScale(scale);
    }

    inline LSMVector2 GetScreenResolution()
    {
        GeoWindowState s = m_WorldWindow->GetCurrentWindowState();

        return LSMVector2(s.scr_width,s.scr_height);
    }
    void DeleteAllScriptLayers()
    {
        m_WorldWindow->RemoveAllUserLayers();
    }

    void DeleteAllLayers()
    {
        m_WorldWindow->RemoveAllUserLayers();
    }

    inline ASUILayer AddTextLayer(QString text, LSMVector3 Position, float size, LSMVector4 Color, GeoProjection *P, bool rel = false, bool center = false)
    {
        std::cout << "add text layer " << center << std::endl;
        ASUILayer ret;


        {
            UITextLayer * lay = new UITextLayer(text,QString(""),Position,*P,size,Color,false,false,false,rel,false,false,center);
            m_WorldWindow->AddUILayer(lay,true,false);


            ret.SetUID(lay->GetUID(),std::bind(&UILayer::IncreaseScriptRef,lay),std::bind(&UILayer::DecreaseScriptRef,lay) );

        }

        return ret;
    }


    inline void SetTextShadow(ASUILayer id, bool active, LSMVector4 color,float size_incr)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "TextLayer")
            {
                ((UITextLayer *)(l))->SetShadow(active,size_incr,color);

            }

        }
    }

    inline ASUILayer AddTextLayer(QString text, LSMVector3 Position, float size, LSMVector4 Color, GeoProjection *P, bool screencoords, bool size_rel, LSMVector4 box_color, bool center = false)
    {
        ASUILayer ret;


        {
            //UITextLayer * lay = new UITextLayer(text,QString(""),Position,P,size,false,false,false,screencoords,screencoords,size_rel,box_color.w > 0.0001,box_color);
            //m_WorldWindow->AddUILayer(lay,true,false);

            //ret.SetUID(lay->GetUID(),std::bind(&UILayer::IncreaseScriptRef,lay),std::bind(&UILayer::DecreaseScriptRef,lay) );

        }

        return ret;
    }

    inline void SetText(ASUILayer id, QString text)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "TextLayer")
            {
                ((UITextLayer *)(l))->SetText(text);

            }

        }

    }
    inline void SetTextColor(ASUILayer id, LSMVector4 color)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "TextLayer")
            {
                ((UITextLayer *)(l))->SetTextColor(color);

            }
        }
    }
    inline void SetTextPosition(ASUILayer id, LSMVector3 pos,GeoProjection *p, bool rel = false)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "TextLayer")
            {
                ((UITextLayer *)(l))->SetTextPosition(pos,p);

            }
        }
    }

    inline void SetTextSize(ASUILayer id, float size)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "TextLayer")
            {
                ((UITextLayer *)(l))->SetTextSize(size);

            }
        }
    }

    inline ASUILayer AddImageLayer(QString file, BoundingBox pos, GeoProjection *p, bool rel = false,int fillmode = 0)
    {
        ASUILayer lay;

        //if(m != nullptr && m2 != nullptr)
        {

            UIImageLayer * ret = new UIImageLayer(file,pos,*p,rel,fillmode);

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;



    }
    inline ASUILayer AddImageLayer2(cTMap * r, cTMap * g, cTMap * b, cTMap * a, BoundingBox pos, GeoProjection *p, bool rel = false, int fillmode = 0)
    {
        ASUILayer lay;

        //if(m != nullptr && m2 != nullptr)
        {

            UIImageLayer * ret = new UIImageLayer(r->GetCopy(),g->GetCopy(),b->GetCopy(),a->GetCopy(),pos,*p,rel,fillmode);

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }

    inline void SetImagePosition(ASUILayer id,BoundingBox pos, GeoProjection *p, bool rel = false )
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ImageLayer")
            {
                ((UIImageLayer *)(l))->setPosition(pos,*p,rel);

            }
        }
    }

    inline void SetImagePositionRotation(ASUILayer id,BoundingBox pos, float angle, GeoProjection *p, bool rel = false )
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ImageLayer")
            {
                ((UIImageLayer *)(l))->setPositionRotation(pos,angle,*p,rel);

            }
        }
    }
    inline void SetImageImage(ASUILayer id, QString file)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ImageLayer")
            {
                ((UIImageLayer *)(l))->setImage(file);

            }
        }
    }
    inline void SetImageImage2(ASUILayer id, cTMap * r, cTMap * g, cTMap * b, cTMap * a)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ImageLayer")
            {
                ((UIImageLayer *)(l))->setImage(r->GetCopy(),g->GetCopy(),b->GetCopy(),a->GetCopy());

            }
        }
    }



    inline ASUILayer AddCustomShader1(QString text,  std::vector<cTMap*>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection p = GeoProjection::GetGeneric(),bool isabs = false, bool scaling = true, bool is2d = true)
    {
        ASUILayer lay;

        //if(m != nullptr && m2 != nullptr)
        {

            UIShaderLayer * ret = new UIShaderLayer({text},{{}},{textures},region,p, isabs,!is2d,resolutionx, resolutiony,true,scaling );

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }


    inline ASUILayer AddCustomShader2(QString text,  std::vector<QString>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection p = GeoProjection::GetGeneric(),bool isabs = false, bool scaling = true, bool is2d = true)
    {
        ASUILayer lay;



        //if(m != nullptr && m2 != nullptr)
        {
            UIShaderLayer * ret = new UIShaderLayer({text},{textures},{{}},region,p, isabs,!is2d,resolutionx, resolutiony,true,scaling );

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }

    inline ASUILayer AddCustomShader1mp(std::vector<QString> text,  std::vector<std::vector<cTMap*>>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony =-1, GeoProjection p = GeoProjection::GetGeneric(),bool isabs = false, bool scaling = true, bool is2d = true)
    {
        ASUILayer lay;

        //if(m != nullptr && m2 != nullptr)
        {

            UIShaderLayer * ret = new UIShaderLayer(text,{{}},{textures},region,p, isabs,!is2d,resolutionx, resolutiony,true,scaling );

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }


    inline ASUILayer AddCustomShader2mp(std::vector<QString> text,  std::vector<std::vector<QString>> textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1,float resolutiony = -1, GeoProjection p = GeoProjection(),bool isabs = false, bool scaling = true, bool is2d = true)
    {
        ASUILayer lay;

        //if(m != nullptr && m2 != nullptr)
        {

            UIShaderLayer * ret = new UIShaderLayer({text},textures,{{}},region,p, isabs,!is2d,resolutionx, resolutiony,true,scaling );

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }

    inline ASUILayer AddParticleEffectLayer(ASEmitter*)
    {
        ASUILayer lay;
        {

            UIParticleEffectLayer * ret = new UIParticleEffectLayer();

            m_WorldWindow->AddUILayer(ret,true);

            lay.SetUID(ret->GetUID(),std::bind(&UILayer::IncreaseScriptRef,ret),std::bind(&UILayer::DecreaseScriptRef,ret));

        }

        return lay;
    }

    inline void SetParticleEffectLayerReset()
    {

    }

    inline void HasParticleCollidedWith()
    {

    }

    inline void OpenFullScreen(bool open = true, bool alt = false)
    {

        CGlobalGLCLManager->RequestToggleFullscreen(open,alt);

    }
    inline void SetAllowShift(bool x)
    {
        m_WorldWindow->SetAllowShift(x);
    }

    inline void SetShaderAutoScale(ASUILayer id, bool scale)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ShaderLayer")
            {
                ((UIShaderLayer *)(l))->SetAutoScale(scale);
            }
        }
    }

    inline void SetLayerStyleHillShade(ASUILayer id, bool x, float angle_1, float angle_2)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            l->GetStyleRef()->angle_hor = angle_2;
            l->GetStyleRef()->angle_vert = angle_1;
            l->GetStyleRef()->m_IsHS = true;


            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }
    }

    inline void SetLayerFillStyle(ASUILayer id,  LSMVector4 color1,float size, float seperation, float angle,LSMVector4 color2,int filltype)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            SPHFillStyle s = GetDefaultFillStyle(LISEM_FILLSTYLE_BLACK);

            s.m_Color1 = ColorF(color1.x,color1.y,color1.z,color1.w);
            s.m_Color2 = ColorF(color2.x,color2.y,color2.z,color2.w);

            s.m_IsLines = filltype == 1 ;
            s.m_LineSize = size;
            s.m_LineAngle = seperation;
            s.m_LineSeperation = angle;
            s.m_IsShapes = filltype == 2 || filltype == 3;
            s.m_ShapeType = filltype == 2?LISEM_FILLSHAPE_CIRCLE : LISEM_FILLSHAPE_SQUARE;
            s.m_ShapeSize = size;
            s.m_ShapeSeperation = seperation;
            s.m_HasBackGroundColor = color1.w > 0.0;



            l->GetStyleRef()->m_PointFillStyle = s;
            l->GetStyleRef()->m_PolygonFillStyle = s;

            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }
    }


    inline void SetLayerLineStyle(ASUILayer id, LSMVector4 color1,float size,LSMVector4 color2,int filltype)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            LSMLineStyle s = GetDefaultLineStyle(LISEM_LINESTYLE_BLACK);


            s.m_Color1 = ColorF(color1.x,color1.y,color1.z,color1.w);
            s.m_Color2 = ColorF(color2.x,color2.y,color2.z,color2.w);

            s.m_HasBackGroundColor = color1.w > 0.0? true:false;
            s.m_HasPattern = filltype > 0?true:false;
            if(s.m_HasPattern)
            {
                s.pattern = {1.0,1.0};
            }else
            {
                s.pattern = {};
                s.m_Color2 = s.m_Color1;
            }
            s.m_LineWMax =size;
            s.m_ShapeType = filltype > 0?LISEM_LINESHAPE_SQUARE:0;

            l->GetStyleRef()->m_LineLineStyle = s;
            l->GetStyleRef()->m_PointLineStyle = s;
            l->GetStyleRef()->m_PolygonLineStyle = s;



            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }

    }

    inline void SetLayerTransparency(ASUILayer id, float trans)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            l->GetStyleRef()->SetTransparancy(1.0-trans);


            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }

    }
    /*from lsmcolorgradient.h
#define LISEM_GRADIENT_WH 0
#define LISEM_GRADIENT_HS 1
#define LISEM_GRADIENT_UI 12 //Marianne Rainbow
#define LISEM_GRADIENT_RYG 3
#define LISEM_GRADIENT_ELEVATION 4
#define LISEM_GRADIENT_RBGB 5
#define LISEM_GRADIENT_BLUES 6
#define LISEM_GRADIENT_REDS 7
#define LISEM_GRADIENT_MAGMA 8
#define LISEM_GRADIENT_WH_FLUIDS 9
#define LISEM_GRADIENT_WH_SOLIDS 10
#define LISEM_GRADIENT_VEL_WHITE 11
#define LISEM_GRADIENT_UI_USER 2
*/

    inline void SetLayerStyleColorGradient(ASUILayer id,QString name)
    {


        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            if(name.compare("Marianne",Qt::CaseInsensitive) == 0 || name.compare("ui",Qt::CaseInsensitive) == 0  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
            }else if(name.compare("HS",Qt::CaseInsensitive) == 0 || name.compare("Hillshade",Qt::CaseInsensitive) == 0 || name.compare("GreyScale",Qt::CaseInsensitive) == 0 || name.compare("GrayScale",Qt::CaseInsensitive) == 0  )
            {
                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_HS);
            }else if(name.compare("Water",Qt::CaseInsensitive) == 0 || name.compare("Rain",Qt::CaseInsensitive) == 0 || name.compare("Flow",Qt::CaseInsensitive) == 0  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH);

            }else if(name.compare("Elevation",Qt::CaseInsensitive) == 0 )
            {
                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_ELEVATION);

            }else if(name.compare("RBGB",Qt::CaseInsensitive) == 0  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_RBGB);
            }else if(name.compare("BLUES",Qt::CaseInsensitive) == 0 || name.compare("Blue",Qt::CaseInsensitive) == 0  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_BLUES);
            }else if(name.compare("REDS",Qt::CaseInsensitive) == 0 || name.compare("Red",Qt::CaseInsensitive) == 0)
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_REDS);
            }else if(name.compare("MAGMA",Qt::CaseInsensitive) == 0 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_MAGMA);

            }else if(name.compare("FLUIDS",Qt::CaseInsensitive) == 0 || name.compare("Fluid",Qt::CaseInsensitive) == 0  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH_FLUIDS);

            }else if(name.compare("SOLIDS",Qt::CaseInsensitive) == 0 || name.compare("Solid",Qt::CaseInsensitive) == 0 || name.compare("Mud",Qt::CaseInsensitive) == 0 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH_SOLIDS);


            }else if(name.compare("RYG",Qt::CaseInsensitive) == 0 || name.compare("Hazard",Qt::CaseInsensitive) == 0 || name.compare("Trafficlight",Qt::CaseInsensitive) == 0 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_RYG);

            }else if(name.compare("White",Qt::CaseInsensitive) == 0 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_VEL_WHITE);
            }else
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
            }


            l->SetStyleChangedFromExternalThread(false);
            l->GetStyleMutex()->unlock();
        }

    }
    inline void SetLayerStyleColorGradient(ASUILayer id,int index)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();


            if(index == 0 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
            }else if(index == 1 )
            {
                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_HS);
            }else if(index == 2 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH);

            }else if(index == 3 )
            {
                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_ELEVATION);

            }else if(index == 4  )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_RBGB);
            }else if(index == 5 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_BLUES);
            }else if(index == 6)
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_REDS);
            }else if(index == 7)
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_MAGMA);

            }else if(index == 8)
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH_FLUIDS);

            }else if(index == 9 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH_SOLIDS);


            }else if(index == 10 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_RYG);

            }else if(index == 11 )
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_VEL_WHITE);
            }else
            {

                l->GetStyleRef()->m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
            }


            l->SetStyleChangedFromExternalThread(false);
            l->GetStyleMutex()->unlock();
        }
    }

    inline void SetLayerStyleColorGradient(ASUILayer id,std::vector<float> stops, std::vector<LSMVector4> colors)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            LSMColorGradient grad;

            {
                for(int i = 0; i < stops.size(); i++)
                {
                    grad.m_Gradient_Stops.append(stops[i]);
                    if(i < colors.size())
                    {
                        grad.m_Gradient_Colors.append(ColorF(colors[i].x,colors[i].y,colors[i].z,colors[i].w));
                    }else
                    {
                        grad.m_Gradient_Colors.append(ColorF(0.0,0.0,0.0,0.0));
                    }

                }
            }
            l->GetStyleRef()->m_ColorGradientb1 = grad;
            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }
    }

    inline void SetLayerValueRange(ASUILayer id, double min, double max, int band)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            l->GetStyleMutex()->lock();

            if(band == 3)
            {
                l->GetStyleRef()->m_Intervalb3.SetMax(max);
                l->GetStyleRef()->m_Intervalb3.SetMin(min);
            }else if(band == 2)
            {
                l->GetStyleRef()->m_Intervalb2.SetMax(max);
                l->GetStyleRef()->m_Intervalb2.SetMin(min);
            }else
            {
                l->GetStyleRef()->m_Intervalb1.SetMax(max);
                l->GetStyleRef()->m_Intervalb1.SetMin(min);
            }

            l->SetStyleChangedFromExternalThread(false);

            l->GetStyleMutex()->unlock();
        }



    }


    inline void SetShaderPosition(ASUILayer id, BoundingBox b, GeoProjection * p, bool is_rel)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ShaderLayer")
            {
                ((UIShaderLayer *)(l))->setPosition(b,p,is_rel);
            }
        }
    }


    inline void SetShaderInputFloats(ASUILayer id, int i, float a, float b, float c, float d)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ShaderLayer")
            {
                ((UIShaderLayer *)(l))->SetInputFloats(i,a,b,c,d);
            }
        }
    }


    inline void SetShaderInputImage(ASUILayer id, QString image, int i, int pass)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ShaderLayer")
            {
                ((UIShaderLayer *)(l))->SetInputImage(image,i,pass);
            }
        }
    }

    inline void SetShaderInputMap(ASUILayer id, cTMap * image,int i, int pass)
    {
        UILayer * l = m_WorldWindow->GetUILayerFromUID(id.GetUID());
        if(l != nullptr)
        {
            if(l->layertypeName() == "ShaderLayer")
            {
                ((UIShaderLayer *)(l))->SetInputMap(image,i,pass);
            }
        }
    }

    int m_LastScriptCounter = -1;

    inline void CallBackMouseScript(asIScriptFunction * f)
    {
        int sc = GetScriptCounter();
        if(sc != m_LastScriptCounter)
        {
            //clear the inputs list
            m_WorldWindow->m_SM.lock();
            m_WorldWindow->m_MouseState.MouseButtonSEventsShift.clear();
            m_WorldWindow->m_MouseState.MouseButtonSEvents.clear();
            m_WorldWindow->m_MouseState.MouseButtonKeySAction.clear();
            m_WorldWindow->m_MouseState.MouseButtonSPosX.clear();
            m_WorldWindow->m_MouseState.MouseButtonSPosY.clear();
            m_LastScriptCounter = sc;
            m_WorldWindow->m_SM.unlock();

        }else
        {
            //use whats there in the list
            m_WorldWindow->m_SM.lock();

            for(int i = 0; i < m_WorldWindow->m_MouseState.MouseButtonSEventsShift.size(); i++)
            {

                bool has_shift = m_WorldWindow->m_MouseState.MouseButtonSEventsShift.at(i);
                int key_id = m_WorldWindow->m_MouseState.MouseButtonSEvents.at(i);
                bool press = m_WorldWindow->m_MouseState.MouseButtonKeySAction.at(i) == GLFW_PRESS;
                double posx = m_WorldWindow->m_MouseState.MouseButtonSPosX.at(i);
                double posy = m_WorldWindow->m_MouseState.MouseButtonSPosY.at(i);


                asIScriptContext *ctx = asGetActiveContext();

                int ps =  ctx->PushState();

                // Store the current context state so we can restore it later
                  if( ctx && ps > -1 )
                  {
                    // Use the context normally, e.g.
                    //  ctx->Prepare(...);
                    //  ctx->Execute(...);



                    asIScriptEngine *engine = ctx->GetEngine();

                    ctx->Prepare(f);

                    QString key;
                    if(key_id == GLFW_MOUSE_BUTTON_1)
                    {
                        key = "Left";
                    }
                    if(key_id == GLFW_MOUSE_BUTTON_2)
                    {
                        key = "Right";
                    }
                    if(key_id == GLFW_MOUSE_BUTTON_3)
                    {
                        key = "Middle";
                    }

                    // Set the function arguments
                    ctx->SetArgObject(0,&key);
                    ctx->SetArgDouble(1,posx);
                    ctx->SetArgDouble(2,posy);
                    ctx->SetArgByte(3,press);
                    ctx->SetArgByte(4,has_shift);

                    int r = ctx->Execute();
                    if( r == asEXECUTION_FINISHED )
                    {

                    }else {

                        ctx->PopState();
                        LISEMS_ERROR("Error when calling optimizing callback function!");
                        throw 1;
                    }



                    // Once done, restore the previous state
                    ctx->PopState();
                  }else {

                      LISEMS_ERROR("Error when calling optimizing callback function, could not push state!");
                      throw 1;
                  }



            }


            m_WorldWindow->m_MouseState.MouseButtonSEventsShift.clear();
            m_WorldWindow->m_MouseState.MouseButtonSEvents.clear();
            m_WorldWindow->m_MouseState.MouseButtonKeySAction.clear();
            m_WorldWindow->m_MouseState.MouseButtonSPosX.clear();
            m_WorldWindow->m_MouseState.MouseButtonSPosY.clear();
            m_WorldWindow->m_SM.unlock();
        }


        //m_MouseState.MouseButtonSEventsShift.append(false);
        //m_MouseState.MouseButtonSEvents.append(key);
        //m_MouseState.MouseButtonKeySAction.append(action);
        //m_MouseState.m_SM.unlock();

        //we set a user-data item on the scriptfunction


    }

    inline void CallBackKeyScript(asIScriptFunction * f)
    {

        //all glfw key codes with names
        std::vector<QString> keynames = {
            "Space",
                        "Apostrophe",
                        "Comma",
                        "Minus",
                        "Period",
                        "Slash",
                        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                        "Semicolon","Equal",
                        "A","B","C","D","E","F","G","H", "i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
                        "LBracket","Backslash","RBracket","Grave","WOrld1","World2",
                        "Escape","enter","tab","backspace","insert","delete","right","left","down","up",
                        "page up","page down","home","end","capslock","scrollock","numlock","printscreen","pause",
                        "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24","F25",
                        "KP 0", "KP 1", "KP 2", "KP 3", "KP 4", "KP 5", "KP 6", "KP 7", "KP 8","KP 9",
                        "KP Decimal","KP divide","KP multiply","kp subtract","kp add","kp enter","kp equal",
                        "LShift","LControl","LAlt","LSuper","RShift","RControl","RAlt","RSuper","Menu"


        };
        std::vector<int> keyindices =
        {
            32,39,44,45,46,47,48,49,50,51,52,53,54,55,56,57,59,61,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
                        80,81,82,83,84,85,86,87,88,89,90,91,92,93,96,161,162,256,257,258,259,260,261,262,263,264,265,266,267,268,269,280,291,282,283,284,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,
                        309,310,311,312,313,314,320,321,322,323,324,325,326,327,328,329,330,331,332,333,
                        334,335,336,340,341,342,343,344,345,346,347,348

        };

        int sc = GetScriptCounter();
        if(sc != m_LastScriptCounter)
        {
            //clear the inputs list
            m_WorldWindow->m_SM.lock();
            m_WorldWindow->m_MouseState.KeySEventShift.clear();
            m_WorldWindow->m_MouseState.KeySEvents.clear();
            m_WorldWindow->m_MouseState.KeySAction.clear();
            m_WorldWindow->m_MouseState.KeySMods.clear();
            m_LastScriptCounter = sc;
            m_WorldWindow->m_SM.unlock();

        }else
        {
            //use whats there in the list
            m_WorldWindow->m_SM.lock();

            for(int i = 0; i < m_WorldWindow->m_MouseState.KeySEventShift.size(); i++)
            {

                bool has_shift = m_WorldWindow->m_MouseState.KeySEventShift.at(i);
                int key_id = m_WorldWindow->m_MouseState.KeySEvents.at(i);
                bool press = m_WorldWindow->m_MouseState.KeySAction.at(i) == GLFW_PRESS;
                int mods = m_WorldWindow->m_MouseState.KeySMods.at(i);


                QString key = "";
                for(int i = 0; i < keynames.size(); i++)
                {
                    if(key_id == keyindices.at(i))
                    {
                        key = keynames.at(i);
                    }
                }



                asIScriptContext *ctx = asGetActiveContext();

                int ps =  ctx->PushState();

                // Store the current context state so we can restore it later
                  if( ctx && ps > -1 )
                  {
                    // Use the context normally, e.g.
                    //  ctx->Prepare(...);
                    //  ctx->Execute(...);



                    asIScriptEngine *engine = ctx->GetEngine();

                    ctx->Prepare(f);


                    // Set the function arguments
                    ctx->SetArgObject(0,&key);
                    ctx->SetArgDouble(1,m_WorldWindow->GetCurrentWindowState().MousePosX);
                    ctx->SetArgDouble(2,m_WorldWindow->GetCurrentWindowState().MousePosY);
                    ctx->SetArgByte(3,press);
                    ctx->SetArgByte(4,has_shift);

                    int r = ctx->Execute();
                    if( r == asEXECUTION_FINISHED )
                    {

                    }else {

                        ctx->PopState();
                        LISEMS_ERROR("Error when calling optimizing callback function!");
                        throw 1;
                    }



                    // Once done, restore the previous state
                    ctx->PopState();
                  }else {

                      LISEMS_ERROR("Error when calling optimizing callback function, could not push state!");
                      throw 1;
                  }



            }


            m_WorldWindow->m_MouseState.KeySEventShift.clear();
            m_WorldWindow->m_MouseState.KeySEvents.clear();
            m_WorldWindow->m_MouseState.KeySAction.clear();
            m_WorldWindow->m_MouseState.KeySMods.clear();
            m_WorldWindow->m_SM.unlock();
        }

        //m_MouseState.KeySEventShift.append(false);
        //m_MouseState.KeySEvents.append(key);
        //m_MouseState.KeySAction.append(action);
        //m_MouseState.KeySMods.append(mods);
        //m_MouseState.m_SM.unlock();
    }


    inline void LimitWorldWindowView2D(BoundingBox b, float scale)
    {

        m_WorldWindow->SetLimitedPosition2D(b,scale);
    }

    inline void FixWorldWindowView2D(BoundingBox b)
    {

        m_WorldWindow->SetFixedCamera2D(b);
    }

    inline void SetFreeView(bool x)
    {

        m_WorldWindow->SetFreeCamera(x);
    }



    void SetScriptFunctions(ScriptManager * sm);



    inline void Internal_CreateAllPlots()
    {

        m_PlotMutex.lock();


        for(int i = 0; i < m_PlotsToCreate.size(); i++)
        {
            MatrixTable * tbl = new MatrixTable();


            TablePlotter * t = new TablePlotter(tbl,true,false);
            t->show();

            m_Plots.push_back(t);
            m_PlotsIds.push_back(m_PlotsToCreate.at(i));
        }

        m_PlotsToCreate.clear();
        m_PlotsToCreateS.clear();

        m_PlotMutex.unlock();

    }

    inline void Internal_CreateAllPlotLayers()
    {

        m_PlotMutex.lock();

        for(int i = 0; i < m_PlotLayersToCreate.size(); i++)
        {
            MatrixTable * tbl = m_PlotLayersToCreateT.at(i);
            QString name = m_PlotLayersToCreateS.at(i);
            int table = m_PlotLayersToCreateP.at(i);
            int id = m_PlotLayersToCreate.at(i);


            for(int j = 0; j < m_Plots.size(); j++)
            {
                TablePlotter * p = m_Plots.at(j);

                if(m_PlotsIds.at(j) == table)
                {

                    //actually add table to plotter

                    p->AddMatrixTable(tbl,true, id);
                    break;

                }
            }

        }

        m_PlotLayersToCreateT.clear();
        m_PlotLayersToCreateS.clear();
        m_PlotLayersToCreateP.clear();
        m_PlotLayersToCreate.clear();

        m_PlotMutex.unlock();

    }


    inline void Internal_ReplaceAllPlotLayers()
    {

        m_PlotMutex.lock();


        for(int i = 0; i < m_PlotLayersToReplace.size(); i++)
        {
            for(int j = 0; j < m_Plots.size(); j++)
            {
                TablePlotter * p = m_Plots.at(j);

                if(p->ReplaceMatrixTable(m_PlotLayersToReplaceT.at(i),true,m_PlotLayersToReplace.at(i)))
                {
                    break;
                }
            }

        }

        m_PlotLayersToReplace.clear();
        m_PlotLayersToReplaceT.clear();

        m_PlotMutex.unlock();
    }




signals:

    void int_on_create_plot();
    void int_on_add_plot();
    void int_on_replace_plot();
    void int_on_style_plot();



public slots:

    inline void onBackGroundColorChanged()
    {

        LSMVector4 color =  m_BackGroundColor->GetColor();
        std::cout << "set color " << color.x << " " << color.y << " " << color.z << std::endl;
        m_WorldWindow->SetBackGroundColor(color);
    }

    inline void OnCreatePlot()
    {
         Internal_CreateAllPlots();
    }

    inline void OnCreatePlotLayer()
    {
        Internal_CreateAllPlots();
        Internal_CreateAllPlotLayers();
    }

    inline void OnReplacePlotLayer()
    {
        Internal_CreateAllPlots();
        Internal_CreateAllPlotLayers();
        Internal_ReplaceAllPlotLayers();
    }

    inline void OnSetPlotLayerStyle()
    {


    }

    inline void OnSelectedLayerChanged()
    {

        QModelIndex i = m_Layers->currentIndex();

        if(i.row() == -1)
        {
            m_WorldWindow->SetFocusLayer(nullptr);
        }else
        {
            LayerWidget * lw = (LayerWidget *)m_Layers->itemWidget(m_Layers->item(i.row()));
            m_WorldWindow->SetFocusLayer(lw->GetMapLayer());
        }

    }


    inline void OnCRSPressed()
    {
        CRSSelectWidget * select = new CRSSelectWidget();
        select->SetCurrentCRS(m_WorldWindow->GetCurrentProjection());
        int res = select->exec();

        if(res == QDialog::Accepted){
               GeoProjection p = select->GetCurrentProjection();
               m_WorldWindow->SetCurrentProjection(p);
               CRSLabel->setText("CRS: " + p.GetName());

        }

        delete select;
    }

    inline void OnAddPressed()
    {
        QString openDir = GetSite();

        QStringList path = QFileDialog::getOpenFileNames(this,
                                            QString("Select map"),
                                            openDir,
                                            QString("*.tif;*.map;*.*"));

        bool is_single = (path.size() == 1);

        if(!path.isEmpty())
        {
            QString errorpath;
            bool exists = true;
            qint64 size = 0;
            bool has_1band = false;
            for(int i = 0; i < path.length(); i++)
            {
                QFileInfo fileInfo(path.at(i));
                if(!fileInfo.exists())
                {
                    exists = false;
                    errorpath = path.at(i);
                    break;
                }
                size += fileInfo.size();

                if(path.size() == 2)
                {
                    cTMapProps props = readRasterProps(path.at(i));
                    if(props.bands != 1)
                    {
                        has_1band = false;
                    }
                }
            }

            bool is_duo = (path.size() == 2) && has_1band;
            bool is_ts = (!is_single)&&(!is_duo);

            if(exists)
            {
                if(is_single)
                {
                    qint64 size_max = MAX_MAP_MEM_MB* 1024*1024;
                    std::cout << "size check " << size << " " <<  size_max << std::endl;
                    if(size > size_max)
                    {
                        AddStreamLayerFromFile(path.at(0));
                    }else {
                        AddLayerFromFile(path.at(0));
                    }

                }else if(is_duo)
                {
                    AddDuoLayerFromFile(path);
                }else if(is_ts)
                {
                    AddTimeSeriesLayerFromFile(path);
                }

            }else
            {
                QMessageBox::critical(this, "SPHazard",
                   QString("File \"%1\" can not be openend.")
                       .arg(errorpath));
            }
        }
    }

    inline void OnAddRasterMemoryPressed()
    {
        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select map"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());

            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());

                AddLayerFromFile(path);
            }else
            {
                QMessageBox::critical(this, "SPHazard",
                   QString("File \"%1\" can not be openend.")
                       .arg(path));
            }
        }

    }


    inline void OnAddRasterStreamPressed()
    {
        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select map"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());

            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddStreamLayerFromFile(path);
            }else
            {
                QMessageBox::critical(this, "SPHazard",
                   QString("File \"%1\" can not be openend.")
                       .arg(path));
            }
        }

    }



    inline void OnAddRasterTimeSeriesPressed()
    {
        QString openDir = GetSite();

        QStringList path = QFileDialog::getOpenFileNames(this,
                                            QString("Select map"),
                                            openDir,
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
            QString errorpath;
            bool exists = true;
            for(int i = 0; i < path.length(); i++)
            {

                QFileInfo fileInfo(path.at(i));
                if(!fileInfo.exists())
                {
                    exists = false;
                    errorpath = path.at(i);
                    break;
                }
            }

            if(exists)
            {
                AddTimeSeriesLayerFromFile(path);
            }else
            {
                QMessageBox::critical(this, "SPHazard",
                   QString("File \"%1\" can not be openend.")
                       .arg(errorpath));
            }
        }
    }



    inline void OnAddRasterDuoPressed()
    {
        QString openDir = GetSite();

        QStringList path = QFileDialog::getOpenFileNames(this,
                                            QString("Select map"),
                                            openDir,
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
            QString errorpath;
            bool exists = true;
            for(int i = 0; i < path.length(); i++)
            {

                QFileInfo fileInfo(path.at(i));
                if(!fileInfo.exists())
                {
                    exists = false;
                    errorpath = path.at(i);
                    break;
                }
            }

            if(exists)
            {
                AddDuoLayerFromFile(path);
            }else
            {
                QMessageBox::critical(this, "SPHazard",
                   QString("File \"%1\" can not be openend.")
                       .arg(errorpath));
            }
        }

    }


    inline void OnAddRasterNewPressed()
    {

        MapCreateWidget * creator = new MapCreateWidget();
        int res = creator->exec();

        if(res == QDialog::Accepted){
               MapConstructInfo i = creator->GetMapInfo();
               cTMap * map = creator->CreateMap(i);

               if(map != nullptr)
               {
                   if(i.save)
                   {
                        AS_SaveMapToFileAbsolute(map,i.savename);

                       if(i.add_to_view)
                       {
                            AddVectorLayerFromFile(i.savename);
                       }else
                       {
                            delete map;
                       }
                   }else
                   {
                        if(i.add_to_view)
                        {
                            AddLayerFromScript(map,"new raster",true);
                        }else
                        {
                            delete map;
                        }
                   }
               }
        }

        delete creator;
    }

    inline void OnAddVNewPressed()
    {

        VectorCreateWidget * creator = new VectorCreateWidget();
        int res = creator->exec();

        if(res == QDialog::Accepted){
               VectorConstructInfo i = creator->GetMapInfo();
               ShapeFile * map = creator->CreateMap(i);

               if(map != nullptr)
               {
                   if(i.save)
                   {
                        AS_SaveVectorToFileAbsolute(map,i.savename);

                       if(i.add_to_view)
                       {
                            AddLayerFromFile(i.savename);
                       }else
                       {
                            delete map;
                       }
                   }else
                   {
                        if(i.add_to_view)
                        {
                            AddLayerFromScript(map,"new vector",true);
                        }else
                        {
                            delete map;
                        }
                   }
               }
        }

        delete creator;

    }
    inline void OnAddVPressed()
    {
        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select vector"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            QString("*.shp;*.kml;*.gpkg;*.osm;*.*"));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());

            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                LISEM_DEBUG("File does not exist");
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddVectorLayerFromFile(path);
            }
        }

    }

    inline void OnAddFLNewPressed()
    {


    }
    inline void OnAddFLPressed()
    {
        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select Field"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            GetExtensionsFileFilter(GetFieldExtensions()));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());


            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                LISEM_DEBUG("File does not exist");
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddFieldLayerFromFile(path);
            }
        }

    }

    inline void OnAddPCPressed()
    {
        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select point cloud"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            QString("*.las;;*.*"));

        if(!path.isEmpty())
        {

            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());


            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                LISEM_DEBUG("File does not exist");
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddPointCloudLayerFromFile(path);
            }
        }

    }

    inline void OnAddEVTextPressed()
    {

    }
    inline void OnAddEVImagePressed()
    {
        QCheckBox * cb = new QCheckBox("Add in screen coordinates");
        cb->setChecked(false);

        const QStringList schemes = QStringList(QStringLiteral("file"));

        QFileDialog F = QFileDialog(this,QString("Select image file"),GetFIODir(LISEM_DIR_FIO_GEODATA),"*.*");
        F.setOption(QFileDialog::DontUseNativeDialog, true);
        F.setNameFilters(GetExtensionsFileFilters(GetImageAndMapExtensions()));
        if(F.layout() != nullptr)
        {
            F.layout()->addWidget(cb);
        }
        F.setSupportedSchemes(schemes);

        if (F.exec() == QDialog::Accepted) {

            QString path =F.selectedUrls().value(0).toString(); /*QFileDialog::getOpenFileName(this,
                                                QString("Select image file"),
                                                ,
                                                GetExtensionsFileFilter(GetImageAndMapExtensions()));*/

            std::cout << "Selected file: " << path.toStdString() << std::endl;
            bool abscoords = cb->isChecked();

            if(path.startsWith("file:///"))
            {
                path.remove("file:///");

            }
            if(!path.isEmpty())
            {
                if(abscoords)
                {
                    BoundingBox pos = BoundingBox(0.4,0.6,0.4,0.6);
                    UIImageLayer * ret = new UIImageLayer(path,pos,GeoProjection::GetGeneric(),true);

                    ret->SetAdjustInitialSizeForAspectRatio();
                    m_WorldWindow->AddUILayer(ret,true);
                }else
                {

                    BoundingBox pos = m_WorldWindow->GetLook();
                    pos.ScaleX(0.2);
                    pos.ScaleY(0.2);

                    UIImageLayer * ret = new UIImageLayer(path,pos,GeoProjection::GetGeneric(),false);


                    ret->SetAdjustInitialSizeForAspectRatio();
                    m_WorldWindow->AddUILayer(ret,true);
                }

            }

        }


    }
    inline void OnAddEVArrowPressed()
    {

        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select object file"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            GetExtensionsFileFilter(GetModelExtensions()));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());


            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                LISEM_DEBUG("File does not exist");
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddObjectLayerFromFile(path);
            }
        }
    }

    inline void OnAddEVOceanPressed()
    {
        m_WorldWindow->AddOceanLayer();

    }
    inline void OnAddEVSkyPressed()
    {
        m_WorldWindow->AddSkyBoxLayer();

    }

    inline void OnAddShaderPressed()
    {
        //QString basicstring = "void main(){ \nfragColor = vec4(1.0,0.0,0.0,1.0);\n}\n";

        QString basicstring = QString("void main(){\n")
        + "\n"
        + "    // Time varying pixel color, shadertoy style\n"
        + "    vec3 col = 0.5 + 0.5*cos(iTime+texcoord.xyx+vec3(0,2,4));\n"
        + "\n"
        + "    fragColor = vec4(col.xyz,1.0);\n"
        + "}\n";


        UIShaderLayer * ret = new UIShaderLayer({basicstring},{{}},{{}},BoundingBox(0.0,1.0,0.0,1.0),GeoProjection::GetGeneric(), true,false,1280, 720,true,false );

        m_WorldWindow->AddUILayer(ret,true);

    }

    inline void OnAddEVCloudPressed()
    {
        m_WorldWindow->AddCloudLayer();

    }
    inline void OnAddTextPressed()
    {
        LSMVector3 Position = LSMVector3(m_WorldWindow->GetLook().GetCenterX(),0.0,m_WorldWindow->GetLook().GetCenterY());
        GeoProjection p= m_WorldWindow->GetCurrentProjection();
        UITextLayer * layer = new UITextLayer("Text","",Position,p,12,LSMVector4(0.0,0.0,0.0,1.0),false,false,false);
        m_WorldWindow->AddUILayer(layer,true,false);
    }

    inline void OnAddArrowPressed()
    {


    }


    inline void OnAddEVObjectPressed()
    {


        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select object file"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            GetExtensionsFileFilter(GetModelExtensions()));

        if(!path.isEmpty())
        {

            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());

            QFileInfo fileInfo(path);
            if(fileInfo.exists())
            {
                LISEM_DEBUG("File does not exist");
                QString filename(fileInfo.fileName());
                QString filedir(fileInfo.dir().path());


                AddObjectLayerFromFile(path);
            }
        }

    }


    inline void OnAddWebPressed()
    {
          AddGoogleLayer();
    }

    inline void OnAddWebDEMPressed()
    {
          AddGoogleDEMLayer();
    }

    inline void OnListOrderChanged()
    {
        std::cout << "start reorderd"<< std::endl;

        m_WorldWindow->m_UILayerMutex.lock();

        intern_OnLayerDataChanged(true);

        QList<UILayer*> layers;
        for(int i = 0; i < m_Layers->count(); ++i)
        {
            QListWidgetItem* item = m_Layers->item(i);
            void* w = m_Layers->itemWidget(item);
            LayerWidget * wi =(LayerWidget *) w;
            UILayer *ml = wi->GetMapLayer();
            layers.append(ml);
        }
        m_WorldWindow->SetLayerOrder(layers,false,true);

        m_WorldWindow->m_UILayerMutex.unlock();
        std::cout << "end reorderd"<< std::endl;


    }

    inline void intern_OnLayerDataChanged(bool is_locked = false)
    {
        bool widgetupdate = false;

        if(!is_locked)
        {
            m_WorldWindow->m_UILayerMutex.lock();

        }

        int layers_oldcount = m_Layers->count();
        QList<bool> layers_oldreplaced;
        for(int j = 0; j < layers_oldcount; j++)
        {
            layers_oldreplaced.append(false);
        }

        for(int i = 0; i < m_WorldWindow->GetUILayerCount(); i++)
        {
            if(m_WorldWindow->GetUILayer(i)->IsUser())
            {
                std::cout << "layer: " << i << " " << m_WorldWindow->GetUILayer(i) << m_WorldWindow->GetUILayer(i)->GetName().toStdString() << " " << m_WorldWindow->GetUILayer(i)->GetReplacedLayer() << std::endl;
            }
        }

        for(int j = 0; j < layers_oldcount; j++)
        {
            QListWidgetItem* item_old = m_Layers->item(j);
            void* w_old = m_Layers->itemWidget(item_old);
            LayerWidget * wi_old =(LayerWidget *) w_old;
            UILayer *ml_old = wi_old->GetMapLayer();

        }

        for(int i = 0; i < m_WorldWindow->GetUILayerCount(); i++)
        {
            UILayer *ml = m_WorldWindow->GetUILayer(i);

            std::cout << " adding layer " << ml << " " << ml->GetName().toStdString() << " " << ml->GetReplacedLayer() << std::endl;

            if(ml->IsUser())
            {
                //check if it is a replacement
                //else add
                UILayer * l_rep =ml->GetReplacedLayer();
                bool found = false;
                if(l_rep != nullptr)
                {

                    std::cout << " found layer to replace " << std::endl;
                    for(int j = 0; j < layers_oldcount; j++)
                    {
                        QListWidgetItem* item_old = m_Layers->item(j);
                        void* w_old = m_Layers->itemWidget(item_old);
                        LayerWidget * wi_old =(LayerWidget *) w_old;
                        UILayer *ml_old = wi_old->GetMapLayer();
                        if(ml_old == l_rep)
                        {
                            ml->SetStyle(ml_old->GetStyle());
                            wi_old->SetMapLayer(ml,false);
                            ml->SetReplacedLayer(nullptr);
                            layers_oldreplaced.replace(j,true);
                            found = true;
                            std::cout << " actually found " << ml_old << " " << l_rep << std::endl;
                            break;
                        }

                    }

                }else
                {

                    for(int j = 0; j < layers_oldcount; j++)
                    {
                        QListWidgetItem* item_old = m_Layers->item(j);
                        void* w_old = m_Layers->itemWidget(item_old);
                        LayerWidget * wi_old =(LayerWidget *) w_old;
                        UILayer *ml_old = wi_old->GetMapLayer();
                        if(ml->GetUID() == ml_old->GetUID())
                        {
                            wi_old->SetMapLayer(ml,false);
                            layers_oldreplaced.replace(j,true);
                            found = true;
                            std::cout << " actually found" << std::endl;
                            break;
                        }
                    }
                }


                if(!found)
                {
                    QListWidgetItem * item1 = new QListWidgetItem(m_Layers);
                    LayerWidget * itemw1 = new LayerWidget(m_Layers,ml,m_WorldWindow,m_ParentTabWidget);
                    m_Layers->addItem(item1);
                    m_Layers->setItemWidget(item1,itemw1);

                    QRect rec = QApplication::desktop()->screenGeometry();
                    float height = rec.height();
                    float width = rec.width();

                    item1->setSizeHint(QSize(140,35 + 15*(std::max(0.f,height-1080.0f)/1080.0f)));

                    std::cout << " add new " << std::endl;
                    widgetupdate = true;
                }
            }
        }
        if(!is_locked)
        {
            m_WorldWindow->m_UILayerMutex.unlock();
        }

        for(int j = layers_oldcount - 1; j > -1 ; j--)
        {
            QListWidgetItem* item_old = m_Layers->item(j);
            void* w_old = m_Layers->itemWidget(item_old);
            LayerWidget * wi_old =(LayerWidget *) w_old;
            UILayer *ml_old = wi_old->GetMapLayer();

            if(!(layers_oldreplaced.at(j)))
            {
                std::cout << "remove replaced layer " << std::endl;
                delete m_Layers->takeItem(j);

                widgetupdate = true;
            }
        }

        QList<UILayer *> listptrs;
        QList<int> iptrs;
        int index=  0;
        //potential re-ordering
        for(int i = 0; i < m_WorldWindow->GetUILayerCount(); i++)
        {
            UILayer *ml = m_WorldWindow->GetUILayer(i);
            if(ml->IsUser())
            {
                listptrs.append(ml);
                iptrs.append(index);
                index ++;
            }
        }

        bool redo_req = false;
        for(int j = 0; j <  m_Layers->count(); j++)
        {
            QListWidgetItem* item_old = m_Layers->item(j);
            void* w_old = m_Layers->itemWidget(item_old);
            LayerWidget * wi_old =(LayerWidget *) w_old;
            UILayer *ml_old = wi_old->GetMapLayer();

            int ind = listptrs.indexOf(ml_old);
            if(!(ind < 0))
            {
                int index_should = iptrs.at(ind);

                if(index_should != ind)
                {
                    redo_req = true;
                    break;
                }



            }else
            {
                std::cout << "layer not found?????";
            }
        }

        if(redo_req)
        {
            QList<QListWidgetItem*> items;


            std::cout << "REDO REQUIRED" << std::endl;
            for(int j = m_Layers->count()-1; j > -1 ; j--)
            {
                QListWidgetItem* item_old = m_Layers->item(j);
                QListWidgetItem* item_take = m_Layers->takeItem(m_Layers->row(item_old));
                items.append(item_take);
            }

            //sort all

            for(int i = 0; i < m_WorldWindow->GetUILayerCount(); i++)
            {
                UILayer *ml = m_WorldWindow->GetUILayer(i);
                for(int j = 0;j < items.length() ; j++)
                {
                    QListWidgetItem* item_old = items.at(j);
                    void* w_old = m_Layers->itemWidget(item_old);
                    LayerWidget * wi_old =(LayerWidget *) w_old;
                    UILayer *ml_old = wi_old->GetMapLayer();

                    if(ml == ml_old)
                    {
                        m_Layers->addItem(item_old);
                        items.removeAt(j);
                        break;
                    }
                }
            }

        }



        if(widgetupdate)
        {
            m_Layers->update();
        }


    }


    inline void OnLayerDataChanged()
    {

        intern_OnLayerDataChanged();
        std::cout << "end layer replace " << std::endl;
    }

    inline void OnMapIndexChanged(int index)
    {
        m_Model->m_ModelDataMutex.lock();
        m_ModelData->m_CurrentUIMap = index;
        m_Model->m_ModelDataMutex.unlock();
    }

    //image recording stuff

    inline void OnImageRecord()
    {

        QImage * image = m_WorldWindow->DoExternalDraw(m_WorldWindow->ExternalDrawGeoWindowState(m_WidthImBox->value(),m_HeightImBox->value(),m_WorldWindow->GetLook(),true,true));

        QString fname=m_RecordImFile->text();
        if(fname.endsWith(".tif"))
        {
            BoundingBox b = m_WorldWindow->GetLook();
            int ncols = image->width();
            int nrows = image->height();
            double ulx = b.GetMinX();
            double uly = b.GetMinY();

            double csx = b.GetSizeX()/((double)(ncols));
            double csy = b.GetSizeY()/((double)(nrows));

            MaskedRaster<float> raster_data1(nrows,ncols, uly,ulx,csx,csy);
            cTMap *rmap = new cTMap(std::move(raster_data1),m_WorldWindow->GetCurrentProjection().GetWKT(),"");
            MaskedRaster<float> raster_data2(nrows,ncols, uly,ulx,csx,csy);
            cTMap *bmap = new cTMap(std::move(raster_data2),m_WorldWindow->GetCurrentProjection().GetWKT(),"");
            MaskedRaster<float> raster_data3(nrows,ncols, uly,ulx,csx,csy);
            cTMap *gmap = new cTMap(std::move(raster_data3),m_WorldWindow->GetCurrentProjection().GetWKT(),"");

            for(int r = 0; r < nrows; r++)
            {
                for(int c = 0; c < ncols; c++)
                {
                    QColor rgb = image->pixel(c,r);
                    rmap->data[r][c] = rgb.redF();
                    gmap->data[r][c] = rgb.greenF();
                    bmap->data[r][c] = rgb.blueF();
                }
            }

            writeRaster({rmap,bmap,gmap},m_RecordImFile->text(),"GTIFF");

            delete rmap;
            delete bmap;
            delete gmap;

        }else
        {
            image->save(m_RecordImFile->text());
        }

        delete image;
    }



    //video recording stuff

    inline void OnVideoRecord()
    {

        try
        {
            m_VideoCapture = VideoExport::Init(m_WidthBox->value(),m_HeightBox->value(),m_RecordFPSBox->value(),std::max(350000,(6*1000000*m_WidthBox->value()/1920)*(m_HeightBox->value()/1080)),m_RecordFile->text());


            m_VideoTimer.start(m_FPSBox->value() * 1000.0f);

            m_RecordButton->setEnabled(false);
            m_StopButton->setEnabled(true);
            m_DelButton->setEnabled(true);
            m_FPSBox->setEnabled(false);
            m_RecordFPSBox->setEnabled(false);
            m_WidthBox->setEnabled(false);
            m_HeightBox->setEnabled(false);
            m_RecordFile->setEnabled(false);


        }catch(...)
        {
            QMessageBox::critical(this, "LISEM",
               QString("Can not record video, try opening LISEM as administrator"));

        }

    }

    inline void OnVideoStop()
    {
        m_RecordButton->setEnabled(true);
        m_StopButton->setEnabled(false);
        m_DelButton->setEnabled(false);
        m_FPSBox->setEnabled(true);
        m_RecordFPSBox->setEnabled(true);
        m_WidthBox->setEnabled(true);
        m_HeightBox->setEnabled(true);
        m_RecordFile->setEnabled(true);

        m_VideoTimer.stop();

        VideoExport::Finish(m_VideoCapture,true);
        m_VideoCapture = nullptr;

    }

    inline void OnVideoDel()
    {
        m_RecordButton->setEnabled(true);
        m_StopButton->setEnabled(false);
        m_DelButton->setEnabled(false);
        m_FPSBox->setEnabled(true);
        m_RecordFPSBox->setEnabled(true);
        m_WidthBox->setEnabled(true);
        m_HeightBox->setEnabled(true);
        m_RecordFile->setEnabled(true);

        m_VideoTimer.stop();

        VideoExport::Finish(m_VideoCapture,false);



    }

    inline void GetVideoFrame()
    {

        std::cout << "get video frame " << std::endl;

        QImage * image = m_WorldWindow->DoExternalDraw(m_WorldWindow->ExternalDrawGeoWindowState(m_WidthBox->value(),m_HeightBox->value(),m_WorldWindow->GetLook(),true,true));
        VideoExport::AddFrame(image->bits(),m_VideoCapture);

        delete image;
        m_VideoTimer.start(m_FPSBox->value() * 1000.0f);
    }


    //ui stuff

    inline void OnLinesToggled(bool b)
    {
        m_WorldWindow->SetLinesDraw(b);
    }
    inline void OnUIToggled(bool b)
    {
        m_WorldWindow->SetUIDraw(b);
    }
    inline void OnLegendToggled(bool b)
    {
        m_WorldWindow->SetLegendDraw(b);
    }
    inline void OnShadowToggled(bool b)
    {
        m_WorldWindow->SetShadowDraw(b);
    }
    inline void OnLightingToggled(bool b)
    {
        m_WorldWindow->SetLightingDraw(b);
    }


    inline void onUIScaleChanged(double s)
    {
        m_WorldWindow->SetUIScale(s);
    }

    inline void On3DToggled(bool x)
    {
        m_3DGlobeButton->setEnabled(x);

        m_WorldWindow->SetDraw3D(x);

    }
    inline void On3DGlobeToggled(bool x)
    {
        m_WorldWindow->SetDraw3DGlobe(x);
    }


    inline void AnimationUpdate()
    {
        //this function is called (approximately) every 10 ms
        bool locked = m_WorldWindow->m_UILayerMutex.tryLock(50);
        if(locked)
        {
            for(int i = 0; i < m_WorldWindow->GetUILayerCount(); i++)
            {
                UILayer *ml = m_WorldWindow->GetUILayer(i);

                if(ml->IsUser())
                {
                    LSMStyle * s = ml->GetStyleRef();
                    if(s->m_HasTime && s->m_Play)
                    {
                        bool change = s->IncreaseTimeBy(0.01);

                    }
                }
            }

            for(int i = 0; i < m_Layers->count(); i++)
            {
                QListWidgetItem* item = m_Layers->item(i);
                void* w = m_Layers->itemWidget(item);
                LayerWidget * wi =(LayerWidget *) w;
                if(wi != nullptr)
                {
                    wi->OnTimingChanged();
                }
            }

            m_WorldWindow->m_UILayerMutex.unlock();

        }

    }

    inline bool IsEditsUnsaved()
    {
        bool ret = m_WorldWindow->IsEditsUnsaved();
        return ret;
    }

    inline void SaveEdits()
    {

    }

    inline void OnFocusLocationChanged()
    {
        std::cout << "focus location changed "<< std::endl;
        m_WorldWindow->m_UILayerMutex.lock();

        QList<LSMVector2> list =m_WorldWindow->GetFocusLocations();

        std::cout << m_Layers->count() << std::endl;
        for(int i = 0; i < m_Layers->count(); i++)
        {
            std::cout << i << std::endl;
            QListWidgetItem* item = m_Layers->item(i);
            void* w = m_Layers->itemWidget(item);
            LayerWidget * wi =(LayerWidget *) w;
            if(wi != nullptr)
            {
                wi->OnFocusLocationChanged(list);
            }
        }

        m_WorldWindow->m_UILayerMutex.unlock();



    }


    inline void SavePressed()
    {

        QJsonObject sceneJson;
        m_WorldWindow->SaveTo(&sceneJson);
        QJsonDocument document(sceneJson);

        QByteArray doc =  document.toJson();


        QString fileName =
          QFileDialog::getSaveFileName(nullptr,
                                       tr("Save Geo Scene"),
                                       GetFIODir(LISEM_DIR_FIO_GEOSCENE),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!fileName.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEOSCENE,QFileInfo(fileName).dir().absolutePath());

          if (!fileName.endsWith("gsc", Qt::CaseInsensitive))
            fileName += ".gsc";

          QFile file(fileName);
          if (file.open(QIODevice::WriteOnly))
          {
            file.write(doc);
          }
        }
    }

    inline void SaveAsPressed()
    {
        QJsonObject sceneJson;
        m_WorldWindow->SaveTo(&sceneJson);
        QJsonDocument document(sceneJson);

        QByteArray doc =  document.toJson();


        QString fileName =
          QFileDialog::getSaveFileName(nullptr,
                                       tr("Save Geo Scene"),
                                       GetFIODir(LISEM_DIR_FIO_GEOSCENE),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!fileName.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEOSCENE,QFileInfo(fileName).dir().absolutePath());

          if (!fileName.endsWith("gsc", Qt::CaseInsensitive))
            fileName += ".gsc";

          QFile file(fileName);
          if (file.open(QIODevice::WriteOnly))
          {
            file.write(doc);
          }
        }

    }

    inline void NewPressed()
    {



    }

    inline void LoadPressed()
    {

        QString fileName =
          QFileDialog::getOpenFileName(nullptr,
                                       tr("Open Geo Scene"),
                                       GetFIODir(LISEM_DIR_FIO_GEOSCENE),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!QFileInfo::exists(fileName))
          return;

        SetFIODir(LISEM_DIR_FIO_GEOSCENE,QFileInfo(fileName).dir().absolutePath());


        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))
          return;

        QByteArray wholeFile = file.readAll();


        QJsonObject jsonDocument = QJsonDocument::fromJson(wholeFile).object();

        m_WorldWindow->LoadFrom(&jsonDocument);

    }
};


inline static void GetCurrentMapLayerNamesAndID(QList<QString> & names, QList<int> &id)
{

    MapViewTool * m = GetMapViewTool();

    for(int i = 0; i < m->m_WorldWindow->GetUILayerCount(); i++)
    {
        UILayer *ml = m->m_WorldWindow->GetUILayer(i);
        if(ml->IsUser())
        {
            names.append(ml->GetName());
            id.append(ml->GetUID());

        }
    }
}

inline static void GetCurrentMapLayerNamesAndBoundingBoxes(QList<QString> & names, QList<int> &id,QList<BoundingBox> &pos,QList<GeoProjection> &proj)
{

    MapViewTool * m = GetMapViewTool();

    for(int i = 0; i < m->m_WorldWindow->GetUILayerCount(); i++)
    {
        UILayer *ml = m->m_WorldWindow->GetUILayer(i);
        if(ml->IsUser() && ml->IsGeo())
        {
            UIGeoLayer * mlg = (UIGeoLayer *) ml;
            names.append(ml->GetName());
            id.append(ml->GetUID());
            pos.append(mlg->GetBoundingBox());
            proj.append(mlg->GetProjection());

        }
    }
}

inline static ASUILayer CAddCustomShaderLayer1(QString text,  std::vector<cTMap*>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection *p = new GeoProjection(),bool isabs = false, bool scaling = true, bool is2d = true)
{
    MapViewTool * m = GetMapViewTool();

    return m->AddCustomShader1(text,textures,region,resolutionx,resolutiony,*p,isabs,scaling,is2d);
}

inline static ASUILayer CAddCustomShaderLayer2(QString text,  std::vector<QString>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection *p = new GeoProjection(),bool isabs = false, bool scaling = true, bool is2d = true)
{
    MapViewTool * m = GetMapViewTool();

    return m->AddCustomShader2(text,textures,region,resolutionx,resolutiony,*p,isabs,scaling,is2d);
}

inline static ASUILayer CAddCustomShaderLayer1mp(std::vector<QString> text,  std::vector<std::vector<cTMap*>>textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection *p = new GeoProjection(),bool isabs = false, bool scaling = true, bool is2d = true)
{
    MapViewTool * m = GetMapViewTool();

    return m->AddCustomShader1mp(text,textures,region,resolutionx,resolutiony,*p,isabs,scaling,is2d);

}

inline static ASUILayer CAddCustomShaderLayer2mp(std::vector<QString> text,  std::vector<std::vector<QString>> textures,BoundingBox region = BoundingBox(0.0,1.0,0.0,1.0),float resolutionx = -1, float resolutiony = -1, GeoProjection *p = new GeoProjection(),bool isabs = false, bool scaling = true, bool is2d = true)
{
    MapViewTool * m = GetMapViewTool();

    return m->AddCustomShader2mp(text,textures,region,resolutionx,resolutiony,*p,isabs,scaling,is2d);

}


inline static void CSetLayerStyleColorGradient(ASUILayer l, std::vector<float> stops, std::vector<LSMVector4> colors)
{
    MapViewTool * m = GetMapViewTool();

    return m->SetLayerStyleColorGradient(l, stops, colors);

}


#endif // MAPVIEWTOOL_H
