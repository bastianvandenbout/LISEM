#ifndef BOXZONE_H
#define BOXZONE_H


#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "random/randomnumbers.h"
#include "particle/common/zone.h"

namespace LISEM
{

class BoxZone : public Zone
{

    LSMVector2 m_Point;
    LSMVector2 m_Size;

public:

    inline BoxZone(LSMVector2 TopLeft, LSMVector2 size)
    {
        m_Point = TopLeft;
        m_Size = size;

    }

    inline virtual LSMVector3 GetRandomPoint()
    {
        //get random point in 2d donut/sphere


        return LSMVector3(m_Point.x + m_Size.x * GetRandom(), 0.0,m_Point.y + m_Size.y * GetRandom());
    }
    inline virtual double GetZoneArea()
    {
        //area of bigger circle minus area of smaller circle
        return std::fabs(m_Size.x) * std::fabs(m_Size.y);
    }

    inline virtual bool Contains(LSMVector3 x)
    {
        //convert point to polar coordinates
        if(x.x >= m_Point.x && x.x <= m_Point.x + m_Size.x)
        {
            if(x.z >= m_Point.y && x.z <= m_Point.y + m_Size.y)
            {
                return true;
            }
        }

        return false;
    }

    inline virtual double DistanceTo(LSMVector3 x)
    {
        //single-expression sdf from iq (https://iquilezles.org/articles/distfunctions/)
        double dx = std::max(std::max(m_Point.x-x.x,0.0f),x.x-m_Point.x - m_Size.x);
        double dy = std::max(std::max(m_Point.y-x.z,0.0f),x.z-m_Point.y - m_Size.y);

        return std::sqrt(dx*dx + dy*dy);
    }

};

}


#endif // BOXZONE_H
