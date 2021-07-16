#include "worldwindow.h"
#include "layer/geo/uirasterstreamlayer.h"
#include "layer/geo/rasterprovider/rasterdataprovider.h"
#include "layer/3d/gl3docean.h"
#include "layer/3d/uicloudlayer.h"
#include "layer/geo/uirigidworldlayer.h"
#include "ioassimp.h"
#include "layer/3d/ui3dobjectlayer.h"

void WorldWindow::AddOceanLayer()
{
    UILayer * ml2 = new UIOceanLayer();
    AddUILayer(ml2,true);
}

void WorldWindow::AddSkyBoxLayer()
{
    UILayer * ml2 = new UISkyBoxLayer();
    AddUILayer(ml2,true);
}

void WorldWindow::AddCloudLayer()
{
    UILayer * ml2 = new UICloudLayer();
    AddUILayer(ml2,true);
}
int WorldWindow::AddUILayerAt(UILayer *ML, bool emitsignal, bool do_zoom, int i)
{
    m_UILayerMutex.lock();

    if(GetUILayers().length() == 0)
    {
        if(ML->IsGeo())
        {
            if(do_zoom)
            {
                LookAt((UIGeoLayer*)(ML));
            }
        }
    }

    //find new id number that is the lowest available positive integer
    int id = 0;

    if (m_UILayerList.length() > 0) {
        int * il = new int[m_UILayerList.length()];

        for(int i = m_UILayerList.length()-1; i > -1; i--)
        {
            il[i] = m_UILayerList.at(i)->GetID();
        }

        id = LISEMMath::findMissingPositive(il,m_UILayerList.length());
        delete[] il;

    }else {
        id = 1;
    }

    ML->SetID(id);
    m_uidcount ++;
    ML->SetUID(m_uidcount);
    m_UILayerList.insert(i,ML);
    m_UILayersChanged = true;

    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }
    return 0;
}


int WorldWindow::AddUILayer(UILayer *ML, bool emitsignal, bool do_zoom)
{
    m_UILayerMutex.lock();

    if(GetUILayers().length() == 0)
    {
        if(ML->IsGeo())
        {
            if(do_zoom)
            {
                LookAt((UIGeoLayer*)(ML));
            }
        }
    }

    //find new id number that is the lowest available positive integer
    int id = 0;

    if (m_UILayerList.length() > 0) {
        int * il = new int[m_UILayerList.length()];

        for(int i = m_UILayerList.length()-1; i > -1; i--)
        {
            il[i] = m_UILayerList.at(i)->GetID();
        }

        id = LISEMMath::findMissingPositive(il,m_UILayerList.length());
        delete[] il;

    }else {
        id = 1;
    }

    ML->SetID(id);
    m_uidcount ++;
    ML->SetUID(m_uidcount);
    m_UILayerList.append(ML);
    m_UILayersChanged = true;

    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }
    return 0;
}
UILayer * WorldWindow::GetUILayerFromName(QString name)
{

    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(name.compare(m_UILayerList.at(i)->GetName())== 0)
        {
            m_UILayerMutex.unlock();
            return m_UILayerList.at(i);
        }
    }

    m_UILayerMutex.unlock();

    return nullptr;
}

UILayer * WorldWindow::GetUILayerFromUID(int id)
{

    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(id == m_UILayerList.at(i)->GetUID())
        {
            m_UILayerMutex.unlock();
            return m_UILayerList.at(i);
        }
    }

    m_UILayerMutex.unlock();

    return nullptr;
}

UILayer * WorldWindow::GetUILayerFromID(int id)
{

    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(id == m_UILayerList.at(i)->GetID())
        {
            m_UILayerMutex.unlock();
            return m_UILayerList.at(i);
        }
    }

    m_UILayerMutex.unlock();

    return nullptr;
}

UILayer * WorldWindow::GetUILayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());
    /*QList<cTMap *> _M;

    try
    {
        _M = readRasterList(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UILayer * ret;

    if(_M.length() == 1)
    else if(_M.length() > 1)
        {
            UIMultiRasterLayer *HS = new UIMultiRasterLayer(_M,filename,true,path);
            HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
            ret = HS;
        }*/


    {
        RasterDataProvider * rdp = new RasterDataProvider(QList<QString>({path}));
        bool setmem = rdp->SetInMemory();

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,path,false);
            if(rdp->GetBandCount() > 2)
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
            }else if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER2),true);
            }else
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
            }
            return ret;

        }else {
            return nullptr;
        }


    }


}

