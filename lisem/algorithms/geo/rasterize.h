#ifndef RASTERIZE_H
#define RASTERIZE_H

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



inline void Rasterize(cTMap * m,ShapeFile * f,QString attribute, bool burn = false, float burn_value = 1.0, bool invert = false, bool all_touched = false, bool add = false)
{
    std::vector<char*> args;


    if(burn)
    {
        args.push_back(strdup(QString("-burn").toStdString().c_str()));
        args.push_back(strdup(QString(QString::number(burn_value)).toStdString().c_str()));
    }else {
        args.push_back(strdup(QString("-a").toStdString().c_str()));
        args.push_back(strdup(QString(attribute).toStdString().c_str()));
    }
    if(invert)
    {
        args.push_back(strdup(QString("-i").toStdString().c_str()));
    }
    if(add)
    {
        args.push_back(strdup(QString("-add").toStdString().c_str()));
    }
    if(all_touched)
    {
        args.push_back(strdup(QString("-at").toStdString().c_str()));
    }

    //args.push_back(strdup(QString("-a_nodata").toStdString().c_str()));
    //args.push_back(strdup(QString(QString::number(0.0)).toStdString().c_str()));


    //args.push_back(strdup(QString("-ot").toStdString().c_str()));
    //args.push_back(strdup(QString("Float32").toStdString().c_str()));

    args.push_back(nullptr);

    const char *pszDriverName = "Memory";
    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get vector file driver for memory");
        throw 1;
    }

    GDALDataset *poDS;

    poDS = poDriver->Create("temp1", 0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL )
    {
        LISEM_ERROR("Could not write to file: temp1 using driver: memory");
        return;
    }

    //std::cout << "export shapes to gdal" << std::endl;
    ToGDALDataset(poDS,f);

    //char * refc;
    //std::cout << poDS->GetSpatialRef() << std::endl;
    //poDS->GetLayer(0)->GetSpatialRef()->exportToWkt(&refc);
    //std::cout << refc << std::endl;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get raster file driver for memory");
        throw 1;
    }

    int const nrRows{m->nrRows()};
    int const nrCols{m->nrCols()};
    int const nrBands{1};
    GDALDataset * d = poDriver->Create("temp2",
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

    ToGDALDataset(d,m);


    GDALDatasetH raster_out = d->ToHandle(d);
    GDALDatasetH vector_in = poDS->ToHandle(poDS);

    GDALRasterizeOptions *psOptions;
    psOptions = GDALRasterizeOptionsNew(args.data(),nullptr);

    int error = 0;
    GDALDatasetH output = GDALRasterize(nullptr,raster_out,vector_in,psOptions,&error);


    if(output == nullptr)
    {
        LISEMS_DEBUG("Error during rasterization process " + QString::number(error));
        throw 1;
    }

    auto band = d->GetRasterBand(1);
    band->SetNoDataValue(-FLT_MAX);
    if(band->RasterIO(GF_Read, 0, 0, nrCols, nrRows, m->data[0],
            nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
        throw 1;
    }

    GDALRasterizeOptionsFree(psOptions);
    GDALClose( (GDALDataset*) d );
    GDALClose( (GDALDataset*) poDS);

    for(int i = 0; i < args.size(); i++)
    {
        free(args.at(i));
    }
}

inline cTMap * AS_Rasterize(cTMap * m,ShapeFile * f,QString attribute, bool burn = false, float burn_value = 1.0, bool invert = false, bool all_touched = false,bool add = false)
{
    cTMap * m2 = m->GetCopy();

    Rasterize(m2,f,attribute,burn,burn_value,invert,all_touched,add);

    return m2;
}


#endif // RASTERIZE_H
