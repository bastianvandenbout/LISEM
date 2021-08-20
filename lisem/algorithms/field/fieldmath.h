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


inline Field * AS_FieldXCoord(Field * Other)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = Other->GetWest() + Other->cellSizeX() * (float(c));
                }
            }
        }
    }

    return nmap;

}

inline Field * AS_FieldYCoord(Field * Other)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = Other->GetNorth() + Other->cellSizeY() * (float(r));
                }
            }
        }
    }

    return nmap;

}

inline Field * AS_FieldZCoord(Field * Other)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = Other->GetBottom() + Other->cellSizeZ() * (float(i));
                }
            }
        }
    }

    return nmap;

}


inline Field * AS_FieldIf(Field * Other, Field * iftrue, Field * iffalse)
{

    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue->at(i)->data[r][c]:iffalse->at(i)->data[r][c];
                }
            }
        }
    }

    return nmap;

}

inline Field * AS_FieldIf(Field * Other, float iftrue, Field * iffalse)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue:iffalse->at(i)->data[r][c];
                }
            }
        }
    }

    return nmap;
}

inline Field * AS_FieldIf(Field * Other, Field * iftrue, float iffalse)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue->at(i)->data[r][c]:iffalse;
                }
            }
        }
    }

    return nmap;
}

inline Field * AS_FieldIf(Field * Other, float iftrue, float iffalse)
{
    Field *nmap;
    nmap = Other->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue:iffalse;
                }
            }
        }
    }

    return nmap;
}


inline Field * AS_FieldIf(float Other, Field * iftrue, Field * iffalse)
{
    Field *nmap;
    nmap = iftrue->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < iftrue->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < iftrue->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < iftrue->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(iftrue->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other)? iftrue->at(i)->data[r][c]:iffalse->at(i)->data[r][c];
                }
            }
        }
    }

    return nmap;
}

inline Field * AS_FieldIf(float Other, float iftrue, Field * iffalse)
{
    Field *nmap;
    nmap = iffalse->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < iffalse->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < iffalse->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < iffalse->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(iffalse->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other)? iftrue:iffalse->at(i)->data[r][c];
                }
            }
        }
    }

    return nmap;
}

inline Field * AS_FieldIf(float Other, Field * iftrue, float iffalse)
{
    Field *nmap;
    nmap = iftrue->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < iftrue->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < iftrue->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < iftrue->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(iftrue->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other)? iftrue->at(i)->data[r][c]:iffalse;
                }
            }
        }
    }

    return nmap;
}

inline Field * AS_FieldIf(Field* Other, Field * iftrue)
{
    Field *nmap;
    nmap = iftrue->GetCopy();

    float mv = 0.0;
    pcr::setMV(mv);

    //do actual calculation loop
    for(int i = 0; i < iftrue->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < iftrue->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < iftrue->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(iftrue->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue->at(i)->data[r][c]:mv;
                }
            }
        }
    }

    return nmap;
}


inline Field * AS_FieldIf(float Other, Field * iftrue)
{
    Field *nmap;
    nmap = iftrue->GetCopy();

    float mv = 0.0;
    pcr::setMV(mv);

    //do actual calculation loop
    for(int i = 0; i < iftrue->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < iftrue->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < iftrue->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(iftrue->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other)? iftrue->at(i)->data[r][c]:mv;
                }
            }
        }
    }

    return nmap;
}


inline Field * AS_FieldIf(Field * Other, float iftrue)
{
    Field *nmap;
    nmap = Other->GetCopy();

    float mv = 0.0;
    pcr::setMV(mv);

    //do actual calculation loop
    for(int i = 0; i < Other->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < Other->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < Other->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(Other->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] =LISEM_ASMAP_BOOLFROMFLOAT(Other->at(i)->data[r][c])? iftrue:mv;
                }
            }
        }
    }

    return nmap;
}




#endif // FIELDMATH_H
