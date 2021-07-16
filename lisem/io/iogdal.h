#ifndef IOGDAL_H
#define IOGDAL_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <QFileInfo>
#include <gdal_priv.h>
#include "geo/raster/map.h"
#include "QDebug"
#include "error.h"
#include "gdal_priv.h"
#include "cpl_string.h"
#include "cpl_conv.h"
#include "gdalwarper.h"
#include "cpl_vsi.h"
#include "QDir"

#include <QApplication>
#include "site.h"


//! Function to close a GDAL GDALDataset.
const auto close_gdal_dataset = [](GDALDataset* dataset) { GDALClose(dataset); };

//! Auto-ptr type for GDAL GDALDatasets.
using GDALDatasetPtr = std::unique_ptr<GDALDataset, decltype(
    close_gdal_dataset)>;


inline static void GDALERROR(CPLErr er, CPLErrorNum ernum, const char* message)
{

    std::cout << "error from gdal (" << ernum << ") " << message << std::endl;

}



inline static bool PreInitializeGDAL(QString exepath)
{

    //this is a bit of a nasty solution to a problem I have with the
    //PROJ library which is used by gdal for spatial reference data
    //this library requires proj.db, a file with all CRC systems
    //however, to find it it wants to use the environment variable
    //It does not seem to work when using any other trick (setting gdal config, etc..)
    //perhaps because the PROJ_LIB is not dynamically linked? idk..

    QString projlibdir = qEnvironmentVariable("PROJ_LIB");
    LISEM_DEBUG("environment variable PROJ_LIB " + projlibdir);

    bool found  = false;
    if(!projlibdir.isEmpty())
    {
        QFileInfo f = QFileInfo(projlibdir);
        if(f.exists())
        {
            if(f.isDir())
            {
                QFileInfo f = QFileInfo(projlibdir + "/proj.db");
                if(f.exists() == true)
                {
                    LISEM_DEBUG("Found proj.db");
                    found = true;
                }
            }
        }
    }

    if(found == false)
    {
        QString temp = exepath;
        LISEM_DEBUG("No proj.db found, so setting environment variable PROJ_LIB");
        qputenv("PROJ_LIB",exepath.toStdString().c_str());
        qputenv("GDAL_MAX_BAND_COUNT", "1000000");
        QFileInfo f = QFileInfo(temp + "/proj.db");
        bool exist = f.exists();
        if(exist == true)
        {
            LISEM_DEBUG("Found proj.db");

        }
    }

    return false;
}


inline static bool InitializeGDAL()
{
    //set the environment variables through local config
    CPLSetConfigOption("GDAL_DATA",QString(GetSite()+ "//gdal-data").toStdString().c_str());
    CPLSetConfigOption("PROJ_LIB",QString(GetSite()).toStdString().c_str());
    CPLSetConfigOption("GDAL_MAX_BAND_COUNT", "1000000");

    // GDAL mustn't throw in case of an error.
    CPLSetErrorHandler(GDALERROR);

    //try to set the data folders for gdal using a specific finder folder
    CPLPushFinderLocation(QString(GetSite()+ "//gdal-data").toStdString().c_str());
    CPLPushFinderLocation(QString(GetSite()).toStdString().c_str());

    // Register all GDAL drivers.
    GDALAllRegister();
    OGRRegisterAll();

    return true;
}



inline void RasterMetaDataToGDALDataset(cTMap *, GDALDatasetPtr & p)
{


   // p->GetMOFlags();
}



inline void RasterMetaDataFromGDALDataset(cTMap *, GDALDatasetPtr & p)
{





}






inline QList<cTMap*> GetRasterListFromQByteArray(QByteArray * ba, QString format= "GTiff", QString ext = ".tif")
{

    const char *pszFormat = format.toStdString().c_str();
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);


    VSILFILE* fpMem = VSIFileFromMemBuffer (QString("/vsimem/temp" + ext).toStdString().c_str(), (GByte*) ba->data(), (vsi_l_offset) ba->length(), FALSE );
    VSIFCloseL(fpMem);


    QList<cTMap *> ret;


    GDALDataset *dataset = (GDALDataset *) GDALOpen( QString("/vsimem/temp" + ext).toStdString().c_str(), GA_ReadOnly );
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg("Byte Array Memory"));
        return ret;
    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg("Byte Array Memory"));
        return ret;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    for(int i = 0; i < nr_bands; i++)
    {

        // Read the first raster band.
        GDALRasterBand* band{dataset->GetRasterBand(i)};
        assert(band);

        int const nr_rows{band->GetYSize()};
        int const nr_cols{band->GetXSize()};
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};


        MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

        // All raster values are read into float. PCRaster value scales are not
        // taken into account.
        if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
                nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("Byte Array Memory"));
            return ret;
        }

        int hasNoDataValue{false};
        double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
        if(hasNoDataValue) {
            raster_data.replace_with_mv(noDataValue);
        }

        ret.append(new cTMap(std::move(raster_data), projection, "Byte Array Memory",false));

        //LISEM_DEBUG("Map found, create map from data.");
    }


    GDALClose( (GDALDatasetH) dataset);
    VSIUnlink( QString("/vsimem/temp" + ext).toStdString().c_str() );


    return ret;

}







