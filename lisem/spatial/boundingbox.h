#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include "defines.h"
#include <math.h>
#include <cmath>
#include <algorithm>
#include "linear/lsm_vector2.h"
#include "QString"
#include "QStringList"
#include "linear/lsm_vector3.h"



class UIDistanceEstimator
{
public:
    virtual double GetNormalizedSmoothVertWeight(LSMVector3 p)
    {

        return 0.0;
    }


    virtual double GetNormalizedSmoothSize(LSMVector3 p)
    {

        return 1e31;
    }

    virtual double GetNormalizedSmoothDistance(LSMVector3 p)
    {

        return 1e31;
    }

};



class LISEM_API BoundingBox3D
{
    friend class BoundingBox;

private:
    double m_MinX = 0.0;
    double m_MaxX = 0.0;
    double m_MinY = 0.0;
    double m_MaxY = 0.0;
    double m_MinZ = 0.0;
    double m_MaxZ = 0.0;
public:

    inline BoundingBox3D()
    {

    }

    inline BoundingBox3D(double xmin,double xmax, double ymin, double ymax,double zmin, double zmax)
    {
        Set(xmin,xmax,ymin,ymax,zmin,zmax);
    }

    inline void Set(LSMVector3 p)
    {
        Set(p.x,p.x,p.y,p.y,p.z,p.z);
    }
    inline void Set(double xmin,double xmax, double ymin, double ymax,double zmin, double zmax)
    {
        m_MinX = xmin < xmax? xmin : xmax;
        m_MaxX = xmin < xmax? xmax : xmin;
        m_MinY = ymin < ymax? ymin : ymax;
        m_MaxY = ymin < ymax? ymax : ymin;
        m_MinZ = zmin < zmax? zmin : zmax;
        m_MaxZ = zmin < zmax? zmax : zmin;
    }

    inline void Merge(BoundingBox3D p)
    {

        this->m_MaxX = std::max((double)p.GetMaxX(),this->m_MaxX);
        this->m_MinX = std::min((double)p.GetMinX(),this->m_MinX);
        this->m_MaxY = std::max((double)p.GetMaxY(),this->m_MaxY);
        this->m_MinY = std::min((double)p.GetMinY(),this->m_MinY);
        this->m_MaxZ = std::max((double)p.GetMaxZ(),this->m_MaxZ);
        this->m_MinZ = std::min((double)p.GetMinZ(),this->m_MinZ);

    }

    inline void Merge(LSMVector3 p)
    {

        this->m_MaxX = std::max((double)p.X(),this->m_MaxX);
        this->m_MinX = std::min((double)p.X(),this->m_MinX);
        this->m_MaxY = std::max((double)p.Y(),this->m_MaxY);
        this->m_MinY = std::min((double)p.Y(),this->m_MinY);
        this->m_MaxZ = std::max((double)p.Z(),this->m_MaxZ);
        this->m_MinZ = std::min((double)p.Z(),this->m_MinZ);

    }
    inline void And(BoundingBox3D b)
    {
        this->m_MaxX = std::min(b.m_MaxX,this->m_MaxX);
        this->m_MinX = std::max(b.m_MinX,this->m_MinX);
        this->m_MaxY = std::min(b.m_MaxY,this->m_MaxY);
        this->m_MinY = std::max(b.m_MinY,this->m_MinY);
        this->m_MaxZ = std::min(b.m_MaxZ,this->m_MaxZ);
        this->m_MinZ = std::max(b.m_MinZ,this->m_MinZ);
        this->Set(m_MinX,m_MaxX,m_MinY,m_MaxY,m_MinZ,m_MaxZ);
    }

    inline double GetVolume()
    {
        return std::fabs((m_MaxX - m_MinX)*(m_MaxY * m_MinY)*(m_MaxZ - m_MinZ));
    }

    inline double GetMinX() const
    {
        return m_MinX;
    }

