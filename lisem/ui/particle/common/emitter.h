#ifndef EMITTERH
#define EMITTERH

#include "particle.h"
#include "initializer.h"
#include "instantiator.h"
#include "behavior.h"
#include "particlerenderer.h"

namespace LISEM
{

bool CompareParticleByXPosition(const Particle & p1, const Particle & p2)
{
    return p1.m_Position.x < p2.m_Position.x;
}

class Emitter
{

    std::vector<Particle> m_Particles;

    std::vector<Initializer> m_Initializers;

    std::vector<Instantiator> m_Instantiators;

    std::vector<Behavior> m_Behaviors;

    ParticleRenderer m_ParticleRenderer;

    int m_Frame = 0;
    float m_TimeStart = 0.0;

public:

    inline Emitter()
    {


    }



    inline void AddBehaviour(Behavior behave)
    {

    }

    inline void AddInitializer(Initializer init)
    {


    }
    inline void AddInstantiator(Instantiator inst)
    {

    }

    inline void Instantiate(int frame, float time, float dt)
    {

        int count_total = 0;
        for(int i = 0; i < m_Instantiators.size(); i++)
        {
            int count = m_Instantiators.at(i).GetCount(frame,time,dt);
            count_total += count;
        }

        for(int i = 0; i < count_total; i++)
        {
            Particle p;

            for(int j = 0; j < m_Initializers.size(); j++)
            {
                m_Initializers.at(j).ApplyToParticle(p);
            }
            m_Particles.push_back(p);
        }

    }


    inline void Update(int frame, float dt, float time)
    {
        //sweep and prune algorithm
        //first let us sort the particles in the x-direction
        //we could do an insertion sort ourselves (better for partially sorted lsits) but std::sort seems to do a good job as well using its three-algorithm hybrid IntroSort

        std::sort(m_Particles.begin(), m_Particles.end(),CompareParticleByXPosition);
        //now we provide the sorted list to the behaviors


        for(int i = 0; i < m_Behaviors.size(); i++)
        {
            for(int j = 0; j < m_Particles.size(); j++)
            {
                m_Behaviors.at(i).ApplyToParticle(m_Particles.at(j),j,m_Particles);
            }
        }

        int removed = 0;

        //some particles might have been removed, we need to check the list and make sure all the desired particles are kept
        for(int i = 0; i < m_Particles.size(); i++)
        {
            if(m_Particles.at(i).m_Remove)
            {
                removed = removed + 1;
            }else
            {
                m_Particles.at(i-removed) = m_Particles.at(i);
            }
        }

        m_Particles.resize(m_Particles.size() - removed);

    }

};
}

#endif
