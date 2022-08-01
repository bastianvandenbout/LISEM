#ifndef INITIALIZERSIZEH
#define INITIALIZERSIZEH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"

namespace LISEM
{

class InitializerScale : public Initializer
{

    Zone m_Zone;

public:
    inline InitializerScale(Zone z)
    {

        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        p.m_Scale = m_Zone.GetRandomPoint();

    }



};



}

#endif
