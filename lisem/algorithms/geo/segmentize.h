#ifndef SEGMENTIZE_H
#define SEGMENTIZE_H

#include "vectorwarp.h"

inline static ShapeFile * AS_Segmentize(ShapeFile * in_s, float dist_max)
{
    return VectorWarp(in_s,(in_s->GetProjection()),true,dist_max);
}


#endif // SEGMENTIZE_H
