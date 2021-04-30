#ifndef DOWNLOADTOOL_H
#define DOWNLOADTOOL_H

#include "defines.h"
#include "QWidget"
#include "QObject"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QLabel"
#include "QProgressBar"
#include "QToolButton"
#include "QStringListModel"
#include "QItemDelegate"
#include "QListWidgetItem"
#include "QLineEdit"
#include "downloadmanager.h"
#include "lsmio.h"
#include "downloaddialog.h"
#include "QMessageBox"
#include "QProgressBar"
#include "widgets/labeledwidget.h"
#include "error.h"
#include "QtXml/QDomDocument"
#include "boundingbox.h"
#include "webgeoservice.h"
#include "site.h"


class LISEM_API DownloadWidget : public QWidget
{

        Q_OBJECT;
public:

    QVBoxLayout * m_MainLayout;
    DownloadTask * m_Task;
    QProgressBar * m_ProgressBar;

    QLabel * m_SizeLabel;
    QLabel * m_StatusLabel;
    bool m_SetSize = false;

    inline DownloadWidget( DownloadTask * task)
    {
        m_Task = task;

        m_MainLayout = new QVBoxLayout();
        m_MainLayout->setSizeConstraint( QLayout::SetFixedSize );
        this->setLayout(m_MainLayout);
        QString nameurl = QUrl(task->Url).fileName();
        QLabel * name = new QLabel(QString(nameurl));
        QFont font = name->font();
        font.setPointSize(13);
        name->setFont(font);

        m_SizeLabel = new QLabel();
        m_StatusLabel = new QLabel();

        m_SizeLabel->setText(" ");
        m_StatusLabel->setText("Waiting to start.. ");

        m_MainLayout->addWidget(name);
        m_MainLayout->addWidget(new QWidgetVDuo(new QLabel("URL: " + task->Url),new QLabel("Path: " + task->file)));
        m_MainLayout->addWidget(new QWidgetHDuo(m_StatusLabel,m_SizeLabel));

        m_ProgressBar = new QProgressBar();
        m_MainLayout->addWidget(m_ProgressBar);

        name->setMinimumSize(100,20);


    }



    bool m_CallBackFinishedSet = false;
    std::function<void(DownloadTask*)> m_CallBackFinished;
    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackFinished(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackFinishedSet = true;
        m_CallBackFinished = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }

public slots:

    inline void OnDownloadStart(DownloadTask*d)
    {

    }

    inline void OnDownloadProgress(DownloadTask*d)
    {
        if(m_SetSize == false)
        {
            m_StatusLabel->setText("Downloading...");
            m_SetSize = true;
            m_SizeLabel->setText(" Total Size: " +QString::number((double)d->bytes_total/(1024.0*1024.0)) + " mb");
        }

        m_ProgressBar->setRange(0,(int)d->bytes_total);
        m_ProgressBar->setValue((int)d->bytes_received);

    }

    inline void OnDownloadFinish(DownloadTask*d)
    {

        //did we succeed?
        if(QNetworkReply::NetworkError::NoError == d->Download->error())
        {

            m_StatusLabel->setText("Finished");

        //otherwise mention this to the user
        }else {
            m_StatusLabel->setText("Error: " + d->Error);
        }

        if(m_CallBackFinishedSet)
        {
            m_CallBackFinished(m_Task);
        }

    }

};


class LISEM_API DownloadManagerWidget : public QWidget
{

        Q_OBJECT;
public:

    QLineEdit * m_SearchEdit;
    QListWidget * m_DownloadList;
    QVBoxLayout * m_MainLayout;


    QMutex m_MutexList;
    QList<QListWidgetItem *> m_ListItems;
    QList<DownloadWidget * > m_ListWidgets;
    QList<QString> m_SearchText;

    QMutex m_MutexScript;
    QList<DownloadTask *> m_TasksFromScriptToBeAdded;

    DownloadManager * m_DownloadManager;


signals:

void int_onscriptdownloadsdone();
void int_downloadaddedfromscript();


public:


