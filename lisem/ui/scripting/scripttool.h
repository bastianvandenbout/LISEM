#ifndef SCRIPTTOOL_H
#define SCRIPTTOOL_H

#include "QWidget"

#include "QVBoxLayout"
#include "QHBoxLayout"
#include "codeeditor.h"
#include "QIcon"
#include "lsmio.h"
#include "QCoreApplication"
#include "QToolButton"
#include <QTabWidget>
#include <QLabel>
#include "QFileDialog"
#include "QMessageBox"
#include "QMovie"
#include "QSplitter"
#include "widgets/labeledwidget.h"
#include "resourcemanager.h"
#include "QTextDocument"
#include "QTextDocumentFragment"

#include "stackdisplay.h"
#include "atomic"
#include "site.h"
class ScriptTool : public QWidget
{
    Q_OBJECT

public:

    QVBoxLayout *m_MainLayout;
    QWidget * m_MenuWidget;
    QHBoxLayout * m_MenuLayout;
    QTabWidget * m_ScriptTabs;

    QString m_Dir;

    QMenu * m_contextmenu;

    QWaitCondition m_PauseWaitCondition;
    QMutex m_PauseMutex;

    std::atomic<bool> m_CodeIsRunning;
    std::atomic<bool> m_CodeIsPaused;
    std::atomic<bool> m_CodeIsStopRequested;
    std::atomic<bool> m_CodeIsPauseRequested;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_info;
    QIcon *icon_debug;

    QIcon *icon_compile;
    QIcon *icon_indent;
    QIcon *icon_indentr;
    QIcon *icon_style;
    QIcon *icon_comment;
    QIcon *icon_normal;

    QLabel *m_LabelFile;

    StackDisplay * m_StackDisplay;
    QSplitter *m_DebugSplitter;

    QToolButton *StartButton;
    QToolButton *PauseButton;
    QToolButton *StopButton;
    QToolButton *DebugButton;

    ScriptManager * m_ScriptManager;

    QString m_Path_Current;
    QString m_Path_OpenFiles;

    QMutex m_MutexQuit;
    QWaitCondition m_WaitConditionQuit;

    bool m_HasCallBackFinished = false;
    std::function<void(void)> m_CallBackFinished;


