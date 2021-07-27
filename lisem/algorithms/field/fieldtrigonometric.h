#ifndef FIELDTRIGONOMETRIC_H
#define FIELDTRIGONOMETRIC_H



#include <algorithm>
#include <QList>
#include "geo/raster/field.h"
#include "geo/raster/fieldalgebra.h"



inline Field * AS_Fieldsin(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinf(x);});
}

inline Field * AS_Fieldcos(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return cosf(x);});
}

inline Field * AS_Fieldtan(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return tanf(x);});
}
inline Field * AS_Fieldasin(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return asinf(x);});

}

inline Field * AS_Fieldacos(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return acosf(x);});
}

inline Field * AS_Fieldatan(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return atanf(x);});

}

inline Field * AS_Fieldatan2(Field * Other,Field * Other2)
{
    return ApplyOperatorToFields(Other,Other2,[](float x1,float x2){return atan2f(x1,x2);},false);

}

inline Field * AS_Fieldsinh(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return sinhf(x);});
}


inline Field * AS_Fieldcosh(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return coshf(x);});


}

inline Field * AS_Fieldtanh(Field * Other)
{
    return ApplySingleFunctionToField(Other,[](float x){return tanhf(x);});


}



#endif // FIELDTRIGONOMETRIC_H