inline cTMap GetRasterFromQByteArrayGTIFF(QByteArray * ba, QString format= "GTiff", QString ext = ".tif")
{

    const char *pszFormat = format.toStdString().c_str();
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);


    VSILFILE* fpMem = VSIFileFromMemBuffer (QString("/vsimem/temp" + ext).toStdString().c_str(), (GByte*) ba->data(), (vsi_l_offset) ba->length(), FALSE );
    VSIFCloseL(fpMem);

    GDALDataset *dataset = (GDALDataset *) GDALOpen( QString("/vsimem/temp" + ext).toStdString().c_str(), GA_ReadOnly );
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg("Byte Array Memory"));
        return cTMap();
    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg("Byte Array Memory"));
        return cTMap();
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    // Read the first raster band.
    GDALRasterBand* band{dataset->GetRasterBand(1)};
    assert(band);

    int const nr_rows{band->GetYSize()};
    int const nr_cols{band->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};


    MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

    // All raster values are read into float. PCRaster value scales are not
    // taken into account.
    if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
            nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg("Byte Array Memory"));
        return cTMap();
    }

    int hasNoDataValue{false};
    double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
    if(hasNoDataValue) {
        raster_data.replace_with_mv(noDataValue);
    }

    //LISEM_DEBUG("Map found, create map from data.");


    GDALClose( (GDALDatasetH) dataset);
    VSIUnlink( QString("/vsimem/temp" + ext).toStdString().c_str() );


    return cTMap(std::move(raster_data), projection, "Byte Array Memory",false);

}

inline bool rasterCanBeOpenedForReading(
    QString const& pathName)
{
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    bool result{dataset};

    return result;
}

inline QList<cTMap*> readRasterList(
    QString const& pathName)
{
    //LISEM_DEBUG("load map from data " + pathName);

    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(pathName));
        throw 1;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};



    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(pathName));
        throw 1;
    }

    QList<cTMap*> ret;

    bool is_ldd = false;

    {
        char ** argv = dataset->GetMetadata();
        if(argv != nullptr)
        {
            while ( *argv )
            {
                QString arg = QString(*argv);


                std::cout << arg.toStdString() << std::endl;
                if((arg).startsWith(QString("PCRASTER_VALUESCALE=VS_LDD")))
                {
                    is_ldd = true;
                }
                argv++;
            }
        }

    }

    std::cout << "is_ldd " << is_ldd << std::endl;
    for(int i = 0; i < nr_bands; i++)
    {
        // Read the first raster band.
        GDALRasterBand* band{dataset->GetRasterBand(i+1)};
        assert(band);

        char ** argv = band->GetMetadata();
        bool set_scale = false;
        float scale = 1.0f;
        bool set_offset = false;
        float offset = 0.0f;

        if(argv != nullptr)
        {
            while ( *argv )
            {
                QString arg = QString(*argv);

                std::cout << arg.toStdString() << std::endl;

                if((arg).startsWith(QString("scale_factor")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_scale = true;
                            scale = val;
                        }
                    }
                }else if((arg).startsWith(QString("add_offset")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_offset = true;
                            offset = val;
                        }
                    }
                }else if((arg).startsWith(QString("PCRASTER_VALUESCALE=VS_LDD")))
                {
                    is_ldd = true;
                }


                argv++;
            }
        }



        int const nr_rows{band->GetYSize()};
        int const nr_cols{band->GetXSize()};
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};


        MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

        // All raster values are read into float. PCRaster value scales are not
        // taken into account.
        if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
                nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be read.").arg(pathName));
            throw 1;
        }

        int hasNoDataValue{false};
        double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
        if(hasNoDataValue) {
            raster_data.replace_with_mv(noDataValue);
        }

        if(set_scale || set_offset)
        {
            raster_data.offsetscale(scale,offset);
        }

        cTMap * add = new cTMap(std::move(raster_data), projection, pathName,is_ldd);
        ret.append(add);
    }
    return ret;
}

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

