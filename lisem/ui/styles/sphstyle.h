#ifndef SPHSTYLE_H
#define SPHSTYLE_H

#include "color/colorf.h"
#include "QList"
#include "interval.h"
#include "sphlinestyle.h"
#include "sphfillstyle.h"
#include "color/lsmcolorgradient.h"
#include "linear/lsm_vector3.h"
#include "QImage"
#include "QMutex"
#include "mutex"
#include "serialization.h"

#define LISEM_STYLE_DEFAULT_RASTERHS 1
#define LISEM_STYLE_DEFAULT_RASTERWH 2
#define LISEM_STYLE_DEFAULT_RASTERUI 3
#define LISEM_STYLE_DEFAULT_VECTORUI 4
#define LISEM_STYLE_DEFAULT_MULTIRASTER 5
#define LISEM_STYLE_DEFAULT_POINTCLOUDUI 6
#define LISEM_STYLE_DEFAULT_DUORASTER 7
#define LISEM_STYLE_DEFAULT_DUORASTER_VEL 8
#define LISEM_STYLE_DEFAULT_MULTIRASTER2 9

class LSMStyle
{
public:

    inline LSMStyle()
    {

    }

    inline ~LSMStyle()
    {

    }

    inline void Save(QJsonObject * obj)
    {
        LSMSerialize::From(obj,"Transparancy",m_Transparancy);
        LSMSerialize::FromObject<LSMColorGradient>(obj,"ColorGradient1",m_ColorGradientb1);
        LSMSerialize::FromObject<LSMColorGradient>(obj,"ColorGradient2",m_ColorGradientb2);
        LSMSerialize::From<LSMInterval>(obj,"IntervalB1",m_Intervalb1);
        LSMSerialize::From<LSMInterval>(obj,"IntervalB2",m_Intervalb2);
        LSMSerialize::From<LSMInterval>(obj,"IntervalB2",m_Intervalb3);


        LSMSerialize::From(obj,"PointSize",m_PointSize);
        LSMSerialize::FromObject<SPHFillStyle>(obj,"PointFillStyle",m_PointFillStyle);
        LSMSerialize::FromObject<SPHFillStyle>(obj,"PolygonFillStyle",m_PolygonFillStyle);
        LSMSerialize::FromObject<LSMLineStyle>(obj,"PointLineStyle",m_PointLineStyle);
        LSMSerialize::FromObject<LSMLineStyle>(obj,"PolygonLineStyle",m_PolygonLineStyle);
        LSMSerialize::FromObject<LSMLineStyle>(obj,"LineLineStyle",m_LineLineStyle);

        LSMSerialize::From(obj,"Band1",m_IndexB1);
        LSMSerialize::From(obj,"Band2",m_IndexB2);
        LSMSerialize::From(obj,"Band3",m_IndexB3);

        LSMSerialize::From(obj,"CurrentTime",m_CurrentTime);
        LSMSerialize::From(obj,"PlaySpeed",m_PlaySpeed);
        LSMSerialize::From(obj,"Play",m_Play);

        LSMSerialize::From(obj,"IsVectorHeight",m_VectorIsHeight);
        LSMSerialize::From(obj,"VectorHeight",m_VectorHeightValue);
        LSMSerialize::From(obj,"VectorHeightParameter",m_VectorHeightParameter);

        LSMSerialize::From(obj,"CurrentTime",m_CurrentTime);
        LSMSerialize::From(obj,"PlaySpeed",m_PlaySpeed);
        LSMSerialize::From(obj,"Play",m_Play);

        LSMSerialize::From(obj,"DrawEdge",m_DrawEdge);
        LSMSerialize::From(obj,"EdgeLevel",m_EdgeLevel);
        LSMSerialize::From(obj,"DrawAxes",m_Draw3DAxes);
        LSMSerialize::From(obj,"AxesScale",m_3DAxesSize);
        LSMSerialize::From(obj,"DrawBasePlane",m_DrawBasePlane);
        LSMSerialize::From(obj,"BasePlaneLevel",m_BasePlaneLevel);
        LSMSerialize::From(obj,"BasePlaneScale",m_BasePlaneScale);

        LSMSerialize::From<ColorF>(obj,"SunColor",SunColor);
        LSMSerialize::From(obj,"AngleVertical", angle_vert);
        LSMSerialize::From(obj,"AngleHorizontal", angle_hor);
        LSMSerialize::From(obj,"IsElevation",m_IsDEM);
        LSMSerialize::From(obj,"IsHillShade",m_IsHS);
        LSMSerialize::From(obj,"IsVelocity",m_IsVelocity);
        LSMSerialize::From(obj,"IsDuoFractional",m_IsFractional);
        LSMSerialize::From(obj,"ArrowSpacing",m_VelSpacing);
    }

