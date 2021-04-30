#pragma once
#include "defines.h"
#include "qlist.h"
#include "downloadmanager.h"
#include "QString"
#include "geo/geoprojection.h"
#include "layer/geo/uigeolayer.h"
#include "boundingbox.h"
#include "downloadmanager.h"
#include "qpixmap.h"
#include "QImage"
#include "openglcltexture.h"
#include "iogdal.h"
#include "gl/openglcldatamanager.h"

struct TileInfo
{
    bool is_in;
    bool is_in_current = false;
    int x;
    int y;
    int z;
    bool is_done = false;
    DownloadTask* download;
    QPixmap * PixMap = nullptr;
    cTMap * TifMap = nullptr;
    QImage *Image;
    bool is_gl_created = false;
    bool is_used_cache = false;
    bool do_draw = false;
    int pixx;
    int pixy;
    float csx;
    float csy;
    float sx;
    float sy;
    float north;
    float west;
    float * red;
    float * green;
    float * blue;

    OpenGLCLTexture * T_R;
    OpenGLCLTexture * T_G;
    OpenGLCLTexture * T_B;
};

static inline bool TileInfoCompare(const TileInfo &ti1, const TileInfo &ti2)
{
    return ti1.z < ti2.z;
}

typedef struct TileServerZoomLevel
{
    int zoomlevel;
    int pixwidth;
    int pixheight;
    int matrixwidth;
    int matrixheight;
    double tilesizex;
    double tilesizey;
    double extenttlx;
    double extenttly;
    double extentbrx;
    double extentbry;

}TileServerZoomLevel;

class TileServerInfo
{
public:
    QString m_Name;
    QString m_Url;
    GeoProjection m_Projection;
    QList<TileServerZoomLevel> m_Data;
    BoundingBox m_Box;
    int m_BackupLevel = 0;
    bool m_IsTiff = false;
    float m_max;
    float m_min;
    int m_cache_size = 200;
    int m_MaxZoomLevel = 0;
    inline void SetProjection(GeoProjection p)
    {
        m_Projection = p;
    }

    inline void SetName(QString name)
    {
        m_Name = name;
    }

    inline void SetUrl(QString url)
    {
        m_Url = url;
    }

    inline void SetIsTiff(bool istiff)
    {
        m_IsTiff = istiff;
    }

    inline void SetCacheSize(int x)
    {
        m_cache_size = x;
    }

    inline void SetMinMax(float min, float max)
    {
        m_min = min;
        m_max = max;
    }
    inline void SetBoundingBox(BoundingBox b)
    {
        m_Box = b;
    }
    inline void SetOutOfBoundBackupLevel(int l)
    {
        m_BackupLevel = l;
    }
    inline void AddZoomLevel(int zoom, int pixwidth, int pixheight, int matrixw, int matrixh, double top, double left, double bottom, double right)
    {
        TileServerZoomLevel l;
        l.zoomlevel =zoom;
        l.pixwidth = pixwidth;
        l.pixheight = pixheight;
        l.matrixwidth = matrixw;
        l.matrixheight = matrixh;
        l.tilesizex = fabs(right-left)/((double)(matrixw));
        l.tilesizey = fabs(bottom-top)/((double)(matrixh));
        l.extentbrx = right;
        l.extentbry = bottom;
        l.extenttlx = left;
        l.extenttly = top;
        m_Data.append(l);

        m_MaxZoomLevel = std::max(zoom,m_MaxZoomLevel);
    }

    inline void AddZoomLevels(QList<QString> l)
    {
        for(int i = 0; i < l.length(); i++)
        {
            QStringList zl = l.at(i).split(";");
            if(zl.length() == 9)
            {
                bool error = false;
                for(int j= 0; j < zl.length(); j++)
                {
                    bool ok = true;
                    zl.at(j).toDouble(&ok);
                    if(!ok)
                    {
                        error = true;
                        break;
                    }
                }
                if(!error)
                {
                    AddZoomLevel(zl.at(0).toInt(),zl.at(1).toInt(),zl.at(2).toInt(),zl.at(3).toInt(),zl.at(4).toInt(),zl.at(5).toDouble(),zl.at(6).toDouble(),zl.at(7).toDouble(),zl.at(8).toDouble());
                }
            }
        }
    }

    inline QList<QString> GetZoomLevels()
    {
        QList<QString> l;

        for(int i = 0; i < m_Data.length(); i++)
        {
            TileServerZoomLevel zl;
            zl = m_Data.at(i);
            l.append(QString::number(zl.zoomlevel) + ";" +
                     QString::number(zl.pixwidth) + ";" +
                     QString::number(zl.pixheight) + ";" +
                     QString::number(zl.matrixwidth) + ";" +
                     QString::number(zl.matrixheight) + ";" +
                     QString::number(zl.extentbrx) + ";" +
                     QString::number(zl.extentbry) + ";" +
                     QString::number(zl.extenttlx) + ";" +
                     QString::number(zl.extenttly) );
        }
        return l;
    }

    inline void Save(QJsonObject * obj)
    {
        QList<QString> l = GetZoomLevels();
        LSMSerialize::FromQListString(obj,"ZoomLevels",l);
        LSMSerialize::FromString(obj,"URL",m_Url);
        LSMSerialize::FromString(obj,"Name",m_Name);
        LSMSerialize::FromInt(obj,"BackupLevel",m_BackupLevel);
        LSMSerialize::FromInt(obj,"CacheSize",m_cache_size);
        LSMSerialize::FromFloat(obj,"MaxVal",m_max);
        LSMSerialize::FromFloat(obj,"MinVal",m_min);
        LSMSerialize::FromBool(obj,"IsTiff",m_IsTiff);
        QString proj = m_Projection.GetWKT();
        LSMSerialize::FromString(obj,"CRS",proj);
        QString bb = m_Box.toString();
        LSMSerialize::FromString(obj,"BoundingBox",bb);
    }

