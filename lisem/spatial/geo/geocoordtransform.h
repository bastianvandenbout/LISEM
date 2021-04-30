#ifndef GEOCOORDTRANSFORM_H
#define GEOCOORDTRANSFORM_H

#include "geo/geoprojection.h"
#include "ogr_core.h"
#include "ogr_spatialref.h"
#include "linear/lsm_vector2.h"
#include "error.h"

class GeoCoordTransformer
{

private:
    OGRCoordinateTransformation *m_Transform = nullptr;
    bool m_IsValid = true;
    bool m_IsGeneric = false;
    bool m_IsOffset = false;
    LSMVector2 m_Offset = LSMVector2(0.0,0.0);
    GeoProjection m_From;
    GeoProjection m_To;
public:

    inline bool IsGeneric()
    {
        return m_IsGeneric;
    }

    inline ~GeoCoordTransformer()
    {
        if(m_Transform != nullptr)
        {
            OCTDestroyCoordinateTransformation(m_Transform->ToHandle(m_Transform));
        }
    }

    inline bool IsOffsetOnly()
    {
        return m_IsOffset;
    }
    inline GeoProjection GetGeoProjectionFrom()
    {
        return m_From;
    }
    inline GeoProjection GetGeoProjectionTo()
    {
        return m_To;
    }

    inline void Transform(int count, float * x, float * y)
    {
        if(m_IsOffset &&  !m_IsGeneric)
        {
            for(int i = 0; i < count; i++)
            {
                x[i] = x[i] + m_Offset.x;
                y[i] = y[i] + m_Offset.y;
            }

        }else if(m_Transform != nullptr && m_IsValid != false && !m_IsGeneric)
        {
            try
            {
                try
                {
                    double * xd= new double[count];
                    double * yd= new double[count];

                    for(int i = 0; i < count; i++)
                    {
                        xd[i] = (double) (x[i]);
                        yd[i] = (double) (y[i]);
                    }
                    m_Transform->Transform(count,xd,yd);
                    for(int i = 0; i < count; i++)
                    {
                        x[i] = (float) (xd[i]);
                        y[i] = (float) (yd[i]);
                    }
                    delete[] xd;
                    delete[] yd;

                }catch (std::bad_alloc&)
                {

                }

            }catch(CPLErr e )
            {
                LISEM_ERROR("Error when transforming raster coordinates");
            }
        }
    }

    inline void Transform(int count, double * x, double * y)
    {
        if(m_IsOffset &&  !m_IsGeneric)
        {
            for(int i = 0; i < count; i++)
            {
                x[i] = x[i] + m_Offset.x;
                y[i] = y[i] + m_Offset.y;
            }

        }else if(m_Transform != nullptr && m_IsValid != false && !m_IsGeneric)
        {
            try
            {
                m_Transform->Transform(count,x,y);
            }catch(CPLErr e )
            {
                LISEM_ERROR("Error when transforming raster coordinates");
            }
        }
    }

    inline LSMVector2 Transform(LSMVector2 in)
    {
        if(m_IsOffset &&  !m_IsGeneric)
        {
            return in + m_Offset;

        }else if(m_Transform != nullptr && m_IsValid != false && !m_IsGeneric)
        {
            double x = in.x;
            double y = in.y;
            m_Transform->Transform(1,&x,&y);
            in.x = x;
            in.y = y;
        }

        return in;

    }

    inline BoundingBox Transform(BoundingBox b)
    {

        LSMVector2 tl = LSMVector2(b.GetMinX(),b.GetMinY());
        LSMVector2 br = LSMVector2(b.GetMaxX(),b.GetMaxY());

        tl = Transform(tl);
        br = Transform(br);

        return BoundingBox(tl.x,br.x,tl.y,br.y);
    }

    static inline GeoCoordTransformer * GetCoordTransformerGeneric()
    {
        GeoCoordTransformer * t = new GeoCoordTransformer();
        //if(from.IsGeneric() || to.IsGeneric())
        {
            t->m_IsGeneric = true;
            return t;
        }
   }



    static inline GeoCoordTransformer *GetCoordTransformer(LSMVector2 offset)
    {
        GeoCoordTransformer * t = new GeoCoordTransformer();
        if(offset.x == 0.0 && offset.y == 0.0)
        {
            t->m_IsGeneric = true;
            return t;
        }

        t->m_IsGeneric = false;
        t->m_IsOffset = true;
        t->m_Offset = offset;
        return t;
    }

    static inline GeoCoordTransformer *GetCoordTransformer(GeoProjection from, GeoProjection to)
    {
        GeoCoordTransformer * t = new GeoCoordTransformer();
        if(from.IsGeneric() || to.IsGeneric())
        {
            t->m_IsGeneric = true;
            return t;
        }
        if(from.IsEqualTo(to))
        {
            t->m_IsGeneric = true;
            return t;
        }
        OGRSpatialReference fromref = from.GetOGRRef();
        OGRSpatialReference toref = to.GetOGRRef();
        fromref.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        toref.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        OGRCoordinateTransformationOptions options;
        t->m_Transform = OGRCreateCoordinateTransformation( &fromref, &toref, options );
        t->m_From = from;
        t->m_To = to;
        if(t->m_Transform == nullptr)
        {
            t->m_IsValid = false;
            t->m_IsGeneric = true;
        }

        return t;
    }

};


#endif // GEOCOORDTRANSFORM_H
