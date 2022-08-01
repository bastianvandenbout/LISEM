#ifndef OPENGLCLMANAGER_H
#define OPENGLCLMANAGER_H

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#include "defines.h"
#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <random>
#include <iostream>
#include <memory>
#include <functional>

#include <QApplication>
#include <QTimer>
#include <QMutex>
#include <QList>
#include <QWaitCondition>
#include <QPixmap>
#include "matrixtable.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

#include "openglprogram.h"
#include "openglcltexture.h"
#include "openglgeometry.h"
#include "openclprogram.h"
#include "error.h"
#include "openglshapepainter.h"
#include "opengltextpainter.h"
#include "opengltexturepainter.h"
#include "openglmsaarendertarget.h"
#include "openglrendertarget.h"
#include "opengl3dpainter.h"


//#include "scriptmanager.h"
//#include "lsmio.h"

//#include "widgets/layerinfowidget.h"
#include "gl3dgeometry.h"

#include "QMessageBox"


LISEM_API extern unsigned long NvOptimusEnablement;
LISEM_API extern int AmdPowerXpressRequestHighPerformance;




LISEM_API extern const char * shader_texcopy_frag;

LISEM_API extern const char * shader_texcopy_vert;


inline const char * gl_error_string(GLenum const err) noexcept
{
  switch (err)
  {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

    case GL_TABLE_TOO_LARGE:
      return "GL_TABLE_TOO_LARGE";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
      assert(!"unknown error");
      return nullptr;
  }
}


typedef std::vector<cl::Platform>::iterator PlatformIter;

static std::string bytesToStringRepr(size_t value)
{
    size_t b = value % 1024;
    value /= 1024;

    size_t kb = value % 1024;
    value /= 1024;

    size_t mb = value % 1024;
    value /= 1024;

    size_t gb = value;

    std::ostringstream stream;

    if (gb > 0)
        stream << gb << " GB ";
    if (mb > 0)
        stream << mb << " MB ";
    if (kb > 0)
        stream << kb << " KB ";
    if (b > 0)
        stream << b << " B";

    std::string s = stream.str();
    if (s[s.size() - 1] == ' ')
        s = s.substr(0, s.size() - 1);
    return s;
}

/*static QString getDeviceTypeString(const cl::Device& device)
{
    if (device.type() == cl::Device::TYPE_CPU) {
        return "CPU";
    }

    if (device.type() == cl::Device::TYPE_GPU) {
        if (device.hostUnifiedMemory()) {
            return "iGPU";
        } else {
            return "dGPU";
        }
    }

    return "unknown";
}*/

typedef struct {
    cl::Device d;
    cl::CommandQueue q;
    cl::Program p;
    cl::Kernel k;
    cl::Buffer i;
    cl::size_t<3> dims;
} process_params;

typedef struct {
    GLuint prg;
    GLuint vao;
    GLuint vbo;
    cl::BufferGL tmp;
} render_params;

static int const wind_width = 1600;
static int const wind_height= 800;

static const float matrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f};

static void glfw_error_callback(int error, const char* desc)
{
    std::cout << "glfw error callback " << error << desc << std::endl;
}

typedef struct UI_STATE
{
    float zoom;
    float Scroll;
    float Drag_x;
    float Drag_y;
    float Drag_zoomcomp_x;
    float Drag_zoomcomp_y;
    float Pos_x;
    float Pos_y;
    bool Dragging;
    int Width;
    int Height;
} UI_STATE;

static void glfw_mouse_scroll_callback(GLFWwindow* wind, double scrollx, double scrolly);
static void glfw_key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods);
static void glfw_mouse_callback(GLFWwindow* wind, int key, int action, int mods);
static void glfw_mouse_inout_callback(GLFWwindow* wind, int inout);
static void glfw_mouse_move_callback(GLFWwindow* wind, double x, double y);
static void glfw_framebuffer_size_callback(GLFWwindow* wind, int width, int height);
static void glfw_drop_callback(GLFWwindow* window, int count, const char** paths);
static void glfw_window_close_callback(GLFWwindow* window);
static void glfw_window_content_scale_callback(GLFWwindow* window, float xscale, float yscale);

