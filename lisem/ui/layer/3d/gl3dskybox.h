#ifndef GL3DSKYBOX_H
#define GL3DSKYBOX_H


#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "gl/openglcldatamanager.h"

class UISkyBoxLayer : public UILayer
{
private:


    GL3DGeometry * m_GeometryBox;
    OpenGLProgram * m_SkyBoxProgram;

        OpenGLCLTexture * m_TextureH = nullptr;
        OpenGLCLTexture * m_TextureN = nullptr;

protected:


public:

    inline UISkyBoxLayer() : UILayer("SkyLayer", false,"",false)
    {
         m_IsNative = false;
         m_IsUser = true;
         m_IsLayerSaveAble = true;

    }

    inline ~UISkyBoxLayer()
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
        return "SkyLayer";
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }
    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnPostDraw3D(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }

    inline void OnPreDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
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
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);

        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());


        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),0);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),1);
        glad_glActiveTexture(GL_TEXTURE1);
        glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);


        // now render stuff
        glad_glBindVertexArray(m_GeometryBox->m_vao);
        glad_glDrawElements(GL_TRIANGLES,m_GeometryBox->m_IndexLength,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);



        glad_glDepthMask(GL_TRUE);
        glad_glEnable(GL_DEPTH_TEST);

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


#endif // GL3DSKYBOX_H
