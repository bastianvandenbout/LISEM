/*************************************************************************
**  openLISEM: a spatial surface water balance and soil erosion model
**  Copyright (C) 2010,2011  Victor Jetten
**  contact:
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  Author: Victor Jetten
**  Developed in: MingW/Qt/
**  website, information and code: http://lisem.sourceforge.net
**
*************************************************************************/
#ifndef CsfMapH
#define CsfMapH
#include <QString>
#include "masked_raster.h"
#include <functional>
#include <iostream>
#include "error.h"
#include "geo/geoobject.h"
#include "linear/lsm_vector4.h"
#include "lisemmath.h"
#include "defines.h"

#define FOR_ROW_COL_MV(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\
    if(!pcr::isMV(map->data[r][c]))

#define FOR_ROW_COL(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\

#define MAPMATH_PI 3.14159f

#define Drc data[r][c]

#define LISEM_ASMAP_BOOLFROMFLOAT(x) (x>0.5f?true:false)

#define SWAPMAP(x, y) do { cTMap * SWAP = x; x = y; y = SWAP; } while (0)

typedef struct RasterBandStats
{
    double min = 0.0;
    double max = 0.0;
    double stdev = 0.0;
    double mean = 0.0;
    int band = 0;
    int n_mv = 0;
} RasterBandStats;


typedef struct cTMapProps
{
    bool can_be_read = false;
    QString file_path = "";
    int sizex = 0.0;
    int sizey = 0.0;
    int bands = 0;
    int band = 0;
    bool is_ldd = false;
    double cellsizex = 0.0;
    double cellsizey = 0.0;

    QString projection;

    double uly = 0.0;
    double ulx = 0.0;

    inline bool equals(cTMapProps p)
    {
        if(p.can_be_read != can_be_read)
        {
            return false;
        }
        if(sizex != p.sizex)
        {
            return false;
        }
        if(sizey != p.sizey)
        {
            return false;
        }
        if(bands != bands)
        {
            return false;
        }
        if(is_ldd != p.is_ldd)
        {
            return false;
        }
        if(cellsizex != p.cellsizex)
        {
            return false;
        }
        if(cellsizey != p.cellsizey)
        {
            return false;
        }
        if(projection != p.projection)
        {
            return false;
        }
        if(ulx != p.ulx)
        {
            return false;
        }
        if(uly != p.uly)
        {
            return false;
        }
        return true;
    }

} cTMapProps;

/*!
    @brief      A cTMap contains all relevant information about a raster.
    @todo       The data member must be made private.

    cTMap instances contain raster data, projection information and a map name.
    I/O of cTMap instances is handles by functions defined in the io module.
*/
class LISEM_API cTMap : public GeoObject
{

public:

    //! The actual raster.
    MaskedRaster<float> data;

                   cTMap               ()=default;

                   cTMap               (MaskedRaster<float>&& data,
                                        QString const& projection,
                                        QString const& mapName,
                                        bool is_ldd = false);

                   cTMap               (cTMap const& other)=default;

                   cTMap               (cTMap&& other)=default;

                   ~cTMap              ()=default;

    cTMap&         operator=           (cTMap const& other)=delete;

    cTMap&         operator=           (cTMap&& other)=default;

    int            nrRows              () const;

    int            nrCols              () const;

    double         north               () const;
    double         west                () const;
    double         cellSize            () const;
    double         cellSizeX            () const;
    double         cellSizeY            () const;

    void           setnorth            (double);
    void           setwest             (double);
    void           setcellSizeX        (double);
    void           setcellSizeY        (double);
    void           setprojectionstring (QString p);

    float          val_min             ();
    float          val_max             ();

    QString const& projection          () const;

    QString const& mapName             () const;

    void           setAllMV            ();

    void           MakeMap             (cTMap *dup,
                                        REAL4 value);

    float          GetPixelCovered      (int r, int c, float cover = 0.0);
    float          Sample              (double x, double y);
    float          SampleLinear              (double x, double y);
    float          SampleCubic              (double x, double y);
    cTMap*         GetCopy             () const;
    cTMap*         GetCopy0             () const;
    cTMap*         GetCopyMask          () const;

