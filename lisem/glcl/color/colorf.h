#ifndef COLORF_H
#define COLORF_H

#include "QColor"
#include "serialization.h"
class ColorF  : public LSMSerialize::LSMStringable
{


public:
        float r,g,b,a = 0;
    ColorF()
    {

    }
    ColorF(double in_r, double in_g, double in_b, double in_a)
    {
        r= in_r; g = in_g; b = in_b; a = in_a;
    }
    ColorF(float in_r, float in_g, float in_b, float in_a)
    {
        r= in_r; g = in_g; b = in_b; a = in_a;
    }
    ColorF(int in_r, int in_g, int in_b, int in_a)
    {
        r = (float)(in_r)/255.0f;
        g = (float)(in_g)/255.0f;
        b = (float)(in_b)/255.0f;
        a = (float)(in_a)/255.0f;


    }
    inline void FromQColor(QColor qc)
    {
        r = qc.redF();
        g = qc.greenF();
        b = qc.blueF();
        a = qc.alphaF();
    }

    inline bool operator == (const ColorF & c)
    {
        return (r == c.r && g == c.g && b == c.b && a == c.a);
    }
    inline QColor ToQColor() const
    {
        return QColor(r*255.0f,g * 255.0f,b*255.0f,a*255.0f);
    }

    inline QString toString() const
    {
        return QString(QString::number(r) + ";" + QString::number(g) + ";" + QString::number(b)+ ";" + QString::number(a));
    }

    inline void fromString(QString x)
    {
        QStringList l = x.split(";");
        if(l.length() == 4)
        {
            r = l.at(0).toDouble();
            g = l.at(1).toDouble();
            b = l.at(2).toDouble();
            a = l.at(3).toDouble();
        }else {
            r = 1.0;
            g = 0.0;
            b = 0.0;
            a = 1.0;
        }
    }

};


#endif // COLORF_H