    inline DownloadManagerWidget()
    {
        m_MainLayout = new QVBoxLayout();
        this->setLayout(m_MainLayout);

        m_DownloadManager = GetDownloadManager();

        QString m_Dir = GetSite();

        m_SearchEdit = new QLineEdit();

        connect(m_SearchEdit, SIGNAL(textChanged(QString)),this,SLOT(OnSearchChanged(QString)));


        m_DownloadList = new QListWidget();

        QWidget * tempwidget = new QWidget();
        QHBoxLayout * templayout = new QHBoxLayout();
        tempwidget->setLayout(templayout);

        QIcon iconD;
        iconD.addFile((m_Dir + LISEM_FOLDER_ASSETS + "download.png"), QSize(), QIcon::Normal, QIcon::Off);

        QToolButton * m_DownloadButton = new QToolButton();
        m_DownloadButton->setIcon(iconD);
        m_DownloadButton->setText("New Download");
        m_DownloadButton->setIconSize(QSize(22,22));
        m_DownloadButton->resize(22,22);
        m_DownloadButton->setEnabled(true);

        connect(m_DownloadButton,&QToolButton::clicked,this,&DownloadManagerWidget::OnAddDownload,Qt::ConnectionType::QueuedConnection);

        connect(this,&DownloadManagerWidget::int_downloadaddedfromscript,this,&DownloadManagerWidget::OnDownloadAddedFromScript,Qt::ConnectionType::QueuedConnection);

        templayout->addWidget(m_DownloadButton);
        templayout->addWidget(m_SearchEdit);


        m_MainLayout->addWidget(tempwidget);
        m_MainLayout->addWidget(m_DownloadList);



    }

    inline void OnDownloadFinished(DownloadTask * task)
    {
        m_MutexList.lock();

        for(int i = 0; i < m_ListWidgets.size(); i++)
        {
            DownloadTask * t = m_ListWidgets.at(i)->m_Task;
            t->FinishMutex.lock();
            if(!t->IsFinished && t->IsScript)
            {
                if(!t->Error.isEmpty())
                {
                    LISEMS_ERROR("Download error for : " + task->Url + " " + task->Error);
                }
                return;
            }
            t->FinishMutex.unlock();
        }



        m_MutexList.unlock();


        emit int_onscriptdownloadsdone();


    }

    inline DownloadWidget *AddDownloadTaskToList(DownloadTask * task)
    {

        m_MutexList.lock();

        QListWidgetItem * item = new QListWidgetItem();
        DownloadWidget * dw = new DownloadWidget(task);

        item->setSizeHint(dw->sizeHint());
        m_DownloadList->addItem(item);
        m_DownloadList->setItemWidget(item,dw);
        m_SearchText.append(task->Url + " " + task->file);


        task->SetCallBackStart(&DownloadWidget::OnDownloadStart,dw);
        task->SetCallBackProgress(&DownloadWidget::OnDownloadProgress,dw);
        task->SetCallBackFinished(&DownloadWidget::OnDownloadFinish,dw);

        dw->SetCallBackFinished(&DownloadManagerWidget::OnDownloadFinished,this);

        m_MutexList.unlock();

        return dw;

    }


    inline void AddDownloadTask(QString Url, QString path, QString Username = "", QString Password = "", int timeout = -1)
    {
        DownloadTask * task = new DownloadTask(Url,path,Username,Password,timeout);
        if(task->CanBeSaved())
        {


            DownloadWidget * widget = AddDownloadTaskToList(task);

            m_DownloadManager->AddDownload(task);
        }else {

            QMessageBox msgBox;
            msgBox.setText("This location can not be used to save a file: " + path);
            msgBox.exec();
        }

    }

    inline bool AS_Intern_AddDownload(QString url, QString file, bool wait, int timeout, QString username, QString password, bool abspath, bool post = false, QString postdata = "")
    {
        DownloadTask * task = new DownloadTask(url,AS_DIR + file,username,password,timeout,true);

        task->HasPost = post;
        task->PostData = postdata;

        if(wait)
        {
            GetDownloadManager()->AddDownloadAndWait(task);

        }else
        {
            if(task->CanBeSaved())
            {
                m_MutexScript.lock();
                m_TasksFromScriptToBeAdded.prepend(task);
                m_MutexScript.unlock();

                emit int_downloadaddedfromscript();
            }else {

                LISEMS_ERROR("This location can not be used to save a file: " + file);
                throw 1;
            }
        }

        return true;
    }

