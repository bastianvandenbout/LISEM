#pragma once

#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "gl/openglcldatamanager.h"

class UIPostFXAALayer : public UILayer
{
private:

    OpenGLProgram * m_Program;

protected:


public:

    inline UIPostFXAALayer() : UILayer("FXAA", false,"",false)
    {

         m_IsNative = true;
         m_IsUser = false;
         m_IsLayerSaveAble = true;
         m_IsPostProcessDrawer = true;

         //m_Parameters->AddParameter("Cloud Height",m_CloudHeight,5000.0);
    }

    inline ~UIPostFXAALayer()
    {

    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = true;
    }



    inline QString layertypeName()
    {
        return "PostFXAALayer";
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }
    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDraw3DPostProcess(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

        std::cout << "draw postprocess FXAA" << std::endl;

        float matrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};


        m_Parameters->UpdateParameters();


        glad_glUseProgram(m_Program->m_program);

        glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrix().GetMatrixDataPtr());
        glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixNoTranslation"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslation().GetMatrixDataPtr());
        //glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixWorldToView"),1,GL_FALSE,s.Camera3D->);
        glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iCameraPos"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());
        glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iCameraDir"),s.Camera3D->GetViewDir().x,s.Camera3D->GetViewDir().y,s.Camera3D->GetViewDir().z);
        glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iSolarDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
        glad_glUniform1f(glad_glGetUniformLocation(m_Program->m_program,"iTime"),s.m_time);
        glad_glUniform2f(glad_glGetUniformLocation(m_Program->m_program,"iViewPortSize"),s.GL_PostProcessBuffer2->GetWidth(),s.GL_PostProcessBuffer2->GetHeight());
        glad_glUniform2f(glad_glGetUniformLocation(m_Program->m_program,"iResolution"),s.GL_PostProcessBuffer2->GetWidth(),s.GL_PostProcessBuffer2->GetHeight());

        // bind texture
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"tex"),0);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_PostProcessBuffer1->GetTexture(0));

        glad_glActiveTexture(GL_TEXTURE1);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosX"),1);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(1));

        glad_glActiveTexture(GL_TEXTURE2);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosY"),2);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(2));

        glad_glActiveTexture(GL_TEXTURE3);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosZ"),3);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(3));

        glad_glActiveTexture(GL_TEXTURE4);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"NormalX"),4);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(4));

        glad_glActiveTexture(GL_TEXTURE5);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"NormalY"),5);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(5));

        glad_glActiveTexture(GL_TEXTURE6);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"NormalZ"),6);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(6));

        glad_glActiveTexture(GL_TEXTURE7);
        glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"props"),7);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(7));


        glad_glBindFramebuffer(GL_FRAMEBUFFER, s.GL_PostProcessBuffer2->GetFrameBuffer());


        std::cout << "buffer post 1 " << s.GL_PostProcessBuffer1->GetTexture(0) << std::endl;
        std::cout << "buffer post 2 " << s.GL_PostProcessBuffer2->GetTexture(0) << std::endl;

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glad_glDrawBuffers(1, DrawBuffers);

        glad_glViewport(0,0,s.GL_PostProcessBuffer2->GetWidth(),s.GL_PostProcessBuffer2->GetHeight());

        glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glad_glClearColor(0.0,0.0,0.0,0.0);
        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        // get uniform locations
        int mat_loc = glad_glGetUniformLocation(m_Program->m_program,"matrix");

        // set project matrix
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
        // now render stuff
        glad_glBindVertexArray(m->m_Quad->m_vao);
        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);


    }


    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_Program = m->GetMGLProgram(KernelDir + "PostProcess.vert", KernelDir + "PostFXAA.frag");

        m_IsPrepared = true;

    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
    }

    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {



    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm)
    {

    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }



};