inline cTMap readRaster(
    QString const& pathName, int bandn = 0)
{

     std::thread::id this_id = std::this_thread::get_id();

    std::cout << "load map from data " << pathName.toStdString() << " " << bandn << "  " << this_id << std::endl;

    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly));
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(pathName));
        throw 1;
    }

    bool is_ldd = false;

    {
        char ** argv = dataset->GetMetadata();
        if(argv != nullptr)
        {
            while ( *argv )
            {
                QString arg = QString(*argv);

                std::cout << "arg " << arg.toStdString() << std::endl;

                if((arg).startsWith(QString("PCRASTER_VALUESCALE=VS_LDD")))
                {
                    std::cout << "is_ldd " << std::endl;
                    is_ldd = true;
                }
                argv++;
            }
        }

    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(pathName));
        throw 1;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    // Read the first raster band.
    GDALRasterBand* band{dataset->GetRasterBand(1 + bandn)};
    assert(band);

    char ** argv = band->GetMetadata();
    bool set_scale = false;
    float scale = 1.0f;
    bool set_offset = false;
    float offset = 0.0f;

    if(argv != nullptr)
    {
        while ( *argv )
        {
            QString arg = QString(*argv);
            if((arg).startsWith(QString("scale_factor")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_scale = true;
                        scale = val;
                    }
                }
            }else if((arg).startsWith(QString("add_offset")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_offset = true;
                        offset = val;
                    }
                }
            }
            argv++;
        }
    }

    int const nr_rows{band->GetYSize()};
    int const nr_cols{band->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};


    MaskedRaster<float> raster_data(nr_rows, nr_cols, north, west, cell_size, cell_sizeY);

    // All raster values are read into float. PCRaster value scales are not
    // taken into account.
    if(band->RasterIO(GF_Read, 0, 0, nr_cols, nr_rows, raster_data[0],
            nr_cols, nr_rows, GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg(pathName));
        throw 1;
    }

    int hasNoDataValue{false};
    double noDataValue{band->GetNoDataValue(&hasNoDataValue)};
    if(hasNoDataValue) {
        raster_data.replace_with_mv(noDataValue);
    }
    if(set_scale || set_offset)
    {
        raster_data.offsetscale(scale,offset);
    }

    //LISEM_DEBUG("Map found, create map from data.");

    GDALClose((GDALDatasetH) dataset);

    return cTMap(std::move(raster_data), projection, pathName,is_ldd);
}

inline RasterBandStats readRasterStats(
    QString const& pathName, int band = 0)
{


    RasterBandStats stats;

    stats.band = band;

    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        return stats;
    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        return stats;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {
        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    // Read the first raster band.
    GDALRasterBand* gdband{dataset->GetRasterBand(1+band)};
    assert(gdband);

    char ** argv = gdband->GetMetadata();
    bool set_scale = false;
    float scale = 1.0f;
    bool set_offset = false;
    float offset = 0.0f;

    if(argv != nullptr)
    {
        while ( *argv )
        {
            QString arg = QString(*argv);
            if((arg).startsWith(QString("scale_factor")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_scale = true;
                        scale = val;
                    }
                }
            }else if((arg).startsWith(QString("add_offset")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_offset = true;
                        offset = val;
                    }
                }
            }
            argv++;
        }
    }

    double min = 0.0;
    double max = 0.0;
    double mean = 0.0;
    double stdev = 0.0;

    gdband->GetStatistics(TRUE,TRUE,&min,&max,&mean,&stdev);
    stats.max = max;
    stats.min = min;
    stats.mean = mean;
    stats.stdev = stdev;
    return stats;

}

