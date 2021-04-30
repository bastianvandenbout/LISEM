#ifndef SPHLINESTYLE_H
#define SPHLINESTYLE_H
#include "color/colorf.h"
#include "interval.h"
#include "QJsonObject"
#include "serialization.h"
#include "color/lsmcolorgradient.h"

struct LSMLineStyle
{
    bool m_DoDraw = false;

    bool m_HasBackGroundColor = false;
    ColorF m_Color1;
    ColorF m_Color2;
    QString m_Color1Parameter;
    QString m_Color2Parameter;
    LSMInterval m_Color1Interval;
    LSMInterval m_Color2Interval;
    LSMColorGradient m_ColorGradient1;
    LSMColorGradient m_ColorGradient2;


    QString m_LineWParameter;
    LSMInterval m_LineWInterval;
    float m_LineWMax = 2.0;
    bool m_HasPattern = false;
    QList<float> pattern;
    int m_ShapeType =0;
    float m_ShapeSize = 1;
    float m_ShapeSeperation = 1;
    bool m_ScreenUnits = false;

    inline void Save(QJsonObject * obj)
    {
        LSMSerialize::From(obj,"Draw",m_DoDraw);
        LSMSerialize::From(obj,"HasBackGroundColor",m_HasBackGroundColor);
        LSMSerialize::From(obj,"Color1",m_Color1);
        LSMSerialize::From(obj,"Color2",m_Color2);
        LSMSerialize::From(obj,"Color1Parameter",m_Color1Parameter);
        LSMSerialize::From(obj,"Color2Parameter",m_Color2Parameter);
        LSMSerialize::From(obj,"Color1Interval",m_Color1Interval);
        LSMSerialize::From(obj,"Color2Interval",m_Color2Interval);
        LSMSerialize::FromObject<LSMColorGradient>(obj,"ColorGradient1",m_ColorGradient1);
        LSMSerialize::FromObject<LSMColorGradient>(obj,"ColorGradient2",m_ColorGradient2);

        LSMSerialize::From(obj,"LineWParameter",m_LineWParameter);
        LSMSerialize::From(obj,"LineWInterval",m_LineWInterval);
        LSMSerialize::From(obj,"LineWMax",m_LineWMax);
        LSMSerialize::From(obj,"HasPattern",m_HasPattern);
        LSMSerialize::From(obj,"Pattern",pattern);
        LSMSerialize::From(obj,"ShapeType",m_ShapeType);
        LSMSerialize::From(obj,"ShapeSize",m_ShapeSize);
        LSMSerialize::From(obj,"ShapeSeperation",m_ShapeSeperation);
        LSMSerialize::From(obj,"UseScreenUnits",m_ScreenUnits);
    }

    inline void Restore(QJsonObject * obj)
    {
        LSMSerialize::To(obj,"Draw",m_DoDraw);
        LSMSerialize::To(obj,"HasBackGroundColor",m_HasBackGroundColor);
        LSMSerialize::To(obj,"Color1",m_Color1);
        LSMSerialize::To(obj,"Color2",m_Color2);
        LSMSerialize::To(obj,"Color1Parameter",m_Color1Parameter);
        LSMSerialize::To(obj,"Color2Parameter",m_Color2Parameter);
        LSMSerialize::To(obj,"Color1Interval",m_Color1Interval);
        LSMSerialize::To(obj,"Color2Interval",m_Color2Interval);
        LSMSerialize::ToObject<LSMColorGradient>(obj,"ColorGradient1",m_ColorGradient1);
        LSMSerialize::ToObject<LSMColorGradient>(obj,"ColorGradient2",m_ColorGradient2);

        LSMSerialize::To(obj,"LineWParameter",m_LineWParameter);
        LSMSerialize::To(obj,"LineWInterval",m_LineWInterval);
        LSMSerialize::To(obj,"LineWMax",m_LineWMax);
        LSMSerialize::To(obj,"HasPattern",m_HasPattern);
        LSMSerialize::To(obj,"Pattern",pattern);
        LSMSerialize::To(obj,"ShapeType",m_ShapeType);
        LSMSerialize::To(obj,"ShapeSize",m_ShapeSize);
        LSMSerialize::To(obj,"ShapeSeperation",m_ShapeSeperation);
        LSMSerialize::To(obj,"UseScreenUnits",m_ScreenUnits);
    }

};

#define LISEM_LINESHAPE_NUM 11

#define LISEM_LINESHAPE_SQUARE 0
#define LISEM_LINESHAPE_CIRCLE 1
#define LISEM_LINESHAPE_DIAMOND 2

#define LISEM_LINESTYLE_NUM 12

#define LISEM_LINESTYLE_BLACK 0
#define LISEM_LINESTYLE_RED 1
#define LISEM_LINESTYLE_BLUE 2
#define LISEM_LINESTYLE_BLACKDASHED 3
#define LISEM_LINESTYLE_REDDASHED 4
#define LISEM_LINESTYLE_BLUEDASHED 5
#define LISEM_LINESTYLE_BLACKDASHEDBACKGROUND 6
#define LISEM_LINESTYLE_BLACKDOTTED 7
#define LISEM_LINESTYLE_REDDOTTED 8
#define LISEM_LINESTYLE_BLUEDOTTED 9
#define LISEM_LINESTYLE_BLACKDOTBACKGROUND 10

static inline LSMLineStyle GetDefaultLineStyle(int g)
{

    LSMLineStyle s;


    if(g == LISEM_LINESTYLE_BLACK)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == LISEM_LINESTYLE_RED)
    {

        s.m_Color1 = ColorF(1.0,0.0,0.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == LISEM_LINESTYLE_BLUE)
    {

        s.m_Color1 = ColorF(0.0,0.0,1.0,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = false;
        return s;
    }else if(g == LISEM_LINESTYLE_BLACKDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_SQUARE;
        return s;
    }else if(g == LISEM_LINESTYLE_BLUEDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_SQUARE;
        return s;
    }else if(g == LISEM_LINESTYLE_REDDASHED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_SQUARE;
        return s;
    }else if(g == LISEM_LINESTYLE_BLACKDASHEDBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_SQUARE;
        return s;
    }else if(g == LISEM_LINESTYLE_BLACKDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_CIRCLE;
        return s;
    }else if(g == LISEM_LINESTYLE_BLUEDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_CIRCLE;
        return s;
    }else if(g == LISEM_LINESTYLE_REDDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = false;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_CIRCLE;
        return s;
    }else if(g == LISEM_LINESTYLE_BLACKDOTBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_HasBackGroundColor = true;
        s.m_HasPattern = true;
        s.pattern = {1.0,1.0};
        s.pattern = {1.0,1.0};
        s.m_ShapeType = LISEM_LINESHAPE_CIRCLE;        return s;
    }

    return GetDefaultLineStyle(LISEM_LINESTYLE_BLACK);




}

#endif // SPHLINESTYLE_H
