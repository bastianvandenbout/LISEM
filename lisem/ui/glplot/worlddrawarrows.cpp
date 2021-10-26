#include "worldwindow.h"
#include "gl/openglcldatamanager.h"

void WorldWindow::Draw3DArrows(GeoWindowState s, bool external)
{

    //if not external and in moving state

    if(m_DrawArrows && !external)
    {

        //for all movable objects

        //draw 3d arrows that show up in camera-space with identical sizes (rescale based on distance and fov)

        for(int i = 0; i < m_UILayerList.length() ; i++)
        {

            UILayer * l = m_UILayerList.at(i);

            if(l->IsMovable() || l->IsRotateAble() || l->IsScaleAble())
            {

                if(l->IsMovable() && m_GizmoMode == GIZMO_MOVE)
                {


                    LSMVector3 FLoc = l->GetPosition();

                    //draw 3 arrows aligned with axes
                    //use view-axis aligned line segments (as done in some other 3d viewers/editors)


                    std::cout << "draw position move gizmo " << std::endl;
                    LSMVector3 camerapos = s.Camera3D->GetPosition();

                    LSMVector3 DrawCenter = FLoc  - LSMVector3(camerapos.x,0.0,camerapos.z);

                    LSMVector3 NormalX = LSMVector3(1.0,0.0,0.0) * 0.015 * DrawCenter.length();
                    LSMVector3 NormalY = LSMVector3(0.0,1.0,0.0) * 0.015 * DrawCenter.length();
                    LSMVector3 NormalZ = LSMVector3(0.0,0.0,1.0) * 0.015 * DrawCenter.length();


                    std::cout << "DrawCenter " << DrawCenter.x << " " << DrawCenter.y << " " << DrawCenter.z << std::endl;
                    std::cout << "CameraPos " << camerapos.x << " " << camerapos.y << " " << camerapos.z << std::endl;

                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalX,LSMVector3(0.0,camerapos.y,0.0),5.0,LSMVector4(1.0,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalY,LSMVector3(0.0,camerapos.y,0.0),5.0,LSMVector4(0.0,1.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalZ,LSMVector3(0.0,camerapos.y,0.0),5.0,LSMVector4(0.0,0.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);





                    //draw 3 squares for plane-aligned movement
                    //we have to make sure these are not drawn if they are to close to the arrows


                    /*gl3dObject * m_Actor = m_ArrowActor;
                    std::vector<gl3dMesh *> meshes = m_Actor->GetMeshes();

                    for(int i = 0; i < meshes.size(); i++)
                    {

                        gl3dMesh * mesh = meshes.at(i);

                        LSMVector3 camerapos = s.Camera3D->GetPosition();
                        LSMVector3 objpos = LSMVector3(0.0,0.0,0.0);
                        LSMMatrix4x4 objrotm = LSMMatrix4x4();


                        LSMMatrix4x4 objtrans;
                        objtrans.Translate(LSMVector3(objpos.x - camerapos.x,objpos.y,objpos.z - camerapos.z));
                        objrotm = objtrans*objrotm;

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

                        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());

                        // now render stuff
                        glad_glBindVertexArray(mesh->VAO);
                        glad_glDrawElements(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0);
                        glad_glBindVertexArray(0);

                    }*/


                }
                if(l->IsRotateAble() && m_GizmoMode == GIZMO_ROTATE)
                {

                }
                if(l->IsScaleAble() && m_GizmoMode == GIZMO_SCALE)
                {

                }



            }


        }

        //keep track of the arrows just so we can determine later if the user clicked one of them



    }


}

void WorldWindow::Arrow3DRayCast()
{




}



void WorldWindow::Draw2DArrows(GeoWindowState s, bool external)
{






}

void WorldWindow::Arrow2DRayCast()
{

    //we need to draw the arrows in the middle of the visible domain of  a layer



}
