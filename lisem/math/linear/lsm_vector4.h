#ifndef VECTOR4_H
#define VECTOR4_H

#include "math.h"
#include "defines.h"

#include "lsm_vector3.h"

class LSMVector4
{
public:

    float x;
    float y;
    float z;
    float w;

    inline LSMVector4()
    {


    }
    inline LSMVector4(float in_x, float in_y, float in_z, float in_w)
    {
        x = in_x;
        y = in_y;
        z = in_z;
        w = in_w;

    }

    inline LSMVector4 normalized()
    {
        double length = sqrt(x*x + y*y + z*z + w*w);
        LSMVector4 out;
        if(length > 0.0)
        {

            out.x = x/length;
            out.y = y/length;
            out.z = z/length;
            out.w = w/length;
        }
        return out;
    }

    inline void Normalize()
    {
        double length = sqrt(x*x + y*y + z*z + w*w);

        if(length > 0.0)
        {
            x = x/length;
            y = y/length;
            z = z/length;
            w = w/length;
        }

    }

    inline LSMVector3 xyz()
    {
        LSMVector3 out;
        out.x = x;
        out.y = y;
        out.z = z;
        return out;
    }
    inline double length() const
    {
        return std::sqrt(x*x + y*y + z*z + w*w);
    }


    inline static float VDot(const LSMVector4 &v1, const LSMVector4 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    inline static LSMVector4 VNormalize(const LSMVector4 &v1)
    {
        LSMVector4 v2 = v1;
        return v2.normalized();
    }
    inline static float VDistance(const LSMVector4 &v1, const LSMVector4 &v2)
    {
        return (v2-v1).length();
    }
    inline static float VLength(const LSMVector4 &v1)
    {
        return v1.length();
    }


    static inline LSMVector4 QFromEulerAngles(double roll, double pitch, double yaw )
    {
        // Abbreviations for the various angular functions
        double cy = cos(yaw * 0.5);
        double sy = sin(yaw * 0.5);
        double cp = cos(pitch * 0.5);
        double sp = sin(pitch * 0.5);
        double cr = cos(roll * 0.5);
        double sr = sin(roll * 0.5);

        LSMVector4 q;
        q.x = cr * cp * cy + sr * sp * sy;
        q.y = sr * cp * cy - cr * sp * sy;
        q.z = cr * sp * cy + sr * cp * sy;
        q.w = cr * cp * sy - sr * sp * cy;

        return q;
    }

    inline LSMVector3 ToEulerAngles()
    {
        LSMVector3 angles;

        // roll (x-axis rotation)
        double sinr_cosp = 2 * (w * x + y * z);
        double cosr_cosp = 1 - 2 * (x * x + y * y);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        double sinp = 2 * (w * y - z * x);
        if (std::abs(sinp) >= 1)
            angles.y = std::copysign(LISEM_PI / 2, sinp); // use 90 degrees if out of range
        else
            angles.y = std::asin(sinp);

        // yaw (z-axis rotation)
        double siny_cosp = 2 * (w * z + x * y);
        double cosy_cosp = 1 - 2 * (y * y + z * z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);

        return angles;

    }

    LSMVector4 operator+(const LSMVector4 & in) const
    {
        LSMVector4 out;
        out.x = x + in.x;
        out.y = y + in.y;
        out.z = z + in.z;
        out.w = w + in.w;
        return out;
    }
    LSMVector4 operator+(const double & in) const
    {
        LSMVector4 out;
        out.x = x + in;
        out.y = y + in;
        out.z = z + in;
        out.w = w + in;
        return out;
    }

    LSMVector4 operator-(const LSMVector4 & in) const
    {
        LSMVector4 out;
        out.x = x - in.x;
        out.y = y - in.y;
        out.z = z - in.z;
        out.w = w - in.w;
        return out;
    }
    LSMVector4 operator-(const double & in) const
    {
        LSMVector4 out;
        out.x = x - in;
        out.y = y - in;
        out.z = z - in;
        out.w = w - in;
        return out;
    }



    LSMVector4 operator-() const
    {
        LSMVector4 out;
        out.x = -x;
        out.y = -y;
        out.z = -z;
        out.w = -w;
        return out;
    }


    LSMVector4 operator*(const double & in) const
    {
        LSMVector4 out;
        out.x = x * in;
        out.y = y * in;
        out.z = z * in;
        out.w = w * in;
        return out;
    }
    LSMVector4 operator*(const float & in) const
    {
        LSMVector4 out;
        out.x = x * in;
        out.y = y * in;
        out.z = z * in;
        out.w = w* in;
        return out;
    }
    LSMVector4 operator/(const LSMVector4 & in) const
    {
        LSMVector4 out;
        out.x = x / in.x;
        out.y = y / in.y;
        out.z = z / in.z;
        out.w = w / in.w;
        return out;
    }

    LSMVector4 operator/(const float & in) const
    {
        LSMVector4 out;
        out.x = x / in;
        out.y = y / in;
        out.z = z / in;
        out.w = w/ in;
        return out;
    }

};

#endif // VECTOR4_H
