#ifndef MODELTOOL_H
#define MODELTOOL_H

#include "QWidget"
#include "model.h"
#include "QSignalMapper"
#include "QProgressBar"
#include "QToolButton"
#include "QMenuBar"
#include "qtplot/modelstattool.h"
#include "qlist.h"
#include "mapview/mapviewtool.h"
#include "site.h"

struct ParameterWidget
{
    QString m_ParameterName;
    int m_ParameterIndex;
    QLineEdit * m_LineEdit;
    QSpinBox * m_SpinBox;
    QDoubleSpinBox * m_DSpinBox;
    QCheckBox * m_CheckBox;
    QList<QRadioButton*> m_RadioButtons;

    QPushButton * m_BrowseButton;
};




class ModelTool  : public QWidget
{
    Q_OBJECT;

    LISEMModel * m_Model;
    MODELTOINTERFACE * m_ModelData;

    ModelStatTool * m_ModelStatTool;

    QProgressBar * m_ProgressBar;

    QVBoxLayout * m_Layout;
    QMenu *fileMenu;
    QTabWidget * m_TabWidget;
    QList<QVBoxLayout *> TabWidgetLayouts;
    QComboBox * m_DropDown;
    QToolButton *m_delButton;
    QSignalMapper *m_SignalMapper_UI;

    QSignalMapper *m_SignalMapper_Bool;
    QSignalMapper *m_SignalMapper_Map;
    QSignalMapper *m_SignalMapper_Map2;
    QSignalMapper *m_SignalMapper_Dir;
    QSignalMapper *m_SignalMapper_File;
    QSignalMapper *m_SignalMapper_Float;
    QSignalMapper *m_SignalMapper_Int;
    QSignalMapper *m_SignalMapper_bool;
    QSignalMapper *m_SignalMapper_MultiCheck;

    QList<ParameterWidget> m_ParameterWidgetList;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_info;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveasAct;
    QAction *printAct;
    QAction *exitAct;

    QAction *startAct;
    QAction *pauseAct;
    QAction *stopAct;

    QString m_Dir;
    QString m_Path_Current;
    QString m_Path_runfiles;

    MapViewTool * m_MapViewTool;
    ParameterManager * m_ParameterManager;

public:

