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

        QAction * eoAct = new QAction(tr("Ocean"), this);
        eoAct->setStatusTip(tr("Ocean layer at elevation 0.0"));

        QAction * esAct = new QAction(tr("Sky"), this);
        esAct->setStatusTip(tr("Sky layer"));

        QAction * eobAct = new QAction(tr("Object"), this);
        eobAct->setStatusTip(tr("Object from textured model"));

        QAction * eclAct = new QAction(tr("Clouds"), this);
        eclAct->setStatusTip(tr("Add Volumetric Cloud Layer"));

        connect(etAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVTextPressed()));
        connect(eiAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVImagePressed()));
        connect(eaAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVArrowPressed()));

        connect(eoAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVOceanPressed()));
        connect(esAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVSkyPressed()));
        connect(eobAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVObjectPressed()));
        connect(eclAct,SIGNAL(triggered(bool)), this, SLOT(OnAddEVCloudPressed()));

        addEVButton->addAction(eoAct);
        addEVButton->addAction(esAct);
        addEVButton->addAction(eobAct);
        addEVButton->addAction(eclAct);

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
        m_DrawingOptionsLayout->addWidget(m_CheckBoxUI);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxLegend);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxShadow);
        m_DrawingOptionsLayout->addWidget(m_CheckBoxLighting);
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


    inline void SetScriptFunctions(ScriptManager * sm)
    {

        std::cout << "add viewlayer calls " << this << std::endl;

        int r = sm->m_Engine->RegisterObjectType("UILayer", sizeof(ASUILayer), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUILayer>());
        sm->m_Engine->RegisterObjectBehaviour("UILayer", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUILayerC0,(void*),void), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectBehaviour("UILayer", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUILayerD0), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectMethod("UILayer", "UILayer& opAssign(const UILayer &in m)", asMETHODPR(ASUILayer,Assign,(ASUILayer *),ASUILayer*), asCALL_THISCALL); assert( r >= 0 );

        r = sm->m_Engine->RegisterObjectType("UIPlot", sizeof(ASUIPlot), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUIPlot>());
        sm->m_Engine->RegisterObjectBehaviour("UIPlot", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUIPlotC0,(void*),void), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectBehaviour("UIPlot", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUIPlotD0), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectMethod("UIPlot", "UIPlot& opAssign(const UIPlot &in m)", asMETHODPR(ASUIPlot,Assign,(ASUIPlot *),ASUIPlot*), asCALL_THISCALL); assert( r >= 0 );

        r = sm->m_Engine->RegisterObjectType("UIPlotLayer", sizeof(ASUIPlotLayer), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUIPlotLayer>());
        sm->m_Engine->RegisterObjectBehaviour("UIPlotLayer", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUIPlotLayerC0,(void*),void), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectBehaviour("UIPlotLayer", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUIPlotLayerD0), asCALL_CDECL_OBJLAST);
        sm->m_Engine->RegisterObjectMethod("UIPlotLayer", "UIPlotLayer& opAssign(const UIPlotLayer &in m)", asMETHODPR(ASUIPlotLayer,Assign,(ASUIPlotLayer *),ASUIPlotLayer*), asCALL_THISCALL); assert( r >= 0 );


        //Create Plot
        sm->m_Engine->RegisterGlobalFunction("UIPlot CreatePlot(string name)", asMETHODPR( MapViewTool ,CreatePlot, (QString),ASUIPlot),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        sm->m_Engine->RegisterGlobalFunction("UIPlotLayer AddPlotLayer(UIPlot p, const Table &in t, string name)", asMETHODPR( MapViewTool ,AddPlotLayer, (ASUIPlot, MatrixTable *, QString),ASUIPlotLayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        sm->m_Engine->RegisterGlobalFunction("void ReplacePlotLayer(UIPlotLayer l, const Table &in t)", asMETHODPR( MapViewTool ,ReplacePlotLayer, (ASUIPlotLayer,MatrixTable * ),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        sm->m_Engine->RegisterGlobalFunction("void SetPlotLayerStyle(UIPlotLayer l, vec3 color)", asMETHODPR( MapViewTool ,SetPlotLayerStyle, (ASUIPlotLayer, LSMVector3),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );





        //add layer

        sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayerVelocity(Map &in mapx, Map &in mapy, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddVelLayerFromScript,(cTMap *,cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

        sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Map &in map, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Shapes &in shapes, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(ShapeFile *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Field &in map, string name, bool removeable =  true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(Field*,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(const RigidModel &in map, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(RigidPhysicsWorld *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

        //remove layer

        r = sm->m_Engine->RegisterGlobalFunction("void RemoveViewLayer(UILayer layer)", asMETHODPR( MapViewTool ,RemoveLayerFromScript,(ASUILayer),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

        //replace layer
        r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Map &in map)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, cTMap *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Shapes &in shapes)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, ShapeFile *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Field &in shapes)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, Field *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

        //Get Layer Id Map

        r = sm->m_Engine->RegisterGlobalFunction("UILayer GetUILayer(string name)", asMETHODPR( MapViewTool ,GetLayerMap,(QString),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

        //get actual layer from id

        r = sm->m_Engine->RegisterGlobalFunction("Map& GetViewLayerMap(UILayer layer)", asMETHODPR( MapViewTool ,ScriptGetLayerMap,(ASUILayer),cTMap*),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
        r = sm->m_Engine->RegisterGlobalFunction("Shapes& GetViewLayerShapes(UILayer layer)", asMETHODPR( MapViewTool ,ScriptGetLayerShapeFile,(ASUILayer),ShapeFile*),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


        sm->m_Engine->RegisterTypeDebugAction(sm->m_Engine->GetTypeIdByDecl("Map"),"View",[this](void* par){
            cTMap * m = (cTMap *)par;
            RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{m->GetCopy()}}),false,true);

            if(rdp->Exists())
            {
                UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,"DebugMap",m->AS_FileName.size() == 0? false : true,m->AS_FileName,!false);
                if(rdp->GetBandCount() > 1)
                {
                    ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
                }else
                {
                    ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
                }
                m_WorldWindow->AddUILayer(ret,true);

            }
        });


        sm->m_Engine->RegisterTypeDebugAction(sm->m_Engine->GetTypeIdByDecl("Map"),"Calculate Stats",[this](void* par){
            cTMap * m = (cTMap *)par;

            RasterBandStats stats = m->GetRasterBandStats(true);
            MatrixTable * t = new MatrixTable();
            t->SetSize(12,1);
            t->SetColumnTitle(0,QString("Value"));
            t->SetRowTitle(0,QString("Band"));t->SetValue(0,0,stats.band);
            t->SetRowTitle(1,QString("Rows"));t->SetValue(1,0,m->nrRows());
            t->SetRowTitle(2,QString("Cols"));t->SetValue(2,0,m->nrCols());
            t->SetRowTitle(3,QString("ULX"));t->SetValue(3,0,m->west());
            t->SetRowTitle(4,QString("ULY"));t->SetValue(4,0,m->north());
            t->SetRowTitle(5,QString("dx"));t->SetValue(5,0,m->cellSizeX());
            t->SetRowTitle(6,QString("dy"));t->SetValue(6,0,m->cellSizeY());
            t->SetRowTitle(7,QString("Mean"));t->SetValue(7,0,stats.mean);
            t->SetRowTitle(8,QString("min"));t->SetValue(8,0,stats.min);
            t->SetRowTitle(9,QString("max"));t->SetValue(9,0,stats.max);
            t->SetRowTitle(10,QString("stdev"));t->SetValue(10,0,stats.stdev);
            t->SetRowTitle(11,QString("MV"));t->SetValue(11,0,stats.n_mv);
            LayerInfoWidget *w = new LayerInfoWidget(t);
            w->show();

        });



        connect(this,&MapViewTool::int_on_create_plot,this,&MapViewTool::OnCreatePlot,Qt::ConnectionType::QueuedConnection);
        connect(this,&MapViewTool::int_on_add_plot,this,&MapViewTool::OnCreatePlotLayer,Qt::ConnectionType::QueuedConnection);
        connect(this,&MapViewTool::int_on_replace_plot,this,&MapViewTool::OnReplacePlotLayer,Qt::ConnectionType::QueuedConnection);
        connect(this,&MapViewTool::int_on_style_plot,this,&MapViewTool::OnSetPlotLayerStyle,Qt::ConnectionType::QueuedConnection);


    }


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
                                            openDir,
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {

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
                                            openDir,
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
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
                                            openDir,
                                            QString("*.shp;*.kml;*.gpkg;*.osm;*.*"));

        if(!path.isEmpty())
        {
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
                                            openDir,
                                            GetExtensionsFileFilter(GetFieldExtensions()));

        if(!path.isEmpty())
        {
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
                                            openDir,
                                            QString("*.las;;*.*"));

        if(!path.isEmpty())
        {
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

    }
    inline void OnAddEVArrowPressed()
    {

        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select object file"),
                                            openDir,
                                            GetExtensionsFileFilter(GetModelExtensions()));

        if(!path.isEmpty())
        {
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

    inline void OnAddEVCloudPressed()
    {
        m_WorldWindow->AddCloudLayer();

    }
    inline void OnAddEVObjectPressed()
    {


        QString openDir = GetSite();

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select object file"),
                                            openDir,
                                            GetExtensionsFileFilter(GetModelExtensions()));

        if(!path.isEmpty())
        {
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
                                       QDir::homePath(),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!fileName.isEmpty())
        {
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
                                       QDir::homePath(),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!fileName.isEmpty())
        {
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
                                       QDir::homePath(),
                                       tr("Geo Scene Files (*.gsc)"));

        if (!QFileInfo::exists(fileName))
          return;

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


#endif // MAPVIEWTOOL_H
