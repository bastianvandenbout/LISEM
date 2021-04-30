#ifndef OPENGLSHAPEPAINTER_H
#define OPENGLSHAPEPAINTER_H
#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
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

class LSMColorGradient;

class LISEM_API OpenGLShapePainter
{
public:


    QString m_AssetDir;

    inline OpenGLShapePainter()
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

    GLuint VAO, VBO, VAO3D, VBO3D,TBO3D;

    OpenGLProgram * m_GLProgram_DrawPolygon;
    OpenGLProgram * m_GLProgram_DrawPolygonGradient;
    OpenGLProgram * m_GLProgram_DrawLine;
    OpenGLProgram * m_GLProgram_Draw3DLine;

    inline void LoadShaders()
    {
        m_GLProgram_DrawLine = new OpenGLProgram();
        m_GLProgram_DrawLine->LoadProgram(m_AssetDir+ "UILineDraw.vert", m_AssetDir+ "UILineDraw.frag");

        m_GLProgram_Draw3DLine = new OpenGLProgram();
        m_GLProgram_Draw3DLine->LoadProgram(m_AssetDir+ "UI3DLineDraw.vert", m_AssetDir+ "UI3DLineDraw.frag");

        m_GLProgram_DrawPolygon = new OpenGLProgram();
        m_GLProgram_DrawPolygon->LoadProgram(m_AssetDir+ "UIPolygonDraw.vert", m_AssetDir+ "UIPolygonDraw.frag");

        m_GLProgram_DrawPolygonGradient = new OpenGLProgram();
        m_GLProgram_DrawPolygonGradient->LoadProgram(m_AssetDir+ "UIGradientDraw.vert", m_AssetDir+ "UIGradientDraw.frag");


        glad_glGenVertexArrays(1, &VAO);
        glad_glGenBuffers(1, &VBO);
        glad_glBindVertexArray(VAO);
        glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glad_glEnableVertexAttribArray(0);
        glad_glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindVertexArray(0);

        glad_glGenVertexArrays(1, &VAO3D);
        glad_glGenBuffers(1, &VBO3D);
        glad_glGenBuffers(1, &TBO3D);
        glad_glBindVertexArray(VAO3D);
        glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
        glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
        glad_glEnableVertexAttribArray(0);
        glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
        glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
        glad_glEnableVertexAttribArray(1);
        glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindVertexArray(0);

    }

    inline void DrawPoints()
    {

    }

