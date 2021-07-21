#pragma once

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
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "gdal_alg.h"


inline static std::vector<cTMap*> AS_RasterOrthoRectify(std::vector<cTMap*> inputmaps, cTMap * dem )
{



    for(int i = 0; i < inputmaps.size(); i++)
    {
        if(inputmaps.at(i)->nrCols() != dem->nrCols() || inputmaps.at(i)->nrRows() != dem->nrRows())
        {
            LISEMS_ERROR("Size of input rasters do not match");
            throw 1;
        }
    }

    QString interpolation = "linear";

    CPLStringList sl;
    sl.AddString("overwrite");
    sl.AddNameValue("s",interpolation.toStdString().c_str());
    sl.AddNameValue("to","RPC_DEM=/vsimem/tempdem.tif");



    GDALWarpAppOptions *warpoptions = GDALWarpAppOptionsNew(sl.List(),nullptr);

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEMS_ERROR("Could not get raster file driver for memory");
        throw 1;
    }

        std::cout << "Creating Source Datasets" << std::endl;
     std::vector<GDALDatasetH> SourceRasters;
     std::vector<GDALDataset *> SourceRastersP;

     cTMap * mtemp = inputmaps.at(0);

     int const snrRows{mtemp->nrRows()};
     int const snrCols{mtemp->nrCols()};
     int const snrBands{inputmaps.size()};
     GDALDataset * d2 = poDriver->Create("temp2",
                   snrCols, snrRows, snrBands, GDT_Float32, nullptr);
     SourceRastersP.push_back(d2);
     SourceRasters.push_back(d2->ToHandle(d2));


     std::cout << "Uploading Source Datasets" << std::endl;

     for(int i = 0; i < inputmaps.size(); i++)
     {
         QString name = "tempin_"+ QString::number(i);
         cTMap * m = inputmaps.at(i);
         ToGDALDataset(d2,m,i+1);
     }


     std::cout << "creating transformer" << std::endl;


     double transformation[]{
         dem->data.west(),
         dem->data.cell_sizeX(),
         0.0,
         dem->data.north(),
         0.0,
         dem->data.cell_sizeY()};

    GDALDataset * d = poDriver->Create("temp2",
                   snrCols, snrRows, snrBands, GDT_Float32, nullptr);

    d->SetGeoTransform(transformation);
    d->SetProjection(dem->GetProjection().GetWKT().toStdString().c_str());


    std::cout << "making output bands" << std::endl;


    std::vector<cTMap*> targets;

    for(int i = 0; i< inputmaps.size(); i++)
    {
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};

        MaskedRaster<float> raster_data(dem->nrRows(), dem->nrCols(), north,west , cell_size, cell_sizeY );
        cTMap *target = new cTMap(std::move(raster_data),dem->GetProjection().GetWKT(),"");
        targets.push_back(target);
    }

    GDALDatasetH TargetRaster = d->ToHandle(d);



    GDALDataset * ddem = poDriver->Create("/vsimem/tempdem.tif",
                  dem->nrCols(), dem->nrRows(), 1, GDT_Float32, nullptr);
    ToGDALDataset(ddem,dem);


   std::cout << "actual warping" << std::endl;
    int error = 0;

    GDALDatasetH out = GDALWarp(nullptr,TargetRaster,1,SourceRasters.data(),warpoptions,&error);

    std::cout << "Loading gdal results into maps" << std::endl;

    for(int i = 0; i < inputmaps.size(); i++)
    {
        auto band = d->GetRasterBand(i+1);
        band->SetNoDataValue(-FLT_MAX);

        if(band->RasterIO(GF_Read, 0, 0, dem->nrRows(), dem->nrCols(), targets.at(i)->data[0],
                dem->nrRows(), dem->nrCols(), GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
            throw 1;
        }
    }


   std::cout << "rasterwarp6" << std::endl;
    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceRasters.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceRastersP.at(i));
    }

    GDALWarpAppOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR("Could not complete warping operation");
        throw 1;
    }

    GDALClose((GDALDataset*) out);
    GDALClose((GDALDataset*) ddem);
    VSIUnlink( "/vsimem/tempdem.tif" );

    return targets;
}

