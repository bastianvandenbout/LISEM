#include "randomnumbers.h""


std::random_device m_RN_rd = std::random_device();
std::mt19937 m_RN_gen = std::mt19937(m_RN_rd());
std::uniform_real_distribution<> m_RN_dist = std::uniform_real_distribution<>(0,1);
