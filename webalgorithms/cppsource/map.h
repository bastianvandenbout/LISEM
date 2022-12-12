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
#include "masked_raster.h"
#include <functional>
#include <iostream>
#include <cmath>
#include "lisemmath.h"
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

/*!
    @brief      A cTMap contains all relevant information about a raster.
    @todo       The data member must be made private.

    cTMap instances contain raster data, projection information and a map name.
    I/O of cTMap instances is handles by functions defined in the io module.
*/
class cTMap
{

public:

std::string m_projection = "";

    //! The actual raster.
    MaskedRaster<float> data;

                   cTMap               ()=default;
				   
				   cTMap 			   (float * data, int width, int height, float north, float west,float dx, float dy);
					
					cTMap               (MaskedRaster<float>&& data,
                                        bool is_ldd = false);
										
                   cTMap               (MaskedRaster<float>&& data,
										std::string projection,
                                        std::string mapName,
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

    float          val_min             ();
    float          val_max             ();

    void           setAllMV            ();

    void           MakeMap             (cTMap *dup,
                                        REAL4 value);

    float          GetPixelCovered      (int r, int c, float cover = 0.0);
    float          Sample              (double x, double y);
    float          SampleLinear              (double x, double y);
    float          SampleCubic              (double x, double y);
    cTMap*         GetCopy             () const;
    cTMap*         GetCopy0             () const;
    cTMap*         GetCopy1             () const;
    cTMap*         GetCopyMask          () const;

	inline std::string projection()
	{
		return m_projection;
	}
    RasterBandStats GetRasterBandStats  (bool stdev);

    //functions related to the Angelscript Scripting Access for maps
    //NOTE: maps can have a single numerical value, as indicated by member AS_IsSingleValue
    //the value itself is contained in the 1x1 raster data accasable by map->data[0][0]
public:

    float&         Value             (int r, int c);
    int            Rows              ();
    int            Cols              ();
    float          North             ();
    float          West              ();
    float          CellSize          ();

   
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

};

inline cTMap::cTMap(float * datain, int width, int height, float north, float west, float dx,float dy)
{
	data = MaskedRaster<float>(height,width, north,west,dx,dy);
	if(datain != nullptr)
	{
		
		for(int r = 0; r < height; r++)
		{
			for(int c = 0; c < width; c++)
			{
				int index = r * width + c;
				if(datain[index] < -1e30)
				{
					pcr::setMV(data[r][c]);
				}else{
					
				data[r][c] = datain[index];
				}
			}
		}
	}else{
		for(int r = 0; r < height; r++)
		{
			for(int c = 0; c < width; c++)
			{
				
				data[r][c] = 0.0;
				
			}
		}
		
	}
	
}


inline cTMap::cTMap(
    MaskedRaster<float>&& data,
        bool is_ldd)

    : data(std::forward<MaskedRaster<float>>(data))

{
    
}
inline cTMap::cTMap(
    MaskedRaster<float>&& da0ta,
	std::string project,
	std::string nam,
        bool is_ldd)

    : data(std::forward<MaskedRaster<float>>(data))

{
	
	m_projection = project;
    
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

}

inline cTMap * cTMap::GetCopy() const
{
    const cTMap * dup = this;
    //MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
    //   dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(nullptr, dup->nrCols(),dup->nrRows(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

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

    cTMap * m = new cTMap(std::move(datac),false);
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
    //MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
    //    dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(nullptr, dup->nrCols(),dup->nrRows(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());

    return m;
}


inline cTMap * cTMap::GetCopy1() const
{
    const cTMap * dup = this;
    //MaskedRaster<REAL4> datac = MaskedRaster<REAL4>(dup->nrRows(), dup->nrCols(), dup->north(),
    //    dup->west(), dup->cellSizeX(),dup->cellSizeY());

    cTMap * m = new cTMap(nullptr, dup->nrCols(),dup->nrRows(), dup->north(),
        dup->west(), dup->cellSizeX(),dup->cellSizeY());


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
