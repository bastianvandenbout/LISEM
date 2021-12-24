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


    GLuint VAO, VBO, VAO3D, VBO3D,TBO3D,NBO3D;

    OpenGLProgram * m_GLProgram_DrawObject;
    OpenGLProgram * m_GLProgram_DrawTexturedPolygon3D;
public:

    inline OpenGL3DPainter()
    {

        m_AssetDir = GetSite()+"/kernels/";


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
    inline void DrawTexturedPolygon(std::vector<LSMVector3> points, std::vector<LSMVector2> uvs, OpenGLCLTexture * tex, LSMMatrix4x4 cameramatrix, bool test_depth = true, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0))
    {


        if(test_depth)
        {
            glad_glEnable(GL_DEPTH_TEST);
        }else
        {
            glad_glDisable(GL_DEPTH_TEST);
        }

        //glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);
        glad_glEnable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawTexturedPolygon3D->m_program);

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,cameramatrix.GetMatrixDataPtr());

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO3D);

        for(int i = 0; i < length; i = i+6)
        {


                    // Update VBO for each character
                    GLfloat vertices[6][3] = {
                        { c1.x,c1.y,c1.z},
                        { c3.x,c3.y,c3.z},
                        { c2.x,c2.y,c2.z},

                        { c1.x,c1.y,c1.z},
                        { c4.x,c4.y,c4.z},
                        { c3.x,c3.y,c3.z},

                    };

                    GLfloat uvs[6][2] = {
                        { 0.0, 0.0 },
                        { 1.0, 1.0 },
                        { 0.0, 1.0 },

                        { 1.0, 1.0},
                        { 0.0, 0.0 },
                        { 1.0, 0.0 }

                    };

                    GLfloat normals[6][3] = {
                        { c1.x,c1.y,c1.z},
                        { c3.x,c3.y,c3.z},
                        { c2.x,c2.y,c2.z},

                        { c1.x,c1.y,c1.z},
                        { c4.x,c4.y,c4.z},
                        { c3.x,c3.y,c3.z},

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) *3 * 6, vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) *2 * 6, uvs);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glad_glBindBuffer(GL_ARRAY_BUFFER, NBO3D);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) *3 * 6, normals);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    glad_glCullFace(GL_FRONT_AND_BACK);

                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);



        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        glad_glEnable(GL_DEPTH_TEST);

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

    inline void DrawText3D(QString text, LSMVector3 topleft, LSMVector3 normal, LSMVector4 Color, float size)
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
