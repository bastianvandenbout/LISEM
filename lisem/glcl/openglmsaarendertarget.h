#ifndef OPENGLMSAARENDERTARGET_H
#define OPENGLMSAARENDERTARGET_H

#include "glad/glad.h"
#include "QImage"
#include "iostream"

class OpenGLCLMSAARenderTarget
{
private:

    int m_FrameBufferWidth = 0;
    int m_FrameBufferHeight = 0;
    int m_MSAA = 1;
    bool m_Created = false;
    bool do_msaa = false;
    int m_nlayersrgba8 = 0;
    int m_nlayersr32 = 0;

    QList<GLuint> fb_TextureID;
    GLuint fb_MSAAId;
    QList<GLuint> fb_Id;
    GLuint fb_DId;
    QList<GLuint> fb_MSAACId;
    GLuint fb_MSAADId;

    GLenum m_format;
    GLenum m_tformat;
    GLenum m_tcomponents;
    GLenum m_tdatatype;

public:

    inline OpenGLCLMSAARenderTarget()
    {
        m_format = GL_RGB8;
        m_tformat = GL_RGBA8;
        m_tcomponents = GL_RGBA;
        m_tdatatype = GL_UNSIGNED_BYTE;

    }

    inline int GetWidth()
    {
        return m_FrameBufferWidth;
    }
    inline int GetHeight()
    {
        return m_FrameBufferHeight;
    }

    inline void SetDrawBuffers()
    {



    }

    inline void Create(int width, int height, int msaa_level, GLenum format, GLenum tformat,GLenum tcomp,GLenum tdatt, int nlayersrgba8 = 0,int nlayersr32 = 0)
    {
        m_FrameBufferWidth = width;
        m_FrameBufferHeight = height;

        m_format = format;
        m_tformat = tformat;
        m_tcomponents = tcomp;
        m_tdatatype = tdatt;
        m_nlayersrgba8 = nlayersrgba8;
        m_nlayersr32 = nlayersr32;
        m_MSAA = msaa_level;


        int MSAA_level = msaa_level;
        do_msaa = msaa_level > 1? true:false;


        // create a MSAA framebuffer object
        // NOTE: All attachment images must have the same # of samples.
        // Ohterwise, the framebuffer status will not be completed.
        glad_glGenFramebuffers(1, &fb_MSAAId);
        glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_MSAAId);


