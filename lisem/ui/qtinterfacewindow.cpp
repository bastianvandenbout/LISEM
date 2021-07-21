#include "qtinterfacewindow.h"

#include <iostream>
#include <error.h>
#include "iogdal.h"
#include "lsmio.h"
#include <QSignalMapper>
#include <QAction>
#include "tools/toolbrowser.h"
#include "extensionprovider.h"
#include "site.h"

int QTInterfaceWindow::Create(ParameterManager * pm, LISEMModel * m, ScriptManager * sm, WorldWindow * window )
{


    LISEM_DEBUG("Creating Interface using QT")


            // set style
            qApp->setStyle(QStyleFactory::create("Fusion"));
            // increase font size for better reading
            QFont defaultFont = QApplication::font();
            defaultFont.setPointSize(defaultFont.pointSize()+2);
            qApp->setFont(defaultFont);


    m_Dir = GetSite();

    m_Model = m;
    m_ParameterManager = pm;
    m_ScriptManager = sm;
    m_WorldWindow = window;


    m_MainWidget = new QWidget;
    setCentralWidget(m_MainWidget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_ParameterWidget = new QWidget(this);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(m_ParameterWidget);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);


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

    mainlayout->addWidget(m_ParameterWidget);
    m_MainWidget->setLayout(mainlayout);


    QSizePolicy newsizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newsizePolicy.setHorizontalStretch(0);
    newsizePolicy.setVerticalStretch(0);
    newsizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(newsizePolicy);

    //QSystemTrayIcon trayIcon = new QSystemTrayIcon(this);l
    //trayIcon->setIcon(QIcon(":/openLisem.ico"));
    //trayIcon->show();

    setWindowTitle("LISEM 0.1.0 alpha");
    setMinimumWidth(800);
    setMinimumHeight(600);
    setAttribute(Qt::WA_DeleteOnClose);




    m_Layout= new QVBoxLayout(m_ParameterWidget);


    m_Layout->setSpacing(8);
    m_Layout->setContentsMargins(8, 4, 8, 4);
    m_Layout->setObjectName(QStringLiteral("MainLayout"));

    this->installEventFilter(this);

    TabWidget = new QTabWidget();
    m_Layout->addWidget(TabWidget);

    QToolButton * m_SettingsButton = new QToolButton();
    m_SettingsButton->setText("Options");

    QAction * dAct = new QAction(tr("&Device"), this);
    dAct->setStatusTip(tr("GPU device"));
    connect(dAct, &QAction::triggered, this, &QTInterfaceWindow::Device);

    QAction * ttAct = new QAction(tr("&Theme"), this);
    ttAct->setStatusTip(tr("Toggle Dark Theme"));
    connect(ttAct, &QAction::triggered, this, &QTInterfaceWindow::ToggleTheme);

    QAction * tcAct = new QAction(tr("&Controls"), this);
    tcAct->setStatusTip(tr("See Control Scheme"));
    connect(tcAct, &QAction::triggered, this, &QTInterfaceWindow::ControlScheme);

    QAction * tfAct = new QAction(tr("&Font"), this);
    tfAct->setStatusTip(tr("Set Font Size"));
    connect(tfAct, &QAction::triggered, this, &QTInterfaceWindow::Font);

    QAction * tiAct = new QAction(tr("&About"), this);
    tiAct->setStatusTip(tr("Information on this software"));
    connect(tiAct, &QAction::triggered, this, &QTInterfaceWindow::Info);

    m_SettingsButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_SettingsButton->addAction(dAct);
    m_SettingsButton->addAction(ttAct);
    m_SettingsButton->addAction(tcAct);
    m_SettingsButton->addAction(tfAct);
    m_SettingsButton->addAction(tiAct);

    TabWidget->setCornerWidget(m_SettingsButton,Qt::TopRightCorner);

    m_MapViewTool = new MapViewTool(m_Model,&m_ModelData,m_WorldWindow,TabWidget,this);

    TabWidget->addTab(m_MapViewTool,"Map View");

    m_DatabaseWidget = new DatabaseTool(m_ScriptManager,this);
    m_DatabaseWidget->SetCallBackFileOpened(&QTInterfaceWindow::OnFileOpenRequest,this);
    TabWidget->addTab(m_DatabaseWidget,"Script");

    m_WorldWindow->SetCallBackFileOpened(&QTInterfaceWindow::OnFileOpenRequest,this);


    m_VisualScripter = new VisualScriptingTool(m_ScriptManager,this);
    TabWidget->addTab(m_VisualScripter,"Visual Script");

    m_ModelTool = new ModelTool( m_Model, & m_ModelData,m_MapViewTool,m_ParameterManager,this);
    TabWidget->addTab(m_ModelTool,"SPH Model");

    m_ToolBoxWidget = new ToolBrowser(m_ScriptManager,m_DatabaseWidget,m_MapViewTool);


    TabWidget->addTab(m_ToolBoxWidget,"ToolBox");

    m_DownloadWidget = new DownloadManagerWidget();
    TabWidget->addTab(m_DownloadWidget,"Downloads");

    m_ConsoleTool = new ConsoleTool(this,0,m_ScriptManager);
    TabWidget->addTab(m_ConsoleTool,"Console");






    m_Path_Current = GetSite()+"/";
    m_Path_runfiles = m_Path_Current + "runfiles.ini";

    return 0;
}