    inline void Restore(QJsonObject * obj)
    {
        std::cout << "style restore " << m_ColorGradientb1.m_Gradient_Stops.length() << std::endl;

        LSMSerialize::To(obj,"Transparancy",m_Transparancy);
        LSMSerialize::ToObject<LSMColorGradient>(obj,"ColorGradient1",m_ColorGradientb1);
        LSMSerialize::ToObject<LSMColorGradient>(obj,"ColorGradient2",m_ColorGradientb2);
        LSMSerialize::To<LSMInterval>(obj,"IntervalB1",m_Intervalb1);
        LSMSerialize::To<LSMInterval>(obj,"IntervalB2",m_Intervalb2);
        LSMSerialize::To<LSMInterval>(obj,"IntervalB2",m_Intervalb3);

        std::cout << "style restore2 " << m_ColorGradientb1.m_Gradient_Stops.length() <<m_Transparancy << std::endl;

        LSMSerialize::To(obj,"PointSize",m_PointSize);
        LSMSerialize::ToObject<SPHFillStyle>(obj,"PointFillStyle",m_PointFillStyle);
        LSMSerialize::ToObject<SPHFillStyle>(obj,"PolygonFillStyle",m_PolygonFillStyle);
        LSMSerialize::ToObject<LSMLineStyle>(obj,"PointLineStyle",m_PointLineStyle);
        LSMSerialize::ToObject<LSMLineStyle>(obj,"PolygonLineStyle",m_PolygonLineStyle);
        LSMSerialize::ToObject<LSMLineStyle>(obj,"LineLineStyle",m_LineLineStyle);

        LSMSerialize::To(obj,"Band1",m_IndexB1);
        LSMSerialize::To(obj,"Band2",m_IndexB2);
        LSMSerialize::To(obj,"Band3",m_IndexB3);

        LSMSerialize::To(obj,"CurrentTime",m_CurrentTime);
        LSMSerialize::To(obj,"PlaySpeed",m_PlaySpeed);
        LSMSerialize::To(obj,"Play",m_Play);

        LSMSerialize::To(obj,"IsVectorHeight",m_VectorIsHeight);
        LSMSerialize::To(obj,"VectorHeight",m_VectorHeightValue);
        LSMSerialize::To(obj,"VectorHeightParameter",m_VectorHeightParameter);

        LSMSerialize::To(obj,"CurrentTime",m_CurrentTime);
        LSMSerialize::To(obj,"PlaySpeed",m_PlaySpeed);
        LSMSerialize::To(obj,"Play",m_Play);

        LSMSerialize::To(obj,"DrawEdge",m_DrawEdge);
        LSMSerialize::To(obj,"EdgeLevel",m_EdgeLevel);
        LSMSerialize::To(obj,"DrawAxes",m_Draw3DAxes);
        LSMSerialize::To(obj,"AxesScale",m_3DAxesSize);
        LSMSerialize::To(obj,"DrawBasePlane",m_DrawBasePlane);
        LSMSerialize::To(obj,"BasePlaneLevel",m_BasePlaneLevel);
        LSMSerialize::To(obj,"BasePlaneScale",m_BasePlaneScale);

        LSMSerialize::To<ColorF>(obj,"SunColor",SunColor);
        LSMSerialize::To(obj,"AngleVertical", angle_vert);
        LSMSerialize::To(obj,"AngleHorizontal", angle_hor);
        LSMSerialize::To(obj,"IsElevation",m_IsDEM);
        LSMSerialize::To(obj,"IsHillShade",m_IsHS);
        LSMSerialize::To(obj,"IsVelocity",m_IsVelocity);
        LSMSerialize::To(obj,"IsDuoFractional",m_IsFractional);
        LSMSerialize::To(obj,"ArrowSpacing",m_VelSpacing);
    }

