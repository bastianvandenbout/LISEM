#ifndef GENERALSETTINGSMANAGER_H
#define GENERALSETTINGSMANAGER_H

#include "defines.h"
#include "QString"
#include "lsmio.h"

#define LISEM_FILE_GENERALSETTINGS "settings.ini"


class GeneralSettingsManager
{
    QString m_File;

    QList<QString> m_Names;
    QList<QString> m_Values;

public:

    inline GeneralSettingsManager()
    {

    }

    inline ~GeneralSettingsManager()
    {

    }

    inline void LoadFromFile()
    {
        m_Names.clear();
        m_Values.clear();

        QFile fin(m_File);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            LISEM_DEBUG("No settings file exists");
            return;
        }

        while (!fin.atEnd())
        {
            QString S = fin.readLine().trimmed();


                if (S.contains("=") && !S.startsWith("["))
                {

                    QStringList SL = S.split(QRegExp("="));

                    if(SL.length() > 1)
                    {
                        m_Names.append(SL.at(0));

                        QString val;

                        for(int i = 1; i < SL.length(); i++)
                        {
                            val += SL.at(i);
                            if(i != SL.length() -1)
                            {
                                val += "=";
                            }
                        }
                        m_Values.append(val);

                    }else
                    {
                        LISEM_ERROR("Could not read parameter in settings : " + S);
                    }

                }

        }

        return;

    }

    inline void UpdateFile()
    {
        QFile fp(m_File);
        if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LISEM_ERROR("Could not write settings file");
            return;
        }

        QTextStream out(&fp);
        out << QString("[OpenLISEM Hazard Settings]\n");
        for(int i= 0; i < m_Names.length(); i++)
        {
            if(m_Names.at(i).length() > 0 && m_Values.at(i).length() > 0)
            {
                out << m_Names.at(i) << "=" << m_Values.at(i) << "\n";
            }

        }
        fp.close();

    }

    inline void SetFile(QString file)
    {

        m_File = file;
        LoadFromFile();
    }

    inline bool ContainsSetting(QString name)
    {
        return m_Names.contains(name);
    }

    inline void SetSetting(QString name, QString val)
    {
        bool found = false;
        for(int i = 0; i < m_Names.length(); i++)
        {
            if(m_Names.at(i).compare(name) == 0)
            {
                found = true;
                m_Values.replace(i,val);
                break;
            }

        }
        if(found == false)
        {
            m_Names.append(name);
            m_Values.append(val);
        }
        UpdateFile();
    }

    inline void SetSetting(QString name, double val)
    {
        SetSetting(name,QString::number(val));
    }

    inline void SetSetting(QString name, int val)
    {
        SetSetting(name,QString::number(val));
    }

    inline void SetSetting(QString name, bool val)
    {
        SetSetting(name,QString::number((int)val));
    }

    inline QString GetSetting(QString name, bool * ok = nullptr)
    {
        for(int i = 0; i < m_Names.length(); i++)
        {
            if(m_Names.at(i).compare(name) == 0)
            {
                if(ok != nullptr)
                {
                    (*ok) = true;
                }
                return m_Values.at(i);
            }
        }

        if(ok != nullptr)
        {
            (*ok) = false;
        }
        return QString();
    }


};

LISEM_API extern GeneralSettingsManager * CGSManager;

static inline void InitializeGeneralSettingsManager(QString appdir)
{

    CGSManager = new GeneralSettingsManager();
    CGSManager->SetFile(appdir + "/settings.ini");
}

static inline void DestroyGeneralSettingsManager()
{

    if(CGSManager != nullptr)
    {
        delete CGSManager;
        CGSManager = nullptr;
    }
}

static inline GeneralSettingsManager * GetSettingsManager()
{
    return CGSManager;
}


#endif // GENERALSETTINGSMANAGER_H
