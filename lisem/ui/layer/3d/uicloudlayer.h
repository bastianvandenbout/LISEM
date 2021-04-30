#ifndef UICLOUDLAYER_H
#define UICLOUDLAYER_H

#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "gl/openglcldatamanager.h"
class UICloudLayer : public UILayer
{
private:


    GL3DGeometry * m_GeometryBox;
    OpenGLProgram * m_SkyBoxProgram;

        OpenGLCLTexture * m_TextureH = nullptr;
        OpenGLCLTexture * m_TextureN = nullptr;


        double m_CloudHeight = 5000.0;
        int m_CloudIterations = 30;
        double m_CloudThickness = 500.0;
        double m_CloudSize = 3;
        double m_CloudRoughness = 0.0;
        double m_CloudOpacity = 0.4;
        double m_CloudCover = 0.2;

protected:


public:

    inline UICloudLayer() : UILayer("CloudLayer", false,"",false)
    {
         m_IsNative = false;
         m_IsUser = true;
         m_IsLayerSaveAble = true;


         m_Parameters->AddParameter("Cloud Height",m_CloudHeight,5000.0);
         m_Parameters->AddParameter("Cloud Thickness",m_CloudThickness,50.0);
         m_Parameters->AddParameter("Cloud Iterations",m_CloudIterations,30,true,6,100);
         m_Parameters->AddParameter("Cloud Size",m_CloudSize,3);
         m_Parameters->AddParameter("Cloud Density",m_CloudRoughness,0.0);
         m_Parameters->AddParameter("Cloud Opacity",m_CloudOpacity,0.4);
         m_Parameters->AddParameter("Cloud Cover",m_CloudCover,0.2);
    }

    inline ~UICloudLayer()
    {

    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = false;
    }



    inline QString layertypeName()
    {
        return "CloudLayer";
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }
    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnPostDraw3D(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

        m_Parameters->UpdateParameters();

        std::cout << "Cloudheight" << m_CloudHeight << std::endl;

        s.GL_3DFrameBuffer->SetAsTarget();

        glad_glDepthMask(GL_FALSE);
        glad_glDisable(GL_DEPTH_TEST);

        //set shader uniform values
        OpenGLProgram * program = m_SkyBoxProgram;

        // bind shader
        glad_glUseProgram(program->m_program);

        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationNormalZ().GetMatrixDataPtr());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),1);
        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iCloudHeight"),m_CloudHeight);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iCloudThickness"),m_CloudThickness);
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iCloudIter"),m_CloudIterations);
        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iCloudCover"),1.0-m_CloudCover);

        glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"cloudSize"),m_CloudSize,m_CloudSize);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"cloudRoughness"),m_CloudRoughness);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"cloudOpacity"),m_CloudOpacity);


        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),0);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),1);
        glad_glActiveTexture(GL_TEXTURE1);
        glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


        glad_glActiveTexture(GL_TEXTURE2);
        glad_glBindTexture(GL_TEXTURE_2D,s.ScreenPosXTex);


        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


        glad_glActiveTexture(GL_TEXTURE3);
        glad_glBindTexture(GL_TEXTURE_2D,s.ScreenPosYTex);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glad_glActiveTexture(GL_TEXTURE4);
        glad_glBindTexture(GL_TEXTURE_2D,s.ScreenPosZTex);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iPosX"),2);
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iPosY"),3);
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iPosZ"),4);




        // now render stuff
        glad_glBindVertexArray(m_GeometryBox->m_vao);
        glad_glDrawElements(GL_TRIANGLES,m_GeometryBox->m_IndexLength,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);



        glad_glDepthMask(GL_TRUE);
        glad_glEnable(GL_DEPTH_TEST);
    }

    inline void OnPreDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_TextureH = new OpenGLCLTexture();
        m_TextureH->Create2DFromFile(AssetDir + "abstract.png");
        m_TextureN = new OpenGLCLTexture();
        m_TextureN->Create2DFromFile(AssetDir + "cloudnoise.jpg");
        m_GeometryBox = new GL3DGeometry();
        m_GeometryBox->CreateFromUnitBox();

        m_SkyBoxProgram = m->GetMGLProgram(KernelDir + "skybox.vert", KernelDir + "skybox.frag");

        m_IsPrepared = true;

    }
    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

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


        m_IsPrepared = false;
    }

    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {



    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm)
    {


        m_IsCRSChanged = false;

    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }



};


#endif // UICLOUDLAYER_H
