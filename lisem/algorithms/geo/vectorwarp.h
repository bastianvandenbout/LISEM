#ifndef VECTORWARP_H
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


inline static GeoProjection*AS_GetCRS(ShapeFile*s)
{
    return new GeoProjection(s->GetProjection());
}

inline static ShapeFile *AS_SetCRS(ShapeFile *s,GeoProjection*p)
{
    ShapeFile * s2 = new ShapeFile();
    s2->CopyFrom(s);
    s2->SetProjection(*p);

    return s2;

}

inline static BoundingBox AS_GetRegion(ShapeFile *s)
{
    return s->GetBoundingBox();
}


//using ogr2ogr or VectorTranslate from GDAL

inline static ShapeFile * VectorWarp(ShapeFile * s_in,GeoProjection p_out,bool segmentize = false,double dist_max = 1.0, bool simplify = false, bool dist_threshold = 1.0)
{
    if(s_in == nullptr)
    {
        LISEMS_ERROR(QString("Could not complete vector warping operation for null object ") + (simplify?" with simplification": "") + (segmentize?" with segmentation": ""));
        throw 1;
    }


    /*CPLStringList sl;
    sl.AddString("overwrite");
    //sl.AddNameValue("s",interpolation.toStdString().c_str());
    if(segmentize)
    {
        sl.AddNameValue("segmentize",QString::number(dist_max).toStdString().c_str());
    }if(simplify)
    {
        sl.AddNameValue("simplify",QString::number(dist_threshold).toStdString().c_str());
    }
    if(!(s_in->GetProjection().IsEqualTo(p_out)))
    {
        std::cout << "add projection" << std::endl;
        sl.AddNameValue("t_srs",QString(QString("EPSG:") + QString::number(p_out.GetEPSG())).toStdString().c_str());
    }
    //sl.AddNameValue("simplify",interpolation.toStdString().c_str());

*/

    std::vector<char*> args;
    if(segmentize)
    {
        args.push_back(strdup(QString("-segmentize").toStdString().c_str()));
        args.push_back(strdup(QString(QString::number(dist_max)).toStdString().c_str()));
    }if(simplify)
    {
        args.push_back(strdup(QString("-simplify").toStdString().c_str()));
        args.push_back(strdup(QString(QString::number(dist_threshold)).toStdString().c_str()));
    }
    //if(!(s_in->GetProjection().IsEqualTo(p_out)))
    {
        args.push_back(strdup(QString("-t_srs").toStdString().c_str()));
        args.push_back(strdup(QString(QString(QString("EPSG:") + QString::number(p_out.GetEPSG()))).toStdString().c_str()));
        std::cout << "add projection" << std::endl;
    }
    args.push_back(nullptr);
    GDALVectorTranslateOptions *warpoptions = GDALVectorTranslateOptionsNew(args.data(),nullptr);

    GDALDriver *poDriver;

    std::vector<GDALDatasetH> SourceVectors;
    std::vector<GDALDataset *> SourceVectorsP;
    const char *pszDriverName = "Memory";

    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get vector file driver for memory");
        return new ShapeFile();
    }

    GDALDataset *poDS;

    poDS = poDriver->Create("temp1", 0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL )
    {
        LISEM_ERROR("Could not write to file: temp1 using driver: memory");
        return new ShapeFile();
    }

    GDALDataset * d = poDriver->Create("temp1", 0, 0, 0, GDT_Unknown, NULL );
    if( poDS == NULL )
    {
        LISEM_ERROR("Could not write to file: temp1 using driver: memory");
        return new ShapeFile();
    }

    GDALDatasetH TargetVector = d->ToHandle(d);


    ToGDALDataset(poDS,s_in);
    SourceVectorsP.push_back(poDS);
    SourceVectors.push_back(poDS->ToHandle(poDS));


    int error = 0;
    GDALDatasetH out = GDALVectorTranslate(nullptr,TargetVector,1,SourceVectors.data(),warpoptions,&error);

    ShapeFile * target = FromGDALDataset(d);

    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceVectors.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceVectorsP.at(i));
    }

    GDALVectorTranslateOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR(QString("Could not complete vector warping operation") + (simplify?" with simplification": "") + (segmentize?" with segmentation": ""));
        throw 1;
    }

    return target;

}

inline static ShapeFile * AS_VectorWarp(ShapeFile * in_s,GeoProjection * out)
{
    return VectorWarp(in_s,*out);
}



#endif // VECTORWARP_H