    bool m_DrawEdge = false;
    double m_EdgeLevel = 0.0;
    bool m_Draw3DAxes = false;
    double m_3DAxesSize = 1.0;
    bool m_DrawBasePlane = false;
    double m_BasePlaneLevel = 0.0;
    double m_BasePlaneScale = 1.0;
    ColorF m_EdgeColor = ColorF(0.95,0.95,0.95,1.0);
    ColorF m_AxesColor = ColorF(0.1,0.1,0.1,1.0);
    ColorF m_BasePlaneColor = ColorF(0.75,0.75,0.75,1.0);

    //these options are available to determine what a user can choose
    float m_Transparancy = 0.0;

    //these values are indications of what the layer needs
    //when setting the style, these values are maintainted from the original
    bool m_HasSingleBand = false;
    bool m_HasDuoBand = false;
    bool m_HasTime = false;
    bool m_HasTripleBand = false;
    bool m_HasVectorPointLine = false;
    bool m_HasVectorPointFill = false;
    bool m_HasVectorLineLine = false;
    bool m_HasVectorPolygonLine = false;
    bool m_HasVectorPolygonFill = false;


    //these are meant for simplified settings within the layer widget
    bool m_StyleSimpleGradient = false; //adds a simple gradient
    bool m_StyleSimpleFillColor = false; //adds a color selection
    bool m_StyleSimpleLineColor = false; //adds a color selection
    bool m_StyleSimpleRange = false; //adds a simple min/max spin box
    bool m_StyleSimpleSize = false; //adds a simple line size selector

    int m_TimeMax = 0;
    int m_TimeMin = 0;

    int m_Bands = 0;




    //the following structures carry the actual drawing information

    //single or multi-band coloring
    //single band means color gradient
    //triple band means rgb
    LSMColorGradient m_ColorGradientb1;
    LSMColorGradient m_ColorGradientb2;
    LSMInterval m_Intervalb1;
    LSMInterval m_Intervalb2;
    LSMInterval m_Intervalb3;
    int m_IndexB1 = 0;
    int m_IndexB2 = 0;
    int m_IndexB3 = 0;

    //time properties
    int m_CurrentTime = 0;
    float m_PlaySpeed = 0.1;
    float m_TimePassed = 0.0;
    bool m_Play = false;

    bool m_VectorIsHeight = false;
    QString m_VectorHeightParameter = "";
    float m_VectorHeightValue = 10.0;


    //DEM properties
    ColorF SunColor;
    float angle_vert = 70.0;
    float angle_hor = 240.0;
    float DemScale = 1.0;
    bool m_IsDEM = false;
    bool m_IsHS = false;

    //surface properties
    bool m_IsSurface = false;
    bool m_IsSurfaceFlow = false;
    bool m_UseSecondDuoBandAsHeight = false;

    //rendering properties
    bool m_RenderTextured = false;

    QMutex * m_RenderTextureMutex = nullptr;

    //textures: bump, normal, specular
    QString m_RenderTextureDiffuseLoc;
    QString m_RenderTextureBumpLoc;
    QString m_RenderTextureNormalLoc;
    QString m_RenderTextureSpecularLoc;

    QString m_RenderTextureDiffuseCLoc;
    QString m_RenderTextureBumpCLoc;
    QString m_RenderTextureNormalCLoc;
    QString m_RenderTextureSpecularCLoc;

    QImage m_RenderTextureDiffuse;
    QImage m_RenderTextureBump;
    QImage m_RenderTextureNormal;
    QImage m_RenderTextureSpecular;

    bool m_RenderInstanced = false;

