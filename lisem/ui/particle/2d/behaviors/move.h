#ifndef MOVE_H
#define MOVE_H


#include "particle/common/zone.h"
#include "particle/common/behavior.h"
#include "particle/common/particle.h"

namespace LISEM
{

class Move : public Behavior
{


public:
    inline Move()
    {


    }

    inline virtual void ApplyToParticle(Particle &p, int index, std::vector<Particle> &sortedlist)
    {


    }

};

}





#endif // MOVE_H