    cTMapProps     GetRasterProps      ();
    RasterBandStats GetRasterBandStats  (bool stdev);

    //functions related to the Angelscript Scripting Access for maps
    //NOTE: maps can have a single numerical value, as indicated by member AS_IsSingleValue
    //the value itself is contained in the 1x1 raster data accasable by map->data[0][0]
public:

    // meta-data
    std::vector<QString> m_MeteDataDomains;
    std::vector<std::vector<QString>> m_MetaData;
    std::vector<QString> m_MOFlags;
    // ground control points
    std::vector<LSMVector4> m_ControlPoints;



    QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;


    std::function<void(cTMap *,QString)> AS_writefunc;
    std::function<void(QList<cTMap *>,QString)> AS_writefunclist;
    std::function<cTMap *(QString)> AS_readfunc;
    std::function<cTMap *(QString,int)> AS_readbandfunc;
    std::function<cTMap *(QString,int)> AS_readbandabsfunc;
    std::function<QList<cTMap *>(QString)> AS_readallfunc;
    std::function<void(cTMap *,QString,int)> AS_writebandfunc;

    bool           AS_Created           = false;
    bool           AS_IsSingleValue     = false;
    bool           AS_IsLDD             = false;

    bool           AS_IsBandLoadable    = false;
    int            AS_TotalNBands       = 1;
    QString        AS_BandFilePath      = "";
    bool           AS_IsBand            = false;
    int            AS_Bandn             = 0;

    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    float&         Value             (int r, int c);
    int            Rows              ();
    int            Cols              ();
    float          North             ();
    float          West              ();
    float          CellSize          ();

    //get another band from the original file;
    cTMap *        GetBand           (int i);
    //cTMap *        AS_AssignArray       (CScriptArray * array);
    //CScriptArray * AS_GetAll            ();




    cTMap *        Assign            (cTMap *other);
    cTMap *        Assign            (float other);

    cTMap *        OpAdd             (cTMap *other);
    cTMap *        OpMul             (cTMap *other);

    cTMap *        OpNeg             ();
    cTMap *        OpSub             (cTMap *other);
    cTMap *        OpDiv             (cTMap *other);
    cTMap *        OpPow             (cTMap *other);
    cTMap *        OpAddAssign       (cTMap *other);
    cTMap *        OpSubAssign       (cTMap *other);
    cTMap *        OpMulAssign       (cTMap *other);
    cTMap *        OpDivAssign       (cTMap *other);
    cTMap *        OpPowAssign       (cTMap *other);

    cTMap *        OpAdd             (float other);
    cTMap *        OpMul             (float other);

    cTMap *        OpSub             (float other);
    cTMap *        OpDiv             (float other);
    cTMap *        OpPow             (float other);
    cTMap *        OpAddAssign       (float other);
    cTMap *        OpSubAssign       (float other);
    cTMap *        OpMulAssign       (float other);
    cTMap *        OpDivAssign       (float other);
    cTMap *        OpPowAssign       (float other);

    cTMap *        OpModAssign       (cTMap *other);
    cTMap *        OpModAssign       (float other);
    cTMap *        OpMod             (cTMap *other);
    cTMap *        OpMod             (float other);

    cTMap *        EqualTo           (cTMap *other);
    cTMap *        LargerThen        (cTMap *other);
    cTMap *        SmallerThen       (cTMap *other);
    cTMap *        NotEqualTo        (cTMap *other);
    cTMap *        LargerEqualThen   (cTMap *other);
    cTMap *        SmallerEqualThen  (cTMap *other);

    cTMap *        EqualTo           (float other);
    cTMap *        LargerThen        (float other);
    cTMap *        SmallerThen       (float other);
    cTMap *        NotEqualTo        (float other);
    cTMap *        LargerEqualThen   (float other);
    cTMap *        SmallerEqualThen  (float other);

