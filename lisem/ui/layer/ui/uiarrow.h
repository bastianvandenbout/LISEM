#ifndef UIARROW_H
#define UIARROW_H


#include "layer/uilayer.h"
#include "openglclmanager.h"
#include "qcolor.h"
#include "model.h"
#include "geo/raster/map.h"
#include "openglcltexture.h"
#include "openglprogram.h"
#include "lsmio.h"
#include "lisemmath.h"
#include "QObject"
#include "openglcldatabuffer.h"
#include "color/colorf.h"
#include "boundingbox.h"
#include "linear/lsm_vector3.h"
#include "time.h"
#include "gl/openglcldatamanager.h"


class UIArrowLayer : public UIGeoLayer
{
private:


public:

    inline UIArrowLayer(LSMVector3 posA, LSMVector3 posB, LSMVector4 color = LSMVector4(0.0,0.0,0.0,1.0),GeoProjection p = GeoProjection::GetGeneric())
    {

    }




};



#endif // UIARROW_H
