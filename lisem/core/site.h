#ifndef SITE_H
#define SITE_H


#include "QString"


extern QString CSite;
extern QString CFSite;
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
    CSite = s;
}

inline QString GetSiteFileLoad()
{
    return CFSite;
}

#endif // SITE_H
