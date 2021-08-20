#ifndef FIELDREDUCE_H
#define FIELDREDUCE_H

#include "geo/raster/field.h"




inline static cTMap *AS_MaxElevationValueAbove(Field * f, float threshold)
{
    std::vector<cTMap *>maps = f->GetMapList();
    if(maps.size() < 1)
    {
        LISEMS_ERROR("Can not do sub-section of field without any layers");
        throw 1;
    }


    cTMap * map = maps.at(0);
    cTMap * ret = map->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols(); c++)
        {
            int level = 0;

            bool mv = true;
            for(int i = 0; i < maps.size(); i++)
            {
                if(pcr::isMV(maps.at(i)->data[r][c]))
                {
                }else
                {
                    mv = false;
                    if(maps.at(i)->data[r][c] >= threshold)
                    {
                        level = i;
                    }
                }

            }

            if(mv)
            {
                pcr::setMV(ret->data[r][c]);
            }else
            {
                ret->data[r][c] = f->GetBottom() + ((float)(level)) * f->cellSizeZ();
            }

        }
    }
    return ret;
}

inline static cTMap * AS_MinValueVertical(Field * f)
{

    std::vector<cTMap *>maps = f->GetMapList();
    if(maps.size() < 1)
    {
        LISEMS_ERROR("Can not do sub-section of field without any layers");
        throw 1;
    }


    cTMap * map = maps.at(0);
    cTMap * ret = map->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols(); c++)
        {

            float min = 1e31f;
            bool mv = true;
            for(int i = 0; i < maps.size(); i++)
            {
                if(pcr::isMV(maps.at(i)->data[r][c]))
                {
                }else
                {
                    mv = false;
                    min = std::min(min,maps.at(i)->data[r][c]);
                }

            }

            if(mv)
            {
                pcr::setMV(ret->data[r][c]);
            }else
            {
                ret->data[r][c] = min;
            }

        }
    }

    return ret;
}


inline static cTMap * AS_MaxValueVertical(Field * f)
{

    std::vector<cTMap *>maps = f->GetMapList();
    if(maps.size() < 1)
    {
        LISEMS_ERROR("Can not do sub-section of field without any layers");
        throw 1;
    }


    cTMap * map = maps.at(0);
    cTMap * ret = map->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols(); c++)
        {

            float max = -1e31f;
            bool mv = true;
            for(int i = 0; i < maps.size(); i++)
            {
                if(pcr::isMV(maps.at(i)->data[r][c]))
                {
                }else
                {
                    mv = false;
                    max = std::max(max,maps.at(i)->data[r][c]);
                }

            }

            if(mv)
            {
                pcr::setMV(ret->data[r][c]);
            }else
            {
                ret->data[r][c] = max;
            }

        }
    }

    return ret;
}

inline static cTMap * AS_VerticalTotal(Field * f)
{

    std::vector<cTMap *>maps = f->GetMapList();
    if(maps.size() < 1)
    {
        LISEMS_ERROR("Can not do sub-section of field without any layers");
        throw 1;
    }


    cTMap * map = maps.at(0);
    cTMap * ret = map->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols(); c++)
        {

            float total = 0.0;
            bool mv = true;
            for(int i = 0; i < maps.size(); i++)
            {
                if(pcr::isMV(maps.at(i)->data[r][c]))
                {
                }else
                {
                    mv = false;
                    total += maps.at(i)->data[r][c];
                }

            }

            if(mv)
            {
                pcr::setMV(ret->data[r][c]);
            }else
            {
                ret->data[r][c] = total;
            }

        }
    }

    return ret;
}

inline static Field * AS_FieldSubSection(Field * f, BoundingBox b)
{

    std::vector<cTMap *>maps = f->GetMapList();
    if(maps.size() < 1)
    {
        LISEMS_ERROR("Can not do sub-section of field without any layers");
        throw 1;
    }
    cTMap * map1 = f->at(0);


    BoundingBox b1 = f->GetBoundingBox();
    b1.And(b);

    float dx = map1->cellSizeX();
    float dy = map1->cellSizeY();

    if(fabs(dx) < 1e-30 || fabs(dy) < 1e-30)
    {
        LISEMS_ERROR("GridcellSize must be larger then 0");
        throw 1;
    }
    int r0 = ((b.GetMinY() - map1->north())/dy);
    int c0 = ((b.GetMinX() - map1->west())/dx);

    int r1 = ((b.GetMaxY() - map1->north())/dy);
    int c1 = ((b.GetMaxX() - map1->west())/dx);

    if(c1 < c0)
    {
        int temp = c0;
        c0 = c1;
        c1 = temp;
    }
    if(r1 < r0)
    {
        int temp = r0;
        r0 = r1;
        r1 = temp;
    }
    r0 = std::max(0,std::min(r0,map1->nrRows()-1));
    c0 = std::max(0,std::min(c0,map1->nrCols()-1));
    r1 = std::max(r0,std::min(r1,map1->nrRows()-1));
    c1 = std::max(c0,std::min(c1,map1->nrCols()-1));
    int rows = r1-r0;
    int cols = c1-c0;


    if(rows == 0 || cols == 0)
    {
        LISEMS_ERROR("Invalid region provided for RasterSubSection");
        throw 1;
    }

    std::vector<cTMap *> newmaps;

    for(int i = 0; i < maps.size(); i++)
    {
        map1 = maps.at(i);
        MaskedRaster<float> raster_data(rows, cols, map1->data.north() + r0 * map1->cellSizeY(), map1->data.west() + c0 * map1->cellSizeX(), map1->data.cell_sizeX(),map1->data.cell_sizeY());
        cTMap *nmap = new cTMap(std::move(raster_data),map1->projection(),"");

        nmap->setAllMV();

        int rbegin = std::min(map1->nrRows() -1,std::max(0,r0));
        int cbegin = std::min(map1->nrCols() -1,std::max(0,c0));
        int rend = std::min(map1->nrRows() -1,std::max(0,r0 + rows));
        int cend = std::min(map1->nrCols() -1,std::max(0,c0 + cols));


        for(int r = rbegin; r < rend; r++)
        {
            for(int c = cbegin; c < cend; c++)
            {
                if(!pcr::isMV(map1->data[r][c]))
                {
                    nmap->data[r-r0][c-c0] = map1->data[r][c];
                }

            }
        }

        newmaps.push_back(nmap);

    }

    Field * fret= new Field();
    fret->SetFromMapList(newmaps,f->GetBottom(),f->cellSizeZ(),false);

    return fret;
}


#endif // FIELDREDUCE_H
