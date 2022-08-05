#ifndef LSMCOLORGRADIENT_H
#define LSMCOLORGRADIENT_H

#include "iostream"
#include "color/colorf.h"
#include "QList"
#include "math.h"
#include "QJsonObject"
#include "serialization.h"
#include "iostream"

#include "QJsonDocument"
class LSMColorGradient
{
public:

    QList<float> m_Gradient_Stops;
    QList<ColorF> m_Gradient_Colors;

    inline LSMColorGradient(QList<float> stops, QList<ColorF> colors)
    {
        m_Gradient_Stops = stops;
        m_Gradient_Colors = colors;
    }
    inline LSMColorGradient()
    {

    }
    inline void MakeValid()
    {

        if(m_Gradient_Stops.length() < 2)
        {

            m_Gradient_Stops.clear();
            m_Gradient_Colors.clear();
            m_Gradient_Stops.append(0.0);
            m_Gradient_Stops.append(1.0);
            m_Gradient_Colors.append(ColorF(0.0,0.0,0.0,1.0));
            m_Gradient_Colors.append(ColorF(1.0,1.0,1.0,1.0));
        }

        if(m_Gradient_Colors.length() < m_Gradient_Stops.length())
        {

        }
    }
    inline bool operator == (const LSMColorGradient & cg)
    {

        return (m_Gradient_Stops == cg.m_Gradient_Stops && m_Gradient_Colors == cg.m_Gradient_Colors);
    }

    inline ColorF MixColors(ColorF c1, ColorF c2, float fac)
    {
        ColorF ret;
        ret.r = fac * c2.r + (1.0-fac) * c1.r;
        ret.g = fac * c2.g + (1.0-fac) * c1.g;
        ret.b = fac * c2.b + (1.0-fac) * c1.b;
        ret.a = fac * c2.a + (1.0-fac) * c1.a;

                return ret;
    }

    inline ColorF ColorAt(float stop)
    {
        if(m_Gradient_Stops.length() > 0)
        {
            for(int i = 0; i < m_Gradient_Stops.length(); i++)
            {
                if(m_Gradient_Stops.at(i) > stop)
                {
                    if(i == 0)
                    {
                        return m_Gradient_Colors.at(0);
                    }else {
                        return MixColors(m_Gradient_Colors.at(i-1),m_Gradient_Colors.at(i),(stop -m_Gradient_Stops.at(i-1))/std::max(0.000001f,m_Gradient_Stops.at(i)-m_Gradient_Stops.at(i-1)));
                    }
                }
            }

            return m_Gradient_Colors.at(m_Gradient_Colors.length()-1);
        }
        return ColorF(0,0,0,1);

    }

    inline LSMColorGradient Invert()
    {
        std::cout <<"pre " << std::endl;
        for(int i = 0; i < m_Gradient_Stops.length(); i++)
        {
            std::cout << "stop i " << i << " " << m_Gradient_Stops.at(i);
            std::cout << "color i " << i << " " << m_Gradient_Colors.at(i).r<< " " << m_Gradient_Colors.at(i).g<< " " << m_Gradient_Colors.at(i).b<< " " << m_Gradient_Colors.at(i).a<<std::endl;;
        }

        LSMColorGradient gradient;
        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;


        int length = m_Gradient_Colors.length();
        for(int i = 0; i < m_Gradient_Colors.length(); i++)
        {
            if(m_Gradient_Stops.at(i) > 1.0)
            {
                length = i;
                break;
            }
        }

        for(int k = 0; k < (length/2); k++)
        {
            std::cout << "swap " << k << " " << length-(1+k) << std::endl;
            gradient.m_Gradient_Colors.swap(k,length-(1+k));
        }

        std::cout <<"post " << length << std::endl;
        for(int i = 0; i < m_Gradient_Stops.length(); i++)
        {
            std::cout << "stop i " << i << " " << gradient.m_Gradient_Stops.at(i);
            std::cout << "color i " << i << " " << gradient.m_Gradient_Colors.at(i).r<< " " <<  gradient.m_Gradient_Colors.at(i).g<< " " <<  gradient.m_Gradient_Colors.at(i).b<< " " <<  gradient.m_Gradient_Colors.at(i).a<<std::endl;;
        }

        return gradient;

    }

