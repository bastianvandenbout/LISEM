#ifndef SPHFILLSTYLE_H
#define SPHFILLSTYLE_H
#include "color/colorf.h"
#include "color/lsmcolorgradient.h"

#include "interval.h"


struct SPHFillStyle
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

    bool m_IsLines = false;
    float m_LineSize = 1.0;
    float m_LineSeperation = 1.0;
    float m_LineAngle = 0.0;

    bool m_IsShapes = false;
    int m_ShapeType =0;
    float m_ShapeSize = 1;
    float m_ShapeSeperation = 1;

    bool  m_ScreenUnits;

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

        LSMSerialize::From(obj,"IsLines",m_IsLines);
        LSMSerialize::From(obj,"LineSize",m_LineSize);
        LSMSerialize::From(obj,"LineSeperation",m_LineSeperation);
        LSMSerialize::From(obj,"LineAngle",m_LineAngle);
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

        LSMSerialize::To(obj,"IsLines",m_IsLines);
        LSMSerialize::To(obj,"LineSize",m_LineSize);
        LSMSerialize::To(obj,"LineSeperation",m_LineSeperation);
        LSMSerialize::To(obj,"LineAngle",m_LineAngle);
        LSMSerialize::To(obj,"ShapeType",m_ShapeType);
        LSMSerialize::To(obj,"ShapeSize",m_ShapeSize);
        LSMSerialize::To(obj,"ShapeSeperation",m_ShapeSeperation);
        LSMSerialize::To(obj,"UseScreenUnits",m_ScreenUnits);
    }

};


#define LISEM_FILLSHAPE_NUM 3

#define LISEM_FILLSHAPE_SQUARE 0
#define LISEM_FILLSHAPE_CIRCLE 1
#define LISEM_FILLSHAPE_DIAMOND 2

#define LISEM_FILLSTYLE_NUM 14

#define LISEM_FILLSTYLE_BLACK 0
#define LISEM_FILLSTYLE_RED 1
#define LISEM_FILLSTYLE_BLUE 2
#define LISEM_FILLSTYLE_BLACKLINES 3
#define LISEM_FILLSTYLE_REDLINES 4
#define LISEM_FILLSTYLE_BLUELINES 5
#define LISEM_FILLSTYLE_BLACKLINESBACKGROUND 6
#define LISEM_FILLSTYLE_BLACKDOTTED 7
#define LISEM_FILLSTYLE_REDDOTTED 8
#define LISEM_FILLSTYLE_BLUEDOTTED 9
#define LISEM_FILLSTYLE_BLACKDOTBACKGROUND 10
#define LISEM_FILLSTYLE_BLACKLINESANGLE1 11
#define LISEM_FILLSTYLE_BLACKLINESANGLE2 12
#define LISEM_FILLSTYLE_BLACKLINESANGLE3 13

static inline SPHFillStyle GetDefaultFillStyle(int g)
{

    SPHFillStyle s;


    if(g == LISEM_FILLSTYLE_BLACK)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == LISEM_FILLSTYLE_RED)
    {

        s.m_Color1 = ColorF(1.0,0.0,0.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLUE)
    {

        s.m_Color1 = ColorF(0.0,0.0,1.0,0.5);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.0);
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLACKLINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLUELINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_REDLINES)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,0.67);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLACKLINESBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 0;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLACKDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = LISEM_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_REDDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(1.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = LISEM_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLUEDOTTED)
    {

        s.m_Color1 = ColorF(0.0,0.0,0.0,0.0);
        s.m_Color2 = ColorF(0.0,0.0,1.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = LISEM_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = false;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLACKDOTBACKGROUND)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = false;
        s.m_IsShapes = true;
        s.m_ShapeType = LISEM_FILLSHAPE_CIRCLE;
        s.m_ShapeSize = 1;
        s.m_ShapeSeperation = 1;
        s.m_HasBackGroundColor = true;
        return s;
    }else if(g == LISEM_FILLSTYLE_BLACKLINESANGLE1)
    {

        s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
        s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
        s.m_IsLines = true;
        s.m_LineSize = 1;
        s.m_LineAngle = 90;
        s.m_LineSeperation = 3;
        s.m_HasBackGroundColor = false;
        return s;
    }
    else if(g == LISEM_FILLSTYLE_BLACKLINESANGLE2)
        {

            s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
            s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
            s.m_IsLines = true;
            s.m_LineSize = 1;
            s.m_LineAngle = 135;
            s.m_LineSeperation = 3;
            s.m_HasBackGroundColor = false;
            return s;
        }
    else if(g == LISEM_FILLSTYLE_BLACKLINESANGLE3)
        {

            s.m_Color1 = ColorF(0.2,0.2,0.2,1.0);
            s.m_Color2 = ColorF(0.0,0.0,0.0,1.0);
            s.m_IsLines = true;
            s.m_LineSize = 1;
            s.m_LineAngle = 45;
            s.m_LineSeperation = 3;
            s.m_HasBackGroundColor = false;
            return s;
        }



    return GetDefaultFillStyle(LISEM_FILLSTYLE_BLACK);




}


#endif // SPHFILLSTYLE_H
