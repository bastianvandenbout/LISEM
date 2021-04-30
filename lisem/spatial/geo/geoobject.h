#ifndef GEOOBJECT_H
#define GEOOBJECT_H

#include "QString"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "boundingbox.h"
#include <iostream>
#include "geo/geoprojection.h"
////
/// \brief The GeoObject class
///
///
///
///


class GeoObject
{
protected:
    GeoProjection m_Projection;
    BoundingBox m_BoundingBox;

public:

    inline GeoObject()
    {


    }


    inline GeoProjection GetProjection()
    {
        return m_Projection;
    }

    inline void SetProjection(GeoProjection proj)
    {
        m_Projection = proj;

    }

    inline void SetProjection(const OGRSpatialReference * ref)
    {
        m_Projection = GeoProjection::FromGDALRef(ref);

    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }


};

#endif // GEOOBJECT_H