    inline void Load(QJsonObject * obj)
    {
        QList<QString> l;
        LSMSerialize::ToQListString(obj,"ZoomLevels",l);
        AddZoomLevels(l);
        LSMSerialize::ToString(obj,"URL",m_Url);
        LSMSerialize::ToString(obj,"Name",m_Name);
        LSMSerialize::ToInt(obj,"BackupLevel",m_BackupLevel);
        LSMSerialize::ToInt(obj,"CacheSize",m_cache_size);
        LSMSerialize::ToFloat(obj,"MaxVal",m_max);
        LSMSerialize::ToFloat(obj,"MinVal",m_min);
        LSMSerialize::ToBool(obj,"IsTiff",m_IsTiff);
        QString proj;
        LSMSerialize::ToString(obj,"CRS",proj);
        m_Projection.FromString(proj);
        QString bb;
        LSMSerialize::ToString(obj,"BoundingBox",bb);
        m_Box.fromString(bb);
    }
};

#define TILESERVER_GOOGLEEARTH 1
#define TILESERVER_GOOGLEELEVATION 2
static TileServerInfo GetDefaultTileServer(int x)
{


    //if(x == TILESERVER_GOOGLEEARTH)
    if(x == TILESERVER_GOOGLEELEVATION)
    {
        TileServerInfo i;
        i.AddZoomLevel(0,512,512,1,1,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(1,512,512,2,2,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(2,512,512,4,4,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(3,512,512,8,8,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(4,512,512,16,16,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(5,512,512,32,32,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(6,512,512,64,64,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(7,512,512,128,128,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(8,512,512,256,256,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(9,512,512,512,512,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(10,512,512,1024,1024,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(11,512,512,2048,2048,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(12,512,512,4096,4096,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(13,512,512,8192,8192,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(14,512,512,16384,16384,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        //i.AddZoomLevel(15,512,512,32768,32768,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        //i.AddZoomLevel(16,512,512,65536,65536,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.SetCacheSize(400);
        i.SetOutOfBoundBackupLevel(2);
        i.SetIsTiff(true);
        i.SetName("Google Elevation Layer");
        i.SetMinMax(-8000.0,8000.0);
        i.SetBoundingBox(BoundingBox(-20037508.3427892439067364,20037508.3427892439067364,-20037508.3427892550826073, 20037508.3427892439067364));
        i.SetUrl("https://s3.amazonaws.com/elevation-tiles-prod/geotiff/{z}/{x}/{y}.tif");
        i.SetProjection(GeoProjection::GetFromEPSG(3857));
        return i;

    }else
    {
        TileServerInfo i;
        i.AddZoomLevel(0,256,256,1,1,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(1,256,256,2,2,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(2,256,256,4,4,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(3,256,256,8,8,           -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(4,256,256,16,16,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(5,256,256,32,32,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(6,256,256,64,64,         -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(7,256,256,128,128,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(8,256,256,256,256,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(9,256,256,512,512,       -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(10,256,256,1024,1024,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(11,256,256,2048,2048,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(12,256,256,4096,4096,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(13,256,256,8192,8192,    -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(14,256,256,16384,16384,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(15,256,256,32768,32768,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(16,256,256,65536,65536,  -20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(17,256,256,131072,131072,-20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.AddZoomLevel(18,256,256,262144,262144,-20037508.342789, -20037508.342789,20037508.342789,20037508.342789);
        i.SetCacheSize(400);
        i.SetOutOfBoundBackupLevel(2);
        i.SetName("Google Sattelite Layer");
        i.SetBoundingBox(BoundingBox(-20037508.3427892439067364,20037508.3427892439067364,-20037508.3427892550826073, 20037508.3427892439067364));
        i.SetUrl("http://mt0.google.com/vt/lyrs=s&hl=en&x={x}&y={y}&z={z}");
        i.SetProjection(GeoProjection::GetFromEPSG(3857));
        return i;
    }

    //i.SetUrl("http://mt1.google.com/vt/lyrs=s&x={x}&y={y}&z={z}");
    //i.SetUrl("https://basemap.nationalmap.gov/arcgis/rest/services/USGSShadedReliefOnly/MapServer/WMTS/tile/1.0.0/USGSShadedReliefOnly/Default/GoogleMapsCompatible/{z}/{y}/{x}");


    //https://basemap.nationalmap.gov/arcgis/rest/services/USGSShadedReliefOnly/MapServer/WMTS/tile/1.0.0/USGSShadedReliefOnly/Default/GoogleMapsCompatible/2/1/1
    //https://basemap.nationalmap.gov/arcgis/rest/services/USGSTopo/MapServer/WMTS/tile/1.0.0/USGSTopo/Default/GoogleMapsCompatible/2/1/1

    //return TileServerInfo();
};


class LISEM_API UIWebTileLayer : public UIGeoLayer
{

private:


    OpenGLCLTexture * m_TextureR = nullptr;
    OpenGLCLTexture * m_TextureG = nullptr;
    OpenGLCLTexture * m_TextureB = nullptr;
    bool m_createdTexture = false;
    OpenGLProgram * m_Program = nullptr;
    bool m_RequiresTileRedraw = false;
    TileServerInfo m_TileServerInfo;

    QList<TileInfo> m_RequiredTilesSortedCache;
    QList<TileInfo> m_RequiredTiles;
    QList<TileInfo> m_ReadyTiles;
    QList<TileInfo> m_OldTiles;

    TileInfo m_CentralDetailTile;
    int64_t time_ms_detailtile = 0;

    QMutex m_TileMutex;

    DownloadManager * m_DownloadManager;

    QList<OpenGLCLTexture * > m_TextureCache;
public:

        inline UIWebTileLayer()
        {

        }


        inline UIWebTileLayer(TileServerInfo info)
        {

            Initialize(info);
        }

        inline void Initialize(TileServerInfo info)
        {
            UIGeoLayer::Initialize(info.m_Projection,info.m_Box,info.m_Name,false,"",false);

            m_TileServerInfo = info;
            m_IsNative = false;
            m_RequiresCRSGLTransform = true;
            m_HasLegend = false;
            m_DownloadManager = GetDownloadManager();

            if(info.m_IsTiff)
            {

                m_CouldBeDEM = true;
                this->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
            }
        }



        inline void SaveLayer(QJsonObject * obj) override
        {
            {
                LSMSerialize::FromString(obj,"Name",m_Name);
                QJsonObject obj_style;
                m_Style.Save(&obj_style);

                obj->insert("Style",obj_style);

                QJsonObject obj_ts;
                m_TileServerInfo.Save(&obj_ts);

                obj->insert("TileServer",obj_ts);

                QString proj;
                LSMSerialize::ToString(obj,"CRS",proj);
                m_Projection.FromString(proj);
            }
        }

        inline void RestoreLayer(QJsonObject * obj)
        {

            QString path;
            QString name;
            LSMSerialize::ToString(obj,"ShapeFile",path);
            LSMSerialize::ToString(obj,"Name",name);

            QJsonValue tsval = (*obj)["TileServer"];
            if(tsval.isObject() && !tsval.isUndefined())
            {
                TileServerInfo si;
                QJsonObject osi = tsval.toObject();
                si.Load(&osi);

                if(si.m_Data.length() > 0)
                {
                    Initialize(si);
                    QJsonValue sval = (*obj)["Style"];
                    if(sval.isObject() && !sval.isUndefined())
                    {
                        LSMStyle st;
                        QJsonObject o =sval.toObject();
                        st.Restore(&o);
                        this->m_Style.CopyFrom(st);
                    }
                }

                QString proj;
                LSMSerialize::ToString(obj,"CRS",proj);
                GeoProjection p;
                p.FromString(proj);
                this->SetProjection(p);
            }else
            {
                m_Exists = false;
            }

        }

        inline QString layertypeName()
        {
            return "WebTileLayer";
        }


        inline void OnDownloadProgress(DownloadTask * dl)
        {
        }

        inline void OnDownloadStart(DownloadTask * dl)
        {


        }
        inline void OnDownloadFinished(DownloadTask * dl)
        {
            std::cout << "download done:  "<< dl->Url.toStdString() << std::endl;

            if(QNetworkReply::NetworkError::NoError == dl->Download->error())
            {

                if(m_CentralDetailTile.download == dl)
                {
                    if(m_TileServerInfo.m_IsTiff)
                    {
                        m_TileMutex.lock();
                        m_CentralDetailTile.is_done = true;
                        m_CentralDetailTile.TifMap = new cTMap(GetRasterFromQByteArrayGTIFF(&(dl->m_Data)));


                        TileServerZoomLevel zl =m_TileServerInfo.m_Data.at(m_TileServerInfo.m_MaxZoomLevel);

                        m_CentralDetailTile.pixx = m_CentralDetailTile.TifMap->nrCols();
                        m_CentralDetailTile.pixy = m_CentralDetailTile.TifMap->nrRows();

                        m_CentralDetailTile.csx = zl.tilesizex/m_CentralDetailTile.pixx;
                        m_CentralDetailTile.csy = zl.tilesizey/m_CentralDetailTile.pixy;

                        m_CentralDetailTile.sx = zl.tilesizex;
                        m_CentralDetailTile.sy = zl.tilesizey;

                        m_CentralDetailTile.north = zl.extentbry - float(m_CentralDetailTile.y) * zl.tilesizey;
                        m_CentralDetailTile.west = zl.extenttlx + float(m_CentralDetailTile.x) * zl.tilesizex;
                            m_TileMutex.unlock();

                    }
                }else {
                    for(int i = 0; i < m_RequiredTiles.length(); i++)
                    {
                        if(m_RequiredTiles.at(i).download == dl)
                        {
                            TileInfo ti =m_RequiredTiles.at(i);

                            TileServerZoomLevel zl =m_TileServerInfo.m_Data.at(ti.z);

                            ti.is_done = true;


                            if(m_TileServerInfo.m_IsTiff)
                            {
                                ti.TifMap = new cTMap(GetRasterFromQByteArrayGTIFF(&(dl->m_Data)));

                                ti.pixx = ti.TifMap->nrCols();
                                ti.pixy = ti.TifMap->nrRows();

                                ti.csx = zl.tilesizex/ti.pixx;
                                ti.csy = zl.tilesizey/ti.pixy;

                                ti.sx = zl.tilesizex;
                                ti.sy = zl.tilesizey;

                                ti.north = zl.extentbry - float(ti.y) * zl.tilesizey;
                                ti.west = zl.extenttlx + float(ti.x) * zl.tilesizex;

                                ti.red = new float[ti.pixx *ti.pixy];
                                ti.green = new float[ti.pixx *ti.pixy];
                                ti.blue = new float[ti.pixx *ti.pixy];

                                for(int r = 0; r <  ti.TifMap->nrCols(); r++)
                                {
                                    for(int c = 0; c <  ti.TifMap->nrRows(); c++)
                                    {
                                        int indexf = r * ti.TifMap->nrCols() + c;
                                        int indexuchar = 4 * (r * ti.TifMap->nrCols() + c);
                                        {
                                            ti.red[indexf] = float(ti.TifMap->data[r][c]);
                                        }
                                    }
                                }


                            }else {

                                ti.PixMap = new QPixmap();
                                ti.PixMap->loadFromData(dl->m_Data);

                                ti.Image = new QImage(ti.PixMap->toImage().convertToFormat(QImage::Format_ARGB32));

                                ti.pixx = ti.Image->size().width();
                                ti.pixy = ti.Image->size().height();

                                ti.csx = zl.tilesizex/ti.pixx;
                                ti.csy = zl.tilesizey/ti.pixy;

                                ti.sx = zl.tilesizex;
                                ti.sy = zl.tilesizey;

                                ti.north = zl.extentbry - float(ti.y) * zl.tilesizey;
                                ti.west = zl.extenttlx + float(ti.x) * zl.tilesizex;

                                ti.red = new float[ti.pixx *ti.pixy];
                                ti.green = new float[ti.pixx *ti.pixy];
                                ti.blue = new float[ti.pixx *ti.pixy];

                                uchar * bits = ti.Image->bits();

                                for(int r = 0; r < ti.Image->size().width(); r++)
                                {
                                    for(int c = 0; c < ti.Image->size().height(); c++)
                                    {
                                        int indexf = r * ti.Image->size().width() + c;
                                        int indexuchar = 4 * (r * ti.Image->size().width() + c);

                                        {
                                            ti.red[indexf] = float(bits[indexuchar + 2])/255.0f;
                                            ti.green[indexf] = float(bits[indexuchar + 1])/255.0f;
                                            ti.blue[indexf] = float(bits[indexuchar + 0])/255.0f;
                                        }

                                    }
                                }

                            }

                            m_TileMutex.lock();
                            m_RequiredTiles.replace(i,ti);
                            m_TileMutex.unlock();

                            break;
                        }
                    }
                }



            }else
            {
            }

        }

        inline int GetRequiredTiles(GeoWindowState state,WorldGLTransformManager * tm)
        {

            m_TileMutex.lock();

            //get widest required extent to cover current window

            WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

            BoundingBox b;
            if(!gltransform->IsGeneric())
            {
                b = gltransform->GetBoundingBox();
            }else
            {
                b.Set(state.tlx,state.brx,state.bry,state.tly);
            }

            //when we have some infinite in the bounding box, load the full backup zoom level to still display something
            bool backup = false;
            if(!b.AllFinite())
            {
                backup = true;
            }


            //zoom level

            float scr_px = state.scr_pixwidth;
            float scr_py = state.scr_pixheight;
            int z = 0;

            int i = 0;

            if(!backup)
            {
                for(i = 0; i < m_TileServerInfo.m_Data.length() -1; i++)
                {

                    if(m_TileServerInfo.m_Data.at(i).tilesizex < (b.GetMaxX() - b.GetMinX()) * ((float)(m_TileServerInfo.m_Data.at(i).pixwidth)/scr_px))
                    {
                        break;
                    }
                }
            }else {
                i = m_TileServerInfo.m_BackupLevel;
            }

            z = i;

            TileServerZoomLevel zl = m_TileServerInfo.m_Data.at(i);

            TileServerZoomLevel zl_detail = m_TileServerInfo.m_Data.at(m_TileServerInfo.m_MaxZoomLevel);




            //calculate which tiles are contained within this extent

            //tile indices
            int x_min;
            int y_min;
            int x_max;
            int y_max;

            if(!backup)
            {
                x_min = floorf((b.GetMinX() - zl.extenttlx)/zl.tilesizex);
                y_min = floorf((b.GetMinY() - zl.extenttly)/zl.tilesizey)-1;
                x_max = floorf((b.GetMaxX() - zl.extenttlx)/zl.tilesizex);
                y_max = floorf((b.GetMaxY() - zl.extenttly)/zl.tilesizey);
            }else
            {
                x_min = 0;
                y_min = 0;
                x_max = zl.matrixwidth;
                y_max = zl.matrixheight;
            }

            QList<TileInfo> RequiredTiles;
            QList<TileInfo> DownloadTiles;
            RequiredTiles.clear();

            for(int x = x_min; x < x_max + 1; x++)
            {
                for(int y = y_min; y < y_max +1; y++)
                {
                    if(x > -1 && x < zl.matrixwidth && y > -1 && y < zl.matrixheight)
                    {
                        TileInfo ti;
                        ti.x = x;
                        ti.y = zl.matrixheight - y-1;
                        ti.z = z;
                        ti.is_in = false;
                        RequiredTiles.append(ti);
                    }
                }
            }

            //check which ones are currently not present in the list
            for(i = 0; i < m_RequiredTiles.length(); i++)
            {
                TileInfo ti = m_RequiredTiles.at(i);
                bool found = false;
                for(int j = 0; j < RequiredTiles.length(); j++)
                {
                    if(m_RequiredTiles.at(i).x == RequiredTiles.at(j).x && m_RequiredTiles.at(i).y == RequiredTiles.at(j).y && m_RequiredTiles.at(i).z == RequiredTiles.at(j).z)
                    {
                        TileInfo tj = RequiredTiles.at(j);
                        tj.is_in= true;

                        RequiredTiles.replace(j,tj);
                        found = true;
                        break;
                    }
                }
                if(found)
                {
                    ti.is_in = true;
                    ti.is_in_current = true;
                    m_RequiredTiles.replace(i,ti);
                }else {
                    ti.is_in_current = false;
                    m_RequiredTiles.replace(i,ti);
                }
            }
            m_RequiresTileRedraw = false;

            int n_download = 0;

            //for each tile that needs to be added and is not yet in the list
            //start the download
            for(int j = 0; j < RequiredTiles.length(); j++)
            {
                if(!RequiredTiles.at(j).is_in)
                {
                    TileInfo ti = RequiredTiles.at(j);

                    //start a download for this tile
                    QString url = m_TileServerInfo.m_Url;
                    url.replace("{x}",QString::number(ti.x));
                    url.replace("{y}",QString::number(ti.y));
                    url.replace("{z}",QString::number(ti.z));

                    ti.download = new DownloadTask(url);
                    ti.download->SetCallBackStart(&UIWebTileLayer::OnDownloadStart,this);
                    ti.download->SetCallBackProgress(&UIWebTileLayer::OnDownloadProgress,this);
                    ti.download->SetCallBackFinished(&UIWebTileLayer::OnDownloadFinished,this);

                    ti.is_in = true;
                    ti.is_in_current = true;
                    m_RequiredTiles.prepend(ti);
                    DownloadTiles.prepend(ti);

                    n_download++;

                    if(state.no_async)
                    {
                        m_TileMutex.unlock();
                        m_DownloadManager->AddDownloadAndWait(ti.download);
                        m_TileMutex.lock();
                    }else
                    {
                        m_DownloadManager->AddDownload(ti.download);
                    }

                    m_RequiresTileRedraw = true;
                }
            }

            int x_detail;
            int y_detail;
            {
                x_detail = floorf((b.GetCenterX() - zl.extenttlx)/zl.tilesizex);
                y_detail = floorf((b.GetCenterY() - zl.extenttly)/zl.tilesizey);
            }

            if(!(m_CentralDetailTile.x == x_detail && m_CentralDetailTile.y == y_detail) && this->m_TileServerInfo.m_IsTiff)
            {
                using namespace std::chrono;
                milliseconds ms = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
                );

                if(ms.count() - time_ms_detailtile > 500)
                {
                    time_ms_detailtile = ms.count();

                    //replace central detail tile
                    m_CentralDetailTile.x = x_detail;
                    m_CentralDetailTile.y = y_detail;
                    m_CentralDetailTile.z =m_TileServerInfo.m_MaxZoomLevel;

                    m_CentralDetailTile.csx = zl_detail.tilesizex/m_CentralDetailTile.pixx;
                    m_CentralDetailTile.csy = zl_detail.tilesizey/m_CentralDetailTile.pixy;

                    m_CentralDetailTile.sx = zl_detail.tilesizex;
                    m_CentralDetailTile.sy = zl_detail.tilesizey;

                    m_CentralDetailTile.north = zl_detail.extentbry - float(m_CentralDetailTile.y) * zl_detail.tilesizey;
                    m_CentralDetailTile.west = zl_detail.extenttlx + float(m_CentralDetailTile.x) * zl_detail.tilesizex;

                    QString url = m_TileServerInfo.m_Url;
                    url.replace("{x}",QString::number(m_CentralDetailTile.x));
                    url.replace("{y}",QString::number(m_CentralDetailTile.y));
                    url.replace("{z}",QString::number(m_TileServerInfo.m_MaxZoomLevel));

                    m_CentralDetailTile.is_done = false;
                    if(!(m_CentralDetailTile.TifMap == nullptr))
                    {
                        delete m_CentralDetailTile.TifMap;
                        m_CentralDetailTile.TifMap = nullptr;
                    }
                    m_CentralDetailTile.download = new DownloadTask(url);
                    m_CentralDetailTile.download->SetCallBackStart(&UIWebTileLayer::OnDownloadStart,this);
                    m_CentralDetailTile.download->SetCallBackProgress(&UIWebTileLayer::OnDownloadProgress,this);
                    m_CentralDetailTile.download->SetCallBackFinished(&UIWebTileLayer::OnDownloadFinished,this);

                    if(state.no_async)
                    {
                        m_TileMutex.unlock();
                        m_DownloadManager->AddDownloadAndWait(m_CentralDetailTile.download);
                        m_TileMutex.lock();
                    }else
                    {
                        m_DownloadManager->AddDownload(m_CentralDetailTile.download);
                    }

                }


            }

            m_TileMutex.unlock();
            return z;
        }


        inline void UpdateMap(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm, int z, bool raw_values = false)
        {
            m_TileMutex.lock();

            int n_draw = 0;
            int n_create = 0;

            int n_glcreate = 0;
            //From all the tiles that are done downloading, create opengltextures
            for(int i = 0; i < m_RequiredTiles.length(); i++)
            {
                TileInfo ti = m_RequiredTiles.at(i);
                if(ti.is_done && !ti.is_gl_created && ti.is_in && (state.no_async || n_glcreate < 10))
                {

                    n_create ++;


                    std::cout << "create opengl tile" << std::endl;

                    n_glcreate ++;

                    if(m_TextureCache.length() > 0)
                    {
                        ti.T_R = m_TextureCache.at(0);
                        m_TextureCache.removeAt(0);
                        ti.T_R->FillData2DRF32(ti.red);
                    }else {
                        ti.T_R = m->GetMGLTexture(ti.pixx,ti.pixy,ti.red,false);
                    }
                    if(m_TextureCache.length() > 0)
                    {
                        ti.T_G = m_TextureCache.at(0);
                        m_TextureCache.removeAt(0);
                        ti.T_G->FillData2DRF32(ti.green);
                    }else {
                        ti.T_G = m->GetMGLTexture(ti.pixx,ti.pixy,ti.green,false);
                    }
                    if(m_TextureCache.length() > 0)
                    {
                        ti.T_B = m_TextureCache.at(0);
                        m_TextureCache.removeAt(0);
                        ti.T_B->FillData2DRF32(ti.blue);
                    }else {
                        ti.T_B = m->GetMGLTexture(ti.pixx,ti.pixy,ti.blue,false);
                    }
                    ti.is_gl_created = true;
                }

                ti.is_used_cache = false;
                ti.do_draw = false;

                m_RequiredTiles.replace(i,ti);
            }

            //if not all tiles are downloaded yet, try to find some other tiles
            //in different zoom levels that are available

            m_RequiredTilesSortedCache =m_RequiredTiles;
            qSort(m_RequiredTilesSortedCache.begin(),m_RequiredTilesSortedCache.end(),TileInfoCompare);

            //for each tile both required and downloaded
            //draw it to the texture
            int n = 0;
            for(int j = 0; j < m_RequiredTilesSortedCache.length(); j++)
            {
                TileInfo ti = m_RequiredTilesSortedCache.at(j);
                bool draw = false;

                float l_width = ti.sx;
                float l_height = ti.sy;
                float l_cx = ti.west + 0.5 * l_width;
                float l_cy = ti.north - 0.5 * l_height;

                if(ti.is_in_current)// && z == ti.z)
                {
                    std::cout << j << "in_current "  << std::endl;
                    if(ti.is_done && ti.is_gl_created)
                    {
                        std::cout << j << "done and created "  << std::endl;

                        ti.do_draw = true;
                        m_RequiredTilesSortedCache.replace(j,ti);
                        draw = true;


                    }else
                    {
                        for(int k = j; k > -1; k--)
                        {
                            TileInfo ti2 = m_RequiredTilesSortedCache.at(k);
                            //if(ti2.z < ti.z)
                            {

                                float l2_width = ti2.sx;
                                float l2_height = ti2.sy;
                                float l2_cx = ti2.west + 0.5 * l2_width;
                                float l2_cy = ti2.north - 0.5 * l2_height;
                                BoundingBox tibb= BoundingBox(l_cx - 0.5 * l_width,l_cx + 0.5 * l_width,l_cy - 0.5 * l_height,l_cy + 0.5 * l_height);
                                BoundingBox ti2bb= BoundingBox(l2_cx - 0.5 * l2_width,l2_cx + 0.5 * l2_width,l2_cy - 0.5 * l2_height,l2_cy + 0.5 * l2_height);
                                //does the bounding box of the larger tile contain the center of the smaller tile?
                                //if(ti2bb.Contains(l_cx,l_cy))//Overlaps(tibb))//
                                {
                                    if(ti2.is_used_cache)
                                    {
                                        //break;
                                    }else if(ti2.is_gl_created && ti2.is_done)
                                    {
                                        draw = true;
                                        ti2.do_draw = true;
                                        ti2.is_used_cache = true;
                                        m_RequiredTilesSortedCache.replace(k,ti2);

                                        //break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            glad_glBindFramebuffer(GL_FRAMEBUFFER,state.GL_FrameBuffer->GetFrameBuffer());
            glad_glDrawBuffer(GL_COLOR_ATTACHMENT0);

            WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());


            for(int j = 0; j < m_RequiredTilesSortedCache.length(); j++)
            {
                TileInfo ti = m_RequiredTilesSortedCache.at(j);

                if(ti.do_draw)
                {
                    n++;

                    //Get style
                    LSMStyle s = GetStyle();

                    OpenGLCLTexture * m_Texture1 = ti.T_R;
                    OpenGLCLTexture * m_Texture2 = ti.T_G;
                    OpenGLCLTexture * m_Texture3 = ti.T_B;

                    if(!(m_TileServerInfo.m_IsTiff))
                    {

                        n_draw ++;

                        float hmax1 = 1.0f;
                        float hmin1 = 0.0f;
                        float hmax2 = 1.0f;
                        float hmin2 = 0.0f;
                        float hmax3 = 1.0f;
                        float hmin3 = 0.0f;

                        //layer geometry
                        float l_width = ti.sx;
                        float l_height = ti.sy;
                        float l_cx = ti.west + 0.5 * l_width;
                        float l_cy = ti.north - 0.5 * l_height;

                        //set shader uniform values
                        OpenGLProgram * program = GLProgram_uimultimap;

                        // bind shader
                        glad_glUseProgram(program->m_program);
                        // get uniform locations


                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h1max"),hmax1);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h1min"),hmin1);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h2max"),hmax2);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h2min"),hmin2);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h3max"),hmax3);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"h3min"),hmin3);

                        int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                        int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                        int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                        int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                        int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                        int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                        int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");

                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),0);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(ti.pixx));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(ti.pixy));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(ti.csx));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(ti.csy));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),state.scr_width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),state.scr_height);

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                            if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(i));
                                ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }


                        // bind texture
                        if(m_Texture1 != nullptr)
                        {
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex1"),0);
                            glad_glActiveTexture(GL_TEXTURE0);
                            glad_glBindTexture(GL_TEXTURE_2D,m_Texture1->m_texgl);
                        }

                        if(m_Texture2 != nullptr)
                        {

                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex2"),1);
                            glad_glActiveTexture(GL_TEXTURE1);
                            glad_glBindTexture(GL_TEXTURE_2D,m_Texture2->m_texgl);
                        }
                        if(m_Texture3 != nullptr)
                        {
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex3"),2);
                            glad_glActiveTexture(GL_TEXTURE2);
                            glad_glBindTexture(GL_TEXTURE_2D,m_Texture3->m_texgl);
                        }

                        if(gltransform != nullptr)
                        {
                            if(!gltransform->IsGeneric())
                            {
                                BoundingBox b = gltransform->GetBoundingBox();

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),b.GetSizeX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),b.GetSizeY());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),b.GetCenterX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),b.GetCenterY());

                                glad_glUniform1i(istransformed_loc,1);
                                glad_glUniform1i(istransformedf_loc,1);

                                glad_glUniform1i(tex_x_loc,3);
                                glad_glActiveTexture(GL_TEXTURE3);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                                glad_glUniform1i(tex_y_loc,4);
                                glad_glActiveTexture(GL_TEXTURE4);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                            }else {

                                glad_glUniform1i(istransformed_loc,0);
                                glad_glUniform1i(istransformedf_loc,0);
                            }

                        }else
                        {
                            glad_glUniform1i(istransformed_loc,0);
                            glad_glUniform1i(istransformedf_loc,0);
                        }

                        // set project matrix
                        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                        glad_glUniform1i(islegend_loc,0);
                        glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                        // now render stuff
                        glad_glBindVertexArray(m->m_Quad->m_vao);
                        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                        glad_glBindVertexArray(0);


                    }else
                    {

                        if(raw_values)
                        {
                            glad_glBindTexture(GL_TEXTURE_2D,m_Texture1->m_texgl);
                            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            glad_glBindTexture(GL_TEXTURE_2D,0);
                        }


                        //layer geometry
                        float l_width = ti.sx;
                        float l_height = ti.sy;
                        float l_cx = ti.west + 0.5 * l_width;
                        float l_cy = ti.north - 0.5 * l_height;

                        //set shader uniform values
                        OpenGLProgram * program = GLProgram_uimap;

                        // bind shader
                        glad_glUseProgram(program->m_program);
                        // get uniform locations

                        int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                        int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                        int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                        int tex_loc = glad_glGetUniformLocation(program->m_program,"tex");
                        int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                        int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                        int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                        int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                        int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                        int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                        int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");


                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),false);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(ti.pixx));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(ti.pixy));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(ti.csx));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(ti.csy));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),state.scr_width);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),state.scr_height);

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                            if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(i));
                                ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }

                        // bind texture
                        glad_glUniform1i(tex_loc,0);
                        glad_glActiveTexture(GL_TEXTURE0);
                        glad_glBindTexture(GL_TEXTURE_2D,m_Texture1->m_texgl);


                        if(raw_values)
                        {
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),state.projection.GetUnitZMultiplier());


                            glad_glUniform1i(israw_loc,1);
                        }else {
                            glad_glUniform1i(israw_loc,0);
                        }



                        if(gltransform != nullptr)
                        {
                            if(!gltransform->IsGeneric())
                            {
                                BoundingBox b = gltransform->GetBoundingBox();

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),b.GetSizeX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),b.GetSizeY());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),b.GetCenterX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),b.GetCenterY());

                                glad_glUniform1i(istransformed_loc,1);
                                glad_glUniform1i(istransformedf_loc,1);

                                glad_glUniform1i(tex_x_loc,1);
                                glad_glActiveTexture(GL_TEXTURE1);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                                glad_glUniform1i(tex_y_loc,2);
                                glad_glActiveTexture(GL_TEXTURE2);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                            }else {

                                glad_glUniform1i(istransformed_loc,0);
                                glad_glUniform1i(istransformedf_loc,0);
                            }

                        }else
                        {
                            glad_glUniform1i(istransformed_loc,0);
                            glad_glUniform1i(istransformedf_loc,0);
                        }

                        glad_glUniform1f(h_max_loc,m_TileServerInfo.m_max);
                        glad_glUniform1f(h_min_loc,m_TileServerInfo.m_min);

                        // set project matrix
                        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                        glad_glUniform1i(islegend_loc,0);
                        glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());


                        // now render stuff
                        glad_glBindVertexArray(m->m_Quad->m_vao);
                        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                        glad_glBindVertexArray(0);

                        if(raw_values)
                        {
                            glad_glBindTexture(GL_TEXTURE_2D,m_Texture1->m_texgl);
                            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                            glad_glBindTexture(GL_TEXTURE_2D,0);
                        }
                    }
                }
            }

            std::cout << "Drew tiles: " << n_draw << std::endl;
            std::cout << "Created tiles: " << n_create << std::endl;
            m_TileMutex.unlock();
        }


        inline void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
        {
            if(IsDraw())
            {
                int z = GetRequiredTiles(s,tm);
                UpdateMap(m,s, tm,z,true);
            }
        }

        inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
        {
            if(IsDraw() && !IsDrawAsDEM())
                {
                    int z = GetRequiredTiles(state,tm);
                    UpdateMap(m,state, tm,z);
                }

        }

        inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
        {
            m_TileMutex.lock();
            int n_in = 0;
            for(int i = m_RequiredTiles.length()-1; i> 0; i--)
            {
                if(!m_RequiredTiles.at(i).is_in)
                {
                    TileInfo ti = m_RequiredTiles.at(i);


                    //cleanup of ti
                    if(!ti.is_done)
                    {
                        m_RequiredTiles.removeAt(i);
                        m_RequiresTileRedraw = true;
                        m_DownloadManager->StopDownload(ti.download);
                    }else
                    {
                        m_RequiredTiles.removeAt(i);
                    }
                }

            }
            for(int i = m_RequiredTiles.length()-1; i> m_TileServerInfo.m_cache_size; i--)
            {
                if(m_RequiredTiles.at(i).is_in)
                {
                    n_in ++;
                }
            }

            for(int i = m_RequiredTiles.length()-1; i> 0; i--)
            {
                if(i > n_in + m_TileServerInfo.m_cache_size)
                {
                    //check if this tile is no longer needed anymore
                    //if so, we either remove or add to cache
                    if(!m_RequiredTiles.at(i).is_in)
                    {
                        TileInfo ti = m_RequiredTiles.at(i);

                        //cleanup of ti
                        if(ti.is_done)
                        {
                            delete[] ti.red;
                            delete[] ti.green;
                            delete[] ti.blue;
                            if(this->m_TileServerInfo.m_IsTiff)
                            {
                                delete ti.TifMap;
                            }else
                            {
                                delete ti.PixMap;
                            }
                            if(ti.is_gl_created)
                            {
                                m_TextureCache.append(ti.T_R);
                                m_TextureCache.append(ti.T_G);
                                m_TextureCache.append(ti.T_B);
                                ti.T_R = nullptr;
                                ti.T_G = nullptr;
                                ti.T_B = nullptr;
                            }
                            if(ti.download != nullptr)
                            {
                                if(ti.download->Download != nullptr)
                                {
                                    //ti.download->Download->deleteLater();
                                    ti.download = nullptr;
                                }
                            }
                        }else {
                            if(ti.download->Download != nullptr)
                            {
                                //m_DownloadManager->StopDownload(ti.download);
                                ti.download = nullptr;
                            }
                        }

                        m_RequiredTiles.removeAt(i);
                        m_RequiresTileRedraw = true;
                    }
                }else if(!m_RequiredTiles.at(i).is_in)
                    {
                        TileInfo ti = m_RequiredTiles.at(i);

                        /*if(ti.is_gl_created)
                        {
                            ti.is_gl_created = false;
                                m_TextureCache.append(ti.T_R);
                                m_TextureCache.append(ti.T_G);
                                m_TextureCache.append(ti.T_B);
                                ti.T_R = nullptr;
                                ti.T_G = nullptr;
                                ti.T_B = nullptr;
                            m_RequiredTiles.replace(i,ti);
                        }*/
                }

            }
            for(int i = m_RequiredTiles.length()-1; i >0; i--)
            {
                TileInfo ti = m_RequiredTiles.at(i);
                ti.is_in = false;
                ti.is_in_current = false;
                m_RequiredTiles.replace(i,ti);
            }
            m_TileMutex.unlock();
        }

        inline bool GetDetailedElevationAtPosition(LSMVector2 pos,  GeoProjection proj, float & val) override
        {
            m_TileMutex.lock();
            if(m_CentralDetailTile.is_done && m_CentralDetailTile.TifMap != nullptr)
            {
                GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

                LSMVector2 locn = transformer->Transform(pos);
                //then if it is within the bounding box of the raster
                BoundingBox b = BoundingBox(m_CentralDetailTile.west,m_CentralDetailTile.west + m_CentralDetailTile.sx,m_CentralDetailTile.north ,m_CentralDetailTile.north + m_CentralDetailTile.sy);

                if(b.Contains(locn.x,locn.y))
                {
                    float frac_x = (locn.x - b.GetMinX())/b.GetSizeX();
                    float frac_z = (locn.y - b.GetMinY())/b.GetSizeY();

                    int r = (float)(frac_z) * m_CentralDetailTile.TifMap->nrRows();
                    int c = (float)(frac_x) * m_CentralDetailTile.TifMap->nrCols();

                    if(r > -1 && c > -1 && r < m_CentralDetailTile.TifMap->nrRows() && c < m_CentralDetailTile.TifMap->nrCols())
                    {
                        float valraw = m_CentralDetailTile.TifMap->data[r][c];
                        if(!pcr::isMV(valraw))
                        {
                            val = valraw;

                            m_TileMutex.unlock();
                            return true;
                        }
                    }



                    m_TileMutex.unlock();
                    return false;
                }

                m_TileMutex.unlock();
                return false;
            }


            m_TileMutex.unlock();
            return false;
        }


        inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
        {

            if(!m_createdTexture)
            {


                    m_createdTexture = true;
            }

            m_IsPrepared = true;
        }

        inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
        {
            m_IsCRSChanged = false;
        }

        inline void OnDestroy(OpenGLCLManager * m) override
        {
            m_TileMutex.lock();

            for(int i = m_RequiredTiles.length()-1; i >0; i--)
            {

                {
                    TileInfo ti = m_RequiredTiles.at(i);
                    m_RequiredTiles.removeAt(i);
                    m_RequiresTileRedraw = true;

                    //cleanup of ti
                    if(ti.is_done)
                    {
                        delete[] ti.red;
                        delete[] ti.green;
                        delete[] ti.blue;
                        if(this->m_TileServerInfo.m_IsTiff)
                        {
                            delete ti.TifMap;
                        }else
                        {
                            delete ti.PixMap;
                        }
                        if(ti.is_gl_created)
                        {
                            ti.T_R->Destroy();
                            ti.T_G->Destroy();
                            ti.T_B->Destroy();
                            delete ti.T_R;
                            delete ti.T_G;
                            delete ti.T_B;
                            delete ti.Image;
                        }
                        if(ti.download != nullptr)
                        {
                            ti.download->Download->deleteLater();
                            delete ti.download;
                        }
                    }else {
                        m_DownloadManager->StopDownload(ti.download);
                    }
                }
            }

            if(!(m_CentralDetailTile.TifMap == nullptr))
            {
                delete m_CentralDetailTile.TifMap;
                m_CentralDetailTile.TifMap = nullptr;
            }


            for(int i = 0; i < m_TextureCache.length(); i++)
            {
                m_TextureCache.at(i)->Destroy();
                delete m_TextureCache.at(i);
            }
            m_TextureCache.clear();


            m_TileMutex.unlock();

            m_IsPrepared = false;
        }

        inline LayerInfo GetInfo()
        {
            LayerInfo l;
            l.Add("Type","Web Tile Layer");
            l.Add("FileName",m_File);
            l.Add("Name",m_Name);
            l.Add(UIGeoLayer::GetInfo());

            return l;
        }


};

