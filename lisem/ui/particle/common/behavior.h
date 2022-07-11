#ifndef BEHAVIORH
#define BEHAVIORH

#include "zone.h"
#include "particle.h"

namespace LISEM
{

class Behavior
{


public:
    inline Behavior()
    {


    }

    inline virtual void ApplyToParticle(Particle &p, int index, std::vector<Particle> &sortedlist)
    {


    }

};



}

#endif
