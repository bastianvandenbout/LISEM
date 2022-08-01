#ifndef PARTICLEIMAGERENDERERH
#define PARTICLEIMAGERENDERERH

#include "particle/common/particlerenderer.h"
#include "particle/common/particle.h"
#include "openglclmanager.h"

namespace LISEM
{

class ParticleImageRenderer : public ParticleRenderer
{


private:
    //the projection is only valid for m_Is2D is true and not absolut
    GeoProjection m_CRS;

    int m_FillMode = 0;

    std::vector<OpenGLCLTexture> * m_Textur;
    std::vector<QString> m_File;
public:

    inline ParticleImageRenderer(std::vector<QString> files, std::vector<float> probs, int filltype = 0)
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
