#ifndef VECTOR3_H
#define VECTOR3_H

#include "math.h"
#include "QString"
#include "QStringList"
#include "angelscript.h"

class LSMVector3
{
    public:

    float x = 0;
    float y = 0;
    float z = 0;


    inline LSMVector3()
    {


    }
    inline LSMVector3(float in_x, float in_y, float in_z)
    {
        x = in_x;
        y = in_y;
        z = in_z;

    }

    void setX(const float &x_) { x = x_; }
    const float &getX() const { return x; }
    void setY(const float &y_) { y = y_; }
    const float &getY() const { return y; }
    void setZ(const float &z_) { z = z_; }
    const float &getZ() const { return z; }

    inline float X()
    {
        return x;
    }
    inline float Y()
    {
        return y;
    }
    inline float Z()
    {
        return z;
    }


    inline void Substract(LSMVector3 vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }
    inline float length() const
    {
        double lsq = (x*x +y*y +z*z);
        return lsq > 0.0f? sqrt(lsq):0.0f;
    }

    inline float &GetComponent(int i)
    {
        if(i == 0)
        {
            return x;
        }else if(i == 1)
        {
            return y;
        }else
        {
            return z;
        }
    }

    float &operator[](int i) {
        if(i == 0)
        {
            return x;
        }else if(i == 1)
        {
            return y;
        }else
        {
            return z;
        }
       }

    inline LSMVector3 ProjectedOnTwoAxes(LSMVector3 axis1, LSMVector3 axis2)
    {
        LSMVector3 res;
        res.x = axis1.dot(*this);
        res.y = axis2.dot(*this);
        return res;
    }


    inline LSMVector3 Normalize()
    {
        LSMVector3 out;
        out.x = x;
        out.y = y;
        out.z = z;
        float l = length();
        if(l < 0.000001f)
        {
            return out;
        }else
        {

            return out/l;
        }

    }

    inline float Angle(const LSMVector3 &other)
    {
        float x1 = x;
        float x2 = other.x;
        float y1 = y;
        float y2 = other.y;
        float z1 = z;
        float z2 = other.z;


        float dot = x1*x2 + y1*y2 + z1*z2;
        float lenSq1 = x1*x1 + y1*y1 + z1*z1;
        float lenSq2 = x2*x2 + y2*y2 + z2*z2;
        if(lenSq1 * lenSq2 > 0.0)
        {
            return acos(dot/sqrt(lenSq1 * lenSq2));
        }else {
            return 0.0;
        }
    }

    inline double dot(const LSMVector3 &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline static LSMVector3 CrossProduct(const LSMVector3 &v1, const LSMVector3 &v2)
    {
        LSMVector3 out;
        out.x = v1.y * v2.z - v1.z * v2.y;
        out.y = v1.z * v2.x - v1.x * v2.z;
        out.z = v1.x * v2.y - v1.y * v2.x;

        return out;
    }

    inline static LSMVector3 VReflect(const LSMVector3 &v1, const LSMVector3 &v2)
    {
        //glsl implementation
        return v1 -v2*2.0 *  VDot(v2,v1);
    }

    inline static LSMVector3 VRefract(const LSMVector3 &I, const LSMVector3 &N, float eta)
    {
        //glsl implementation
        float k = 1.0 - eta * eta * (1.0 - VDot(N, I) * VDot(N, I));
        LSMVector3 R;
       if (k < 0.0)
       {
           R = LSMVector3(0.0,0.0,0.0);
       }
       else
       {
           R =  I *eta-  N*(eta * VDot(N, I) + sqrt(k));
       }
       return R;
    }

    inline static LSMVector3 mix(const LSMVector3 &v1, const LSMVector3 &v2, float a)
    {
        return v1 * (1.0-a) + v2 * a;
    }

    inline static LSMVector3 mix(const LSMVector3 &v1, const LSMVector3 &v2, LSMVector3 a)
    {
        return LSMVector3(v1.x * (1.0-a.x) + v2.x* a.x,v1.y * (1.0-a.y) + v2.y* a.y,v1.z * (1.0-a.z) + v2.x* a.z);
    }


    inline static float VDot(const LSMVector3 &v1, const LSMVector3 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    inline static LSMVector3 VNormalize(const LSMVector3 &v1)
    {
        LSMVector3 v2 = v1;
        return v2.Normalize();
    }
    inline static float VDistance(const LSMVector3 &v1, const LSMVector3 &v2)
    {
        return (v2-v1).length();
    }
    inline static float VLength(const LSMVector3 &v1)
    {
        return v1.length();
    }


    inline static float smoothstep(float edge0, float edge1, float x)
    {
        x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        // Evaluate polynomial
        return x * x * (3 - 2 * x);
    }

    LSMVector3 operator+(const LSMVector3 & in) const
    {
        LSMVector3 out;
        out.x = x + in.x;
        out.y = y + in.y;
        out.z = z + in.z;
        return out;
    }
    LSMVector3 operator+(const double & in) const
    {
        LSMVector3 out;
        out.x = x + in;
        out.y = y + in;
        out.z = z + in;
        return out;
    }

    LSMVector3 operator-(const LSMVector3 & in) const
    {
        LSMVector3 out;
        out.x = x - in.x;
        out.y = y - in.y;
        out.z = z - in.z;
        return out;
    }
    LSMVector3 operator*(const double & in) const
    {
        LSMVector3 out;
        out.x = x * in;
        out.y = y * in;
        out.z = z * in;
        return out;
    }
    LSMVector3 operator*(const float & in) const
    {
        LSMVector3 out;
        out.x = x * in;
        out.y = y * in;
        out.z = z * in;
        return out;
    }
    LSMVector3 operator/(const LSMVector3 & in) const
    {
        LSMVector3 out;
        out.x = x / in.x;
        out.y = y / in.y;
        out.z = z / in.z;
        return out;
    }

    LSMVector3 operator/(const float & in) const
    {
        LSMVector3 out;
        out.x = x / in;
        out.y = y / in;
        out.z = z / in;
        return out;
    }

    LSMVector3 operator/(const double & in) const
    {
        LSMVector3 out;
        out.x = x / in;
        out.y = y / in;
        out.z = z / in;
        return out;
    }


    LSMVector3 operator-() const
    {
        LSMVector3 out;
        out.x = -x;
        out.y = -y;
        out.z = -z;
        return out;
    }


    inline QString toString() const
    {
        return QString(QString::number(x) + ";" + QString::number(y) + ";" + QString::number(z));
    }

    inline void fromString(QString in)
    {
        QStringList l = in.split(";");
        if(l.length() == 3)
        {
            x = l.at(0).toDouble();
            y = l.at(1).toDouble();
            z = l.at(2).toDouble();
        }else {
            x = 0.0;
            y = 0.0;
            z = 0.0;
        }
    }

    inline void AS_FromList(void*buf)
    {
        asUINT length = *(asUINT*)buf;
        float * f = (float*)buf;

        x = *f;
        f++;
        y = *f;
        f++;
        z = *f;
    }
};



inline LSMVector3 operator*(const double & in, const LSMVector3 &v)
{
    LSMVector3 out;
    out.x = v.x * in;
    out.y = v.y * in;
    out.z = v.z * in;
    return out;
}
inline LSMVector3 operator*(const float & in, const LSMVector3 &v)
{
    LSMVector3 out;
    out.x = v.x * in;
    out.y = v.y * in;
    out.z = v.z * in;
    return out;
}



#endif // VECTOR3_H
