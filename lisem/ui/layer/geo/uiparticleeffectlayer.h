#ifndef UIPARTICLEEFFECTLAYER_H
#define UIPARTICLEEFFECTLAYER_H

#include "layer/uilayer.h"
#include "layer/geo/uigeolayer.h"
#include "particle/particlemanager.h"

class UIParticleEffectLayer : public UIGeoLayer
{
private:

    QMutex m_Mutex;

    LISEM::Emitter * m_Emitter = nullptr;

public:
    inline UIParticleEffectLayer() : UIGeoLayer()
    {
        m_IsDynamic = true;
    }

    inline void SetEmitter(LISEM::Emitter E)
    {
        m_Emitter = new LISEM::Emitter(std::move(E));
    }


    inline ~UIParticleEffectLayer()
    {

    }

    inline QString layertypeName()
    {
        return "ParticleEffectLayer";
    }


    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = true;
    }



    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

    }


    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {



    }

    inline void OnDraw(OpenGLCLManager *m, GeoWindowState s)
    {
        //draw the current state of the emitter using the renderer we already set up

    }

    inline void OnFrame(float dt, GeoWindowState s )
    {
        //update particles and their behavior



    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        //get the renderer for the particles working

        //
        if(m_Emitter != nullptr)
        {
            m_Emitter->Instantiate(0,0.0,0.0);
        }



        m_IsPrepared = true;
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {



    }


    inline LayerInfo GetInfo()
    {
        m_Mutex.lock();
        LayerInfo l;
        l.Add("Type","Particle Effect Layer");

        m_Mutex.unlock();

        return l;
    }

    virtual inline bool IsMovable() override
    {
        return false;
    }


};









#endif // UIPARTICLEEFFECTLAYER_H