class GLListener
{
public:
    virtual void OnMouseScroll(double,double) = 0;
    virtual void OnMouseMove(double,double) = 0;
    virtual void OnMouseInOut(bool) = 0;
    virtual void OnMouseKey(int,int,int) = 0;
    virtual void OnFrameBufferSize(int,int) = 0;
    virtual void OnKey(int,int,int) = 0;
    virtual void OnFileDrop(QString) = 0;
};


class OpenGLCLManager;

LISEM_API extern OpenGLCLManager * CGlobalGLCLManager;

class LISEM_API OpenGLCLManager : public QObject
{
    Q_OBJECT

public:
    OpenGLCLManager();

    cl::Context context;
    GLFWwindow* window;
    cl::CommandQueue q;
    bool m_HasOpenCL = false;
    cl::Device m_device;
    GLFWmonitor* monitor;

    QMutex m_GLMutex;

    std::array< int, 2 > _wndPos         {0, 0};
    std::array< int, 2 > _wndSize        {0, 0};
    std::array< int, 2 > _vpSize         {0, 0};

    bool is_fullscreen= false;

    QMutex m_GLRestartMutex;
    bool m_RequestGLRestart = false;

    QMutex m_GLOutputUIMutex;

    QList<GLListener*> m_ListenerList;
    QList<std::function<bool(void)>> m_CallBackFrameList;
    std::function<bool(void)> m_CallBackFrame;

    //QString m_KernelDir;
    //QString m_AssetDir;

    OpenGLProgram * m_GLProgram_CopyText;
    OpenGLGeometry * m_Quad;
    GL3DGeometry * m_GeometryPlane;



    OpenGLTextPainter * m_TextPainter;
    OpenGLShapePainter * m_ShapePainter;
    OpenGLTexturePainter * m_TexturePainter;
    OpenGL3DPainter * m_3DPainter;

    /*OpenGLProgram * m_GLProgram_uimap;
    OpenGLProgram * m_GLProgram_uiduomap;
    OpenGLProgram * m_GLProgram_uimapshape;
    OpenGLProgram * m_GLProgram_uimultimap;
    OpenGLProgram * m_GLProgram_uivectorl;
    OpenGLProgram * m_GLProgram_uivectorp;
    OpenGLProgram * m_GLProgram_uipointsimple;
    OpenGLProgram * m_GLProgram_particles;
    OpenGLProgram * m_GLProgram_pointcloud;
    OpenGLProgram * m_GLProgram_pointcloud3d;
    OpenGLProgram * m_GLProgram_uiterrain;
    OpenGLProgram * m_GLProgram_uiterrainlayer;
    OpenGLProgram * m_GLProgram_uiocean;
    OpenGLProgram * m_GLProgram_uiobject;
    OpenGLProgram * m_GLProgram_uiobjectinstanced;*/

    OpenGLCLMSAARenderTarget * m_ScreenTarget = 0;

    OpenGLCLMSAARenderTarget * m_MSAATarget = 0;

    int m_width = wind_width;
    int m_height = wind_height;
    bool m_DoFinalRender = true;

    UI_STATE GL_GLOBAL;


    int CreateGLWindow(QPixmap pixmap, bool visible = true);

    int ToggleFullScreen(bool switch_mon);

    inline void Restart()
    {
        m_GLRestartMutex.lock();
        m_RequestGLRestart = true;
        m_GLRestartMutex.unlock();
    }

    /*inline void SetScriptFunctions(ScriptManager * sm)
    {

        sm->m_Engine->RegisterGlobalFunction("void Restart()", asMETHOD(OpenGLCLManager, Restart), asCALL_THISCALL_ASGLOBAL, this);
    }*/


    void InitGLCL_int();
    inline int InitGLCL()
    {

        InitGLCL_int();

        return 0;

    }

