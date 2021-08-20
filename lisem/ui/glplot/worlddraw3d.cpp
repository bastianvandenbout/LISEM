#include "worldwindow.h"


void WorldWindow::DrawToFrameBuffer3D(GeoWindowState s, bool external = false)
{





    DrawColoredTerrain3D(&s); //this one fills the 2d to 3d render targets

    //fill elevation model

    for(int i = 0; i < m_2D3DRenderDTargets.length(); i++)
    {
        m_ElevationProvider->SetElevationModel(i,m_OpenGLCLManager,m_2D3DRenderDTargets.at(i),m_2D3DRenderTargetBoundingBox.at(i));
    }

    s.GL_3DFrameBuffer->ClearAll();

    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glClearColor(0.5,0.5,1.0,1.0);


    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_3DFrameBuffer->GetFrameBuffer());
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);
    if(external)
    {
        glad_glClearColor(1.0,0.5,0.5,1.0);
    }else
    {
        glad_glClearColor(0.5,0.5,0.5,1.0);
    }
    glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);


    s.GL_3DFrameBuffer->SetAsTarget();
    s.GL_3DFrameBuffer->ClearAll();

    DrawBaseLayers3D(s);

    s.GL_3DFrameBuffer->BlitToTexture();

    if(
            (m_OpenGLCLManager->GL_GLOBAL.Height != m_3DScreenPosX->nrRows() || m_OpenGLCLManager->GL_GLOBAL.Width != m_3DScreenPosX->nrCols())
            || (s.GL_3DFrameBuffer->GetHeight() != m_3DScreenPosX->nrRows() || s.GL_3DFrameBuffer->GetWidth() != m_3DScreenPosX->nrCols())

            )
    {
        s.ScreenPosX = nullptr;
        s.ScreenPosY = nullptr;
        s.ScreenPosZ = nullptr;

        s.ScreenPosXTex = -1;
        s.ScreenPosYTex = -1;
        s.ScreenPosZTex = -1;

        s.ScreenNormalXTex = -1;
        s.ScreenNormalYTex = -1;
        s.ScreenNormalZTex = -1;

    }else if(s.GL_3DFrameBuffer != m_3DRenderTarget)
    {
        s.ScreenPosX = nullptr;
        s.ScreenPosY = nullptr;
        s.ScreenPosZ = nullptr;

        s.ScreenPosXTex = -1;
        s.ScreenPosYTex = -1;
        s.ScreenPosZTex = -1;

        s.ScreenNormalXTex = -1;
        s.ScreenNormalYTex = -1;
        s.ScreenNormalZTex = -1;

    }else
    {
        m_OpenGLCLManager->CopyTextureToMap(s.GL_3DFrameBuffer->GetTexture(1),&(m_3DScreenPosZ->data));
        m_OpenGLCLManager->CopyTextureToMap(s.GL_3DFrameBuffer->GetTexture(2),&(m_3DScreenPosY->data));
        m_OpenGLCLManager->CopyTextureToMap(s.GL_3DFrameBuffer->GetTexture(3),&(m_3DScreenPosX->data));

        s.ScreenPosX = m_3DScreenPosX;
        s.ScreenPosY = m_3DScreenPosY;
        s.ScreenPosZ = m_3DScreenPosZ;

        s.ScreenPosXTex =s.GL_3DFrameBuffer->GetTexture(1);
        s.ScreenPosYTex =s.GL_3DFrameBuffer->GetTexture(2);
        s.ScreenPosZTex =s.GL_3DFrameBuffer->GetTexture(3);

        s.ScreenNormalXTex =s.GL_3DFrameBuffer->GetTexture(4);
        s.ScreenNormalYTex =s.GL_3DFrameBuffer->GetTexture(5);
        s.ScreenNormalZTex =s.GL_3DFrameBuffer->GetTexture(6);
    }


    if(s.ScreenPosX == nullptr || s.ScreenPosY == nullptr || s.ScreenPosZ == nullptr)
    {
        return;
    }

    //Objects must be aware that the sceenPos maps are not guaranteed


    s.GL_3DFrameBuffer->SetAsTarget();

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(s.GL_3DFrameBuffer->GetFrameBuffer(),s.scr_pixwidth,s.scr_pixheight);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(s.GL_3DFrameBuffer->GetFrameBuffer(),s.scr_pixwidth,s.scr_pixheight);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(s.GL_3DFrameBuffer->GetFrameBuffer(),s.scr_pixwidth,s.scr_pixheight);



    Draw3DObjects(s);


    Draw3DPost(s);

    s.GL_3DFrameBuffer->BlitToTexture();

    s.GL_FrameBuffer->SetDefaultTarget();
    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());


    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glad_glDrawBuffers(1, DrawBuffers);

    glad_glViewport(0,0,s.GL_FrameBuffer->GetWidth(),s.GL_FrameBuffer->GetHeight());//m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

    glad_glDisable(GL_DEPTH_TEST);
    // bind shader
    glad_glUseProgram(m_OpenGLCLManager->m_GLProgram_CopyText->m_program);
    // get uniform locations
    int mat_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"matrix");
    int tex_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"tex");
    // bind texture

    glad_glUniform1i(tex_loc,0);
    glad_glActiveTexture(GL_TEXTURE0);
    glad_glBindTexture(GL_TEXTURE_2D, (s.GL_3DFrameBuffer->GetTexture(0)));//m_2D3DRenderDTargets.at(0)->GetTexture(0));//
    // set project matrix
    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
    // now render stuff
    glad_glBindVertexArray(m_OpenGLCLManager->m_Quad->m_vao);
    glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
    glad_glBindVertexArray(0);


    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(s.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);

    GeoWindowState s2 = s;
    //s2.GL_FrameBuffer = m_OpenGLCLManager->GetFrameBuffer();


    DrawPostProcess(s2,m_TransformManager);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_PrimaryFrameBuffer->GetFrameBuffer());
    glad_glDrawBuffers(1, DrawBuffers);


    Draw3DUI(s2);

}

