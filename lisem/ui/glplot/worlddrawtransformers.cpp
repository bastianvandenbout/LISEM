#include "worldwindow.h"

bool WorldWindow::UpdateTransformers()
{

    m_TransformManager->ResetRefsZero();
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);
        if(l->IsGeo())
        {
            UIGeoLayer * gl = (UIGeoLayer *) (l);
            m_TransformManager->Add(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,m_CurrentWindowState.projection, gl->GetProjection());
        }

    }
    m_TransformManager->RemoveWithRefsZero();


    for(int j= 0; j < m_2D3DTransformManager.length();j++)
    {
        WorldGLTransformManager * m = m_2D3DTransformManager.at(j);

        m->ResetRefsZero();
        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            UILayer * l = m_UILayerList.at(i);
            if(l->IsGeo())
            {
                UIGeoLayer * gl = (UIGeoLayer *) (l);
                m->Add(m_2D3DRenderCTargets.at(j)->GetWidth(),m_2D3DRenderCTargets.at(j)->GetHeight(),m_CurrentWindowState.projection, gl->GetProjection());
            }

        }
        m->RemoveWithRefsZero();

    }

    return true;
}