    inline LSMColorGradient ApplyLog()
    {
        LSMColorGradient gradient;

        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        for(int i = 0; i < gradient.m_Gradient_Colors.length(); i++)
        {
            if(gradient.m_Gradient_Stops.at(i) < 1.0 && gradient.m_Gradient_Stops.at(i) > 0.0)
            {
                float stop_new = std::pow(gradient.m_Gradient_Stops.at(i),4);
                gradient.m_Gradient_Stops.replace(i,stop_new);
            }
        }

        return gradient;

    }
    inline LSMColorGradient ApplyInvLog()
    {
        LSMColorGradient gradient;


        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;

        for(int i = 0; i < gradient.m_Gradient_Colors.length(); i++)
        {
            if(gradient.m_Gradient_Stops.at(i) < 1.0 && gradient.m_Gradient_Stops.at(i) > 0.0)
            {
                float stop_new = std::pow(gradient.m_Gradient_Stops.at(i),0.25);
                gradient.m_Gradient_Stops.replace(i,stop_new);
            }
        }

        return gradient;

    }

    inline LSMColorGradient ToggleTransparancy()
    {
        LSMColorGradient gradient;
        gradient.m_Gradient_Stops = m_Gradient_Stops;
        gradient.m_Gradient_Colors = m_Gradient_Colors;


        if(m_Gradient_Stops.length() < 2)
        {
            gradient.m_Gradient_Stops.clear();
            gradient.m_Gradient_Colors.clear();
            gradient.m_Gradient_Stops.append(0.0);
            gradient.m_Gradient_Stops.append(1.0);
            gradient.m_Gradient_Colors.append(ColorF(0.0,0.0,0.0,1.0));
            gradient.m_Gradient_Colors.append(ColorF(1.0,1.0,1.0,1.0));

        }else {

            ColorF c1 = gradient.m_Gradient_Colors.at(0);
            if(c1.a < 0.99)
            {
                //remove transparancy
                for(int i = 0; i < m_Gradient_Stops.length(); i++)
                {
                    ColorF c = gradient.m_Gradient_Colors.at(i);
                    c.a = 1.0;
                    gradient.m_Gradient_Colors.replace(i,c);

                }
            }else {

                //add transparancy
                float threshold = std::sqrt(((float)(m_Gradient_Stops.length())));
                float visibility = 0.0;
                for(int i = 0; i < std::min(threshold,(float)m_Gradient_Stops.length()); i++)
                {
                    ColorF c = gradient.m_Gradient_Colors.at(i);
                    c.a = visibility;
                    gradient.m_Gradient_Colors.replace(i,c);
                    if(i == 0)
                    {
                        visibility = 0.5;
                    }else {
                        visibility = 1.0-((1.0-visibility) * 0.5);
                    }
                }
            }

        }

        return gradient;


    }

    inline void Save(QJsonObject * obj)
    {
        LSMSerialize::FromQListFloat(obj,"ColorStops",m_Gradient_Stops);

        QList<QString> colors;
        for(int i = 0; i < m_Gradient_Colors.length(); i++)
        {
            colors.append(m_Gradient_Colors.at(i).toString());
        }

        LSMSerialize::FromQListString(obj,"Colors",colors);


    }

    inline void Restore(QJsonObject * obj)
    {
        std::cout << "restore color gradient" << std::endl;
        m_Gradient_Stops.clear();
        m_Gradient_Colors.clear();
        LSMSerialize::ToQListFloat(obj,"ColorStops",m_Gradient_Stops);

        QList<QString> colors;
        LSMSerialize::ToQListString(obj,"Colors",colors);

        for(int i = 0;i < colors.length(); i++)
        {
            ColorF c;
            c.fromString(colors.at(i));
            m_Gradient_Colors.append(c);
        }

        std::cout << "count " << m_Gradient_Stops.length() << " " << m_Gradient_Colors.length() << std::endl;

    }
};


#define LISEM_GRADIENT_NCOLORS 10

