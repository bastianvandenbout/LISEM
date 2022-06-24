#include "worldwindow.h"


void WorldWindow::DrawToFrameBuffer2D(GeoWindowState S, WorldGLTransformManager * glt, bool external)
{


    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    GLuint FB = S.GL_FrameBuffer->GetFrameBuffer();

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_3DPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
    glad_glViewport(0,0,S.scr_pixwidth,S.scr_pixheight);
    glad_glClearColor(m_BackGroundColor.x,m_BackGroundColor.y,m_BackGroundColor.z,m_BackGroundColor.w);

    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);

    DrawGeoLayers2DColor(S,glt);
    DrawUILayers(S,glt);

    Draw2DArrows(S, external);

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_3DPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
}


void WorldWindow::DrawToFrameBuffer2DElevation(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    GLuint FB = S.GL_FrameBuffer->GetFrameBuffer();

    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);
    m_OpenGLCLManager->m_3DPainter->UpdateRenderTargetProperties(FB,S.scr_pixwidth,S.scr_pixheight);

    glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);
    glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glad_glDisable(GL_DEPTH_TEST);

    static GLfloat mv[] = {-1e30f, 0.0f, 0.0f, 1.0f};
    glad_glClearBufferfv(GL_COLOR, 0, mv);

    glad_glViewport(0,0,S.scr_pixwidth,S.scr_pixheight);

    DrawGeoLayers2DElevation(S,glt);
    m_OpenGLCLManager->m_ShapePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TextPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_TexturePainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);
    m_OpenGLCLManager->m_3DPainter->UpdateRenderTargetProperties(S.GL_PrimaryFrameBuffer->GetFrameBuffer(),m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->m_height);



}

void WorldWindow::DrawGeoLayers2DColor(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()  && (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
                l->OnCRSChanged(m_OpenGLCLManager,S,glt);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && (!l->IsDrawAsDEM() || l->IsDrawAsHillShade()))
            {
                l->OnDrawGeo(m_OpenGLCLManager,S,glt);
            }
        }
    }

    if(m_LayerEditor != nullptr)
    {

        if(m_LayerEditor->IsPrepared() == false)
        {
            m_LayerEditor->OnPrepare(m_OpenGLCLManager);
            m_LayerEditor->OnCRSChanged(m_OpenGLCLManager,S,glt);
        }
        if(m_LayerEditor->ShouldBeRemoved() == false && m_LayerEditor->Exists() && m_LayerEditor->IsDraw())
        {
            m_LayerEditor->OnDrawGeo(m_OpenGLCLManager,S,glt);
        }
    }

}
void WorldWindow::DrawGeoLayers2DElevation(GeoWindowState S, WorldGLTransformManager * glt)
{
    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }


    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if(m_UILayerList.at(i)->IsUser()  && (!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists() && l->IsDraw() && l->IsDrawAsDEM())
            {
                l->OnDrawGeoElevation(m_OpenGLCLManager,S, glt);
            }
        }
    }



}
void WorldWindow::DrawUILayers(GeoWindowState S, WorldGLTransformManager * glt)
{

    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

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
        if(!m_UILayerList.at(i)->IsUser())
        {
            UILayer * l = m_UILayerList.at(i);

            if(l->IsPrepared() == false)
            {
                l->OnPrepare(m_OpenGLCLManager,S);
            }

            if(l->ShouldBeRemoved() == false && l->Exists())
            {
                l->OnDrawGeo(m_OpenGLCLManager,S, glt);
            }
        }
    }

    //all legends
    float y_pos = 0.0;
    int ysizeindex = 0;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        if((!S.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (S.ChannelFilter && S.Channel == m_UILayerList.at(i)->GetChannel())))
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
    }

}
