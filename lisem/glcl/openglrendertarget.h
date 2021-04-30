#ifndef OPENGLRENDERTARGET_H
#define OPENGLRENDERTARGET_H

#include "glad/glad.h"
#include "QImage"
#include "iostream"

class OpenGLCLRenderTarget
{
private:

    int m_FrameBufferWidth = 0;
    int m_FrameBufferHeight = 0;
    int m_MSAA = 1;
    bool m_Created = false;

GLuint FramebufferName = 0;
GLuint renderedTexture;
GLuint depthrenderbuffer;
    int m_format;
    int m_components;
    int m_datatype;

public:

    inline OpenGLCLRenderTarget()
    {
        m_format = GL_RGBA8;
        m_components = GL_RGBA;
        m_datatype = GL_UNSIGNED_BYTE;

    }

    inline int GetWidth()
    {
        return m_FrameBufferWidth;
    }

    inline int GetHeight()
    {
        return m_FrameBufferHeight;
    }

    inline void Create(int width, int height, int format, int components, int datatype)
    {
        m_format = format;
        m_components = components;
        m_datatype = datatype;

        m_FrameBufferWidth = width;
        m_FrameBufferHeight = height;

        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        GLuint FramebufferName = 0;
        glad_glGenFramebuffers(1, &FramebufferName);
        glad_glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        // The texture we're going to render to
        GLuint renderedTexture;
        glad_glGenTextures(1, &renderedTexture);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glad_glBindTexture(GL_TEXTURE_2D, renderedTexture);

        // Give an empty image to OpenGL ( the last "0" )
        glad_glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // The depth buffer
        GLuint depthrenderbuffer;
        glad_glGenRenderbuffers(1, &depthrenderbuffer);
        glad_glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glad_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
        glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

        // Set "renderedTexture" as our colour attachement #0
        glad_glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glad_glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "!!!!!!!!!!!!!! ERROR " << std::endl;
        }else {
            std::cout << "success with framebuffer" << std::endl;
        }

        m_Created = true;
    }


    inline void CreateR32(int width, int height)
    {
        Create(width,height,GL_R32F,GL_RED,GL_FLOAT);


    }
    inline void CreateRGBA8(int width, int height)
    {

        Create(width,height,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE);
    }



    inline void Resize(int FrameBufferWidth, int FrameBufferHeight)
    {
        if(m_FrameBufferWidth!= FrameBufferWidth || m_FrameBufferHeight != FrameBufferHeight)
        {
            Destroy();
            Create(FrameBufferWidth,FrameBufferHeight,m_format,m_components,m_datatype);
        }
    }

    inline void Resize(int FrameBufferWidth, int FrameBufferHeight, int msaa)
    {
        if(m_FrameBufferWidth!= FrameBufferWidth || m_FrameBufferHeight != FrameBufferHeight)
        {
            Destroy();
            Create(FrameBufferWidth,FrameBufferHeight,m_format,m_components,m_datatype);
        }
    }

    inline void BlitToTexture()
    {
        if(m_Created)
        {

            /*glad_glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_MSAAId);
            glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_Id);
            glad_glBlitFramebuffer(0, 0, m_FrameBufferWidth, m_FrameBufferHeight,  // src rect
                0, 0, m_FrameBufferWidth, m_FrameBufferHeight,  // dst rect
                GL_COLOR_BUFFER_BIT, // buffer mask
                GL_LINEAR); // scale filter

            glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
        }
    }

    inline GLuint GetTexture()
    {
        return renderedTexture;
    }

    inline GLuint GetFrameBuffer()
    {

        return FramebufferName;

    }

    inline QImage * ToQImage()
    {
        if(m_Created)
        {

            glad_glBindTexture(GL_TEXTURE_2D,GetTexture());
            uchar * data = new uchar[3 * m_FrameBufferWidth * m_FrameBufferHeight * sizeof(uchar)];
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,data);
            glad_glBindTexture(GL_TEXTURE_2D,0);


            QImage * m = new QImage(QImage(data,m_FrameBufferWidth,m_FrameBufferHeight,QImage::Format_RGB888).mirrored());

            return m;
        }else {
            return new QImage();
        }




    }


    inline void Destroy()
    {

        if(m_Created)
        {
            glad_glDeleteRenderbuffers(1,&depthrenderbuffer);
            //glad_glDeleteRenderbuffers(1,&fb_MSAACId);
            //glad_glDeleteRenderbuffers(1,&fb_DId);
            glad_glDeleteFramebuffers(1,&FramebufferName);
            //glad_glDeleteFramebuffers(1,&fb_Id);
            glad_glDeleteTextures(1,&renderedTexture);
            m_Created = false;
        }


    }


};

#endif // OPENGLRENDERTARGET_H
