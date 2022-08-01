#ifndef BOUNDEDBYBOXH
#define BOUNDEDBYBOXH


#include "particle/common/zone.h"
#include "particle/common/behavior.h"
#include "particle/common/particle.h"
#include "boundingbox.h"

namespace LISEM
{

class BoundedByBox : public Behavior
{

    BoundingBox m_BoundingBox;

public:
    inline BoundedByBox(BoundingBox b)
    {
        m_BoundingBox = b;

    }

    inline virtual void ApplyToParticle(Particle &p, int index, std::vector<Particle> &sortedlist)
    {


    }

};

}

#endif
