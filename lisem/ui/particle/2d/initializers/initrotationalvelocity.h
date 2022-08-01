#ifndef INITIALIZERROTATIONVELOCITYH
#define INITIALIZERROTATIONVELOCITYH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"

namespace LISEM
{

class InitializerRotationVelocity : public Initializer
{

    Zone m_Zone;

public:
    inline InitializerRotationVelocity(Zone z)
    {

        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        p.m_RotationVel = m_Zone.GetRandomPoint();

    }

};



}

#endif
