#ifndef PARTICLEMANAGERH
#define PARTICLEMANAGERH

#include <mutex>
#include <vector>
#include "common/emitter.h"

namespace LISEM
{
/*//this class manages all particle emitters, updates them,keep track of them, and removes them if requested
class ParticleManager
{

    std::mutex m_EmittersMutex;

    std::vector<Emitter*> m_Emitters;

    //we use a direct r-value Emitter, and move it directly into the list
    //to get access to the provided Emitter,
    inline Emitter *AddEmitter(Emitter && e)
    {
         Emitter * m = new Emitter(std::move(e));

         m_EmittersMutex.lock();

         m_Emitters.push_back(m);


         m_EmittersMutex.unlock();

         return m;
    }

    inline int EmitterCount()
    {
        m_EmittersMutex.lock();
        int n =m_Emitters.size();
        m_EmittersMutex.unlock();
        return n;
    }
    inline void LockEmitters()
    {
        m_EmittersMutex.lock();
    }
    inline void UnlockEmitters()
    {
        m_EmittersMutex.unlock();
    }
    inline Emitter *GetEmitter(int i)
    {
        return m_Emitters.at(i);
    }

    inline void RemoveEmitter(int i)
    {
        m_EmittersMutex.lock();

        m_Emitters.erase(m_Emitters.begin() + i);

        m_EmittersMutex.unlock();
    }
    inline void RemoveEmitter(Emitter * m)
    {
        m_EmittersMutex.lock();

        for(int i = 0; i < m_Emitters.size(); i++)
        {
            if(m_Emitters.at(i) == m)
            {
                m_Emitters.erase(m_Emitters.begin() + i);
                break;
            }
        }

        m_EmittersMutex.unlock();
    }




};*/



}



#endif