inline cTMapProps readRasterProps(
    QString const& pathName, int band = 0)
{

    cTMapProps props;


    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        props.can_be_read = false;
        return props;
    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        props.can_be_read = false;
        return props;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    // Read the first raster band.
    GDALRasterBand* gdband{dataset->GetRasterBand(1+band)};
    assert(gdband);

    char ** argv = gdband->GetMetadata();
    bool set_scale = false;
    float scale = 1.0f;
    bool set_offset = false;
    float offset = 0.0f;

    if(argv != nullptr)
    {
        while ( *argv )
        {
            QString arg = QString(*argv);
            if((arg).startsWith(QString("scale_factor")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_scale = true;
                        scale = val;
                    }
                }
            }else if((arg).startsWith(QString("add_offset")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_offset = true;
                        offset = val;
                    }
                }
            }
            argv++;
        }
    }

    int const nr_rows{gdband->GetYSize()};
    int const nr_cols{gdband->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};


    props.file_path = pathName;
    props.ulx = west;
    props.uly = north;
    props.sizex = nr_cols;
    props.sizey = nr_rows;
    props.cellsizex = cell_size;
    props.cellsizey = cell_sizeY;
    props.bands = nr_bands;
    props.projection = projection;
    props.is_ldd = ldd;
    props.can_be_read = true;
    props.band = band;
    return props;

}
inline QList<RasterBandStats> readRasterListStats(
    QString const& pathName)
{


    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(pathName));
        throw 1;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};



    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(pathName));
        throw 1;
    }

    QList<RasterBandStats> ret;

    for(int i = 0; i < nr_bands; i++)
    {
        // Read the first raster band.
        GDALRasterBand* band{dataset->GetRasterBand(i+1)};
        assert(band);

        char ** argv = band->GetMetadata();
        bool set_scale = false;
        float scale = 1.0f;
        bool set_offset = false;
        float offset = 0.0f;

        if(argv != nullptr)
        {
            while ( *argv )
            {
                QString arg = QString(*argv);
                if((arg).startsWith(QString("scale_factor")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_scale = true;
                            scale = val;
                        }
                    }
                }else if((arg).startsWith(QString("add_offset")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_offset = true;
                            offset = val;
                        }
                    }
                }
                argv++;
            }
        }


        double min = 0.0;
        double max = 0.0;
        double mean = 0.0;
        double stdev = 0.0;

        RasterBandStats stats;
        band->ComputeStatistics(FALSE,&min,&max,&mean,&stdev,nullptr,nullptr);
        //band->GetStatistics(FALSE,TRUE,&min,&max,&mean,&stdev);
        stats.max = max;
        stats.min = min;
        stats.mean = mean;
        stats.stdev = stdev;
        stats.band = i;
        ret.append(stats);
    }
    return ret;
}

inline QList<cTMapProps> readRasterListProps(
    QString const& pathName)
{
    //LISEM_DEBUG("load map from data " + pathName);

    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(pathName));
        throw 1;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};



    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(pathName));
        throw 1;
    }

    QList<cTMapProps> ret;

    for(int i = 0; i < nr_bands; i++)
    {
        // Read the first raster band.
        GDALRasterBand* band{dataset->GetRasterBand(i+1)};
        assert(band);

        char ** argv = band->GetMetadata();
        bool set_scale = false;
        float scale = 1.0f;
        bool set_offset = false;
        float offset = 0.0f;

        if(argv != nullptr)
        {
            while ( *argv )
            {
                QString arg = QString(*argv);
                if((arg).startsWith(QString("scale_factor")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_scale = true;
                            scale = val;
                        }
                    }
                }else if((arg).startsWith(QString("add_offset")))
                {
                    QStringList split = arg.split("=");
                    if(split.length() > 1)
                    {
                        bool ok = true;
                        double val = split.at(1).toDouble(&ok);
                        if(ok)
                        {
                            set_offset = true;
                            offset = val;
                        }
                    }
                }
                argv++;
            }
        }



        int const nr_rows{band->GetYSize()};
        int const nr_cols{band->GetXSize()};
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};

        cTMapProps props;
        props.can_be_read = true;
        props.file_path = pathName;
        props.ulx = west;
        props.uly = north;
        props.sizex = nr_cols;
        props.sizey = nr_rows;
        props.cellsizex = cell_size;
        props.cellsizey = cell_sizeY;
        props.bands = nr_bands;
        props.projection = projection;
        props.is_ldd = ldd;
        props.can_be_read = true;
        props.band = i;
        props.is_ldd = ldd;
        ret.append(props);
    }
    return ret;
}

