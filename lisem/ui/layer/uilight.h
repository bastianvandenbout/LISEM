#pragma once

#include "styles/sphstyle.h"
#include "openglclmanager.h"
#include "glplot/worldgltransforms.h"
#include "geowindowstate.h"
#include "matrixtable.h"
#include "QMutex"
#include "layer/uilayereditor.h"
#include "layerparameters.h"
#include "geo/shapes/shapefile.h"
#include "boundingbox.h"
#include "linear/lsm_vector3.h"
#include "openglcltexture.h"

class UILight
{

protected:

    LSMVector3 m_Color;
    LSMVector3 m_Position;
    float m_Intensity = 0.0;
    bool m_Active = false;
    bool m_Universal = true;
    bool m_RequiresShadowMaps = true;
    bool m_IsPrepared= false;
    bool m_DoShadowMapping = true;


    int m_ShadowMapSizeX = 2048;
    int m_ShadowMapSizeY = 2048;
public:

    inline UILight()
    {

    }

    inline ~UILight()
    {


    }

    virtual inline bool IsUniversal()
    {
        return m_Universal;
    }

    virtual inline LSMVector3 GetLightColor()
    {
        return m_Color;
    }

    virtual inline LSMVector3 GetLightPosition()
    {
        return m_Position;
    }
    virtual inline bool IsActive()
    {
        return m_Active;
    }
    inline void SetActive(bool x)
    {
        m_Active = x;
    }

    inline void SetDoShadowMapping(bool x)
    {
        m_DoShadowMapping = x;
    }
    virtual inline bool RequiresShadowMaps()
    {
        return m_RequiresShadowMaps;
    }

    virtual inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s)
    {


    }

    virtual inline bool IsPrepared()
    {
        return m_IsPrepared;
    }

    virtual inline void OnDrawShadowMaps(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, std::vector<UILayer *> shadowcasters)
    {


    }

    virtual inline void OnRenderLightBuffer(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, OpenGLCLMSAARenderTarget * target, int bufferindex)
    {


    }
};