UILayer * WorldWindow::GetUIStreamLayerFromFile(QString path)
{
    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());

    RasterDataProvider * rdp = new RasterDataProvider(QList<QString>({path}));

    if(rdp->Exists())
    {
        UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,path,false);
        if(rdp->GetBandCount() > 1)
        {
            ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
        }else
        {
            ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
        }
        return ret;

    }else {
        return nullptr;
    }

}

UILayer * WorldWindow::GetUIDuoLayerFromFile(QList<QString> paths)
{
    if(paths.length() > 0)
    {
        QFileInfo fileInfo(paths.at(0));
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        RasterDataProvider * rdp = new RasterDataProvider(paths,true);

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,paths.at(0),false);
            ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_DUORASTER),false);

            return ret;

        }else {
            return nullptr;
        }
    }

    return nullptr;
}


UILayer * WorldWindow::GetUITimeSeriesLayerFromFile(QList<QString> paths)
{
    if(paths.length() > 0)
    {
        QFileInfo fileInfo(paths.at(0));
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        RasterDataProvider * rdp = new RasterDataProvider(paths);

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,filename,true,paths.at(0),false);
            if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),false);
            }else
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),false);
            }
            return ret;

        }else {
            return nullptr;
        }
    }

    return nullptr;
}


UILayer * WorldWindow::GetUILayerFromMap(cTMap * map, QString name, bool native)
{
    if(map == nullptr)
    {
        return nullptr;
    }
    UIRasterLayer *HS = new UIRasterLayer(map,name,false,"",native);
    HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
    return HS;

}

UILayer * WorldWindow::GetUIVectorLayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());
    ShapeFile * _M;

    try
    {
        _M = LoadVector(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UIVectorLayer *HS = new UIVectorLayer(_M,filename,true,path);
    HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_VECTORUI),true);

    return HS;

}


UILayer * WorldWindow::GetUIObjectLayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());

    ModelGeometry * _M;
    try
    {
        _M = Read3DModel(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    _M->GetAndCalcBoundingBox();
    UI3DObjectLayer *HS = new UI3DObjectLayer(_M,filename,true,path);
    HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_POINTCLOUDUI),true);

    return HS;

}

UILayer * WorldWindow::GetUIPointCloudLayerFromFile(QString path)
{

    QFileInfo fileInfo(path);
    QString filename(fileInfo.fileName());
    QString filedir(fileInfo.dir().path());

    PointCloud * _M;
    try
    {
        _M = ReadPointCloud(path);
    }
    catch (int e)
    {
       return nullptr;
    }

    UIPointCloudLayer *HS = new UIPointCloudLayer(_M,filename,true,path);
    HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_POINTCLOUDUI),true);

    return HS;

}

UILayer * WorldWindow::GetUILayerFromShapeFile(ShapeFile * map, QString name, bool native)
{
    if(map == nullptr)
    {
        return nullptr;
    }
    UIVectorLayer *HS = new UIVectorLayer(map,name,false,"",native);
    HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_VECTORUI),true);
    return HS;

}

UILayer * WorldWindow::GetGoogleLayer()
{
    UIWebTileLayer * HS = new UIWebTileLayer(GetDefaultTileServer(TILESERVER_GOOGLEEARTH));
    return HS;
}
UILayer * WorldWindow::GetGoogleDEMLayer()
{
    UIWebTileLayer * HS = new UIWebTileLayer(GetDefaultTileServer(TILESERVER_GOOGLEELEVATION));
    return HS;
}
void WorldWindow::RemoveUILayer(UILayer *ML, bool emitsignal)
{
    ML->SetRemove();
}

void WorldWindow::ReplaceUILayer(UILayer *ML, UILayer *ML2)
{
    ML->SetReplace(ML2);
}
void WorldWindow::SetUILayerAs(UILayer *ML,  bool emitsignal)
{
    m_UILayerMutex.lock();
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->GetID() == ML->GetID())
        {
            m_UILayerList.replace(i,ML);
            break;
        }
    }
    m_UILayerMutex.unlock();

    if(emitsignal)
    {
        emit OnMapsChanged();
    }

}
QList<UILayer *> WorldWindow::GetUILayers()
{
    QList<UILayer *> ls;
    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            ls.append(m_UILayerList.at(i));
        }
    }
    return ls;
}

