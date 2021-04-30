#ifndef RASTERSTACK_H
#define RASTERSTACK_H

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
#include "vector"
#include "geo/raster/map.h"

inline static bool RasterFileStack(QString target,QString format, QString file1, QString file2)
{

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(
        format.toLatin1().constData());
    if(!driver) {
        LISEM_ERROR(QString("Format driver %1 not available.").arg(
            target.toStdString().c_str()));
        throw 1;
    }

    char** metadata{driver->GetMetadata()};
    bool driverSupportsCreate{CSLFetchBoolean(metadata, GDAL_DCAP_CREATE,
        FALSE) != FALSE};


    // Open raster dataset and obtain some properties.
    GDALDataset* dataset1 = static_cast<GDALDataset*>(GDALOpen(
        file1.toStdString().c_str(), GA_ReadOnly));
    if(!dataset1) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(file1));
        throw 1;
    }

    // Open raster dataset and obtain some properties.
    GDALDataset* dataset2 = static_cast<GDALDataset*>(GDALOpen(
        file2.toStdString().c_str(), GA_ReadOnly));
    if(!dataset2) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(file2));
        throw 1;
    }



    int nr_bands1 = dataset1->GetRasterCount();
    if(nr_bands1 == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(file1));
        throw 1;
    }
    int nr_bands2 = dataset2->GetRasterCount();
    if(nr_bands2 == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(file2));
        throw 1;
    }

    const OGRSpatialReference * ref1 = dataset1->GetSpatialRef();
    const OGRSpatialReference * ref2 = dataset2->GetSpatialRef();
    char * refc;
    if(!(ref1 == nullptr && ref1 == nullptr))
    {

    }

    int nrRows = 0;
    int nrCols = 0;
    int nrBands = nr_bands1 + nr_bands2;

    for(int i = 0; i < nr_bands1; i++)
    {
        GDALRasterBand* band{dataset1->GetRasterBand(i+1)};
        if(i == 0)
        {
            nrRows = band->GetYSize();
            nrCols = band->GetXSize();
        }

        if(!(band->GetYSize() == nrRows && band->GetXSize() == nrCols))
        {
            LISEM_ERROR(QString("Size of bands are not identical"));
            throw 1;
        }
    }
    for(int i = 0; i < nr_bands2; i++)
    {
        GDALRasterBand* band{dataset2->GetRasterBand(i+1)};
        if(!(band->GetYSize() == nrRows && band->GetXSize() == nrCols))
        {
            LISEM_ERROR(QString("Size of bands are not identical"));
            throw 1;
        }
    }


    QString projection1{dataset1->GetProjectionRef()};
    QString projection2{dataset2->GetProjectionRef()};


    GDALDataset * d = driver->Create(target.toStdString().c_str(),
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

    //d->SetGeoTransform(dataset1->GetGeoTransform());
    d->SetMetadata(dataset1->GetMetadata());
    d->SetProjection(dataset1->GetProjectionRef());

    for(int i = 0; i < nr_bands1; i++)
    {
        GDALRasterBand* band{dataset1->GetRasterBand(i+1)};
        //d->
    }
    for(int i = 0; i < nr_bands2; i++)
    {
        GDALRasterBand* band{dataset2->GetRasterBand(i+1)};

    }



}

inline static void AS_RasterFileStack(QString target, QString file1, QString file2)
{



}


#endif // RASTERSTACK_H
