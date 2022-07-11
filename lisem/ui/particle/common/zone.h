#ifndef ZONEH
#define ZONEH

#include "linear/lsm_vector3.h"
namespace LISEM
{

class Zone
{

public:

    inline Zone()
    {

    }

    inline virtual LSMVector3 GetRandomPoint()
    {
        return LSMVector3(0.0,0.0,0.0);
    }
    inline virtual double GetZoneArea()
    {
        return 0.0;
    }

    inline virtual bool Contains(LSMVector3 x)
    {
        return false;
    }
    inline virtual double DistanceTo(LSMVector3 x)
    {
        return 0.0;
    }

    inline Zone * GetCopy()
    {
        return new Zone(*this);
    }

};

}

#endif
