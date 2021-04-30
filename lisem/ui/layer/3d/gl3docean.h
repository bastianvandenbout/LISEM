#pragma once

#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "boundingbox.h"
#include "gl/openglcldatamanager.h"

class UIOceanLayer : public UILayer
{
private:


    GL3DGeometry * m_GeometryPlane;
    OpenGLProgram * m_ColoredTerrainProgram;

    OpenGLCLTexture * m_TextureH = nullptr;
    OpenGLCLTexture * m_TextureN = nullptr;

    OpenGLCLTexture * m_TextureI1 = nullptr;
    OpenGLCLTexture * m_TextureI2 = nullptr;
protected:


public:

    inline UIOceanLayer() : UILayer("Ocean", false,"",false)
    {
        m_IsNative = false;
        m_IsUser = true;
        m_IsLayerSaveAble = true;
    }

    inline ~UIOceanLayer()
    {

    }

    inline QString layertypeName()
    {
        return "OceanLayer";
    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = false;
    }



    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


        s.GL_3DFrameBuffer->SetAsTarget();

        {
            double zscale = s.projection.GetUnitZMultiplier();

            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);

            //set shader uniform values
            OpenGLProgram * program = m_ColoredTerrainProgram;

            // bind shader
            glad_glUseProgram(program->m_program);

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"viewportSize"),s.scr_pixwidth,s.scr_pixheight);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSize"),std::max(10.0,s.GL_FrameBuffer3DWindow.at(0).GetSizeX() * 50.0),std::max(1.0,s.GL_FrameBuffer3DWindow.at(0).GetSizeY() * 50.0));

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ZScale"),zscale);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTextureScale"),100.0 * zscale);

            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time/50.0);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);


            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureH"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureN"),1);
            glad_glActiveTexture(GL_TEXTURE1);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),2);
            glad_glActiveTexture(GL_TEXTURE2);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureI1->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),3);
            glad_glActiveTexture(GL_TEXTURE3);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureI2->m_texgl);


            // now render stuff
            glad_glBindVertexArray(m_GeometryPlane->m_vao);
            glad_glPatchParameteri(GL_PATCH_VERTICES,3);
            glad_glDrawElements(GL_PATCHES,m_GeometryPlane->m_IndexLength,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);
            glad_glBindTexture(GL_TEXTURE_2D,0);
        }
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_TextureI1= new OpenGLCLTexture();
        m_TextureI1->Create2DFromFile(AssetDir + "abstract.jpg");
        m_TextureI2 = new OpenGLCLTexture();
        m_TextureI2->Create2DFromFile(AssetDir + "cloudnoise.jpg");

        m_TextureH = new OpenGLCLTexture();
        m_TextureH->Create2DFromFile(AssetDir + "ocean_waveheight.jpg");
        m_TextureN = new OpenGLCLTexture();
        m_TextureN->Create2DFromFile(AssetDir + "ocean_normal.png");
        m_GeometryPlane = new GL3DGeometry();
        m_GeometryPlane->CreateFromInfiniteCenteredGrid(50);

        m_ColoredTerrainProgram = GLProgram_uiocean;//m->GetMGLProgram(m->m_AssetDir + "terrain_vs.glsl", m->m_AssetDir + "terrain_fs.glsl", m->m_AssetDir + "terrain_gs.glsl", m->m_AssetDir + "terrain_tc.glsl", m->m_AssetDir + "terrain_te.glsl");

        m_IsPrepared = true;
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
        if(m_TextureI1 != nullptr)
        {
            m_TextureI1->Destroy();
            delete m_TextureI1;
            m_TextureI1 = nullptr;
        }
        if(m_TextureI2 != nullptr)
        {
            m_TextureI2->Destroy();
            delete m_TextureI2;
            m_TextureI2 = nullptr;
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