    cTMap *        And               (cTMap *other);
    cTMap *        Or                (cTMap *other);
    cTMap *        Xor               (cTMap *other);

    cTMap *        And               (float other);
    cTMap *        Or                (float other);
    cTMap *        Xor               (float other);

    cTMap *        AndAssign         (cTMap *other);
    cTMap *        OrAssign          (cTMap *other);
    cTMap *        XorAssign         (cTMap *other);

    cTMap *        AndAssign         (float other);
    cTMap *        OrAssign          (float other);
    cTMap *        XorAssign         (float other);

    cTMap *        Negate            ();

    cTMap *        OpMod_r           (float other);
    cTMap *        OpAdd_r           (float other);
    cTMap *        OpMul_r           (float other);
    cTMap *        OpSub_r           (float other);
    cTMap *        OpDiv_r           (float other);
    cTMap *        OpPow_r           (float other);
    cTMap *        LargerThen_r      (float other);
    cTMap *        SmallerThen_r     (float other);
    cTMap *        NotEqualTo_r      (float other);
    cTMap *        LargerEqualThen_r (float other);
    cTMap *        SmallerEqualThen_r(float other);
    cTMap *        EqualTo_r         (float other);
    cTMap *        And_r             (float other);
    cTMap *        Or_r              (float other);
    cTMap *        Xor_r             (float other);


    inline static float DER_X(cTMap *m, int r, int c)
    {
        if(m != nullptr)
        {

            float val = 0.0;
            float val1 = 0.0;
            float val2 = 0.0;
            if(r > 0 && c > 0 && r < m->nrRows() && c < m->nrCols())
            {
                val = m->data[r][c];
                if(pcr::isMV(val))
                {
                    val = 0.0;
                }
            }
            if(r > 0 && c-1 > 0 && r < m->nrRows() && c-1 < m->nrCols())
            {
                val1 = m->data[r][c-1];
                if(pcr::isMV(val1))
                {
                    val1 = val;
                }
            }
            if(r > 0 && c+1 > 0 && r < m->nrRows() && c+1 < m->nrCols())
            {
                val2 = m->data[r][c+1];
                if(pcr::isMV(val2))
                {
                    val2 = val;
                }
            }
            return 0.5 * ((val2 - val) + (val - val1));
        }
        return 0.0;
    }
    inline static float DER_Y(cTMap *m, int r, int c)
    {
        if(m != nullptr)
        {

            float val = 0.0;
            float val1 = 0.0;
            float val2 = 0.0;
            if(r > 0 && c > 0 && r < m->nrRows() && c < m->nrCols())
            {
                val = m->data[r][c];
                if(pcr::isMV(val))
                {
                    val = 0.0;
                }
            }
            if(r-1 > 0 && c > 0 && r-1 < m->nrRows() && c < m->nrCols())
            {
                val1 = m->data[r-1][c];
                if(pcr::isMV(val1))
                {
                    val1 = val;
                }
            }
            if(r+1 > 0 && c > 0 && r+1 < m->nrRows() && c < m->nrCols())
            {
                val2 = m->data[r+1][c];
                if(pcr::isMV(val2))
                {
                    val2 = val;
                }
            }
            return 0.5 * ((val2 - val) + (val - val1));
        }
        return 0.0;
    }

private:

    //! Projection string as WKT string. Possibly empty.
    QString        _projection;

    QString        _mapName;
};


inline cTMap::cTMap(
    MaskedRaster<float>&& data,
    QString const& projection,
    QString const& mapName,
        bool is_ldd)

    : data(std::forward<MaskedRaster<float>>(data)),
      _projection(projection),
      _mapName(mapName)

{

    m_Projection = GeoProjection::FromString(projection);
    m_BoundingBox = BoundingBox(data.west(),data.west() + data.cell_sizeX() * ((double)(data.nr_cols())),data.north(),data.north() + data.cell_sizeY() * ((double)(data.nr_rows())));

    AS_IsLDD = is_ldd;
}

