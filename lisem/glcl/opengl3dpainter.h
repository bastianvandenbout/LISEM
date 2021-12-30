#ifndef OPENGL3DPAINTER_H
#define OPENGL3DPAINTER_H

#include "defines.h"
#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <map>
#include "linear/lsm_vector2.h"
#include <QList>
#include <iostream>
#include <QApplication>
#include "error.h"
#include "openglprogram.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"
#include "math.h"
#include "color/lsmcolorgradient.h"
#include "site.h"
#include "openglcltexture.h"


class OpenGL3DPainter
{
private:
    QString m_AssetDir;

    QString m_ShaderDir;


    GLuint VAO, VBO, VAO3D, VBO3D,TBO3D,NBO3D;

    OpenGLProgram * m_GLProgram_DrawObject;
    OpenGLProgram * m_GLProgram_DrawTexturedPolygon3D;
public:

    inline OpenGL3DPainter()
    {

        m_AssetDir = GetSite()+"/kernels/";

        m_ShaderDir = GetSite()+"/shaders/";

    }


    GLuint m_RenderTarget;
    int m_RenderTargetWidth;
    int m_RenderTargetHeight;

    inline void UpdateRenderTargetProperties(GLuint render_target, int width, int height)
    {
        m_RenderTarget = render_target;
        m_RenderTargetWidth = width;
        m_RenderTargetHeight = height;

    }

    void LoadShaders();

    //polygon must be convex, so drawn as triangle strip
    inline void Draw3DPolygon(std::vector<LSMVector3> points, std::vector<LSMVector2> uvs, LSMMatrix4x4 cameramatrix, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0), OpenGLCLTexture * tex = nullptr, float tex_weight = 0.0,bool test_depth = true)
    {


        if(test_depth)
        {
            glad_glEnable(GL_DEPTH_TEST);
        }else
        {
            glad_glDisable(GL_DEPTH_TEST);
        }

        //glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);
        //glad_glEnable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawTexturedPolygon3D->m_program);

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,cameramatrix.GetMatrixDataPtr());

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO3D);

        glad_glUniform1f(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"TextureWeight"),tex_weight);


        if(tex != nullptr)
        {

            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"TextureIsText"),0);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"SampleTexture"),1);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"tex1"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,tex->m_texgl);
        }else
        {

            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"TextureIsText"),0);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"SampleTexture"),0);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"tex1"),0);

        }

        for(int i = 1; i < points.size()-1; i+=2)
        {
            int draw_num = 3;

            if((i + 2) < points.size())
            {
                draw_num = 6;
                LSMVector3 normal = LSMVector3::CrossProduct(points[i]-points[0],points[i+1]-points[0]).Normalize();
                LSMVector3 normal2 = LSMVector3::CrossProduct(points[i+1]-points[0],points[i+2]-points[0]).Normalize();
                // Update VBO for each character
                GLfloat vertices[6][3] = {
                    { points[0].x,points[0].y,points[0].z},
                    { points[i].x,points[i].y,points[i].z},
                    { points[i+1].x,points[i+1].y,points[i+1].z},

                    { points[0].x,points[0].y,points[0].z},
                    { points[i+1].x,points[i+1].y,points[i+1].z},
                    { points[i+2].x,points[i+2].y,points[i+2].z},

                };

                GLfloat uv[6][2] = {
                    { uvs[0].x,uvs[0].y},
                    { uvs[i].x,uvs[i].y},
                    { uvs[i+1].x,uvs[i+1].y},

                    { uvs[0].x,uvs[0].y},
                    { uvs[i+1].x,uvs[i+1].y},
                    { uvs[i+2].x,uvs[i+2].y},

                };

                GLfloat normals[6][3] = {
                    { normal.x,normal.y,normal.z},
                    { normal.x,normal.y,normal.z},
                    { normal.x,normal.y,normal.z},

                    { normal2.x,normal2.y,normal2.z},
                    { normal2.x,normal2.y,normal2.z},
                    { normal2.x,normal2.y,normal2.z},

                };



                // Update content of VBO memory
                glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uv), uv);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                glad_glBindBuffer(GL_ARRAY_BUFFER, NBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normals), normals);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            }else
            {
                LSMVector3 normal = LSMVector3::CrossProduct(points[i]-points[0],points[i+1]-points[0]).Normalize();

                // Update VBO for each character
                GLfloat vertices[6][3] = {
                    { points[0].x,points[0].y,points[0].z},
                    { points[i].x,points[i].y,points[i].z},
                    { points[i+1].x,points[i+1].y,points[i+1].z},
                    { 0.0,0.0,0.0},
                    { 0.0,0.0,0.0},
                    { 0.0,0.0,0.0},

                };

                GLfloat uv[6][2] = {
                    { uvs[0].x,uvs[0].y},
                    { uvs[i].x,uvs[i].y},
                    { uvs[i+1].x,uvs[i+1].y},

                    { 1.0, 1.0},
                    { 0.0, 0.0 },
                    { 1.0, 0.0 }

                };

                GLfloat normals[6][3] = {
                    { normal.x,normal.y,normal.z},
                    { normal.x,normal.y,normal.z},
                    { normal.x,normal.y,normal.z},

                    { 0.0,0.0,0.0},
                    { 0.0,0.0,0.0},
                    { 0.0,0.0,0.0},

                };



                // Update content of VBO memory
                glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uv), uv);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                glad_glBindBuffer(GL_ARRAY_BUFFER, NBO3D);
                glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normals), normals);
                glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


            }



            glad_glCullFace(GL_FRONT_AND_BACK);

            // Render quad
            glad_glDrawArrays(GL_TRIANGLES, 0, draw_num);

            glad_glBindVertexArray(0);
            glad_glBindTexture(GL_TEXTURE_2D, 0);

            glad_glEnable(GL_DEPTH_TEST);
        }

    }

    inline void Draw3DSquare(LSMVector3 p1, LSMVector3 p2, LSMVector3 p3, LSMVector3 p4, LSMVector3 CameraPosition, LSMMatrix4x4 cameramatrix, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0), OpenGLCLTexture * tex = nullptr, float tex_weight = 0.0,bool test_depth = true)
    {
        Draw3DPolygon({p1,p2,p3,p4},{LSMVector2(0.0,0.0),LSMVector2(1.0,0.0),LSMVector2(1.0,1.0),LSMVector2(0.0,1.0)},cameramatrix,color,tex,tex_weight,test_depth);
    }
    inline void DrawTexturedPlane(LSMVector3 p1, LSMVector3 p2, LSMVector3 p3, LSMVector3 p4)
    {



    }


    inline void DrawLine()
    {


    }

    inline void DrawArrow()
    {


    }

    inline double RayCastArrow()
    {

        return 0.0;
    }

    inline void DrawBillboardTexture()
    {



    }

    inline void DrawTextBillBoarded(QString text, LSMVector3 topleft)
    {



    }

    inline void DrawText3D(QString text, LSMVector3 topleft, LSMVector3 normal, LSMVector4 Color, float size,LSMVector3 CameraPosition, LSMMatrix4x4 cameramatrix)
    {





    }


    inline void DrawText3DBillboard(QString text, LSMVector3 topleft, LSMVector4 Color, float size)
    {



    }

    inline void Destroy()
    {


    }


};

#endif // OPENGL3DPAINTER_H