    void LoadPreferredCLContext();


    inline void AddListener(GLListener * l)
    {
        m_ListenerList.append(l);
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackFrame(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackFrame = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackFrameList.append(m_CallBackFrame);
    }

    inline OpenGLCLMSAARenderTarget * GetFrameBuffer()
    {
        return m_MSAATarget;
    }
    inline void CreateMSAABuffer()
    {
        if(!m_ScreenTarget->IsCreated())
        {
            m_ScreenTarget->Resize(m_width,m_height,1);
        }
        //if(!m_MSAATarget->IsCreated())
        {
            if(!(m_MSAATarget->GetWidth() == m_width && m_MSAATarget->GetHeight() == m_height))
            {
                m_MSAATarget->Destroy();
                m_MSAATarget= new OpenGLCLMSAARenderTarget();
                m_MSAATarget->Resize(m_width,m_height,1);
            }
            //m_MSAATarget->Resize(m_width,m_height,1);
        }
    }

    void GLCLLoop();
    inline int CopyTextureToFrameBuffer(GLuint Source, GLuint Target)
    {


        return 0.0;
    }

    QMutex m_RequestFullScreenMutex;
    bool m_RequestFullScreen = false;
    bool m_RequestFullScreenOpen = false;
    bool m_RequestFullScreenAlt = false;

    inline void RequestToggleFullscreen(bool open, bool alt)
    {
        m_RequestFullScreenMutex.lock();
        m_RequestFullScreen = true;
        m_RequestFullScreenOpen = open;
        m_RequestFullScreenAlt = alt;
        m_RequestFullScreenMutex.unlock();

    }

    inline int CopyTextureUsingShaders(ModelTexture * m_Texture_source, OpenGLCLTexture * m_Texture_Target)
    {

        if(m_Texture_source->m_IsCPU)
        {
             CopyMapToTexture(m_Texture_Target,(m_Texture_source->m_TextureCPU));
        }else
        {
             CopyTextureUsingShaders(m_Texture_source->m_TextureGPU,m_Texture_Target);
        }

        return 0;

    }

    inline int CopyTextureUsingShaders(OpenGLCLTexture * m_Texture_source, OpenGLCLTexture * m_Texture_Target)
    {
        if(!(m_Texture_Target->m_IsFrambuffer))
        {
            return 1;
        }

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_Texture_Target->m_FramebufferName);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glad_glDrawBuffers(1, DrawBuffers);

        glad_glViewport(0,0,m_Texture_Target->m_dims[0],m_Texture_Target->m_dims[1]);

        glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glad_glClearColor(0.0,0.0,0.0,0.0);
        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_CopyText->m_program);
        // get uniform locations
        int mat_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"matrix");
        int tex_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"tex");
        // bind texture
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glUniform1i(tex_loc,0);
        glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->m_texgl);
        // set project matrix
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
        // now render stuff
        glad_glBindVertexArray(m_Quad->m_vao);
        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);

        return 0;

    }
    inline int CopyTextureToMap(ModelTexture * m_Texture_source, MaskedRaster<float> * m_Target)
    {
        if(m_Texture_source->m_IsCPU)
        {
            FOR_ROW_COL_RASTER(m_Target)
            {
                 (*m_Target)[r][c] = (*m_Texture_source->m_TextureCPU)[r][c];
            }
        }else
        {
            CopyTextureToMap(m_Texture_source->m_TextureGPU,m_Target);
        }
        return 0;
    }
    inline int CopyMapToTexture(ModelTexture * m_Texture_source, MaskedRaster<float> * m_Target)
    {
        if(m_Texture_source->m_IsCPU)
        {
            FOR_ROW_COL_RASTER(m_Target)
            {
                (*m_Texture_source->m_TextureCPU)[r][c] =(*m_Target)[r][c];
            }
        }else
        {
            CopyMapToTexture(m_Texture_source->m_TextureGPU,m_Target);
        }
        return 0;
    }

    inline int CopyMapToTexture(OpenGLCLTexture * m_Texture_source, MaskedRaster<float> * m_Target)
    {
        if(m_Target != 0 && m_Texture_source != 0)
        {
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->m_texgl);
            glad_glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_Target->nr_cols(),m_Target->nr_rows(),GL_RED,GL_FLOAT,(*m_Target)[0]);
        }
        return 0;
    }
    inline int CopyTextureToMap(GLuint m_Texture_source, MaskedRaster<float> * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source);
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,(*m_Target)[0]);
        }

        return 0;
    }

    inline int CopyTextureToMap(OpenGLCLTexture * m_Texture_source, MaskedRaster<float> * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->m_texgl);
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,(*m_Target)[0]);
        }

        return 0;
    }
    inline int CopyTextureToMap(OpenGLCLMSAARenderTarget* m_Texture_source, MaskedRaster<float> * m_Target)
    {

        if(m_Target != 0 && m_Texture_source != 0)
        {
            //void * data = new float(m_Texture_source->m_dims[0] * m_Texture_source->m_dims[1]);//m_Target->data[0];
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture_source->GetTexture());
            glad_glGetTexImage(GL_TEXTURE_2D,0,GL_RED,GL_FLOAT,(*m_Target)[0]);
        }

        return 0;
    }


    QList<OpenGLProgram*> m_GLProgramList;
    QList<OpenCLProgram*> m_CLProgramList;
    QList<OpenGLCLTexture*> m_GLCLTextureList;
    QList<OpenGLProgram*> m_GLProgramModelList;
    QList<OpenCLProgram*> m_CLProgramModelList;
    QList<OpenGLCLTexture*> m_GLCLTextureModelList;

    inline OpenGLProgram* ModelGetMGLProgram(QString file_vs, QString file_fs, bool add = true)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs);
        if(add)
        {
            m_GLProgramModelList.append(Program);
        }
        return Program;

    }

    inline OpenCLProgram* ModelGetMCLProgram(QString file, QString function, bool add = true)
    {
        OpenCLProgram* Program = new OpenCLProgram();
        Program->LoadProgram(this->context,this->m_device,file,function);
        if(add)
        {
            m_CLProgramModelList.append(Program);
        }
        return Program;
    }

    inline OpenGLCLTexture * ModelGetMCLGLTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add =  true )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureModelList.append(_T);
        }
        return _T;

    }

    inline OpenGLProgram* GetMGLProgram(QString file_vs, QString file_fs, bool add = false)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs);
        m_GLProgramList.append(Program);
        return Program;

    }

    inline OpenGLProgram* GetMGLProgram(QString file_vs, QString file_fs, QString file_gs, QString file_tcs, QString file_tes, bool add = false)
    {
        OpenGLProgram* Program = new OpenGLProgram();
        Program->LoadProgram(file_vs, file_fs, file_gs, file_tcs,file_tes);
        m_GLProgramList.append(Program);
        return Program;

    }

    inline OpenCLProgram* GetMCLProgram(QString file, QString function, bool add = false)
    {
        OpenCLProgram* Program = new OpenCLProgram();
        Program->LoadProgram(this->context,this->m_device,file,function);
        if(add)
        {
            m_CLProgramList.append(Program);
        }
        return Program;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add = false)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }

    inline OpenGLCLTexture * GetMGLTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,0.0,rendertarget,false);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32(context,width,height,0.0,rendertarget);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLLinearTexture(int width, int height, float value = 0.0, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32Linear(context,width,height,value,rendertarget);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }

    inline OpenGLCLTexture * GetMCLGLLinearTexture(int width, int height, float * data, bool rendertarget = false, bool add = false )
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DRF32Linear(context,width,height,0.0,rendertarget);
        _T->FillData2DRF32(data);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;

    }


    inline void DestroyMCLGLTexture(OpenGLCLTexture * t)
    {
        for(int i = m_GLCLTextureList.length()-1; i > -1; i--)
        {
            if(t ==m_GLCLTextureList.at(i))
            {
                m_GLCLTextureList.at(i)->Destroy();
                delete m_GLCLTextureList.at(i);
                m_GLCLTextureList.removeAt(i);
                break;
            }

        }
    }


    inline OpenGLCLTexture * ModelGetMCLGLTexture(MaskedRaster<float> * map, bool rendertarget =false, bool flipv = false, bool add = true)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DFromMap(context,map,rendertarget, flipv);
        if(add)
        {
            m_GLCLTextureModelList.append(_T);
        }
        return _T;
    }

    inline OpenGLCLTexture * GetMCLGLTexture(MaskedRaster<float> * map, bool rendertarget =false, bool flipv = false, bool add = false)
    {
        OpenGLCLTexture * _T = new OpenGLCLTexture();

        _T->Create2DFromMap(context,map,rendertarget, flipv);
        if(add)
        {
            m_GLCLTextureList.append(_T);
        }
        return _T;
    }

    void DeleteMGLCLModelObjects()
    {
        for(int i = m_GLProgramModelList.length()-1; i > -1; i--)
        {
            m_GLProgramModelList.at(i)->Destroy();
            delete m_GLProgramModelList.at(i);
        }
        m_GLProgramModelList.clear();

        for(int i = m_CLProgramModelList.length()-1; i > -1; i--)
        {
            m_CLProgramModelList.at(i)->Destroy();
            delete m_CLProgramModelList.at(i);
        }
        m_CLProgramModelList.clear();

        for(int i = m_GLCLTextureModelList.length()-1; i > -1; i--)
        {
            m_GLCLTextureModelList.at(i)->Destroy();
            delete m_GLCLTextureModelList.at(i);
        }
        m_GLCLTextureModelList.clear();
    }

    void DeleteMGLCLObjects()
    {
        for(int i = m_GLProgramList.length()-1; i > -1; i--)
        {
            m_GLProgramList.at(i)->Destroy();
            delete m_GLProgramList.at(i);
        }
        m_GLProgramList.clear();

        for(int i = m_CLProgramList.length()-1; i > -1; i--)
        {
            m_CLProgramList.at(i)->Destroy();
            delete m_CLProgramList.at(i);
        }
        m_CLProgramList.clear();

        for(int i = m_GLCLTextureList.length()-1; i > -1; i--)
        {
            m_GLCLTextureList.at(i)->Destroy();
            delete m_GLCLTextureList.at(i);
        }
        m_GLCLTextureList.clear();
    }

    void Destroy()
    {
        m_GLProgram_CopyText->Destroy();
        SAFE_DELETE(m_GLProgram_CopyText);

        m_Quad->Destroy();
        SAFE_DELETE(m_Quad);

        DeleteMGLCLModelObjects();
        DeleteMGLCLObjects();

        m_TexturePainter->Destroy();
        m_TextPainter->Destroy();
        m_ShapePainter->Destroy();
        m_3DPainter->Destroy();

        SAFE_DELETE(m_TexturePainter);
        SAFE_DELETE(m_TextPainter);
        SAFE_DELETE(m_ShapePainter);
        SAFE_DELETE(m_3DPainter);

        m_MSAATarget->Destroy();

        glfwDestroyWindow(window);


        glfwTerminate();

        m_CallBackFrameList.clear();

    }

    //let objects call a dialog
