#pragma once

#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "models/3dmodel.h"
#include "opengl3dobject.h"
#include "gl/openglcldatamanager.h"

class UI3DObjectLayer : public UIGeoLayer
{
private:


    ModelGeometry* m_Model;
    gl3dObject * m_Actor;

protected:


public:

    inline UI3DObjectLayer() : UIGeoLayer(GeoProjection(),BoundingBox(),"3D Object",false,"",false)
    {
         m_IsNative = false;
         m_IsUser = true;
         m_IsLayerSaveAble = true;
         m_is3D = true;

         m_IsMovable = true;

    }

    inline UI3DObjectLayer(ModelGeometry*g,QString name, bool is_file, QString file_name) : UIGeoLayer(GeoProjection(),BoundingBox(g->GetBoundingBox().GetMinX(),g->GetBoundingBox().GetMaxX(),g->GetBoundingBox().GetMinZ(),g->GetBoundingBox().GetMaxZ()),QString("3D Object"),false,QString(""),false)
    {
         m_Model = g;
         m_IsNative = false;
         m_IsUser = true;
         m_IsLayerSaveAble = true;
         m_is3D = true;


         m_IsMovable = true;

         m_BoundingBox3D = g->GetBoundingBox();
         m_Position = LSMVector3(m_BoundingBox3D.GetCenterX(),m_BoundingBox3D.GetCenterY(),m_BoundingBox3D.GetCenterZ());
    }

    inline ~UI3DObjectLayer()
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
        return "UI3DObjectLayer";
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

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_Actor = new gl3dObject(m,m_Model);

        m_IsPrepared = true;

    }
    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


    }

    inline void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        std::vector<gl3dMesh *> meshes = m_Actor->GetMeshes();

        for(int i = 0; i < meshes.size(); i++)
        {

            gl3dMesh * mesh = meshes.at(i);

            LSMVector3 camerapos = s.Camera3D->GetPosition();
            LSMVector3 objpos = LSMVector3(0.0,0.0,0.0) + m_Position;
            LSMMatrix4x4 objrotm = LSMMatrix4x4();
            LSMMatrix4x4 scalem = LSMMatrix4x4();
            scalem.SetScaling(m_Scale.x,m_Scale.y,m_Scale.z);


            LSMMatrix4x4 objtrans;
            objtrans.Translate(LSMVector3(objpos.x - camerapos.x,objpos.y,objpos.z - camerapos.z));
            objrotm = objtrans*objrotm*scalem;

            s.GL_3DFrameBuffer->SetAsTarget();

            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);

            //set shader uniform values
            OpenGLProgram * program = GLProgram_uiobject;


            // bind shader
            glad_glUseProgram(program->m_program);

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"OMatrix"),1,GL_FALSE,(float*)(&objrotm));//.GetMatrixDataPtr());

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjTranslate"),objpos.x - camerapos.x,objpos.y,objpos.z  - camerapos.z);

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"Transparancy"),1.0 - this->GetStyle().GetTransparancy());


            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());


            /*glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),1);
            glad_glActiveTexture(GL_TEXTURE1);
            glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);*/


            // now render stuff
            glad_glBindVertexArray(mesh->VAO);
            glad_glDrawElements(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);

        }

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