inline void readRasterPixels(QString const& pathName, cTMap * map,int px0, int py0, int sizex, int sizey, int band = 0)
{
    //LISEM_DEBUG("load map from data " + pathName);

    //MAP *pcrm = Mopen(pathName.toStdString().c_str(),M_READ);
    bool ldd = false;
    /*if(pcrm != NULL)
    {
        int valscale = RgetValueScale(pcrm);
        if(valscale == VS_LDD)
        {
            ldd = true;
        }
        Mclose(pcrm);
    }*/

    // Open raster dataset and obtain some properties.
    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        pathName.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);
    if(!dataset) {
        LISEM_ERROR(QString("Map %1 cannot be opened.").arg(pathName));
        throw 1;
    }

    int nr_bands = dataset->GetRasterCount();
    if(nr_bands == 0) {
        LISEM_ERROR(QString("Map %1 does not contain any bands.").arg(pathName));
        throw 1;
    }

    double transformation[6];
    dataset->GetGeoTransform(transformation);
    const OGRSpatialReference * ref = dataset->GetSpatialRef();
    char * refc;
    if(ref != nullptr)
    {

        ref->exportToWkt(&refc);
    }

    QString projection{dataset->GetProjectionRef()};


    // Read the first raster band.
    GDALRasterBand* gdband{dataset->GetRasterBand(1+band)};
    assert(gdband);

    char ** argv = gdband->GetMetadata();
    bool set_scale = false;
    float scale = 1.0f;
    bool set_offset = false;
    float offset = 0.0f;

    if(argv != nullptr)
    {
        while ( *argv )
        {
            QString arg = QString(*argv);
            if((arg).startsWith(QString("scale_factor")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_scale = true;
                        scale = val;
                    }
                }
            }else if((arg).startsWith(QString("add_offset")))
            {
                QStringList split = arg.split("=");
                if(split.length() > 1)
                {
                    bool ok = true;
                    double val = split.at(1).toDouble(&ok);
                    if(ok)
                    {
                        set_offset = true;
                        offset = val;
                    }
                }
            }
            argv++;
        }
    }

    int const nr_rows{gdband->GetYSize()};
    int const nr_cols{gdband->GetXSize()};
    double const west{transformation[0]};
    double const north{transformation[3]};
    double const cell_size{transformation[1]};
    double const cell_sizeY{transformation[5]};

    if(px0 == -1)
    {
        px0 = 0;
    }
    if(py0 == -1)
    {
        py0 = 0;
    }
    if(sizex == -1)
    {
        sizex = nr_cols;
    }
    if(sizey == -1)
    {
        sizey = nr_rows;
    }
    // All raster values are read into float. PCRaster value scales are not
    // taken into account.
    if(gdband->RasterIO(GF_Read, px0, py0, sizex,sizey, map->data[0],
            map->nrCols(), map->nrRows(), GDT_Float32, 0, 0) != CE_None) {
        LISEM_ERROR(QString("Raster band %1 cannot be read.").arg(pathName));
        throw 1;
    }

    int hasNoDataValue{false};
    double noDataValue{gdband->GetNoDataValue(&hasNoDataValue)};
    if(hasNoDataValue) {
        map->data.replace_with_mv(noDataValue);
    }
    if(set_scale || set_offset)
    {
        map->data.offsetscale(scale,offset);
    }

}



/*inline void writePCRasterRaster(
    cTMap const& raster,
    QString pathName)
{
    QFileInfo fi(pathName);
    if(!fi.dir().exists())
    {
        fi.dir().mkdir(fi.dir().path());
    }

    if(!raster.AS_IsLDD)
    {

        // Create and configure CSF map.
        MapPtr csfMap{Rcreate(pathName.toLatin1().constData(), raster.nrRows(),
            raster.nrCols(), CR_REAL4,  VS_SCALAR , PT_YDECT2B, raster.west(),
            raster.north(), 0.0, raster.cellSize()), close_csf_map};

        if(!csfMap) {


            LISEM_ERROR(QString("Dataset %1 cannot be created.").arg(pathName));
            return;

        }

        RuseAs(csfMap.get(), CR_REAL4);

        // Copy cells to write to new buffer.
        auto const& raster_data(raster.data);
        std::unique_ptr<float[]> buffer{new float[raster_data.nr_cells()]};
        std::memcpy(buffer.get(), raster_data[0], sizeof(float) *
            raster_data.nr_cells());

        // Write cells from buffer to file.
        size_t nr_cells_written = RputSomeCells(csfMap.get(), 0,
            raster_data.nr_cells(), buffer.get());

        if(nr_cells_written != raster_data.nr_cells()) {
            LISEM_ERROR("rputsomecells write ERROR with " + pathName);
        }
    }else {

        //when writing an ldd, we need to provide data as a 1-byte unsigned int type

        MaskedRaster<uint8_t> raster_data(raster.nrRows(),raster.nrCols(),raster.north(),raster.west(),raster.cellSize(),raster.cellSizeY());
        for(int r = 0; r < raster_data.nr_rows(); r++)
        {
            for(int c = 0; c < raster_data.nr_cols(); c++)
            {
                if(pcr::isMV(raster.data[r][c]))
                {
                    pcr::setMV(raster_data[r][c]);
                }else {
                    raster_data[r][c] = (uint8_t) raster.data[r][c];
                }

            }
        }

        // Create and configure CSF map.
        MapPtr csfMap{Rcreate(pathName.toLatin1().constData(), raster.nrRows(),
            raster.nrCols(), CR_UINT1,  VS_LDD , PT_YDECT2B, raster.west(),
            raster.north(), 0.0, raster.cellSize()), close_csf_map};

        if(!csfMap) {
            LISEM_ERROR(QString("Dataset %1 cannot be created.").arg(pathName));
        }

        RuseAs(csfMap.get(), CR_UINT1);

        // Copy cells to write to new buffer.
        std::unique_ptr<uint8_t[]> buffer{new uint8_t[raster_data.nr_cells()]};
        std::memcpy(buffer.get(), raster_data[0], sizeof(uint8_t) *
            raster_data.nr_cells());

        // Write cells from buffer to file.
        size_t nr_cells_written = RputSomeCells(csfMap.get(), 0,
            raster_data.nr_cells(), buffer.get());

        if(nr_cells_written != raster_data.nr_cells()) {
            LISEM_ERROR("rputsomecells write ERROR with " + pathName);
        }



    }
}*/

