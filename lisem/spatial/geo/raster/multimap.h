#ifndef MULTIMAP_H
#define MULTIMAP_H

#include "geo/geoobject.h"
#include "geo/raster/map.h"

class cTMultiMap : public GeoObject
{

    //! The actual rasters
    //! At all times, the rasters are owned by this object and can be deleted.
    //! do not use pointers to the internal data, instead, get a copy of the raster
    QList<cTMap *> m_BandMaps;

public:

    cTMultiMap               ()=default;

    cTMultiMap               (QList<cTMap *> maps);

    cTMultiMap              (cTMultiMap const& other)=delete;

    cTMultiMap               (cTMultiMap&& other)=default;

    ~cTMultiMap              ()=default;

    cTMultiMap&         operator=           (cTMultiMap const& other)=delete;

    cTMultiMap&         operator=           (cTMultiMap&& other)=default;

    cTMap *             GetBand(int n);
    int                 GetNumberOfBands();



    //Angelscript related functions
    public:

        QString        AS_FileName          = "";
        bool           AS_writeonassign     = false;
        std::function<void(cTMultiMap *,QString)> AS_writefunc;
        std::function<cTMultiMap *(QString)> AS_readfunc;
        bool           AS_Created           = false;
        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        cTMultiMap*    AS_Assign            (cTMultiMap*);

};

//Angelscript related functionality

inline void cTMultiMap::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void cTMultiMap::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        delete this;
    }

}

inline cTMultiMap* cTMultiMap::AS_Assign(cTMultiMap* sh)
{
    if(AS_writeonassign)
    {
        AS_writefunc(sh,AS_FileName);
    }
    this->AS_writeonassign = false;

    for(int i = 0; i < m_BandMaps.length(); i++)
    {
        if(m_BandMaps.at(i) != nullptr)
        {
            delete m_BandMaps.at(i);
        }
    }
    m_BandMaps.clear();

    for(int i = 0; i < sh->GetNumberOfBands(); i++)
    {
        m_BandMaps.append(sh->GetBand(i)->GetCopy());
    }

    return this;
}

inline cTMultiMap* MultiMapFactory()
{
    return new cTMultiMap();

}

inline cTMultiMap::cTMultiMap(QList<cTMap *> maps)
{
    m_BandMaps = maps;

    if(m_BandMaps.length() > 0)
    {
        SetProjection(m_BandMaps.at(0)->GetProjection());
    }
}

inline int cTMultiMap::GetNumberOfBands()
{
    return m_BandMaps.length();
}
inline cTMap * cTMultiMap::GetBand(int n)
{
    return m_BandMaps.at(n);

}

#endif // MULTIMAP_H
