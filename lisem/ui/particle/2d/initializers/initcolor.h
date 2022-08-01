#ifndef INITIALIZERCOLORH
#define INITIALIZERCOLORH

#include "particle/common/initializer.h"
#include "particle/common/zone.h"
#include "particle/common/particle.h"
#include "random/randomnumbers.h"

namespace LISEM
{

class InitializerColor : public Initializer
{

    Zone m_Zone;
    float m_Alpha_Min = 0.0;
    float m_Alpha_Max = 1.0;

public:
    inline InitializerColor(Zone z, float alpha_min, float alpha_max)
    {

        m_Alpha_Min =alpha_min;
        m_Alpha_Max = alpha_max;
        m_Zone = z;

    }

    inline virtual void ApplyToParticle(Particle &p)
    {

        LSMVector3 c1 =m_Zone.GetRandomPoint();
        float alpha = GetRandom(m_Alpha_Min, m_Alpha_Max);
        p.m_Color = LSMVector4(c1.x,c1.y,c1.z,alpha);

    }

};



}

#endif
