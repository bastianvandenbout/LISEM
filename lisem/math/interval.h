#ifndef LSM_INTERVAL_H
#define LSM_INTERVAL_H

#include "QString"
#include "QStringList"
#include "serialization.h"

class LSMInterval : public LSMSerialize::LSMStringable
{
    float m_min = 0.0f;
    float m_max = 0.0f;
public:

    inline LSMInterval()
    {
    }

    inline LSMInterval(float min, float max)
    {
        Set(min,max);

    }
    void Set(float min, float max)
    {
        if(max>min)
        {

            m_min = min;
            m_max= max;
        }else {

            m_min = max;
            m_max= min;
        }
    }
    bool Contains(float val)
    {
        if(!(val < m_min || val > m_max))
        {
            return true;
        }
        return false;
    }
    void SetMax(float x)
    {
        m_max = x;
    }
    void SetMin(float x)
    {
        m_min = x;
    }
    float GetMin()
    {
        return m_min;
    }
    float GetMax()
    {
        return m_max;
    }

    inline QString toString() const override
    {
        return QString(QString::number(m_min) + ";" + QString::number(m_max));
    }

    inline void fromString(QString x) override
    {
        QStringList l = x.split(";");
        if(l.length() == 2)
        {
            m_min = l.at(0).toDouble();
            m_max = l.at(1).toDouble();
        }else {
            m_min =0;
            m_max = 0;
        }
    }

};

class LSMInterval2D
{
    float m_minx;
    float m_maxx;
    float m_miny;
    float m_maxy;

};

class LSMInterval3D
{
    float m_minx;
    float m_maxx;
    float m_miny;
    float m_maxy;
    float m_minz;
    float m_maxz;

};
#endif // LSM_INTERVAL_H