inline int cTMap::nrRows() const
{
    return static_cast<int>(data.nr_rows());
}


inline int cTMap::nrCols() const
{
    return static_cast<int>(data.nr_cols());
}

inline double cTMap::north() const
{
    return data.north();
}

inline double cTMap::west() const
{
    return data.west();
}


inline double cTMap::cellSize() const
{
    return data.cell_size();
}

inline double cTMap::cellSizeX() const
{
    return data.cell_sizeX();
}


inline double cTMap::cellSizeY() const
{
    return data.cell_sizeY();
}

inline void cTMap::setnorth(double x)
{
    data._north = x;

}
inline void cTMap::setwest(double x)
{

    data._west = x;
}
inline void cTMap::setcellSizeX(double x)
{

    data._cell_size= x;
    data._cell_sizeX= x;
}
inline void cTMap::setcellSizeY(double x)
{
    data._cell_sizeY= x;
}
inline void cTMap::setprojectionstring (QString p)
{
    _projection = p;
}


inline QString const& cTMap::projection() const
{
    return _projection;
}


inline QString const& cTMap::mapName() const
{
    return _mapName;
}


inline void cTMap::setAllMV()
{
    data.set_all_mv();
}


// make a new map according to dup as a mask and filled with value
inline void cTMap::MakeMap(
    cTMap *dup,
    REAL4 value)
{
  if (dup == nullptr)
    return;

  data = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
      dup->west(), dup->cellSizeX(),dup->cellSizeY());

  data.set_all_mv();

  for(int r=0; r < nrRows(); r++)
    for(int c=0; c < nrCols(); c++)
      if (!pcr::isMV(dup->data[r][c]))
        {
          data[r][c] = value;
        }

    SetProjection(dup->GetProjection());
    m_BoundingBox = dup->GetBoundingBox();
}

inline cTMap * cTMap::GetCopy() const
{
    const cTMap * dup = this;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(std::move(datac),_projection,_mapName,AS_IsLDD);
    m->AS_IsSingleValue = AS_IsSingleValue;
    m->AS_FileName = AS_FileName;
    m->AS_IsLDD = AS_IsLDD;
    m->m_Projection = m_Projection;
    m->m_BoundingBox = m_BoundingBox;

    m->setAllMV();

    for(int r=0; r < m->nrRows(); r++)
    {
      for(int c=0; c < m->nrCols(); c++)
      {
        if (!pcr::isMV(data[r][c]))
          {
            m->data[r][c] = data[r][c];
          }
      }
    }

    return m;
}

inline cTMap * cTMap::GetCopyMask() const
{
    const cTMap * dup = this;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(std::move(datac),_projection,_mapName,AS_IsLDD);
    m->AS_IsSingleValue = AS_IsSingleValue;
    m->AS_FileName = AS_FileName;
    m->AS_IsLDD = AS_IsLDD;
    m->m_Projection = m_Projection;
    m->m_BoundingBox = m_BoundingBox;

    m->setAllMV();

    for(int r=0; r < m->nrRows(); r++)
    {
      for(int c=0; c < m->nrCols(); c++)
      {
        if (!pcr::isMV(data[r][c]))
          {
            m->data[r][c] = 1.0;
          }else {
            m->data[r][c] = 0.0;
        }
      }
    }

    return m;
}

inline cTMap * cTMap::GetCopy0() const
{
    const cTMap * dup = this;
    MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(std::move(datac),_projection,_mapName,AS_IsLDD);
    m->AS_IsSingleValue = AS_IsSingleValue;
    m->AS_FileName = AS_FileName;
    m->AS_IsLDD = AS_IsLDD;
    m->m_Projection = m_Projection;
    m->m_BoundingBox = m_BoundingBox;

    return m;
}



//Angelscript related functionality

inline void cTMap::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void cTMap::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;

    if(AS_refcount == 0)
    {
        delete this;
    }

}



inline float& cTMap::Value(int r, int c)
{
    return this->data[r][c];
}


inline int cTMap::Rows()
{
    return this->nrRows();
}