    inline AS_ByteArray * AS_Download(QString url, QString post, int timeout, QString username, QString password)
    {
        DownloadTask * task = new DownloadTask(url,"",username,password,timeout);

            task->shouldbesaved = false;
            m_DownloadManager->AddDownloadAndWait(task);

        AS_ByteArray * ba = new AS_ByteArray();
        ba->data = task->m_Data;
        return ba;
    }

    inline bool AS_AddDownloadPost(QString url, QString file, QString post, bool wait, int timeout, QString username, QString password)
    {
        return AS_Intern_AddDownload(url, file,wait,timeout,username,password,false,true,post);
    }

    inline bool AS_AddDownloadAbsPathPost(QString url, QString file, QString post, bool wait, int timeout, QString username, QString password)
    {
        return AS_Intern_AddDownload(url, file,wait,timeout,username,password,true,true,post);
    }

    inline bool AS_AddDownload(QString url, QString file, bool wait, int timeout, QString username, QString password)
    {
        return AS_Intern_AddDownload(url, file,wait,timeout,username,password,false);
    }

    inline bool AS_AddDownloadAbsPath(QString url, QString file, bool wait, int timeout, QString username, QString password)
    {
        return AS_Intern_AddDownload(url, file,wait,timeout,username,password,true);
    }


    inline bool AS_WaitForDownloads(int timeout = 0)
    {


        std::cout << "wait for all "<< std::endl;

        m_MutexScript.lock();

        m_MutexList.lock();


        bool alldone = true;

        for(int i = m_TasksFromScriptToBeAdded.length()-1; i > -1; i--)
        {
            DownloadTask * t =  m_TasksFromScriptToBeAdded.at(i);

            std::cout << "download " << i << t->IsFinished << " "<<t->IsScript <<std::endl;
            if(!t->IsFinished && t->IsScript)
            {
                alldone =false;
            }
        }

        for(int i = 0; i < m_ListWidgets.size(); i++)
        {
            DownloadTask * t = m_ListWidgets.at(i)->m_Task;
            t->FinishMutex.lock();
            std::cout << "download " << i << t->IsFinished << " "<<t->IsScript <<std::endl;
            if(!t->IsFinished && t->IsScript)
            {
                alldone =false;
            }
            t->FinishMutex.unlock();
        }


        m_MutexList.unlock();


        m_MutexScript.unlock();

        if(alldone)
        {
            return true;
        }

        std::cout << "start wait"<< std::endl;
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        connect( this, &DownloadManagerWidget::int_onscriptdownloadsdone, &loop, &QEventLoop::quit );
        connect( &timer, &QTimer::timeout, &loop, &QEventLoop::quit );
        if(timeout > 0)
        {
            timer.start(timeout);
        }

        loop.exec();

        return true;
    }

