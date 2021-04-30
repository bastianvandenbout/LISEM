#ifndef RASTERCUBE_H
#define RASTERCUBE_H


#include "geo/raster/map.h"
/*inline static CScriptArray * AS_DataCube(int rows, int cols, int levels)
{

    return nullptr;
}

inline static CScriptArray * AS_DataCube(int rows, int cols, int levels, double y_min, double y_max, double x_min, double x_max, double z_min, double z_max)
{

    return nullptr;
}

inline static bool AS_CheckCube(CScriptArray * maps)
{


    return true;
}
inline static void AS_WriteCube(CScriptArray * maps)
{

}

inline static CScriptArray *  AS_WriteCube(QString file)
{
    return nullptr;

}
inline static CScriptArray *  AS_WriteCubeAbsPath(QString file)
{
    return nullptr;

}*/

inline static cTMap * AS_GetXSlice(std::vector<cTMap*> qmaps, int col, double zmin, double zmax, int levelinterval, int leveloffset)
{

    if(qmaps.size() < 1)
    {
        LISEMS_ERROR("Can not create profile from empty datacube");
        throw 1;
    }

    if( leveloffset < qmaps.size())
    {
        for(int i = 0; i < leveloffset; i++)
        {
            qmaps.erase(qmaps.begin() + 0);
        }
    }else {
        LISEMS_ERROR("Can not create profile with datacube offset");
        throw 1;
    }
    if(!(levelinterval > 0))
    {
        LISEMS_ERROR("Interval must be greater then 0");
        throw 1;
    }
    int nlevels = qmaps.size()/levelinterval;
    double csy = (zmin - zmax)/((double)(nlevels));


    MaskedRaster<float> raster_data(nlevels, qmaps.at(0)->nrRows(), zmax, qmaps.at(0)->north() , std::fabs(qmaps.at(0)->data.cell_sizeY()),csy);
    cTMap *map = new cTMap(std::move(raster_data),qmaps.at(0)->projection(),"");


    for(int r =  nlevels-1; r >-1; r--)
    {
        for(int c = 0; c < qmaps.at(0)->nrRows(); c++)
        {
                map->data[nlevels-1 - r][c] = qmaps.at(r * levelinterval)->data[c][col];
        }
    }

    return map;


}
inline static cTMap * AS_GetYSlice(std::vector<cTMap*> qmaps, int row, double zmin, double zmax, int levelinterval, int leveloffset)
{
    if(qmaps.size() < 1)
    {
        LISEMS_ERROR("Can not create profile from empty datacube");
        throw 1;
    }
    if( leveloffset < qmaps.size())
    {
        for(int i = 0; i < leveloffset; i++)
        {
            qmaps.erase(qmaps.begin()+0);
        }
    }else {
        LISEMS_ERROR("Can not create profile with datacube offset");
        throw 1;
    }
    if(!(levelinterval > 0))
    {
        LISEMS_ERROR("Interval must be greater then 0");
        throw 1;
    }

    int nlevels = qmaps.size()/levelinterval;
    double csy = (zmin - zmax)/((double)(nlevels));

    MaskedRaster<float> raster_data( qmaps.at(0)->nrCols(),qmaps.size(), zmax, qmaps.at(0)->west(), qmaps.at(0)->data.cell_sizeX(),csy);
    cTMap *map = new cTMap(std::move(raster_data),qmaps.at(0)->projection(),"");

    for(int r =  nlevels-1; r >-1; r--)
    {
        for(int c = 0; c < qmaps.at(0)->nrCols(); c++)
        {
                map->data[nlevels-1 -r][c] = qmaps.at(r * levelinterval)->data[row][c];
        }
    }

    return map;
}










#endif // RASTERCUBE_H