const static float COLORGRADIENT_DEFEAULT_WH_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.001,0.01,0.025,0.05,0.1,0.25,0.5,0.75,1.0};

const static ColorF COLORGRADIENT_DEFEAULT_WH_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(0.9019,0.9764,1.0,0.0),
    ColorF(0.9019,0.9764,1.0,0.25),
    ColorF(0.0,1.0,1.0,0.5),
    ColorF(0.0,0.8,1.0,0.75),
    ColorF(0.0,0.6,1.0,1.0),
    ColorF(0.0,0.2,0.8,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.5,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_UI_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.725,0.85,1.0};

const static ColorF COLORGRADIENT_DEFEAULT_UI_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(53,3,355,255),
    ColorF(33,101,112,255),
    ColorF(115,238,255,255),
    ColorF(11,255,3,255),
    ColorF(171,255,3,255),
    ColorF(255,221,3,255),
    ColorF(255,150,3,255),
    ColorF(255,11,3,255),
    ColorF(255,3,255,255),
    ColorF(50,50,50,255)

};

const static float COLORGRADIENT_DEFEAULT_GRAYSCALE_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_GRAYSCALE_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_RYG_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.5,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_RYG_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,0.0,1.0),
    ColorF(0.0,1.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_ELEVATION_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.499999,0.50001,0.505,0.65,0.75,1.0,2.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_ELEVATION_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(0.01, 0.02, 0.08, 1.0),
    ColorF(0.11, 0.28, 0.51, 1.0),
    ColorF(0.08, 0.14, 0.03, 1.0),
    ColorF(0.18, 0.26, 0.08, 1.0),
    ColorF(0.52, 0.39, 0.26, 1.0),
    ColorF(0.32, 0.3, 0.2, 1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_RBGB_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.33,0.66,1.0,2.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_RBGB_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.8,0.2,0.2,1.0),
    ColorF(0.0,1.0,0.0,1.0),
    ColorF(0.0,0.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_BLUES_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_BLUES_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_REDS_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_REDS_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(0.0,0.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};



const static float COLORGRADIENT_DEFEAULT_MAGMA_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.2,0.4,0.6,0.8,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_MAGMA_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(0.2,0.1,0.4,1.0),
    ColorF(0.65,0.12,0.3,1.0),
    ColorF(0.95,0.4,0.12,1.0),
    ColorF(0.96,0.97,0.75,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};

const static float COLORGRADIENT_DEFEAULT_FLUIDS_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.001,0.05,0.15,0.3,0.6,1.0,2.0,2.0,2.0};

const static ColorF COLORGRADIENT_DEFEAULT_FLUIDS_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(0.9019,0.9764,1.0,0.0),
    ColorF(0.9019,0.9764,1.0,0.25),
    ColorF(0.0,1.0,1.0,0.5),
    ColorF(0.0,0.8,1.0,0.75),
    ColorF(0.0,0.6,1.0,1.0),
    ColorF(0.0,0.2,0.8,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0),
    ColorF(0.0,0.0,0.5,1.0)

};


const static float COLORGRADIENT_DEFEAULT_SOLIDS_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,0.1,0.3,0.5,0.75,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_SOLIDS_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,0.0),
    ColorF(1.0,1.0,0.0,1.0),
    ColorF(1.0,0.6,0.0,1.0),
    ColorF(0.65,0.4,0.0,1.0),
    ColorF(1.0,0.0,0.0,1.0),
    ColorF(0.0,0.0,0.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};


const static float COLORGRADIENT_DEFEAULT_VEL_WHITE_STOPS[LISEM_GRADIENT_NCOLORS] =
{0.0,1.0,2.0,0.6,0.8,1.0,2.0,0.0,0.0,0.0};

const static ColorF COLORGRADIENT_DEFEAULT_VEL_WHITE_COLORS[LISEM_GRADIENT_NCOLORS] =
{
    ColorF(1.0,1.0,1.0,0.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0),
    ColorF(1.0,1.0,1.0,1.0)

};





#define LISEM_GRADIENT_NUM 13

