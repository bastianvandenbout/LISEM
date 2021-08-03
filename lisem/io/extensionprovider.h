#ifndef EXTENSIONPROVIDER_H
#define EXTENSIONPROVIDER_H

#include "QList"
#include "QString"




inline static QList<QString> GetMapExtensions()
{
    QList<QString> ret;
    ret.append(".map");
    ret.append(".tif");
    ret.append(".jp2");
    ret.append(".tiff");
    ret.append(".asc");
    ret.append(".jpg");
    ret.append(".jpeg");
    ret.append(".png");
    ret.append(".bmp");
    ret.append(".cdf"); //netCDF
    return ret;
}
inline static QList<QString> GetMapExtensionGDALDriver()
{
    QList<QString> ret;
    ret.append("PCRaster");
    ret.append("GTIFF");
    ret.append("JP2OpenJPEG");
    ret.append("GTIFF");
    ret.append("AAIGRID");
    ret.append("JPEG");
    ret.append("JPEG");
    ret.append("PNG");
    ret.append("BMP");
    ret.append("netCDF");
    ret.append("SAFE");

    return ret;
}

inline static bool IsMapFile(QString file)
{
    QList<QString> extensions = GetMapExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}


inline static QList<bool> GetMapExtensionGDALDriverMultiLayerSupport()
{
    QList<bool> ret;
    ret.append(false);
    ret.append(true);
    ret.append(true);
    ret.append(true);
    ret.append(false);
    ret.append(true);
    ret.append(true);
    ret.append(true);
    ret.append(true);
    ret.append(true);

    return ret;
}
inline static QList<bool> GetMapExtensionGDALDriverMultiLayerSupport3or1Only()
{
    QList<bool> ret;
    ret.append(false);
    ret.append(false);
    ret.append(false);
    ret.append(false);
    ret.append(false);
    ret.append(true);
    ret.append(true);
    ret.append(true);
    ret.append(true);
    ret.append(false);
    return ret;
}

inline static bool GetGDALDriverForRasterFileBandCountRSupport(QString file, int bands)
{
    if(bands == 1)
    {
        return true;
    }
    if(bands == 0)
    {
        return false;
    }
    QList<QString> drivers = GetMapExtensionGDALDriver();
    QList<QString> extensions = GetMapExtensions();
    QList<bool> mlsup =GetMapExtensionGDALDriverMultiLayerSupport();
    QList<bool> mlsup31 =GetMapExtensionGDALDriverMultiLayerSupport3or1Only();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            if(bands > 2)
            {
                return mlsup.at(i) && !mlsup31.at(i);
            }else {
                return mlsup.at(i);
            }

        }
    }

    return false;

}


inline static bool GetGDALDriverForRasterFileBandCountSupport(QString file, int bands)
{
    if(bands == 1)
    {
        return true;
    }
    if(bands == 0)
    {
        return false;
    }
    QList<QString> drivers = GetMapExtensionGDALDriver();
    QList<QString> extensions = GetMapExtensions();
    QList<bool> mlsup =GetMapExtensionGDALDriverMultiLayerSupport();
    QList<bool> mlsup31 =GetMapExtensionGDALDriverMultiLayerSupport3or1Only();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            if(!bands == 3)
            {
                return mlsup.at(i) && !mlsup31.at(i);
            }else {
                return mlsup.at(i);
            }

        }
    }

    return false;

}


inline static QString GetGDALDriverForRasterFile(QString file)
{
    QList<QString> drivers = GetMapExtensionGDALDriver();
    QList<QString> extensions = GetMapExtensions();
    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return drivers.at(i);
            break;
        }
    }

    return QString();

}


inline static QList<QString> GetShapeExtensions()
{
    QList<QString> ret;
    ret.append(".shp");
    ret.append(".gpkg");
    ret.append(".kml");
    ret.append(".osm");

    return ret;
}
inline static QList<QString> GetShapeExtensionsGDALDriver()
{
    QList<QString> ret;
    ret.append("ESRI ShapeFile");
    ret.append("GPKG");
    ret.append("LIBKML");
    ret.append("OSM");

    return ret;
}

inline static bool IsShapeFile(QString file)
{
    QList<QString> extensions = GetShapeExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}


