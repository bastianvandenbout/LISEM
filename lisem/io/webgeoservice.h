#ifndef WEBGEOSERVICE_H
#define WEBGEOSERVICE_H


#include "zlib.h"
#include "geo/raster/map.h"
#include "downloadmanager.h"
#include "iogdal.h"
#include "extensionprovider.h"
#include "qxmlstream.h"



class AS_ByteArray
{
public:

    QByteArray data;

    inline AS_ByteArray()
    {

    }
    int            AS_refcount          = 1;
    inline void           AS_AddRef            ()
    {

        AS_refcount = AS_refcount + 1;
    }
    void           AS_ReleaseRef        ()
    {
        AS_refcount = AS_refcount - 1;
        if(AS_refcount == 0)
        {

            delete this;
        }
    }
    inline AS_ByteArray * AS_Assign            (AS_ByteArray*o)
    {

        data = o->data;
        return this;
    }


};

inline static AS_ByteArray * AS_ByteArrayFactory()
{
    return new AS_ByteArray();
}


inline static std::vector<cTMap*> ToMap(AS_ByteArray * data, QString Format)
{

    QList<QString> exts = GetMapExtensions();
    QList<QString> drivers = GetMapExtensionGDALDriver();

    bool found_gdal = false;
    QString ext = ".tif";
    if(!(Format == "PNG" || Format == "JPG"))
    {
        for(int i = 0; i < exts.size(); i++)
        {
            if(drivers.at(i) == Format)
            {

                found_gdal = true;
                ext = exts.at(i);
            }
        }
    }

    QList<cTMap *> maps;

    if(found_gdal)
    {
        maps = GetRasterListFromQByteArray(&(data->data),Format,ext);

    }else
    {
        std::cout << "a " << std::endl;
        QPixmap PixMap;

        std::cout << "b " << std::endl;
        PixMap.loadFromData(data->data);


        std::cout << "c " << std::endl;

        QImage Image = QImage(PixMap.toImage().convertToFormat(QImage::Format_ARGB32));


        std::cout << "d " << Image.size().width() << " " << Image.size().width() << std::endl;
        MaskedRaster<float> raster_datar( Image.size().height(), Image.size().width(),0.0,0.0,1.0,1.0);
        MaskedRaster<float> raster_datag( Image.size().height(), Image.size().width(),0.0,0.0,1.0,1.0);
        MaskedRaster<float> raster_datab( Image.size().height(), Image.size().width(),0.0,0.0,1.0,1.0);

        cTMap * Mapr = new cTMap(std::move(raster_datar),"","");
        cTMap * Mapg = new cTMap(std::move(raster_datag),"","");
        cTMap * Mapb = new cTMap(std::move(raster_datab),"","");
        maps.push_back(Mapr);
        maps.push_back(Mapg);
        maps.push_back(Mapb);

        std::cout << "e" << std::endl;
        uchar * bits = Image.bits();

        for(int r = 0; r < Image.size().height(); r++)
        {
            for(int c = 0; c < Image.size().width(); c++)
            {
                int indexuchar = 4 * (r * Image.size().width() + c);

                Mapr->data[r][c] = float(bits[indexuchar + 2])/255.0f;
                Mapg->data[r][c] = float(bits[indexuchar + 1])/255.0f;
                Mapb->data[r][c] = float(bits[indexuchar + 0])/255.0f;

            }
        }

        std::cout <<"f " << std::endl;

    }

    std::vector<cTMap*> ret;
    for(int i = 0; i < maps.size(); i++)
    {
        ret.push_back(maps.at(i));
    }

    return ret;


}

inline static QString ToText(AS_ByteArray * data)
{
    QString s = QString(data->data);
    return s;
}

inline static QString ToXML(AS_ByteArray * data)
{
    QString s = QString(data->data);
    return s;
}

inline static QString GetOWSCapabilitiesType(QString xml)
{
    std::cout << xml.toStdString() << std::endl;
    QString ret;
    QXmlStreamReader reader(xml);
    if (reader.readNextStartElement()) {
        if (reader.name() == "Capabilities"){
            std::cout << "a" << std::endl;
            while(reader.readNextStartElement()){
                std::cout << "reader " << reader.name().toString().toStdString() << std::endl;
                if(reader.name() == "ows:OperationsMetadata")
                {

                    std::cout << "b" << std::endl;
                    while(reader.readNextStartElement()){
                        if(reader.name() == "ows:Parameter"){

                            std::cout << "c" << std::endl;
                            if(reader.attributes().hasAttribute("name"))
                            {
                                if(reader.attributes().value("name").toString() == "Service")
                                {

                                    std::cout << "d" << std::endl;
                                    while(reader.readNextStartElement()){
                                        if(reader.name() == "ows:AllowedValues"){

                                            std::cout << "e" << std::endl;
                                            while(reader.readNextStartElement()){
                                                if(reader.name() == "ows:Value"){

                                                    std::cout << "f" << std::endl;
                                                    QString s = reader.readElementText();
                                                    if(ret.length() > 0)
                                                    {
                                                        ret.append("|");
                                                    }

                                                    ret.append(s);



                                                }else
                                                {

                                                    reader.skipCurrentElement();
                                                }

                                            }

                                        }else
                                        {
                                            reader.skipCurrentElement();
                                        }
                                    }

                                    break;

                                }
                            }
                        }
                        else
                        {
                            reader.skipCurrentElement();
                        }
                    }

                }
                else
                {
                    reader.skipCurrentElement();
                }
            }
        }
        else
        {
            reader.skipCurrentElement();
        }
    }
    return ret;

}

