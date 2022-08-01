#ifndef AUDIOWAVEH
#define AUDIOWAVEH

#include "audioclip.h"
#include "defines.h"

inline bool isBigEndian()
{
    int a = 1;
    return !((char*)&a)[0];
}

inline int convertToInt(char* buffer, int len)
{
    int a = 0;
    if (!isBigEndian())
        for (int i = 0; i<len; i++)
            ((char*)&a)[i] = buffer[i];
    else
        for (int i = 0; i<len; i++)
            ((char*)&a)[3 - i] = buffer[i];
    return a;
}

LISEM_API inline AudioClip * LoadWaveFile(QString filename)
{




}



#endif