inline static std::vector<cTMap*> AS_RasterRegister(std::vector<cTMap*> inputmaps,  std::vector<LSMVector2>inputppix, std::vector<LSMVector2> inputpworld)
{
    cTMap * dem = inputmaps.at(0);

    for(int i = 0; i < inputmaps.size(); i++)
    {
        if(inputmaps.at(i)->nrCols() != dem->nrCols() || inputmaps.at(i)->nrRows() != dem->nrRows())
        {
            LISEMS_ERROR("Size of input rasters do not match");
            throw 1;
        }
    }

    QString interpolation = "linear";

    CPLStringList sl;
    sl.AddString("overwrite");
    sl.AddNameValue("s",interpolation.toStdString().c_str());

    GDALWarpAppOptions *warpoptions = GDALWarpAppOptionsNew(sl.List(),nullptr);

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEMS_ERROR("Could not get raster file driver for memory");
        throw 1;
    }

        std::cout << "Creating Source Datasets" << std::endl;
     std::vector<GDALDatasetH> SourceRasters;
     std::vector<GDALDataset *> SourceRastersP;

     cTMap * mtemp = inputmaps.at(0);

     int const snrRows{mtemp->nrRows()};
     int const snrCols{mtemp->nrCols()};
     int const snrBands{inputmaps.size()};
     GDALDataset * d2 = poDriver->Create("temp2",
                   snrCols, snrRows, snrBands, GDT_Float32, nullptr);
     SourceRastersP.push_back(d2);
     SourceRasters.push_back(d2->ToHandle(d2));


     std::cout << "Uploading Source Datasets" << std::endl;

     for(int i = 0; i < inputmaps.size(); i++)
     {
         QString name = "tempin_"+ QString::number(i);
         cTMap * m = inputmaps.at(i);
         ToGDALDataset(d2,m,i+1);
     }

     std::vector<GDAL_GCP> gcps;
     for(int i = 0; i < inputppix.size(); i++)
     {
         GDAL_GCP p;
         p.dfGCPPixel = inputppix.at(i).x;
         p.dfGCPLine  = inputppix.at(i).y;
         p.dfGCPX = inputpworld.at(i).x;
         p.dfGCPY =inputpworld.at(i).y;
         p.pszId = (char *) malloc( 10*sizeof(char) );
         p.pszInfo = (char *) malloc( 10*sizeof(char) );
         QString n = QString::number(i).left(10);
         strcpy(p.pszId,n.toStdString().c_str());
         strcpy(p.pszInfo,n.toStdString().c_str());
     }

     GeoProjection p = dem->GetProjection();

     d2->SetGCPs(inputppix.size(), gcps.data(),p.GetOGRRefP());


     std::cout << "creating transformer" << std::endl;


     double transformation[]{
         dem->data.west(),
         dem->data.cell_sizeX(),
         0.0,
         dem->data.north(),
         0.0,
         dem->data.cell_sizeY()};

    GDALDataset * d = poDriver->Create("temp2",
                   snrCols, snrRows, snrBands, GDT_Float32, nullptr);

    d->SetGeoTransform(transformation);
    d->SetProjection(dem->GetProjection().GetWKT().toStdString().c_str());


    std::cout << "making output bands" << std::endl;


    std::vector<cTMap *> targets;

    for(int i = 0; i< inputmaps.size(); i++)
    {
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};

        MaskedRaster<float> raster_data(dem->nrRows(), dem->nrCols(), north,west , cell_size, cell_sizeY );
        cTMap *target = new cTMap(std::move(raster_data),dem->GetProjection().GetWKT(),"");
        targets.push_back(target);
    }

    GDALDatasetH TargetRaster = d->ToHandle(d);



    GDALDataset * ddem = poDriver->Create("/vsimem/tempdem.tif",
                  dem->nrCols(), dem->nrRows(), 1, GDT_Float32, nullptr);
    ToGDALDataset(ddem,dem);


   std::cout << "actual warping" << std::endl;
    int error = 0;

    GDALDatasetH out = GDALWarp(nullptr,TargetRaster,1,SourceRasters.data(),warpoptions,&error);

    std::cout << "Loading gdal results into maps" << std::endl;

    for(int i = 0; i < inputmaps.size(); i++)
    {
        auto band = d->GetRasterBand(i+1);
        band->SetNoDataValue(-FLT_MAX);

        if(band->RasterIO(GF_Read, 0, 0, dem->nrRows(), dem->nrCols(), targets.at(i)->data[0],
                dem->nrRows(), dem->nrCols(), GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
            throw 1;
        }
    }


   std::cout << "rasterwarp6" << std::endl;
    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceRasters.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceRastersP.at(i));
    }

    GDALWarpAppOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR("Could not complete warping operation");
        throw 1;
    }

    GDALClose((GDALDataset*) out);

    return targets;


}

