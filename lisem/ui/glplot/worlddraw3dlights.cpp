#include "worldwindow.h"



void WorldWindow::CreateShadowMaps(GeoWindowState s, bool external = false)
{
    for(int i = 0; i < m_UILightList.length() ; i++)
    {
        if(m_DrawShadows && m_UILightList.at(i)->RequiresShadowMaps())
        {
            m_UILightList.at(i)->SetDoShadowMapping(true);

        }else
        {
            m_UILightList.at(i)->SetDoShadowMapping(false);
        }

        if(m_UILightList.at(i)->IsActive())
        {

            if(!m_UILightList.at(i)->IsPrepared())
            {
                m_UILightList.at(i)->OnPrepare(m_OpenGLCLManager,s);
            }


        }
    }

    //get list of shadowcasters

    std::vector<UILayer *> shadowcasters;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);


        if(l->IsDraw() && l->IsShadowCaster() && (!s.ChannelFilter  || m_UILayerList.at(i)->IsRequired() || (s.ChannelFilter && s.Channel == m_UILayerList.at(i)->GetChannel())))
        {
            shadowcasters.push_back(l);
        }
    }

    //create shadowmaps
    for(int i = 0; i < m_UILightList.length() ; i++)
    {
        if(m_UILightList.at(i)->IsActive())
        {
            m_UILightList.at(i)->OnDrawShadowMaps(m_OpenGLCLManager,s, m_TransformManager,shadowcasters);
        }
    }
}

void WorldWindow::CreateLightBuffer(GeoWindowState s, bool external = false)
{

    //create shadowmaps
    for(int i = 0; i < m_UILightList.length() ; i++)
    {
        if(m_UILightList.at(i)->IsActive())
        {
            m_UILightList.at(i)->OnRenderLightBuffer(m_OpenGLCLManager,s, m_TransformManager,s.GL_3DFrameBuffer,6);
        }
    }



}

void WorldWindow::DeferredLightPass(GeoWindowState s, bool external = false)
{


}