    //full textured object to instance

    QMutex * m_RenderInstancedMutex = nullptr;

    QString m_RenderInstancedLoc;
    QString m_RenderInstancedCLoc;

    //velocity properties
    bool m_IsVelocity = false;
    int m_VelSpacing = 36;

    bool m_IsFractional = false;

    //for each type of element in shapefile
    //there is a fill or linestyle struct
    float m_PointSize = 5;
    SPHFillStyle m_PointFillStyle;
    LSMLineStyle m_PointLineStyle;
    LSMLineStyle m_LineLineStyle;
    SPHFillStyle m_PolygonFillStyle;
    LSMLineStyle m_PolygonLineStyle;

    bool IncreaseTimeBy(float sec)
    {
        m_TimePassed += sec;
        if(m_TimePassed/m_PlaySpeed > 1.0)
        {
            m_CurrentTime += std::max(0.0f,std::floor(m_TimePassed/m_PlaySpeed));
            m_TimePassed = 0;
            if(m_CurrentTime > m_TimeMax)
            {
                m_CurrentTime = m_TimeMin;
            }
            return true;
        }

        return false;

    }

    float GetTransparancy()
    {
        return m_Transparancy;
    }
    void SetTransparancy(float trans)
    {
        m_Transparancy = trans;
    }

    inline bool FunctionalEquivalence(LSMStyle * s)
    {
        if(s->m_HasSingleBand != m_HasSingleBand)
        {
            return false;
        }
        if(s->m_HasDuoBand != m_HasDuoBand)
        {
            return false;
        }
        if(s->m_HasTime != m_HasTime)
        {
            return false;
        }
        if(s->m_HasTripleBand != m_HasTripleBand)
        {
            return false;
        }
        if(s->m_HasVectorPointLine != m_HasVectorPointLine)
        {
            return false;
        }
        if(s->m_HasVectorPointFill != m_HasVectorPointFill)
        {
            return false;
        }
        if(s->m_HasVectorLineLine != m_HasVectorLineLine)
        {
            return false;
        }
        if(s->m_HasVectorPolygonLine != m_HasVectorPolygonLine)
        {
            return false;
        }
        if(s->m_HasVectorPolygonFill != m_HasVectorPolygonFill)
        {
            return false;
        }


        if(s->m_StyleSimpleGradient != m_StyleSimpleGradient)
        {
            return false;
        }
        if(s->m_StyleSimpleFillColor != m_StyleSimpleFillColor)
        {
            return false;
        }
        if(s->m_StyleSimpleLineColor != m_StyleSimpleLineColor)
        {
            return false;
        }
        if(s->m_StyleSimpleRange != m_StyleSimpleRange)
        {
            return false;
        }
        if(s->m_StyleSimpleSize != m_StyleSimpleSize)
        {
            return false;
        }
        return true;
    }

    inline void CopyFrom(LSMStyle s)
    {
        m_Transparancy = s.m_Transparancy;
        m_ColorGradientb1 = s.m_ColorGradientb1;
        m_ColorGradientb2 = s.m_ColorGradientb2;
        m_Intervalb1 = s.m_Intervalb1;
        m_Intervalb2 = s.m_Intervalb2;
        m_Intervalb3 = s.m_Intervalb3;

        m_PointSize = s.m_PointSize;
        m_PointFillStyle = s.m_PointFillStyle;
        m_PointLineStyle = s.m_PointLineStyle;
        m_LineLineStyle = s.m_LineLineStyle;
        m_PolygonFillStyle = s.m_PolygonFillStyle;
        m_PolygonLineStyle = s.m_PolygonLineStyle;

        m_IndexB1 = s.m_IndexB1;
        m_IndexB2 = s.m_IndexB2;
        m_IndexB3 = s.m_IndexB3;

        m_CurrentTime = s.m_CurrentTime;
        m_PlaySpeed = s.m_PlaySpeed;
        m_Play = s.m_Play;

        m_VectorIsHeight = s.m_VectorIsHeight;
        m_VectorHeightParameter = s.m_VectorHeightParameter;
        m_VectorHeightValue = s.m_VectorHeightValue;

        //DEM properties
        SunColor = s.SunColor;
        DemScale = s.DemScale;
        angle_hor = s.angle_hor;
        angle_vert = s.angle_vert;
        m_IsDEM = s.m_IsDEM;
        m_IsHS = s.m_IsHS;
        m_IsVelocity = s.m_IsVelocity;
        m_IsFractional = s.m_IsFractional;
    }