inline void writeGDALRaster(
    QList<cTMap*> const& raster,
    QString const& pathName,
    GDALDriver& driver)
{

    QFileInfo fi(pathName);
    if(!fi.dir().exists())
    {
        fi.dir().mkdir(fi.dir().path());
    }

    // Create new dataset.
    int const nrRows{raster.at(0)->nrRows()};
    int const nrCols{raster.at(0)->nrCols()};
    int const nrBands{raster.length()};
    GDALDataset * d = driver.Create(pathName.toLatin1().constData(),
                  nrCols, nrRows, nrBands, GDT_Float32, nullptr);
    //GDALDatasetPtr dataset{driver.Create(pathName.toLatin1().constData(),
    //    nrCols, nrRows, nrBands, GDT_Float32, nullptr), close_gdal_dataset};

    if(d == NULL) {
         LISEM_ERROR(QString("Dataset cannot be created.") + pathName);
         throw 1;
    }

    for(int i = 0; i < raster.length(); i++)
    {
        MaskedRaster<float> const& raster_data{raster.at(i)->data};

        // Set some metadata.
        double transformation[]{
            raster_data.west(),
            raster_data.cell_size(),
            0.0,
            raster_data.north(),
            0.0,
            raster_data.cell_sizeY()};

        d->SetGeoTransform(transformation);

        if(!(raster.at(0)->projection() == "LISEM_GENERIC" || raster.at(0)->projection().isEmpty()))
        {
            d->SetProjection(raster.at(0)->projection().toLatin1().constData());
        }


        // PCRaster supports value scales, but other formats don't. We set the
        // value scale as a meta data item in the raster. If the format supports
        // setting meta data items, this allows for round tripping values scale
        // information back to the PCRaster format, in case the raster is
        // translated to PCRaster format later.

        if(raster.at(0)->AS_IsLDD)
        {
            d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_LDD");
        }else
        {
            d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_SCALAR");
        }

        // Write values to the raster band.
        auto band = d->GetRasterBand(i+1);

        band->SetNoDataValue(-FLT_MAX);


        if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
                const_cast<float*>(&raster_data.cell(0)),
                nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster band %1 cannot be written.").arg(pathName));
            throw 1;
        }
    }

    GDALClose( (GDALDataset*) d );

    QFile f(pathName + ".aux.xml");
    f.remove();

}