    inline ModelTool( LISEMModel * m, MODELTOINTERFACE *minterface, MapViewTool  * mpview, ParameterManager * pm,QWidget *parent = 0): QWidget( parent)
    {

        m_Dir = GetSite();

        m_Layout = new QVBoxLayout();
        this->setLayout(m_Layout);

        m_TabWidget = new QTabWidget();


        m_ParameterManager = pm;
        m_MapViewTool = mpview;
        m_Model = m;
        m_ModelData = minterface;

        m_Model->SetModelToInterfaceCallback(&ModelTool::OnModelStepDone,this);
        m_Model->SetModelStoppedCallback(&ModelTool::OnModelStopped,this);
        m_Model->SetModelPausedCallback(&ModelTool::OnModelPaused,this);

        ///create interface using QWidgets

        m_SignalMapper_UI = new QSignalMapper(this);

        m_SignalMapper_Bool = new QSignalMapper(this);
        m_SignalMapper_Map = new QSignalMapper(this);
        m_SignalMapper_Map2 = new QSignalMapper(this);
        m_SignalMapper_Dir = new QSignalMapper(this);
        m_SignalMapper_File = new QSignalMapper(this);
        m_SignalMapper_Float = new QSignalMapper(this);
        m_SignalMapper_Int = new QSignalMapper(this);
        m_SignalMapper_MultiCheck = new QSignalMapper(this);

        connect(m_SignalMapper_UI,SIGNAL(mapped(QString)),this,SLOT(SignalFunction_UI(QString)));
        connect(m_SignalMapper_Dir,SIGNAL(mapped(int)),this,SLOT(SignalFunction_Dir(int)));
        connect(m_SignalMapper_Map,SIGNAL(mapped(int)),this,SLOT(SignalFunction_Map(int)));
        connect(m_SignalMapper_Map2,SIGNAL(mapped(int)),this,SLOT(SignalFunction_Map2(int)));
        connect(m_SignalMapper_File,SIGNAL(mapped(int)),this,SLOT(SignalFunction_File(int)));
        connect(m_SignalMapper_Float,SIGNAL(mapped(int)),this,SLOT(SignalFunction_Float(int)));
        connect(m_SignalMapper_Int,SIGNAL(mapped(int)),this,SLOT(SignalFunction_Int(int)));
        connect(m_SignalMapper_MultiCheck,SIGNAL(mapped(int)),this,SLOT(SignalFunction_MultiCheck(int)));

        QVBoxLayout *mainlayout = new QVBoxLayout;
        mainlayout->setMargin(0);
        mainlayout->setSpacing(0);

        QWidget * MenuWidgetExt = new QWidget();
        QHBoxLayout * mlExt = new QHBoxLayout(MenuWidgetExt);
        QWidget * MenuWidget = new QWidget();
        QHBoxLayout * ml = new QHBoxLayout(MenuWidget);
        MenuWidget->setLayout(ml);
        ml->setMargin(0);
        ml->setSpacing(0);
        mlExt->setMargin(0);
        mlExt->setSpacing(0);
        MenuWidgetExt->setLayout(mlExt);
        QMenuBar* mb = new QMenuBar();


        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();

        icon_start->addFile((m_Dir + LISEM_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + LISEM_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + LISEM_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);


        newAct = new QAction(tr("&New"), this);
        newAct->setShortcuts(QKeySequence::New);
        newAct->setStatusTip(tr("Create a new file"));
        connect(newAct, &QAction::triggered, this, &ModelTool::newFile);

        openAct = new QAction(*icon_open,tr("&Open"), this);
        //openAct->setShortcuts(QKeySequence::Open);
        openAct->setStatusTip(tr("Open an existing file"));
        connect(openAct, &QAction::triggered, this, &ModelTool::open);

        saveAct = new QAction(*icon_save,tr("&Save"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setStatusTip(tr("Save the document to disk"));
        connect(saveAct, &QAction::triggered, this, &ModelTool::save);

        saveasAct = new QAction(*icon_saveas,tr("&Save As"), this);
        //saveasAct->setShortcuts(QKeySequence::SaveAs);
        saveasAct->setStatusTip(tr("Save the document to disk"));
        connect(saveasAct, &QAction::triggered, this, &ModelTool::saveas);

        startAct = new QAction(*icon_start,tr("&Start"), this);
        //startAct->setShortcuts(QKeySequence::Paste);
        startAct->setStatusTip(tr("Start the simulation"));
        startAct->setCheckable(true);
        connect(startAct, &QAction::triggered, this, &ModelTool::start);

        pauseAct = new QAction(*icon_pause,tr("&Pause"), this);
        //pauseAct->setShortcuts(QKeySequence::Paste);
        pauseAct->setStatusTip(tr("Pause the simulation"));
        connect(pauseAct, &QAction::triggered, this, &ModelTool::pause);
        pauseAct->setCheckable(true);

        stopAct = new QAction(*icon_stop,tr("&Stop"), this);
        //stopAct->setShortcuts(QKeySequence::Paste);
        stopAct->setStatusTip(tr("Stop the simulation"));
        connect(stopAct, &QAction::triggered, this, &ModelTool::stop);

        mb->addSeparator();
        mb->addAction(newAct);
        mb->addAction(openAct);
        mb->addAction(saveAct);
        mb->addAction(saveasAct);
        mb->addSeparator();
        mb->addAction(startAct);
        mb->addAction(pauseAct);
        mb->addAction(stopAct);
        mb->addSeparator();

        ml->setMenuBar(mb);


        mlExt->addWidget(MenuWidget);


        m_DropDown = new QComboBox();
        mlExt->addWidget(m_DropDown);

        QIcon icon_del;
        icon_del.addFile((m_Dir + LISEM_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_delButton = new QToolButton(this);
        m_delButton->setIcon(icon_del);
        m_delButton->setIconSize(QSize(18,18));
        m_delButton->resize(18,18);
        m_delButton->setEnabled(true);
        connect(m_delButton,SIGNAL(pressed()),this,SLOT(OnRunFileDeleteFromList()));
        mlExt->addWidget(m_delButton);

        m_Layout->addWidget(MenuWidgetExt);


        m_ProgressBar = new QProgressBar();


        m_ModelStatTool = new ModelStatTool(m_Model,m_ModelData);


        CreateParameterWidgets();

        m_TabWidget->addTab(m_ModelStatTool,"Simulation Stats");

        m_Layout->addWidget(m_TabWidget);
        m_Layout->addWidget(m_ProgressBar);



        m_Path_Current = GetSite()+"/";
        m_Path_runfiles = m_Path_Current + "runfiles.ini";

        m_DropDown->setDuplicatesEnabled(false);
        m_DropDown->setInsertPolicy(QComboBox::InsertAtTop);
        connect(m_DropDown, SIGNAL(currentIndexChanged(int)), this, SLOT(OnRunfileIndexChanged(int)));

        QFile fin(m_Path_runfiles);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
            {
                QTextStream out(&fin);
                out << QString("[Runfile List]\n");
            }
            fin.close();
        }else
        {
            while (!fin.atEnd())
            {
                QString S = fin.readLine().trimmed();
                QFile fin_temp(S);
                if(!S.startsWith("["))
                {
                    if (fin_temp.open(QFile::ReadOnly | QFile::Text))
                    {
                        fin_temp.close();

                        if(m_DropDown->count() == 0)
                        {
                            ImportRunfile(S);

                        }else
                        {
                            m_DropDown->insertItem(m_DropDown->count() -1, S);
                        }
                    }
                }
            }
        }

        ExportRunfileList();


    }


    inline void OnModelStepDone()
    {

        //this is the moment to exchange any data between the main interface and the model while it is running
        m_Model->m_ModelDataMutex.lock();



        m_Model->m_InterfaceData.m_CurrentUIMap = m_ModelData->m_CurrentUIMap;

        //from model to interface
        m_Model->m_InterfaceData.m_UIMapNames =m_Model->m_UIMapNames;
        m_Model->m_InterfaceData.val_max = m_ModelData->val_max;
        m_Model->m_InterfaceData.val_min = m_ModelData->val_min;

        *m_ModelData = m_Model->m_InterfaceData;

        //for pointers we must make a duplicate manually
        if(m_ModelData->H.length() < m_ModelData->outlets.length())
        {
            for(int i = 0; i < m_ModelData->outlets.length(); i++)
            {
                m_ModelData->H.append(new QList<float>());
            }

        }
        if(m_ModelData->V.length() < m_ModelData->outlets.length())
        {
            for(int i = 0; i < m_ModelData->outlets.length(); i++)
            {
                m_ModelData->V.append(new QList<float>());
            }
        }
        if(m_ModelData->Q.length() < m_ModelData->outlets.length())
        {
            for(int i = 0; i < m_ModelData->outlets.length(); i++)
            {
                m_ModelData->Q.append(new QList<float>());
            }
        }
        for(int i = 0; i < m_ModelData->outlets.length(); i++)
        {
            m_ModelData->H.at(i)->fromStdList(m_Model->m_InterfaceData.H.at(i)->toStdList());
            m_ModelData->V.at(i)->fromStdList(m_Model->m_InterfaceData.V.at(i)->toStdList());
            m_ModelData->Q.at(i)->fromStdList(m_Model->m_InterfaceData.Q.at(i)->toStdList());
        }


        //from interface to model
        if(m_Model->m_InterfaceData.m_mapnameschanged == true)
        {
            m_Model->m_InterfaceData.m_mapnameschanged = false;
        }


        m_Model->m_ModelDataMutex.unlock();

        QTimer::singleShot(0,this,SLOT(UpdateInterfaceFromModelData()));



    }

public slots:


    void UpdateInterfaceFromModelData();

    void newFile();
    void open();
    void save();
    void saveas();

    inline void SignalFunction_UIButton(QString arg)
    {


    }

    void InterfaceOpenRunFile(QString file);
    void ExportRunfile(bool saveas = false);
    void ImportRunfile(QString name= QString(""));
    void ExportRunfileList();
    void OnRunfileIndexChanged(int i);
    void OnRunFileDeleteFromList();

    void CreateParameterWidgets();

    void UpdateParameters();
    void UpdateParameterActive();
    void UpdateInterface();

    void start();
    void pause();
    void stop();
    void OnModelPaused();
    void OnModelStopped(bool);
    void ModelPaused();
    void ModelStopped(bool has_error);


    void SignalFunction_UI(QString);


    void SignalFunction_Bool(int index);
    void SignalFunction_Map(int index);
    void SignalFunction_Map2(int index);
    void SignalFunction_Dir(int index);
    void SignalFunction_File(int index);
    void SignalFunction_Float(int index);
    void SignalFunction_Int(int index);
    void SignalFunction_MultiCheck(int index);


};
#endif // MODELTOOL_H