    inline void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,LSMVector4 color)
    {

    }

    inline void DrawSquareRot(float xmid, float ymid, float width, float height, float rotation,LSMVector4 color)
    {


    }


    void DrawPolygonGradient(float * x, float *y, int length,LSMColorGradient * grad, LSMVector2 dir = LSMVector2(0.0,1.0));
    void DrawPolygonDoubleGradient(float * x, float *y, int length,LSMColorGradient * grad,LSMColorGradient * grad2, LSMVector2 dir = LSMVector2(0.0,1.0));
    void DrawSquare(float xlt, float ylt, float width, float height,LSMVector4 color);
    void DrawSquareGradient(float xlt, float ylt, float width, float height, LSMColorGradient * grad, LSMVector2 dir = LSMVector2(0,1.0));
    void DrawSquareDoubleGradient(float xlt, float ylt, float width, float height, LSMColorGradient * grad,LSMColorGradient * grad2, LSMVector2 dir = LSMVector2(0,1.0));

    inline void DrawSquareLine(float xlt, float ylt, float width, float height, float lwidth, LSMVector4 color)
    {
        float * x = new float[5];
        float * y = new float[5];

        x[0] = xlt;
        y[0] = ylt;
        x[1] = xlt + width;
        y[1] = ylt;
        x[2] = xlt + width;
        y[2] = ylt + height;

        x[3] = xlt;
        y[3] = ylt + height;
        x[4] = xlt;
        y[4] = ylt;

        DrawLines(x,y,lwidth,5,color);

        delete[] x;
        delete[] y;
    }

    inline void DrawLine(float x1, float y1, float x2, float y2, float lwidth, LSMVector4 color)
    {
        float * x = new float[2];
        float * y = new float[2];

        x[0] = x1;
        y[0] = y1;
        x[1] = x2;
        y[1] = y2;

        DrawLines(x,y,lwidth,2,color);

        delete[] x;
        delete[] y;
    }

    inline void DrawSquare(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,LSMVector4 color)
    {


    }

    inline void DrawPolygon(float * x, float *y, int length,LSMVector4 color)
    {

        length = 3*(length / 3);
        if(!(length > 3))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawPolygon->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygon->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawPolygon->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length; i = i+6)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

            if( i + 3 <  length)
            {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        0.0, 0.0 },
                    { x[i+1],  y[i+1],       0.0, 1.0 },
                    { x[i+2],  y[i+2],       1.0, 1.0 },

                    { x[i+3],  y[i+3],      0.0, 0.0 },
                    { x[i+4],  y[i+4],      1.0, 1.0 },
                    { x[i+5],  y[i+5],      1.0, 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            }else {
                // Update VBO for each character
                GLfloat vertices[6][4] = {
                    { x[i],    y[i],        0.0, 0.0 },
                    { x[i+1],  y[i+1],       0.0, 1.0 },
                    { x[i+2],  y[i+2],       1.0, 1.0 },

                    { 0.0f,  0.0f,      0.0, 0.0 },
                    { 0.0f,  0.0f,      1.0, 1.0 },
                    { 0.0f,  0.0f,      1.0, 0.0 }
                  };

               // Update content of VBO memory
               glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
               glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
               glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            }

            // Render quad
            if( i + 3 <  length)
            {
                glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            }else {
                glad_glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;
    }

    inline void DrawLinesInduvidual(float * x, float *y, float width, int length, LSMVector4 color)
    {
        if(!(length > 1))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawLine->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length-1; i = i+2)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

                float dx = x[i] - x[i+1];
                float dy = y[i] - y[i+1];

                float l = sqrt(dx*dx + dy*dy);
                dx = dx/l;
                dy = dy/l;
                if(l > 0.01)
                {
                    float dxe = 0.5f * width *(-dy);
                    float dye = 0.5f * width *(dx);

                    // Update VBO for each character
                    GLfloat vertices[6][4] = {
                        { x[i] - dxe,    y[i] - dye,        0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,       1.0, 1.0 },
                        { x[i+1] - dxe,  y[i+1] - dye,       0.0, 1.0 },

                        { x[i+1] + dxe,  y[i+1] + dye,      1.0, 1.0},
                        { x[i+1] - dxe,  y[i+1] - dye,      0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,      1.0, 0.0 }

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);
                }

        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;


    }

    inline void DrawLines(float * x, float *y, float width, int length, LSMVector4 color)
    {
        if(!(length > 1))
        {
            return;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawLine->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawLine->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        for(int i = 0; i < length-1; i = i+1)
        {
            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, 0);

                float dx = x[i] - x[i+1];
                float dy = y[i] - y[i+1];

                float l = sqrt(dx*dx + dy*dy);
                dx = dx/l;
                dy = dy/l;
                if(l > 0.01)
                {
                    float dxe = 0.5f * width *(-dy);
                    float dye = 0.5f * width *(dx);

                    // Update VBO for each character
                    GLfloat vertices[6][4] = {
                        { x[i] - dxe,    y[i] - dye,        0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,       1.0, 1.0 },
                        { x[i+1] - dxe,  y[i+1] - dye,       0.0, 1.0 },

                        { x[i+1] + dxe,  y[i+1] + dye,      1.0, 1.0},
                        { x[i+1] - dxe,  y[i+1] - dye,      0.0, 0.0 },
                        { x[i] + dxe,  y[i] + dye,      1.0, 0.0 }

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);
                }

        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;

    }

    inline void DrawRegularNGon(float x, float y, float r, int npoints, LSMVector4 color)
    {

        std::vector<float> v_x;
        std::vector<float> v_y;

        for(int i = 0; i < npoints; i++)
        {
            float thetai = float(i) * 2.0f * 3.14159f/((float)(npoints));
            float thetaip = float(i+1) * 2.0f * 3.14159f/((float)(npoints));

            float x_i = x + r*sin(thetai);
            float y_i = y + r*cos(thetai);

            float x_ip = x + r*sin(thetaip);
            float y_ip = y + r*cos(thetaip);

            bool connect = (i > 0);

            v_x.push_back(x_i);
            v_y.push_back(y_i);
            v_x.push_back(x_ip);
            v_y.push_back(y_ip);
            v_x.push_back(x);
            v_y.push_back(y);
        }

        DrawPolygon(v_x.data(),v_y.data(),v_x.size(),color);

    }

    inline void DrawRegularNGonOutline(float x, float y, float r, float width, int npoints, LSMVector4 color)
    {
        std::vector<float> v_x;
        std::vector<float> v_y;

        for(int i = 0; i < npoints; i++)
        {
            float thetai = float(i) * 2.0f * 3.14159f/((float)(npoints));
            float thetaip = float(i+1) * 2.0f * 3.14159f/((float)(npoints));

            float x_i = x + r*sin(thetai);
            float y_i = y + r*cos(thetai);

            float x_ip = x + r*sin(thetaip);
            float y_ip = y + r*cos(thetaip);

            bool connect = (i > 0);

            v_x.push_back(x_i);
            v_y.push_back(y_i);
            v_x.push_back(x_ip);
            v_y.push_back(y_ip);
        }

        DrawLines(v_x.data(),v_y.data(),v_x.size(),width,color);

    }

    inline void Destroy()
    {
        //delete shaders and buffer object


    }


    inline void DrawLine3D(LSMVector3 p0, LSMVector3 p1, LSMVector3 CameraPosition, float width, LSMVector4 color, LSMMatrix4x4 cameramatrix, bool with_is_cameraspace)
    {

        //glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);
        glad_glEnable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_Draw3DLine->m_program);

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_Draw3DLine->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,cameramatrix.GetMatrixDataPtr());

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_Draw3DLine->m_program, "inColor"), color.x, color.y, color.z, color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO3D);

        LSMVector3 dir = p1 - p0;
        LSMVector3 viewdir0 = p0 - CameraPosition;
        LSMVector3 viewdir1 = p1 - CameraPosition;
        LSMVector3 tangent0 = LSMVector3::CrossProduct(dir,viewdir0).Normalize();
        LSMVector3 tangent1 = LSMVector3::CrossProduct(dir,viewdir1).Normalize();

        LSMVector3 c1 = p0 + tangent0 * 0.5f * width;
        LSMVector3 c2 = p0 - tangent0 * 0.5f * width;
        LSMVector3 c3 = p1 + tangent1 * 0.5f * width;
        LSMVector3 c4 = p1 - tangent1 * 0.5f * width;

                    // Update VBO for each character
                    GLfloat vertices[6][3] = {
                        { c1.x,c1.y,c1.z},
                        { c3.x,c3.y,c3.z},
                        { c2.x,c2.y,c2.z},

                        { c2.x,c2.y,c2.z},
                        { c4.x,c4.y,c4.z},
                        { c3.x,c3.y,c3.z},

                    };

                    GLfloat uv[6][2] = {
                        { 0.0, 0.0 },
                        { 1.0, 1.0 },
                        { 0.0, 1.0 },

                        { 1.0, 1.0},
                        { 0.0, 0.0 },
                        { 1.0, 0.0 }

                    };

                    // Update content of VBO memory
                    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
                    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uv), uv);
                    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


                    glad_glCullFace(GL_FRONT_AND_BACK);

                    // Render quad
                    glad_glDrawArrays(GL_TRIANGLES, 0, 6);



        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

    }

};

#endif // OPENGLSHAPEPAINTER_H