inline static void RasterWarpMultiple(QList<cTMap *> targets,  QList<QList<cTMap *>> inputmaps, QString interpolation)
{
    /*std::cout << "rasterwarp" << std::endl;
    CPLStringList sl;
    sl.AddString("overwrite");
    sl.AddNameValue("s",interpolation.toStdString().c_str());

    GDALWarpAppOptions *warpoptions = GDALWarpAppOptionsNew(sl.List(),nullptr);

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get raster file driver for memory");
        return;
    }

    int const nrRows{target->nrRows()};
    int const nrCols{target->nrCols()};
    int const nrBands{1};
    GDALDataset * d = poDriver->Create("temp2",
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

       std::cout << "rasterwarp2" << std::endl;

    ToGDALDataset(d,target);
    GDALDatasetH TargetRaster = d->ToHandle(d);

       std::cout << "rasterwarp3" << std::endl;
    std::vector<GDALDatasetH> SourceRasters;
    std::vector<GDALDataset *> SourceRastersP;
    for(int i = 0; i < inputmaps.length(); i++)
    {
        QString name = "tempin_"+ QString::number(i);
        cTMap * m = inputmaps.at(i);

        int const snrRows{m->nrRows()};
        int const snrCols{m->nrCols()};
        int const snrBands{1};
        GDALDataset * d2 = poDriver->Create("temp2",
                      snrCols, snrRows, snrBands, GDT_Float32, nullptr);

        ToGDALDataset(d2,m);
        SourceRastersP.push_back(d2);
        SourceRasters.push_back(d2->ToHandle(d2));


    }
   std::cout << "rasterwarp4" << std::endl;
    int error = 0;

    GDALDatasetH out = GDALWarp(nullptr,TargetRaster,SourceRasters.size(),SourceRasters.data(),warpoptions,&error);
   std::cout << "rasterwarp5" << std::endl;
    auto band = d->GetRasterBand(1);
    band->SetNoDataValue(-FLT_MAX);
    if(band->RasterIO(GF_Read, 0, 0, nrCols, nrRows, target->data[0],
            nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
        throw 1;
    }
   std::cout << "rasterwarp6" << std::endl;
    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceRasters.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceRastersP.at(i));
    }

    GDALWarpAppOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR("Could not complete warping operation");
        throw 1;
    }*/

}



inline static void RasterWarp(cTMap * target, std::vector<cTMap *> inputmaps, QString interpolation)
{

    std::cout << "rasterwarp" << std::endl;
    CPLStringList sl;
    sl.AddString("overwrite");
    sl.AddNameValue("s",interpolation.toStdString().c_str());

    GDALWarpAppOptions *warpoptions = GDALWarpAppOptionsNew(sl.List(),nullptr);

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEM_ERROR("Could not get raster file driver for memory");
        return;
    }

    int const nrRows{target->nrRows()};
    int const nrCols{target->nrCols()};
    int const nrBands{1};
    GDALDataset * d = poDriver->Create("temp2",
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);

       std::cout << "rasterwarp2" << std::endl;

    ToGDALDataset(d,target);
    GDALDatasetH TargetRaster = d->ToHandle(d);


       std::cout << "rasterwarp3" << std::endl;
    std::vector<GDALDatasetH> SourceRasters;
    std::vector<GDALDataset *> SourceRastersP;
    for(int i = 0; i < inputmaps.size(); i++)
    {
        QString name = "tempin_"+ QString::number(i);
        cTMap * m = inputmaps.at(i);

        int const snrRows{m->nrRows()};
        int const snrCols{m->nrCols()};
        int const snrBands{1};
        GDALDataset * d2 = poDriver->Create("temp2",
                      snrCols, snrRows, snrBands, GDT_Float32, nullptr);

        ToGDALDataset(d2,m);
        SourceRastersP.push_back(d2);
        SourceRasters.push_back(d2->ToHandle(d2));


    }
   std::cout << "rasterwarp4" << std::endl;
    int error = 0;

    GDALDatasetH out = GDALWarp(nullptr,TargetRaster,SourceRasters.size(),SourceRasters.data(),warpoptions,&error);
   std::cout << "rasterwarp5" << std::endl;
    auto band = d->GetRasterBand(1);
    band->SetNoDataValue(-FLT_MAX);
    if(band->RasterIO(GF_Read, 0, 0, nrCols, nrRows, target->data[0],
            nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
        throw 1;
    }
   std::cout << "rasterwarp6" << std::endl;
    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceRasters.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceRastersP.at(i));
    }

    GDALWarpAppOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR("Could not complete warping operation");
        throw 1;
    }

}

