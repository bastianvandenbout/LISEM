#include "worldwindow.h"




bool WorldWindow::DrawPostProcess(GeoWindowState &s, WorldGLTransformManager * glt)
{
    //check if there are any post-process things at all


    int n = 0;
    for(int i = 0; i < m_UILayerList.length() ; i++)
    {
        UILayer * l = m_UILayerList.at(i);

        bool post = l->IsPostProcess();
        if(post)
        {
            n++;
        }
        post = l->IsPostPostProcess();
        if(post)
        {
            n++;
        }
    }

    if(n == 0)
    {
        return false;
    }



    //now do the actual post-processing

    if(glt == nullptr)
    {
        glt =  m_TransformManager;
    }

    if(s.is_3d)
    {


        //first copy color buffer to first source texture
        {
            glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_PostProcessBuffer1->GetFrameBuffer());
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glad_glDrawBuffers(1, DrawBuffers);

            glad_glViewport(0,0,s.GL_PostProcessBuffer1->GetWidth(),s.GL_PostProcessBuffer1->GetHeight());//m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

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
        }



        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            if(!m_UILayerList.at(i)->IsUser())
            {
                UILayer * l = m_UILayerList.at(i);

                if(l->IsPrepared() == false)
                {
                    l->OnPrepare(m_OpenGLCLManager,s);
                }

                if(l->ShouldBeRemoved() == false && l->Exists())
                {
                     if(l->IsPostProcess())
                     {
                         l->OnDraw3DPostProcess(m_OpenGLCLManager,s, m_TransformManager);
                         OpenGLCLMSAARenderTarget * temp = s.GL_PostProcessBuffer1;
                         s.GL_PostProcessBuffer1 = s.GL_PostProcessBuffer2;
                         s.GL_PostProcessBuffer2 = temp;
                     }
                }
            }
        }
        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            if(m_UILayerList.at(i)->IsUser())
            {
                UILayer * l = m_UILayerList.at(i);

                if(l->IsPrepared() == false)
                {
                    l->OnPrepare(m_OpenGLCLManager,s);
                }

                if(l->ShouldBeRemoved() == false && l->Exists())
                {
                    if(l->IsPostProcess())
                    {
                        l->OnDraw3DPostProcess(m_OpenGLCLManager,s, m_TransformManager);
                        OpenGLCLMSAARenderTarget * temp = s.GL_PostProcessBuffer1;
                        s.GL_PostProcessBuffer1 = s.GL_PostProcessBuffer2;
                        s.GL_PostProcessBuffer2 = temp;
                    }
                }
            }
        }


        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            if(!m_UILayerList.at(i)->IsUser())
            {
                UILayer * l = m_UILayerList.at(i);

                if(l->IsPrepared() == false)
                {
                    l->OnPrepare(m_OpenGLCLManager,s);
                }

                if(l->ShouldBeRemoved() == false && l->Exists())
                {
                    if(l->IsPostPostProcess())
                    {
                        l->OnDraw3DPostPostProcess(m_OpenGLCLManager,s, m_TransformManager);
                        OpenGLCLMSAARenderTarget * temp = s.GL_PostProcessBuffer1;
                        s.GL_PostProcessBuffer1 = s.GL_PostProcessBuffer2;
                        s.GL_PostProcessBuffer2 = temp;
                    }
                }
            }
        }
        for(int i = 0; i < m_UILayerList.length() ; i++)
        {
            if(m_UILayerList.at(i)->IsUser())
            {
                UILayer * l = m_UILayerList.at(i);

                if(l->IsPrepared() == false)
                {
                    l->OnPrepare(m_OpenGLCLManager,s);
                }

                if(l->ShouldBeRemoved() == false && l->Exists())
                {
                    if(l->IsPostPostProcess())
                    {
                        l->OnDraw3DPostPostProcess(m_OpenGLCLManager,s, m_TransformManager);
                        OpenGLCLMSAARenderTarget * temp = s.GL_PostProcessBuffer1;
                        s.GL_PostProcessBuffer1 = s.GL_PostProcessBuffer2;
                        s.GL_PostProcessBuffer2 = temp;
                    }
                }
            }
        }


        //first copy back final color buffer to initial source texture
        //first copy color buffer to first source texture
        {



            glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_FrameBuffer->GetFrameBuffer());
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glad_glDrawBuffers(1, DrawBuffers);

            glad_glViewport(0,0,s.GL_PostProcessBuffer1->GetWidth(),s.GL_PostProcessBuffer1->GetHeight());//m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);


            glad_glDisable(GL_DEPTH_TEST);
            // bind shader
            glad_glUseProgram(m_OpenGLCLManager->m_GLProgram_CopyText->m_program);
            // get uniform locations
            int mat_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"matrix");
            int tex_loc = glad_glGetUniformLocation(m_OpenGLCLManager->m_GLProgram_CopyText->m_program,"tex");
            // bind texture

            glad_glUniform1i(tex_loc,0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D, (s.GL_PostProcessBuffer1->GetTexture(0)));//m_2D3DRenderDTargets.at(0)->GetTexture(0));//
            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            // now render stuff
            glad_glBindVertexArray(m_OpenGLCLManager->m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);
        }


    }else
    {
        //not 3d, so do 2d post





    }




    return true;


}






