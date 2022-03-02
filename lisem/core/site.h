#ifndef SITE_H
#define SITE_H


#include "QString"
#include "defines.h"

#define LISEM_DIR_FIO_GENERAL 0
#define LISEM_DIR_FIO_CODE 1
#define LISEM_DIR_FIO_GEOSCENE 2
#define LISEM_DIR_FIO_GEODATA 3

LISEM_API extern std::vector<QString> AS_DIR_FIO;
LISEM_API extern std::vector<int> AS_DIR_FIO_ID;

LISEM_API extern QString CSite;
LISEM_API extern QString CFSite;

inline void SetSite(QString s)
{
    CSite = s;
}

inline QString GetSite()
{
    return CSite;
}

inline void SetSiteFileLoad(QString s)
{
    CFSite = s;
}

inline QString GetSiteFileLoad()
{
    return CFSite;
}

//GetFIODir(LISEM_FIO_GENERAL),
//SetFIODir(LISEM_FIO_GENERAL,QFileInfo(fileName).dir().absolutePath());

//GetFIODir(LISEM_FIO_GEODATA),
//SetFIODir(LISEM_FIO_GEODATA,QFileInfo(fileName).dir().absolutePath());


inline void SetFIODir(int index, QString dir)
{
    bool found = false;
    for(int i = 0; i < AS_DIR_FIO_ID.size(); i++)
    {
        if(AS_DIR_FIO_ID.at(i) == index)
        {
            found = true;
            AS_DIR_FIO[i] == dir;
        }
    }
    if(!found)
    {
        AS_DIR_FIO.push_back(dir);
        AS_DIR_FIO_ID.push_back(index);
    }

}
inline QString GetFIODir(int index)
{
    for(int i = 0; i < AS_DIR_FIO_ID.size(); i++)
    {
        if(AS_DIR_FIO_ID.at(i) == index)
        {
            return AS_DIR_FIO[i];
        }
    }


    QString fl = GetSiteFileLoad();
    if(!fl.isEmpty())
    {
        return fl;
    }

    return GetSite();
}

#endif // SITE_H
