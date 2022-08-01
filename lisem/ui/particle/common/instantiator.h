#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H

#include "defines.h"

namespace LISEM
{


class Instantiator
{
public:

    inline Instantiator()
    {

    }

    virtual bool IsDone(int frame, float time, float dt)
    {
        return true;
    }

    virtual int GetCount(int frame, float time, float dt)
    {
        return 0;
    }




};



class BurstInstantiator : public Instantiator
{

    int m_Count= 1;
    int m_frame = 0;

public:

    inline BurstInstantiator(int count)
    {
        m_Count = count;
    }

    virtual int GetCount(int frame, float time, float dt)
    {
        if(frame == 0)
        {
            m_frame ++;
            return m_Count;

        }else
        {
            m_frame ++;
            return 0;
        }
    }

    virtual bool IsDone(int frame, float time, float dt)
    {
        if(frame > 0)
        {
            return true;
        }
        return false;
    }



};

class StreamInstantiator : public Instantiator
{

    float m_rate= 1;
    int m_frame = 0;

public:

    inline StreamInstantiator(float rate)
    {
        m_rate = rate;
    }

    virtual int GetCount(int frame, float time, float dt)
    {
        return dt * m_rate;

    }
};


class NoisyStreamInstantiator : public Instantiator
{

    float m_rate= 1;
    int m_frame = 0;
    float m_noise_amp = 1.0;
    float m_noise_period = 2.5;

public:

    inline NoisyStreamInstantiator(float rate, float noise_amp, float noise_period)
    {
        m_rate = rate;
        m_noise_amp = noise_amp;
        m_noise_period = noise_period;

    }

    virtual int GetCount(int frame, float time, float dt)
    {
        //aperiodic sine sum for the noise
        //this is fast and easy for frequency selection
        float x = time / m_noise_period;
        float noise = m_noise_amp *  0.3*(-3.2 * sin(-1.3 * x) - 1.2 * sin(-1.7 *M_E*x) + 1.9 * sin(0.7 * M_PI* x));
        return std::round(dt * std::max(0.0f,m_rate + noise));

    }
};




class DecliningNoisyStreamInstantiator : public Instantiator
{

    float m_rate= 1;
    int m_frame = 0;
    float m_noise_amp = 1.0;
    float m_noise_period = 2.5;
    float m_decline = 0.01;

public:

    inline DecliningNoisyStreamInstantiator(float rate, float noise_amp, float noise_period, float decline)
    {
        m_rate = rate;
        m_noise_amp = noise_amp;
        m_noise_period = noise_period;

    }

    virtual int GetCount(int frame, float time, float dt)
    {
        //aperiodic sine sum for the noise
        //this is fast and easy for frequency selection
        float x = time / m_noise_period;
        float noise = m_noise_amp *  0.3*(-3.2 * sin(-1.3 * x) - 1.2 * sin(-1.7 *M_E*x) + 1.9 * sin(0.7 * M_PI* x));
        return std::round(std::max(0.0f,(1.0f - m_decline * time))* dt * std::max(0.0f,m_rate + noise));

    }


    virtual bool IsDone(int frame, float time, float dt)
    {
        if(!(std::max(0.0f,(1.0f - m_decline * time)) > 1e-12))
        {
            return true;
        }else
        {
            return false;
        }
    }
};




}
#endif // INSTANTIATOR_H
