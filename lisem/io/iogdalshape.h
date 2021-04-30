#ifndef IOGDALSHAPE_H
#define IOGDALSHAPE_H

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

inline ShapeFile * LoadVector(QString path)
{


    GDALDataset * poDS(GDALDataset::Open( path.toStdString().c_str(), GDAL_OF_VECTOR));

    GDALDriver*  d = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if(d == NULL) {

        LISEM_ERROR("Could not find shapefile Driver");
        LISEMS_ERROR("Could not find shapefile Driver");
    }

    if( poDS == nullptr )
    {
        LISEM_ERROR("Could not load vector file: " + path);
        return new ShapeFile();
    }

    ShapeFile * ret = FromGDALDataset(poDS);
    ret->AS_FileName = path;

    const OGRSpatialReference * ref = poDS->GetLayer(0)->GetSpatialRef();
    if(ref != nullptr)
    {
        ret->SetProjection(ref);
    }

    ret->GetAndCalcBoundingBox();

    GDALClose(poDS);


    return ret;

}

inline void SaveVector(ShapeFile * sf,QString path, QString format = "ESRI Shapefile")
{

    QFileInfo fi(path);
    if(!fi.dir().exists())
    {
        fi.dir().mkdir(fi.dir().path());
    }


    std::cout << "save vector "  << path.toStdString() <<std::endl;



    const char *pszDriverName = format.toStdString().c_str();
    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get vector file driver: " + format)
        return;
    }

    GDALDataset *poDS;

    poDS = poDriver->Create( path.toStdString().c_str(),0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL )
    {
        LISEM_ERROR("Could not write to file: " + path + " using driver: " + format)
        return;
    }

    ToGDALDataset(poDS,sf);

    GDALClose( poDS );


}

#endif // IOGDALSHAPE_H
