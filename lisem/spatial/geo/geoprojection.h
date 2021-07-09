#ifndef GEOPROJECTION_H
#define GEOPROJECTION_H

#include "QString"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "boundingbox.h"
#include <iostream>
#include "error.h"


////
/// \brief The GeoProjection class
///
/// a class containing Coordinate Reference System and Datum
/// as defined through the GDAL OGRSPATIALREFERENCE class
/// It can also be a special type, Generic, which is a unit-less
/// coordinate system that transforms every coordinate point to
/// its original value (thus displaying maps simply based on their
/// coordinates and ignoring the CRS
/// Can be set form EPSG values
class GeoProjection
{
private:
    OGRSpatialReference m_GDALSpatialReference;
    QString m_ProjectionString;

    bool m_IsValid = true;
    bool m_IsGeneric = false;
    bool m_IsProjected = false;
    int m_EPSG = 0;

public:
    inline OGRSpatialReference GetOGRRef()
    {
        return m_GDALSpatialReference;
    }
    inline OGRSpatialReference *GetOGRRefP()
    {
        return &m_GDALSpatialReference;
    }


    inline void SetGeneric()
    {
        m_IsGeneric = true;
        m_IsValid = true;
    }
    inline void SetInValid()
    {
        m_IsValid = false;
    }
    inline bool IsGeneric()
    {
        return m_IsGeneric;
    }
    inline bool IsValid()
    {
        return m_IsValid;
    }

    inline bool IsProjected()
    {
        if(m_IsGeneric)
        {
            return true;
        }else {
            return m_GDALSpatialReference.IsProjected();
        }
    }

    //get height multiplier
    inline double GetUnitZMultiplier()
    {

        if(m_IsGeneric)
        {
            return 1.0;
        }else {



            if(IsProjected())
            {
                return m_GDALSpatialReference.GetLinearUnits(nullptr);
            }else {
                double zmult = 1.0/((6371000.0)* (m_GDALSpatialReference.GetAngularUnits(nullptr)));
                return zmult;
            }
        }
        return 1.0;

    }

    //get multiplier to convert to meter or radians (depending on wether it is projected)
    inline double GetUnitMultiplier()
    {

        if(m_IsGeneric)
        {
            return 1.0;
        }else {



            if(IsProjected())
            {
                return m_GDALSpatialReference.GetLinearUnits(nullptr);
            }else {
                return m_GDALSpatialReference.GetAngularUnits(nullptr);
            }
        }
        return 1.0;

    }

    inline QString GetUnitName()
    {

        if(m_IsGeneric)
        {
            return "(-)";
        }else {

            const char * name1;
            const char * name2;

            if(IsProjected())
            {
                m_GDALSpatialReference.GetLinearUnits(&name1);
                return QString(name1);
            }else {
                m_GDALSpatialReference.GetAngularUnits(&name2);
                return QString(name2);
            }
        }
    }

    inline int GetEPSG()
    {
        if(m_IsGeneric)
        {
            return -1;
        }else if( !m_GDALSpatialReference.IsEmpty())
        {
             return m_GDALSpatialReference.GetEPSGGeogCS();

            /*m_GDALSpatialReference.AutoIdentifyEPSG();

            int nPCS = -1;
            if( m_GDALSpatialReference.GetAuthorityName("**PROJCS") != nullptr
            && EQUAL(m_GDALSpatialReference.GetAuthorityName("**PROJCS"),"EPSG") )
            {
            nPCS = atoi(m_GDALSpatialReference.GetAuthorityCode("**PROJCS"));
            }
            return nPCS;*/
        }else {
            return -1;
        }

    }
    inline bool IsEqualTo(GeoProjection p)
    {
        if((!p.m_IsValid || !m_IsValid) && !(!p.m_IsValid && !m_IsValid))
        {
            return false;
        }else if(m_IsGeneric && p.m_IsGeneric)
        {
            return true;
        }else if(m_GDALSpatialReference.IsSame(&(p.m_GDALSpatialReference)))
        {
            return true;
        }else
        {
            return false;
        }
    }
    inline QString GetName()
    {
        if(m_IsGeneric)
        {
            return "LISEM_GENERIC";
        }else if( !m_GDALSpatialReference.IsEmpty())
        {
            return QString(m_GDALSpatialReference.GetName());
        }else {
            return "";
        }
    }
    inline QString GetWKT()
    {
        if(m_IsGeneric)
        {
            return "LISEM_GENERIC";
        }else if( !m_GDALSpatialReference.IsEmpty())
        {
            char * refc;
            m_GDALSpatialReference.exportToWkt(&refc);
            return QString(refc);
        }else {
            return "";
        }
    }
    static inline GeoProjection FromString(QString refc)
    {
        GeoProjection gp;

        if(!refc.isEmpty())
        {
            OGRSpatialReference * ref = new OGRSpatialReference();
            ref->importFromWkt(refc.toStdString().c_str());
            gp = FromGDALRef(ref);

            delete ref;
        }else {
            gp.m_IsGeneric = true;
            gp.m_IsValid = true;
        }

        return gp;
    }

