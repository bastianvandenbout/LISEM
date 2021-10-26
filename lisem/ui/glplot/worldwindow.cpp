#include "worldwindow.h"
#include "gdal_version.h"
#include "QThread"


bool WorldWindow::Draw()
{

    //get global width and depth
    int widtht = 0;
    int heightt = 0;
    glfwGetFramebufferSize(m_OpenGLCLManager->window,&widtht, &heightt);

    if(widtht < 3 || heightt < 3)
    {
        return false;
    }


    bool require_redraw = false;


    qint64 timenew =  m_time.nsecsElapsed();
    double timedif = double(timenew - m_timeold)/1000000000.0;
    m_GLDT = std::max(0.00001,timedif);
    if(dt_first)
    {
        m_GLDT = 0.00001;
        dt_first = false;
    }else {
        m_GLT += m_GLDT;
    }
    if(m_GLDT < 0.03333333333)
    {
        double sleepsec = 0.03333333333- m_GLDT;
        QThread::usleep(((int)(sleepsec * 1000000.0)));
    }
    if(draw_first)
    {
        draw_first = false;

        //do initial calculations for drawing code

        //generate framebuffers that are used for drawing 2d content and rendering this onto 3d geometry


        int size_color = 1500;
        int size_depth = 1000;

        OpenGLCLMSAARenderTarget * targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(size_color,size_color);
        OpenGLCLMSAARenderTarget * targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(size_depth,size_depth);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(6.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(size_depth,size_depth);

        targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(size_color,size_color);
        targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(size_depth,size_depth);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(40.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(size_depth,size_depth);

        targetc = new OpenGLCLMSAARenderTarget();
        targetc->CreateRGBA8(size_color/2,size_color/2);
        targetd = new OpenGLCLMSAARenderTarget();
        targetd->CreateR32(size_depth/2,size_depth/2);
        m_2D3DRenderCTargets.append(targetc);
        m_2D3DRenderDTargets.append(targetd);
        m_2D3DRenderTargetScales.append(250.0);
        m_2D3DTransformManager.append( new WorldGLTransformManager(m_OpenGLCLManager));
        m_ElevationProvider->AddElevationModel(size_depth/2,size_depth/2);

        m_3DLayerRenderTargetC = new OpenGLCLMSAARenderTarget();
        m_3DLayerRenderTargetC->CreateRGBA8(size_color,size_color);
        m_3DLayerRenderTargetD = new OpenGLCLMSAARenderTarget();
        m_3DLayerRenderTargetD->CreateR32(1000,1000);

        m_3DRenderTarget = new OpenGLCLMSAARenderTarget();
        m_TDRenderTarget = new OpenGLCLMSAARenderTarget();
        m_Post1RenderTarget = new OpenGLCLMSAARenderTarget();
        m_Post2RenderTarget = new OpenGLCLMSAARenderTarget();

        m_3DRenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,3,4);
        m_Post1RenderTarget->CreateRGBA8(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
        m_Post2RenderTarget->CreateRGBA8(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
        m_TDRenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,3,4);

        MaskedRaster<float> raster_data(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosX = new cTMap(std::move(raster_data),"","");
        m_3DScreenPosX->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data2(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosY = new cTMap(std::move(raster_data2),"","");
        m_3DScreenPosY->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data3(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosZ = new cTMap(std::move(raster_data3),"","");
        m_3DScreenPosZ->AS_IsSingleValue = false;



        m_ArrowModel = new ModelGeometry();
        LSMMesh base;
        base.SetAsCone(0.1,0.1,0.4,10);
        base.Move(LSMVector3(0.0,0.4,0.0));
        LSMMesh top;
        top.SetAsCone(0.2,0.0,0.1,10);
        top.Move(LSMVector3(0.0,0.9,0.0));

        m_ArrowModel->AddMesh(base);
        m_ArrowModel->AddMesh(top);

        m_ArrowActor = new gl3dObject(m_OpenGLCLManager,m_ArrowModel);

        require_redraw = true;
    }


    //check if they are the same as currently said by the glfw database

    if(widtht != m_OpenGLCLManager->GL_GLOBAL.Width)
    {
        require_redraw = true;
        m_OpenGLCLManager->GL_GLOBAL.Width = widtht;
    }
    if(heightt != m_OpenGLCLManager->GL_GLOBAL.Height)
    {
        require_redraw = true;
        m_OpenGLCLManager->GL_GLOBAL.Height = heightt;
    }




    m_3DRenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
    m_TDRenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
    m_Post1RenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);
    m_Post2RenderTarget->Resize(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

    if(m_OpenGLCLManager->GL_GLOBAL.Height != m_3DScreenPosX->nrRows() || m_OpenGLCLManager->GL_GLOBAL.Width != m_3DScreenPosX->nrCols())
    {
        require_redraw = true;
        delete m_3DScreenPosX;
        delete m_3DScreenPosY;
        delete m_3DScreenPosZ;
        m_3DScreenPosX= nullptr;
        m_3DScreenPosY= nullptr;
        m_3DScreenPosZ= nullptr;

        MaskedRaster<float> raster_data(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosX = new cTMap(std::move(raster_data),"","");
        m_3DScreenPosX->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data2(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosY = new cTMap(std::move(raster_data2),"","");
        m_3DScreenPosY->AS_IsSingleValue = false;

        MaskedRaster<float> raster_data3(m_OpenGLCLManager->GL_GLOBAL.Height,m_OpenGLCLManager->GL_GLOBAL.Width,0.0,0.0,1.0);
        m_3DScreenPosZ = new cTMap(std::move(raster_data3),"","");
        m_3DScreenPosZ->AS_IsSingleValue = false;
    }else {

        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(1),m_3DScreenPosX);
        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(2),m_3DScreenPosY);
        //m_OpenGLCLManager->CopyTextureToMap(m_3DRenderTarget->GetTexture(3),m_3DScreenPosZ);
    }


    m_timeold = timenew;

    //layer management


    m_Model->UILayerRemoveMutex.lock();


    if(m_Model->m_UIMapsSet)
    {

        if(m_Model->m_RequiresUIReset)
        {
            m_Model->m_RequiresUIReset = false;

            RemoveNativeUILayers();
            AddNativeUILayers();

            require_redraw = true;

        }
        if(m_Model->m_DidUpdate)
        {
            require_redraw = true;
            m_Model->m_DidUpdate = false;
        }
    }else if(m_Model->m_RequiresUIReset)
    {
        m_Model->m_RequiresUIReset = false;

        RemoveNativeUILayers();
        require_redraw = true;
    }


    m_UILayerMutex.lock();


    QList<UILayer *> layer_replace_remove;
    QList<int> layer_replace_id;
    QList<int> layer_replace_uid;
    QList<UILayer *> layer_replace_add;
    QList<int> layer_replace_index;

    //if required, add the replacement layers
    //set the replaced layers to be removed

    for(int i = m_UILayerList.length()-1; i >-1 ; i--)
    {
        UILayer * l = m_UILayerList.at(i);

        if(l->ShouldBeReplaced() )
        {
            UILayer * l2 = l->GetReplaceLayer();
            l2->SetReplacedLayer(l);
            l2->SetStyle(l->GetStyle());

            if(l2 != nullptr)
            {
                layer_replace_remove.append(l);
                layer_replace_add.append(l2);
                layer_replace_index.append(i);

            }
            require_redraw = true;
        }

    }

    m_UILayerMutex.unlock();


    if(layer_replace_remove.length() > 0)
    {
        m_UILayerMutex.lock();

        //actually add and remove layers
        for(int i = 0; i <  layer_replace_remove.length(); i++)
        {
            UILayer * l = layer_replace_remove.at(i);

            layer_replace_id.append(l->GetID());
            layer_replace_uid.append(l->GetUID());

            l->OnDestroy(m_OpenGLCLManager);
            m_UILayerList.removeOne(l);


        }
        require_redraw = true;


        m_UILayerMutex.unlock();
    }

    int k = 0;
    for(int i = 0; i  < layer_replace_add.length() ; i++)
    {
        UILayer * l = layer_replace_add.at(i);
        //this->AddUILayer(l,true,false);
        this->AddUILayerAt(l,true,false,layer_replace_index.at(i));
        k++;
        l->SetID(layer_replace_id.at(i));
        l->SetUID(layer_replace_uid.at(i));
        require_redraw = true;
    }

    m_UILayerMutex.lock();




    bool removed = false;
    for(int i = m_UILayerList.length()-1; i >-1 ; i--)
    {
        UILayer * l = m_UILayerList.at(i);

        if(l->ShouldBeRemoved() )
        {
            removed = true;
            l->OnDestroy(m_OpenGLCLManager);
            m_UILayerList.removeAt(i);
            require_redraw = true;
        }

    }

    if(m_LayerEditor != nullptr)
    {
        UILayerEditor * l = m_LayerEditor;

        if(l->ShouldBeRemoved() )
        {
            removed = true;
            l->OnClose();
            l->OnDestroy(m_OpenGLCLManager);
            delete m_LayerEditor;
            m_LayerEditor = nullptr;
            require_redraw = true;

        }
    }


    if(removed)
    {
        require_redraw = true;
        emit OnMapsChanged();
    }



    std::vector<UIDistanceEstimator*> dlist;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);
        if(l->IsGeo())
        {
            UIDistanceEstimator* ld= dynamic_cast<UIDistanceEstimator*>(l);
            if(ld != nullptr)
            {
                dlist.push_back(ld);
            }else
            {
                std::cout << "conversion failed " << std::endl;
            }
        }
    }

    m_ElevationProvider->SetDistanceObjects(dlist);

    //Update Camera properties
    bool cam_dyn = UpdateCamera();
    require_redraw = require_redraw || cam_dyn;


    //update GeoWindowState
    bool need_redraw = UpdateCurrentWindowState();

    require_redraw = require_redraw || need_redraw;

    m_CRSMutex.lock();

    //check if projection of either the layer or the window has changed
    //if so, call the relevant function for the layer class (OnCRSChanged())
    m_CurrentWindowState.projection = m_CurrentProjection;

    bool crs_doneeditor = false;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);
        if(m_CRSChanged || l->IsCRSChanged())
        {
            require_redraw = true;

            l->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
            if(m_LayerEditor != nullptr)
            {
                if(m_LayerEditor->GetLayer() == l)
                {
                    m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
                    crs_doneeditor = true;
                }
            }
        }
    }
    if(m_LayerEditor != nullptr && !crs_doneeditor && m_CRSChanged)
    {
        require_redraw = true;
        m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
    }
    m_CRSChanged = false;

    //now update the opengl crs transforming textures based on the current layers and CRS
    UpdateTransformers();

    m_CRSMutex.unlock();


    //actually draw the crs transformers
    m_TransformManager->UpdateFromWindowState(this->m_CurrentWindowState);

    m_CurrentDrawWindowState = m_CurrentWindowState;

    //m_CurrentDrawWindowState.m_FocusLocations =  m_CurrentWindowState.m_FocusLocations;
    //m_CurrentDrawWindowState.m_FocusSquare = m_CurrentWindowState.m_FocusSquare;

    m_CurrentDrawWindowState.GL_3DFrameBuffer = m_3DRenderTarget;
    m_CurrentDrawWindowState.GL_FrameBuffer = m_Draw3D? m_OpenGLCLManager->GetFrameBuffer():m_OpenGLCLManager->GetFrameBuffer();
    m_CurrentDrawWindowState.GL_PrimaryFrameBuffer = m_Draw3D? m_OpenGLCLManager->GetFrameBuffer() :m_OpenGLCLManager->GetFrameBuffer();
    m_CurrentDrawWindowState.GL_PostProcessBuffer1 = m_Post1RenderTarget;
    m_CurrentDrawWindowState.GL_PostProcessBuffer2 = m_Post2RenderTarget;


   MouseStateMutex.lock();

    //send all input signals to the child objects (layers, editors etc..)



    InputToLayers();

    if(m_MouseState.Move_X != 0.0 || m_MouseState.Move_Y != 0.0 || m_MouseState.MoveScroll_X != 0.0 || m_MouseState.MoveScroll_Y != 0.0 || m_MouseState.MouseButtonEvents.length() > 0 || m_MouseState.KeyEvents.length() > 0)
    {
        require_redraw = true;

    }

    //reset some values in the mouse state

    m_MouseState.PosOld_x = m_MouseState.Pos_x;
    m_MouseState.PosOld_y = m_MouseState.Pos_y;
    m_MouseState.Drag_x = 0.0f;
    m_MouseState.Drag_y = 0.0f;
    m_MouseState.scrollold = m_MouseState.scroll;
    m_MouseState.Move_X = 0.0f;
    m_MouseState.Move_Y = 0.0f;
    m_MouseState.MoveScroll_X = 0.0f;
    m_MouseState.MoveScroll_Y = 0.0f;
    m_MouseState.MouseButtonEventsShift.clear();
    m_MouseState.MouseButtonEvents.clear();
    m_MouseState.MouseButtonKeyAction.clear();
    m_MouseState.KeyEventShift.clear();
    m_MouseState.KeyEvents.clear();
    m_MouseState.KeyAction.clear();
    m_MouseState.KeyMods.clear();
    MouseStateMutex.unlock();

    //std::cout << "drawdo g " << require_redraw << std::endl;

    //OnDraw callback

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
               //std::cout << "prepare layer " << l->GetName().toStdString()<< std::endl;
                require_redraw = true;
                l->OnPrepare(m_OpenGLCLManager,m_CurrentWindowState);
                l->OnCRSChanged(m_OpenGLCLManager,m_CurrentWindowState,m_TransformManager);
            }
        }

        if(m_UILayerList.at(i)->IsUser() )
        {
            UILayer * l = m_UILayerList.at(i);
            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                l->OnDraw(m_OpenGLCLManager,m_CurrentWindowState);

                //check if this layer has something new to draw
                require_redraw = require_redraw || l->RequiresDraw(m_CurrentWindowState);
            }
        }
    }

    if(m_LayerEditor != nullptr)
    {
        m_LayerEditor->OnDraw(m_OpenGLCLManager,m_CurrentWindowState);
    }
    m_RedrawNeedMutex.lock();


    require_redraw = require_redraw || m_RedrawNeed;

    m_RedrawNeed = false;

    if(require_redraw )
    {

        std::cout<< "draw current dims " << widtht << " " << heightt << " " <<  m_OpenGLCLManager->GL_GLOBAL.Height << " " << m_OpenGLCLManager->GL_GLOBAL.Width << " " << m_3DRenderTarget->GetWidth() << " " << m_3DRenderTarget->GetHeight() <<  std::endl;


        //std::cout << "redraw " << m_GLDT << " " << " " <<  m_RedrawNeed <<  std::endl;

        //now depending on what kind of window we are drawing (3d, globe or 2d) call the relevant function
        if(!m_Draw3D)
        {

            DrawToFrameBuffer2D(m_CurrentDrawWindowState);

        }else {
            DrawToFrameBuffer3D(m_CurrentDrawWindowState,false);
        }


    }else
    {
        //std::cout << "no redraw " << m_GLDT << std::endl;
    }

    m_RedrawNeedMutex.unlock();

    m_Model->UILayerRemoveMutex.unlock();


    m_UILayerMutex.unlock();


    //only return true if it needed an update
    return require_redraw;

}

