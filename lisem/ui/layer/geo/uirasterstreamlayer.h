#pragma once

#include "defines.h"
#include "layer/geo/uigeolayer.h"
#include "layer/editors/uirasterlayereditor.h"
#include "layer/geo/rasterprovider/rasterdataprovider.h"
#include "QMutex"
#include "gl/openglcldatamanager.h"
#include "QMutex"
#include "layer/editors/uirasterlayereditor.h"
#include "QWaitCondition"

class RasterStreamBuffer
{
public:

     OpenGLCLTexture * GLTexture;
     cTMap * Map;

     GeoProjection projection;
     float tlx = 0.0;
     float tly = 0.0;
     float brx = 0.0;
     float bry = 0.0;
     float width = 0.0;
     float height = 0.0;
     int sizex = 0;
     int sizey = 0;

     float ftlx = 0.0;
     float ftly = 0.0;
     float fbrx = 0.0;
     float fbry = 0.0;
     float fwidth = 0.0;
     float fheight = 0.0;

     int band = 0;

     int fband = 0;

     GeoProjection projection_old;
     float tlx_old = 0.0;
     float tly_old = 0.0;
     float brx_old = 0.0;
     float bry_old = 0.0;
     float width_old = 0.0;
     float height_old = 0.0;

     float cellsize_x = 1.0;
     float cellsize_y = 1.0;

     float cellsize_x_old = 1.0;
     float cellsize_y_old = 1.0;

     std::thread tRead;
     bool bufferused = false;
     bool rRead_Started = false;
     bool write_done = true;
     bool update_gpu = false;

     BoundingBox box;

     BoundingBox box_old;

     QMutex * m_MapMutex;
     QMutex * m_SignMutex;

     inline RasterStreamBuffer(OpenGLCLManager * m, int pix_x, int pix_y, BoundingBox b, GeoProjection p, int in_band)
     {
         int rows = std::max(10,(int)(pix_y * 1.2));
         int cols = std::max(10,(int)(pix_x * 1.2));
         sizex = cols;
         sizey = rows;
         MaskedRaster<float> raster_data(rows,cols,0.0f,0.0f,1.0f);
         Map = new cTMap(std::move(raster_data),"","");
         GLTexture = m->GetMCLGLTexture(&(Map->data),false,false,false);

         band = in_band;

         box = b;
         box_old = b;
         m_MapMutex = new QMutex();
         m_SignMutex = new QMutex();
         //set projection and boundingbox
         projection = p;
         projection_old = p;
         BoundingBox b2 = b;
         b2.Scale(1.0);
         tlx = b2.GetMinX();
         brx = b2.GetMaxX();
         tly = b2.GetMinY();
         bry = b2.GetMaxY();

         width = brx - tlx;
         height = bry - tly;

         tlx_old = tlx;
         brx_old = brx;
         tly_old = tly;
         bry_old = bry;

         width_old = width;
         height_old = height;


         cellsize_x = b2.GetSizeX()/(double(Map->nrCols()));
         cellsize_y = b2.GetSizeY()/(double(Map->nrRows()));
         Map->data._cell_sizeX = b2.GetSizeX()/(double(Map->nrCols()));
         Map->data._cell_sizeY = b2.GetSizeY()/(double(Map->nrRows()));

         cellsize_x_old = cellsize_x;
         cellsize_y_old = cellsize_y;

     }

     bool m_HasFutureFrom = false;
     BoundingBox m_FutureB;
     GeoProjection m_FutureP;
     int m_FutureBand;
     inline void SetFutureFrom(BoundingBox b, GeoProjection p, int in_band)
     {
         BoundingBox b2 = b;
         b2.Scale(1.0);
         ftlx = b2.GetMinX();
         fbrx = b2.GetMaxX();
         ftly = b2.GetMinY();
         fbry = b2.GetMaxY();

         fwidth = fbrx - ftlx;
         fheight = fbry - ftly;
         fband = in_band;

        m_HasFutureFrom = true;
        m_FutureB = b;
        m_FutureP = p;
        m_FutureBand = in_band;

     }
     inline bool HasFutureFrom()
     {

         return m_HasFutureFrom;
     }
     inline void SetFromFuture()
     {
        SetFrom(m_FutureB,m_FutureP,m_FutureBand);
        m_HasFutureFrom = false;


     }

     inline void SetFrom(BoundingBox b, GeoProjection p, int in_band)
     {

         band = in_band;

         tlx_old = tlx;
         brx_old = brx;
         tly_old = tly;
         bry_old = bry;

         width_old = width;
         height_old = height;

         projection_old = projection;

         BoundingBox b2 = b;
         b2.Scale(1.0);
         tlx = b2.GetMinX();
         brx = b2.GetMaxX();
         tly = b2.GetMinY();
         bry = b2.GetMaxY();

         width = brx - tlx;
         height = bry - tly;

         projection = p;

         Map->data._west = b2.GetMinX();
         Map->data._north = b2.GetMinY();

         cellsize_x_old = cellsize_x;
         cellsize_y_old = cellsize_y;

         cellsize_x = b2.GetSizeX()/(double(Map->nrCols()));
         cellsize_y = b2.GetSizeY()/(double(Map->nrRows()));
         Map->data._cell_sizeX = b2.GetSizeX()/(double(Map->nrCols()));
         Map->data._cell_sizeY = b2.GetSizeY()/(double(Map->nrRows()));

     }

     inline void Destroy(OpenGLCLManager * m)
     {
        delete m_MapMutex;
        delete m_SignMutex;
        delete Map;
        Map = nullptr;
        GLTexture->Destroy();
        delete GLTexture;
        GLTexture = nullptr;
     }
};


class UIRasterLayerEditor;


class LISEM_API UIStreamRasterLayer : public UIGeoLayer
{
private:



    //maps for each band that can be used to draw from memory
    QList<OpenGLCLTexture * > m_Textures;
    bool m_CreatedTextures = false;
    int m_TextureIndex = 0;

    int m_CurrentTime = 0;
    int m_CurrentTimeIndex = 0;
    bool m_DataHasChanged = false;
    bool m_HasDoneDraw = false;

    bool m_HasbeenChangedByScript = false;

    //rgb buffers that are filled with stream data, red green blue for each drawing that has to be done for a frame
    QList<RasterStreamBuffer*> m_Buffersr;
    QList<RasterStreamBuffer*> m_Buffersg;
    QList<RasterStreamBuffer*> m_Buffersb;

    bool m_IsInMemory = true;
    RasterDataProvider * m_RDP = nullptr;

    OpenGLProgram * m_Program = nullptr;



    OpenGLCLTexture * m_TextureH = nullptr;
    OpenGLCLTexture * m_TextureN = nullptr;

    OpenGLCLTexture * m_TextureI1 = nullptr;
    OpenGLCLTexture * m_TextureI2 = nullptr;


    QList<BoundingBox> m_DataEdits;


    QMutex m_RedrawNeedMutex;
    bool m_RedrawNeeded;


public:

    inline UIStreamRasterLayer() : UIGeoLayer()
    {
        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;
    }

    inline UIStreamRasterLayer(RasterDataProvider * map, QString name, bool file = false, QString filepath = "", bool native = false, bool model = false) : UIGeoLayer()
    {
        m_IsModel = model;
        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;

        Initialize(map,name,file,filepath,native);

    }
    std::thread m_ThreadRead;

    inline void Initialize(RasterDataProvider * map,QString name,bool file = false, QString filepath = "", bool native = false)
    {

        m_ThreadRead = std::thread((&UIStreamRasterLayer::ReadDataThread),this);

        m_RequiresCRSGLTransform = true;
        m_Style.m_HasDuoBand = false;
        m_Style.m_HasTripleBand = false;
        m_Style.m_HasSingleBand = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;


        UIGeoLayer::Initialize(map->GetProjection(), map->GetBoundingBox(), name,file,filepath,false);

        m_IsInMemory = map->IsMemoryMap();

        std::cout << "rasterstreamlayer is editable ? " << map->IsEditable() << std::endl;
        if(map->IsEditable())
        {
            m_Editable = true;
        }
        m_IsNative = native;
        m_RDP = map;
        m_CouldBeDEM = true;


        if(m_RDP->GetBandCount() > 1)
        {
            m_IsSpectraAble = true;
        }
        m_IsProfileAble = true;

        if(m_RDP->HasTime())
        {
            m_IsTimeSeriesAble = true;
        }

        if(map->GetBandCount() == 1 || map->IsDuoMap())
        {
            m_HasLegend = true;
            m_DrawLegend = true;
        }

        m_Style.m_Bands = map->GetBandCount();

        if(map->IsDuoMap())
        {
            m_Style.m_HasDuoBand = true;
            m_Style.m_HasTripleBand = false;
            m_Style.m_HasSingleBand = false;
            m_Style.m_StyleSimpleGradient = false;
            m_Style.m_StyleSimpleRange = false;

        }else if(map->GetBandCount() == 1)
        {
            m_Style.m_HasDuoBand = false;
            m_Style.m_HasTripleBand = false;
            m_Style.m_HasSingleBand = true;
            m_Style.m_StyleSimpleGradient = true;
            m_Style.m_StyleSimpleRange = true;
        }else {
            m_Style.m_HasDuoBand = false;
            m_Style.m_HasTripleBand = true;
            m_Style.m_HasSingleBand = false;
            m_Style.m_StyleSimpleGradient = false;
            m_Style.m_StyleSimpleRange = false;

            m_Style.m_IndexB1 = 0;
            m_Style.m_IndexB2 = 1;
            if(map->GetBandCount() >2)
            {
                m_Style.m_IndexB3 = 2;
            }else
            {
                m_Style.m_IndexB3 = 1;
            }

        }

        if(map->HasTime())
        {
            m_Style.m_HasTime = true;
            m_Style.m_TimeMax = map->GetMaxTime();
            m_Style.m_TimeMin = map->GetMinTime();
        }

    }

    inline ~UIStreamRasterLayer()
    {

    }

    inline QString layertypeName() override
    {
        return "RasterStreamLayer";
    }

    inline bool IsLayerDirectReplaceable(QList<QList<cTMap *>> maps)
    {
        if(!m_RDP->IsMemoryMap())
        {
            return false;
        }

        {
            if(m_RDP->HasTime()  && (m_RDP->GetTimes().length() > 0))
            {
                if(maps.length() != m_RDP->GetTimes().length())
                {
                    return false;
                }
            }else
            {
                if(maps.length() > 1)
                {
                    return false;
                }
            }

            for(int i = 0; i < maps.length(); i++)
            {
                if(maps.at(i).length() != maps.at(i).length())
                {
                    return false;
                }
                for(int j = 0; j < maps.at(i).length(); j++)
                {

                    cTMap * m = m_RDP->GetMemoryMap(i,j);
                    if(m->nrRows() != maps.at(i).at(j)->nrRows())
                    {
                        return false;
                    }
                    if(m->nrCols() != maps.at(i).at(j)->nrCols())
                    {
                        return false;
                    }
                }
            }

        }
        return true;

    }
    inline void DirectReplace(QList<QList<cTMap *>> maps)
    {
        RasterDataProvider * RDP = new RasterDataProvider(maps,false,true);
        if(m_RDP != nullptr)
        {
            m_RDP->Destroy();
        }
        m_RDP = RDP;
        m_BoundingBox = RDP->GetBoundingBox();
        m_HasbeenChangedByScript = true;

    }
    inline bool IsLayerSaveAble() override
    {
        return (!IsNative()) && m_RDP->IsSaveAble();
    }

    inline bool IsSaveAble() override
    {
        return m_RDP->IsSaveAble();
    }
    inline virtual bool SaveLayerToFile(QString filepath)
    {
        QList<cTMap *> data = m_RDP->CopyMapListAtTime(m_CurrentTimeIndex);

        QString format;
        if(filepath.length() > 3)
        {
            format = GetGDALDriverForRasterFile(filepath);
            bool supportn = GetGDALDriverForRasterFileBandCountSupport(filepath,data.length());

            if(!supportn)
            {
                for(int i = 0; i < data.length(); i++)
                {
                    delete data.at(i);
                }
                data.clear();
                return false;
            }else
            {
                if(data.length() > 1)
                {
                    writeRaster(data,filepath,format);

                }else if(data.length() > 0)
                {
                    writeRaster(*data.at(0),filepath,format);
                }else
                {
                    for(int i = 0; i < data.length(); i++)
                    {
                        delete data.at(i);
                    }
                    data.clear();
                    return false;
                }
            }
        }else
        {
            for(int i = 0; i < data.length(); i++)
            {
                delete data.at(i);
            }
            data.clear();
            return false;
        }

        for(int i = 0; i < data.length(); i++)
        {
            delete data.at(i);
        }
        data.clear();
        return true;
    }

    inline QList<QString> GetSaveExtHint()
    {
        QList<QString> ret;

        QList<QString> drivers = GetMapExtensionGDALDriver();
        QList<QString> extensions = GetMapExtensions();
        QList<bool> mlsup =GetMapExtensionGDALDriverMultiLayerSupport();
        QList<bool> mlsup31 =GetMapExtensionGDALDriverMultiLayerSupport3or1Only();

        for(int i = 0; i < extensions.length(); i++)
        {
            if((m_RDP->GetBandCount() == 1) || (m_RDP->GetBandCount() == 3 && mlsup31.at(i)) || (mlsup.at(i)))
            {
                ret.append(extensions.at(i));
            }
        }
        return ret;
    }

    inline void SaveLayer(QJsonObject * obj) override
    {
        if(m_RDP->IsSaveAble())
        {
            QJsonObject obj_rdp;
            m_RDP->Save(&obj_rdp);

            obj->insert("RasterDataProvider",obj_rdp);
            LSMSerialize::FromBool(obj,"File",m_IsNative);
            LSMSerialize::FromString(obj,"FilePath",m_File);
            LSMSerialize::FromString(obj,"Name",m_Name);

            QJsonObject obj_style;
            m_Style.Save(&obj_style);

            obj->insert("Style",obj_style);

            QString proj;
            LSMSerialize::ToString(obj,"CRS",proj);
            m_Projection.FromString(proj);

        }

    }