    static inline GeoProjection FromGDALRef(const OGRSpatialReference ref)
    {
        char * refc;
        ref.exportToWkt(&refc);


        GeoProjection gp;
        gp.m_GDALSpatialReference = ref;
        gp.m_GDALSpatialReference.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        gp.m_ProjectionString = QString(refc);
        gp.m_EPSG = ref.GetEPSGGeogCS();
        gp.m_IsProjected = ref.IsProjected();
        gp.m_IsValid = true;
        gp.m_IsGeneric = false;
        return gp;

    }
    static inline GeoProjection FromGDALRef(const OGRSpatialReference * ref)
    {
        if(ref != nullptr)
        {
            char * refc;
            ref->exportToWkt(&refc);

            GeoProjection gp;
            gp.m_GDALSpatialReference = *ref;
            gp.m_GDALSpatialReference.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
            gp.m_ProjectionString = QString(refc);
            gp.m_EPSG = ref->GetEPSGGeogCS();
            gp.m_IsProjected = ref->IsProjected();
            gp.m_IsValid = true;
            gp.m_IsGeneric = false;
            return gp;


        }
        GeoProjection gp;
        gp.m_IsGeneric = true;
        gp.m_IsValid = true;
        return gp;

    }

    static inline GeoProjection GetGeneric()
    {
        GeoProjection gp;
        gp.m_IsGeneric = true;
        gp.m_IsValid = true;
        return gp;
    }
    inline OGRSpatialReference ToGDALRef()
    {
        OGRSpatialReference oSRS;

        if(!m_IsGeneric)
        {
            oSRS.importFromWkt(this->GetWKT().toStdString().c_str());
        }else
        {

        }

        return oSRS;
    }

    static inline GeoProjection GetFromEPSG(int code)
    {
        OGRSpatialReference oSRS;
        if(oSRS.importFromEPSG(code) == OGRERR_NONE)
        {
            return FromGDALRef(oSRS);
        }
        else
        {
            GeoProjection gp;
            gp.m_IsValid = false;
            gp.m_IsGeneric = true;
            return gp;
        }
    }

    static inline GeoProjection GetFromWKT(QString wkt)
    {
        OGRSpatialReference oSRS;
        if(oSRS.importFromWkt(wkt.toStdString().c_str()) == OGRERR_NONE)
        {
            return FromGDALRef(oSRS);
        }
        else
        {
            GeoProjection gp;
            gp.m_IsValid = false;
            gp.m_IsGeneric = true;
            return gp;
        }
    }

    static inline GeoProjection GetFromWGS84UTM(int zone, bool north)
    {
        OGRSpatialReference oSRS;

        int err1 = oSRS.SetProjCS( QString(QString("UTM ") + QString::number(zone) + (north?"N":"S") + " WGS84 in northern hemisphere.").toStdString().c_str() );
        int err2 = oSRS.SetWellKnownGeogCS( "WGS84" );
        int err3 = oSRS.SetUTM( zone, north? TRUE:FALSE );

        if(err1 == OGRERR_NONE && err2 == OGRERR_NONE && err3 == OGRERR_NONE)
        {
            return FromGDALRef(oSRS);
        }
        else
        {
            GeoProjection gp;
            gp.m_IsValid = false;
            gp.m_IsGeneric = true;
            return gp;
        }
    }

//Angelscript related functions
public:

    int            m_refcount          = 1;
    void           AddRef            ();
    void           ReleaseRef        ();
    GeoProjection* Assign            (GeoProjection*);

};

//Angelscript related functionality

inline void GeoProjection::AddRef()
{
    m_refcount = m_refcount + 1;

}

inline void GeoProjection::ReleaseRef()
{
    m_refcount = m_refcount - 1;
    if(m_refcount == 0)
    {
        delete this;
    }

}

inline GeoProjection* GeoProjection::Assign(GeoProjection* sh)
{
    *this = *sh;
    return this;
}

inline GeoProjection* GeoProjectionFactory()
{
    return new GeoProjection();

}


inline GeoProjection* GetGeoProjectionFromEPSG(int epsg)
{
    GeoProjection * ret = new GeoProjection(GeoProjection::GetFromEPSG(epsg));
    if(!(ret->IsValid()))
    {
        delete ret;
        ret = new GeoProjection();
    }
    if(ret->IsGeneric())
    {
        LISEMS_STATUS("Could not get CRS for epsg code: " + QString::number(epsg));
    }
    return ret;
}
inline GeoProjection* GetGeoProjectionFromWKT(std::string wkt)
{
    GeoProjection * ret = new GeoProjection(GeoProjection::GetFromWKT(QString(wkt.c_str())));
    if(!(ret->IsValid()))
    {
        delete ret;
        ret = new GeoProjection();
    }
    if(ret->IsGeneric())
    {
        LISEMS_STATUS("Could not get CRS for wkt code: " + QString(wkt.c_str()));
    }
    return ret;
}
inline GeoProjection* GetGeoProjectionFromWGS84UTM(int zone, bool north)
{
    GeoProjection * ret = new GeoProjection(GeoProjection::GetFromWGS84UTM(zone,north));
    if(!(ret->IsValid()))
    {
        delete ret;
        ret = new GeoProjection();
    }
    if(ret->IsGeneric())
    {
        LISEMS_STATUS("Could not get CRS for utm zone: " + QString::number(zone) + (north? "n" : "s") );
    }
    return ret;
}






#endif // GEOPROJECTION_H