inline void writeGDALRaster(
    cTMap const& raster,
    QString const& pathName,
    GDALDriver& driver,
    int bandn = -1)
{

    QFileInfo fi(pathName);
    if(!fi.dir().exists())
    {
        fi.dir().mkdir(fi.dir().path());
    }

    CPLStringList sl;
    if(raster.AS_IsLDD)
    {
        sl.AddNameValue("PCRASTER_VALUESCALE","VS_LDD");
    }else
    {
        sl.AddNameValue("PCRASTER_VALUESCALE","VS_SCALAR");
    }


    std::cout << "write raster " << bandn << " " << pathName.toStdString() << std::endl;
    if(bandn == -1)
    {
        std::cout << "write single-band "<< std::endl;
        // Create new dataset.
        int const nrRows{raster.nrRows()};
        int const nrCols{raster.nrCols()};
        int const nrBands{1};
        GDALDataset * d;
        if(raster.AS_IsLDD)
        {
            d = driver.Create(pathName.toLatin1().constData(),
                      nrCols, nrRows, nrBands, GDT_Byte, sl.List());
        }else
        {
            d = driver.Create(pathName.toLatin1().constData(),
                      nrCols, nrRows, nrBands, GDT_Float32, sl.List());

        }
        //GDALDatasetPtr dataset{driver.Create(pathName.toLatin1().constData(),
        //    nrCols, nrRows, nrBands, GDT_Float32, nullptr), close_gdal_dataset};

        if(d == NULL) {
             LISEM_ERROR(QString("Dataset cannot be created.") + pathName);
             const char * c = CPLGetLastErrorMsg();
             if(c!=nullptr)
             {
                 LISEM_ERROR(QString(c));
             }
             throw 1;
        }


        MaskedRaster<float> const& raster_data{raster.data};

        // Set some metadata.
        double transformation[]{
            raster_data.west(),
            raster_data.cell_size(),
            0.0,
            raster_data.north(),
            0.0,
            raster_data.cell_sizeY()};

        d->SetGeoTransform(transformation);


        if(!(raster.projection() == "LISEM_GENERIC" || raster.projection().isEmpty()))
        {
            d->SetProjection(raster.projection().toLatin1().constData());
        }

        // PCRaster supports value scales, but other formats don't. We set the
        // value scale as a meta data item in the raster. If the format supports
        // setting meta data items, this allows for round tripping values scale
        // information back to the PCRaster format, in case the raster is
        // translated to PCRaster format later.

        if(raster.AS_IsLDD)
        {
            d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_LDD");
        }else
        {
            d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_SCALAR");
        }

        // Write values to the raster band.
        auto band = d->GetRasterBand(1);

        band->SetNoDataValue(-FLT_MAX);

        if(raster.AS_IsLDD)
        {
            MaskedRaster<int32_t> data = MaskedRaster<int32_t>(raster_data.nr_rows(),raster_data.nr_cols(),0.0,0.0,1.0,1.0);
            for(int r = 0; r < data.nr_rows(); r ++)
            {
                for(int c = 0; c < data.nr_cols(); c ++)
                {

                    data[r][c] = (int32_t) raster_data[r][c];
                }
            }
            if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
                    const_cast<int32_t*>(&data.cell(0)),
                    nrCols, nrRows, GDT_Int32, 0, 0) != CE_None) {
                LISEM_ERROR(QString("Raster band %1 cannot be written.").arg(pathName));
                throw 1;
            }
        }else
        {
            if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
                    const_cast<float*>(&raster_data.cell(0)),
                    nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
                LISEM_ERROR(QString("Raster band %1 cannot be written.").arg(pathName));
                throw 1;
            }
        }

        GDALClose( (GDALDataset*) d );


        QFile f(pathName + ".aux.xml");
        f.remove();

        return;
    }else {

        // Open raster dataset and obtain some properties.
        GDALDataset* dataset(static_cast<GDALDataset*>(GDALOpen(
            pathName.toLatin1().constData(), GA_Update)));
        if(!dataset) {

            // Create new dataset.
            int const nrRows{raster.nrRows()};
            int const nrCols{raster.nrCols()};
            int const nrBands{1};
            GDALDataset * d = driver.Create(pathName.toLatin1().constData(),
                          nrCols, nrRows, nrBands, GDT_Float32, nullptr);
            //GDALDatasetPtr dataset{driver.Create(pathName.toLatin1().constData(),
            //    nrCols, nrRows, nrBands, GDT_Float32, nullptr), close_gdal_dataset};

            if(d == NULL) {
                 LISEM_ERROR(QString("Dataset cannot be created.") + pathName);
                 throw 1;
            }


            MaskedRaster<float> const& raster_data{raster.data};

            // Set some metadata.
            double transformation[]{
                raster_data.west(),
                raster_data.cell_size(),
                0.0,
                raster_data.north(),
                0.0,
                raster_data.cell_sizeY()};

            d->SetGeoTransform(transformation);



            if(!(raster.projection() == "LISEM_GENERIC" || raster.projection().isEmpty()))
            {
                d->SetProjection(raster.projection().toLatin1().constData());
            }

            // PCRaster supports value scales, but other formats don't. We set the
            // value scale as a meta data item in the raster. If the format supports
            // setting meta data items, this allows for round tripping values scale
            // information back to the PCRaster format, in case the raster is
            // translated to PCRaster format later.

            if(raster.AS_IsLDD)
            {
                d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_LDD");
            }else
            {
                d->SetMetadataItem("PCRASTER_VALUESCALE", "VS_SCALAR");
            }

            // Write values to the raster band.
            auto band = d->GetRasterBand(1);

            band->SetNoDataValue(-FLT_MAX);


            std::cout << "writing to band create"<< band->GetAccess() << std::endl;

            if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
                    const_cast<float*>(&raster_data.cell(0)),
                    nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
                LISEM_ERROR(QString("Raster band %1 cannot be written.").arg(pathName));
                throw 1;
            }


            GDALClose( (GDALDataset*) d );


            QFile f(pathName + ".aux.xml");
            f.remove();

            return;
        }

        int nr_bands = dataset->GetRasterCount();
        if(nr_bands < bandn) {
            LISEM_ERROR(QString("Map %1 does not contain band ").arg(pathName) + QString::number(bandn));
            throw 1;
        }

        auto band = dataset->GetRasterBand(1);

        band->SetNoDataValue(-FLT_MAX);

        int const nrRows{raster.nrRows()};
        int const nrCols{raster.nrCols()};

        MaskedRaster<float> const& raster_data{raster.data};


        std::cout << "writing to band update "<< band->GetAccess() << std::endl;

        if(band->RasterIO(GF_Write, 0, 0, nrCols, nrRows,
                const_cast<float*>(&raster_data.cell(0)),
                nrCols, nrRows, GDT_Float32, 0, 0) != CE_None) {
            LISEM_ERROR(QString("Raster %1 cannot be written. ").arg(pathName) + "(band n: "+ QString::number(bandn) + ")");
            throw 1;
        }

        GDALClose( (GDALDataset*) dataset );


        QFile f(pathName + ".aux.xml");
        f.remove();

        return;


   }



}