inline static cTMap * AS_RasterWarp(cTMap * target, std::vector<cTMap*> inputmaps, QString interpolation)
{

    cTMap *res = target->GetCopy();


    RasterWarp(res,inputmaps,interpolation);

    return res;

}


inline static cTMap * AS_RasterWarp(cTMap * target, cTMap  * inputmaps, QString interpolation)
{

    cTMap *res = target->GetCopy();

    RasterWarp(res,{inputmaps},interpolation);

    return res;

}

inline static std::vector<cTMap *> RasterProject(std::vector<cTMap *> inputmaps, GeoProjection * p, QString interpolation)
{

    std::cout << "rasterproject, similar to warp, but target raster is determined by algorithm" << std::endl;
    CPLStringList sl;
    sl.AddString("overwrite");
    sl.AddNameValue("s",interpolation.toStdString().c_str());


    GDALWarpAppOptions *warpoptions = GDALWarpAppOptionsNew(sl.List(),nullptr);

    GDALDriver *poDriver;

    poDriver = GetGDALDriverManager()->GetDriverByName(
        "MEM");
    if( poDriver == NULL )
    {
        LISEMS_ERROR("Could not get raster file driver for memory");
        throw 1;
    }

        std::cout << "Creating Source Datasets" << std::endl;
     std::vector<GDALDatasetH> SourceRasters;
     std::vector<GDALDataset *> SourceRastersP;

     cTMap * mtemp = inputmaps.at(0);

     int const snrRows{mtemp->nrRows()};
     int const snrCols{mtemp->nrCols()};
     int const snrBands{inputmaps.size()};
     GDALDataset * d2 = poDriver->Create("temp2",
                   snrCols, snrRows, snrBands, GDT_Float32, nullptr);
     SourceRastersP.push_back(d2);
     SourceRasters.push_back(d2->ToHandle(d2));


     std::cout << "Uploading Source Datasets" << std::endl;

     for(int i = 0; i < inputmaps.size(); i++)
     {
         QString name = "tempin_"+ QString::number(i);
         cTMap * m = inputmaps.at(i);
         ToGDALDataset(d2,m,i+1);
     }


     std::cout << "creating transformer" << std::endl;

    // Create a transformer that maps from source pixel/line coordinates
    // to destination georeferenced coordinates (not destination
    // pixel line).  We do that by omitting the destination dataset
    // handle (setting it to NULL).
    void *hTransformArg;
    hTransformArg =
        GDALCreateGenImgProjTransformer( SourceRasters.at(0), NULL, NULL, p->GetWKT().toStdString().c_str(),
                                         FALSE, 0, 1 );
    CPLAssert( hTransformArg != NULL );


    std::cout << "creating output definition" << std::endl;


    // Get approximate output georeferenced bounds and resolution for file.
    double transformation[6];
    int nPixels=0, nLines=0;
    CPLErr eErr;
    eErr = GDALSuggestedWarpOutput( SourceRasters.at(0),
                                    GDALGenImgProjTransform, hTransformArg,
                                    transformation, &nPixels, &nLines );
    CPLAssert( eErr == CE_None );
    GDALDestroyGenImgProjTransformer( hTransformArg );

    std::cout << "creating output gdal dataset " << nPixels << " " << nLines << " " << std::endl;

    GDALDataset * d = poDriver->Create("temp2",
                  nPixels, nLines, inputmaps.size(), GDT_Float32, nullptr);

    d->SetGeoTransform(transformation);
    d->SetProjection(p->GetWKT().toStdString().c_str());


    std::cout << "making output bands" << std::endl;


    std::vector<cTMap *> targets;

    for(int i = 0; i< inputmaps.size(); i++)
    {
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};

        MaskedRaster<float> raster_data(nLines, nPixels, north,west , cell_size, cell_sizeY );
        cTMap *target = new cTMap(std::move(raster_data),p->GetWKT(),"");
        targets.push_back(target);
    }

    GDALDatasetH TargetRaster = d->ToHandle(d);


   std::cout << "actual warping" << std::endl;
    int error = 0;

    GDALDatasetH out = GDALWarp(nullptr,TargetRaster,1,SourceRasters.data(),warpoptions,&error);

    std::cout << "Loading gdal results into maps" << std::endl;

    for(int i = 0; i < inputmaps.size(); i++)
    {
        auto band = d->GetRasterBand(i+1);
        band->SetNoDataValue(-FLT_MAX);

        if(band->RasterIO(GF_Read, 0, 0, nPixels, nLines, targets.at(i)->data[0],
                nPixels, nLines, GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("temp2"));
            throw 1;
        }
    }


   std::cout << "rasterwarp6" << std::endl;
    GDALClose( (GDALDataset*) d );
    for(int i = 0; i < SourceRasters.size(); i++)
    {
        GDALClose( (GDALDataset*) SourceRastersP.at(i));
    }

    GDALWarpAppOptionsFree(warpoptions);


    if(out == nullptr)
    {
        LISEMS_ERROR("Could not complete warping operation");
        throw 1;
    }

    return targets;

}

