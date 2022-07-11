#ifndef DISCZONEH
#define DISCZONEH

#include "linear/lsm_vector3.h"
#include "random/randomnumbers.h"

namespace LISEM
{

class DiscZone
{

    LSMVector3 m_Point;
    double m_radius_inner;
    double m_radius_outer;
public:

    inline DiscZone(LSMVector2 p, double radius, double radius_inner = 0.0)
    {
        m_Point = LSMVector3(p.x,0.0,p.y);
        radius =std::abs(radius);
        radius_inner = std::abs(radius_inner);
        if(radius < radius_inner)
        {
            double temp = radius;
            radius = radius_inner;
            radius_inner = temp;
        }

        m_radius_inner = radius_inner;
        m_radius_outer = radius;
    }

    inline virtual LSMVector3 GetRandomPoint()
    {
        //get random point in 2d donut/sphere
        float r = GetRandom();
        r = m_radius_inner + (1.0-r*r) * (m_radius_outer - m_radius_inner);
        float theta = 2.0 *M_PI * GetRandom();

        return LSMVector3(r * cos(theta), 0.0,r * sin(theta));
    }
    inline virtual double GetZoneArea()
    {
        //area of bigger circle minus area of smaller circle

        return M_PI*(m_radius_outer*m_radius_outer - m_radius_inner * m_radius_inner);
    }

    inline virtual bool Contains(LSMVector3 x)
    {
        //convert point to polar coordinates
        LSMVector3 dist = (x- m_Point);
        float rsq= dist.x * dist.x + dist.z * dist.z;

        //check radius
        if(rsq >= m_radius_inner * m_radius_inner && rsq <= m_radius_outer * m_radius_outer)
        {
            return true;
        }

        return false;
    }

    inline virtual double DistanceTo(LSMVector3 x)
    {
        //convert point to polar coordinates
        LSMVector3 dist = (x- m_Point);
        float r= std::sqrt(dist.x * dist.x + dist.z * dist.z);

        if(r < m_radius_inner)
        {
            return m_radius_inner - r;
        }
        if(r > m_radius_outer)
        {
            return r - m_radius_outer;
        }
        return 0.0;
    }

};

}

#endif
