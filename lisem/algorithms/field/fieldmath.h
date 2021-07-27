#ifndef FIELDMATH_H
#define FIELDMATH_H

#include <algorithm>
#include <QList>
#include "geo/raster/field.h"
#include "geo/raster/fieldalgebra.h"
#include "raster/rasterlimit.h"
//abs
//min
//max
//exp
//pow
//sqrt
//log
//log10
//ceil
//floor
//fraction
//cover
//setmv
//IsMV
//XCoord, YCoord, ZCoord
//XIndex, YIndex, ZIndex
//FieldIf
//GradientX, GradientY, GradientZ
//GradientXX, GradientYY, GradientZZ
//GradientXY, Gradient XZ, Gradient YZ
//GradientYX, Gradient ZX, Gradient ZY


inline Field * AS_Fieldabs(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return fabsf(x);});
}


inline Field * AS_Fieldmax(Field * Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fmaxf(x,y);});
}


inline Field * AS_Fieldmin(Field * Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fminf(x,y);});
}

inline Field * AS_Fieldmax1(float Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fmaxf(x,y);});
}
inline Field * AS_Fieldmax2( Field * Other, float Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fmaxf(x,y);});
}
inline Field * AS_Fieldmin1(float Other, Field* Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fminf(x,y);});
}

inline Field * AS_Fieldmin2(Field * Other, float Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return fminf(x,y);});
}


inline Field * AS_Fieldexp(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return expf(x);});
}

inline Field * AS_Fieldpow(Field * Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return powf(x,y);});
}
inline Field * AS_Fieldpow2(Field * Other, float Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return powf(x,y);});
}
inline Field * AS_Fieldpow1(float Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x,float y){return powf(x,y);});
}

inline Field * AS_Fieldsqrt(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return sqrtf(x);});
}

inline Field * AS_Fieldlog(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return logf(x);});
}


inline Field * AS_Fieldlog10(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return log10f(x);});
}

inline Field * AS_Fieldceil(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return std::ceil(x);});
}


inline Field * AS_Fieldfloor(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return std::floor(x);});
}

inline Field * AS_Fieldfraction(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return AS_Temp_fractionf(x);});
}

inline Field * AS_Fieldcover(Field * Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x, float y){return pcr::isMV(x)?y:x;});
}
inline Field * AS_Fieldcover(float Other, Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x, float y){return pcr::isMV(x)?y:x;});
}
inline Field * AS_Fieldcover(Field * Other, float Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x, float y){return pcr::isMV(x)?y:x;});
}


inline Field * AS_Fieldsetmv(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float y){float x; pcr::setMV(x); return x;});
}

inline Field * AS_Fieldismv(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return pcr::isMV(x)?1.0:0.0;});
}



/*

inline Field * AS_FieldIf(Field * Other, Field * iftrue, Field * iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_FieldIf(Field * Other, float iftrue, Field * iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_FieldIf(Field * Other, Field * iftrue, float iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_FieldIf(float Other, Field * iftrue, Field * iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_FieldIf(float Other, float iftrue, Field * iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_FieldIf(float Other, Field * iftrue, float iffalse)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}*/

#endif // FIELDMATH_H