    inline void RegisterScriptFunctions(ScriptManager * sm)
    {

        int r = 1;

        r = sm->m_Engine->RegisterObjectType("ByteArray",0,asOBJ_REF);//Shapes

        //register reference counting for garbage collecting
        r = sm->m_Engine->RegisterObjectBehaviour("ByteArray",asBEHAVE_ADDREF,"void f()",asMETHOD(AS_ByteArray,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
        r = sm->m_Engine->RegisterObjectBehaviour("ByteArray",asBEHAVE_RELEASE,"void f()",asMETHOD(AS_ByteArray,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
        r = sm->m_Engine->RegisterObjectMethod("ByteArray", "ByteArray& opAssign(ByteArray &in m)", asMETHOD(AS_ByteArray,AS_Assign), asCALL_THISCALL); assert( r >= 0 );
        r = sm->m_Engine->RegisterObjectBehaviour("ByteArray",asBEHAVE_FACTORY,"ByteArray@ CSF0()",asFUNCTIONPR(AS_ByteArrayFactory,(),AS_ByteArray *),asCALL_CDECL); assert( r >= 0 );



        sm->m_Engine->RegisterGlobalFunction("ByteArray @Download(string url, string post = \"\", int timeout = 0, string username = \"\", string password = \"\")", asMETHODPR(DownloadManagerWidget,AS_Download,(QString,QString, int, QString, QString),AS_ByteArray*),  asCALL_THISCALL_ASGLOBAL,this);
        sm->m_Engine->RegisterGlobalFunction("bool DownloadFilePost(string url, string filename, string post, bool wait = false, int timeout = 0, string username = \"\", string password = \"\")", asMETHODPR(DownloadManagerWidget,AS_AddDownloadPost,(QString,QString,QString, bool, int, QString, QString),bool),  asCALL_THISCALL_ASGLOBAL,this);
        sm->m_Engine->RegisterGlobalFunction("bool DownloadFileAbsPathPost(string url, string filename, string post, bool wait = false, int timeout = 0, string username = \"\", string password = \"\")", asMETHODPR(DownloadManagerWidget,AS_AddDownloadAbsPathPost,(QString,QString, QString, bool, int, QString, QString),bool),  asCALL_THISCALL_ASGLOBAL,this);
        sm->m_Engine->RegisterGlobalFunction("bool DownloadFile(string url, string filename, bool wait = false, int timeout = 0, string username = \"\", string password = \"\")", asMETHODPR(DownloadManagerWidget,AS_AddDownload,(QString,QString, bool, int, QString, QString),bool),  asCALL_THISCALL_ASGLOBAL,this);
        sm->m_Engine->RegisterGlobalFunction("bool DownloadFileAbsPath(string url, string filename,bool wait = false, int timeout = 0, string username = \"\", string password = \"\")", asMETHODPR(DownloadManagerWidget,AS_AddDownloadAbsPath,(QString,QString, bool, int, QString, QString),bool),  asCALL_THISCALL_ASGLOBAL,this);
        sm->m_Engine->RegisterGlobalFunction("bool WaitForAllDownloads(int timeout = 0)", asMETHODPR(DownloadManagerWidget,AS_WaitForDownloads,(int),bool),  asCALL_THISCALL_ASGLOBAL,this);

        sm->m_Engine->RegisterGlobalFunction("string ToText(const ByteArray &in ba)", asFUNCTION( ToText),  asCALL_CDECL);
        sm->m_Engine->RegisterGlobalFunction("string ToXML(const ByteArray &in ba)", asFUNCTION( ToXML),  asCALL_CDECL);
        sm->m_Engine->RegisterGlobalFunction("array<Map> @ToMap(const ByteArray &in ba, const string &in format)",asFUNCTION(ToMap),asCALL_CDECL);

        sm->m_Engine->RegisterGlobalFunction("string OWSCapabilities(const string &in xml)", asFUNCTION( GetOWSCapabilitiesType),  asCALL_CDECL);



    }

public slots:


    inline void OnDownloadAddedFromScript()
    {
        m_MutexScript.lock();

        for(int i = m_TasksFromScriptToBeAdded.length()-1; i > -1; i--)
        {

            DownloadTask * t =  m_TasksFromScriptToBeAdded.at(i);

            DownloadTask * task = new DownloadTask(t->Url,t->file,t->UserName,t->Password,t->timeout,t->IsScript);
            task->PostData = t->PostData;
            task->HasPost = t->HasPost;

            DownloadWidget * widget = AddDownloadTaskToList(task);
            m_DownloadManager->AddDownload(task);

            m_TasksFromScriptToBeAdded.removeAt(i);

        }

        m_MutexScript.unlock();

    }

    inline void OnAddDownload(bool)
    {
        DownloadDialog * dialog = new DownloadDialog(m_SearchEdit->text());
        int res = dialog->exec();

        if(res = QDialog::Accepted)
        {
            AddDownloadTask(dialog->m_Url,dialog->m_File,dialog->m_Username,dialog->m_Password,dialog->do_time? dialog->time : -1);
        }else {

        }

        delete dialog;
        dialog = nullptr;
    }

    inline void OnSearchChanged(QString t)
    {

        m_MutexList.lock();

        if(t.isEmpty())
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                m_DownloadList->setItemHidden(m_ListItems.at(i),false);
            }
        }else
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                bool find = m_SearchText.at(i).contains(t,Qt::CaseSensitivity::CaseInsensitive);
                m_DownloadList->setItemHidden(m_ListItems.at(i),!find);
            }
        }

        m_MutexList.unlock();
    }


};


#endif // DOWNLOADTOOL_H