    inline double GetMaxX() const
    {
        return m_MaxX;
    }
    inline double GetMinY() const
    {
        return m_MinY;
    }
    inline double GetMinZ() const
    {
        return m_MinZ;
    }
    inline double GetMaxY() const
    {
        return m_MaxY;
    }
    inline double GetMaxZ() const
    {
        return m_MaxZ;
    }

    inline double GetCenterX() const
    {
        return 0.5f*(m_MinX + m_MaxX);
    }
    inline double GetCenterY() const
    {
        return 0.5f*(m_MinY + m_MaxY);
    }
    inline double GetCenterZ() const
    {
        return 0.5f*(m_MinZ+ m_MaxZ);
    }
    inline double GetSizeX() const
    {
        return fabs((m_MaxX - m_MinX));
    }
    inline double GetSizeY() const
    {
        return fabs((m_MaxY - m_MinY));
    }
    inline double GetSizeZ() const
    {
        return fabs((m_MaxZ - m_MinZ));
    }

    inline bool Overlaps(BoundingBox3D b)
    {
        if((b.m_MinX <= m_MinX && b.m_MaxX >= m_MaxX) || (b.m_MaxX <= m_MaxX && b.m_MaxX >= m_MinX) || (b.m_MinX >= m_MinX && b.m_MinX <= m_MaxX))
        {
            if((b.m_MinY <= m_MinY && b.m_MaxY >= m_MaxY) || (b.m_MaxY <= m_MaxY && b.m_MaxY >= m_MinY) || (b.m_MinY >= m_MinY && b.m_MinY <= m_MaxY))
            {
                if((b.m_MinZ <= m_MinZ && b.m_MaxZ >= m_MaxZ) || (b.m_MaxZ <= m_MaxZ && b.m_MaxZ >= m_MinZ) || (b.m_MinZ >= m_MinZ && b.m_MinZ <= m_MaxZ))
                {
                    return true;
                }
            }
        }
        return false;
    }



};

class LISEM_API BoundingBox
{
    friend class BoundingBox3D;

    private:
    double m_MinX = 0.0;
    double m_MaxX = 0.0;
    double m_MinY = 0.0;
    double m_MaxY = 0.0;


    inline float n3min(float a, float b, float c)
    {
        if(a < b)
        {
            return c <a? c: a;
        }else
        {
            return c <b? c: b;
        }
    }


    inline float n3max(float a, float b, float c)
    {
        if(a > b)
        {
            return c >a? c: a;
        }else
        {
            return c >b? c: b;
        }
    }
public:

    inline BoundingBox()
    {

    }
    inline BoundingBox(double xmin,double xmax, double ymin, double ymax)
    {
        Set(xmin,xmax,ymin,ymax);
    }
    inline BoundingBox(LSMVector3 p1, LSMVector3 p2, LSMVector3 p3)
    {

        Set(n3min(p1.x,p2.x,p3.x),n3max(p1.x,p2.x,p3.x),n3min(p1.z,p2.z,p3.z),n3max(p1.z,p2.z,p3.z));

    }

    inline BoundingBox(LSMVector3 p)
    {
        m_Minx = p.x;
        m_Maxx = p.x;
        m_Miny = p.y;
        m_Maxy = p.y;
    }

    inline void Set(double xmin,double xmax, double ymin, double ymax)
    {
        m_MinX = xmin < xmax? xmin : xmax;
        m_MaxX = xmin < xmax? xmax : xmin;
        m_MinY = ymin < ymax? ymin : ymax;
        m_MaxY = ymin < ymax? ymax : ymin;
    }

    inline void Scale(double s)
    {
        double width = GetSizeX();
        double height = GetSizeY();
        double cx = GetCenterX();
        double cy = GetCenterY();

        Set(cx - s* 0.5 * width,cx + s* 0.5 * width,cy - s* 0.5 * height,cy + s* 0.5 * height);
    }

