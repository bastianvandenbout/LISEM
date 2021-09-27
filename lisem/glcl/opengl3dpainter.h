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


class OpenGL3DPainter
{
private:
    QString m_AssetDir;


    GLuint VAO, VBO, VAO3D, VBO3D,TBO3D;

    OpenGLProgram * m_GLProgram_DrawObject;

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

    inline void DrawTexturedPlane()
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

    inline void DrawText3D(QString text, LSMVector3 topleft, LSMVector3 normal, LSMVector4 Color)
    {



    }

    inline void Destroy()
    {


    }


};

#endif // OPENGL3DPAINTER_H
