#ifndef VECTOR2_H
#define VECTOR2_H

class LSMVector2
{
public:

    float x;
    float y;

    inline LSMVector2()
    {
        x = 0.0f;
        y = 0.0f;
    }

    inline LSMVector2(float in_x, float in_y)
    {
        x = in_x;
        y = in_y;
    }

    void setX(const float &x_) { x = x_; }
    const float &getX() const { return x; }
    void setY(const float &y_) { y = y_; }
    const float &getY() const { return y; }


    float dot(LSMVector2 other)
    {
        return x * other.x + y * other.y;
    }

    inline float X()
    {
        return x;
    }

    inline float Y()
    {
        return y;
    }

    inline float length() const
    {
        double lsq = (x*x +y*y);
        return lsq > 0.0f? sqrt(lsq):0.0f;
    }

    inline LSMVector2 Normalize()
    {
        LSMVector2 out;
        out.x = x;
        out.y = y;
        float l = length();
        if(l < 0.000001f)
        {
            return out;
        }else
        {

            return out/l;
        }

    }
    inline LSMVector2 Perpendicular()
    {
        LSMVector2 out;
        out.x = -y;
        out.y = x;
        return out;

    }


    inline static float VDot(const LSMVector2 &v1, const LSMVector2 &v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }
    inline static LSMVector2 VNormalize(const LSMVector2 &v1)
    {
        LSMVector2 v2 = v1;
        return v2.Normalize();
    }
    inline static float VDistance(const LSMVector2 &v1, const LSMVector2 &v2)
    {
        return (v2-v1).length();
    }
    inline static float VLength(const LSMVector2 &v1)
    {
        return v1.length();
    }

    inline double AngleWith(LSMVector2 v2)
    {
        return atan2(x*v2.y-y*v2.x,x*v2.x+y*v2.y);
    }

    inline LSMVector2 RotatedClockWise(float angle)
    {
        float sn = sin(angle);
        float cs = cos(angle);
        LSMVector2 out;
        out.x = x * cs - y * sn;
        out.y = x * sn + y * cs;

        return out;
    }



    LSMVector2 operator+(const LSMVector2 & in) const
    {
        LSMVector2 out;
        out.x = x + in.x;
        out.y = y + in.y;
        return out;
    }
    LSMVector2 operator-(const LSMVector2 & in) const
    {
        LSMVector2 out;
        out.x = x - in.x;
        out.y = y - in.y;
        return out;
    }

    /*LSMVector2 operator*(const float & in) const
    {
        LSMVector2 out;
        out.x = x * in;
        out.y = y * in;
        return out;
    }
    LSMVector2 operator*(const double & in) const
    {
        LSMVector2 out;
        out.x = x * in;
        out.y = y * in;
        return out;
    }*/


    /*LSMVector2 operator/(float in) const
    {
        LSMVector2 out;
        out.x = x / in;
        out.y = y / in;
        return out;
    }*/

    LSMVector2 operator*(float in) const
    {
        LSMVector2 out;
        out.x = x * in;
        out.y = y * in;
        return out;
    }
    LSMVector2 operator*(double in) const
    {
        LSMVector2 out;
        out.x = x * in;
        out.y = y * in;
        return out;
    }


    LSMVector2 operator/(float in) const
    {
        LSMVector2 out;
        out.x = x / in;
        out.y = y / in;
        return out;
    }


    LSMVector2 operator-() const
    {
        LSMVector2 out;
        out.x = -x;
        out.y = -y;
        return out;
    }

   


};
static inline LSMVector2 operator+(const double & in, const LSMVector2 & in2)
{
    LSMVector2 out;
    out.x = in2.x * in;
    out.y = in2.y * in;
    return out;
}
static inline LSMVector2 operator*(const double & in, const LSMVector2 & in2)
{
    LSMVector2 out;
    out.x = in2.x + in;
    out.y = in2.y + in;
    return out;
}

static inline LSMVector2 operator+(const float & in,  const LSMVector2 & in2)
{
    LSMVector2 out;
    out.x = in2.x + in;
    out.y = in2.y + in;
    return out;
}
static inline LSMVector2 operator*(const float & in, const LSMVector2 & in2)
{
    LSMVector2 out;
    out.x = in2.x * in;
    out.y = in2.y * in;
    return out;
}

#endif // VECTOR2_H