inline int cTMap::Cols()
{
    return this->nrCols();
}

inline float cTMap::North()
{
    return this->north();

}

inline float cTMap::West()
{
    return this->west();

}

inline float cTMap::CellSize()
{
    return this->cellSize();

}

inline static cTMap * AS_SetCRS(cTMap *m ,GeoProjection * p)
{
    m = m->GetCopy();
    if(p != nullptr)
    {
        m->SetProjection(*p);
    }
    return m;

}

inline float cTMap::SampleLinear(double x, double y)
{
    float ans = 0.0;

    float x_pix = (x - west())/cellSizeX();
    float y_pix = (y - north())/cellSizeY();

    //get closest pixel


    int xpix = x_pix;
    int ypix = y_pix;

    float x_frac = 1.0-(x_pix - (double)(xpix));
    float y_frac =  (cellSizeY() < 0)? 1.0-(y_pix - (double)(ypix)) : (y_pix - (double)(ypix));

    ypix = (cellSizeY() < 0? ypix + 1 : ypix);

    int xpix_x = xpix+(cellSizeX() > 0?1:-1);
    int ypix_y = ypix+(cellSizeY() > 0?1:-1);

    float val_x1y1,val_x1y2,val_x2y1,val_x2y2;

    int tmv =0;
    float tvaltemp = 0.0;
    int bmv =0;
    float bvaltemp = 0.0;

    if(!((xpix < 0)|| (xpix > (nrCols() - 1)) || (ypix < 0)|| (ypix > (nrRows() - 1))))
    {
       val_x1y1 = data[ypix][xpix];
       if(pcr::isMV(val_x1y1))
       {
           tmv ++;
       }else {
           tvaltemp = val_x1y1;
        }
    }else {
       pcr::setMV(val_x1y1);
       tmv ++;
    }

    if(!((xpix_x < 0)|| (xpix_x > (nrCols() - 1)) || (ypix < 0)|| (ypix > (nrRows() - 1))))
    {
       val_x2y1 = data[ypix][xpix_x];
       if(pcr::isMV(val_x2y1))
       {
           tmv ++;
       }else {
           tvaltemp = val_x2y1;
        }
    }else {
       pcr::setMV(val_x2y1);
       tmv ++;
    }

    if(!((xpix < 0)|| (xpix > (nrCols() - 1)) || (ypix_y < 0)|| (ypix_y > (nrRows() - 1))))
    {
       val_x1y2 = data[ypix_y][xpix];
       if(pcr::isMV(val_x1y2))
       {
           bmv ++;
       }else {
           bvaltemp = val_x1y2;
        }
    }else {
       pcr::setMV(val_x1y2);
       bmv ++;
    }

    if(!((xpix_x < 0)|| (xpix_x > (nrCols() - 1)) || (ypix_y < 0)|| (ypix_y > (nrRows() - 1))))
    {
       val_x2y2 = data[ypix_y][xpix_x];
       if(pcr::isMV(val_x2y2))
       {
           bmv ++;
       }else {
           bvaltemp = val_x2y2;
        }
    }else {
       pcr::setMV(val_x2y2);
       bmv++;
    }

    if((tmv == 2) && (bmv == 2))
    {
        float val;
        pcr::setMV(val);
        return val;

    }else if((tmv == 1) && (bmv == 2))
    {
        return tvaltemp;
    }else if((tmv == 2) && (bmv == 1))
    {
        return bvaltemp;
    }else if((tmv == 0) && (bmv == 2))
    {
        return x_frac * val_x2y1 + (1.0-x_frac)* val_x1y1;
    }else if((tmv == 2) && (bmv == 0))
    {
        return x_frac * val_x1y2 + (1.0-x_frac)* val_x2y2;
    }else if((tmv == 1) && (bmv == 0))
    {
        return y_frac * ( x_frac * val_x1y2 + (1.0-x_frac)* val_x2y2) + (1.0-y_frac)* tvaltemp;
    }else if((tmv == 1) && (bmv == 1))
    {
        return y_frac * bvaltemp + (1.0-y_frac)* tvaltemp;
    }else if((tmv == 0) && (bmv == 1))
    {
        return y_frac * bvaltemp + (1.0-y_frac)* ( x_frac * val_x1y1 + (1.0-x_frac)* val_x2y1) ;
    }else if((tmv == 0) && (bmv == 0))
    {
        return y_frac * ( x_frac * val_x1y2 + (1.0-x_frac)* val_x2y2) + (1.0-y_frac)* ( x_frac * val_x1y1 + (1.0-x_frac)* val_x2y1) ;
    }

    return data[ypix][xpix];
}

