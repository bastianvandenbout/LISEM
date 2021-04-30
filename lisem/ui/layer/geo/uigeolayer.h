#pragma once
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

////
/// \brief The UIGeoLayer class
///
/// This subclass of UILayer provides geo-referencing,
/// and deals with having a projection, a CRS based bounding
/// box and other CRS related stuff
///
/// @see UILayer
///
class UIGeoLayer : public UILayer,UIDistanceEstimator
{
private:


protected:


    GeoCoordTransformer * m_Transformer = nullptr;
    GeoCoordTransformer * m_Transformerinv = nullptr;


    GeoProjection m_Projection = GeoProjection();
    BoundingBox m_BoundingBox = BoundingBox();
    BoundingBox3D m_BoundingBox3D = BoundingBox3D();

    OpenGLGeometry * m_LineGeometry;
    OpenGLGeometry * m_PointInnerGeometry;
    OpenGLGeometry * m_PointOuterGeometry;
    OpenGLGeometry * m_PolygonInnerGeometry;
    OpenGLGeometry * m_PolygonOuterGeometry;

public:

    inline UIGeoLayer()
    {
        m_IsGeo = true;
    }
    inline UIGeoLayer(GeoProjection p,BoundingBox b, QString name, bool file = false, QString filepath = "", bool dynamic = false) : UILayer(name, file,filepath,dynamic)
    {
        m_IsGeo = true;
        Initialize(p,b,name,file,filepath,dynamic);
    }

    inline void Initialize(GeoProjection p,BoundingBox b, QString name, bool file = false, QString filepath = "", bool dynamic = false)
    {
        UILayer::Initialize(name,file,filepath,dynamic);

        m_Projection = p;
        m_BoundingBox = b;

        if(m_Transformer != nullptr)
        {
            delete m_Transformer;
        }
        if(m_Transformerinv != nullptr)
        {
            delete m_Transformerinv;
        }
        m_Transformer = GeoCoordTransformer::GetCoordTransformerGeneric();
        m_Transformerinv = GeoCoordTransformer::GetCoordTransformerGeneric();


    }
    inline ~UIGeoLayer()
    {

    }
    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        m_IsPrepared = true;
    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
    }

    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {

    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        if(m_Transformer != nullptr)
        {
            delete m_Transformer;
        }
        if(m_Transformerinv != nullptr)
        {
            delete m_Transformerinv;
        }
        m_Transformer = GeoCoordTransformer::GetCoordTransformer(s.projection,m_Projection);
        m_Transformerinv = GeoCoordTransformer::GetCoordTransformer(m_Projection,s.projection);


        m_IsCRSChanged = false;
    }

    inline LSMVector3 GetDistance(LSMVector3 p)
    {
        //by defeault, get distance to bounding box

        return LSMVector3();
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline void  SetProjection(GeoProjection p)
    {
        m_Projection = p;
        m_IsCRSChanged = true;
    }
    inline GeoProjection GetProjection()
    {
        return m_Projection;
    }
    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox3D GetBoundingBox3D()
    {
        return m_BoundingBox3D;
    }


    inline BoundingBox GetGeoExtentInProjection(GeoProjection * p)
    {

        return BoundingBox();
    }

    //the camera must be able to request special ele
    inline virtual bool GetDetailedElevationAtPosition(LSMVector2 pos,  GeoProjection proj, float & val)
    {
        return false;
    }

    inline LayerInfo GetInfo() override
    {
        LayerInfo l;
        l.Add("CRS EPSG",QString::number(m_Projection.GetEPSG()));
        l.Add("CRS Name",m_Projection.GetName());
        l.Add("CRS WKT",m_Projection.GetWKT());
        l.Add(UILayer::GetInfo());
        return l;
    }

};

