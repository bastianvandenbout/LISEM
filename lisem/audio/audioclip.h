#ifndef AUDIOCLIPH
#define AUDIOCLIPH

#include "AL/al.h"
#include "AL/alc.h"

class AudioClip
{

private:

    int m_Channels = 0;
    int m_SampleRate = 0;
    int m_BPS = 0;
    int m_Size = 0;
    char * data = nullptr;



};



#endif