public:
    std::function<void()> m_dialog;
    int m_DialogReturn = 0;
    bool m_DoDialog = false;
    QMutex m_DialogMutex;
    QWaitCondition m_DialogWaitCondition;

    inline void DoDialog(std::function<void()> f)
    {
        m_dialog = f;

        m_DialogMutex.lock();

        m_DoDialog = true;

        m_DialogWaitCondition.wait(&m_DialogMutex);

        m_DialogMutex.unlock();
        return ;
    }

private:
    void DoQTLoopCallbackPrivate();
public slots:

    //this function has to be called from main thread
    inline void ProcessEvents()
    {
        if(!glfwWindowShouldClose(window))
        {


            bool dodialog = false;
            m_DialogMutex.lock();


            if(m_DoDialog)
            {
                dodialog = true;
                m_DoDialog = false;
            }

            m_DialogMutex.unlock();


            if(dodialog)
            {
                DoQTLoopCallbackPrivate();
            }

            QTimer::singleShot(30,this,&OpenGLCLManager::ProcessEvents);

        }
    }

};



inline static void glfw_mouse_scroll_callback(GLFWwindow* wind, double scrollx, double scrolly)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);

    if(man == NULL)
    {
        return;
    }
    man->GL_GLOBAL.Scroll = std::min(100000000.0,std::max(-10000000.0,man->GL_GLOBAL.Scroll +scrolly));

    man->GL_GLOBAL.zoom = std::min(10000000.0,std::max(0.00000001,man->GL_GLOBAL.zoom * std::pow(0.95,scrolly)));

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseScroll(scrollx,scrolly);
    }
}