void QTInterfaceWindow::start()
{
    UpdateParameters();
    m_Model->RequestStart(m_ParameterManager->GetParameterList());

    m_ProgressBar->resetFormat();

    //indicate loading
    m_ProgressBar->setMinimum(0);
    m_ProgressBar->setMaximum(0);
    m_ProgressBar->setValue(0);

    //startAct->blockSignals(true);
    //startAct->setChecked(true);
    //startAct->blockSignals(false);
}


void QTInterfaceWindow::pause()
{
    m_Model->RequestPause();
}

void QTInterfaceWindow::stop()
{
    m_Model->RequestStop();
}

void QTInterfaceWindow::OnModelPaused()
{
    QTimer::singleShot(0,this,SLOT(ModelPaused()));

}

void QTInterfaceWindow::ModelPaused()
{
    m_ProgressBar->setTextVisible(true);
    m_ProgressBar->setFormat(" Paused at %p% ");
}

void QTInterfaceWindow::OnModelStopped(bool has_error)
{

    QTInterfaceWindow * w = this;
    QTimer::singleShot(0,this, [w,has_error] () {w->ModelStopped(has_error); });

}

void QTInterfaceWindow::ModelStopped(bool has_error)
{

    m_ProgressBar->setTextVisible(true);
    m_ProgressBar->setMinimum(0);
    m_ProgressBar->setMaximum(1);
    m_ProgressBar->setValue(0);
    if(has_error)
    {
        m_ProgressBar->setFormat(" Error %p% ");


        QMessageBox messageBox;
        messageBox.critical(0,"Error","A critical error has occured! \nSee console for details.");
        messageBox.setFixedSize(500,200);

        messageBox.show();

    }else
    {
        m_ProgressBar->setFormat(" Finished %p% ");
    }

}


void QTInterfaceWindow::newFile()
{
    m_ParameterManager->SetAllDefault();
    this->UpdateInterface();
    m_DropDown->insertItem(0,QString(""));
}

void QTInterfaceWindow::open()
{
    ImportRunfile("");
}

void QTInterfaceWindow::save()
{
    ExportRunfile(false);
}

void QTInterfaceWindow::saveas()
{
    ExportRunfile(true);
}

void QTInterfaceWindow::ImportRunfile(QString name)
{

    LISEM_STATUS("import runfile " + name);

    QString path;


    if(name.length() == 0)
    {
        QString openDir;
        QString currentDir = GetSite()+"/";

        if (m_DropDown->count() > 0)
        {
            openDir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
        }
        else
        {
            openDir = currentDir;
        }

        path = QFileDialog::getOpenFileName(this,
                                            QString("Select run file"),
                                            openDir,
                                            QString("*.run"));
    }else
    {
        path = name;
    }

    if (path.isEmpty())
    {

        return;
    }

    int ret = m_ParameterManager->LoadFromRunFile(path);

    if(ret != 0)
    {
        return;
    }

    UpdateInterface();

    bool exst = false;
    int nr = 0;
    for (int i = 0; i < m_DropDown->count(); i++)
    {
        if (m_DropDown->itemText(i).compare(path) == 0)
        {
            exst = true;
            nr = i;
        }
    }
    m_DropDown->blockSignals(true);
    if (!exst)
    {
        m_DropDown->insertItem(0,path);
        m_DropDown->setCurrentIndex(0);
    }else
    {
        m_DropDown->removeItem(nr);
        m_DropDown->insertItem(0,path);
        m_DropDown->setCurrentIndex(0);
    }
    m_DropDown->blockSignals(false);

    ExportRunfileList();

}

