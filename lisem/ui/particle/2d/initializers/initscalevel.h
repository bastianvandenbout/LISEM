#ifndef INITIALIZERPOSITIONH
#define INITIALIZERPOSITIONH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"

namespace LISEM
{

class InitializerScaleVelocity : public Initializer
{

    Zone m_Zone;

public:
    inline InitializerScaleVelocity(Zone z)
    {

        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        p.m_ScaleVel = m_Zone.GetRandomPoint();

    }



};



}

#endif
