#ifndef INITIALIZERVELOCITYH
#define INITIALIZERVELOCITYH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"

namespace LISEM
{

class InitializerVelocity : public Initializer
{

    Zone m_Zone;

public:
    inline InitializerVelocity(Zone z)
    {

        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        p.m_PositionVel = m_Zone.GetRandomPoint();

    }

};



}

#endif
