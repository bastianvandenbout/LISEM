#ifndef OPENGLCLTEXTURE_H
#define OPENGLCLTEXTURE_H


#include "defines.h"

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "OpenGLUtil.h"

#include <QList>
#include <QString>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "masked_raster.h"
#include "geo/raster/map.h"
#include "qimage.h"
#include "QGLWidget"
#include "error.h"

class LISEM_API OpenGLCLTexture
{
public:
    QImage m_Image;

    cl::ImageGL m_texcl;
    cl::size_t<3> m_dims;
    GLuint m_FramebufferName;
    GLuint m_texgl;
    bool m_IsFrambuffer;

    //for 3d object drawing
    QString m_Path;
    QString m_Type;

    int Operator = 0;
    float Strength = 1.0;
    int MapU = 0;
    int MapV = 0;
public:
    OpenGLCLTexture();

    inline static OpenGLCLTexture GetTempTextureFromExistingGLUINT(GLuint i)
    {
        OpenGLCLTexture ret;
        ret.m_texgl = i;
        return ret;
    }

    inline int GetWidth()
    {
        return m_dims[0];
    }
    inline int GetHeight()
    {
        return m_dims[1];
    }
    inline void SetPath(QString t)
    {
        m_Path = t;
    }

    inline void SetType(QString t)
    {
        m_Type = t;
    }

    inline QString GetPath()
    {
        return m_Path;
    }

    inline QString GetType()
    {
        return m_Type;
    }

    inline int Create2D(cl::Context &c, int width, int height,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA, bool render_target = false)
    {

        int errCode = 0;
        m_texgl = createTexture2D(width,height);
        m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DRF32Linear(cl::Context &c, int width, int height,float value_init = 0, bool render_target = false)
    {
        std::vector<float> data(width * height , value_init);

        int errCode = 0;
        m_texgl = createTexture2DLinear(width,height,data.data(),GL_RED,GL_FLOAT,GL_R32F);
        m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DRF32(cl::Context &c, int width, int height,float value_init = 0, bool render_target = false, bool use_cl = true)
    {
        std::vector<float> data(width * height , value_init);

        int errCode = 0;
        m_texgl = createTexture2D(width,height,data.data(),GL_RED,GL_FLOAT,GL_R32F);
        if(use_cl)
        {
            m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        }
        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int FillData2DRF32(void * data)
    {
        glad_glBindTexture(GL_TEXTURE_2D, this->m_texgl);
        glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_dims[0],m_dims[1],GL_RED,GL_FLOAT,data);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        return 0;
    }

    inline int Create2DFromFile(QString path)
    {


        bool load = m_Image.load(path);
        if(!load)
        {
            LISEM_ERROR("Could not load texture " + path);
            return 1;
        }

        m_Image = QGLWidget::convertToGLFormat(m_Image);



        glad_glEnable(GL_TEXTURE_2D); // Enable texturing

        glad_glActiveTexture(GL_TEXTURE0);
        glad_glGenTextures(1, &m_texgl); // Obtain an id for the texture
        glad_glBindTexture(GL_TEXTURE_2D, m_texgl); // Set as the current texture

        glad_glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        m_dims[0] = m_Image.width();
        m_dims[1] = m_Image.height();
        m_dims[2] = 0;

        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Image.width(), m_Image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Image.bits());

        glad_glGenerateMipmap(GL_TEXTURE_2D);

        glad_glBindTexture(GL_TEXTURE_2D, m_texgl);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glad_glBindTexture(GL_TEXTURE_2D, 0);
        return 0;
    }

    inline int Create2DFromColor(cl::Context &c, int r, int g, int b, int a = 255)
    {



        return 0;
    }

    inline int Create2DFromMap(cl::Context &c, MaskedRaster<float> * map, bool render_target = false, bool flipv = false)
    {

        int errCode = 0;

        if(flipv)
        {
            std::vector<float> data(map->nr_cols() * map->nr_rows() , 0.0);

            FOR_ROW_COL_RASTER_MV(map)
            {
                data[r * map->nr_cols() + c] = (*map)[map->nr_rows() -1 -r][ c];
            }
            m_texgl = createTexture2D(map->nr_cols(),map->nr_rows(), map->data(),GL_RED,GL_FLOAT,GL_R32F);

        }else
        {
            m_texgl = createTexture2D(map->nr_cols(),map->nr_rows(), map->data(),GL_RED,GL_FLOAT,GL_R32F);
        }

        glad_glFinish();
        try
        {
            std::cout << "create CL Image " << (&c) << " " << m_texgl << std::endl;
            m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);

        }catch(...)
        {

            m_dims[0] = map->nr_cols();
            m_dims[1] = map->nr_rows();
            m_dims[2] = 0;

            if(render_target)
            {
                m_FramebufferName = 0;
                m_IsFrambuffer = true;
                glad_glGenFramebuffers(1, &m_FramebufferName);
                glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
                glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
                if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    m_IsFrambuffer = false;
                }
            }



            std::cout<<"Error converting GL Image to CL Texture, is OpenGL Device equal to OpenCL Device?"<<errCode<<std::endl;
            return 250;
        }

        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = map->nr_cols();
        m_dims[1] = map->nr_rows();
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }

    inline int Create2DFromMaps(cl::Context &c,std::vector<cTMap *> maps, bool render_target = false, bool flipv = false)
    {
        if(maps.size() == 0)
        {
            std::cout<<"Failed to create OpenGL texture refrence with no maps as input" <<std::endl;
            return 250;
        }

        int errCode = 0;

        int bands = std::min(4,(int) maps.size());
        cTMap * map = maps.at(0);


        std::vector<float> data(bands * maps.at(0)->nrCols() * maps.at(0)->nrRows() , 0.0);

        if(flipv)
        {
            std::vector<float> data(map->nrCols() * map->nrRows() , 0.0);

            FOR_ROW_COL_MV(maps.at(0))
            {

                data[(bands) *(r * map->nrCols() + c) + 0] = maps.at(0)->data[maps.at(0)->nrRows() -1 -r][c];
                if(bands > 1)
                {
                    data[(bands) *(r * map->nrCols() + c) + 1] = maps.at(1)->data[maps.at(0)->nrRows() -1 -r][c];
                }
                if(bands > 2)
                {
                    data[(bands) *(r * map->nrCols() + c) + 2] = maps.at(2)->data[maps.at(0)->nrRows() -1 -r][c];
                }
                if(bands > 3)
                {
                    data[(bands) *(r * map->nrCols() + c) + 3] = maps.at(3)->data[maps.at(0)->nrRows() -1 -r][c];
                }
            }
        }else
        {
            FOR_ROW_COL_MV(maps.at(0))
            {

                data[(bands) *(r * map->nrCols() + c) + 0] = maps.at(0)->data[r][c];
                if(bands > 1)
                {
                    data[(bands) *(r * map->nrCols() + c) + 1] = maps.at(1)->data[r][c];
                }
                if(bands > 2)
                {
                    data[(bands) *(r * map->nrCols() + c) + 2] = maps.at(2)->data[r][c];
                }
                if(bands > 3)
                {
                    data[(bands) *(r * map->nrCols() + c) + 3] = maps.at(3)->data[r][c];
                }

            }
        }

        if(bands == 1)
        {
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), data.data(),GL_RED,GL_FLOAT,GL_R32F);
        }else if(bands == 2)
        {
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), data.data(),GL_RG,GL_FLOAT,GL_RG32F);
        }else if(bands == 3)
        {
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), data.data(),GL_RGB,GL_FLOAT,GL_RGB32F);
        }else if(bands == 4)
        {
            m_texgl = createTexture2D(map->nrCols(),map->nrRows(), data.data(),GL_RGBA,GL_FLOAT,GL_RGBA32F);
        }

        glad_glFinish();
        try
        {
            std::cout << "create CL Image " << (&c) << " " << m_texgl << std::endl;
            m_texcl = cl::ImageGL(c,CL_MEM_READ_WRITE,GL_TEXTURE_2D,0,m_texgl,&errCode);
        }catch(...)
        {
            std::cout<<"Error converting GL Image to CL Texture, is OpenGL Device equal to OpenCL Device?"<<errCode<<std::endl;
            return 250;
        }

        if(render_target)
        {
            m_FramebufferName = 0;
            m_IsFrambuffer = true;
            glad_glGenFramebuffers(1, &m_FramebufferName);
            glad_glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName);
            glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texgl, 0);
            if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                m_IsFrambuffer = false;
            }
        }

        m_dims[0] = map->nrCols();
        m_dims[1] = map->nrRows();
        m_dims[2] = 0;

        if (errCode!=CL_SUCCESS) {
            std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
            return 250;
        }

        return errCode;
    }


    inline int Create2DFromData(cl::Context &c, float * data, int width, int height)
    {



        return 0;
    }
    inline int Create2DFromData(cl::Context &c, char * data, int width, int height)
    {



        return 0;
    }



    inline void Destroy()
    {
        glad_glDeleteTextures(1,&m_texgl);
    }
};


