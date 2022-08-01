#ifndef PARTICLEPOINTRENDERERH
#define PARTICLEPOINTRENDERERH

#include "particle/common/particle.h"
#include "linear/lsm_vector4.h"
#include "openglclmanager.h"

namespace LISEM
{

class ParticlePointRenderer{


private:


public:

    inline ParticlePointRenderer(LSMVector4 color, float size)
    {


    }

    inline void Init(OpenGLCLManager * m)
    {

    }

    inline void DrawParticles(std::vector<Particle> &p,OpenGLCLManager * m,GeoWindowState s, WorldGLTransformManager * tm)
    {





    }


};

}



#endif