        for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32 +1; i++)
        {
            GLenum i_format;
            GLenum i_tformat;
            GLenum i_tcomp;
            GLenum i_tdatt;

            if(i == 0)
            {
                i_format = format;
                i_tformat = tformat;
                i_tcomp = tcomp;
                i_tdatt = tdatt;

            }else if( i < m_nlayersrgba8 + 1)
            {
                i_format = GL_RGB8;
                i_tformat = GL_RGBA8;
                i_tcomp = GL_RGBA;
                i_tdatt = GL_UNSIGNED_BYTE;
            }else {
                i_format = GL_R32F;
                i_tformat = GL_R32F;
                i_tcomp = GL_RED;
                i_tdatt = GL_FLOAT;
            }

            GLuint i_fb_TextureID;

            m_MSAA = msaa_level;
            glad_glGenTextures(1, &i_fb_TextureID);
            glad_glBindTexture(GL_TEXTURE_2D, i_fb_TextureID);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glad_glTexImage2D(GL_TEXTURE_2D, 0, i_tformat, m_FrameBufferWidth, m_FrameBufferHeight, 0, i_tcomp, i_tdatt, 0);
            glad_glBindTexture(GL_TEXTURE_2D, 0);


            GLuint i_fb_MSAACId;
            GLuint i_fb_Id;

            // create a MSAA renderbuffer object to store color info
            if(do_msaa)
            {
                glad_glGenRenderbuffers(1, &i_fb_MSAACId);
                glad_glBindRenderbuffer(GL_RENDERBUFFER, i_fb_MSAACId);
                glad_glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_level, i_format, m_FrameBufferWidth,  m_FrameBufferHeight);
                glad_glBindRenderbuffer(GL_RENDERBUFFER, 0);


                glad_glGenFramebuffers(1, &i_fb_Id);
                glad_glBindFramebuffer(GL_FRAMEBUFFER, i_fb_Id);
                glad_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, i_fb_TextureID, 0);


                glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_MSAAId);

            }

            fb_Id.append(i_fb_Id);
            fb_TextureID.append(i_fb_TextureID);
            fb_MSAACId.append(i_fb_MSAACId);
        }


        if(do_msaa)
        {

           for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
           {
               GLenum colattach = static_cast<GLenum>(static_cast<int>(GL_COLOR_ATTACHMENT0)+i);
               // attach msaa RBOs to FBO attachment points
               glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, colattach, GL_RENDERBUFFER, fb_MSAACId.at(i));
           }

        }else {


            for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
            {
                GLenum colattach = static_cast<GLenum>(static_cast<int>(GL_COLOR_ATTACHMENT0)+i);

                // attach a texture to FBO color attachement point
                glad_glFramebufferTexture2D(GL_FRAMEBUFFER, colattach, GL_TEXTURE_2D, fb_TextureID.at(i), 0);
            }


        }

        // create a MSAA renderbuffer object to store depth info
        // NOTE: A depth renderable image should be attached the FBO for depth test.
        // If we don't attach a depth renderable image to the FBO, then
        // the rendering output will be corrupted because of missing depth test.
        // If you also need stencil test for your rendering, then you must
        // attach additional image to the stencil attachement point, too.


        glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_MSAAId);

        if(do_msaa)
        {
            glad_glGenRenderbuffers(1, &fb_MSAADId);
            glad_glBindRenderbuffer(GL_RENDERBUFFER, fb_MSAADId);
            glad_glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_level, GL_DEPTH_COMPONENT, m_FrameBufferWidth,  m_FrameBufferHeight);
            glad_glBindRenderbuffer(GL_RENDERBUFFER, 0);


            glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb_MSAADId);
        }else {

            glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_MSAAId);
            glad_glGenRenderbuffers(1, &fb_DId);
            glad_glBindRenderbuffer(GL_RENDERBUFFER, fb_DId);
            glad_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_FrameBufferWidth, m_FrameBufferHeight);
            glad_glBindRenderbuffer(GL_RENDERBUFFER, 0);


            // attach a rbo to FBO depth attachement point
            glad_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb_DId);

        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_MSAAId);
        if(glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "!!!!!!!!!!!!!! ERROR " << std::endl;
        }else {
            std::cout << "success with framebuffer" << std::endl;
        }


        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);



        m_Created = true;
    }

    inline void CreateR32(int width, int height,int nlayersrgb8 = 0,int nlayersr32 =0)
    {
        Create(width,height,1,GL_R32F,GL_R32F,GL_RED,GL_FLOAT,nlayersrgb8,nlayersr32);


    }
    inline void CreateRGBA8(int width, int height,int nlayersrgb8 = 0,int nlayersr32 = 0)
    {

        Create(width,height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,nlayersrgb8,nlayersr32);
    }


    inline void Resize(int FrameBufferWidth, int FrameBufferHeight)
    {
        if(m_FrameBufferWidth!= FrameBufferWidth || m_FrameBufferHeight != FrameBufferHeight)
        {
            Destroy();
            Create(FrameBufferWidth,FrameBufferHeight,m_MSAA,m_format,m_tformat,m_tcomponents,m_tdatatype, m_nlayersrgba8,m_nlayersr32);
        }
    }

    inline void Resize(int FrameBufferWidth, int FrameBufferHeight, int msaa)
    {
        if(m_FrameBufferWidth!= FrameBufferWidth || m_FrameBufferHeight != FrameBufferHeight)
        {
            std::cout << "create msaa buffer "  <<  FrameBufferWidth << "  "  <<FrameBufferHeight<< std::endl;

            Destroy();
            Create(FrameBufferWidth,FrameBufferHeight,msaa,m_format,m_tformat,m_tcomponents,m_tdatatype, m_nlayersrgba8,m_nlayersr32);
        }
    }

    inline void BlitToTexture()
    {
        if(m_Created)
        {

            if(do_msaa)
            {
                for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
                {
                    glad_glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_MSAAId);
                    glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_Id.at(i));

                    GLenum colattach = static_cast<GLenum>(static_cast<int>(GL_COLOR_ATTACHMENT0)+i);
                    glad_glReadBuffer(colattach);
                    glad_glDrawBuffer(GL_COLOR_ATTACHMENT0);

                    glad_glBlitFramebuffer(0, 0, m_FrameBufferWidth, m_FrameBufferHeight,  // src rect
                        0, 0, m_FrameBufferWidth, m_FrameBufferHeight,  // dst rect
                        GL_COLOR_BUFFER_BIT, // buffer mask
                        GL_LINEAR); // scale filter

                    glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }
            }
        }
    }

    inline void SetAsTarget()
    {


        std::vector<GLuint> attachments = std::vector<GLuint>(m_nlayersrgba8 + m_nlayersr32+1);
        for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
        {
            GLenum colattach = static_cast<GLenum>(static_cast<int>(GL_COLOR_ATTACHMENT0)+i);
            attachments[i] = colattach;
        }
        glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_MSAAId);
        glad_glDrawBuffers(m_nlayersrgba8 + m_nlayersr32+1,attachments.data());
    }

    inline void SetDefaultTarget()
    {
        glad_glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline GLuint GetTexture(int i)
    {
        if(i < fb_TextureID.length())
        {
            return fb_TextureID.at(i);
        }else {
            return 0;
        }
        return 0;
    }

    inline GLuint GetTexture()
    {
        return fb_TextureID.at(0);

    }

    inline bool IsCreated()
    {
        return m_Created;
    }

    inline GLuint GetFrameBuffer(int i)
    {

        return fb_Id.at(i);

    }

    inline GLuint GetFrameBuffer()
    {

        return fb_MSAAId;

    }

    inline void ClearAll()
    {
        GLint drawFboId = 0, readFboId = 0;
        glad_glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
        glad_glGetIntegerv(GL_FRAMEBUFFER_BINDING, &readFboId);

        std::vector<GLuint> attachments = std::vector<GLuint>(m_nlayersrgba8 + m_nlayersr32+1);
        for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
        {
            GLenum colattach = static_cast<GLenum>(static_cast<int>(GL_COLOR_ATTACHMENT0)+i);
            attachments[i] = colattach;
        }
        glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_MSAAId);
        glad_glDrawBuffers(m_nlayersrgba8 + m_nlayersr32+1,attachments.data());

        for(int i = 0; i < m_nlayersrgba8 + m_nlayersr32+1; i++)
        {
            if(i == 0)
            {
                glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glad_glDisable(GL_DEPTH_TEST);
                glad_glClearColor(0.0,0.0,0.0,0.0);

            }else if(i < m_nlayersrgba8+1)
            {
                glad_glBindFramebuffer(GL_FRAMEBUFFER, fb_Id.at(i-1));
                glad_glClear(GL_COLOR_BUFFER_BIT);
                glad_glDisable(GL_DEPTH_TEST);
                glad_glClearColor(0.0,0.0,0.0,0.0);

            }else {
                static GLfloat mv[] = {-1e30f, 0.0f, 0.0f, 1.0f};
                glad_glClearBufferfv(GL_COLOR, i, mv);
            }

        }

        glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER,drawFboId);
        glad_glBindFramebuffer(GL_READ_FRAMEBUFFER,readFboId);



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

    inline void ToUCharBuffer(uchar * data)
    {
        if(m_Created)
        {

            glad_glBindTexture(GL_TEXTURE_2D,GetTexture());
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,data);
            glad_glBindTexture(GL_TEXTURE_2D,0);

        }else {

        }




    }


    inline uchar * ToUChar()
    {
        if(m_Created)
        {

            glad_glBindTexture(GL_TEXTURE_2D,GetTexture());
            uchar * data = new uchar[3 * m_FrameBufferWidth * m_FrameBufferHeight * sizeof(uchar)];
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,data);
            glad_glBindTexture(GL_TEXTURE_2D,0);

            return data;
        }else {

            uchar * data = new uchar[3 * m_FrameBufferWidth * m_FrameBufferHeight * sizeof(uchar)];
            return data;
        }




    }



    inline void Destroy()
    {

        if(m_Created)
        {
            glad_glDeleteRenderbuffers(1,&fb_MSAADId);
            for(int i = 0; i < fb_MSAACId.length(); i++)
            {
                glad_glDeleteRenderbuffers(1,&fb_MSAACId.at(i));
            }
            glad_glDeleteRenderbuffers(1,&fb_DId);
            glad_glDeleteFramebuffers(1,&fb_MSAAId);
            for(int i = 0; i < fb_Id.length(); i++)
            {
                glad_glDeleteFramebuffers(1,&fb_Id.at(i));
            }
            for(int i = 0; i < fb_TextureID.length(); i++)
            {
                glad_glDeleteTextures(1,&fb_TextureID.at(i));
            }
            m_Created = false;
        }


    }


};

#endif // OPENGLMSAARENDERTARGET_H
