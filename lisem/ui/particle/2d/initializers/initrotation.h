#ifndef INITIALIZERROTATIONH
#define INITIALIZERROTATIONH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"

namespace LISEM
{

class InitializerRotation : public Initializer
{

    Zone m_Zone;

public:
    inline InitializerRotation(Zone z)
    {

        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        p.m_Rotation = m_Zone.GetRandomPoint();

    }

};



}

#endif
