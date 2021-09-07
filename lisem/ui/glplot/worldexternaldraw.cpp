#include "worldwindow.h"

QImage *WorldWindow::DoExternalDraw(GeoWindowState s)
{

    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
    m_OpenGLCLManager->m_GLMutex.lock();

    m_CurrentDrawWindowState = s;


    glfwMakeContextCurrent(m_OpenGLCLManager->window);


    if(ExternalTarget == nullptr)
    {
            ExternalTarget = new OpenGLCLMSAARenderTarget();
            ExternalTarget->Create(s.scr_pixwidth,s.scr_pixheight,1,GL_RGB8,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE);

            ExternalPost1RenderTarget = new OpenGLCLMSAARenderTarget();
            ExternalPost2RenderTarget = new OpenGLCLMSAARenderTarget();
            ExternalPost1RenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,0);
            ExternalPost2RenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,0);
    }else
    {
        ExternalTarget->Resize(s.scr_pixwidth,s.scr_pixheight);

        ExternalPost1RenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
        ExternalPost2RenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
    }



    m_CurrentDrawWindowState.GL_FrameBuffer = ExternalTarget;
    m_CurrentDrawWindowState.GL_PrimaryFrameBuffer = ExternalTarget;
    m_CurrentDrawWindowState.GL_PostProcessBuffer1 = ExternalPost1RenderTarget;
    m_CurrentDrawWindowState.GL_PostProcessBuffer2 = ExternalPost2RenderTarget;

    if(s.is_3d)
    {

        if(External3DRenderTarget == nullptr)
        {
            External3DRenderTarget = new OpenGLCLMSAARenderTarget();
            External3DRenderTarget->Create(s.scr_pixwidth,s.scr_pixheight,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,6);


        }else
        {
            //this function already checks if it needs to recreate object
            External3DRenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
        }

        std::cout << "external 3d draw " << External3DRenderTarget->GetFrameBuffer() << std::endl;

        m_CurrentDrawWindowState.GL_FrameBuffer = ExternalTarget;
        m_CurrentDrawWindowState.GL_3DFrameBuffer = External3DRenderTarget;

        this->DrawToFrameBuffer3D(m_CurrentDrawWindowState, true);

    }else
    {
        this->DrawToFrameBuffer2D(m_CurrentDrawWindowState);
    }



    QImage * Im;

    //if(!s.is_3d)
    //{
        ExternalTarget->BlitToTexture();
        Im = ExternalTarget->ToQImage();
    //}else
    //{
    //    External3DRenderTarget->BlitToTexture();
    //    Im = External3DRenderTarget->ToQImage();
    //}

    glfwMakeContextCurrent(NULL);

    m_OpenGLCLManager->m_GLMutex.unlock();
    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

    return Im;
}



uchar *WorldWindow::DoExternalDrawToUChar(GeoWindowState s)
{

    std::cout << "external draw start " <<  s.scr_pixwidth << "  " << s.scr_pixheight <<std::endl;


    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
    m_OpenGLCLManager->m_GLMutex.lock();

    m_CurrentDrawWindowState = s;
    s.flip = true;


    glfwMakeContextCurrent(m_OpenGLCLManager->window);


    if(ExternalTarget == nullptr)
    {
            ExternalTarget = new OpenGLCLMSAARenderTarget();
            ExternalTarget->Create(s.scr_pixwidth,s.scr_pixheight,1,GL_RGB8,GL_RGBA8,GL_RGBA,GL_UNSIGNED_BYTE);

            ExternalPost1RenderTarget = new OpenGLCLMSAARenderTarget();
            ExternalPost2RenderTarget = new OpenGLCLMSAARenderTarget();
            ExternalPost1RenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,0);
            ExternalPost2RenderTarget->Create(m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,0);
    }else
    {
        ExternalTarget->Resize(s.scr_pixwidth,s.scr_pixheight);

        ExternalPost1RenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
        ExternalPost2RenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
    }



    m_CurrentDrawWindowState.GL_FrameBuffer = ExternalTarget;
    m_CurrentDrawWindowState.GL_PrimaryFrameBuffer = ExternalTarget;
    m_CurrentDrawWindowState.GL_PostProcessBuffer1 = ExternalPost1RenderTarget;
    m_CurrentDrawWindowState.GL_PostProcessBuffer2 = ExternalPost2RenderTarget;

    if(s.is_3d)
    {

        if(External3DRenderTarget == nullptr)
        {
            External3DRenderTarget = new OpenGLCLMSAARenderTarget();
            External3DRenderTarget->Create(s.scr_pixwidth,s.scr_pixheight,1,GL_RGB8,GL_RGBA8,GL_RGBA, GL_UNSIGNED_BYTE,0,6);


        }else
        {
            //this function already checks if it needs to recreate object
            External3DRenderTarget->Resize(s.scr_pixwidth,s.scr_pixheight);
        }

        std::cout << "external 3d draw " << External3DRenderTarget->GetFrameBuffer() << std::endl;

        m_CurrentDrawWindowState.GL_3DFrameBuffer = External3DRenderTarget;

        this->DrawToFrameBuffer3D(m_CurrentDrawWindowState, true);

    }else
    {
        std::cout << "external 2d draw" <<std::endl;
        this->DrawToFrameBuffer2D(m_CurrentDrawWindowState);
    }


    ExternalTarget->BlitToTexture();

    int size = 3 * ExternalTarget->GetWidth() * ExternalTarget->GetHeight();
    if(m_InternalCharBufferSize < size || m_InternalCharBuffer  == nullptr)
    {
        if(!(m_InternalCharBuffer == nullptr))
        {
            delete[] m_InternalCharBuffer;
        }
        m_InternalCharBuffer = new uchar[3 * ExternalTarget->GetWidth() *ExternalTarget->GetHeight()];
    }
    ExternalTarget->ToUCharBuffer(m_InternalCharBuffer);

    uchar * data = new uchar[3 * ExternalTarget->GetWidth() * ExternalTarget->GetHeight()];

    for(int r = 0; r < ExternalTarget->GetHeight(); r++)
    {
        for(int c = 0; c < ExternalTarget->GetWidth(); c++)
        {
            data[3 * ((ExternalTarget->GetHeight()-1-r)*ExternalTarget->GetWidth() + c) + 0] = m_InternalCharBuffer[3*((r)*ExternalTarget->GetWidth() + c) + 0];
            data[3 * ((ExternalTarget->GetHeight()-1-r)*ExternalTarget->GetWidth() + c) + 1] = m_InternalCharBuffer[3*((r)*ExternalTarget->GetWidth() + c) + 1];
            data[3 * ((ExternalTarget->GetHeight()-1-r)*ExternalTarget->GetWidth() + c) + 2] = m_InternalCharBuffer[3*((r)*ExternalTarget->GetWidth() + c) + 2];

        }
    }

    glfwMakeContextCurrent(NULL);

    m_OpenGLCLManager->m_GLMutex.unlock();
    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

    return data;
}