inline static QString GetGDALDriverForShapeFile(QString file)
{
    QList<QString> drivers = GetShapeExtensionsGDALDriver();
    QList<QString> extensions = GetShapeExtensions();
    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return drivers.at(i);
            break;
        }
    }

    return QString();

}
inline static QList<QString> GetTableExtensions()
{
    QList<QString> ret;
    ret.append(".csv");
    ret.append(".tbl");
    return ret;
}

inline static bool IsTableFile(QString file)
{
    QList<QString> extensions = GetTableExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}

inline static QList<QString> GetScriptExtensions()
{
    QList<QString> ret;
    ret.append(".script");
    ret.append(".cl");
    ret.append(".c");
    ret.append(".glsl");
    ret.append(".txt");
    return ret;
}

inline static bool IsScriptFile(QString file)
{
    QList<QString> extensions = GetScriptExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}

inline static QList<QString> GetModelExtensions()
{
    QList<QString> ret;
    ret.append(".obj");
    ret.append(".ply");
    ret.append(".blend");
    ret.append(".stl");
    ret.append(".3ds");
    ret.append(".dae");
    return ret;
}
inline static QList<QString> GetModelExtensionsFormat()
{
    QList<QString> ret;
    ret.append("obj");
    ret.append("ply");
    ret.append("");
    ret.append("stl");
    ret.append("3ds");
    ret.append("collada");
    return ret;
}


inline static bool IsModelFile(QString file)
{
    QList<QString> extensions = GetModelExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}

inline static QList<QString> GetPointCloudExtensions()
{
    QList<QString> ret;
    ret.append(".las");
    ret.append(".ply");
    ret.append(".npy");
    ret.append(".mat");
    return ret;
}

inline static QList<QString> GetFieldExtensions()
{
    QList<QString> ret;
    ret.append(".fld");
    ret.append(".nc"); //netcdf - version 4, could also be a raster
    return ret;
}


inline static bool IsFieldFile(QString file)
{
    QList<QString> extensions = GetFieldExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}

inline static bool IsPointCloudFile(QString file)
{
    QList<QString> extensions = GetPointCloudExtensions();

    for(int i = 0; i < extensions.length(); i++)
    {
        if(file.endsWith(extensions.at(i)))
        {
            return true;
        }
    }

    return false;
}

inline static QString GetExtensionsFileFilter(QList<QString> exts)
{
    QString ret;


    for(int i = 0; i < exts.length(); i++)
    {
        ret += "*" + exts.at(i);
        ret += ";";
    }

    ret += ";*.*";

    return ret;

}

inline static QList<QString> GetAllExtensions()
{

    QList<QString> ext1 = GetMapExtensions();
    QList<QString> ext2 = GetShapeExtensions();
    QList<QString> ext3 = GetTableExtensions();
    QList<QString> ext4 = GetModelExtensions();
    QList<QString> ext5 = GetPointCloudExtensions();
    QList<QString> ext6 = GetFieldExtensions();

    QList<QList<QString>> exts = {ext1,ext2,ext3,ext4,ext5,ext6};

    QList<QString> ret;


    for(int i = 0; i < exts.length(); i++)
    {
        for(int j = 0; j < exts.at(i).length(); j++)
        {
            ret.append(exts.at(i).at(j));
        }
    }


    return ret;
}

inline static QList<QString> GetAllExtensionsFilter()
{

    QList<QString> ext1 = GetMapExtensions();
    QList<QString> ext2 = GetShapeExtensions();
    QList<QString> ext3 = GetTableExtensions();
    QList<QString> ext4 = GetModelExtensions();
    QList<QString> ext5 = GetPointCloudExtensions();
    QList<QString> ext6 = GetFieldExtensions();

    QList<QList<QString>> exts = {ext1,ext2,ext3,ext4,ext5,ext6};

    QList<QString> ret;


    for(int i = 0; i < exts.length(); i++)
    {
        for(int j = 0; j < exts.at(i).length(); j++)
        {
            ret.append("*"+exts.at(i).at(j));
        }
    }


    return ret;
}

#endif // EXTENSIONPROVIDER_H
