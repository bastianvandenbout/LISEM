#ifndef RASTERPROFILE_H
#define RASTERPROFILE_H


#include "geo/raster/map.h"

inline static cTMap * AS_ProfileMap(cTMap * terrain, LSMVector2 start, LSMVector2 end, float cellsize, float zmin, float zmax)
{
    LSMVector2 delta = end-start;
    int n_cells = std::max(1, ((int)(delta.length()/cellsize)));

    double dist = delta.length();

    if(zmax < zmin)
    {
        float temp = zmax;
        zmax = zmin;
        zmin = temp;
    }
    int n_rows = std::max(1, ((int)((zmax - zmin)/cellsize)));

    //create map
    MaskedRaster<float> raster_data(n_rows,n_cells, zmax, 0.0, cellsize,-cellsize);
    cTMap *map = new cTMap(std::move(raster_data),"","");


    //sample elevation for each column

    for(int c = 0; c <  map->nrCols(); c++)
    {
        LSMVector2 coord = start + (((float)(c))*cellsize/dist) * (end-start);
        float x = start.x +(((float)(c))*cellsize/dist)  * (end.x-start.x);
        float y = start.y +(((float)(c))*cellsize/dist)  * (end.y-start.y);

        float terrainh = terrain->SampleCubic(x,y);

        std::cout << (end-start).x << " " << (end-start).y << " " << ((((float)(c))*cellsize/dist) * (end-start)).x << " " <<  ((((float)(c))*cellsize/dist) * (end-start)).y << " " << x << " " << y << std::endl;
        std::cout << "col " << c << " " << terrainh << " " << coord.x << " " << coord.y << " " << (((float)(c))*cellsize/dist) << " " << dist << " " << cellsize <<  " " << ((float)(c))*cellsize << std::endl;
        for(int r = 0; r < map->nrRows(); r++)
        {
            float z = zmax - ((float)(r))*cellsize;
            if(z < terrainh)
            {
                map->data[r][c] = z - terrainh ;
            }else
            {
                pcr::setMV(map->data[r][c]);
            }
        }
    }

    return map;

}





#endif // RASTERPROFILE_H
