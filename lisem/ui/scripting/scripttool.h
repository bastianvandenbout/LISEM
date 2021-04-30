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


signals:


    inline void int_update_buttons_start();
    inline void int_update_buttons_pause();
    inline void int_update_buttons_stop();

public slots:

    inline void int_buttons_onpause()
    {
        StartButton->setEnabled(true);
        PauseButton->setEnabled(false);
        StopButton->setEnabled(true);
        m_LabelFile->setVisible(true);
    }
    inline void int_buttons_onstop()
    {
        StartButton->setEnabled(true);
        PauseButton->setEnabled(false);
        StopButton->setEnabled(false);
        m_LabelFile->setVisible(false);
    }
    inline void int_buttons_onstart()
    {
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
    inline void OpenCode(QString path)
    {
        CodeEditor * ce = new CodeEditor(this,m_ScriptManager);
        ce->SetHomeDir(m_HomeDir);
        m_ScriptTabs->addTab(ce,"");
        m_ScriptTabs->setCurrentIndex(m_ScriptTabs->count()-1);
        ce->LoadFileDirect(path);
        ExportOpenFileList();
        OnTitleChanged();
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
        {
            CodeEditor * ce = (CodeEditor *)m_ScriptTabs->widget(m_ScriptTabs->currentIndex());
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

            s->SetCallBackDone(std::function<void(ScriptTool *,SPHScript*,bool x)>([ce](ScriptTool *st,SPHScript*, bool finished) ->
                                                                        void{

                st->m_CodeIsPaused.store(false);
                st->m_CodeIsStopRequested.store(false);
                st->m_CodeIsPauseRequested.store(false);
                st->m_CodeIsRunning.store(false);

                st->int_emitupdatebuttons_stop();


                ce->SetHighlightCurrentRunLine(-1);
                LISEMS_DEBUG("Done");
                ;
                                                                        }),this,s,std::placeholders::_1);

            s->SetCallBackCompilerError(std::function<void(ScriptTool *,CodeEditor *,SPHScript*,const asSMessageInfo *msg)>([](ScriptTool *st,CodeEditor * ced,SPHScript*,const asSMessageInfo *msg) ->
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
                LISEMS_ERROR(QString(type) + QString(msg->section) + " (" + QString::number(msg->row) + ", " + QString::number(msg->col) + ") " + " : " + QString(msg->message));
                ;

                ced->SetHighlightErrorLocation(msg->row, msg->col);


                                                                        }),this,ce,s,std::placeholders::_1);

            s->SetCallBackException(std::function<void(ScriptTool *,SPHScript*,asIScriptContext *ctx)>([ce](ScriptTool *st,SPHScript*,asIScriptContext *ctx) ->
                                                                        void{

                LISEMS_ERROR("Exception encountered when running script");

                ce->SetHighlightErrorLocation( ctx->GetLineNumber(), -1);

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
