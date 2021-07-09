#ifndef GL3DELEVATIONPROVIDER_H
#define GL3DELEVATIONPROVIDER_H

#include "QList"
#include "geo/raster/map.h"
#include "linear/lsm_vector3.h"
#include "openglclmanager.h"
#include "boundingbox.h"
#include "openglmsaarendertarget.h"
#include "vector"

class GL3DElevationProvider
{
private:

    QList<cTMap *> m_ElevationMaps;
    QList<BoundingBox> m_MapExtents;

    double m_ZMult = 1.0;

    std::vector<UIDistanceEstimator*> m_Dlist;

    double m_ElevationCenter = 0.0;
    bool m_HasElevationCenter = false;
public:

    inline GL3DElevationProvider()
    {

    }

    inline void SetZMult(double zm)
    {
        m_ZMult = zm;
    }

    inline double GetZMult()
    {
        return m_ZMult;
    }
    inline void AddElevationModel(int x, int y)
    {
        MaskedRaster<float> raster_data(y, x,0.0,0.0,1.0);
        cTMap *map = new cTMap(std::move(raster_data),"","");
        map->AS_IsSingleValue = false;
        m_ElevationMaps.append(map);
        m_MapExtents.append(BoundingBox());
    }

    inline void SetElevationModel(int l, OpenGLCLManager * m,OpenGLCLMSAARenderTarget *t, BoundingBox b)
    {
        m->CopyTextureToMap(t,&(m_ElevationMaps.at(l)->data));
        m_MapExtents.replace(l,b);
    }

    inline void SetDetailedElevationAtCenter(bool has, float elevation)
    {
        m_HasElevationCenter = has;
        m_ElevationCenter = elevation;

    }

    inline void SetDistanceObjects(std::vector<UIDistanceEstimator*> list)
    {
        m_Dlist = list;
    }
    inline LSMVector3 GetDistance()
    {

        return LSMVector3(0.0,0.0,0.0);

    }

    inline bool HasElevation(float x, float z)
    {
        if(m_HasElevationCenter)
        {
            return true;
        }
        for(int i =0; i < m_MapExtents.length(); i++)
        {
            if(m_MapExtents.at(i).Contains(x,z))
            {
                float frac_x = (x - m_MapExtents.at(i).GetMinX())/m_MapExtents.at(i).GetSizeX();
                float frac_z = (z - m_MapExtents.at(i).GetMinY())/m_MapExtents.at(i).GetSizeY();

                int r = (float)(frac_z) * m_ElevationMaps.at(i)->nrRows();
                int c = (float)(frac_x) * m_ElevationMaps.at(i)->nrCols();

                if(r > -1 && r < m_ElevationMaps.at(i)->nrRows() && c > -1 && c < m_ElevationMaps.at(i)->nrCols() )
                {
                    if(!pcr::isMV(m_ElevationMaps.at(i)->data[r][c]))
                    {
                        return true;
                    }
                }

            }
        }

        return false;

    }
    inline double GetElevation(float x, float z)
    {
        for(int i =0; i < m_MapExtents.length(); i++)
        {
            if(m_MapExtents.at(i).Contains(x,z))
            {
                float frac_x = (x - m_MapExtents.at(i).GetMinX())/m_MapExtents.at(i).GetSizeX();
                float frac_z = (z - m_MapExtents.at(i).GetMinY())/m_MapExtents.at(i).GetSizeY();

                int r = (float)(frac_z) * m_ElevationMaps.at(i)->nrRows();
                int c = (float)(frac_x) * m_ElevationMaps.at(i)->nrCols();

                if(r > -1 && r < m_ElevationMaps.at(i)->nrRows() && c > -1 && c < m_ElevationMaps.at(i)->nrCols() )
                {
                    return m_ElevationMaps.at(i)->data[r][c];
                }

            }
        }

        return 0.0;

    }


    inline double GetElevationAtCenter(float x, float z)
    {
        if(m_HasElevationCenter)
        {
            return m_ElevationCenter;
        }else
        {
            return GetElevation(x,z);
        }
    }

    inline void SetAllZero()
    {
        for(int i =0; i < m_ElevationMaps.length(); i++)
        {
            for(int r = 0; r < m_ElevationMaps.at(i)->nrRows(); r++)
            {
                for(int c = 0; c < m_ElevationMaps.at(i)->nrCols(); c++)
                {
                    m_ElevationMaps.at(i)->data[r][c] = 0.0;
                }
            }

        }
    }

    inline double GetClosestDistanceToObjectVertWeight(LSMVector3 Position)
    {
        double size = 1e31;
        double dist_min = 1e31;
        for(int i = 0; i < m_Dlist.size(); i++)
        {
            double distance = m_Dlist.at(i)->GetNormalizedSmoothDistance(Position);
            if(distance < dist_min)
            {
                size = m_Dlist.at(i)->GetNormalizedSmoothVertWeight(Position);
            }

            dist_min = std::min(distance,dist_min);
        }

        return size;
    }
    inline double GetClosestDistanceToObjectSize(LSMVector3 Position)
    {
        double size = 1e31;
        double dist_min = 1e31;
        for(int i = 0; i < m_Dlist.size(); i++)
        {
            double distance = m_Dlist.at(i)->GetNormalizedSmoothDistance(Position);
            if(distance < dist_min)
            {
                size = m_Dlist.at(i)->GetNormalizedSmoothSize(Position);
            }

            dist_min = std::min(distance,dist_min);
        }

        return size;
    }

    inline double GetClosestDistanceToObject(LSMVector3 Position)
    {
        double dist_min = 1e31;
        for(int i = 0; i < m_Dlist.size(); i++)
        {
            double distance = m_Dlist.at(i)->GetNormalizedSmoothDistance(Position);
            dist_min = std::min(distance,dist_min);
        }

        return dist_min;

    }

    inline LSMVector3 RayCast(LSMVector3 From, LSMVector3 dir, bool globe = false, float conv_m = 1.0)
    {



        return LSMVector3(0.0,0.0,0.0);
    }


};


#endif // GL3DELEVATIONPROVIDER_H
