#ifndef RANDOMNUMBERS_H
#define RANDOMNUMBERS_H

#include <random>
#include "defines.h"

LISEM_API extern std::random_device m_RN_rd;
LISEM_API extern std::mt19937 m_RN_gen;
LISEM_API extern std::uniform_real_distribution<> m_RN_dist;

inline double GetRandom(double a = 0.0, double b = 1.0)
{
    return  a + (b-a) * m_RN_dist(m_RN_gen);
}


#endif // RANDOMNUMBERS_H
