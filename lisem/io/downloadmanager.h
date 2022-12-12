#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "defines.h"
#include "QString"
#include "QList"
#include <QtNetwork>
#include <QtCore>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <QSslSocket>
#include "error.h"


//define listener functions for download progress and finish
class LISEM_API DownloadTask : public QObject
{
    Q_OBJECT


    bool m_CallBackStartSet = false;
    bool m_CallBackProgressSet = false;
    bool m_CallBackFinishedSet = false;
    std::function<void(DownloadTask*)> m_CallBackStart;
    std::function<void(DownloadTask*)> m_CallBackProgress;
    std::function<void(DownloadTask*)> m_CallBackFinish;

    bool m_CallBackAddedSet = false;
    std::function<void(DownloadTask*)> m_CallBackAdd;

public:



    QNetworkReply * Download = nullptr;

    QString Url = "";
    QString file = "";
    QString Password = "";
    QString UserName = "";

    QString Error = "";
    int timeout;
    QByteArray m_Data;
    bool canbesaved = false;
    bool shouldbesaved = true;

    bool HasPost = false;
    QString PostData = "";

    bool IsScript = false;

    bool IsFinished = false;
    QMutex FinishMutex;


    qint64 bytes_received = 0;
    qint64 bytes_total = 0;

    inline DownloadTask(QString url)
    {
        Url = url;
    }

    inline DownloadTask(QString url, QString file_save, QString username, QString password, int in_timeout, bool script =false)
    {
        Url = url;
        file = file_save;
        UserName = username;
        Password = password;
        timeout = in_timeout;
        IsScript = script;

        if(!file_save.isEmpty())
        {
            QFileInfo fi(file_save);
            if(!fi.dir().exists())
            {
                fi.dir().mkdir(fi.dir().path());
            }
            if(fi.dir().exists())
            {
                canbesaved = true;
            }
        }
    }

    inline bool CanBeSaved()
    {
        return canbesaved;
    }

    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackStart(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackStartSet = true;
        m_CallBackStart = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }
    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackProgress(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackProgressSet = true;
        m_CallBackProgress = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }
    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackFinished(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackFinishedSet = true;
        m_CallBackFinish = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }

    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackAdd(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackAddedSet = true;
        m_CallBackAdd = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }


    inline ~DownloadTask()
    {


    }

    inline void OnAdd()
    {
        if(m_CallBackAddedSet)
        {
            m_CallBackAdd(this);
        }
    }


    inline void OnStart()
    {
        if(m_CallBackStartSet)
        {
            m_CallBackStart(this);

        }
    }


public slots:

    inline void downloadProgress(qint64 received,qint64 total)
    {
        bytes_total = total;
        bytes_received = received;
        if(m_CallBackProgressSet)
        {
            m_CallBackProgress(this);
        }

    }

    inline void downloadFinished()
    {

        m_Data = Download->readAll();

        std::cout << "m_Data " << m_Data.size() << std::endl;// << " " << m_Data.toStdString() << std::endl;

        std::cout << "done " << canbesaved << " " << file.toStdString()  << std::endl;
        if(canbesaved && shouldbesaved)
        {
            QSaveFile sfile(file);
            sfile.open(QIODevice::WriteOnly);
            sfile.write(m_Data);
            // Calling commit() is mandatory, otherwise nothing will be written.
            sfile.commit();
        }

        FinishMutex.lock();
        IsFinished = true;
        FinishMutex.unlock();

        if(m_CallBackFinishedSet)
        {
            m_CallBackFinish(this);
        }
    }

    inline void downloadReadyRead()
    {


    }

    inline void downloadError(QNetworkReply::NetworkError code)
    {
        if(code == QNetworkReply::AuthenticationRequiredError)
        {
            Error = "Network error when requesting: " + Url + " (Authentification required)";
            if(IsScript)
            {
                LISEMS_ERROR("Network error when requesting: " + Url + " (Authentification required)");
            }
        }else
        {
            Error = "Network error when requesting: " + Url;
            if(IsScript)
            {
                LISEMS_ERROR("Network error when requesting: " + Url);
            }
        }

    }
    inline void OnSSLError(const QList<QSslError> &)
    {
       {
            Error = "Network error when requesting: " + Url + " (SSL)";
            if(IsScript)
            {
                LISEMS_ERROR("Network error when requesting: " + Url + " (SSL)");
            }
        }
    }
    inline void OnRedirected(const QUrl &url)
    {
        std::cout << "download redirect " << url.toString().toStdString() << std::endl;



    }
};

class LISEM_API DownloadManager : QObject
{
    Q_OBJECT

private:

    QNetworkAccessManager *manager;
    QNetworkAccessManager *managernw;
    QList<DownloadTask *> m_TaskList;
    QList<DownloadTask *> m_CurrentList;
    QWaitCondition m_WaitCondition;
    QMutex *m_TaskMutex;
    bool m_DoStop = false;
    QThread *thread;
    QMutex * m_MutexNW;
    QWaitCondition * m_WaitNW;
    QList<DownloadTask *> m_TaskListNW;
    QMutex *m_TaskMutexNW;

    inline void RequestStop()
    {
        m_DoStop = true;
    }

    void StartDownload(DownloadTask * t, bool wait = false);

public:

    DownloadManager();

    inline ~DownloadManager()
    {

        delete m_TaskMutex;

    }

    inline void Destroy()
    {

        this->StopAllDownloads();

    }


    void AddDownload(DownloadTask *t);

    void AddDownloadAndWait(DownloadTask *t);

    void StopDownload(DownloadTask *t);

    inline void StopAllDownloads()
    {

    }

    inline bool CheckConnection()
    {

        return false;
    }

public:
signals:
   inline void OnTaskAdded();
    inline void OnTaskAddedNW();

public slots:

    void ProcessDownloads();
    void ProcessDownloadsNW();

    inline void downloadFinished(QNetworkReply * /*p*/)
    {

    }

};


LISEM_API extern DownloadManager * CDownloadManager;

inline static void InitDownloadManager()
{
    CDownloadManager = new DownloadManager();
}

inline static DownloadManager * GetDownloadManager()
{
    return CDownloadManager;

}

inline static void DestroyDownloadManager()
{
    if(CDownloadManager != nullptr)
    {
        delete CDownloadManager;
        CDownloadManager = nullptr;

    };
}

#endif // DOWNLOADMANAGER_H