static cTMap * AS_RasterProject(cTMap * input, GeoProjection * proj, QString interpolation)
{


    std::vector<cTMap *> bands = RasterProject({input},proj,interpolation);

    return bands.at(0);
}

static std::vector<cTMap *> AS_RasterProjectMultiImage(std::vector<cTMap *> inputs, GeoProjection * proj)
{

    //TODO
    return {nullptr};
}

static std::vector<cTMap *> AS_RasterProject(std::vector<cTMap *> ins, GeoProjection * proj, QString interpolation)
{

    if(ins.size() == 0)
    {
        {
            LISEMS_ERROR("Could not project without input");
            throw 1;
        }
    }

    std::vector<cTMap *> bands = RasterProject(ins,proj,interpolation);
    return bands;
}




inline static std::vector<cTMap *> AS_RasterlSubSection(std::vector<cTMap *> maps1, int r0, int c0, int rows, int cols)
{
    std::vector<cTMap *> ret;

    for(int i = 0; i < maps1.size(); i++)
    {
        cTMap * map1 = maps1.at(i);
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

        ret.push_back(nmap);
    }


        return ret;

}

inline static cTMap * AS_RasterSubSection(cTMap * map1, int r0, int c0, int rows, int cols)
{

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

    return nmap;

}

inline static std::vector<cTMap *> AS_RasterSublSection(std::vector<cTMap * > maps1, BoundingBox b)
{
    std::vector<cTMap *> ret;

    for(int i = 0; i < maps1.size(); i++)
    {
        cTMap *map1 = maps1.at(i);

        BoundingBox b1 = map1->GetBoundingBox();
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
        ret.push_back(nmap);
    }

    return ret;



}


inline static cTMap * AS_RasterSubSection(cTMap * map1, BoundingBox b)
{
    BoundingBox b1 = map1->GetBoundingBox();
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


    std::cout << "test " << r0 << " " << c0 << " " << r1 <<  " " << c1 << std::endl;


    if(rows == 0 || cols == 0)
    {
        LISEMS_ERROR("Invalid region provided for RasterSubSection");
        throw 1;
    }

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

    return nmap;

}

inline static LSMVector2 AS_GetRasterCellSize(cTMap *m)
{
    return LSMVector2(m->cellSizeX(),m->cellSizeY());
}


inline static LSMVector2 AS_GetRasterULC(cTMap *m)
{

    return LSMVector2(m->west(),m->north());
}

inline static cTMap * AS_SetRasterCellSize(cTMap * m, double dx, double dy)
{
    if(dy == 0.0)
    {
        dy = dx;
    }

    MaskedRaster<float> raster_data(m->nrRows(), m->nrCols(), m->north(), m->west(), dx,dy);
    cTMap *nmap = new cTMap(std::move(raster_data),m->projection(),"");

    FOR_ROW_COL(nmap)
    {
        nmap->data[r][c] = m->data[r][c];
    }
    return nmap;
}

inline static cTMap * AS_SetRasterULC(cTMap * m, double x, double y)
{

    MaskedRaster<float> raster_data(m->nrRows(), m->nrCols(), y,x,m->cellSizeX(),m->cellSizeY());
    cTMap *nmap = new cTMap(std::move(raster_data),m->projection(),"");

    FOR_ROW_COL(nmap)
    {
        nmap->data[r][c] = m->data[r][c];
    }
    return nmap;

}