void QTInterfaceWindow::ExportRunfile(bool saveas)
{
    bool current_exists = false;
    QString fileName;
    if(!saveas)
    {
        if(m_DropDown->count() > 0)
        {
            QString text = m_DropDown->currentText();
            if(text.length() > 0)
            {
                fileName = text;
                current_exists =true;
            }

        }
    }


    if(!current_exists)
    {
        QString selectedFilter;
        fileName = QFileDialog::getSaveFileName(this,
                                                        QString("Give a new runfile name"),
                                                        QString("sph.run"),
                                                        QString("Text Files (*.run);;All Files (*)"),
                                                        &selectedFilter);
    }

    if (!fileName.isEmpty())
    {

        UpdateParameters();
        int res =m_ParameterManager->SaveToRunFile(fileName);

        if(res != 0)
        {
            LISEM_ERROR("could not write to file: " + fileName);
        }
    }


    bool exst = false;
    int nr = 0;
    for (int i = 0; i < m_DropDown->count(); i++)
    {
        if (m_DropDown->itemText(i).compare(fileName) == 0)
        {
            exst = true;
            nr = i;
        }
    }

    m_DropDown->blockSignals(true);
    if (!exst)
    {
        m_DropDown->insertItem(0,fileName);
        m_DropDown->setCurrentIndex(0);
    }else
    {
        m_DropDown->removeItem(nr);
        m_DropDown->insertItem(0,fileName);
        m_DropDown->setCurrentIndex(0);

    }
    m_DropDown->blockSignals(false);

    ExportRunfileList();
}

void QTInterfaceWindow::ExportRunfileList()
{

    QFile fin(m_Path_runfiles);
    if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&fin);
        out << QString("[Runfile List]\n");
        for(int i = 0; i < m_DropDown->count(); i++)
        {

            out << m_DropDown->itemText(i) << "\n";
        }
    }
    fin.close();

}

void QTInterfaceWindow::OnRunFileDeleteFromList()
{

    m_DropDown->blockSignals(true);
    if(m_DropDown->count() > 0)
    {
        m_DropDown->removeItem(0);
    }
    if(m_DropDown->count() > 0)
    {
        m_DropDown->setCurrentIndex(0);
    }
    m_DropDown->blockSignals(false);

    if(m_DropDown->count() > 0)
    {
        ImportRunfile(m_DropDown->itemText(0));
    }

}
void QTInterfaceWindow::OnRunfileIndexChanged(int i)
{
    ImportRunfile(m_DropDown->itemText(i));
}


void QTInterfaceWindow::print()
{

}
void QTInterfaceWindow::cut()
{


}
void QTInterfaceWindow::copy()
{


}
void QTInterfaceWindow::paste()
{


}

void QTInterfaceWindow::SignalFunction_UI(QString action)
{


}


void QTInterfaceWindow::UpdateInterfaceFromModelData()
{

    m_Model->m_ModelDataMutex.lock();
    m_MapViewTool->m_map_combo->blockSignals(true);

    if(m_ModelData.m_mapnameschanged==true)
    {
        m_MapViewTool->m_map_combo->clear();

        for(int i =0; i < m_ModelData.m_UIMapNames.length();i++)
        {
            m_MapViewTool->m_map_combo->addItem(m_ModelData.m_UIMapNames.at(i));
        }
        m_MapViewTool->m_map_combo->setCurrentIndex(0);

        m_ModelData.m_mapnameschanged = false;
    }

    m_ProgressBar->setRange(0,m_ModelData.timesteps);
    m_ProgressBar->setValue(m_ModelData.step);
    m_MapViewTool->m_map_combo->blockSignals(false);

    m_ModelStatTool->OnModelDataUpdate();

    m_Model->m_ModelDataMutex.unlock();



}


bool QTInterfaceWindow::OnFileOpenRequest(QString filepath, int type)
{
    std::cout << "fopen request " << filepath.toStdString() << std::endl;
    bool use = false;

    if(type == LISEM_FILE_TYPE_RUN)
    {

        m_ModelTool->ImportRunfile(filepath);

    }else if(type == LISEM_FILE_TYPE_TABLE)
    {
        //table extension
        MatrixTable * t = new MatrixTable();
        t->LoadAsAutoFormat(filepath);

        LayerInfoWidget *w = new LayerInfoWidget(t);
        w->show();

    }else if(type == LISEM_FILE_TYPE_SCRIPT)
    {
        m_DatabaseWidget->m_FileEditor->OpenCode(filepath);

    }else if(type == LISEM_FILE_TYPE_MAP)
    {
        use = true;
        m_MapViewTool->AddLayerFromFile(filepath);
    }else if(type == LISEM_FILE_TYPE_UNKNOWN){

        if(filepath.length() > 3)
        {
            QString ext = filepath.right(4);

            if(IsMapFile(filepath))
            {
                //raster extension, try to open with map display
                use = true;
                m_MapViewTool->AddLayerFromFileAutoStream(filepath);

            }
            if(IsShapeFile(filepath))
            {
                use = true;
                m_MapViewTool->AddVectorLayerFromFile(filepath);
            }

            if(IsPointCloudFile(filepath))
            {
                use = true;
                m_MapViewTool->AddPointCloudLayerFromFile(filepath);
            }

            if((ext.compare(".run") == 0))
            {
                use = true;
                //run file...
            }

        }


    }



    return use;
}
