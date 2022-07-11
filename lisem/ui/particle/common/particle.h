#ifndef PARTICLEH
#define PARTICLEH

#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
namespace LISEM
{

class Particle
{
public:

    LSMVector3 m_Position;
    LSMVector3 m_Rotation;
    LSMVector3 m_Scale;
    LSMVector4 m_Color;

    LSMVector3 m_PositionVel;
    LSMVector3 m_RotationVel;
    LSMVector3 m_ScaleVel;
    LSMVector4 m_ColorVel;

    bool m_Remove = false;



public:

    inline Particle()
    {

    }



};



}


#endif