    inline void RestoreLayer(QJsonObject * obj)
    {

        if(m_RDP != nullptr)
        {
            delete m_RDP;
        }

        m_RDP = new RasterDataProvider();
        QJsonValue rdpval = (*obj)["RasterDataProvider"];
        if(rdpval.isObject() && !rdpval.isUndefined())
        {
            QJsonObject o = rdpval.toObject();
            m_RDP->load(&o);
        }

        LSMSerialize::ToBool(obj,"File",m_IsNative);
        LSMSerialize::ToString(obj,"FilePath",m_File);
        LSMSerialize::ToString(obj,"Name",m_Name);



        if(!m_RDP->Exists())
        {
            m_Exists = false;
        }else
        {

            Initialize(m_RDP,m_Name,true,m_File,false);

            QJsonValue sval = (*obj)["Style"];
            if(sval.isObject() && !sval.isUndefined())
            {

                LSMStyle st;
                QJsonObject o =sval.toObject();
                st.Restore(&o);
                this->m_Style.CopyFrom(st);
            }

            QString proj;
            LSMSerialize::ToString(obj,"CRS",proj);
            GeoProjection p;
            p.FromString(proj);
            this->SetProjection(p);
        }
    }


    QMutex m_ReadInstructMutex;
    QMutex m_ReadThreadStartMutex;
    QMutex m_ReadThreadDoneMutex;
    QWaitCondition m_ReadThreadWaitCondition;
    QWaitCondition m_ReadThreadDoneCondition;
    std::function<void(void)> m_ReadThreadWork;
    bool m_ReadThreadStop = false;

    inline void ReadDataThread()
    {

        while(true)
        {
            m_ReadThreadStartMutex.lock();
            m_ReadThreadWaitCondition.wait(&m_ReadThreadStartMutex);

            m_ReadThreadDoneMutex.lock();

            if(m_ReadThreadStop)
            {
                m_ReadThreadDoneMutex.unlock();
                m_ReadThreadStartMutex.unlock();
                m_ReadThreadDoneCondition.notify_all();
                break;
            }
            m_ReadThreadWork();


            m_ReadThreadDoneMutex.unlock();
            m_ReadThreadDoneCondition.notify_all();
            m_ReadThreadStartMutex.unlock();
        }

    }



    inline void UpdateGLData()
    {

        for(int i = 0; i < m_DataEdits.length(); i++)
        {
            for(int j = 0; j < m_RDP->GetBandCount(); j++)
            {
                cTMap * m_Map = m_RDP->GetMemoryMap(j,0);
                OpenGLCLTexture * m_Texture = m_Textures.at(j);

                BoundingBox replacebb = m_DataEdits.at(i);

                int minr = replacebb.GetMinX();
                int minc = replacebb.GetMinY();
                int sizer = replacebb.GetSizeX();
                int sizec = replacebb.GetSizeY();

                minr = std::max(0,std::min(m_Map->nrRows()-1,minr-2));
                minc = std::max(0,std::min(m_Map->nrCols()-1,minc-2));
                sizer = std::max(0,std::min(m_Map->nrRows(),std::min(m_Map->nrRows()-std::max(0,(minr-2)),sizer+4)));
                sizec = std::max(0,std::min(m_Map->nrCols(),std::min(m_Map->nrCols()-std::max(0,(minc-2)),sizec+4)));

                MaskedRaster<float> datareplace = MaskedRaster<float>(sizer,sizec,0.0,0.0,1.0);

                for(int r = minr; r < minr + sizer; r++)
                {
                    for(int c = minc; c < minc + sizec; c++)
                    {

                        int r2 = std::max(0,std::min(m_Map->nrRows(),r));
                        int c2 = std::max(0,std::min(m_Map->nrCols(),c));

                        datareplace[r - minr][c - minc] = m_Map->data[r2][c2];
                    }

                }
                glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
                glad_glTexSubImage2D(GL_TEXTURE_2D,0,minc,minr,sizec,sizer,GL_RED,GL_FLOAT,datareplace.data());
                glad_glBindTexture(GL_TEXTURE_2D,0);
            }

        }

        m_DataEdits.clear();
    }

    RasterStreamBuffer *  Fill_RasterStreamBuffers(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm, QList<RasterStreamBuffer*> &Buffers, int band, bool &reused, bool check_ref,RasterStreamBuffer* rsb_ref = nullptr)
    {

        std::cout << "raster fill stream buffer" << std::endl;
        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

        BoundingBox b;
        if(!gltransform->IsGeneric())
        {
            b = gltransform->GetBoundingBox();
        }else
        {
            b.Set(state.tlx,state.brx,state.bry,state.tly);
        }

        if(!b.AllFinite())
        {
            b = GetBoundingBox();
        }

        std::cout << "search box " << b.GetMinX() << " " << b.GetMaxX() << " " << b.GetMinY() << " " << b.GetMaxY() << std::endl;
        std::cout << "search through "  << Buffers.length() <<  "     " << GetBoundingBox().Overlaps(b) << std::endl;
        for(int i = 0; i < Buffers.length(); i++)
        {
            RasterStreamBuffer * rsb = Buffers.at(i);



            rsb->m_SignMutex->lock();


            //check if we have access to the memory
            if(rsb->write_done && !rsb->rRead_Started)
            {

                //now check if the data from the cpu needs to be refreshed to gpu

                if(rsb->update_gpu)
                {

                    if(rsb->tRead.joinable())
                    {
                        rsb->tRead.join();
                    }

                    //std::cout << "data upload"<< std::endl;

                    rsb->update_gpu = false;
                    rsb->SetFromFuture();

                    glad_glBindTexture(GL_TEXTURE_2D,rsb->GLTexture->m_texgl);
                    glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,rsb->Map->nrCols(),rsb->Map->nrRows(),GL_RED,GL_FLOAT,rsb->Map->data[0]);
                    glad_glBindTexture(GL_TEXTURE_2D,0);
                }

            }


            rsb->m_SignMutex->unlock();

        }


        //check if we are out of sight
        if(GetBoundingBox().Overlaps(b))
        {
            BoundingBox bz = GetBoundingBox();

            float pixy = std::max(10,(int)(state.scr_pixwidth * 0.25));
            float pixx = std::max(10,(int)(state.scr_pixheight * 0.25));

            float ratiox = (b.GetSizeX()/pixx)/(std::fabs(m_RDP->GetCellSizeX(band)));
            float ratioy = (b.GetSizeY()/pixy)/(std::fabs(m_RDP->GetCellSizeY(band)));


            std::cout << b.GetSizeX() << " " << b.GetSizeY() << " " << pixx << " "<< pixy << " " << m_RDP->GetCellSizeX(band) << " " << m_RDP->GetCellSizeY(band) <<std::endl;

            if(ratiox < 0.25 || ratioy < 0.25)
            {
                if(ratiox > ratioy)
                {
                    b.ScaleX(1.0/std::max(0.00001f,ratiox));
                    b.ScaleY(1.0/std::max(0.00001f,ratiox));
                }else
                {
                    b.ScaleX(1.0/std::max(0.00001f,ratioy));
                    b.ScaleY(1.0/std::max(0.00001f,ratioy));

                }
            }

            std::cout << pixx << " " << pixy << " " << ratiox << " " << ratioy << std::endl;
            std::cout << "search box " << b.GetMinX() << " " << b.GetMaxX() << " " << b.GetMinY() << " " << b.GetMaxY() << std::endl;


            //limit box of view for the map to the map extent

            BoundingBox blim = bz;
            blim.MoveInto(GetBoundingBox());
            blim.And(b);
            blim.MoveInto(GetBoundingBox());

            BoundingBox bfinal = blim;

            if(check_ref)
            {
                bfinal = BoundingBox(rsb_ref->tlx,rsb_ref->brx, rsb_ref->tly ,rsb_ref->bry);
            }


            std::cout << "search box " << bfinal.GetMinX() << " " << bfinal.GetMaxX() << " " << bfinal.GetMinY() << " " << bfinal.GetMaxY() << std::endl;

            //check if the stream buffer is available for this geowindowstate
            RasterStreamBuffer * rsb = nullptr;

            std::cout << "data has changed ?" << m_DataHasChanged << " " << Buffers.length() << std::endl;

            bool unloaded_exists = false;
            if(!m_DataHasChanged)
            {

                for(int i = 0; i < Buffers.length(); i++)
                {
                    RasterStreamBuffer * rsb_i = Buffers.at(i);

                    //check if this buffer is good in terms of pixels

                    std::cout << "check " << i << " " << rsb_i->bufferused << " " << rsb_i->rRead_Started << " " << band << " " <<  rsb_i->band << std::endl;

                    rsb_i->m_SignMutex->lock();

                    if(rsb_i->rRead_Started)
                    {
                        unloaded_exists = true;

                    }else if(!rsb_i->bufferused && band == rsb_i->band)
                    {
                        std::cout << "check " << i << std::endl;
                        if(rsb_i->sizex >= std::max(10,(int)(0.95 * state.scr_pixwidth)) && rsb_i->sizey >= std::max(10,(int)(0.95 * state.scr_pixheight)) )
                        {

                            std::cout << "check2 " << i << std::endl;

                            if(rsb_i->sizex <= std::max(10,(int)(1.35 * state.scr_pixwidth)) && rsb_i->sizey <= std::max(10,(int)(1.35 * state.scr_pixheight)) )
                            {

                                std::cout << "check3 " << std::endl;
                                BoundingBox b1 = bfinal;
                                BoundingBox b2 = BoundingBox(rsb_i->tlx,rsb_i->brx, rsb_i->tly ,rsb_i->bry);
                                BoundingBox b3 = b1;
                                b3.And(b2);
                                std::cout  << std::setprecision(10) << bfinal.GetMinX() << " " << bfinal.GetMaxX() << " " << bfinal.GetMinY() << " "  << bfinal.GetMaxY() << std::endl;
                                std::cout  << std::setprecision(10) << rsb_i->tlx << " " << rsb_i->brx << " " << rsb_i->tly << " "  << rsb_i->bry << std::endl;
                                std::cout  << std::setprecision(10) << b3.GetMinX() << " " << b3.GetMaxX() << " " << b3.GetMinY() << " "  << b3.GetMaxY() << std::endl;

                                std::cout << b1.GetArea() << " " << b3.GetArea() << " " << !(bfinal.GetMinX() < rsb_i->tlx) << " " << (b3.GetArea() / b1.GetArea())<< std::endl;
                                //!(bfinal.GetMinX() < rsb_i->tlx) && !(bfinal.GetMinY() < rsb_i->tly) && !(bfinal.GetMaxX() > rsb_i->brx) && !(bfinal.GetMaxY() > rsb_i->bry)

                                if(b1.GetArea() > 1e-12)
                                {
                                    //is it inside?
                                    if((b3.GetArea() / b1.GetArea()) > 0.95)
                                    {

                                        std::cout << "check4 " <<  std::endl;

                                        //does it cover at least 90 percent
                                        if(bfinal.GetSizeX() > 0.75*rsb_i->width && bfinal.GetSizeY() > 0.75*rsb_i->height)
                                        {


                                            if(check_ref)
                                            {
                                                BoundingBox bref1 = BoundingBox(rsb_i->tlx,rsb_i->brx, rsb_i->tly ,rsb_i->bry);
                                                BoundingBox bref2 = BoundingBox(rsb_ref->tlx,rsb_ref->brx, rsb_ref->tly ,rsb_ref->bry);
                                                BoundingBox bref3 = bref1;
                                                bref3.And(bref2);

                                                std::cout << (bref3.GetArea() / bref1.GetArea()) << " " << (bref3.GetArea() / bref1.GetArea()) <<" "<<(bref3.GetArea() / bref2.GetArea()) << "  " <<  (bref3.GetArea() / bref2.GetArea()) << std::endl;

                                                if((bref3.GetArea() / bref1.GetArea()) > 0.9995 && (bref3.GetArea() / bref1.GetArea()) < 1.0005)
                                                {

                                                    if((bref3.GetArea() / bref2.GetArea()) > 0.9995 && (bref3.GetArea() / bref2.GetArea()) < 1.0005)
                                                    {
                                                        rsb = rsb_i;
                                                        rsb_i->m_SignMutex->unlock();

                                                        reused = true;

                                                        break;
                                                    }
                                                }

                                            }else
                                            {
                                                std::cout << "found " <<  std::endl;

                                                //std::cout << "found buffer" << std::endl;
                                                rsb = rsb_i;
                                                rsb_i->m_SignMutex->unlock();

                                                reused = true;

                                                break;
                                            }
                                        }
                                    }
                                }

                            }
                        }
                    }

                    rsb_i->m_SignMutex->unlock();


                }
            }

            if(rsb == nullptr)
            {
                reused = false;
            }

            if(unloaded_exists && rsb == nullptr)
            {
                float score_best = 1e31;

                //get one with best score
                for(int i = 0; i < Buffers.length(); i++)
                {
                    RasterStreamBuffer * rsb_i = Buffers.at(i);

                    //check if this buffer is good in terms of pixels

                    BoundingBox box_rsb = BoundingBox(rsb_i->brx,rsb_i->tlx,rsb_i->bry,rsb_i->tly);

                    //calculate score

                    BoundingBox overlap = box_rsb;
                    overlap.And(bfinal);
                    float score = std::fabs(bfinal.GetArea() - overlap.GetArea()) + std::fabs(box_rsb.GetArea() - bfinal.GetArea());

                    if(score < score_best)
                    {
                        score_best = score;
                        rsb = rsb_i;
                    }
                }

                //std::cout << "return best score rsb since unloaded exists" << std::endl;
                return rsb;
            }

            //not found
            if(rsb == nullptr)
            {
                std::cout << "not found, check recycle " << Buffers.length() << std::endl;

                //std::cout << "rsb is not found "<< std::endl;
                if(!check_ref)
                {
                    bfinal.Scale(1.2);
                    bfinal.And(GetBoundingBox());
                }


                //check for one with correct pixels, as to not reallocate memory
                for(int i = 0; i < Buffers.length(); i++)
                {
                    //std::cout << "check reuse buffer " << i << std::endl;
                    RasterStreamBuffer * rsb_i = Buffers.at(i);

                    //check if this buffer is good in terms of pixels

                    std::cout << "recycle? " <<state.scr_pixwidth << " " <<  rsb_i->sizex << " " <<  state.scr_pixheight << " " << rsb_i->sizey << std::endl;
                    rsb_i->m_SignMutex->lock();
                    if(!rsb_i->bufferused && !rsb_i->rRead_Started)
                    {
                        //enough pixels?
                        if(rsb_i->sizex >= std::max(10,(int)(0.95 * state.scr_pixwidth)) && rsb_i->sizey >= std::max(10,(int)(0.95 * state.scr_pixheight)) )
                        {
                            if(rsb_i->sizex <= std::max(10,(int)(1.35 * state.scr_pixwidth)) && rsb_i->sizey <= std::max(10,(int)(1.35 * state.scr_pixheight)) )
                            {
                                //std::cout << "reuse buffer " << std::endl;
                                //reuse
                                rsb = rsb_i;
                                rsb_i->m_SignMutex->unlock();
                                break;

                            }
                        }
                    }
                    rsb_i->m_SignMutex->unlock();
                }

                //make new one
                if(rsb == nullptr)
                {
                    std::cout << "create  buffer " << std::endl;
                    //create new buffer object


                    rsb = new RasterStreamBuffer(m,state.scr_pixwidth,state.scr_pixheight,bfinal,GetProjection(),band);
                    //rsb->SetFutureFrom(bfinal,GetProjection(),band);
                    Buffers.prepend(rsb);

                    //std::cout << "created " << std::endl;
                }
                //do actual loading on a seperate thread

                rsb->m_SignMutex->lock();
                if(rsb->write_done)
                {

                    rsb->rRead_Started = true;


                    //rsb->m_SignMutex->lock();
                    rsb->write_done = false;
                    //rsb->m_SignMutex->unlock();

                    rsb->tRead = std::thread([bfinal,band,this,rsb]()
                    {

                        m_ReadInstructMutex.lock();

                        m_ReadThreadStartMutex.lock();
                        m_ReadThreadDoneMutex.lock();

                        m_ReadThreadWork =  [rsb,bfinal,band,this](){
                            rsb->m_SignMutex->lock();
                            rsb->write_done = false;

                            rsb->m_SignMutex->unlock();


                            //std::cout << "read values pre " << bfinal.GetMinX() << " " << bfinal.GetMaxX() << " " << bfinal.GetMinY() << " " << bfinal.GetMaxY() << std::endl;
                            m_RDP->FillValuesToRaster(bfinal,rsb->Map,rsb->m_MapMutex,&(rsb->write_done),rsb->m_SignMutex,band,m_CurrentTimeIndex,[this, rsb,bfinal,band]()
                            {

                                rsb->m_SignMutex->lock();

                                rsb->SetFutureFrom(bfinal,GetProjection(),band);

                                //std::cout << "read values post " << std::endl;
                                rsb->rRead_Started = false;
                                rsb->write_done = true;
                                rsb->update_gpu = true;
                                rsb->m_SignMutex->unlock();

                                m_RedrawNeedMutex.lock();

                                m_RedrawNeeded = true;

                                m_RedrawNeedMutex.unlock();


                            });
                        };

                        m_ReadThreadStartMutex.unlock();
                        m_ReadThreadWaitCondition.notify_all();
                        m_ReadThreadDoneCondition.wait(&m_ReadThreadDoneMutex);
                        m_ReadThreadDoneMutex.unlock();


                        m_ReadInstructMutex.unlock();


                    });

                    rsb->m_SignMutex->unlock();

                }else
                {
                    rsb->m_SignMutex->unlock();
                }
            }

            rsb->m_SignMutex->lock();

            //std::cout << "set buffer used true"<< std::endl;
            rsb->bufferused = true;

            rsb->m_SignMutex->unlock();

            //std::cout << "fill raster buffers done " << band <<  std::endl;


            return rsb;
        }