    inline ScriptTool( ScriptManager * sm, QWidget *parent = 0, const char *name = 0 ): QWidget( parent)
    {
        m_ScriptManager = sm;
        m_Dir = GetSite();

        m_CodeIsRunning.store(false);
        m_CodeIsPaused.store(false);
        m_CodeIsStopRequested.store(false);
        m_CodeIsPauseRequested.store(false);

        m_MainLayout = new QVBoxLayout(this);
        m_MenuWidget = new QWidget(this);
        m_MenuLayout = new QHBoxLayout(m_MenuWidget);


        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();
        icon_debug = new QIcon();

        icon_compile =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_COMPILE),-1));
        icon_indent =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTINDENT),-1));
        icon_indentr =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTINDENTR),-1));
        icon_style =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTSTYLE),-1));
        icon_comment =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTCOMMENT),-1));
        icon_normal =(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_TEXTREGULAR),-1));


        icon_start->addFile((m_Dir + LISEM_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + LISEM_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + LISEM_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_debug->addFile((m_Dir + LISEM_FOLDER_ASSETS + "debug.png"), QSize(), QIcon::Normal, QIcon::Off);


        QToolButton *NewButton = new QToolButton(this);
        NewButton->setIcon(*icon_new);
        NewButton->setIconSize(QSize(22,22));
        NewButton->setMaximumSize(QSize(22,22));
        NewButton->resize(22,22);
        NewButton->setEnabled(true);

        QToolButton *OpenButton = new QToolButton(this);
        OpenButton->setIcon(*icon_open);
        OpenButton->setIconSize(QSize(22,22));
        OpenButton->setMaximumSize(QSize(22,22));
        OpenButton->resize(22,22);
        OpenButton->setEnabled(true);

        QToolButton *SaveButton = new QToolButton(this);
        SaveButton->setIcon(*icon_save);
        SaveButton->setIconSize(QSize(22,22));
        SaveButton->setMaximumSize(QSize(22,22));
        SaveButton->resize(22,22);
        SaveButton->setEnabled(true);

        QToolButton *SaveAsButton = new QToolButton(this);
        SaveAsButton->setIcon(*icon_saveas);
        SaveAsButton->setIconSize(QSize(22,22));
        SaveAsButton->setMaximumSize(QSize(22,22));
        SaveAsButton->resize(22,22);
        SaveAsButton->setEnabled(true);

        StartButton = new QToolButton(this);
        StartButton->setIcon(*icon_start);
        StartButton->setIconSize(QSize(22,22));
        StartButton->setMaximumSize(QSize(22,22));
        StartButton->resize(22,22);
        StartButton->setEnabled(true);

        PauseButton = new QToolButton(this);
        PauseButton->setIcon(*icon_pause);
        PauseButton->setIconSize(QSize(22,22));
        PauseButton->setMaximumSize(QSize(22,22));
        PauseButton->resize(22,22);
        PauseButton->setEnabled(true);

        StopButton = new QToolButton(this);
        StopButton->setIcon(*icon_stop);
        StopButton->setIconSize(QSize(22,22));
        StopButton->setMaximumSize(QSize(22,22));
        StopButton->resize(22,22);
        StopButton->setEnabled(true);

        DebugButton = new QToolButton(this);
        DebugButton->setIcon(*icon_debug);
        DebugButton->setIconSize(QSize(22,22));
        DebugButton->setMaximumSize(QSize(22,22));
        DebugButton->resize(22,22);
        DebugButton->setEnabled(true);
        DebugButton->setToolTip("Toggles debug view");

        QToolButton *CompileButton = new QToolButton(this);
        CompileButton->setIcon(*icon_compile);
        CompileButton->setIconSize(QSize(22,22));
        CompileButton->setMaximumSize(QSize(22,22));
        CompileButton->resize(22,22);
        CompileButton->setEnabled(true);
        CompileButton->setToolTip("Compile the script, but do not run it");


        QToolButton *IndentButton = new QToolButton(this);
        IndentButton->setIcon(*icon_indent);
        IndentButton->setIconSize(QSize(22,22));
        IndentButton->setMaximumSize(QSize(22,22));
        IndentButton->resize(22,22);
        IndentButton->setEnabled(true);
        IndentButton->setToolTip("Add a tab to the beginning of selected lines");


        QToolButton *IndentRButton = new QToolButton(this);
        IndentRButton->setIcon(*icon_indentr);
        IndentRButton->setIconSize(QSize(22,22));
        IndentRButton->setMaximumSize(QSize(22,22));
        IndentRButton->resize(22,22);
        IndentRButton->setEnabled(true);
        IndentRButton->setToolTip("Remove a tab to the beginning of selected lines");


        QToolButton *StyleButton = new QToolButton(this);
        StyleButton->setIcon(*icon_style);
        StyleButton->setIconSize(QSize(22,22));
        StyleButton->setMaximumSize(QSize(22,22));
        StyleButton->resize(22,22);
        StyleButton->setEnabled(true);
        StyleButton->setToolTip("Auto-Style and format the code");


        QToolButton *CommentButton = new QToolButton(this);
        CommentButton->setIcon(*icon_comment);
        CommentButton->setIconSize(QSize(22,22));
        CommentButton->setMaximumSize(QSize(22,22));
        CommentButton->resize(22,22);
        CommentButton->setEnabled(true);
        CommentButton->setToolTip("Add double slashes to the beginning of selected lines");

        QToolButton *RegularButton = new QToolButton(this);
        RegularButton->setIcon(*icon_normal);
        RegularButton->setIconSize(QSize(22,22));
        RegularButton->setMaximumSize(QSize(22,22));
        RegularButton->resize(22,22);
        RegularButton->setEnabled(true);
        RegularButton->setToolTip("Remove double slashes to the beginning of selected lines");



        m_LabelFile = new QLabel();
        QMovie *movie = new QMovie(m_Dir + LISEM_FOLDER_ASSETS + "loader_1.gif");
        m_LabelFile ->setMovie(movie);
        movie->start();
        m_LabelFile->setVisible(false);

        m_MenuLayout->addWidget(NewButton);
        m_MenuLayout->addWidget(OpenButton);
        m_MenuLayout->addWidget(SaveButton);
        m_MenuLayout->addWidget(SaveAsButton);

        m_MenuLayout->addWidget(StartButton);
        m_MenuLayout->addWidget(PauseButton);
        m_MenuLayout->addWidget(StopButton);
        m_MenuLayout->addWidget(DebugButton);

        m_MenuLayout->addWidget(CompileButton);
        m_MenuLayout->addWidget(IndentButton);
        m_MenuLayout->addWidget(IndentRButton);
        m_MenuLayout->addWidget(CommentButton);
        m_MenuLayout->addWidget(RegularButton);
        m_MenuLayout->addWidget(StyleButton);

        m_ScriptTabs = new QTabWidget();



        m_ScriptTabs->setTabsClosable(true);

        this->setLayout(m_MainLayout);

        QWidget * m_fill = new QWidget();
        m_fill->setMinimumSize(QSize(100,20));
        m_fill->setMaximumSize(QSize(100,20));

        m_MenuLayout->stretch(0);
        m_MenuWidget->setMaximumHeight(25);
        m_MainLayout->addWidget(m_MenuWidget);

        m_StackDisplay = new StackDisplay();
        m_DebugSplitter = new QSplitter();


        m_DebugSplitter->addWidget(m_ScriptTabs);
        m_DebugSplitter->addWidget(m_StackDisplay);
        m_DebugSplitter->setSizes({1,0});

        m_DebugSplitter->blockSignals(true);
        m_DebugSplitter->handle(0)->blockSignals(true);

        m_MainLayout->addWidget(m_DebugSplitter);

        m_MainLayout->setMargin(2);
        m_MainLayout->setSpacing(2);

        m_DebugSplitter->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Ignored));


        //QSpacerItem * si = new QSpacerItem(10000,1);
        //m_MenuLayout->addItem(si);

        m_MenuLayout->addWidget(m_fill);
        m_MenuLayout->addWidget(m_LabelFile);
        m_MenuLayout->addStretch();

        m_MenuLayout->setMargin(0);
        m_MenuLayout->setSpacing(0);


        //all connections
        connect(SaveButton,SIGNAL(clicked()),this,SLOT(SaveCode()));
        connect(SaveAsButton,SIGNAL(clicked()),this,SLOT(SaveAsCode()));
        connect(NewButton,SIGNAL(clicked()),this,SLOT(NewCode()));
        connect(OpenButton,SIGNAL(clicked()),this,SLOT(OpenCode()));

        connect(StartButton,SIGNAL(clicked()),this,SLOT(OnRequestRunCode()));
        connect(PauseButton,SIGNAL(clicked()),this,SLOT(OnRequestPauseCode()));
        connect(StopButton,SIGNAL(clicked()),this,SLOT(OnRequestStopCode()));

        connect(DebugButton,SIGNAL(clicked()),this,SLOT(OnDebugView()));

        connect(CompileButton,SIGNAL(clicked()),this,SLOT(OnRequestCompileCode()));
        connect(IndentButton,SIGNAL(clicked()),this,SLOT(OnRequestIndentCode()));
        connect(IndentRButton,SIGNAL(clicked()),this,SLOT(OnRequestIndentRCode()));
        connect(CommentButton,SIGNAL(clicked()),this,SLOT(OnRequestCommentCode()));
        connect(RegularButton,SIGNAL(clicked()),this,SLOT(OnRequestRegularCode()));
        connect(StyleButton,SIGNAL(clicked()),this,SLOT(OnRequestStyleCode()));


        connect(m_ScriptTabs,SIGNAL(tabCloseRequested(int)),this,SLOT(CloseFile(int)));
        connect(m_ScriptTabs,SIGNAL(currentChanged(int)),this,SLOT(OnIndexChanged(int)));

        //open list of open file names
        m_Path_Current = GetSite()+"/";
        m_Path_OpenFiles = m_Path_Current + "openfiles.ini";

        int index_read = 0;

        QFile fin(m_Path_OpenFiles);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
            {
                QTextStream out(&fin);
                out << QString("[Open Script Files]\n");
            }
            fin.close();
        }else
        {
            while (!fin.atEnd())
            {
                QString S = fin.readLine().trimmed();
                QFile fin_temp(S);
                if(S.startsWith("[open index]="))
                {
                    QStringList SL = S.split(QRegExp("="));
                    if(SL.length() > 1)
                    {
                        bool ok;
                        index_read = SL.at(1).toInt(&ok);
                        if(!ok)
                        {
                            index_read = 0;
                        }
                    }

                }else if(!S.startsWith("["))
                {
                    if (fin_temp.open(QFile::ReadOnly | QFile::Text))
                    {
                        fin_temp.close();

                        CodeEditor * ce = new CodeEditor(this,m_ScriptManager);
                        connect(ce,SIGNAL(OnEditedSinceSave()),this,SLOT(OnTitleChanged()));


                        m_ScriptTabs->addTab(ce,"");
                        m_ScriptTabs->setCurrentIndex(m_ScriptTabs->count()-1);

                        ce->SetFile(S);
                    }
                }
            }
        }

        this->m_ScriptTabs->setCurrentIndex(index_read);
        ExportOpenFileList();

        StartButton->setEnabled(true);
        PauseButton->setEnabled(false);
        StopButton->setEnabled(false);

        connect(this,&ScriptTool::int_update_buttons_stop,this,&ScriptTool::int_buttons_onstop,Qt::ConnectionType::QueuedConnection);
        connect(this,&ScriptTool::int_update_buttons_start,this,&ScriptTool::int_buttons_onstart,Qt::ConnectionType::QueuedConnection);
        connect(this,&ScriptTool::int_update_buttons_pause,this,&ScriptTool::int_buttons_onpause,Qt::ConnectionType::QueuedConnection);

        connect(QApplication::instance(), &QCoreApplication::aboutToQuit,[this](){

            m_MutexQuit.lock();

            if(m_CodeIsRunning.load())
            {
                m_CodeIsStopRequested.store(true);

                m_PauseMutex.lock();
                if(m_CodeIsPaused.load())
                {
                    m_PauseMutex.unlock();
                    //re-start code
                    m_PauseWaitCondition.notify_all();
                }else
                {
                    m_PauseMutex.unlock();
                }


                m_WaitConditionQuit.wait(&m_MutexQuit);

                m_MutexQuit.unlock();
            }else
            {
                m_MutexQuit.unlock();
            }

            ;

        });


        m_contextmenu = new QMenu();


        QAction * closeAct = new QAction(*icon_new,tr("Close"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        closeAct->setToolTip(tr("Close this file"));
        connect(closeAct, &QAction::triggered, this, &ScriptTool::CMCloseTab);

        QAction * newAct = new QAction(*icon_new,tr("New"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        newAct->setToolTip(tr("New file"));
        connect(newAct, &QAction::triggered, this, &ScriptTool::CMNewTab);

        QAction * saveAct = new QAction(*icon_new,tr("Save"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setToolTip(tr("Save file"));
        connect(saveAct, &QAction::triggered, this, &ScriptTool::CMSave);


        QAction * closeallAct = new QAction(*icon_new,tr("Close all"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        closeallAct->setToolTip(tr("Close all"));
        connect(closeallAct, &QAction::triggered, this, &ScriptTool::CMCloseAll);

        QAction * closeallbutAct = new QAction(*icon_new,tr("Close all but this"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        closeallbutAct->setToolTip(tr("Close all but this file"));
        connect(closeallbutAct, &QAction::triggered, this, &ScriptTool::CMCloseAllBut);


        m_contextmenu->addAction(closeAct);
        m_contextmenu->addAction(newAct);
        m_contextmenu->addAction(saveAct);
        m_contextmenu->addAction(closeallAct);
        m_contextmenu->addAction(closeallbutAct);

        m_ScriptTabs->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_ScriptTabs->tabBar(), SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(on_contextMenuRequested(const QPoint &)));


    }

    inline ~ScriptTool()
    {


    }

    inline void ExportOpenFileList()
    {
        QFile fin(m_Path_OpenFiles);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            out << QString("[Runfile List]\n");
            out << "[open index]=" << QString::number(m_ScriptTabs->currentIndex()) << "\n";
            for(int i = 0; i < m_ScriptTabs->count(); i++)
            {
                CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);

                out << ce->m_File << "\n";
            }
        }
        fin.close();

    }

    std::function<void(QString, int)> m_StartCallBack;
    bool m_StartCallBackSet = false;

    template<typename _Callable, typename... _Args>
    inline void SetCallBackStart(_Callable&& __f, _Args&&... __args)
    {
        //now we store this function pointer and call it later when a file is openened
        m_StartCallBack = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1,std::placeholders::_2);
        m_StartCallBackSet = true;
    }


    std::function<void(QString, int)> m_StopCallBack;
    bool m_StopCallBackSet = false;

    template<typename _Callable, typename... _Args>
    inline void SetCallBackStop(_Callable&& __f, _Args&&... __args)
    {
        //now we store this function pointer and call it later when a file is openened
        m_StopCallBack = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1,std::placeholders::_2);
        m_StopCallBackSet = true;
    }


    QString m_HomeDir;

    inline void SetHomeDir(QString dir)
    {
        m_HomeDir = dir;
        for(int i = 0;i < m_ScriptTabs->count();i++)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);
            ce->SetHomeDir(dir);

        }
    }

    inline void int_emitupdatebuttons_start()
    {
        emit int_update_buttons_start();
    }
    inline void int_emitupdatebuttons_pause()
    {
        emit int_update_buttons_pause();
    }
    inline void int_emitupdatebuttons_stop()
    {
        emit int_update_buttons_stop();
    }


    int m_LastIndexContex = -1;

signals:


    inline void int_update_buttons_start();
    inline void int_update_buttons_pause();
    inline void int_update_buttons_stop();

public slots:

    inline void CMCloseTab()
    {
        if(m_LastIndexContex > -1)
        {
            CloseFile(m_LastIndexContex);
        }
        m_LastIndexContex = -1;
    }
    inline void CMNewTab()
    {
        if(m_LastIndexContex > -1)
        {
            NewCode();
        }
        m_LastIndexContex = -1;
    }
    inline void CMCloseAll()
    {
        if(m_LastIndexContex > -1)
        {
            for(int i = m_ScriptTabs->count()-1; i > -1; i--)
            {
                //if(i != m_LastIndexContex)
                {
                    CloseFile(i);
                }
            }
        }
        m_LastIndexContex = -1;
    }
    inline void CMCloseAllBut()
    {
        if(m_LastIndexContex > -1)
        {
            for(int i = m_ScriptTabs->count()-1; i > -1; i--)
            {
                if(i != m_LastIndexContex)
                {
                    CloseFile(i);
                }
            }
        }
        m_LastIndexContex = -1;
    }

    inline void CMSave()
    {
        if(m_LastIndexContex > -1)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_LastIndexContex );
            if(ce != nullptr)
            {
                ce->SaveFile(false);
            }

            ExportOpenFileList();
        }
        m_LastIndexContex = -1;

    }
    inline void on_contextMenuRequested(const QPoint &point)
    {
        if (point.isNull())
        {
            return;
        }

        int tabIndex = m_ScriptTabs->tabBar()->tabAt(point);

        m_LastIndexContex = tabIndex;
        m_contextmenu->popup(QCursor::pos());
    }


    inline void int_buttons_onpause()
    {
        if(m_StopCallBackSet)
        {
            m_StopCallBack("",0);
        }

        StartButton->setEnabled(true);
        PauseButton->setEnabled(false);
        StopButton->setEnabled(true);
        m_LabelFile->setVisible(true);
    }
    inline void int_buttons_onstop()
    {
        if(m_StopCallBackSet)
        {
            m_StopCallBack("",0);
        }

        StartButton->setEnabled(true);
        PauseButton->setEnabled(false);
        StopButton->setEnabled(false);
        m_LabelFile->setVisible(false);
    }
    inline void int_buttons_onstart()
    {
        if(m_StartCallBackSet)
        {
            m_StartCallBack("",0);
        }

        StartButton->setEnabled(false);
        PauseButton->setEnabled(true);
        StopButton->setEnabled(true);
        m_LabelFile->setVisible(true);
    }
    inline void OnIndexChanged(int x)
    {

        ExportOpenFileList();
    }

    inline void SaveCode()
    {
        if(m_ScriptTabs->currentIndex() < 0)
        {
            return;
        }
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
        ce->SaveFile(false);

        ExportOpenFileList();
    }

    inline void SaveAsCode()
    {
        if(m_ScriptTabs->currentIndex() < 0)
        {
            return;
        }
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
         ce->SaveFileAs();

         ExportOpenFileList();
    }

    inline void NewCode()
    {
        CodeEditor * ce = new CodeEditor(this,m_ScriptManager);
        connect(ce,SIGNAL(OnEditedSinceSave()),this,SLOT(OnTitleChanged()));
        ce->SetHomeDir(m_HomeDir);
        m_ScriptTabs->addTab(ce,"");
        m_ScriptTabs->setCurrentIndex(m_ScriptTabs->count()-1);

        ce->SetEmpty();

    }

    inline void OpenAndRunCode(QString path, std::function<void(void)> onfinish)
    {

        m_HasCallBackFinished = true;
        m_CallBackFinished = onfinish;

        OpenCode(path);
        OnRequestRunCode();
    }

    inline void OpenAndCompileCode(QString path, std::function<void(void)> onfinish)
    {

        m_HasCallBackFinished = true;
        m_CallBackFinished = onfinish;

        OpenCode(path);
        OnRequestCompileCode();
    }



    inline void OpenCode(QString path)
    {
        bool found = false;
        QList<QString> list;
        for(int i = 0;i < m_ScriptTabs->count();i++)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);
            QString name = ce->m_FileName;
            if(name == path)
            {

                m_ScriptTabs->setCurrentIndex(i);
                break;
            }

        }

        if(!found)
        {
            CodeEditor * ce = new CodeEditor(this,m_ScriptManager);
            ce->SetHomeDir(m_HomeDir);
            m_ScriptTabs->addTab(ce,"");
            m_ScriptTabs->setCurrentIndex(m_ScriptTabs->count()-1);
            ce->LoadFileDirect(path);
            ExportOpenFileList();
            OnTitleChanged();
        }
    }
    inline void OpenCode()
    {
        QString openDir = GetSite();

        CodeEditor * ce = new CodeEditor(this,m_ScriptManager);
        connect(ce,SIGNAL(OnEditedSinceSave()),this,SLOT(OnTitleChanged()));
        ce->SetHomeDir(m_HomeDir);

        m_ScriptTabs->addTab(ce,"");
        m_ScriptTabs->setCurrentIndex(m_ScriptTabs->count()-1);


        ce->LoadFile(openDir);

        ExportOpenFileList();
        OnTitleChanged();
    }

    inline void CloseFile(int index)
    {
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(index);
        if(ce == nullptr)
        {
            return;
        }

        bool close = true;
        if(ce->m_HasBeenEditedSinceSave)
        {
            close = false;
            if (QMessageBox::Yes == QMessageBox::question(this, "Close?", "File has been changed, Close?", QMessageBox::Yes | QMessageBox::No))
            {
                close = true;
            }

        }

        if(close)
        {
            m_ScriptTabs->removeTab(index);

        }

        ExportOpenFileList();

    }

    inline void OnRequestRunCode()
    {
        std::cout <<  m_ScriptTabs->currentIndex() << std::endl;
        if(m_ScriptTabs->currentIndex() < 0)
        {
            return;
        }

        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());

            if(ce == nullptr)
            {
                return;
            }
            ce->SetHighlightErrorLocation(-1,-1);
        }



        //get wether a code is running

        bool is_running = m_CodeIsRunning.load();

        if(is_running)
        {
            m_PauseMutex.lock();
            if(m_CodeIsPaused.load())
            {
                m_PauseMutex.unlock();
                //re-start code
                m_PauseWaitCondition.notify_all();
            }else
            {
                m_PauseMutex.unlock();
            }



        }else
        {

            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());


            ce->SetHighlightErrorLocation( -1, -1);

            QString command = ce->document()->toPlainText();

            SPHScript *s = new SPHScript();
            s->SetCode(command);
            s->SetSingleLine(false);
            s->SetPreProcess(true);
            s->SetHomeDir(m_HomeDir+"/");

            s->SetCallBackPrint(std::function<void(ScriptTool *,SPHScript*,QString)>([](ScriptTool *,SPHScript*,QString) ->
                                                                        void{


                ;
                                                                        }),this,s,std::placeholders::_1);

            s->SetCallBackDone(std::function<void(ScriptTool *,SPHScript*,bool x)>([this,ce](ScriptTool *st,SPHScript*, bool finished) ->
                                                                        void{

                st->m_CodeIsPaused.store(false);
                st->m_CodeIsStopRequested.store(false);
                st->m_CodeIsPauseRequested.store(false);
                st->m_CodeIsRunning.store(false);

                st->int_emitupdatebuttons_stop();


                ce->SetHighlightCurrentRunLine(-1);
                LISEMS_DEBUG("Done");

                                   if(m_StopCallBackSet)
                                   {
                                       m_StopCallBack("",0);
                                   }

                ;
                                                                        }),this,s,std::placeholders::_1);

            s->SetCallBackCompilerError(std::function<void(ScriptTool *,CodeEditor *,SPHScript*,const asSMessageInfo *msg)>([this](ScriptTool *st,CodeEditor * ced,SPHScript*,const asSMessageInfo *msg) ->
                                                                        void{
                const char *type = "Error: ";
                if( msg->type == asMSGTYPE_WARNING )
                {
                    type = "Warning: ";
                }
                else if( msg->type == asMSGTYPE_INFORMATION )
                {
                    type = "Info: ";
                }
                LISEMS_ERROR(QString(type) + " Line: (" + QString::number(msg->row) + " (" + QString::number(msg->col) + ") " + " : " + QString(msg->message));
                ;

                ced->SetHighlightErrorLocation(msg->row, msg->col);

                                            if(m_StopCallBackSet)
                                            {
                                                m_StopCallBack("",0);
                                            }



                                                                        }),this,ce,s,std::placeholders::_1);

            s->SetCallBackException(std::function<void(ScriptTool *,SPHScript*,asIScriptContext *ctx)>([this,ce](ScriptTool *st,SPHScript*,asIScriptContext *ctx) ->
                                                                        void{

                LISEMS_ERROR("Exception encountered when running script");

                                        try
                                          {
                                            // Retrow the original exception so we can catch it again
                                            throw;
                                          }
                                          catch( std::exception &e )
                                          {
                                            // Tell the VM the type of exception that occurred
                                           LISEMS_ERROR("std::exception " +QString(e.what()));
                                            //ctx->SetException(e.what());
                                          }catch(int e)
                                        {
                                           LISEMS_ERROR("int exception " +QString::number(e));
                                         }
                                          catch(...)
                                          {
                                            LISEMS_ERROR("Unknown exception");
                                            // The callback must not allow any exception to be thrown, but it is not necessary
                                            // to explicitly set an exception string if the default exception string is sufficient
                                          }

                                        if(m_StopCallBackSet)
                                        {
                                            m_StopCallBack("",0);
                                        }

                ce->SetHighlightErrorLocation( ctx->GetLineNumber(), -1);
                std::cout << 1  << std::endl;
                st->m_StackDisplay->SetFromContext(ctx,0,true);
                std::cout << 2  << std::endl;
                //pause by setting up waitcondition
                st->m_PauseMutex.lock();
                std::cout << 3  << std::endl;
                st->m_CodeIsPaused.store(true);
                std::cout << 4  << std::endl;
                LISEMS_STATUS("Paused script execution");
                st->int_emitupdatebuttons_pause();
                std::cout << 5  << std::endl;
                st->m_PauseWaitCondition.wait(&(st->m_PauseMutex));
                std::cout << 6  << std::endl;
                st->m_CodeIsPauseRequested.store(false);
                std::cout << 7  << std::endl;
                st->m_CodeIsPaused.store(false);
                std::cout << 8  << std::endl;
                st->int_emitupdatebuttons_start();
                std::cout << 9  << std::endl;
                st->m_PauseMutex.unlock();

                st->m_StackDisplay->Clear();

                ce->SetHighlightErrorLocation( -1, -1);

                ;
                                                                        }),this,s,std::placeholders::_1);

            s->SetCallBackLine(std::function<void(ScriptTool *,SPHScript*,asIScriptContext *ctx)>([ce](ScriptTool * st,SPHScript*,asIScriptContext *ctx) ->
                               void{
                ce->SetHighlightCurrentRunLine(ctx->GetLineNumber());

                if(st->m_CodeIsPauseRequested.load())
                {
                    st->m_StackDisplay->SetFromContext(ctx,0,true);

                    //pause by setting up waitcondition
                    st->m_PauseMutex.lock();
                    st->m_CodeIsPaused.store(true);
                    LISEMS_STATUS("Paused script execution");
                    st->int_emitupdatebuttons_pause();
                    st->m_PauseWaitCondition.wait(&(st->m_PauseMutex));
                    st->m_CodeIsPauseRequested.store(false);
                    st->m_CodeIsPaused.store(false);
                    st->int_emitupdatebuttons_start();
                    st->m_PauseMutex.unlock();


                    st->m_StackDisplay->Clear();

                }

                if(st->m_CodeIsStopRequested.load())
                {
                    //abort execution
                    ctx->Abort();
                    ctx->Suspend();





                    st->m_WaitConditionQuit.notify_all();

                    LISEMS_STATUS("Stopped script execution");
                }

                //check break points
                if(ce->HasBreakPointAt(ctx->GetLineNumber()))
                {

                           st->m_StackDisplay->SetFromContext(ctx,0,true);

                           //pause by setting up waitcondition
                           st->m_PauseMutex.lock();
                           st->m_CodeIsPaused.store(true);
                           LISEMS_STATUS("Paused script execution");
                           st->int_emitupdatebuttons_pause();
                           st->m_PauseWaitCondition.wait(&(st->m_PauseMutex));
                           st->m_CodeIsPauseRequested.store(false);
                           st->m_CodeIsPaused.store(false);
                           st->int_emitupdatebuttons_start();
                           st->m_PauseMutex.unlock();

                           st->m_StackDisplay->Clear();
                }

                ;
                               }),this,s,std::placeholders::_1);

            m_ScriptManager->CompileScript_Generic(s);

            if(s->IsCompiled())
            {
                m_CodeIsPaused.store(false);
                m_CodeIsStopRequested.store(false);
                m_CodeIsPauseRequested.store(false);
                m_CodeIsRunning.store(true);
                ce->SetHighlightCurrentRunLine(0);


                int_emitupdatebuttons_start();

                m_ScriptManager->RunScript(s);
            }else
            {

                LISEMS_STATUS("Compilation Error");
                m_CodeIsPaused.store(false);
                m_CodeIsStopRequested.store(false);
                m_CodeIsPauseRequested.store(false);
                m_CodeIsRunning.store(false);
            }


        }

    }

    inline void OnRequestPauseCode()
    {

        if(m_CodeIsRunning.load())
        {
            if(!m_CodeIsPaused.load())
            {
                m_CodeIsPauseRequested.store(true);
            }
        }

    }

    inline void OnRequestStopCode()
    {
        if(m_CodeIsRunning.load())
        {
            m_CodeIsStopRequested.store(true);

            m_PauseMutex.lock();
            if(m_CodeIsPaused.load())
            {
                m_PauseMutex.unlock();
                //re-start code
                m_PauseWaitCondition.notify_all();
            }else
            {
                m_PauseMutex.unlock();
            }
        }

    }


    inline void OnRequestCompileCode()
    {
        std::cout <<  m_ScriptTabs->currentIndex() << std::endl;
        if(m_ScriptTabs->currentIndex() < 0)
        {
            return;
        }

        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());

            if(ce == nullptr)
            {
                return;
            }
            ce->SetHighlightErrorLocation(-1,-1);
        }



        //get wether a code is running

        bool is_running = m_CodeIsRunning.load();

        if(is_running)
        {
            m_PauseMutex.lock();
            if(m_CodeIsPaused.load())
            {
                m_PauseMutex.unlock();
                //re-start code
                m_PauseWaitCondition.notify_all();
            }else
            {
                m_PauseMutex.unlock();
            }



        }else
        {

            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());


            ce->SetHighlightErrorLocation( -1, -1);

            QString command = ce->document()->toPlainText();

            SPHScript *s = new SPHScript();
            s->SetCode(command);
            s->SetSingleLine(false);
            s->SetPreProcess(true);
            s->SetHomeDir(m_HomeDir+"/");

            //now set up a compile/run request
            s->SetCallBackPrint(std::function<void(SPHScript*,QString)>([](SPHScript*,QString) ->
                                                                        void{


                ;
                                                                        }),s,std::placeholders::_1);

            s->SetCallBackDone(std::function<void(bool x)>([]( bool finished) ->
                                                                        void{


                LISEMS_DEBUG("Done");
                ;
                                                                        }),std::placeholders::_1);

            s->SetCallBackCompilerError(std::function<void(SPHScript*,const asSMessageInfo *msg)>([](SPHScript*,const asSMessageInfo *msg) ->
                                                                        void{
                const char *type = "Error: ";
                if( msg->type == asMSGTYPE_WARNING )
                {
                    type = "Warning: ";
                }
                else if( msg->type == asMSGTYPE_INFORMATION )
                {
                    type = "Info: ";
                }
                LISEMS_ERROR(QString(type) + " Line: (" + QString::number(msg->row) + " (" + QString::number(msg->col) + ") " + " : " + QString(msg->message));
                ;



                                                                        }),s,std::placeholders::_1);

            s->SetCallBackException(std::function<void(SPHScript*,asIScriptContext *ctx)>([](SPHScript*,asIScriptContext *ctx) ->
                                                                        void{

                LISEMS_ERROR("Exception encountered when running script");

                                        try
                                          {
                                            // Retrow the original exception so we can catch it again
                                            throw;
                                          }
                                          catch( std::exception &e )
                                          {
                                            // Tell the VM the type of exception that occurred
                                           LISEMS_ERROR("std::exception " +QString(e.what()));
                                            //ctx->SetException(e.what());
                                          }catch(int e)
                                        {
                                           LISEMS_ERROR("int exception " +QString::number(e));
                                         }
                                          catch(...)
                                          {
                                            LISEMS_ERROR("Unknown exception");
                                            // The callback must not allow any exception to be thrown, but it is not necessary
                                            // to explicitly set an exception string if the default exception string is sufficient
                                          }



                ;
                                                                        }),s,std::placeholders::_1);

            s->SetCallBackLine(std::function<void(SPHScript*,asIScriptContext *ctx)>([](SPHScript*,asIScriptContext *ctx) ->
                               void{


                ;
                               }),s,std::placeholders::_1);

            //compile it

            m_ScriptManager->CompileScript_Generic(s);

            //output errors
            if(s->IsCompiled())
            {
                LISEMS_STATUS("Compiled without errors");
            }else
            {
                LISEMS_STATUS("Errors during compilation");

            }


        }

    }

    inline void OnRequestIndentCode()
    {
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());

        if(ce == nullptr)
        {
            return;
        }
        QString m_tabReplace;
        m_tabReplace.clear();
        m_tabReplace.fill(' ', 4);

        int selectedLines = 0;
        {
            QTextCursor cursor = ce->textCursor();

            if(!cursor.selection().isEmpty())
            {
                cursor.position();
                QString str = cursor.selection().toPlainText();

                selectedLines = str.count("\n")+1;
                str.replace("\n","\n"+ m_tabReplace);
                str.prepend(m_tabReplace);
                cursor.insertText(str);
                cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
                ce->setTextCursor(cursor);
            }
        }



    }
    inline void OnRequestIndentRCode()
    {
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());

        if(ce != nullptr)
        {
            QString m_tabReplace;
            m_tabReplace.clear();
            m_tabReplace.fill(' ', 4);

            int selectedLines = 0;
            {
                QTextCursor cursor = ce->textCursor();

                if(!cursor.selection().isEmpty())
                {
                    cursor.position();
                    QString str = cursor.selection().toPlainText();

                    selectedLines = str.count("\n")+1;
                    str.replace("\n"+ m_tabReplace,"\n");
                     str.replace("\n\t","\n");
                     if(str.startsWith("\t"))
                     {
                         str.remove(0,1);
                     }else if(str.startsWith(m_tabReplace))
                     {
                         str.remove(0,4);
                     }

                    cursor.insertText(str);
                    cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
                    ce->setTextCursor(cursor);
                }
            }
        }

    }
    inline void OnRequestCommentCode()
    {
        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
        if(ce == nullptr)
        {
            return;
        }
        QString m_tabReplace;
        m_tabReplace.clear();
        m_tabReplace.fill(' ', 4);

        int selectedLines = 0;
        {
            QTextCursor cursor = ce->textCursor();

            if(!cursor.selection().isEmpty())
            {
                cursor.position();
                QString str = cursor.selection().toPlainText();

                selectedLines = str.count("\n")+1;

                //insert double forward slashes after all the white-spaces that come after a newline and before a character is encountered

                QRegExp rews("\\s+");
                bool newline_found = false;
                for(int i = 0; i < str.length();i++)
                {
                    QString si = str.at(i);
                    QString sin = "/";
                    if(i < str.length()-1)
                    {
                        sin = str.at(i+1);
                    }
                    if(!(sin == "\n" || sin == "\r\n"))
                    {
                        if(si == "\n" || si == "\r\n")
                        {
                            newline_found = true;

                        }else if(!rews.exactMatch(sin) && newline_found)
                        {
                            str.insert(i,"//");
                            newline_found = false;
                        }
                    }
                }


                str.prepend(m_tabReplace);
                cursor.insertText(str);
                cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
                ce->setTextCursor(cursor);
            }
        }

    }
    inline void OnRequestRegularCode()
    {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
            if(ce == nullptr)
            {
                return;
            }
            QString m_tabReplace;
            m_tabReplace.clear();
            m_tabReplace.fill(' ', 4);

            int selectedLines = 0;
            {
                QTextCursor cursor = ce->textCursor();

                if(!cursor.selection().isEmpty())
                {
                    cursor.position();
                    QString str = cursor.selection().toPlainText();

                    selectedLines = str.count("\n")+1;

                    //remove double forward slashes after all the white-spaces that come after a newline and before a character is encountered


                    QRegExp rews("\\s+");
                    bool newline_found = false;
                    int i = 0;
                    while(i < str.length())
                    {
                        QString si = str.at(i);
                        QString sin = "-";
                        if(i < str.length()-1)
                        {
                            sin = str.at(i+1);
                        }
                        if(!(sin == "\n" || sin == "\r\n"))
                        {
                            if(si == "\n" || si == "\r\n")
                            {
                                newline_found = true;

                            }else if(!rews.exactMatch(sin) && newline_found && si =="/" && sin == "/")
                            {
                                str.remove(i,2);
                                newline_found = false;
                            }
                        }
                        i++;
                    }


                    str.prepend(m_tabReplace);
                    cursor.insertText(str);
                    cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
                    ce->setTextCursor(cursor);
                }
            }


    }
    inline void OnRequestStyleCode()
    {

        //ce->selectAll();

        CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
        if(ce == nullptr)
        {
            return;
        }

        ce->SetHighlightErrorLocation( -1, -1);

        QString command = ce->document()->toPlainText();

        QString scriptn = BeautifyScript(command);
        ce->document()->setPlainText(scriptn);

    }


    inline void OnDebugView()
    {
        if(m_DebugSplitter->sizes()[1] > 0)
        {
            m_DebugSplitter->setSizes({1,0});
        }else
        {
            m_DebugSplitter->setSizes({2,1});

        }

    }

    inline void OnTitleChanged()
    {

        for(int i = 0;i < m_ScriptTabs->count();i++)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);
            QString title;
            title = ce->m_FileName;
            if(ce->m_HasBeenEditedSinceSave == true)
            {
                title = title + "*";
            }
            m_ScriptTabs->setTabText(i,title);
        }

    }


    inline QList<QString> GetUnsavedFileNames()
    {
        QList<QString> list;
        for(int i = 0;i < m_ScriptTabs->count();i++)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);
            QString title = ce->m_FileName;
            if(ce->m_HasBeenEditedSinceSave == true)
            {
                list.append(title);
            }
        }

        return list;
    }

    inline void SaveUnsavedFiles()
    {
        for(int i = 0;i < m_ScriptTabs->count();i++)
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(i);
            QString title = ce->m_FileName;
            if(ce->m_HasBeenEditedSinceSave == true)
            {
                ce->SaveFile(false);
            }
        }
    }

};

#endif // SCRIPTTOOL_H
