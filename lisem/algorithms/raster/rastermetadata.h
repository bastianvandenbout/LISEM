#ifndef RASTERMETADATA_H
#define RASTERMETADATA_H

#include "QString"
#include "lsmio.h"
#include "extensionprovider.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <QFileInfo>
#include <gdal_priv.h>
#include "geo/raster/map.h"
#include "QDebug"
#include "error.h"
#include "csf.h"
#include "gdal_priv.h"
#include "cpl_string.h"
#include "cpl_conv.h"
#include "gdalwarper.h"
#include "cpl_vsi.h"
#include "QDir"
#include "iogdal.h"

/*inline static CScriptArray * AS_GetVectorMetaDataAbsPath(QString file)
{


    return nullptr;
}


inline static CScriptArray * AS_GetVectorMetaData(QString file)
{

    return AS_GetVectorMetaDataAbsPath(AS_DIR+file);

}

inline static CScriptArray * AS_GetVectorSubDataSetsAbsPath(QString file)
{

    return nullptr;
}


inline static CScriptArray * AS_GetVectorSubDataSets(QString file)
{

    return AS_GetVectorSubDataSetsAbsPath(AS_DIR+file);

}



inline static CScriptArray * AS_GetRasterSubDataSetsAbsPath(QString file)
{

    return nullptr;

}

inline static CScriptArray * AS_GetRasterSubDataSets(QString file)
{

    return AS_GetRasterSubDataSetsAbsPath(AS_DIR+file);

}


inline static CScriptArray * AS_GetRasterMetaDataAbsPath(QString file, int band)
{

    //auto-detect format


    QString format = GetGDALDriverForRasterFile(file);
    if(format.isEmpty())
    {
        LISEMS_WARNING("Raster format not recognized");
    }

    GDALDatasetPtr dataset(static_cast<GDALDataset*>(GDALOpen(
        file.toLatin1().constData(), GA_ReadOnly)), close_gdal_dataset);

    if(!dataset)
    {
        LISEMS_ERROR("File not found: " + file);
        throw 1;
    }



    std::cout << "GCP Count " << dataset->GetGCPCount() << std::endl;
    std::cout << "Band Count " << dataset->GetRasterCount() << std::endl;


    QList<QString> meta;
    {
        char ** argv = dataset->GetMetadata();
        if(argv != nullptr)
        {
            while ( *argv != nullptr)
            {
                std::cout << "metadata: " << *argv << std::endl;
                meta.push_back(QString(*argv));

                argv++;
            }
        }
    }

    {
        char** papszMDDList = dataset->GetMetadataDomainList();
        char** papszIter = papszMDDList;
        while( papszIter != nullptr && *papszIter != nullptr )
        {
            std::cout << "======= Metadata domain: "<< *papszIter << std::endl;
            char ** argv = dataset->GetMetadata(*papszIter);
            if(argv != nullptr)
            {
                while ( *argv != nullptr)
                {
                    std::cout << *argv << std::endl;
                    meta.push_back(QString(*argv));

                    argv++;
                }
            }

            papszIter ++;

        }
    }


    for(int i = 0; i < dataset->GetRasterCount(); i++)
    {
        GDALRasterBand* gdband{dataset->GetRasterBand(1+i)};

        double transformation[6];
        dataset->GetGeoTransform(transformation);

        const OGRSpatialReference * ref = dataset->GetSpatialRef();
        char * refc;
        if(ref != nullptr)
        {

            ref->exportToWkt(&refc);
        }

        QString projection{dataset->GetProjectionRef()};

        int const nr_rows{gdband->GetYSize()};
        int const nr_cols{gdband->GetXSize()};
        double const west{transformation[0]};
        double const north{transformation[3]};
        double const cell_size{transformation[1]};
        double const cell_sizeY{transformation[5]};


        std::cout << "!=====================================" << std::endl;
        std::cout << "Band " << i << " " << nr_rows << " " << nr_cols << " " << west << " " << north << " " << cell_size << " "<< cell_sizeY << std::endl;


        {
            char ** argv = gdband->GetMetadata();
            if(argv != nullptr)
            {
                while ( *argv != nullptr)
                {
                    std::cout << "metadata: " << *argv << std::endl;
                    meta.push_back(QString(*argv));

                    argv++;
                }
            }
        }


        char** papszMDDList = gdband->GetMetadataDomainList();
        char** papszIter = papszMDDList;
        while( papszIter != nullptr && *papszIter != nullptr )
        {
            std::cout << "======= Metadata domain: "<< *papszIter << std::endl;
            char ** argv = gdband->GetMetadata(*papszIter);
            if(argv != nullptr)
            {
                while ( *argv != nullptr)
                {
                    std::cout << *argv << std::endl;
                    meta.push_back(QString(*argv));

                    argv++;
                }
            }

            papszIter ++;

        }

    }



    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: This should only be done once
    // TODO: This assumes that CScriptArray was already registered
    asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");

    // Create the array object
    CScriptArray *array = CScriptArray::Create(arrayType);
    array->Resize(meta.length());
    for(int i = 0; i < meta.length(); i++)
    {
        array->SetValue(i,new QString(meta.at(i)),false);
    }

    return array;

}




inline static CScriptArray * AS_GetRasterMetaData(QString file, int band)
{
    return AS_GetRasterMetaDataAbsPath(AS_DIR + file,band);

}*/


#endif // RASTERMETADATA_H