    inline bool IsSimpleStyleGradient()
    {
        return m_StyleSimpleGradient;
    }

    inline bool IsSimpleStyleFill()
    {
        return m_StyleSimpleFillColor;
    }

    inline bool IsSimpleStyleLine()
    {
        return m_StyleSimpleLineColor;
    }

    inline bool IsSimpleStyleSize()
    {
        return m_StyleSimpleSize;
    }

    inline bool IsSimpleStyleRange()
    {
        return m_StyleSimpleRange;
    }


};

inline static LSMStyle GetStyleDefault(int style)
{


    LSMStyle s;
    if(style == LISEM_STYLE_DEFAULT_RASTERHS)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_HS);

    }else if(style == LISEM_STYLE_DEFAULT_RASTERWH)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH);


    }else if(style == LISEM_STYLE_DEFAULT_RASTERUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
    }else if(style == LISEM_STYLE_DEFAULT_VECTORUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;

        s.m_HasVectorLineLine = true;
        s.m_HasVectorPointLine = true;
        s.m_HasVectorPolygonLine = true;
        s.m_HasVectorPointFill = true;
        s.m_HasVectorPolygonFill = true;

        LSMLineStyle ls = GetDefaultLineStyle(LISEM_LINESTYLE_BLACK);
        SPHFillStyle fs = GetDefaultFillStyle(LISEM_FILLSTYLE_BLACK);

        s.m_LineLineStyle = ls;
        s.m_PointLineStyle = ls;
        s.m_PolygonLineStyle = ls;
        s.m_PointFillStyle = fs;
        s.m_PolygonFillStyle = fs;
    }else if(style == LISEM_STYLE_DEFAULT_MULTIRASTER)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = true;

        s.m_HasVectorLineLine = false;
        s.m_HasVectorPointLine = false;
        s.m_HasVectorPolygonLine = false;
        s.m_HasVectorPointFill = false;
        s.m_HasVectorPolygonFill = false;

        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
    }else if(style == LISEM_STYLE_DEFAULT_MULTIRASTER2)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = true;

        s.m_HasVectorLineLine = false;
        s.m_HasVectorPointLine = false;
        s.m_HasVectorPolygonLine = false;
        s.m_HasVectorPointFill = false;
        s.m_HasVectorPolygonFill = false;

        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 1;
    }else if(style == LISEM_STYLE_DEFAULT_POINTCLOUDUI)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = true;
        s.m_StyleSimpleRange = true;
        s.m_HasSingleBand = true;
        s.m_HasTripleBand = false;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_UI);
    }else if(style == LISEM_STYLE_DEFAULT_DUORASTER)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasDuoBand = true;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;
        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_WH_FLUIDS);
        s.m_ColorGradientb2 = GetDefaultGradient(LISEM_GRADIENT_WH_SOLIDS);
    }else if(style == LISEM_STYLE_DEFAULT_DUORASTER_VEL)
    {
        s.m_Transparancy = 0.0f;
        s.m_StyleSimpleGradient = false;
        s.m_StyleSimpleRange = false;
        s.m_HasDuoBand = true;
        s.m_HasSingleBand = false;
        s.m_HasTripleBand = false;
        s.m_IsVelocity = true;
        s.m_IndexB1 = 0;
        s.m_IndexB2 = 1;
        s.m_IndexB3 = 2;
        s.m_ColorGradientb1 = GetDefaultGradient(LISEM_GRADIENT_VEL_WHITE);
        s.m_ColorGradientb2 = GetDefaultGradient(LISEM_GRADIENT_WH);
    }




    return s;
}

#endif // SPHSTYLE_H