void WorldWindow::DrawBaseLayers3D(GeoWindowState s)
{
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPreDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPreDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnDraw3D(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }

}


void WorldWindow::DrawColoredTerrain3D(GeoWindowState *s)
{

    s->GL_FrameBuffer3DWindow.clear();
    s->GL_FrameBuffer3DColor.clear();
    s->GL_FrameBuffer3DElevation.clear();

    m_2D3DRenderTargetBoundingBox.clear();

    float scalebase = 1.0;


    bool found = false;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);


        if(l->IsDraw() && l->IsDrawAsDEM() && (!s->ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s->ChannelFilter && s->Channel == m_UILayerList.at(i)->GetChannel())))
        {
            if(l->IsGeo())
            {
                UIGeoLayer * gl = (UIGeoLayer *) l;
                float val = 0.0;
                if(gl->GetDetailedElevationAtPosition(LSMVector2(m_Camera3D->GetPosition().x,m_Camera3D->GetPosition().z),m_CurrentProjection,val))
                {
                    m_ElevationProvider->SetDetailedElevationAtCenter(true,val);
                    found = true;
                    break;
                }
            }

        }
    }
    if(!found)
    {
        m_ElevationProvider->SetDetailedElevationAtCenter(false,0.0);
    }

    BoundingBox lookbb = m_Camera3D->Get2DViewEquivalent(m_ElevationProvider);

    bool has_elevation = m_ElevationProvider->HasElevation(m_Camera3D->GetPosition().x,m_Camera3D->GetPosition().z);

    double o_size = m_ElevationProvider->GetClosestDistanceToObjectSize((m_Camera3D->GetPosition()));
    double o_distance = m_ElevationProvider->GetClosestDistanceToObject((m_Camera3D->GetPosition()));

    //not above a dem, but something exists
    if(has_elevation == false && (o_size < 1e30) && (o_distance < 1e30))
    {
        double size_now = std::max(lookbb.GetSizeX(), lookbb.GetSizeY());
        if(size_now < (o_size + o_distance) && (size_now > 0.0))
        {
            lookbb = BoundingBox(m_Camera3D->GetPosition().x - 0.2 * (o_size + o_distance),m_Camera3D->GetPosition().x + 0.2 * (o_size + o_distance),m_Camera3D->GetPosition().z - 0.2 * (o_size + o_distance),m_Camera3D->GetPosition().z + 0.2 * (o_size + o_distance));

        }

    }

    for(int i = 0; i < m_2D3DRenderTargetScales.length(); i++)
    {

        if(i == 0)
        {
            scalebase = m_2D3DRenderTargetScales.at(i);
        }
        OpenGLCLMSAARenderTarget * TargetC = m_2D3DRenderCTargets.at(i);
        OpenGLCLMSAARenderTarget * TargetD = m_2D3DRenderDTargets.at(i);
        float TargetScale = m_2D3DRenderTargetScales.at(i);

        //find bounding box to look at

        float cx = lookbb.GetCenterX();
        float cy = lookbb.GetCenterY();
        float sx = lookbb.GetSizeX();
        float sy = lookbb.GetSizeY();
        LSMVector3 C_Pos = m_Camera3D->GetPosition();

        float sxn = sx * TargetScale;
        float syn = sy * TargetScale;

        //we are above a dem, make sure the largest target scale covers a significant portion of the full dataset
        if((i == m_2D3DRenderTargetScales.length() -1) && (has_elevation == true) && (o_size < 1e30) && (o_distance < 1e30))
        {
            if(std::max(syn,sxn) < (o_size))
            {
                TargetScale = o_size/sx;

                sxn = sx * TargetScale;
                syn = sy * TargetScale;
            }
        }

        sxn = std::max(sxn,syn);
        syn = sxn;

        LSMVector2 cn = LSMVector2(C_Pos.X(),C_Pos.Z());// + ((LSMVector2(C_Pos.X(),C_Pos.Z())-LSMVector2(cx,cy)) * TargetScale);

        float cxn = cn.X();
        float cyn = cn.Y();
        //create new GeoWindowStates for each 2d render target

        GeoWindowState sn = *s;


        sn.scale = sxn;
        sn.width = sxn;
        sn.height = syn;
        sn.tlx = cxn - 0.5 * sxn;
        sn.brx = cxn + 0.5 * sxn;
        sn.tly = cyn - 0.5 * syn;
        sn.bry = cyn + 0.5 * syn;
        sn.scr_pixwidth = TargetC->GetWidth();
        sn.scr_pixheight = TargetC->GetHeight();
        sn.scr_width = TargetC->GetWidth();
        sn.scr_height = TargetC->GetHeight();
        sn.draw_ui = false;
        sn.draw_legends = false;
        sn.GL_FrameBuffer = TargetC;
        sn.ui_scale2d3d = scalebase/m_2D3DRenderTargetScales.at(i);
        //update gl transformer

        m_2D3DTransformManager.at(i)->UpdateFromWindowState(sn);

        //draw to this

        DrawToFrameBuffer2D(sn,m_2D3DTransformManager.at(i));
        TargetC->BlitToTexture();

        sn.GL_FrameBuffer = TargetD;
        sn.scr_pixwidth = TargetD->GetWidth();
        sn.scr_pixheight = TargetD->GetHeight();
        sn.scr_width = TargetD->GetWidth();
        sn.scr_height = TargetD->GetHeight();
        //draw to this

        DrawToFrameBuffer2DElevation(sn,m_2D3DTransformManager.at(i));
        TargetD->BlitToTexture();

        //store it in our current GeoWindowState
        s->GL_FrameBuffer3DColor.append(TargetC);
        s->GL_FrameBuffer3DElevation.append(TargetD);
        s->GL_FrameBuffer3DWindow.append(BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));

        m_2D3DRenderTargetBoundingBox.append(BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));

        m_ElevationProvider->SetElevationModel(i,m_OpenGLCLManager,TargetD,BoundingBox(sn.tlx,sn.brx,sn.tly,sn.bry));
    }

    s->m_2D3DRenderTargetBoundingBox = m_2D3DRenderTargetBoundingBox;
}


