#ifndef RASTERCONSTRUCTORS_H
#define RASTERCONSTRUCTORS_H

#include "geo/raster/map.h"

inline cTMap * MapFactory()
{
    cTMap * m = new cTMap();


    return m;
}

inline cTMap * MapFactory(float x)
{
    MaskedRaster<float> raster_data(1, 1,0.0f,0.0f,1.0f);
    cTMap * m = new cTMap(std::move(raster_data),"","");
    m->data[0][0] = x;
    m->AS_IsSingleValue = true;
    return m;
}

inline cTMap * MapFactory(int x)
{
    MaskedRaster<float> raster_data(1, 1,0.0f,0.0f,1.0f);
    cTMap * m = new cTMap(std::move(raster_data),"","");
    m->data[0][0] = x;
    m->AS_IsSingleValue = true;
    return m;
}


inline cTMap * MapFactory(int x, int y)
{
    MaskedRaster<float> raster_data(y, x,0.0f,0.0f,1.0f);
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    return map;
}

inline cTMap * MapFactory(int x, int y, float dx)
{
    MaskedRaster<float> raster_data(y, x,0.0f,0.0f,dx);
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    return map;
}


inline cTMap * MapFactory(int x, int y, float dx, float value)
{
    MaskedRaster<float> raster_data(y, x,0.0f,0.0f,dx);
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = value;
        }
    }

    return map;
}

inline cTMap * MapFactory(int x, int y, float dx, float value, float north, float west)
{
    MaskedRaster<float> raster_data(y, x,north,west,dx);
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = value;
        }
    }

    return map;
}

inline cTMap * MapFactory(int x, int y, float dx, float dy, float value, float north, float west)
{
    MaskedRaster<float> raster_data(y, x,north,west,dx,dy);
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = value;
        }
    }

    return map;
}

inline cTMap * MapFactory(int x, int y, float dx, float dy, float value, float north, float west, QString Projection)
{
    MaskedRaster<float> raster_data(y, x,north,west,dx,dy);
    cTMap *map = new cTMap(std::move(raster_data),Projection,"");
    map->AS_IsSingleValue = false;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = value;
        }
    }

    return map;
}

inline cTMap * MapFactory(BoundingBox m, int rows, int cols )
{
    MaskedRaster<float> raster_data(rows,cols,m.GetMinY(),m.GetMinX(),m.GetSizeX()/((float)(cols)),m.GetSizeY()/((float)(rows)));
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = 0.0;
        }
    }

    return map;
}

inline cTMap * MapFactory(BoundingBox m, int rows, int cols, GeoProjection * p )
{
    MaskedRaster<float> raster_data(rows,cols,m.GetMinY(),m.GetMinX(),m.GetSizeX()/((float)(cols)),m.GetSizeY()/((float)(rows)));
    cTMap *map = new cTMap(std::move(raster_data),"","");
    map->AS_IsSingleValue = false;
    map->SetProjection(*p);
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            map->data[r][c] = 0.0;
        }
    }

    return map;
}

inline GeoProjection * AS_GetCRS(cTMap *m)
{
    return new GeoProjection(m->GetProjection());
}

inline BoundingBox AS_GetRegion(cTMap *m)
{
    return m->GetBoundingBox();
}

inline cTMap * MapFactory(const std::string &path)
{
    cTMap * m = new cTMap();
    return m;
}


#endif // RASTERCONSTRUCTORS_H