void WorldWindow::SetLayerOrder(QList<UILayer *> newlist, bool emitsignal, bool is_locked)
{
    if(!is_locked)
    {

        m_UILayerMutex.lock();
    }

    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsUser())
        {
            m_UILayerList.removeAt(i);
        }
    }

    for(int i = 0; i < newlist.length(); i++)
    {
        m_UILayerList.append(newlist.at(i));
    }

    if(!is_locked)
    {
        m_UILayerMutex.unlock();
    }

    if(emitsignal)
    {
        emit OnMapsChanged();
    }

}

int WorldWindow::GetUILayerCount()
{
    return m_UILayerList.length();
}

UILayer* WorldWindow::GetUILayer(int i)
{
    return m_UILayerList.at(i);
}

BoundingBox WorldWindow::AddNativeUILayers(int channel, LISEMModel * m)
{
    BoundingBox b;
    if(m == nullptr)
    {
        m = m_Model;
    }

    if(!m->m_MemorySave)
    {
        UIStreamRasterLayer *HS = GetUIModelRasterLayer(m->HILLSHADE,m->TUI_HILLSHADE, "Hillshade","HS (model)",true);
        HS->SetChannel(channel);
        HS->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERHS),true);
        AddUILayer(HS,false);
        b = HS->GetBoundingBox();
        LookAt(HS);
    }

    if(!m->m_MemorySave)
    {
        if((m->m_DoSlopeFailure || m->m_DoInitialSolids))
        {
            UIStreamRasterLayer * WH = GetUIModelDuoRasterLayer(m->MUIH,m->TUI_H,m->MUIHS,m->TUI_HS,"Flow","Flow height and content (Model)",true);
            WH->SetChannel(channel);
            LSMStyle s = GetStyleDefault(LISEM_STYLE_DEFAULT_DUORASTER);
            s.m_IsFractional = true;
            WH->SetStyle(s,true);
            AddUILayer(WH,false);

        }else {

            UIStreamRasterLayer *WH = GetUIModelRasterLayer(m->MUIH,m->TUI_H, "Water Height (m)","WH (Model)",true);
            WH->SetChannel(channel);
            WH->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERWH),true);
            AddUILayer(WH,false);
        }
    }else
    {
        UIStreamRasterLayer *WH = GetUIModelRasterLayer(m->MUIH,m->TUI_H, "Water Height (m)","WH (Model)",true);
        WH->SetChannel(channel);
        WH->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERWH),true);
        AddUILayer(WH,false);
        b = WH->GetBoundingBox();
        LookAt(WH);
    }


    if(!m->m_MemorySave)
    {
        UIStreamRasterLayer * WV = GetUIModelDuoRasterLayer(m->MUIUX,m->TUI_UX,m->MUIUY,m->TUI_UY,"Velocity","Flow velocity (Model)",true);
        WV->SetChannel(channel);
        WV->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_DUORASTER_VEL),true);
        AddUILayer(WV,false);

    }

    if(m->m_DoSlopeFailure || m->m_DoInitialSolids)
    {

    }

    if(!m->m_MemorySave)
    {
        UIStreamRasterLayer *UI = GetUIModelRasterLayer(m->MUI,m->TUI_UI, "User Map","UI (Model)",true);
        UI->SetChannel(channel);
        UI->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
        UI->SetDraw(false);



        AddUILayer(UI,false);
    }


    if(m->m_DoParticleSolids && (m->m_DoInitialSolids || (m->m_DoSlopeFailure && m->m_DoHydrology && m->m_DoSlopeStability)))
    {
        //if(!m->m_DoCPU)
        {

           UIModelPointLayer *UIP = new UIModelPointLayer(m->m_DataPos,m->DEM->west(),m->DEM->north());
           UIP->SetChannel(channel);
           UIP->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);

           AddUILayer(UIP,false);
        }
    }

    //add rigid physics world layer
    if(m->m_DoRigidWorld)
    {
        UIRigidWorldLayer *UIP = new UIRigidWorldLayer(m->m_RigidWorld,"Rigid Object","Rigid (Model)",true);
        UIP->SetChannel(channel);
        AddUILayer(UIP,false);
    }



    emit OnMapsChanged();

    return b;


}

void WorldWindow::RemoveNativeUILayers(int channel)
{
    m_UILayerMutex.lock();

    for(int i = m_UILayerList.length()-1; i > -1; i--)
    {
        if(m_UILayerList.at(i)->IsModel() == true)
        {
            if(m_UILayerList.at(i)->GetChannel()==channel)
            {

                m_UILayerList.at(i)->SetRemove();
            }
        }
    }

    m_UILayersChanged = true;
    m_UILayerMutex.unlock();

    {
        emit OnMapsChanged();
    }
}