#define LISEM_GRADIENT_WH 0
#define LISEM_GRADIENT_HS 1
#define LISEM_GRADIENT_UI 12 //Marianne Rainbow
#define LISEM_GRADIENT_RYG 3
#define LISEM_GRADIENT_ELEVATION 4
#define LISEM_GRADIENT_RBGB 5
#define LISEM_GRADIENT_BLUES 6
#define LISEM_GRADIENT_REDS 7
#define LISEM_GRADIENT_MAGMA 8
#define LISEM_GRADIENT_WH_FLUIDS 9
#define LISEM_GRADIENT_WH_SOLIDS 10
#define LISEM_GRADIENT_VEL_WHITE 11
#define LISEM_GRADIENT_UI_USER 2

//#include "generalsettingsmanager.h"


inline static LSMColorGradient GetDefaultGradient(int g)
{

    LSMColorGradient grad;

    if(g == LISEM_GRADIENT_WH)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_WH_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_WH_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_HS)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_GRAYSCALE_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_GRAYSCALE_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_UI)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_RYG)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_RYG_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_RYG_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_ELEVATION)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_ELEVATION_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_ELEVATION_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_RBGB)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_RBGB_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_RBGB_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_BLUES)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_BLUES_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_BLUES_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_REDS)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_REDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_REDS_COLORS[i]);
        }
    }else if(g == LISEM_GRADIENT_MAGMA)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_MAGMA_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_MAGMA_COLORS[i]);
        }

    }else if(g == LISEM_GRADIENT_WH_FLUIDS)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_FLUIDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_FLUIDS_COLORS[i]);
        }

    }else if(g == LISEM_GRADIENT_WH_SOLIDS)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_SOLIDS_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_SOLIDS_COLORS[i]);
        }

    }else if(g == LISEM_GRADIENT_VEL_WHITE)
    {
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_VEL_WHITE_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_VEL_WHITE_COLORS[i]);
        }

    }else if(g == LISEM_GRADIENT_UI_USER)
    {

        //get gradient from general setting manager
        //bool ok;
        //QString data = GetSettingsManager()->GetSetting("USERGRADIENT",&ok); // assume this holds the json string

        //if(!ok )
        {
            for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
            {
                grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
                grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
            }
        }/*else
        {
            QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
            QJsonObject obj;
            if(!doc.isNull())
            {
                if(doc.isObject())
                {
                    obj = doc.object();
                    grad.Restore(&obj);
                }
                else
                {
                    for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
                    {
                        grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
                        grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
                    }
                }
            }
            else
            {
                for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
                {
                    grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
                    grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
                }
            }
        }*/


    }else {//default grayscale
        for(int i = 0; i < LISEM_GRADIENT_NCOLORS; i++)
        {
            grad.m_Gradient_Stops.append(COLORGRADIENT_DEFEAULT_UI_STOPS[i]);
            grad.m_Gradient_Colors.append(COLORGRADIENT_DEFEAULT_UI_COLORS[i]);
        }
    }

    return grad;

}

inline static LSMColorGradient GetDefaultGradientFromName(QString name)
{

    if(name.compare("Water",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_WH);
    }
    if(name.compare("Hillshade",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_HS);
    }
    if(name.compare("Marianne",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_UI);
    }
    if(name.compare("RYG",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_RYG);
    }
    if(name.compare("Elevation",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_ELEVATION);
    }
    if(name.compare("RBGB",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_RBGB);
    }
    if(name.compare("Blues",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_BLUES);
    }
    if(name.compare("Reds",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_REDS);
    }
    if(name.compare("Magma",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_MAGMA);
    }
    if(name.compare("Fluids",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_WH_FLUIDS);
    }
    if(name.compare("Solids",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_WH_SOLIDS);
    }
    if(name.compare("White",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_VEL_WHITE);
    }
    if(name.compare("User",Qt::CaseInsensitive)==0)
    {
        return GetDefaultGradient(LISEM_GRADIENT_UI_USER);
    }

    return GetDefaultGradient(LISEM_GRADIENT_UI);

}


#endif // LSMCOLORGRADIENT_H
