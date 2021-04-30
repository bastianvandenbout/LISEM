#include "downloadmanager.h"


DownloadManager * CDownloadManager;


DownloadManager::DownloadManager()
{
    m_TaskMutex = new QMutex();
    m_TaskMutexNW = new QMutex();
    thread = QThread::currentThread();

    manager = new QNetworkAccessManager();
    managernw = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)),this , SLOT(downloadFinished(QNetworkReply*)));
    connect(this,SIGNAL(OnTaskAdded()),this,SLOT(ProcessDownloads()),Qt::ConnectionType::QueuedConnection);
    connect(this,SIGNAL(OnTaskAddedNW()),this,SLOT(ProcessDownloadsNW()),Qt::ConnectionType::QueuedConnection);
}



void DownloadManager::StartDownload(DownloadTask * t, bool wait)
{

    QNetworkRequest * request = new QNetworkRequest(t->Url);
    t->OnStart();
    if(!t->UserName.isEmpty())
    {
        QString concatenated = t->UserName + ":" + t->Password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        request->setRawHeader("Authorization", headerData.toLocal8Bit());
    }


    QSslConfiguration configSsl = QSslConfiguration::defaultConfiguration();
    configSsl.setProtocol(QSsl::AnyProtocol);
    request->setSslConfiguration(configSsl);

    std::cout << "download " << t->Url.toStdString() << std::endl;

    if(!t->HasPost)
    {
        if(wait)
        {
            t->Download = managernw->get(*request);
        }else {
            t->Download = manager->get(*request);
         }
    }else
    {
        request->setRawHeader("content-type","application/xml");

        std::cout << "post data " << t->PostData.toStdString() << std::endl;

        if(wait)
        {
            t->Download = managernw->post(*request,QByteArray().fromStdString(t->PostData.toStdString()));
        }else {
            t->Download = manager->post(*request,QByteArray().fromStdString(t->PostData.toStdString()));
         }
    }

    connect(t->Download, SIGNAL(downloadProgress(qint64,qint64)),t, SLOT(downloadProgress(qint64,qint64)));
    connect(t->Download, SIGNAL(finished()),t,SLOT(downloadFinished()));
    connect(t->Download, SIGNAL(readyRead()),t,SLOT(downloadReadyRead()));
    connect(t->Download, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),t,&DownloadTask::downloadError);
    connect(t->Download,SIGNAL(sslErrors(const QList<QSslError> &)), t, SLOT(OnSSLError(const QList<QSslError> &)));
    connect(t->Download,SIGNAL(redirected(const QUrl &)), t,SLOT(OnRedirected(const QUrl &)));



}


void DownloadManager::AddDownload(DownloadTask *t)
{
    m_TaskMutex->lock();

    t->moveToThread(thread);
    m_TaskList.append(t);

    emit OnTaskAdded();

    m_TaskMutex->unlock();


}

void DownloadManager::AddDownloadAndWait(DownloadTask *t)
{
    if(QThread::currentThread() != this->thread)
    {

        m_MutexNW = new QMutex();
        m_WaitNW = new QWaitCondition();

        m_MutexNW->lock();

        m_TaskMutexNW->lock();

        std::cout << "add download 1 " << std::endl;
        t->moveToThread(thread);
        m_TaskListNW.append(t);

        std::cout << "add download 2 " << std::endl;
        emit OnTaskAddedNW();

        std::cout << "add download 3 " << std::endl;
        m_TaskMutexNW->unlock();


        std::cout << "add download 4 " << std::endl;
        m_WaitNW->wait(m_MutexNW);

        std::cout << "add download 5 " << std::endl;
        delete m_MutexNW;
        delete m_WaitNW;
        m_MutexNW = nullptr;
        m_WaitNW = nullptr;
    }else
    {

        m_MutexNW = new QMutex();
        m_WaitNW = new QWaitCondition();


        std::cout << "add download 1 " << std::endl;
        m_MutexNW->lock();
        m_TaskMutexNW->lock();


        std::cout << "add download 2" << std::endl;
        t->moveToThread(thread);
        m_TaskListNW.append(t);

        std::cout << "add download 3 " << std::endl;
        m_MutexNW->unlock();
        m_TaskMutexNW->unlock();

        std::cout << "add download 4 " << std::endl;
        ProcessDownloadsNW();

        std::cout << "add download 5 " << std::endl;

    }

}



void DownloadManager::StopDownload(DownloadTask *t)
{
    if(t != nullptr)
    {
        if(t->Download != nullptr)
        {
            if(t->Download->isRunning())
            {
                t->Download->abort();
            }
            t->Download->deleteLater();
        }
    }
}


void DownloadManager::ProcessDownloads()
{

    m_TaskMutex->lock();


    for(int i = 0; i < m_TaskList.length(); i++)
    {
        m_TaskList.at(i)->OnAdd();
        StartDownload(m_TaskList.at(i));
    }
    m_TaskList.clear();
    m_TaskMutex->unlock();
}
void DownloadManager::ProcessDownloadsNW()
{

    std::cout << "do download 1 " << std::endl;
    m_MutexNW->lock();
    m_TaskMutexNW->lock();


    std::cout << "do download 2 " << std::endl;
    for(int i = 0; i < m_TaskListNW.length(); i++)
    {

        std::cout << "do download 3 " << std::endl;
        StartDownload(m_TaskListNW.at(i),true);


        std::cout << "do download 4 " << std::endl;
        //wait untill done
        while(!(m_TaskListNW.at(i)->Download->isFinished()) && (m_TaskListNW.at(i)->Download->error() == QNetworkReply::NetworkError::NoError))
        {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents,25);

        }

    }

    std::cout << "do download 5 " << std::endl;
    m_TaskListNW.clear();
    m_TaskMutexNW->unlock();

    std::cout << "do download 6 " << std::endl;

    m_MutexNW->unlock();
    m_WaitNW->wakeAll();

    std::cout << "do download 7 " << std::endl;


}