//control of the display geolocation

BoundingBox WorldWindow::GetLook()
{
    return BoundingBox(m_CurrentWindowState.tlx,m_CurrentWindowState.brx,m_CurrentWindowState.tly,m_CurrentWindowState.bry);
}

void WorldWindow::LookAt(UIGeoLayer* geolayer)
{
    BoundingBox bb_orgref = geolayer->GetBoundingBox();


    GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(geolayer->GetProjection(),m_CurrentWindowState.projection);

    BoundingBox bb_newref = transformer->Transform(bb_orgref);

    delete transformer;

    LookAt(bb_newref);
}

void WorldWindow::LookAtbb(BoundingBox b)
{
    LookAt(b);
}

void WorldWindow::LookAt(BoundingBox3D b3d)
{
    BoundingBox b;
    b.Set(b3d.GetMinX(),b.GetMaxX(), b.GetMinY(), b.GetMaxY());
    m_Camera2D->LookAt(b);

    m_Camera3D->PlaceAndLookAtAuto(b3d);


    //get screen dim (min 1)
    float scw = std::max(1.0f,((float)(m_OpenGLCLManager->GL_GLOBAL.Width)));
    float sch = std::max(1.0f,(float)( m_OpenGLCLManager->GL_GLOBAL.Height));

    m_CurrentWindowState.translation_x = b.GetCenterX();
    m_CurrentWindowState.translation_y = b.GetCenterY();

    bool do_x = b.GetSizeX() > b.GetSizeY()? true:false;

    if(do_x)
    {
        m_CurrentWindowState.scale = b.GetSizeX();

        m_CurrentWindowState.width = m_CurrentWindowState.scale;
        float ratiohw = (sch/scw);
        m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    }else
    {
        float ratiohw = (sch/scw);

        m_CurrentWindowState.scale = b.GetSizeX() * 1.0/ratiohw;

        m_CurrentWindowState.width = m_CurrentWindowState.scale;
        m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    }

    m_CurrentWindowState.tlx = m_CurrentWindowState.translation_x - 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.brx = m_CurrentWindowState.translation_x + 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.tly = m_CurrentWindowState.translation_y - 0.5f *m_CurrentWindowState.height;
    m_CurrentWindowState.bry = m_CurrentWindowState.translation_y + 0.5f *m_CurrentWindowState.height;

}

void WorldWindow::LookAt(BoundingBox b, bool auto_3d )
{
    m_Camera2D->LookAt(b);
    if(auto_3d)
    {
        m_Camera3D->PlaceAndLookAtAuto(m_Camera2D->GetBoundingBox());
    }

    std::cout << b.GetMinX() << " "<< b.GetMaxX() << " " << b.GetMinY() << " " << b.GetMaxY() << std::endl;
    //get screen dim (min 1)
    float scw = std::max(1.0f,((float)(m_OpenGLCLManager->GL_GLOBAL.Width)));
    float sch = std::max(1.0f,(float)( m_OpenGLCLManager->GL_GLOBAL.Height));

    m_CurrentWindowState.translation_x = b.GetCenterX();
    m_CurrentWindowState.translation_y = b.GetCenterY();

    bool do_x = b.GetSizeX() > b.GetSizeY()? true:false;

    if(do_x)
    {
        m_CurrentWindowState.scale = b.GetSizeX();

        m_CurrentWindowState.width = m_CurrentWindowState.scale;
        float ratiohw = (sch/scw);
        m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    }else
    {
        float ratiohw = (sch/scw);

        m_CurrentWindowState.scale = b.GetSizeX() * 1.0/ratiohw;

        m_CurrentWindowState.width = m_CurrentWindowState.scale;
        m_CurrentWindowState.height = m_CurrentWindowState.scale * ratiohw;

    }

    m_CurrentWindowState.tlx = m_CurrentWindowState.translation_x - 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.brx = m_CurrentWindowState.translation_x + 0.5f *m_CurrentWindowState.width;
    m_CurrentWindowState.tly = m_CurrentWindowState.translation_y - 0.5f *m_CurrentWindowState.height;
    m_CurrentWindowState.bry = m_CurrentWindowState.translation_y + 0.5f *m_CurrentWindowState.height;

}

