#ifndef UISHADERLAYER_H
#define UISHADERLAYER_H



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
#include "layer/uilayer.h"
#include "boundingbox.h"
#include "linear/lsm_vector3.h"


class UIShaderLayer : public UIGeoLayer
{
private:

    QString m_ShaderText;
    std::vector<QString> m_TextureLinks;


public:



}

#endif // UISHADERLAYER_H
