#ifndef OPENGLCLTEXTURE3D_H
#define OPENGLCLTEXTURE3D_H



#include "defines.h"

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#include "geo/raster/field.h"
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
#include "qimage.h"
#include "QGLWidget"
#include "error.h"

//this class provides a float32 based 3d texture with mipmapping

class LISEM_API OpenGLCLTexture3D
{

    //for 3d object drawing
    QString m_Path;
    QString m_Type;

public:

    cl::size_t<3> m_dims;
    GLuint m_texgl;


public:

    inline OpenGLCLTexture3D()
    {


    }


    inline void CreateFromField(Field * f)
    {
        int errCode = 0;

        std::vector<float> data(f->nrCols()*f->nrRows()*f->nrLevels() , 0.5);

        for(int i = 0; i < f->nrLevels(); i++)
        {
            for(int r = 0; r < f->nrRows(); r++)
            {
                for(int c = 0; c < f->nrCols(); c++)
                {
                    float val = f->ValueAt(i,r,c);
                    data[i * f->nrRows() * f->nrCols() + r *f->nrCols() + c] = f->ValueAt(i,r,c);
                }
            }
        }

        glad_glEnable(GL_TEXTURE_3D); // Enable texturing

        glad_glActiveTexture(GL_TEXTURE0);
        glad_glGenTextures(1, &m_texgl); // Obtain an id for the texture
        glad_glBindTexture(GL_TEXTURE_3D, m_texgl); // Set as the current texture

        m_dims[0] = f->nrCols();
        m_dims[1] = f->nrRows();
        m_dims[2] = f->nrLevels();

        glad_glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, f->nrCols(),f->nrRows(),f->nrLevels(), 0, GL_RED, GL_FLOAT,data.data());

        //for(int i = 0; i < f->nrLevels(); i++)
        //{
        //    cTMap * m = f->GetMapList().at(i);
        //    glad_glTexSubImage3D(GL_TEXTURE_2D,0,0,0,i,m_dims[0],m_dims[1],1,GL_RED,GL_FLOAT,m->data[0]);
//
//        }
        //upload per level
        //

        glad_glBindTexture(GL_TEXTURE_3D, m_texgl);
        //glad_glGenerateMipmap(GL_TEXTURE_3D);

        glad_glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glad_glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glad_glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glad_glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glad_glBindTexture(GL_TEXTURE_3D, 0);
    }

    inline void UpdateDataFromField(Field * f)
    {
        std::vector<float> data(f->nrCols()*f->nrRows()*f->nrLevels() , 0.5);

        for(int i = 0; i < f->nrLevels(); i++)
        {
            for(int r = 0; r < f->nrRows(); r++)
            {
                for(int c = 0; c < f->nrCols(); c++)
                {
                    data[i * f->nrRows() * f->nrCols() + r *f->nrCols() + c] = f->ValueAt(i,r,c);
                }
            }
        }

        glad_glBindTexture(GL_TEXTURE_3D, m_texgl);
        glad_glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, f->nrCols(),f->nrRows(),f->nrLevels(), GL_RED, GL_FLOAT,data.data());
        //glad_glGenerateMipmap(GL_TEXTURE_3D);
        glad_glBindTexture(GL_TEXTURE_3D, 0);
    }

    inline void Destroy()
    {
        glad_glDeleteTextures(1,&m_texgl);
    }
};


#endif // OPENGLCLTEXTURE3D_H