    inline void ScaleX(double s)
    {
        double width = GetSizeX();
        double height = GetSizeY();
        double cx = GetCenterX();
        double cy = GetCenterY();

        Set(cx - s* 0.5 * width,cx + s* 0.5 * width,cy -  0.5 * height,cy + 0.5 * height);
    }

    inline void ScaleY(double s)
    {
        double width = GetSizeX();
        double height = GetSizeY();
        double cx = GetCenterX();
        double cy = GetCenterY();

        Set(cx -  0.5 * width,cx +  0.5 * width,cy - s* 0.5 * height,cy + s* 0.5 * height);
    }

    inline LSMVector2 GetCoordinateRelativeFromAbsolute(LSMVector2 absolute)
    {
        LSMVector2 ret;
        ret.x = (absolute.x - GetMinX())/(std::max(1.0e-30,GetSizeX()));
        ret.y = (absolute.y - GetMinY())/(std::max(1.0e-30,GetSizeY()));
        return ret;
    }

    inline LSMVector2 GetCoordinateAbsoluteFromRelative(LSMVector2 relative)
    {
        LSMVector2 ret;
        ret.x = GetMinX() + relative.x * GetSizeX();
        ret.y = GetMinY() + relative.y * GetSizeY();
        return ret;
    }

    inline bool AllFinite() const
    {
        return (std::isfinite(m_MinX) &&std::isfinite(m_MaxX)&&std::isfinite(m_MaxY)&&std::isfinite(m_MinY));
    }
    inline double GetMinX() const
    {
        return m_MinX;
    }

    inline double GetMaxX() const
    {
        return m_MaxX;
    }
    inline double GetMinY() const
    {
        return m_MinY;
    }
    inline double GetMaxY() const
    {
        return m_MaxY;
    }

    inline double GetCenterX() const
    {
        return 0.5f*(m_MinX + m_MaxX);
    }
    inline double GetCenterY() const
    {
        return 0.5f*(m_MinY + m_MaxY);
    }
    inline double GetSizeX() const
    {
        return fabs((m_MaxX - m_MinX));
    }
    inline double GetSizeY() const
    {
        return fabs((m_MaxY - m_MinY));
    }

    inline bool Contains(double x, double y, double tolerence = 0.0) const
    {
        if((x >= m_MinX - tolerence) && (x <= m_MaxX + tolerence) && (y >= m_MinY - tolerence) && (y <= m_MaxY + tolerence))
        {
            return true;
        }
        return false;
    }

    inline bool Overlaps(BoundingBox b)
    {
        if((b.m_MinX <= m_MinX && b.m_MaxX >= m_MaxX) || (b.m_MaxX <= m_MaxX && b.m_MaxX >= m_MinX) || (b.m_MinX >= m_MinX && b.m_MinX <= m_MaxX))
        {
            if((b.m_MinY <= m_MinY && b.m_MaxY >= m_MaxY) || (b.m_MaxY <= m_MaxY && b.m_MaxY >= m_MinY) || (b.m_MinY >= m_MinY && b.m_MinY <= m_MaxY))
            {
                return true;
            }
        }
        return false;
    }

    inline bool Overlaps(BoundingBox3D b)
    {
        if((b.m_MinX <= m_MinX && b.m_MaxX >= m_MaxX) || (b.m_MaxX <= m_MaxX && b.m_MaxX >= m_MinX) || (b.m_MinX >= m_MinX && b.m_MinX <= m_MaxX))
        {
            if((b.m_MinZ <= m_MinY && b.m_MaxZ >= m_MaxY) || (b.m_MaxZ <= m_MaxY && b.m_MaxZ >= m_MinY) || (b.m_MinZ >= m_MinY && b.m_MinZ <= m_MaxY))
            {
                return true;
            }
        }
        return false;
    }