inline float cTMap::GetPixelCovered(int r, int c, float cover)
{
    float ret = cover;

    if(r > -1 && r< nrRows() && c > -1 && c < nrCols())
    {
        float val = data[r][c];
        if(!pcr::isMV(val))
        {
            ret = val;
        }
    }
    return ret;
}

inline float cTMap::SampleCubic(double x, double y)
{
    // calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel

    x = (x - 0.5 * cellSizeX() - west())/cellSizeX();
    y = (y - 0.5 * cellSizeY() - north())/cellSizeY();

    int xint = int(x);
    float xfract = std::fabs((x - floor(x)));//(cellSizeY() > 0?  std::fabs((x - floor(x))) : 1.0 - std::fabs((x - floor(x))));

    int yint = int(y);
    float yfract = std::fabs((y - floor(y)));// (cellSizeY() > 0? std::fabs( (y - floor(y))) : 1.0 - std::fabs( (y - floor(y))));
    float value;

    if(!((xint < 0)|| (xint > (nrCols() - 1)) || (yint < 0)|| (yint> (nrRows() - 1))))
    {

        value = data[yint][xint];

    }else {
        float x;
        pcr::setMV(x);
        return x;
    }

    int signx = 1.0;//(cellSizeX() > 0? 1:-1);
    int signy = 1.0;//(cellSizeY() > 0? 1:-1);

    // 1st row
    float p00 = GetPixelCovered(yint-signy*1,xint-signx*1,value);
    float p10 = GetPixelCovered(yint-signy*1,xint+signx*0,value);//GetPixelClamped(image, xint + 0, yint - 1);
    float p20 = GetPixelCovered(yint-signy*1,xint+signx*1,value);//GetPixelClamped(image, xint + 1, yint - 1);
    float p30 = GetPixelCovered(yint-signy*1,xint+signx*2,value);//GetPixelClamped(image, xint + 2, yint - 1);

    // 2nd row
    float p01 = GetPixelCovered(yint+signy*0,xint-signx*1,value);//GetPixelClamped(image, xint - 1, yint + 0);
    float p11 = GetPixelCovered(yint+signy*0,xint+signx*0,value);//GetPixelClamped(image, xint + 0, yint + 0);
    float p21 = GetPixelCovered(yint+signy*0,xint+signx*1,value);//GetPixelClamped(image, xint + 1, yint + 0);
    float p31 = GetPixelCovered(yint+signy*0,xint+signx*2,value);//GetPixelClamped(image, xint + 2, yint + 0);

    // 3rd row
    auto p02 = GetPixelCovered(yint+signy*1,xint-signx*1,value);//GetPixelClamped(image, xint - 1, yint + 1);
    auto p12 = GetPixelCovered(yint+signy*1,xint+signx*0,value);//GetPixelClamped(image, xint + 0, yint + 1);
    auto p22 = GetPixelCovered(yint+signy*1,xint+signx*1,value);//GetPixelClamped(image, xint + 1, yint + 1);
    auto p32 = GetPixelCovered(yint+signy*1,xint+signx*2,value);//GetPixelClamped(image, xint + 2, yint + 1);

    // 4th row
    float p03 = GetPixelCovered(yint+signy*2,xint-signx*1,value);//GetPixelClamped(image, xint - 1, yint + 2);
    float p13 = GetPixelCovered(yint+signy*2,xint+signx*0,value);//GetPixelClamped(image, xint + 0, yint + 2);
    float p23 = GetPixelCovered(yint+signy*2,xint+signx*1,value);//GetPixelClamped(image, xint + 1, yint + 2);
    float p33 = GetPixelCovered(yint+signy*2,xint+signx*2,value);//GetPixelClamped(image, xint + 2, yint + 2);

    // interpolate bi-cubically!

    float col0 = LISEMMath::CubicHermite(p00, p10, p20, p30, xfract);
    float col1 = LISEMMath::CubicHermite(p01, p11, p21, p31, xfract);
    float col2 = LISEMMath::CubicHermite(p02, p12, p22, p32, xfract);
    float col3 = LISEMMath::CubicHermite(p03, p13, p23, p33, xfract);
    float valuen = LISEMMath::CubicHermite(col0, col1, col2, col3, yfract);

    return valuen;

}