void WorldWindow::SetRedrawNeeded()
{
    m_RedrawNeedMutex.lock();
    m_RedrawNeed = true;
    m_RedrawNeedMutex.unlock();
}

void WorldWindow::SetDraw3D(bool d)
{
    m_UILayerMutex.lock();
    m_Draw3D = d;
    m_UILayerMutex.unlock();
}

void WorldWindow::SetDraw3DGlobe(bool d)
{
    m_UILayerMutex.lock();
    m_Draw3DGlobe = d;
    m_UILayerMutex.unlock();
}

void WorldWindow::SetUIDraw(bool d)
{
     m_UILayerMutex.lock();
     m_DrawUI = d;
     m_UILayerMutex.unlock();
}
void WorldWindow::SetLinesDraw(bool d)
{
     m_UILayerMutex.lock();
     m_DrawLines = d;
     m_UILayerMutex.unlock();
}

void WorldWindow::SetUIScale(double d)
{
     m_UILayerMutex.lock();
     m_DrawUIScale = d;
     m_UILayerMutex.unlock();

}
void WorldWindow::SetShadowDraw(bool d)
{
    m_UILayerMutex.lock();
    m_DrawShadows = d;
    m_UILayerMutex.unlock();
}

void WorldWindow::SetLegendDraw(bool d)
{
    m_UILayerMutex.lock();
    m_DrawLegends = d;
    m_UILayerMutex.unlock();
}
GeoProjection WorldWindow::GetCurrentProjection()
{
    return m_CurrentWindowState.projection;
}
void WorldWindow::SetCurrentProjection(GeoProjection p, bool forceupdate)
{
    if(!m_CurrentWindowState.projection.IsEqualTo(p) || forceupdate)
    {
        m_CRSMutex.lock();

        m_CRSChanged =true;
        m_CurrentProjection = p;
        m_ElevationProvider->SetZMult(p.GetUnitZMultiplier());

        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_FocusLocations.clear();
        m_FocusMutex.unlock();

        m_CRSMutex.unlock();
    }
}

GeoWindowState WorldWindow::GetCurrentWindowState()
{
    return m_CurrentWindowState;
}

void WorldWindow::SignalClose()
{
        glfwSetWindowShouldClose(m_OpenGLCLManager->window,GL_TRUE);
}

void WorldWindow::SetModel( LISEMModel * m)
{
    m_Model = m;

}
