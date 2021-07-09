#ifndef RASTERCONTOUR_H
#define RASTERCONTOUR_H

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

#include "raster/rasternetwork.h"
#include "geo/raster/map.h"


inline ShapeFile* RasterContour(cTMap * m,float interval,float start = 0.0, QString attributename = "value")
{
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
    std::cout << "rcontour 0 " << std::endl;
    OGRSpatialReference * ref = new OGRSpatialReference(m->GetProjection().ToGDALRef());
    std::cout << "rcontour 1 " << ref << " " <<  poDS <<  std::endl;

    if(m->GetProjection().IsGeneric())
    {
        std::cout << "projection is generic" << std::endl;
        ref = nullptr;
    }
    std::cout << "can create layer "<< poDS->TestCapability(ODsCCreateLayer) << std::endl;
    OGRLayer * l = poDS->CreateLayer("Vectorize",nullptr,wkbUnknown,nullptr);

    std::cout << "rcontour 1 " << std::endl;
    if(d->GetRasterBand(1) != nullptr)
    {
        GDALContourGenerate(d->GetRasterBand(1)->ToHandle(d->GetRasterBand(1)),interval,0,0,nullptr,0,0,l->ToHandle(l),0,1,nullptr,nullptr);
    }else {
        LISEMS_ERROR("No rasterband to create contours from");
    }

    std::cout << "rcontour 2 " << std::endl;
    ShapeFile * res = FromGDALDataset(poDS);

    std::cout << "rcontour 4 " << std::endl;
    delete ref;
    GDALClose( (GDALDataset*) d );
    GDALClose( (GDALDataset*) poDS);

    return res;

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval)
{

    return RasterContour(m,interval,0.0,QString("value"));

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval, std::string attributename = "value")
{

    return RasterContour(m,interval,0.0,QString(attributename.c_str()));

}

inline ShapeFile * AS_RasterContour(cTMap * m, float interval, float start, QString attributename = "value")
{

    return RasterContour(m,interval,0.0,QString(attributename));

}



inline ShapeFile * AS_DrainageNetworkToShapes(cTMap * ldd)
{
    if(ldd == nullptr)
    {

            LISEM_ERROR("Could not create vector file from drainage network");
            return new ShapeFile();


    }



    ShapeFile * ret_sf = new ShapeFile();
    ShapeLayer * ret_layer = new ShapeLayer();
    ret_layer->AddAttribute("Value",ATTRIBUTE_TYPE_INT32);

    ret_sf->AddLayer(ret_layer);
    ret_sf->SetProjection(ldd->GetProjection());



    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;
    float dist[LDD_DIR_LENGTH] = LDD_DIST_LIST;

    FOR_ROW_COL_MV(ldd)
    {
        int val = (int) ldd->data[r][c];
        if(val > 0 && val < 10 && val != 5)
        {
            double x1 = ldd->west() + ldd->cellSizeX() * (float(c));
            double y1 = ldd->north()+ ldd->cellSizeY() * (float(r));
            double x2 = ldd->west() + ldd->cellSizeX() * (float(c+dx[val]));
            double y2 = ldd->north()+ ldd->cellSizeY() * (float(r+dy[val]));
            Feature * f = new Feature();
            LSMLine * s = new LSMLine();
            s->AddVertex(x1,y1);
            s->AddVertex(x2,y2);
            f->AddShape(s);
            ret_layer->AddFeature(f);

        }
    }

    return ret_sf;

}
#endif // RASTERCONTOUR_H