inline float cTMap::Sample(double x, double y)
{
    float ans = 0.0;

    float x_pix = (x - west())/cellSizeX();
    float y_pix = (y - north())/cellSizeY();

    //get closest pixel

    int xpix = x_pix;
    int ypix = y_pix;

    if((xpix < 0)|| (xpix > (nrCols() - 1)))
    {
        pcr::setMV(ans);
        return ans;
    }
    if((ypix < 0)|| (ypix > (nrRows() - 1)))
    {
        pcr::setMV(ans);
        return ans;
    }

    return data[ypix][xpix];
}


inline cTMap* FromGDALDatasetR(GDALDataset * dataset)
{
    cTMap * ret;

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        return new cTMap();
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};

    QString datatype(dataset->GetMetadataItem("PCRASTER_VALUESCALE"));

    // Read the first raster band.
    GDALRasterBand* band{dataset->GetRasterBand(1)};
    assert(band);

    int const nr_rows{band->GetYSize()};
    int const nr_cols{band->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};

    MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

    // All raster values are read into float. PCRaster value scales are not
    // taken into account.
    if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
            nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band cannot be read."));
        return new cTMap();
    }

    int hasNoDataValue{false};
    double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
    if(hasNoDataValue) {
        raster_data.replace_with_mv(noDataValue);
    }


    return new cTMap(std::move(raster_data), projection, "",datatype.compare("VS_LDD") == 0);

}

inline GDALDataset * ToGDALDataset(GDALDataset * to, cTMap * raster, int bandn = 1)
{
    //return new ShapeFile();

    int const nrRows{raster->nrRows()};
    int const nrCols{raster->nrCols()};
    int const nrBands{1};

    MaskedRaster<float> const& raster_data{raster->data};

    double transformation[]{
        raster_data.west(),
        raster_data.cell_sizeX(),
        0.0,
        raster_data.north(),
        0.0,
        raster_data.cell_sizeY()};

    to->SetGeoTransform(transformation);
    to->SetProjection(raster->GetProjection().GetWKT().toStdString().c_str());

    auto band = to->GetRasterBand(bandn);
    band->SetNoDataValue(-FLT_MAX);
    if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
            const_cast<float*>(&raster_data.cell(0)),
            nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band cannot be written to memory file."));
        return to;
    }




    return to;
}

inline cTMapProps     cTMap::GetRasterProps      ()
{
    cTMapProps p;

    p.ulx = west();
    p.uly = north();
    p.band = 0;
    p.bands = 1;
    p.sizex = nrCols();
    p.sizey = nrRows();
    p.is_ldd = AS_IsLDD;
    p.projection = projection();
    p.cellsizex = cellSizeX();
    p.cellsizey = cellSizeY();
    p.can_be_read = AS_FileName.isEmpty()?false:true;
    p.file_path = AS_FileName;

    return p;
}

