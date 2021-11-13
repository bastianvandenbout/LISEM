#ifndef GL3DTERRAIN_H
#define GL3DTERRAIN_H


#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "boundingbox.h"
#include "gl/openglcldatamanager.h"

class UITerrainLayer : public UILayer
{
private:


    GL3DGeometry * m_GeometryPlane;
    OpenGLProgram * m_ColoredTerrainProgram;
    OpenGLProgram * m_ColoredTerrainShadowProgram;
protected:


public:

    inline UITerrainLayer() : UILayer("Terrain", false,"",false)
    {
        m_IsNative = true;
        m_IsUser = false;
        m_ShadowCaster = true;
    }

    inline ~UITerrainLayer()
    {

    }


    inline QString layertypeName()
    {
        return "TerrainLayer";
    }


    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

        glad_glClear(GL_DEPTH_BUFFER_BIT);


        s.GL_3DFrameBuffer->SetAsTarget();

        for(int i = s.GL_FrameBuffer3DWindow.length()-1; i > -1; i--)
        {
            double zscale = 1.0f;//s.projection.GetUnitZMultiplier();

            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);
            //set shader uniform values
            OpenGLProgram * program = m_ColoredTerrainProgram;

            // bind shader
            glad_glUseProgram(program->m_program);

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"viewportSize"),s.scr_pixwidth,s.scr_pixheight);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(i).GetCenterX(),0.0,s.GL_FrameBuffer3DWindow.at(i).GetCenterY());
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSize"),s.GL_FrameBuffer3DWindow.at(i).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i).GetSizeY());

            if(i > 0)
            {
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeY());
            }else
            {
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),0,0);
            }

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ZScale"),zscale);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureC"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DColor.at(i)->GetTexture());

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureD"),1);
            glad_glActiveTexture(GL_TEXTURE1);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DElevation.at(i)->GetTexture());


            // now render stuff
            glad_glBindVertexArray(m->m_GeometryPlane->m_vao);
            glad_glPatchParameteri(GL_PATCH_VERTICES,3);
            glad_glDrawElements(GL_PATCHES,m->m_GeometryPlane->m_IndexLength,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);

            glad_glBindTexture(GL_TEXTURE_2D,0);
        }
    }


    inline void OnDraw3DShadowDepth(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, OpenGLCLMSAARenderTarget * target, LSMMatrix4x4 perspectiveview, BoundingBox bb, int scale_level) override
    {

        //get the 3d window we are currently closest to

        int i = 0;
        if(scale_level > -1 && scale_level < s.GL_FrameBuffer3DWindow.size())
        {
            i =scale_level;

        }

        double zscale = 1.0f;//s.projection.GetUnitZMultiplier();


        //set framebuffer to the rendertarget

        target->SetAsTarget();
        glad_glViewport(0,0,target->GetWidth(),target->GetHeight());//m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);


        glad_glClear(GL_DEPTH_BUFFER_BIT);

        glad_glDepthMask(GL_TRUE);
        glad_glEnable(GL_DEPTH_TEST);
        //set shader uniform values
        OpenGLProgram * program = m_ColoredTerrainShadowProgram;

        // bind shader
        glad_glUseProgram(program->m_program);

        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,perspectiveview.GetMatrixDataPtr());
        glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"viewportSize"),target->GetWidth(),target->GetHeight());
        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(i).GetCenterX(),0.0,s.GL_FrameBuffer3DWindow.at(i).GetCenterY());
        glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSize"),s.GL_FrameBuffer3DWindow.at(i).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i).GetSizeY());

        if(i > 0)
        {
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeX(),s.GL_FrameBuffer3DWindow.at(i-1).GetSizeY());
        }else
        {
            glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"TerrainSizeL"),0,0);
        }

        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ZScale"),zscale);

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureC"),0);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DColor.at(i)->GetTexture());

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"TextureD"),1);
        glad_glActiveTexture(GL_TEXTURE1);
        glad_glBindTexture(GL_TEXTURE_2D,s.GL_FrameBuffer3DElevation.at(i)->GetTexture());


        // now render stuff
        glad_glBindVertexArray(m->m_GeometryPlane->m_vao);
        glad_glPatchParameteri(GL_PATCH_VERTICES,3);
        glad_glDrawElements(GL_PATCHES,m->m_GeometryPlane->m_IndexLength,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);

        glad_glBindTexture(GL_TEXTURE_2D,0);

    }




    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_GeometryPlane = new GL3DGeometry();
        m_GeometryPlane->CreateFromRegularUnitGrid(25);

        m_ColoredTerrainProgram = GLProgram_uiterrain;//m->GetMGLProgram(m->m_AssetDir + "terrain_vs.glsl", m->m_AssetDir + "terrain_fs.glsl", m->m_AssetDir + "terrain_gs.glsl", m->m_AssetDir + "terrain_tc.glsl", m->m_AssetDir + "terrain_te.glsl");
        m_ColoredTerrainShadowProgram = GLProgram_uiterrainshadow;//m->GetMGLProgram(m->m_AssetDir + "terrain_vs.glsl", m->m_AssetDir + "terrain_fs.glsl", m->m_AssetDir + "terrain_gs.glsl", m->m_AssetDir + "terrain_tc.glsl", m->m_AssetDir + "terrain_te.glsl");

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


        m_IsCRSChanged = false;

    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }



};




#endif // GL3DTERRAIN_H
