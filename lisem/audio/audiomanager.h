#ifndef AUDIOMANAGERH
#define AUDIOMANAGERH


#include "AL/al.h"
#include "AL/alc.h"
#include "error.h"
#include "defines.h"
#include "audioclip.h"

class AudioManager;

LISEM_API extern AudioManager * CAudioManager;

LISEM_API inline static void InitAudioManager(AudioManager * m)
{
    CAudioManager = m;
}

LISEM_API inline static AudioManager * GetAudioManager()
{
    return CAudioManager;
}




inline int al_check_error(QString given_label) { // generic OpenAL error checker

    ALenum al_error;
    al_error = alGetError();

    if(AL_NO_ERROR != al_error) {

        QString errortext = QString(alGetString(al_error));
        LISEM_ERROR("Error with openAL during " + given_label + " : " +errortext);
        return al_error;
    }
    return 0;
}


//class to register a playing sound, manage velocity, location, volume, panning, etc..
LISEM_API class AudioPlay
{
private:

public:

    inline AudioPlay()
    {


    }


};

LISEM_API class AudioManager{

private:


    ALCdevice* m_Device;
    ALCcontext* m_Context;


public:

    inline AudioManager()
    {

        InitAudioManager(this);
    }


    inline int SetupDefaultOutputDevice()
    {
        const char * devicename = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

        if(devicename == nullptr)
        {
            LISEM_ERROR("No default sound device exists");
            return 1;
        }
        m_Device = alcOpenDevice(devicename);
        if (m_Device == nullptr)
        {
            LISEM_ERROR("Can not open default sound hardware");
            return 1;
        }
        ALCcontext* context = alcCreateContext(m_Device, NULL);
        if (context == NULL)
        {
            LISEM_ERROR("Can not create context for sound hardware");
            return 2;
        }
        alcMakeContextCurrent(context);

        al_check_error("Creating Context");
        return 0;
    }





    inline void Destroy()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_Context);
        alcCloseDevice(m_Device);

    }





};



#endif
