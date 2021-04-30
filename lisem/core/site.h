#ifndef SITE_H
#define SITE_H


#include "QString"


extern QString CSite;

inline void SetSite(QString s)
{
    CSite = s;
}

inline QString GetSite()
{
    return CSite;
}
#endif // SITE_H