        //std::cout << "fill raster buffers done " << band <<  std::endl;

        return nullptr;
    }


    void Draw_Raster(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm,bool raw_value, float zscale = 1.0f)
    {
        m_HasDoneDraw = true;

        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());
        LSMStyle s = GetStyle();
        BoundingBox bb = GetBoundingBox();

        bool is_hs = false;
        if(!raw_value)
        {
            is_hs = s.m_IsHS;
        }


        //if there is a streaming map
        if(!(m_RDP->IsMemoryMap()))
        {

            RasterStreamBuffer * rsb_r = nullptr;
            RasterStreamBuffer * rsb_g = nullptr;
            RasterStreamBuffer * rsb_b = nullptr;


            if(m_RDP->GetBandCount() > 1 && !m_RDP->IsDuoMap())
            {
                int index1 = s.m_IndexB1;
                int index2 = s.m_IndexB2;
                int index3 = s.m_IndexB3;

                bool reused;
                bool reused2;
                bool reused3;
                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,index1,reused,false,nullptr);
                std::cout << "1 is resused?  " << reused << std::endl;
                rsb_g = Fill_RasterStreamBuffers(m,state,tm, m_Buffersg,index2,reused2,reused,rsb_r);
                std::cout << "2 is resused?  " << reused2 << std::endl;
                rsb_b = Fill_RasterStreamBuffers(m,state,tm, m_Buffersb,index3,reused3,reused && reused2,rsb_r);
                std::cout << "3 is resused?  " << reused3 << std::endl;

                if(rsb_r == nullptr || rsb_g == nullptr || rsb_r == nullptr )
                {
                    return;
                }
            }else if(m_RDP->IsDuoMap())
            {
                bool reused;
                bool reused2;
                std::cout << "Get duo buffer 1 " << std::endl;
                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,0,reused,false,nullptr);
                std::cout << "Get duo buffer 2 " << std::endl;
                rsb_g = Fill_RasterStreamBuffers(m,state,tm, m_Buffersg,1,reused2,reused,rsb_r);
                std::cout << "Get duo buffer done " << std::endl;

                if(rsb_r == nullptr || rsb_g == nullptr  )
                {
                    return;
                }
            }else
            {
                bool reused;
                rsb_r = Fill_RasterStreamBuffers(m,state,tm, m_Buffersr,0,reused,false,nullptr);
                if(rsb_r == nullptr)
                {
                    return;
                }
            }


            BoundingBox b;
            if(!gltransform->IsGeneric())
            {
                b = gltransform->GetBoundingBox();
            }else
            {
                b.Set(state.tlx,state.brx,state.bry,state.tly);
            }

            if(!b.AllFinite())
            {
                b = GetBoundingBox();
            }

            //check if we are out of sight
            if(GetBoundingBox().Overlaps(b))
            {

                //now draw this rasterstreambuffer
                if(m_RDP->GetBandCount() == 1 && rsb_r != nullptr)
                {

                    float hmin = m_RDP->GetBandStats(rsb_r->band).min;
                    float hmax = m_RDP->GetBandStats(rsb_r->band).max;


                    if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                    {
                        hmax = s.m_Intervalb1.GetMax() ;
                        hmin = s.m_Intervalb1.GetMin() ;
                    }

                    //layer geometry
                    float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                    float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;
                    float l_cx = rsb_r->tlx+ 0.5f * l_width;
                    float l_cy = rsb_r->tly+ 0.5f * l_height;

                    //set shader uniform values
                    OpenGLProgram * program = GLProgram_uimap;
                    LSMStyle s = GetStyle();

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
                    int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                    int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                    int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");

                    glad_glUniform1f(hsa1_loc,s.angle_hor);
                    glad_glUniform1f(hsa2_loc,s.angle_vert);
                    glad_glUniform1i(ishs_loc,is_hs? 1:0);
                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->cellsize_x));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->cellsize_y));
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
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);

                    if(gltransform != nullptr)
                    {
                        if(!gltransform->IsGeneric())
                        {
                            BoundingBox btrf = gltransform->GetBoundingBox();

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

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
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),raw_value?zscale:s.DemScale);

                    if(raw_value)
                    {
                        glad_glUniform1i(tex_loc,0);
                        glad_glActiveTexture(GL_TEXTURE0);
                        glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


                        glad_glUniform1i(israw_loc,1);
                    }else {
                        glad_glUniform1i(israw_loc,0);
                    }


                    glad_glUniform1f(h_max_loc,hmax);
                    glad_glUniform1f(h_min_loc,hmin);

                    // set project matrix
                    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                    glad_glUniform1i(islegend_loc,0);
                    glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                    // now render stuff
                    glad_glBindVertexArray(m->m_Quad->m_vao);
                    glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                    glad_glBindVertexArray(0);

                    if(raw_value)
                    {
                        glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                    }


                }else if(m_RDP->IsDuoMap() == false && m_RDP->GetBandCount() > 1 && rsb_r != nullptr  && rsb_g != nullptr  && rsb_b != nullptr)
                {

                    if(IsDraw())
                    {
                        WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                        //Get style
                        LSMStyle s = GetStyle();
                        BoundingBox bb = GetBoundingBox();


                        int index1 = s.m_IndexB1;
                        int index2 = s.m_IndexB2;
                        int index3 = s.m_IndexB3;



                        OpenGLCLTexture * m_Texture1 = rsb_r->GLTexture;
                        OpenGLCLTexture * m_Texture2 = rsb_g->GLTexture;
                        OpenGLCLTexture * m_Texture3 = rsb_b->GLTexture;
                        float hmax1 = -1e31f;
                        float hmin1 = 1e31f;
                        float hmax2 = -1e31f;
                        float hmin2 = 1e31f;
                        float hmax3 = -1e31f;
                        float hmin3 = 1e31f;

                        hmax1 = m_RDP->GetBandStats(index1).max;
                        hmin1 = m_RDP->GetBandStats(index1).min;
                        hmax2 = m_RDP->GetBandStats(index2).max;
                        hmin2 = m_RDP->GetBandStats(index2).min;
                        hmax3 = m_RDP->GetBandStats(index3).max;
                        hmin3 = m_RDP->GetBandStats(index3).min;

                        if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                        {
                            hmax1 = s.m_Intervalb1.GetMax() ;
                            hmin1 = s.m_Intervalb1.GetMin() ;
                        }
                        if(!(s.m_Intervalb2.GetMax()  == 0.0f && s.m_Intervalb2.GetMin()  == 0.0f) && s.m_Intervalb2.GetMax()  > s.m_Intervalb2.GetMin() )
                        {
                            hmax2 = s.m_Intervalb2.GetMax() ;
                            hmin2 = s.m_Intervalb2.GetMin() ;
                        }
                        if(!(s.m_Intervalb3.GetMax()  == 0.0f && s.m_Intervalb3.GetMin()  == 0.0f) && s.m_Intervalb3.GetMax()  > s.m_Intervalb3.GetMin() )
                        {
                            hmax3 = s.m_Intervalb3.GetMax() ;
                            hmin3 = s.m_Intervalb3.GetMin() ;
                        }

                        //layer geometry
                        float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                        float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;

                        float l_width1 = ((float)(rsb_g->Map->nrCols()))*rsb_g->cellsize_x;
                        float l_height1 = ((float)(rsb_g->Map->nrRows()))*rsb_g->cellsize_y;

                        float l_width2 = ((float)(rsb_b->Map->nrCols()))*rsb_b->cellsize_x;
                        float l_height2 = ((float)(rsb_b->Map->nrRows()))*rsb_b->cellsize_y;

                        float l_cx = rsb_r->tlx+ 0.5f * l_width;
                        float l_cy = rsb_r->tly+ 0.5f * l_height;

                        float l_cx1 = rsb_g->tlx+ 0.5f * l_width1;
                        float l_cy1 = rsb_g->tly+ 0.5f * l_height1;

                        float l_cx2 = rsb_b->tlx+ 0.5f * l_width2;
                        float l_cy2 = rsb_b->tly+ 0.5f * l_height2;

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
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex1"),l_width1);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey1"),l_height1);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx1"),l_cx1);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy1"),l_cy1);

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex2"),l_width2);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey2"),l_height2);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx2"),l_cx2);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy2"),l_cy2);

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->Map->cellSizeX()));
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->Map->cellSizeY()));
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

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex1"),b.GetSizeX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey1"),b.GetSizeY());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx1"),b.GetCenterX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy1"),b.GetCenterY());

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex2"),b.GetSizeX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey2"),b.GetSizeY());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx2"),b.GetCenterX());
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy2"),b.GetCenterY());

                                glad_glUniform1i(istransformed_loc,1);
                                glad_glUniform1i(istransformedf_loc,1);

                                glad_glUniform1i(tex_x_loc,3);
                                glad_glActiveTexture(GL_TEXTURE3);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                                glad_glUniform1i(tex_y_loc,4);
                                glad_glActiveTexture(GL_TEXTURE4);
                                glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                            }else {

                                std::cout << l_width1/std::max(1e-10f,l_width) << " " << l_width2/std::max(1e-10f,l_width) <<  std::endl;

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relscale1"),l_width1/std::max(1e-10f,l_width));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshiftx1"),((rsb_g->tlx +l_width1) - (rsb_r->tlx + l_width))/std::max(1e-10f,l_width));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshifty1"),((rsb_g->tly +l_height1) - (rsb_r->tly +l_height))/std::max(1e-10f,l_height));

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relscale2"),l_width2/std::max(1e-10f,l_width));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshiftx2"),((rsb_b->tlx +l_width2) - (rsb_r->tlx + l_width))/std::max(1e-10f,l_width));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshifty2"),((rsb_b->tly +l_height2) - (rsb_r->tly +l_height))/std::max(1e-10f,l_height));


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
                    }


                }else if(m_RDP->IsDuoMap())
                {


                    float hmin1 = m_RDP->GetBandStats(rsb_r->band).min;
                    float hmax1 = m_RDP->GetBandStats(rsb_r->band).max;

                    float hmin2 = m_RDP->GetBandStats(rsb_g->band).min;
                    float hmax2 = m_RDP->GetBandStats(rsb_g->band).max;

                    if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                    {
                        hmax1 = s.m_Intervalb1.GetMax() ;
                        hmin1 = s.m_Intervalb1.GetMin() ;
                    }
                    if(!(s.m_Intervalb2.GetMax()  == 0.0f && s.m_Intervalb2.GetMin()  == 0.0f) && s.m_Intervalb2.GetMax()  > s.m_Intervalb2.GetMin() )
                    {
                        hmax2 = s.m_Intervalb2.GetMax() ;
                        hmin2 = s.m_Intervalb2.GetMin() ;
                    }

                    //layer geometry
                    float l_width = ((float)(rsb_r->Map->nrCols()))*rsb_r->cellsize_x;
                    float l_height = ((float)(rsb_r->Map->nrRows()))*rsb_r->cellsize_y;
                    float l_cx = rsb_r->tlx+ 0.5f * l_width;
                    float l_cy = rsb_r->tly+ 0.5f * l_height;

                    //set shader uniform values
                    OpenGLProgram * program = GLProgram_uiduomap;
                    LSMStyle s = GetStyle();

                    // bind shader
                    glad_glUseProgram(program->m_program);
                    // get uniform locations

                    int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                    int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                    int h_max2_loc = glad_glGetUniformLocation(program->m_program,"h_max2");
                    int h_min2_loc = glad_glGetUniformLocation(program->m_program,"h_min2");
                    int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                    int tex_loc = glad_glGetUniformLocation(program->m_program,"tex1");
                    int tex2_loc = glad_glGetUniformLocation(program->m_program,"tex2");
                    int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                    int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                    int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                    int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                    int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                    int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                    int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");
                    int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                    int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                    int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");
                    int vel1_loc = glad_glGetUniformLocation(program->m_program,"is_vel");
                    int vel2_loc = glad_glGetUniformLocation(program->m_program,"vel_spacing");


                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_fractional"),s.m_IsFractional? 1:0);

                    glad_glUniform1i(vel1_loc,s.m_IsVelocity? 1:0);
                    glad_glUniform1f(vel2_loc,s.m_VelSpacing);

                    glad_glUniform1f(hsa1_loc,s.angle_hor);
                    glad_glUniform1f(hsa2_loc,s.angle_vert);
                    glad_glUniform1i(ishs_loc,is_hs? 1:0);
                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(rsb_r->Map->nrCols()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(rsb_r->Map->nrRows()));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(rsb_r->cellsize_x));
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(rsb_r->cellsize_y));
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

                    for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                    {
                        QString is = QString::number(i);
                        int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                        int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                        if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                        {
                            glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb2.m_Gradient_Stops.at(i));
                            ColorF c = s.m_ColorGradientb2.m_Gradient_Colors.at(i);
                            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                        }else {
                            glad_glUniform1f(colorstop_i_loc,1e30f);
                            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                        }
                    }



                    // bind texture
                    glad_glUniform1i(tex_loc,0);
                    glad_glActiveTexture(GL_TEXTURE0);
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_r->GLTexture->m_texgl);

                    glad_glUniform1i(tex2_loc,1);
                    glad_glActiveTexture(GL_TEXTURE1);
                    glad_glBindTexture(GL_TEXTURE_2D,rsb_g->GLTexture->m_texgl);


                    if(gltransform != nullptr)
                    {
                        if(!gltransform->IsGeneric())
                        {
                            BoundingBox btrf = gltransform->GetBoundingBox();

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

                            glad_glUniform1i(istransformed_loc,1);
                            glad_glUniform1i(istransformedf_loc,1);

                            glad_glUniform1i(tex_x_loc,2);
                            glad_glActiveTexture(GL_TEXTURE2);
                            glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                            glad_glUniform1i(tex_y_loc,3);
                            glad_glActiveTexture(GL_TEXTURE3);
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

                    glad_glUniform1i(israw_loc,0);


                    glad_glUniform1f(h_max_loc,hmax1);
                    glad_glUniform1f(h_min_loc,hmin1);

                    glad_glUniform1f(h_max2_loc,hmax2);
                    glad_glUniform1f(h_min2_loc,hmin2);

                    // set project matrix
                    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                    glad_glUniform1i(islegend_loc,0);
                    glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                    // now render stuff
                    glad_glBindVertexArray(m->m_Quad->m_vao);
                    glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                    glad_glBindVertexArray(0);


                }

            }



        //if there is a memory map
        }else {


            //get the relevant GL Textures for the required bands
            if(m_RDP->IsDuoMap() == false)
            {

                //1 band visualization
                if(m_RDP->GetBandCount() == 1)
                {
                    if(m_CreatedTextures)
                    {
                        if(IsDraw() && m_Textures.at(0) != nullptr)
                        {

                            OpenGLCLTexture * Texture = m_Textures.at(0);
                            cTMap * Map = m_RDP->GetMemoryMap(0,0);

                            WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                            //Get style
                            LSMStyle s = GetStyle();
                            BoundingBox bb = GetBoundingBox();

                            float hmax = -1e31f;
                            float hmin = 1e31f;
                            float havg = 0.0f;
                            float n = 0.0;

                            UpdateGLData();

                            hmax = m_RDP->GetBandStats(0).max;
                            hmin = m_RDP->GetBandStats(0).min;


                            if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                            {
                                hmax = s.m_Intervalb1.GetMax() ;
                                hmin = s.m_Intervalb1.GetMin() ;
                            }

                            //layer geometry
                            float l_width = ((float)(Map->nrCols()))*Map->data.cell_sizeX();
                            float l_height = ((float)(Map->nrRows()))*Map->data.cell_sizeY();
                            float l_cx = Map->data.west() + 0.5f * l_width;
                            float l_cy = Map->data.north()+ 0.5f * l_height;

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
                            int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                            int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                            int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");

                            glad_glUniform1f(hsa1_loc,s.angle_hor);
                            glad_glUniform1f(hsa2_loc,s.angle_vert);
                            glad_glUniform1i(ishs_loc,is_hs? 1:0);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),Map->AS_IsLDD? 1:0);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(Map->nrCols()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(Map->nrRows()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(Map->cellSizeX()));
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(Map->cellSizeY()));
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
                            glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);

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

                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),raw_value?zscale:s.DemScale);

                            if(raw_value)
                            {
                                glad_glUniform1i(tex_loc,0);
                                glad_glActiveTexture(GL_TEXTURE0);
                                glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



                                glad_glUniform1i(israw_loc,1);
                            }else {
                                glad_glUniform1i(israw_loc,0);
                            }


                            glad_glUniform1f(h_max_loc,hmax);
                            glad_glUniform1f(h_min_loc,hmin);

                            // set project matrix
                            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                            glad_glUniform1i(islegend_loc,0);
                            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                            // now render stuff
                            glad_glBindVertexArray(m->m_Quad->m_vao);
                            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                            glad_glBindVertexArray(0);

                            if(raw_value)
                            {
                                glad_glBindTexture(GL_TEXTURE_2D,Texture->m_texgl);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                            }
                        }
                    }






                //three band visualization
                }else
                {
                    int index1 = s.m_IndexB1;
                    int index2 = s.m_IndexB2;
                    int index3 = s.m_IndexB3;

                    if(m_CreatedTextures)
                    {
                            if(IsDraw())
                            {
                                WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                                //Get style
                                LSMStyle s = GetStyle();
                                BoundingBox bb = GetBoundingBox();


                                int index1 = s.m_IndexB1;
                                int index2 = s.m_IndexB2;
                                int index3 = s.m_IndexB3;

                                cTMap * m_Map1 = m_RDP->GetMemoryMap(index1,0);
                                cTMap * m_Map2 = m_RDP->GetMemoryMap(index2,0);
                                cTMap * m_Map3 = m_RDP->GetMemoryMap(index3,0);
                                cTMap * m_Map;

                                if(m_Map1 == nullptr && m_Map2 == nullptr && m_Map3 == nullptr)
                                {
                                    return;
                                }else
                                {
                                    if(m_Map1 != nullptr)
                                    {
                                        m_Map =m_Map1;
                                    }else if(m_Map2 != nullptr)
                                    {
                                        m_Map = m_Map2;
                                    }else
                                    {
                                        m_Map = m_Map3;
                                    }
                                }

                                OpenGLCLTexture * m_Texture1 = m_Textures.at(index1);
                                OpenGLCLTexture * m_Texture2 = m_Textures.at(index2);
                                OpenGLCLTexture * m_Texture3 = m_Textures.at(index3);
                                float hmax1 = -1e31f;
                                float hmin1 = 1e31f;
                                float hmax2 = -1e31f;
                                float hmin2 = 1e31f;
                                float hmax3 = -1e31f;
                                float hmin3 = 1e31f;

                                hmax1 = m_RDP->GetBandStats(index1).max;
                                hmin1 = m_RDP->GetBandStats(index1).min;
                                hmax2 = m_RDP->GetBandStats(index2).max;
                                hmin2 = m_RDP->GetBandStats(index2).min;
                                hmax3 = m_RDP->GetBandStats(index3).max;
                                hmin3 = m_RDP->GetBandStats(index3).min;

                                if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                                {
                                    hmax1 = s.m_Intervalb1.GetMax() ;
                                    hmin1 = s.m_Intervalb1.GetMin() ;
                                }
                                if(!(s.m_Intervalb2.GetMax()  == 0.0f && s.m_Intervalb2.GetMin()  == 0.0f) && s.m_Intervalb2.GetMax()  > s.m_Intervalb2.GetMin() )
                                {
                                    hmax2 = s.m_Intervalb2.GetMax() ;
                                    hmin2 = s.m_Intervalb2.GetMin() ;
                                }
                                if(!(s.m_Intervalb3.GetMax()  == 0.0f && s.m_Intervalb3.GetMin()  == 0.0f) && s.m_Intervalb3.GetMax()  > s.m_Intervalb3.GetMin() )
                                {
                                    hmax3 = s.m_Intervalb3.GetMax() ;
                                    hmin3 = s.m_Intervalb3.GetMin() ;
                                }

                                //layer geometry
                                float l_width = ((float)(m_Map1->nrCols()))*m_Map1->data.cell_sizeX();
                                float l_height = ((float)(m_Map1->nrRows()))*m_Map1->data.cell_sizeY();
                                float l_cx = m_Map1->data.west() + 0.5f * l_width;
                                float l_cy = m_Map1->data.north()+ 0.5f * l_height;

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
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(m_Map1->nrCols()));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(m_Map1->nrRows()));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(m_Map1->cellSizeX()));
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(m_Map1->cellSizeY()));
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

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex1"),l_width);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey1"),l_height);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx1"),l_cx);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy1"),l_cy);

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex2"),l_width);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey2"),l_height);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx2"),l_cx);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy2"),l_cy);

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relscale1"),1.0);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshiftx1"),0.0);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshifty1"),0.0);

                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relscale2"),1.0);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshiftx2"),0.0);
                                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_relshifty2"),0.0);

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


                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex1"),b.GetSizeX());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey1"),b.GetSizeY());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx1"),b.GetCenterX());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy1"),b.GetCenterY());

                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex2"),b.GetSizeX());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey2"),b.GetSizeY());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx2"),b.GetCenterX());
                                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy2"),b.GetCenterY());


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
                            }

                    }

                }


            }else {


                std::cout << "draw duo band " << std::endl;

                //duo band visualization

                int index1 = s.m_IndexB1;
                int index2 = s.m_IndexB2;

                OpenGLCLTexture * Texture1 = m_Textures.at(index1);
                cTMap * Map1 = m_RDP->GetMemoryMap(index1,0);

                OpenGLCLTexture * Texture2 = m_Textures.at(index2);
                cTMap * Map2 = m_RDP->GetMemoryMap(index2,0);

                float hmin1 = m_RDP->GetBandStats(index1).min;
                float hmax1 = m_RDP->GetBandStats(index1).max;

                float hmin2 = m_RDP->GetBandStats(index2).min;
                float hmax2 = m_RDP->GetBandStats(index2).max;





                if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                {
                    hmax1 = s.m_Intervalb1.GetMax() ;
                    hmin1 = s.m_Intervalb1.GetMin() ;
                }
                if(!(s.m_Intervalb2.GetMax()  == 0.0f && s.m_Intervalb2.GetMin()  == 0.0f) && s.m_Intervalb2.GetMax()  > s.m_Intervalb2.GetMin() )
                {
                    hmax2 = s.m_Intervalb2.GetMax() ;
                    hmin2 = s.m_Intervalb2.GetMin() ;
                }


                //layer geometry
                float l_width = ((float)(Map1->nrCols()))*Map1->data.cell_sizeX();
                float l_height = ((float)(Map1->nrRows()))*Map1->data.cell_sizeY();
                float l_cx = Map1->data.west() + 0.5f * l_width;
                float l_cy = Map1->data.north()+ 0.5f * l_height;


                //set shader uniform values
                OpenGLProgram * program = GLProgram_uiduomap;
                LSMStyle s = GetStyle();

                // bind shader
                glad_glUseProgram(program->m_program);
                // get uniform locations

                int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                int h_max2_loc = glad_glGetUniformLocation(program->m_program,"h_max2");
                int h_min2_loc = glad_glGetUniformLocation(program->m_program,"h_min2");
                int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                int tex_loc = glad_glGetUniformLocation(program->m_program,"tex1");
                int tex2_loc = glad_glGetUniformLocation(program->m_program,"tex2");
                int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");
                int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");
                int hsa1_loc = glad_glGetUniformLocation(program->m_program,"hs_angle1");
                int hsa2_loc = glad_glGetUniformLocation(program->m_program,"hs_angle2");
                int vel1_loc = glad_glGetUniformLocation(program->m_program,"is_vel");
                int vel2_loc = glad_glGetUniformLocation(program->m_program,"vel_spacing");

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_fractional"),s.m_IsFractional? 1:0);

                glad_glUniform1i(vel1_loc,s.m_IsVelocity? 1:0);
                glad_glUniform1f(vel2_loc,s.m_VelSpacing);
                glad_glUniform1f(hsa1_loc,s.angle_hor);
                glad_glUniform1f(hsa2_loc,s.angle_vert);
                glad_glUniform1i(ishs_loc,is_hs? 1:0);
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_RDP->IsLDD()? 1:0);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(Map1->nrCols()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(Map1->nrRows()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(Map1->cellSizeX()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(Map1->cellSizeY()));
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

                for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                {
                    QString is = QString::number(i);
                    int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                    int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                    if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                    {
                        glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb2.m_Gradient_Stops.at(i));
                        ColorF c = s.m_ColorGradientb2.m_Gradient_Colors.at(i);
                        glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                    }else {
                        glad_glUniform1f(colorstop_i_loc,1e30f);
                        glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                    }
                }



                // bind texture
                glad_glUniform1i(tex_loc,0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,Texture1->m_texgl);

                glad_glUniform1i(tex2_loc,1);
                glad_glActiveTexture(GL_TEXTURE1);
                glad_glBindTexture(GL_TEXTURE_2D,Texture2->m_texgl);


                if(gltransform != nullptr)
                {
                    if(!gltransform->IsGeneric())
                    {
                        BoundingBox btrf = gltransform->GetBoundingBox();

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),btrf.GetSizeX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),btrf.GetSizeY());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),btrf.GetCenterX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),btrf.GetCenterY());

                        glad_glUniform1i(istransformed_loc,1);
                        glad_glUniform1i(istransformedf_loc,1);

                        glad_glUniform1i(tex_x_loc,2);
                        glad_glActiveTexture(GL_TEXTURE2);
                        glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                        glad_glUniform1i(tex_y_loc,3);
                        glad_glActiveTexture(GL_TEXTURE3);
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

                glad_glUniform1i(israw_loc,0);


                glad_glUniform1f(h_max_loc,hmax1);
                glad_glUniform1f(h_min_loc,hmin1);

                glad_glUniform1f(h_max2_loc,hmax2);
                glad_glUniform1f(h_min2_loc,hmin2);

                // set project matrix
                glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                glad_glUniform1i(islegend_loc,0);
                glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                // now render stuff
                glad_glBindVertexArray(m->m_Quad->m_vao);
                glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                glad_glBindVertexArray(0);

           }


        }




    }

    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        m_HasDoneDraw = true;
        LSMStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            return;
        }
        Draw_Raster(m,s,tm,true,s.projection.GetUnitZMultiplier() * GetStyle().DemScale);
    }

    inline void OnDraw3DTransparentLayer(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        m_HasDoneDraw = true;

        //is this a raster that acts as a seperate surface layer (value indicates density of objects)
        //in case of transparancy, we need a double pass that provides us with an indication of the depth of the closest surface fragment
        //we only render for the area contained both within the layer extent and the view extent
        //if available, we only render surface for upper two levels of detail

        //we get the height and coloring of the layer just like the normal mapdraw shader

        LSMStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            float scalebase = s.m_2D3DRenderTargetScales.at(0);

            for(int i = s.GL_FrameBuffer3DWindow.length()-1; i > -1; i--)
            {



                OpenGLCLMSAARenderTarget * TargetC = s.GL_FrameBuffer3DLayerColor;
                OpenGLCLMSAARenderTarget * TargetD = s.GL_FrameBuffer3DLayerElevation;


                {

                    //draw color

                    GeoWindowState sn = s;

                    BoundingBox bn = s.m_2D3DRenderTargetBoundingBox.at(i);

                    sn.scale = bn.GetSizeX();
                    sn.width = bn.GetSizeX();
                    sn.height = bn.GetSizeY();
                    sn.tlx = bn.GetCenterX() - 0.5 * bn.GetSizeX();
                    sn.brx = bn.GetCenterX() + 0.5 * bn.GetSizeX();
                    sn.tly = bn.GetCenterY() - 0.5 * bn.GetSizeY();
                    sn.bry = bn.GetCenterY() + 0.5 * bn.GetSizeY();
                    sn.scr_pixwidth = TargetC->GetWidth();
                    sn.scr_pixheight = TargetC->GetHeight();
                    sn.scr_width = TargetC->GetWidth();
                    sn.scr_height = TargetC->GetHeight();
                    sn.draw_ui = false;
                    sn.draw_legends = false;
                    sn.GL_FrameBuffer = TargetC;
                    sn.ui_scale2d3d = scalebase/s.m_2D3DRenderTargetScales.at(i);

                    GLuint FB = TargetC->GetFrameBuffer();

                    m->m_ShapePainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);
                    m->m_TextPainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);
                    m->m_TexturePainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);
                    m->m_3DPainter->UpdateRenderTargetProperties(FB,sn.scr_pixwidth,sn.scr_pixheight);

                    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
                    glad_glViewport(0,0,sn.scr_pixwidth,sn.scr_pixheight);
                    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glad_glDisable(GL_DEPTH_TEST);
                    glad_glClearColor(0.1,0.1,0.1,1.0);

                    //request raster draw

                    Draw_Raster(m,sn,tm,false);

                    TargetC->BlitToTexture();
                    m->m_ShapePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);
                    m->m_TextPainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);
                    m->m_TexturePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);
                    m->m_3DPainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m->GL_GLOBAL.Width,m->m_height);


                    //draw elevation
                    FB = TargetD->GetFrameBuffer();
                    sn.GL_FrameBuffer = TargetD;
                    sn.scr_pixwidth = TargetD->GetWidth();
                    sn.scr_pixheight = TargetD->GetHeight();
                    sn.scr_width = TargetD->GetWidth();
                    sn.scr_height = TargetD->GetHeight();


                    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
                    glad_glDisable(GL_DEPTH_TEST);
                    glad_glViewport(0,0,sn.scr_pixwidth,sn.scr_pixheight);
                    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                    static GLfloat mv[] = {-1e30f, 0.0f, 0.0f, 1.0f};
                    glad_glClearBufferfv(GL_COLOR, 0, mv);


                    //request raster draw
                    Draw_Raster(m,sn,tm,true,s.projection.GetUnitZMultiplier() * GetStyle().DemScale);



                    TargetD->BlitToTexture();
                }



                s.GL_3DFrameBuffer->SetAsTarget();

                double zscale = 1.0f;//s.projection.GetUnitZMultiplier();

                glad_glDepthMask(GL_TRUE);
                glad_glEnable(GL_DEPTH_TEST);
                glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);
                //set shader uniform values
                OpenGLProgram * program = GLProgram_uiterrainlayer;

                // bind shader
                glad_glUseProgram(program->m_program);

                glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"viewportSize"),s.scr_pixwidth,s.scr_pixheight);
                glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(i).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(i).GetCenterY());
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSize"),s.GL_FrameBuffer3DWindow.at(i).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i).GetSizeY());

                if(i > 0)
                {
                    glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeY());
                }else
                {
                    glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),0,0);
                }
                glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTextureScale"),100.0 * zscale);

                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time/50.0);

                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ZScale"),zscale);
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_flow"),style.m_IsSurfaceFlow);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureC"),0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,TargetC->GetTexture());

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureD"),1);
                glad_glActiveTexture(GL_TEXTURE1);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DElevation.at(i)->GetTexture());

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureDL"),2);
                glad_glActiveTexture(GL_TEXTURE2);
                glad_glBindTexture(GL_TEXTURE_2D,TargetD->GetTexture());

                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenColor"),3);
                glad_glActiveTexture(GL_TEXTURE3);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(0));

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosX"),4);
                glad_glActiveTexture(GL_TEXTURE4);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(1));

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosY"),5);
                glad_glActiveTexture(GL_TEXTURE5);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(2));

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosZ"),6);
                glad_glActiveTexture(GL_TEXTURE6);
                glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(3));



                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureH"),7);
                glad_glActiveTexture(GL_TEXTURE7);
                glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureN"),8);
                glad_glActiveTexture(GL_TEXTURE8);
                glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),9);
                glad_glActiveTexture(GL_TEXTURE9);
                glad_glBindTexture(GL_TEXTURE_2D,m_TextureI1->m_texgl);

                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),10);
                glad_glActiveTexture(GL_TEXTURE10);
                glad_glBindTexture(GL_TEXTURE_2D,m_TextureI2->m_texgl);


                // now render stuff
                glad_glBindVertexArray(m->m_GeometryPlane->m_vao);
                glad_glPatchParameteri(GL_PATCH_VERTICES,3);
                glad_glDrawElements(GL_PATCHES,m->m_GeometryPlane->m_IndexLength,GL_UNSIGNED_INT,0);
                glad_glBindVertexArray(0);

                glad_glBindTexture(GL_TEXTURE_2D,0);
            }

        }


    }

    inline float Distance3DPointLine( LSMVector3 p, LSMVector3 a, LSMVector3 b, float r )
    {
      LSMVector3 pa = p - a, ba = b - a;
      float h = std::max(0.0,std::min( pa.dot(ba)/ba.dot(ba), 1.0 ));
      return (pa - ba*h).length() - r;
    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

        //draw 3d axes and base layer,
        //baseplane automatically shadowmapped, but do not cast shadows themselves
        LSMStyle style = GetStyle();


        std::cout << style.m_BasePlaneScale << " " << style.m_BasePlaneLevel << std::endl;
        //std::cout  << " " << style.m_DrawBasePlane << " " << style.m_DrawEdge << "  " << style.m_Draw3DAxes << std::endl;
        if(this->CouldBeDEM() && this->IsDrawAsDEM())
        {
            BoundingBox bb = this->GetBoundingBox();
            bb.Scale(style.m_BasePlaneScale);

            float hmax = -1e31f;
            float hmin = 1e31f;
            hmax = m_RDP->GetBandStats(0).max;
            hmin = m_RDP->GetBandStats(0).min;
            hmin = hmin + style.m_BasePlaneLevel;

            s.GL_3DFrameBuffer->SetAsTarget();

            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);
            glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);

            double zscale = 1.0f;//s.projection.GetUnitZMultiplier();


            LSMVector3 camerapos = s.Camera3D->GetPosition();

            bool is_in = bb.Contains(camerapos.x,camerapos.z);

            double xmin_rel = bb.GetMinX() - camerapos.x;
            double xmax_rel = bb.GetMaxX() - camerapos.x;
            double zmin_rel = bb.GetMinY() - camerapos.z;
            double zmax_rel = bb.GetMaxY() - camerapos.z;

            //we need to draw all except the closest one

            float dis1 = LSMVector3(xmin_rel,hmin - camerapos.y,zmin_rel).length();
            float dis2 = LSMVector3(xmax_rel,hmin - camerapos.y,zmin_rel).length();
            float dis3 = LSMVector3(xmax_rel,hmin - camerapos.y,zmax_rel).length();
            float dis4 = LSMVector3(xmin_rel,hmin - camerapos.y,zmax_rel).length();

            float dis1t = LSMVector3(xmin_rel,hmax - camerapos.y,zmin_rel).length();
            float dis2t = LSMVector3(xmax_rel,hmax - camerapos.y,zmin_rel).length();
            float dis3t = LSMVector3(xmax_rel,hmax - camerapos.y,zmax_rel).length();
            float dis4t = LSMVector3(xmin_rel,hmax - camerapos.y,zmax_rel).length();

            //we want to draw the vertical legend ticks to the most left visible axis



            //we want to draw the horizontal legend ticks for those two horizontal axes that are closest to the camera

            float line_thickness= 3  * style.m_3DAxesSize;

            if(style.m_DrawBasePlane)
            {

                //m->m_ShapePainter->DrawSquare3D(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),1.0,LSMVector4(style.m_BasePlaneColor.r,style.m_BasePlaneColor.g,style.m_BasePlaneColor.b,style.m_BasePlaneColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                m->m_3DPainter->Draw3DSquare(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),LSMVector4(style.m_BasePlaneColor.r,style.m_BasePlaneColor.g,style.m_BasePlaneColor.b,style.m_BasePlaneColor.a));
            }
            if(style.m_Draw3DAxes)
            {


                float pixelysize_y1 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmax,zmin_rel),s.scr_width,s.scr_height);
                float pixelysize_x1 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmax,zmin_rel),s.scr_width,s.scr_height);
                float pixelysize_x2 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmax,zmax_rel),s.scr_width,s.scr_height);
                float pixelysize_y2 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmax,zmax_rel),s.scr_width,s.scr_height);


                float pixelxpos_1 = s.Camera3D->ProjectVector(LSMVector3(xmin_rel,hmin,zmin_rel),s.scr_width,s.scr_height).x;
                float pixelxpos_2 = s.Camera3D->ProjectVector(LSMVector3(xmax_rel,hmax,zmin_rel),s.scr_width,s.scr_height).x;
                float pixelxpos_3 = s.Camera3D->ProjectVector(LSMVector3(xmax_rel,hmin,zmax_rel),s.scr_width,s.scr_height).x;
                float pixelxpos_4 = s.Camera3D->ProjectVector(LSMVector3(xmin_rel,hmin,zmax_rel),s.scr_width,s.scr_height).x;

                if(!(dis1 < dis2 && dis1 < dis3 && dis1 < dis4))
                {

                }else
                {
                    pixelxpos_1 = 1e30;
                }
                if(!(dis2 < dis1 && dis2 < dis3 && dis2 < dis4))
                {
                }else
                {
                    pixelxpos_2 = 1e30;
                }
                if(!(dis3 < dis1 && dis3 < dis2 && dis3 < dis4))
                {
                }else
                {
                    pixelxpos_3 = 1e30;
                }
                if(!(dis4 < dis1 && dis4 < dis3 && dis4 < dis3))
                {

                }else
                {
                    pixelxpos_4 = 1e30;
                }

                //bottom edges
                float distancey1 = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmin,zmax_rel),0.0);
                float distancex1 = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmin_rel),0.0);
                float distancex2 = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmin,zmax_rel),0.0);
                float distancey2 = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmin,zmin_rel),0.0);

                LSMVector3 axes_start;
                LSMVector3 axes_end;
                int n_tick = 0;

                {
                    if(!(dis1 <= dis2 && dis1 <= dis3 && dis1 <= dis4))
                    {
                        float distance = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmax,zmin_rel),0.0);
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmax,zmin_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distance,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(pixelxpos_1 < pixelxpos_2 && pixelxpos_1 < pixelxpos_3 && pixelxpos_1 < pixelxpos_4)
                        {
                            axes_start = LSMVector3(xmin_rel,hmin,zmin_rel);
                            axes_end =  LSMVector3(xmin_rel,hmax,zmin_rel);
                            n_tick = std::min(10.0, std::max(0.0,pixelysize_y1/(20.0 * line_thickness)));
                            //m->m_TextPainter->DrawText3D(QString::number(hmin),nullptr,LSMVector3(xmin_rel,hmin,zmin_rel),(LSMVector3(xmin_rel,hmin,zmin_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis1,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                            //m->m_TextPainter->DrawText3D(QString::number(hmax),nullptr,LSMVector3(xmin_rel,hmax,zmin_rel),(LSMVector3(xmin_rel,hmax,zmin_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis1t,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                        }
                    }
                    if(!(dis2 <= dis1 && dis2 <= dis3 && dis2 <= dis4))
                    {
                        float distance = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmax,zmin_rel),0.0);
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmax,zmin_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distance,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                        if(pixelxpos_2 < pixelxpos_1 && pixelxpos_2 < pixelxpos_3 && pixelxpos_2 < pixelxpos_4)
                        {
                            axes_start = LSMVector3(xmax_rel,hmin,zmin_rel);
                            axes_end =  LSMVector3(xmax_rel,hmax,zmin_rel);
                            n_tick = std::min(10.0, std::max(0.0,pixelysize_x1/(20.0 * line_thickness)));
                            //m->m_TextPainter->DrawText3D(QString::number(hmin),nullptr,LSMVector3(xmax_rel,hmin,zmin_rel),(LSMVector3(xmax_rel,hmin,zmin_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis2,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                            //m->m_TextPainter->DrawText3D(QString::number(hmax),nullptr,LSMVector3(xmax_rel,hmax,zmin_rel),(LSMVector3(xmax_rel,hmax,zmin_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis2t,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                        }
                    }
                    if(!(dis3 < dis1 && dis3 < dis2 && dis3 < dis4))
                    {
                        float distance = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmax,zmax_rel),0.0);
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmax,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distance,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(pixelxpos_3 < pixelxpos_2 && pixelxpos_3 < pixelxpos_1 && pixelxpos_3 < pixelxpos_4)
                        {
                            axes_start = LSMVector3(xmax_rel,hmin,zmax_rel);
                            axes_end =  LSMVector3(xmax_rel,hmax,zmax_rel);
                            n_tick = std::min(10.0, std::max(0.0,pixelysize_x2/(20.0 * line_thickness)));
                            //m->m_TextPainter->DrawText3D(QString::number(hmin),nullptr,LSMVector3(xmax_rel,hmin,zmax_rel),(LSMVector3(xmax_rel,hmin,zmax_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis3,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                            //m->m_TextPainter->DrawText3D(QString::number(hmax),nullptr,LSMVector3(xmax_rel,hmax,zmax_rel),(LSMVector3(xmax_rel,hmax,zmax_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis3t,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                        }
                    }
                    if(!(dis4 < dis1 && dis4 < dis3 && dis4 < dis3))
                    {
                        float distance = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmax,zmax_rel),0.0);
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmax,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distance,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(pixelxpos_4 < pixelxpos_2 && pixelxpos_4 < pixelxpos_3 && pixelxpos_4 < pixelxpos_1)
                        {
                            axes_start = LSMVector3(xmin_rel,hmin,zmax_rel);
                            axes_end =  LSMVector3(xmin_rel,hmax,zmax_rel);
                            n_tick = std::min(10.0, std::max(0.0,pixelysize_y2/(20.0 * line_thickness)));
                            //m->m_TextPainter->DrawText3D(QString::number(hmin),nullptr,LSMVector3(xmin_rel,hmin,zmax_rel),(LSMVector3(xmin_rel,hmin,zmax_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis4,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                            //m->m_TextPainter->DrawText3D(QString::number(hmax),nullptr,LSMVector3(xmin_rel,hmax,zmax_rel),(LSMVector3(xmin_rel,hmax,zmax_rel)-camerapos).Normalize(),LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * dis4t,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                        }
                    }

                    for(int i = 0; i < n_tick+2; i++)
                    {
                        float step = ((float)(i))/(n_tick + 1.0);
                        LSMVector3 pos = axes_start + step * (axes_end- axes_start);
                        QString text = QString::number(pos.y);
                        if(i ==  n_tick+1)
                        {
                            text = "z = "+ text;
                        }
                        LSMVector3 normal = (pos-LSMVector3(0.0,camerapos.y,0.0)).Normalize();

                        LSMVector3 updir = LSMVector3(0.0,1.0,0.0);
                        LSMVector3 rightdir = -LSMVector3::CrossProduct(normal,updir).Normalize();
                        float disthis = (pos - LSMVector3(0.0,camerapos.y,0.0)).length();
                        float size = line_thickness*0.025* 0.5 * disthis;
                        m->m_TextPainter->DrawText3D(text,nullptr,pos - rightdir * (2.0 + 0.4 * ((float)text.size())) * size,normal,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),size,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);

                    }

                }



                float pixelsize_y1 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmin,zmax_rel),s.scr_width,s.scr_height);
                float pixelsize_x1 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmin_rel),s.scr_width,s.scr_height);
                float pixelsize_x2 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmin,zmax_rel),s.scr_width,s.scr_height);
                float pixelsize_y2 = s.Camera3D->ProjectLineSegmentLength(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmin,zmin_rel),s.scr_width,s.scr_height);

                {
                    LSMVector3 axes_start_x;
                    LSMVector3 axes_end_x;
                    LSMVector3 axes_start_y;
                    LSMVector3 axes_end_y;

                    int n_tick_x = 0;
                    int n_tick_y = 0;

                    {
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distancey1,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                        if(distancey1 < distancey2)
                        {
                            axes_start_y = LSMVector3(xmin_rel,hmin,zmin_rel);
                            axes_end_y = LSMVector3(xmin_rel,hmin,zmax_rel);
                            n_tick_y = std::min(10.0, std::max(1.0,pixelsize_y1/(60.0 * line_thickness)));

                        }

                    }
                    {
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmin_rel,hmin,zmin_rel),LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distancex1,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(distancex1 < distancex2)
                        {
                            axes_start_x = LSMVector3(xmin_rel,hmin,zmin_rel);
                            axes_end_x = LSMVector3(xmax_rel,hmin,zmin_rel);
                            n_tick_x = std::min(10.0, std::max(1.0,pixelsize_x1/(60.0 * line_thickness)));

                        }
                    }
                    {
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmin_rel,hmin,zmax_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distancex2,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(distancex2 < distancex1)
                        {
                            axes_start_x = LSMVector3(xmin_rel,hmin,zmax_rel);
                            axes_end_x = LSMVector3(xmax_rel,hmin,zmax_rel);
                            n_tick_x = std::min(10.0, std::max(1.0,pixelsize_x1/(60.0 * line_thickness)));

                        }
                    }
                    {
                        m->m_ShapePainter->DrawLine3D(LSMVector3(xmax_rel,hmin,zmax_rel),LSMVector3(xmax_rel,hmin,zmin_rel),LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.003* 0.5 * distancey2,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        if(distancey2 < distancey1)
                        {
                            axes_start_y = LSMVector3(xmax_rel,hmin,zmin_rel);
                            axes_end_y = LSMVector3(xmax_rel,hmin,zmax_rel);
                            n_tick_y = std::min(10.0, std::max(1.0,pixelsize_y1/(60.0 * line_thickness)));

                        }
                    }

                    //draw tick lines


                    if(distancey2 < distancey1)
                    {
                        //axes_start_y = LSMVector3(xmin_rel,hmin,zmin_rel);
                        //axes_end_y = LSMVector3(xmin_rel,hmin,zmax_rel);

                        for(int i = 0; i < n_tick+2; i++)
                        {
                            float step = ((float)(i))/(n_tick + 1.0);
                            LSMVector3 pos = axes_start + step * (axes_end- axes_start);
                            LSMVector3 a = LSMVector3(xmin_rel,pos.y,zmin_rel);
                            LSMVector3 b = LSMVector3(xmin_rel,pos.y,zmax_rel);
                            float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                            m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        }



                    }else
                    {
                        //axes_start_y = LSMVector3(xmax_rel,hmin,zmin_rel);
                        //axes_end_y = LSMVector3(xmax_rel,hmin,zmax_rel);

                        for(int i = 0; i < n_tick+2; i++)
                        {
                            float step = ((float)(i))/(n_tick + 1.0);
                            LSMVector3 pos = axes_start + step * (axes_end- axes_start);
                            LSMVector3 a = LSMVector3(xmax_rel,pos.y,zmin_rel);
                            LSMVector3 b = LSMVector3(xmax_rel,pos.y,zmax_rel);
                            float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                            m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        }

                    }


                    if(distancex2 < distancex1)
                    {
                        //axes_start_x = LSMVector3(xmin_rel,hmin,zmin_rel);
                        //axes_end_x = LSMVector3(xmax_rel,hmin,zmin_rel);

                        for(int i = 0; i < n_tick+2; i++)
                        {
                            float step = ((float)(i))/(n_tick + 1.0);
                            LSMVector3 pos = axes_start + step * (axes_end- axes_start);
                            LSMVector3 a = LSMVector3(xmin_rel,pos.y,zmin_rel);
                            LSMVector3 b = LSMVector3(xmax_rel,pos.y,zmin_rel);
                            float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                            m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        }



                    }else
                    {
                        //axes_start_x = LSMVector3(xmin_rel,hmin,zmax_rel);
                        //axes_end_x = LSMVector3(xmax_rel,hmin,zmax_rel);

                        for(int i = 0; i < n_tick+2; i++)
                        {
                            float step = ((float)(i))/(n_tick + 1.0);
                            LSMVector3 pos = axes_start + step * (axes_end- axes_start);
                            LSMVector3 a = LSMVector3(xmin_rel,pos.y,zmax_rel);
                            LSMVector3 b = LSMVector3(xmax_rel,pos.y,zmax_rel);
                            float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                            m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);
                        }


                    }


                    for(int i = 0; i < n_tick_x+2; i++)
                    {
                        float step = ((float)(i))/(n_tick_x + 1.0);
                        LSMVector3 pos = axes_start_x + step * (axes_end_x- axes_start_x);
                        LSMVector3 a = LSMVector3(pos.x,hmin + 1e-6,zmin_rel);
                        LSMVector3 b = LSMVector3(pos.x,hmin + 1e-6,zmax_rel);
                        float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                        m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                    }

                    for(int i = 0; i < n_tick_y+2; i++)
                    {
                        float step = ((float)(i))/(n_tick_y + 1.0);
                        LSMVector3 pos = axes_start_y + step * (axes_end_y- axes_start_y);
                        LSMVector3 a = LSMVector3(xmin_rel,hmin + 1e-6,pos.z);
                        LSMVector3 b = LSMVector3(xmax_rel,hmin + 1e-6,pos.z);
                        float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                        m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                    }

                    for(int i = 0; i < n_tick_x+2; i++)
                    {
                        float ythis = axes_start_y.x;
                        if(distancex2 > distancex1)
                        {
                             ythis = zmax_rel;
                        }else
                        {
                            ythis = zmin_rel;
                        }
                        float step = ((float)(i))/(n_tick_x + 1.0);
                        LSMVector3 pos = axes_start_x + step * (axes_end_x- axes_start_x);
                        LSMVector3 a = LSMVector3(pos.x,hmin,ythis);
                        LSMVector3 b = LSMVector3(pos.x,hmax,ythis);
                        float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                        m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                    }

                    for(int i = 0; i < n_tick_y+2; i++)
                    {
                        float xthis = axes_start_y.x;
                        if(distancey2 > distancey1)
                        {
                             xthis = xmax_rel;
                        }else
                        {
                            xthis = xmin_rel;
                        }
                        float step = ((float)(i))/(n_tick_y + 1.0);
                        LSMVector3 pos = axes_start_y + step * (axes_end_y- axes_start_y);
                        LSMVector3 a = LSMVector3(xthis,hmin,pos.z);
                        LSMVector3 b = LSMVector3(xthis,hmax,pos.z);
                        float disline = Distance3DPointLine(LSMVector3(0.0,camerapos.y,0.0),a,b,0.0);
                        m->m_ShapePainter->DrawLine3D(a,b,LSMVector3(0.0,camerapos.y,0.0),line_thickness*0.0015* 0.5 * disline,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a*0.5),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,true);

                    }


                    //draw tick labels
                    for(int i = 0; i < n_tick_x+2; i++)
                    {
                        float step = ((float)(i))/(n_tick_x + 1.0);
                        LSMVector3 pos = axes_start_x + step * (axes_end_x- axes_start_x);
                        if((axes_start - pos).length() < 1e-6)
                        {
                            continue;
                        }
                        QString text = QString::number(pos.x + camerapos.x);
                        if(i ==  0 && (axes_start_x -LSMVector3(0.0,camerapos.y,0.0)).length() < (axes_end_x -LSMVector3(0.0,camerapos.y,0.0)).length())
                        {
                            text = "x = "+ text;
                        }else if(i == n_tick_x+1 && !((axes_start_x -LSMVector3(0.0,camerapos.y,0.0)).length() < (axes_end_x -LSMVector3(0.0,camerapos.y,0.0)).length()))
                        {
                            text = "x = "+ text;
                        }
                        LSMVector3 normal1 = (pos-LSMVector3(0.0,camerapos.y,0.0)).Normalize();
                        LSMVector3 normal = ((axes_end_x- axes_start_x)).Normalize();
                        bool flip = false;
                        if(normal1.dot(normal) < 0.0)
                        {
                            normal = -normal;
                        }
                        normal = 0.5*(normal + normal1).Normalize();

                        LSMVector3 updir = LSMVector3(0.0,1.0,0.0);
                        LSMVector3 rightdir = -LSMVector3::CrossProduct(normal,updir).Normalize();


                        float mult = 0.7;
                        if((pos- rightdir - LSMVector3((xmin_rel + xmax_rel)*0.5,pos.y,(zmin_rel + zmax_rel)*0.5)).length() < (pos - LSMVector3((xmin_rel + xmax_rel)*0.5,pos.y,(zmin_rel + zmax_rel)*0.5)).length() )
                        {
                            mult = -0.1;
                        }

                       float disthis = (pos - LSMVector3(0.0,camerapos.y,0.0)).length();
                        float size = line_thickness*0.025* 0.5 * disthis;
                        m->m_TextPainter->DrawText3D(text,nullptr,pos-mult * rightdir * ((float)text.size()) * size,normal,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * disthis,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);
                    }


                    for(int i = 0; i < n_tick_y+2; i++)
                    {
                        float step = ((float)(i))/(n_tick_y + 1.0);
                        LSMVector3 pos = axes_start_y + step * (axes_end_y- axes_start_y);

                        if((axes_start - pos).length() < 1e-6)
                        {
                            continue;
                        }
                        QString text = QString::number(pos.z + camerapos.z);
                        if(i ==  0 && (axes_start_y -LSMVector3(0.0,camerapos.y,0.0)).length() < (axes_end_y -LSMVector3(0.0,camerapos.y,0.0)).length())
                        {
                            text = "y = "+ text;
                        }else if(i == n_tick_x+1 && !((axes_start_y -LSMVector3(0.0,camerapos.y,0.0)).length() < (axes_end_y -LSMVector3(0.0,camerapos.y,0.0)).length()))
                        {
                            text = "y = "+ text;
                        }
                        LSMVector3 normal1 = (pos-LSMVector3(0.0,camerapos.y,0.0)).Normalize();
                        LSMVector3 normal =((axes_end_y- axes_start_y)).Normalize();
                        if(normal1.dot(normal) < 0.0)
                        {
                            normal = -normal;
                        }
                        normal = 0.5*(normal + normal1).Normalize();


                        LSMVector3 updir = LSMVector3(0.0,1.0,0.0);
                        LSMVector3 rightdir = -LSMVector3::CrossProduct(normal,updir).Normalize();

                        float mult = 0.6;
                        if((pos- rightdir - LSMVector3((xmin_rel + xmax_rel)*0.5,pos.y,(zmin_rel + zmax_rel)*0.5)).length() < (pos - LSMVector3((xmin_rel + xmax_rel)*0.5,pos.y,(zmin_rel + zmax_rel)*0.5)).length() )
                        {
                            mult = -0.1;
                        }
                        float disthis = (pos - LSMVector3(0.0,camerapos.y,0.0)).length();
                        float size = line_thickness*0.025* 0.5 * disthis;
                        m->m_TextPainter->DrawText3D(text,nullptr,pos- mult *rightdir * ((float)text.size()) * size,normal,LSMVector4(style.m_AxesColor.r,style.m_AxesColor.g,style.m_AxesColor.b,style.m_AxesColor.a),line_thickness*0.025* 0.5 * disthis,camerapos,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),true);

                    }


                }

            }
        }

        //determine line size and line interval


        //determine text size based on distance




        //determine base plane size and level



        //


    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        m_HasDoneDraw = true;
        LSMStyle style = GetStyle();

        if(style.m_IsSurface && this->CouldBeDEM())
        {
            return;
        }
        Draw_Raster(m,s,tm,false);
    }

    inline void CheckDeleteFromBuffers(OpenGLCLManager * m,QList<RasterStreamBuffer *> &list)
    {
        std::cout << "check delete buffer "<< std::endl;


        std::cout << "check buffer delete " << list.length()-1 << std::endl;
        //std::cout << "set all buffers as non-used " << std::endl;
        for(int i = list.length()-1; i >-1 ; i--)
        {
            RasterStreamBuffer * rsb_i =  list.at(i);

            //this buffer has not been used, and we have repeatedly used another, so this must be obsolete
            if(rsb_i->bufferused == false)
            {

                rsb_i->m_SignMutex->lock();



                //destroy and remove buffer
                if(rsb_i->write_done == true && !rsb_i->rRead_Started && !rsb_i->update_gpu)
                {

                    std::cout << "delete rsb " << i << std::endl;
                    rsb_i->m_MapMutex->lock();
                    rsb_i->m_MapMutex->unlock();
                    rsb_i->Destroy(m);
                    list.removeAt(i);

                    rsb_i->m_SignMutex->unlock();
                    delete rsb_i;
                }else
                {
                    rsb_i->m_SignMutex->unlock();
                }


            }else {
                std::cout << "set buffer used false " << i << std::endl;
                rsb_i->bufferused = false;

            }

        }
    }

    inline void DestroyBuffers(OpenGLCLManager * m,QList<RasterStreamBuffer *> &list)
    {
        for(int i =  list.length()-1; i >-1 ; i--)
        {
            RasterStreamBuffer * rsb_i =  list.at(i);

            //std::cout << "check usage" << i <<  std::endl;

            //this buffer has not been used, and we have repeatedly used another, so this must be obsolete

            {
                //destroy and remove buffer
                {
                    //std::cout << "destroy buffer " << std::endl;
                    rsb_i->m_MapMutex->lock();
                    rsb_i->m_MapMutex->unlock();
                    rsb_i->Destroy(m);
                    list.removeAt(i);
                    delete rsb_i;
                }
            }
        }
    }

    inline bool RequiresDraw(GeoWindowState s)
    {
        //we assume that if we indicate a redras is needed, its definately going to happen

        bool ret = false;
        m_RedrawNeedMutex.lock();
        ret = m_RedrawNeeded;
        m_RedrawNeeded = false;
        m_RedrawNeedMutex.unlock();

        return ret;
    }


    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {
        //std::cout << "raster on draw" << std::endl;
        UpdateGLData();

        m_DataHasChanged = false;
        int time = GetStyle().m_CurrentTime;

        if(m_CurrentTime != time)
        {
            m_CurrentTime = time;
            m_CurrentTimeIndex = m_RDP->GetClosestTimeIndex(time);
            CreateGLTextures(m,s);
            m_DataHasChanged = true;

        }else if(m_HasbeenChangedByScript)
        {
            m_HasbeenChangedByScript = false;
            CreateGLTextures(m,s);
            m_DataHasChanged = true;
        }
        if(m_HasDoneDraw)
        {

            CheckDeleteFromBuffers(m,m_Buffersr);
            CheckDeleteFromBuffers(m,m_Buffersg);
            CheckDeleteFromBuffers(m,m_Buffersb);

            m_HasDoneDraw = false;

            //std::cout << "return from delete buffers " << std::endl;
            //return;
        }else
        {

        }


        m_HasDoneDraw = false;

        if(m_RDP->IsNative())
        {
            m_RDP->UpdateBandStats();
        }

    }

    inline void CreateGLTextures(OpenGLCLManager * m,GeoWindowState s)
    {

        std::cout << "create gl textures" << std::endl;

        if(m_RDP->IsMemoryMap())
        {
            if(!m_RDP->IsNative())
            {
                if(!m_CreatedTextures)
                {
                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);
                        OpenGLCLTexture * _T = m->GetMCLGLTexture(&(map->data),false,false,false);
                        this->m_Textures.append(_T);
                    }
                    m_CreatedTextures = true;
                }else
                {
                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);

                        glad_glBindTexture(GL_TEXTURE_2D,m_Textures.at(i)->m_texgl);
                        glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,map->nrCols(),map->nrRows(),GL_RED,GL_FLOAT,map->data[0]);
                        glad_glBindTexture(GL_TEXTURE_2D,0);
                    }
                    m_CreatedTextures = true;
                }
            }else {
                    m_Textures.clear();

                    for(int i = 0; i < m_RDP->GetBandCount(); i++)
                    {
                        cTMap * map = m_RDP->GetMemoryMap(i,m_CurrentTimeIndex);
                        OpenGLCLTexture * _T = m_RDP->GetTexture(i,m_CurrentTimeIndex);
                        this->m_Textures.append(_T);
                    }
                    m_CreatedTextures = true;
             }
        }
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        std::cout << "raster prepare" << std::endl;

        //TODO: get textures from resource manager only once per app
        CreateGLTextures(m,s);

        m_TextureI1= new OpenGLCLTexture();
        m_TextureI1->Create2DFromFile(AssetDir + "abstract.jpg");
        m_TextureI2 = new OpenGLCLTexture();
        m_TextureI2->Create2DFromFile(AssetDir + "cloudnoise.jpg");

        m_TextureH = new OpenGLCLTexture();
        m_TextureH->Create2DFromFile(AssetDir + "ocean_waveheight.jpg");
        m_TextureN = new OpenGLCLTexture();
        m_TextureN->Create2DFromFile(AssetDir + "ocean_normal.png");

        m_IsPrepared = true;

    }

    inline float MinimumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        //as fraction of the vertical screen resolution

        //space = 0.5*text

        //space
        ////Text
        //space
        ////Gradient-Text
        ////Gradient
        ////Gradient-Text
        //space

        //need at least 5.5 * text height


        if((m_RDP->IsDuoMap()) && m_Style.m_IsVelocity)
        {
            return s.ui_textscale * 12.0 * 4.5/s.scr_height;
        }
        return s.ui_textscale * 12.0 * 5.5/s.scr_height; // 5.5 times the text width
    }

    inline float MaximumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        //as fraction of the vertical screen resolution
        if((m_RDP->IsDuoMap()) && m_Style.m_IsVelocity)
        {
            return s.ui_textscale * 12.0 * 4.5/s.scr_height;
        }
        return 1.0; //full height
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s, float posy_start = 0.0, float posy_end = 0.0)
    {
        if(s.legendindex < s.legendtotal)
        {
            if((m_RDP->IsDuoMap()))
            {
                float width = s.scr_pixwidth;
                float height = s.scr_pixheight;

                LSMVector4 LineColor = LSMVector4(0.4,0.4,0.4,1.0);
                float linethickness = std::max(1.0f,1.0f*s.ui_scale);
                float framewidth = 0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
                float ui_ticktextscale = s.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

                float legendwidth =s.ui_scalem*0.085 * width ;
                float legendspacing = s.ui_scalem*0.015* width;

                float legendsq_top = framewidth +posy_start;//framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
                float legendsq_bottom = framewidth +posy_end - 2.0 *12.0 * ui_ticktextscale;//legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
                float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
                float legendsq_right = width - legendspacing;

                float space_text = std::max(50.0f,(legendsq_right  - legendsq_left) * 0.60f);

                float legendcsq_right = std::max(legendsq_left + 10.0f,legendsq_left + (legendsq_right - legendsq_left)-space_text);


                //title

                m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_bottom + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);


                if( m_Style.m_IsVelocity)
                {

                    //draw a single arrow
                    float * x = new float[5];
                    float * y = new float[5];

                    x[0] = legendsq_left;
                    y[0] = legendsq_bottom + 3.0*12 * ui_ticktextscale;
                    x[1] = legendsq_right;
                    y[1] = legendsq_bottom + 3.0*12 * ui_ticktextscale;
                    x[2] = 0.25 * legendsq_left + 0.75 *legendsq_right;
                    y[2] = legendsq_bottom + 4.0*12 * ui_ticktextscale;

                    x[3] = legendsq_right;
                    y[3] = legendsq_bottom + 3.0*12 * ui_ticktextscale;
                    x[4] = 0.25 * legendsq_left + 0.75 *legendsq_right;
                    y[4] = legendsq_bottom + 2.0*12 * ui_ticktextscale;

                    m->m_ShapePainter->DrawLines(x,y,linethickness,5,LineColor);

                    delete[] x;
                    delete[] y;


                }else
                {
                    m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);

                    //Gradient
                    m->m_ShapePainter->DrawSquare(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,LSMVector4(0.1,0.1,0.1,1));

                    m->m_ShapePainter->DrawSquareDoubleGradient(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,&(GetStyleRef()->m_ColorGradientb1),&(GetStyleRef()->m_ColorGradientb2));


                    //ticks

                    LSMVector4 TickColor = LSMVector4(0.2,0.2,0.2,1.0);
                    LSMVector4 TickSmallColor = LSMVector4(0.5,0.5,0.5,1.0);
                    float tickwidth = 2.0f*s.ui_scale;
                    float ticksmallwidth = std::max(2.0f,1.5f * ui_ticktextscale);
                    float ticklength = 7.0f*s.ui_scale;
                    float ticksmalllength = 4.0f* s.ui_scale;
                    float tickdist = 2.0f *s.ui_scale;

                    int n_ticks  = 10;

                    n_ticks = std::min(10,std::max(2,int(std::fabs(legendsq_top-legendsq_bottom)/(2.0 * 12.0*ui_ticktextscale))));

                    double hmax = 0.0;
                    double hmin = 0.0;

                    hmax = m_RDP->GetBandStats(0).max;
                    hmin = m_RDP->GetBandStats(0).min;

                    if(!(GetStyle().m_Intervalb1.GetMax()  == 0.0f && GetStyle().m_Intervalb1.GetMin()  == 0.0f) && GetStyle().m_Intervalb1.GetMax() > GetStyle().m_Intervalb1.GetMin() )
                    {
                        hmax = GetStyle().m_Intervalb1.GetMax() ;
                        hmin = GetStyle().m_Intervalb1.GetMin() ;
                    }

                    for(int i = 0; i < n_ticks + 1; i++)
                    {
                        double val = hmax + float(i) * (hmin -hmax)/float(n_ticks);
                        double y = legendsq_bottom +float(i) * float(legendsq_top -legendsq_bottom)/float(n_ticks);
                        m->m_ShapePainter->DrawLine(legendcsq_right,y,legendcsq_right+ticklength + tickdist,y,tickwidth,TickColor);
                        m->m_TextPainter->DrawString(QString::number(val),NULL,legendcsq_right + tickdist+ticklength+1,y + (i == n_ticks? 0.0:-9.0f* ui_ticktextscale),LSMVector4(0.0,0.0,0.0,1.0),9 * ui_ticktextscale);

                    }


                }

            }else {



                float width = s.scr_pixwidth;
                float height = s.scr_pixheight;

                LSMVector4 LineColor = LSMVector4(0.4,0.4,0.4,1.0);
                float linethickness = std::max(1.0f,1.0f*s.ui_scale);
                float framewidth = 0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
                float ui_ticktextscale = s.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

                float legendwidth =s.ui_scalem*0.085 * width ;
                float legendspacing = s.ui_scalem*0.015* width;

                float legendsq_top = framewidth +posy_start;//framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
                float legendsq_bottom = framewidth +posy_end - 2.0 *12.0 * ui_ticktextscale;//legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
                float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
                float legendsq_right = width - legendspacing;

                float space_text = std::max(50.0f,(legendsq_right  - legendsq_left) * 0.60f);

                float legendcsq_right = std::max(legendsq_left + 10.0f,legendsq_left + (legendsq_right - legendsq_left)-space_text);

                m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);


                //Gradient
                m->m_ShapePainter->DrawSquare(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,LSMVector4(0.1,0.1,0.1,1));

                m->m_ShapePainter->DrawSquareGradient(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,&(GetStyleRef()->m_ColorGradientb1));

                //title

                m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_bottom + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);


                //ticks

                LSMVector4 TickColor = LSMVector4(0.2,0.2,0.2,1.0);
                LSMVector4 TickSmallColor = LSMVector4(0.5,0.5,0.5,1.0);
                float tickwidth = 2.0f*s.ui_scale;
                float ticksmallwidth = std::max(2.0f,1.5f * ui_ticktextscale);
                float ticklength = 7.0f*s.ui_scale;
                float ticksmalllength = 4.0f* s.ui_scale;
                float tickdist = 2.0f *s.ui_scale;

                int n_ticks  = 10;

                n_ticks = std::min(10,std::max(2,int(std::fabs(legendsq_top-legendsq_bottom)/(2.0 * 12.0*ui_ticktextscale))));

                double hmax = 0.0;
                double hmin = 0.0;

                hmax = m_RDP->GetBandStats(0).max;
                hmin = m_RDP->GetBandStats(0).min;

                if(!(GetStyle().m_Intervalb1.GetMax()  == 0.0f && GetStyle().m_Intervalb1.GetMin()  == 0.0f) && GetStyle().m_Intervalb1.GetMax() > GetStyle().m_Intervalb1.GetMin() )
                {
                    hmax = GetStyle().m_Intervalb1.GetMax() ;
                    hmin = GetStyle().m_Intervalb1.GetMin() ;
                }

                for(int i = 0; i < n_ticks + 1; i++)
                {
                    double val = hmax + float(i) * (hmin -hmax)/float(n_ticks);
                    double y = legendsq_bottom +float(i) * float(legendsq_top -legendsq_bottom)/float(n_ticks);
                    m->m_ShapePainter->DrawLine(legendcsq_right,y,legendcsq_right+ticklength + tickdist,y,tickwidth,TickColor);
                    m->m_TextPainter->DrawString(QString::number(val),NULL,legendcsq_right + tickdist+ticklength+1,y + (i == n_ticks? 0.0:-9.0f* ui_ticktextscale),LSMVector4(0.0,0.0,0.0,1.0),9 * ui_ticktextscale);

                }
            }
        }
    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_IsCRSChanged = false;
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_ReadInstructMutex.lock();
        m_ReadThreadStartMutex.lock();
        m_ReadThreadDoneMutex.lock();
        m_ReadThreadStop = true;
        m_ReadThreadStartMutex.unlock();
        m_ReadThreadDoneMutex.unlock();
        m_ReadThreadWaitCondition.notify_all();
        m_ReadInstructMutex.unlock();

        if(m_TextureH != nullptr)
        {
            m_TextureH->Destroy();
            delete m_TextureH;
            m_TextureH = nullptr;
        }
        if(m_TextureN != nullptr)
        {
            m_TextureN->Destroy();
            delete m_TextureN;
            m_TextureN = nullptr;
        }
        if(m_TextureI1 != nullptr)
        {
            m_TextureI1->Destroy();
            delete m_TextureI1;
            m_TextureI1 = nullptr;
        }
        if(m_TextureI2 != nullptr)
        {
            m_TextureI2->Destroy();
            delete m_TextureI2;
            m_TextureI2 = nullptr;
        }


        //destroy all rasterstreambuffers
        DestroyBuffers(m,m_Buffersr);
        DestroyBuffers(m,m_Buffersg);
        DestroyBuffers(m,m_Buffersb);

        if(!m_RDP->IsNative())
        {
            //destroy all openGL textures for maps
            for(int i = 0; i < m_Textures.length(); i++)
            {
                m_Textures.at(i)->Destroy();
            }
        }

        m_Textures.clear();
        m_CreatedTextures = false;

        //destroy rasterdataprovider
        m_RDP->Destroy();
        delete m_RDP;

        m_IsPrepared = false;
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline double GetMinimumValue(int i) override
    {
        return m_RDP->GetBandStats(i).min;
    }

    inline double GetAverageValue(int i) override
    {
        return m_RDP->GetBandStats(i).mean;
    }

    inline double GetMaximumValue(int i) override
    {
        return m_RDP->GetBandStats(i).max;
    }


    //the camera must be able to request special ele
    inline bool GetDetailedElevationAtPosition(LSMVector2 pos,  GeoProjection proj, float & val) override
    {
        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        LSMVector2 locn = transformer->Transform(pos);
        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();

        if(b.Contains(locn.x,locn.y))
        {
            float valraw = m_RDP->GetValueAtLocation(locn,LISEM_RASTER_SAMPLE_NEAREST,0,m_CurrentTimeIndex);
            if(!pcr::isMV(valraw))
            {
                val = valraw;
                return true;
            }

            return false;
        }
        return false;
    }



    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        //convert location to local coordinate system
        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        LSMVector2 locn = transformer->Transform(Pos);
        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();

        if(b.Contains(locn.x,locn.y))
        {
            p.hit = true;

            float valraw = m_RDP->GetValueAtLocation(locn,LISEM_RASTER_SAMPLE_NEAREST,0,m_CurrentTimeIndex);

            QString value = QString::number(valraw);
            if(pcr::isMV(valraw))
            {
                value = "Missing Value";
            }

            //finally get a cell location
            p.AttrNames.append(QString("Raster Value"));
            p.AttrValues.append(value);
        }

        return p;
    }

    inline LayerInfo GetInfo()
    {
        LayerInfo l;
        l.Add("Type","Raster Layer");
        l.Add("FileName",m_RDP->GetFilePathAtTime(0));
        l.Add("Name",m_Name);
        l.Add(UIGeoLayer::GetInfo());
        l.Add("Nr of Bands", QString::number(m_RDP->GetBandCount()));
        l.Add("Nr of Rows", QString::number(m_RDP->GetTotalSizeY()));
        l.Add("Nr of Cols", QString::number(m_RDP->GetTotalSizeX()));
        l.Add("CellSize x", QString::number(m_RDP->GetCellSizeX()));
        l.Add("CellSize y", QString::number(m_RDP->GetCellSizeY()));
        l.Add("Timeslices", QString::number(m_RDP->GetTimes().length()));
        return l;
    }

    inline cTMap * GetMap()
    {
        if(m_RDP->GetBandCount()==1 && m_RDP->HasTime() == false)
        {
            if(m_RDP->IsMemoryMap())
            {
                return m_RDP->GetMemoryMap(0,0);
            }
        }

        return nullptr;//m_Map;
    }

    inline QList<cTMap *> GetMaps() override
    {
        if(m_RDP->HasTime() == false)
        {
            if(m_RDP->IsMemoryMap())
            {
                QList<cTMap * > ret;
                for(int i = 0; i < m_RDP->GetBandCount(); i++)
                {

                   ret.append(m_RDP->GetMemoryMap(i,0));
                }
                return ret;
            }
        }

        QList<cTMap*> ret;
        return ret;//m_Map;
    }
    inline OpenGLCLTexture * GetMainTexture()
    {
        if(m_RDP->GetBandCount()==1 && m_RDP->HasTime() == false)
        {
            if(m_RDP->IsMemoryMap())
            {
                return m_Textures.at(0);
            }
        }

        return nullptr;//m_Texture;
    }

    inline QList<OpenGLCLTexture *> GetMainTextures()
    {

        return m_Textures;

    }



    inline virtual MatrixTable * GetTimeSeries(QList<LSMVector2> Pos, GeoProjection proj)
    {
        //allocate matrixtable
        MatrixTable * m = new MatrixTable();

        if(m_RDP->HasTime())
        {
            //get coordinate transformer

            GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

            //get all time values
            QList<int> times = m_RDP->GetTimes();

            //get the number of position (nr of cols), add 1 for time
            //get the number of timesteps (nr of rows)
            m->SetSize(times.length(),Pos.length()+1);

            for(int i = 0; i < times.length(); i++)
            {
                m->SetValue(i,0,times.at(i));
            }

            for(int j = 0; j < Pos.length(); j++)
            {
                LSMVector2 pos = Pos.at(j);

                if(!transformer->IsGeneric())
                {
                    pos = transformer->Transform(pos);
                }

                QList<double> ts = m_RDP->GetTimeSeries(pos);

                for(int i = 0; i < times.length(); i++)
                {
                    m->SetValue(i,j+1,ts.at(i));
                }
            }

            delete transformer;

        }
        return m;
    }



    inline virtual MatrixTable * GetProfile(QList<LSMVector2> Pos, GeoProjection proj)
    {
        MatrixTable * m = new MatrixTable();


        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        QList<LSMVector2> pos_tr;


        for(int j = 0; j < Pos.length(); j++)
        {
            LSMVector2 pos = Pos.at(j);

            if(!transformer->IsGeneric())
            {
                pos = transformer->Transform(pos);
                pos_tr.append(pos);
            }else
            {
                pos_tr.append(pos);
            }
        }

        int pcount = 0;

        for(int j = 0; j < Pos.length()-1; j++)
        {

            LSMVector2 pos1 = pos_tr.at(j);
            LSMVector2 pos2 = pos_tr.at(j+1);

            LSMVector2 posf1 = pos_tr.at(j);
            LSMVector2 posf2 = pos_tr.at(j+1);


            //get required point count
            int count = m_RDP->GetLineWithinRasterExtent(pos1,pos2,posf1,posf2);

            pcount += count;
        }

        m->SetSize(pcount,2);

        double distance = 0.0;

        int  k = 0;

        for(int j = 0; j < Pos.length()-1; j++)
        {

            LSMVector2 pos1 = pos_tr.at(j);
            LSMVector2 pos2 = pos_tr.at(j+1);

            LSMVector2 posf1 = pos_tr.at(j);
            LSMVector2 posf2 = pos_tr.at(j+1);

            //get required point count and fixes line;
            int count_this = m_RDP->GetLineWithinRasterExtent(pos1,pos2,posf1,posf2);

            LSMVector2 pos_this = posf1;
            LSMVector2 delta = (posf2-posf1)/((float)(count_this));
            double delta_l = delta.length();

            //get all values along this line, excluding the end point (except if it is the last line segment)

            for(int l = 0; l < count_this; l++)
            {
                double val = m_RDP->GetValueAtLocation(pos_this,LISEM_RASTER_SAMPLE_LINEAR,0,m_CurrentTimeIndex);
                m->SetValue(k,0,distance);
                m->SetValue(k,1,val);
                distance += delta_l;
                pos_this = pos_this + delta;
                k++;
            }
        }


        delete transformer;

        return m;
    }

    inline virtual MatrixTable * GetSpectra(QList<LSMVector2> Pos, GeoProjection proj)
    {
        MatrixTable * m = new MatrixTable();

        int n_bands = m_RDP->GetBandCount();

        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        QList<LSMVector2> pos_tr;


        m->SetSize(n_bands,Pos.length()+1);

        for(int k= 0; k < n_bands; k++)
        {
            m->SetValue(k,0,k);
        }

        for(int j = 0; j < Pos.length(); j++)
        {
            LSMVector2 pos = Pos.at(j);

            if(!transformer->IsGeneric())
            {
                pos = transformer->Transform(pos);
                pos_tr.append(pos);
            }else
            {
                pos_tr.append(pos);
            }

            for(int k= 0; k < n_bands; k++)
            {
                double val = m_RDP->GetValueAtLocation(pos,LISEM_RASTER_SAMPLE_NEAREST,0,m_CurrentTimeIndex);
                m->SetValue(k,j+1,val);
            }

        }

        delete transformer;

        return m;
    }



    inline void NotifyDataChange(BoundingBox b)
    {
        //b should contain row and column bounds to replace
        m_DataEdits.append(b);

    }



    UILayerEditor* GetEditor();



};


inline UIStreamRasterLayer * GetUIModelRasterLayer(cTMap * map, OpenGLCLTexture * m, QString name,QString file, bool native)
{

    QList<QList<cTMap *>> lm;
    QList<QList<OpenGLCLTexture *>> lt;

    QList<cTMap*> lm1;
    lm1.append(map);
    lm.append(lm1);

    QList<OpenGLCLTexture*> lt1;
    lt1.append(m);
    lt.append(lt1);

    return new UIStreamRasterLayer(new RasterDataProvider(lm,lt,false,false),name,false,file,true,true);

}

inline UIStreamRasterLayer * GetUIModelDuoRasterLayer(cTMap * map, OpenGLCLTexture * m,cTMap * map2, OpenGLCLTexture * m2, QString name,QString file, bool native)
{

    QList<QList<cTMap *>> lm;
    QList<QList<OpenGLCLTexture *>> lt;

    QList<cTMap*> lm1;
    lm1.append(map);
    lm1.append(map2);
    lm.append(lm1);
    QList<OpenGLCLTexture*> lt1;
    lt1.append(m);
    lt1.append(m2);
    lt.append(lt1);
    return new UIStreamRasterLayer(new RasterDataProvider(lm,lt,true,false),name,false,file,true,true);

}

