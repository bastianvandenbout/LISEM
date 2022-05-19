#include "error.h"
#include "iostream"

QMutex *MessageMutex_ALL = NULL;
QList<QString> *Messages_ALL = NULL;
QList<int> *Levels_ALL = NULL;
bool MessageUseCommandLine = false;

QMutex *MessageMutex = NULL;
QList<QString> *Messages = NULL;
QList<int> *Levels = NULL;

QMutex *MessageMutexS = NULL;
QList<QString> *MessagesS = NULL;
QList<int> *LevelsS = nullptr;
bool MessageUseCommandLineS = false;

QMutex *MessageMutexSTD = NULL;
QList<QString> *MessagesSTD = NULL;
QList<int> *LevelsSTD = NULL;

void InitMessages(bool use_commandline)
{
    MessageUseCommandLine = use_commandline;
    Messages = new QList<QString>();
    Levels = new QList<int>();
    MessageMutex = new QMutex();

    Messages_ALL = new QList<QString>();
    Levels_ALL = new QList<int>();
    MessageMutex_ALL = new QMutex();

}

void AddMessage(QString message, int level)
{
    if(MessageUseCommandLine)
    {
        std::cout << level << " " <<  message.toStdString() << std::endl;
    }
    PrintQStringStdOut("LISEM " + QString::number(level) + message);

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            Messages->append(message);
            Levels->append(level);
            Messages_ALL->append(message);
            Levels_ALL->append(level);
        }

        MessageMutex->unlock();
    }

}

bool LISEM_use_cout = false;

void SetPrintUseStdOut(bool x)
{
    LISEM_use_cout = x;
}
void PrintQStringStdOut(const QString &s)
{

    if(LISEM_use_cout)
    {
        std::cout << s.toStdString() << std::endl;
    }
}


QList<LeveledMessage> GetMessages()
{

    QList<LeveledMessage> ret;

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            for(int i = 0; i < Messages->length(); i++)
            {
                LeveledMessage m;
                QString s = Messages->at(i);
                int level = Levels->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            Messages->clear();
            Levels->clear();
        }
        MessageMutex->unlock();
    }

    return ret;
}

QList<LeveledMessage> GetMessages_ALL()
{

    QList<LeveledMessage> ret;

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages_ALL != NULL && Levels_ALL != NULL)
        {
            for(int i = 0; i < Messages_ALL->length(); i++)
            {
                LeveledMessage m;
                QString s = Messages_ALL->at(i);
                int level = Levels_ALL->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            Messages_ALL->clear();
            Levels_ALL->clear();
        }
        MessageMutex->unlock();
    }

    return ret;
}

void InitMessagesS(bool use_commandline)
{
    MessageUseCommandLineS = use_commandline;
    MessagesS = new QList<QString>();
    LevelsS = new QList<int>();
    MessageMutexS = new QMutex();

    MessagesSTD = new QList<QString>();
    LevelsSTD = new QList<int>();
    MessageMutexSTD = new QMutex();

}

void AddMessageS(QString message, int level)
{

    if(MessageUseCommandLineS)
    {
        std::cout << level << " " << message.toStdString() << std::endl;
    }
    PrintQStringStdOut("LISEM_S2 " + QString::number(level) + message);
    if(MessageMutexS != NULL)
    {
        MessageMutexS->lock();

        if(MessagesS != NULL && LevelsS != NULL)
        {
            MessagesS->append(message);
            LevelsS->append(level);
        }

        MessageMutexS->unlock();
    }
    if(MessageMutexSTD != NULL)
    {
        MessageMutexSTD->lock();

        if(MessagesSTD != NULL && LevelsSTD != NULL)
        {
            MessagesSTD->append(message);
            LevelsSTD->append(level);
        }

        MessageMutexSTD->unlock();
    }

}

QList<LeveledMessage> GetMessagesS()
{

    QList<LeveledMessage> ret;

    if(MessageMutexS != NULL)
    {
        MessageMutexS->lock();

        if(MessagesS != NULL && LevelsS != NULL)
        {
            for(int i = 0; i < MessagesS->length(); i++)
            {
                LeveledMessage m;
                QString s = MessagesS->at(i);
                int level = LevelsS->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            MessagesS->clear();
            LevelsS->clear();
        }
        MessageMutexS->unlock();
    }

    return ret;
}

QList<LeveledMessage> GetMessagesSTD()
{

    QList<LeveledMessage> ret;

    if(MessageMutexSTD != NULL)
    {
        MessageMutexSTD->lock();

        if(MessagesSTD != NULL && LevelsSTD != NULL)
        {
            for(int i = 0; i < MessagesSTD->length(); i++)
            {
                LeveledMessage m;
                QString s = MessagesSTD->at(i);
                int level = LevelsSTD->at(i);
                m.Message = s;
                m.Level = level;
                ret.append(m);
            }

            MessagesSTD->clear();
            LevelsSTD->clear();
        }
        MessageMutexSTD->unlock();
    }

    return ret;
}

void PrintLISEMDebug()
{

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            for(int i = 0; i < Messages->length(); i++)
            {
                LeveledMessage m;
                QString s = Messages->at(i);
                int level = Levels->at(i);

                std::cout << level << "::  " << s.toStdString() << std::endl;
            }

        }
        MessageMutex->unlock();
    }

    return;
}


QString getStringFromUnsignedChar( const unsigned char *str ){
    QString result = "";
    int lengthOfString = strlen( reinterpret_cast<const char*>(str) );

    // print string in reverse order
    QString s;
    for( int i = 0; i < lengthOfString; i++ ){
        s = QString( "%1" ).arg( str[i], 0, 16 );

        // account for single-digit hex values (always must serialize as two digits)
        if( s.length() == 1 )
            result.append( "0" );

        result.append( s );
    }

    return result;
}

QString GetLastErrorMessage()
{

    QString ret = "<undefined>";

    if(MessageMutex != NULL)
    {
        MessageMutex->lock();

        if(Messages != NULL && Levels != NULL)
        {
            if(Messages->length() > 0)
            {
                ret = Messages->at(Messages->length() -1);
            }
        }
        MessageMutex->unlock();
    }
    return ret;

}