inline static std::vector<QString> GetOWSLayers(QString xml)
{
    QString type = GetOWSCapabilitiesType(xml);

    std::vector<QString> ret;

    QXmlStreamReader reader(xml);
    while(reader.readNextStartElement()) {
        if (reader.name() == "Capabilities"){
            while(reader.readNextStartElement()){
                if(reader.name() == "childA"){
                    QString s = reader.readElementText();


                }
                else
                {
                    reader.skipCurrentElement();
                }
            }
        }else
            {
                reader.skipCurrentElement();
            }
    }


    return ret;


}
/*inline static QString GetOWSTiledLayerTemplate()
{


}
inline static QString GetOWSTiledLayerScales()
{


}
inline static QString GetOWSTiledLayerULX()
{


}
inline static QString GetOWSTiledLayerULY()
{


}
inline static QString GetOWSTiledLayerTileSizeX()
{


}
inline static QString GetOWSTiledLayerTileSizeY()
{


}
inline static QString GetOWSTiledLayerMatrixSizeX()
{


}
inline static QString GetOWSTiledLayerMatrixSizeY()
{


}
inline static QString GetOWSOperations()
{


}
inline static QString GetOWSLayerDescription()
{


}
inline static QString GetOWSLayerExtent()
{


}
inline static QString GetOWSOperationValues()
{


}*/


//request for data in geotiff format
//http://maps.isric.org/mapserv?map=/map/wrb.map&SERVICE=WCS&VERSION=1.0.0&REQUEST=GetCoverage&coverage=MostProbable&CRS=EPSG:3978&BBOX=-2200000,-712631,3072800,3840000&WIDTH=3199&HEIGHT=2833&FORMAT=GTiff

//
//http://geoport.whoi.edu/thredds/wcs/bathy/srtm30plus_v6?request=GetCoverage&version=1.0.0&service=WCS&format=geotiff&coverage=topo&BBOX=0,50,10,55

//onegeology WCS
//http://ogc.bgs.ac.uk/fcgi-bin/exemplars/BGS_Bedrock_and_Superficial_Geology/wms?version=1.1.1&



//request for capabilities, returns xml file
//http://maps.isric.org/mapserv?map=/map/wrb.map&SERVICE=WCS&VERSION=1.1.0&REQUEST=GetCapabilities

//request for data in png format
//http://maps.isric.org/mapserv?map=/map/wrb.map&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&LAYERS=Arenosols&STYLES=&SRS=EPSG:4326&BBOX=-173.537,35.8775,-11.9603,83.8009&WIDTH=1400&HEIGHT=1300&FORMAT=image/png

//https://view.onda-dias.eu/instance00/ows?&service=WMS&request=GetMap&layers=S1_IW_GRDH_FullResolution&styles=&format=image/png&transparent=true&version=1.1.1&width=1720&height=2048&srs=EPSG:4326&bbox=11.5,40.5,14.5,43.5&time=2020-06-07/2020-06-09
//https://gitlab.com/pub-sobloo/support/-/wikis/Tutorials/Basic-API-usage

//http://tiles.tapviewer.vgt.vito.be/geoserver/CGS/ows?SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&LAYERS=CGS_S2_10M_BANDS&STYLES=&SRS=EPSG:4326&BBOX=-173.537,35.8775,-11.9603,83.8009&WIDTH=1400&HEIGHT=1300&FORMAT=image/png
//sentinel data?
//https://sentineldata.vito.be/wmts?service=WMTS&request=GetCapabilities


//nasa SRTM data
//https://e4ftl01.cr.usgs.gov/MEASURES/SRTMGL1.003/2000.02.11/S02E010.SRTMGL1.hgt.zip





/*inline static CScriptArray * DownloadAndMergeTileServiceRegion()
{
    //is this 1 band or 3 band?


    //get all the tiles in a large list
    //now




    return nullptr;
}

inline static cTMap * GetWMSRegion()
{


    return nullptr;
}

inline static cTMap * GetWCSRegion()
{



    return nullptr;
}*/


#endif // WEBGEOSERVICE_H