void WorldWindow::Draw3DObjects(GeoWindowState s)
{
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                    l->OnDraw3DGeo(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                    l->OnDraw3DGeo(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }

    s.GL_3DFrameBuffer->BlitToTexture();

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser()&& (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                    l->OnDraw3DTransparancyDepthMap(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                   l->OnDraw3DTransparancyDepthMap(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser()&& (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                    l->OnDraw3DTransparentLayer(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,s);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw())
            {
                   l->OnDraw3DTransparentLayer(m_OpenGLCLManager,s, m_TransformManager);
            }
        }
    }

}


void WorldWindow::Draw3DPost(GeoWindowState S)
{

    /*S.legendtotal = 0;
    S.legendindex = 0;

    QList<float> minheights;
    QList<float> maxheights;
    QList<float> actheights;
    float minheight_total = 0.0;
    float maxheight_total = 0.0;

    //all legends
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM() && !l->IsDrawAsHillShade())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    S.legendtotal = S.legendtotal + 1;
                    float minheight = l->MinimumLegendHeight(m_OpenGLCLManager,S);
                    float maxheight = l->MaximumLegendHeight(m_OpenGLCLManager,S);

                    minheights.append(minheight);
                    maxheights.append(maxheight);
                    minheight_total += minheight;
                    maxheight_total += maxheight;
                }
            }
        }
    }
    //Find all the appropriate legend heights and pass them to the layers when drawing

    float threshold = (S.scr_height - 2.0 *S.ui_framewidth)/S.scr_height;
    //if the total of max-height is < 1.0 (full screen height) we continue
    if(maxheight_total > threshold)
    {
        //if the total of min-height is > 1.0, we have to skip the last few layers
        if(minheight_total < threshold)
        {
            //if the total of max-height is > 1.0
            //re-scale all layers as : minheight + (maxheight-minheight) * (threshold-minheight_total) /(maxheight_total-minheight_total)
            //all ui layers

            float fac = std::max(0.00001f,(threshold-minheight_total))/std::max(0.00001f,(maxheight_total-minheight_total));

            for(int i = 0; i < minheights.length(); i++)
            {
                actheights.append(minheights.at(i) + (maxheights.at(i) - minheights.at(i))*fac);
            }

        }else
        {
            for(int i = 0; i < minheights.length(); i++)
            {
                actheights.append(minheights.at(i));
            }
        }
    }else
    {
        for(int i = 0; i < minheights.length(); i++)
        {
            actheights.append(minheights.at(i));
        }
    }*/


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser()&& (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3D(m_OpenGLCLManager,S, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3D(m_OpenGLCLManager,S, m_TransformManager);
            }
        }
    }


    /*

    //all legends
    float y_pos = 0.0;
    int ysizeindex = 0;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        //if(m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM()  && !l->IsDrawAsHillShade())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,S,y_pos,y_pos + S.scr_height * actheights.at(ysizeindex));
                    if( ysizeindex < actheights.length())
                    {
                        y_pos = y_pos + S.scr_height * actheights.at(ysizeindex);
                        ysizeindex ++;
                    }
                    S.legendindex ++;
                }
            }
        }
    }*/
}


void WorldWindow::Draw3DUI(GeoWindowState S)
{

        S.legendtotal = 0;
        S.legendindex = 0;

        QList<float> minheights;
        QList<float> maxheights;
        QList<float> actheights;
        float minheight_total = 0.0;
        float maxheight_total = 0.0;

        //all legends
        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            if((!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
            {
                UILayer * l = m_UILayerList.at(i);

                if(l->IsPrepared() == false)
                {
                    l->OnPrepare(m_OpenGLCLManager,S);
                }

                if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && !l->IsDrawAsDEM() && !l->IsDrawAsHillShade())
                {
                    if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                    {
                        S.legendtotal = S.legendtotal + 1;
                        float minheight = l->MinimumLegendHeight(m_OpenGLCLManager,S);
                        float maxheight = l->MaximumLegendHeight(m_OpenGLCLManager,S);

                        minheights.append(minheight);
                        maxheights.append(maxheight);
                        minheight_total += minheight;
                        maxheight_total += maxheight;
                    }
                }
            }
        }
        //Find all the appropriate legend heights and pass them to the layers when drawing

        float threshold = (S.scr_height - 2.0 *S.ui_framewidth)/S.scr_height;
        //if the total of max-height is < 1.0 (full screen height) we continue
        if(maxheight_total > threshold)
        {
            //if the total of min-height is > 1.0, we have to skip the last few layers
            if(minheight_total < threshold)
            {
                //if the total of max-height is > 1.0
                //re-scale all layers as : minheight + (maxheight-minheight) * (threshold-minheight_total) /(maxheight_total-minheight_total)
                //all ui layers

                float fac = std::max(0.00001f,(threshold-minheight_total))/std::max(0.00001f,(maxheight_total-minheight_total));

                for(int i = 0; i < minheights.length(); i++)
                {
                    actheights.append(minheights.at(i) + (maxheights.at(i) - minheights.at(i))*fac);
                }

            }else
            {
                for(int i = 0; i < minheights.length(); i++)
                {
                    actheights.append(minheights.at(i));
                }
            }
        }else
        {
            for(int i = 0; i < minheights.length(); i++)
            {
                actheights.append(minheights.at(i));
            }
        }

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser() && (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3DUI(m_OpenGLCLManager,S, m_TransformManager);
            }
        }
    }
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                    l->OnPostDraw3DUI(m_OpenGLCLManager,S, m_TransformManager);
            }
        }
    }


    float y_pos = 0.0;
    int ysizeindex = 0;
    //all legends
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(!m_UILayerList.at(i)->IsUser()&& (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,S,y_pos,y_pos + S.scr_height * actheights.at(ysizeindex));
                    if( ysizeindex < actheights.length())
                    {
                        y_pos = y_pos + S.scr_height * actheights.at(ysizeindex);
                        ysizeindex ++;
                    }
                    S.legendindex ++;
                }
            }
        }
    }

    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()&& (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                if(l->DrawLegend() && S.draw_legends && S.draw_ui)
                {
                    l->OnDrawLegend(m_OpenGLCLManager,S,y_pos,y_pos + S.scr_height * actheights.at(ysizeindex));
                    if( ysizeindex < actheights.length())
                    {
                        y_pos = y_pos + S.scr_height * actheights.at(ysizeindex);
                        ysizeindex ++;
                    }
                    S.legendindex ++;
                }
            }
        }
    }
}