    inline void MoveInto(BoundingBox b)
    {

        double difminx = m_MinX - b.m_MinX;
        if(difminx < 0)
        {
            m_MinX -= difminx;
            m_MaxX -= difminx;
        }
        double difmaxx = m_MaxX - b.m_MaxX;
        if(difmaxx > 0)
        {
            m_MinX -= difmaxx;
            m_MaxX -= difmaxx;
        }

        double difminy = m_MinY - b.m_MinY;
        if(difminy < 0)
        {
            m_MinY -= difminy;
            m_MaxY -= difminy;
        }
        double difmaxy = m_MaxY - b.m_MaxY;
        if(difmaxy > 0)
        {
            m_MinY -= difmaxy;
            m_MaxY -= difmaxy;
        }

    }
    inline float GetArea()
    {
        return (m_MaxX-m_MinX) *(m_MaxY-m_MinY);
    }

    inline void And(BoundingBox b)
    {
        this->m_MaxX = std::min(b.m_MaxX,this->m_MaxX);
        this->m_MinX = std::max(b.m_MinX,this->m_MinX);
        this->m_MaxY = std::min(b.m_MaxY,this->m_MaxY);
        this->m_MinY = std::max(b.m_MinY,this->m_MinY);
        if(m_MaxX < m_MinX)
        {
            m_MaxX = 0.0;
            m_MinX = 0.0;
        }
        if(m_MaxY < m_MinY)
        {
            m_MaxY = 0.0;
            m_MinY = 0.0;
        }
    }

    inline void MergeWith(BoundingBox b)
    {
        this->m_MaxX = std::max(b.m_MaxX,this->m_MaxX);
        this->m_MinX = std::min(b.m_MinX,this->m_MinX);
        this->m_MaxY = std::max(b.m_MaxY,this->m_MaxY);
        this->m_MinY = std::min(b.m_MinY,this->m_MinY);
    }

    inline void SetAs(LSMVector2 p)
    {
        this->m_MaxX = (double)p.X();
        this->m_MinX = (double)p.X();
        this->m_MaxY = (double)p.Y();
        this->m_MinY = (double)p.Y();
    }

    inline void MergeWith(LSMVector2 p)
    {
        this->m_MaxX = std::max((double)p.X(),this->m_MaxX);
        this->m_MinX = std::min((double)p.X(),this->m_MinX);
        this->m_MaxY = std::max((double)p.Y(),this->m_MaxY);
        this->m_MinY = std::min((double)p.Y(),this->m_MinY);
    }

    static inline BoundingBox Merge(BoundingBox b1, BoundingBox b2)
    {
        b1.MergeWith(b2);
        return b1;
    }


    inline void Move(LSMVector2 d)
    {

        m_MaxX += d.x;
        m_MinX += d.x;
        m_MinY += d.y;
        m_MaxY += d.y;
    }

    inline QString toString() const
    {
        return QString(QString::number(m_MinX) + ";" + QString::number(m_MaxX) + ";" + QString::number(m_MinY)+ ";" + QString::number(m_MaxY));
    }

    inline void fromString(QString x)
    {
        QStringList l = x.split(";");
        if(l.length() == 4)
        {
            m_MinX = l.at(0).toDouble();
            m_MaxX = l.at(1).toDouble();
            m_MinY = l.at(2).toDouble();
            m_MaxY = l.at(3).toDouble();
        }else {
            m_MinX = 0.0;
            m_MaxX = 0.0;
            m_MinY = 0.0;
            m_MaxY = 0.0;
        }
    }

    inline LSMVector3 LimitWithin(LSMVector3 v)
    {
        if(v.x > m_MaxX)
        {
            v.x = m_MaxX;
        }else if(v.x < m_MinX)
        {
            v.x = m_MinX;
        }

        if(v.y > m_MaxY)
        {
            v.y = m_MaxY;
        }else if(v.x < m_MinY)
        {
            v.y = m_MinY;
        }

        return v;
    }

};




#endif // BOUNDINGBOX_H


