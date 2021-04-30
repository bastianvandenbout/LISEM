#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include "vectorwarp.h"

inline static ShapeFile * AS_Simplify(ShapeFile * in_s, float dist_threshold)
{
    return VectorWarp(in_s,(in_s->GetProjection()),false,0.0,true,dist_threshold);
}


#endif // SIMPLIFY_H
