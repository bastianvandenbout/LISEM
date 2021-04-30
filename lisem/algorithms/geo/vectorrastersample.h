#ifndef VECTORRASTERSAMPLE_H
#define VECTORRASTERSAMPLE_H


#define VECTORWARP_H

#include "gdal.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "gdal_utils.h"
#include "geo/shapes/shapefile.h"
#include "geo/shapes/shapelayer.h"
#include <QString>
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include "gdal_utils.h"
#include "gdal.h"

#include "geo/raster/map.h"


inline static MatrixTable * AS_VectorRasterSample(cTMap * map, ShapeFile * shapefile)
{

    //first check some conditions for stability
    if(map == nullptr)
    {
        LISEMS_ERROR(QString("Could not complete vector raster sampling operation for null raster object "));
        throw 1;
    }

    if(shapefile == nullptr)
    {

        LISEMS_ERROR(QString("Could not complete vector raster sampling operation for null shapefile object "));
        throw 1;
    }


    //if needed, warn about CRS (if they dont match, we dont convert here manually)
    GeoProjection p1 = map->GetProjection();
    GeoProjection p2 = shapefile->GetProjection();
    if(!p1.IsEqualTo(p2))
    {
        LISEMS_WARNING(QString("CRS for raster and shapefile are not equal (") + p1.GetName() + " and "  + p2.GetName() + ")");
        LISEMS_WARNING(QString("Conversion of coordinates is not done here, use VectorWarp or RasterWarp instead!"));
    }

    //oke, we have a raster and a shapefile with features
    //first create table with required dimensions

    MatrixTable * ret = new MatrixTable();
    if(shapefile->GetLayerCount() >0)
    {
        ShapeLayer * l = shapefile->GetLayer(0);
        int count = l->GetFeatureCount();
        ret->SetSize(count,1);
        ret->SetColumnType(0,TABLE_TYPE_FLOAT64);
        ret->SetColumnTypesActive(true);
        //iterate over all features

        for(int i = 0; i < count; i++)
        {
            //get the centroid location
            LSMVector2 centroid = l->GetFeature(i)->GetCentroid();

            //sample value
            float val = map->Sample(centroid.x,centroid.y);

            if(pcr::isMV(val))
            {
                val = NAN;
            }
            ret->SetValue(i,0,val);
        }

    }

    //return the table

    return ret;
}

#endif // VECTORRASTERSAMPLE_H
