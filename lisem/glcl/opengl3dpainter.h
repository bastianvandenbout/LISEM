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


class opengl3dpainter
{
    QString m_AssetDir;

    inline opengl3dpainter()
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

    inline void LoadShaders()
    {


    }

    inline void DrawTexturedPlane()
    {



    }

    inline void DrawBillboardTexture()
    {



    }

    inline void DrawTextBillBoarded()
    {



    }


};

#endif // OPENGL3DPAINTER_H