inline static void glfw_key_callback(GLFWwindow* wind, int key, int scancode, int action, int mods)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        //glfwSetWindowShouldClose(wind, GL_TRUE);
    }
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        man->ToggleFullScreen(false);
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        man->ToggleFullScreen(true);
    }


    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnKey(key,action,mods);
    }
}

inline static void glfw_mouse_callback(GLFWwindow* wind, int key, int action, int mods)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if (key == GLFW_MOUSE_BUTTON_1  && action == GLFW_PRESS) {
        man->GL_GLOBAL.Dragging = true;
    }else if(key == GLFW_MOUSE_BUTTON_1  && action == GLFW_RELEASE) {
        man->GL_GLOBAL.Dragging = false;
    }
    if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_PRESS) {
    }

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseKey(key,action,mods);
    }
}

inline static void glfw_mouse_inout_callback(GLFWwindow* wind, int inout)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseInOut(inout? true:false);
    }

}

inline static void glfw_mouse_move_callback(GLFWwindow* wind, double x, double y)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    if(man->GL_GLOBAL.Dragging)
    {
        double dx = x - man->GL_GLOBAL.Pos_x;
        double dy = y - man->GL_GLOBAL.Pos_y;

        man->GL_GLOBAL.Drag_x += dx;
        man->GL_GLOBAL.Drag_y += dy;

        man->GL_GLOBAL.Drag_zoomcomp_x += man->GL_GLOBAL.zoom * dx;
        man->GL_GLOBAL.Drag_zoomcomp_y += man->GL_GLOBAL.zoom * dy;
    }
    man->GL_GLOBAL.Pos_x = x;
    man->GL_GLOBAL.Pos_y = y;

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnMouseMove(x,y);
    }
}

inline static void glfw_framebuffer_size_callback(GLFWwindow* wind, int width, int height)
{
    glad_glViewport(0,0,width,height);
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(wind);
    if(man == NULL)
    {
        return;
    }
    man->m_width = width;
    man->m_height = height;
    man->GL_GLOBAL.Width = width;
    man->GL_GLOBAL.Height = height;

    for(int i = 0; i < man->m_ListenerList.length(); i++)
    {
        man->m_ListenerList.at(i)->OnFrameBufferSize(width,height);
    }
}

inline static void glfw_drop_callback(GLFWwindow* window, int count, const char** paths)
{
    OpenGLCLManager * man = (OpenGLCLManager *)glfwGetWindowUserPointer(window);
    if(man == NULL)
    {
        return;
    }

    int i;
    for (i = 0;  i < count;  i++)
    {
        for(int i = 0; i < man->m_ListenerList.length(); i++)
        {
            man->m_ListenerList.at(i)->OnFileDrop(QString(paths[i]));
        }

    }

}

static void glfw_window_close_callback(GLFWwindow* window)
{



}
static void glfw_window_content_scale_callback(GLFWwindow* window, float xscale, float yscale)
{


}



#endif // OPENGLCLMANAGER_H
