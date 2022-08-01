#ifndef POINTZONEH
#define POINTZONEH

#include "linear/lsm_vector3.h"
#include "particle/common/zone.h"
namespace LISEM
{

class PointZone : public Zone
{

    LSMVector3 m_Point;
public:

    inline PointZone(LSMVector2 p)
    {
        m_Point = LSMVector3(p.x,0.0,p.y);

    }

    inline virtual LSMVector3 GetRandomPoint()
    {
        return LSMVector3(m_Point.x, 0.0,m_Point.y);
    }
    inline virtual double GetZoneArea()
    {
        return 0.0;
    }

    inline virtual bool Contains(LSMVector3 x)
    {

        if(m_Point.x == x.x && m_Point.y == x.y)
        {
            return true;
        }

        return false;
    }

    inline virtual double DistanceTo(LSMVector3 x)
    {
        return (x - m_Point).length();
    }

};

}

#endif