inline void writeRaster(
    cTMap const& raster,
    QString const& pathName,
    QString format = "PCRaster",
    int bandn = -1)
{

    {
        if(raster.AS_IsLDD)
        {
            format = "GTIFF";
        }
    }

    if(raster.nrRows() == 0 || raster.nrCols() == 0) {
        return;
    }

    if(pathName.isEmpty()) {

        throw 1;
    }

    //LISEM_DEBUG(QString("report file ") + pathName);

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(
        format.toLatin1().constData());

    if(!driver) {
        LISEM_ERROR(QString("Format driver %1 not available.").arg(
            format.toLatin1().constData()));
        throw 1;
    }

    char** metadata{driver->GetMetadata()};
    bool driverSupportsCreate{CSLFetchBoolean(metadata, GDAL_DCAP_CREATE,
        FALSE) != FALSE};

    /*if(format == "PCRaster") {

            writePCRasterRaster(raster, pathName);
    }else */
    if(driverSupportsCreate) {
    // All is well, write using GDAL.
    writeGDALRaster(raster, pathName, *driver,bandn);
    }else {
        LISEM_ERROR(QString(
            "Format driver %1 cannot be used to create datasets.").arg(
                format.toLatin1().constData()));
        throw 1;
    }

    QFile f(pathName + ".aux.xml");
    f.remove();

}

inline bool CheckAllRastersInListSameSize(QList<cTMap*> const& rasters)
{
    if(rasters.length() == 0)
    {
        return true;
    }

    cTMap * ref = rasters.at(0);
    for (int i = 1; i < rasters.length();i++) {

        if(!(ref->nrCols() == rasters.at(i)->nrCols() && ref->nrRows() == rasters.at(i)->nrRows()))
        {
            return false;
        }
    }

    return true;
}

inline void writeRaster(
    QList<cTMap*> const& raster,
    QString const& pathName,
    QString format = "PCRaster")
{
    if(raster.length() == 1)
    {
        if(raster.at(0)->AS_IsLDD)
        {
            format = "GTIFF";
        }
    }
    if(!CheckAllRastersInListSameSize(raster)) {
        return;
    }

    if(pathName.isEmpty()) {

        throw 1;
    }
    if(raster.isEmpty())
    {
        LISEM_ERROR("No maps to write to file");
        throw 1;
    }

    //LISEM_DEBUG(QString("report file ") + pathName);

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(
        format.toLatin1().constData());

    if(!driver) {
        LISEM_ERROR(QString("Format driver %1 not available.").arg(
            format.toLatin1().constData()));
        throw 1;
    }

    char** metadata{driver->GetMetadata()};
    bool driverSupportsCreate{CSLFetchBoolean(metadata, GDAL_DCAP_CREATE,
        FALSE) != FALSE};

    /*if(format == "PCRaster") {

        if(raster.size() > 1)
        {
            LISEM_ERROR(QString(
                "Format driver %1 cannot be used to create datasets with multiple bands").arg(
                    format.toLatin1().constData()));
            throw 1;
        }else {

            writePCRasterRaster(*raster.at(0),pathName);
        }

    }else */
    if(driverSupportsCreate) {
    // All is well, write using GDAL.
        writeGDALRaster(raster, pathName, *driver);
    }else {
        LISEM_ERROR(QString(
            "Format driver %1 cannot be used to create datasets.").arg(
                format.toLatin1().constData()));
        throw 1;
    }

    QFile f(pathName + ".aux.xml");
    f.remove();

}

/// makes mapname if (name.map) or mapseries (name0000.001 to name0009.999)
inline void WriteMapSeries(
    cTMap const& raster,
    QString const& Dir,
    QString Name,
    int count,
    QString const& format = "PCRaster")
{
    QString path;
    QFileInfo fi(Name);

    if(Name.indexOf(".") < 0) {
        QString nam, dig;

        nam = Name + "00000000";

        nam.remove(7, 10);
        dig = QString("%1").arg(count, 4, 10, QLatin1Char('0'));
        dig.insert(1, ".");
        Name = nam + dig;
    }

    path = Dir + Name;
    writeRaster(raster, path, format);

}





#endif // IOGDAL_H