class OpenCLTexture
{
public:
    cl::Image2D m_texcl;
    cl::size_t<3> m_dims;


public:
    inline OpenCLTexture() : m_texcl()
    {

    }

    inline int Create2DRF32(cl::Context &c, int width, int height,float value_init = 0)
    {

        m_dims[0] = width;
        m_dims[1] = height;
        m_dims[2] = 0;


        std::size_t row_pitch = 0;
        cl_int err = 0;
        int errCode = 0;
        try{
            m_texcl = cl::Image2D(c, CL_MEM_READ_WRITE , cl::ImageFormat(CL_R, CL_FLOAT), width, height, row_pitch, nullptr,&err);// (void*)image.data);
        }catch(cl::Error &e){
            std::cout<<"Failed to create OpenGL texture refrence (RF32) : "<< err << " " <<  e.what() << " " <<  m_dims[0] << " " << m_dims[1] << std::endl;
            errCode = err;
        }

        std::cout << "success " << m_dims[0] << " " << m_dims[1] << std::endl;

        return errCode;
    }

    inline int Create2DFromMap(cl::Context &c, MaskedRaster<float> * map)
    {

        m_dims[0] = map->nr_cols();
        m_dims[1] = map->nr_rows();
        m_dims[2] = 0;


        std::size_t row_pitch = 0;
        cl_int err = 0;
        int errCode = 0;
        try{
            m_texcl = cl::Image2D(c, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, cl::ImageFormat(CL_R, CL_FLOAT), map->nr_cols(), map->nr_rows(), row_pitch, (*map)[0],&err);// (void*)image.data);
        }catch(cl::Error &e){
            std::cout<<"Failed to create OpenGL texture refrence (Map) : "<< err << " " <<  e.what() << " " <<  m_dims[0] << " " << m_dims[1] << std::endl;
            errCode = err;
        }

        std::cout << "success " << m_dims[0] << " " << m_dims[1] << std::endl;

        return errCode;
    }


    inline void Destroy()
    {

    }
};


struct ModelTexture
{
    bool m_IsCPU = false;
    bool m_IsCLOnly = false;
    OpenGLCLTexture * m_TextureGPU;
    OpenCLTexture * m_TextureGPUCL;
    MaskedRaster<float> * m_TextureCPU = nullptr;

};

#endif // OPENGLCLTEXTURE_H
