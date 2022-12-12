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

    static inline LSMVector4 QMultiply(const LSMVector4 &a,const LSMVector4 &b)
    {
        LSMVector4 ret;
        ret.x = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;  // 1
        ret.y = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;   // i
        ret.z = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;  // j
        ret.w = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;   // k

        return ret;
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

    static inline LSMVector4 QFromAxisAngle(const double &xx, const double &yy, const double &zz, const double &a)
    {
        // Here we calculate the sin( theta / 2) once for optimization
        double factor = sin( a / 2.0 );

        // Calculate the x, y and z of the quaternion
        double x = xx * factor;
        double y = yy * factor;
        double z = zz * factor;

        // Calcualte the w value by cos( theta / 2 )
        double w = cos( a / 2.0 );

        return LSMVector4(w,x,y,z).normalized();
    }

    //rotation matrix from quaternion
    /*inline QMatrix4x4 quatToMat(QQuaternion q)
    {
        //based on algorithm on wikipedia
        // http://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
        float w = q.scalar ();
        float x = q.x();
        float y = q.y();
        float z = q.z();

        float n = q.lengthSquared();
        float s =  n == 0?  0 : 2 / n;
        float wx = s * w * x, wy = s * w * y, wz = s * w * z;
        float xx = s * x * x, xy = s * x * y, xz = s * x * z;
        float yy = s * y * y, yz = s * y * z, zz = s * z * z;

        float m[16] = { 1 - (yy + zz),         xy + wz ,         xz - wy ,0,
                             xy - wz ,    1 - (xx + zz),         yz + wx ,0,
                             xz + wy ,         yz - wx ,    1 - (xx + yy),0,
                                   0 ,               0 ,               0 ,1  };
        QMatrix4x4 result =  QMatrix4x4(m,4,4);
        result.optimize ();
        return result;
    }



    Quaternion Quaternion::Lerp(Quaternion& from, Quaternion& to, float t) {
        Quaternion src = from * (1.0f - t);
        Quaternion dst = to * t;

        Quaternion q = src + dst;
        return q;
    }

    Quaternion Quaternion::Slerp(Quaternion& from, Quaternion& to, float t) {
        float cosTheta = Quaternion::Dot(from, to);
        Quaternion temp(to);

        if (cosTheta < 0.0f) {
            cosTheta *= -1.0f;
            temp = temp * -1.0f;
        }

        float theta = acosf(cosTheta);
        float sinTheta = 1.0f / sinf(theta);

        return sinTheta * (
            ((Quaternion)(from * sinf(theta * (1.0f - t)))) +
            ((Quaternion)(temp * sinf(t * theta)))
            );
    }

    #define m00 right.x
    #define m01 up.x
    #define m02 forward.x
    #define m10 right.y
    #define m11 up.y
    #define m12 forward.y
    #define m20 right.z
    #define m21 up.z
    #define m22 forward.z

    Quaternion Quaternion::LookRotation(Vector& lookAt, Vector& upDirection) {
        Vector forward = lookAt; Vector up = upDirection;
        Vector::OrthoNormalize(&forward, &up);
        Vector right = Vector::Cross(up, forward);

        Quaternion ret;
        ret.w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
        float w4_recip = 1.0f / (4.0f * ret.w);
        ret.x = (m21 - m12) * w4_recip;
        ret.y = (m02 - m20) * w4_recip;
        ret.z = (m10 - m01) * w4_recip;

        return ret;
    }

    Quaternion Quaternion::LookRotation(Vector& lookAt) {
        Vector up = (Vector)Vector::up;
        Vector forward = lookAt;
        Vector::OrthoNormalize(&forward, &up);
        Vector right = Vector::Cross(up, forward);

        Quaternion ret;
        ret.w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
        float w4_recip = 1.0f / (4.0f * ret.w);
        ret.x = (m21 - m12) * w4_recip;
        ret.y = (m02 - m20) * w4_recip;
        ret.z = (m10 - m01) * w4_recip;

        return ret;
    }

    void Quaternion::SetLookRotation(Vector& lookAt) {
        Vector up = (Vector)Vector::up;
        Vector forward = lookAt;
        Vector::OrthoNormalize(&forward, &up);
        Vector right = Vector::Cross(up, forward);

        w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
        float w4_recip = 1.0f / (4.0f * w);
        x = (m21 - m12) * w4_recip;
        y = (m02 - m20) * w4_recip;
        z = (m10 - m01) * w4_recip;
    }

    void Quaternion::SetLookRotation(Vector& lookAt, Vector& upDirection) {
        Vector forward = lookAt; Vector up = upDirection;
        Vector::OrthoNormalize(&forward, &up);
        Vector right = Vector::Cross(up, forward);

        w = sqrtf(1.0f + m00 + m11 + m22) * 0.5f;
        float w4_recip = 1.0f / (4.0f * w);
        x = (m21 - m12) * w4_recip;
        y = (m02 - m20) * w4_recip;
        z = (m10 - m01) * w4_recip;
    }



    */

    inline LSMVector3 ToEulerAngles()
    {
        double wh = x;
        double xh = y;
        double yh = z;
        double zh = w;

        LSMVector3 angles;

        // roll (x-axis rotation)
        double sinr_cosp = 2 * (wh * xh + yh * zh);
        double cosr_cosp = 1 - 2 * (xh * xh + yh * yh);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        double sinp = 2 * (wh * yh - zh * xh);
        if (std::abs(sinp) >= 1)
            angles.y = std::copysign(LISEM_PI / 2, sinp); // use 90 degrees if out of range
        else
            angles.y = std::asin(sinp);

        // yaw (z-axis rotation)
        double siny_cosp = 2 * (wh * zh + xh * yh);
        double cosy_cosp = 1 - 2 * (yh * yh + zh * zh);
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