inline RasterBandStats cTMap::GetRasterBandStats(bool do_stdev = false)
{
    RasterBandStats r;

    float hmax = -1e31f;
    float hmin = 1e31f;
    float havg = 0.0f;
    float n = 0.0;
    float stdev = 0.0;

    int n_mv = 0;
    for(int r = 0; r < this->nrRows(); r++)
    {
        for (int c = 0; c < this->nrCols(); c++)
        {


        if(!pcr::isMV(this->data[r][c]))
        {
            if(std::isfinite(this->data[r][c]) && !std::isnan((this->data[r][c])))
            {
                n++;
                havg += (this->data[r][c]);
                hmax = std::max(hmax,this->data[r][c]);
                hmin = std::min(hmin,this->data[r][c]);
            }else {
                n_mv ++;
            }
        }else {
            n_mv++;
        }
        }
    }

    n = std::max(1.0f,n);

    if(do_stdev)
    {
        FOR_ROW_COL_MV(this)
        {
            if(std::isfinite(this->data[r][c]) && !std::isnan((this->data[r][c])))
            {
                stdev += (this->data[r][c] -havg)*(this->data[r][c] -havg);
            }
        }
        stdev = stdev/n;
    }else {
        stdev = 0.35 * (hmax-hmin);
    }

    r.mean = havg/n;
    r.min = hmin;
    r.max = hmax;
    r.stdev = std::sqrt(stdev);
    r.n_mv = n_mv;

    return r;

}




static void DeNormalizeMapInPlace(cTMap *m, double min, double max)
{

    bool found = false;
    //get minimum and maximum
    for(int r = 0; r < m->data.nr_rows();r++)
    {
        for(int c = 0; c < m->data.nr_cols();c++)
        {
            if(!pcr::isMV(m->data[r][c]))
            {
                m->data[r][c] = min + (max-min) * m->data[r][c];
            }
        }
    }
}
static void NormalizeMapInPlace(cTMap *m, double &min, double &max)
{

    bool found = false;
    //get minimum and maximum
    for(int r = 0; r < m->data.nr_rows();r++)
    {
        for(int c = 0; c < m->data.nr_cols();c++)
        {
            if(!pcr::isMV(m->data[r][c]))
            {
                if(!found)
                {
                    min = m->data[r][c];
                    max = m->data[r][c];
                    found = true;
                }else {

                    min = std::min(min,(double)m->data[r][c]);
                    max = std::max(max,(double)m->data[r][c]);
                }
            }
        }
    }
    //normalize values between zero and 1
    if(max > min)
    {
        for(int r = 0; r < m->data.nr_rows();r++)
        {
            for(int c = 0; c < m->data.nr_cols();c++)
            {
                if(!pcr::isMV(m->data[r][c]))
                {
                    m->data[r][c] = std::min(1.0,std::max(0.0,(m->data[r][c] - min)/(max-min)));
                }else {
                    pcr::setMV(m->data[r][c]);
                }
            }
        }
    }
}


static cTMap * NormalizeMap(cTMap *m, double &min, double &max)
{
    cTMap * res = m->GetCopy();

    bool found = false;
    //get minimum and maximum
    for(int r = 0; r < m->data.nr_rows();r++)
    {
        for(int c = 0; c < m->data.nr_cols();c++)
        {
            if(!pcr::isMV(m->data[r][c]))
            {
                if(!found)
                {
                    min = m->data[r][c];
                    max = m->data[r][c];
                    found = true;
                }else {

                    min = std::min(min,(double)m->data[r][c]);
                    max = std::max(max,(double)m->data[r][c]);
                }
            }
        }
    }
    //normalize values between zero and 1
    if(max > min)
    {
        for(int r = 0; r < m->data.nr_rows();r++)
        {
            for(int c = 0; c < m->data.nr_cols();c++)
            {
                if(!pcr::isMV(m->data[r][c]))
                {
                    res->data[r][c] = std::min(1.0,std::max(0.0,(res->data[r][c] - min)/(max-min)));
                }
            }
        }
    }

    return res;
}

static cTMap * AS_NormalizeMap(cTMap * m)
{
    double min;
    double max;
    cTMap * ret = NormalizeMap(m,min,max);
    return ret;
}


#endif
