#ifndef VECTORIZE_H
#define VECTORIZE_H

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
#include "gdal_alg.h"

#include "geo/raster/map.h"



inline ShapeFile * Vectorize(cTMap * m,QString attributename = "value", bool connect_diagonal = false)
{

    CPLStringList sl;
    sl.AddString("8");
    //sl.AddNameValue("burn","1.0");

    GDALDriver *poDriver;
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


    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get raster file driver for memory");
        return new ShapeFile();
    }

    int const nrRows{m->nrRows()};
    int const nrCols{m->nrCols()};
    int const nrBands{1};
    GDALDataset * d = poDriver->Create("temp2",
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

    ToGDALDataset(d,m);

    OGRSpatialReference * ref = new OGRSpatialReference(m->GetProjection().ToGDALRef());
    OGRLayer * l = poDS->CreateLayer("Vectorize",ref,wkbPolygon,nullptr);

    if(d->GetRasterBand(1) != nullptr)
    {
        GDALFPolygonize(d->GetRasterBand(1),nullptr,l,0,sl.List(),nullptr,0);
    }else {
        LISEMS_ERROR("No rasterband to vectorize");
    }

    ShapeFile * res = FromGDALDataset(poDS);

    delete ref;
    GDALClose( (GDALDataset*) d );
    GDALClose( (GDALDataset*) poDS);

    return res;
}


inline ShapeFile * VectorizePoints(cTMap * m,QString attributename = "id", bool connect_diagonal = false)
{
    if(m == nullptr)
    {
        LISEM_ERROR("Map is null for vectorize to points operation");
        return new ShapeFile();

    }
    ShapeFile * ret = new ShapeFile();
    ret->SetProjection(m->GetProjection());
    ShapeLayer * lay = new ShapeLayer();
    ret->AddLayer(lay);

    lay->AddAttribute(attributename,ATTRIBUTE_TYPE_FLOAT64);
    lay->AddAttribute("POS_X",ATTRIBUTE_TYPE_FLOAT64);
    lay->AddAttribute("POS_Y",ATTRIBUTE_TYPE_FLOAT64);

    FOR_ROW_COL_MV(m)
    {
        float val = m->data[r][c];

        Feature * f = new Feature();
        LSMPoint * p = new LSMPoint(m->west() + (0.5f+c) * m->cellSizeX(),m->north() + (0.5f+r) * m->cellSizeY());
        f->AddShape(p);
        int index = lay->AddFeature(f);
        lay->SetFeatureAttribute(index,"POS_X",p->GetX());
        lay->SetFeatureAttribute(index,"POS_Y",p->GetY());
        lay->SetFeatureAttribute(index,attributename,val);
    }

    return ret;
}

inline ShapeFile* AS_VectorizePoints(cTMap * m,QString attributename = "id")
{

    return VectorizePoints(m,attributename);


}

inline ShapeFile* AS_Vectorize(cTMap * m,QString attributename = "value", bool connect_diagonal = false)
{

    return Vectorize(m,attributename,connect_diagonal);


}


#endif // VECTORIZE_H
