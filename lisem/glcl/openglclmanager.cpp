#include "openglclmanager.h"

#include "lisem_glfw3native.h"

unsigned long NvOptimusEnablement = 0x00000001;
int AmdPowerXpressRequestHighPerformance = 1;

const char * shader_texcopy_frag = "#version 330 \n"
"uniform sampler2D tex; \n"
"in vec2 texcoord; \n"
"out vec4 fragColor; \n"
"void main() \n"
"{ \n"
"    fragColor = texture(tex,texcoord); \n"
"} \n";


const char * shader_texcopy_vert = "#version 330 \n"
"layout(location = 0) in vec3 pos; \n"
"layout(location = 1) in vec2 tex; \n"
"uniform mat4 matrix; \n"
"out vec2 texcoord; \n"
"void main() { \n"
"    texcoord = tex; \n"
"    gl_Position = matrix * vec4(pos,1.0); \n"
"} \n";



OpenGLCLManager * CGlobalGLCLManager;

OpenGLCLManager::OpenGLCLManager()
{



}
void OpenGLCLManager::DoQTLoopCallbackPrivate()
{

    m_DialogMutex.lock();
    m_DoDialog = false;


    {
        m_dialog();

    }

    m_DialogMutex.unlock();
    m_DialogWaitCondition.notify_all();

}


int OpenGLCLManager::CreateGLWindow(QPixmap pixmap, bool visible)
{

    GL_GLOBAL.zoom = 1.0;
    GL_GLOBAL.Drag_zoomcomp_x = 0.0;
    GL_GLOBAL.Drag_zoomcomp_y = 0.0;
    GL_GLOBAL.Scroll = 0;
    GL_GLOBAL.Drag_x = 0;
    GL_GLOBAL.Drag_y = 0;
    GL_GLOBAL.Pos_x = 0;
    GL_GLOBAL.Pos_y = 0;
    GL_GLOBAL.Dragging = false;
    GL_GLOBAL.Width = wind_width;
    GL_GLOBAL.Height = wind_height;

    //m_KernelDir = QCoreApplication::applicationDirPath() + LISEM_FOLDER_KERNELS;
    //m_AssetDir = QCoreApplication::applicationDirPath() + LISEM_FOLDER_ASSETS;
    LISEM_DEBUG("init glcl manager");
    if (!glfwInit())
    {
        LISEM_ERROR("glfwInit failed!");

        return 255;
    }

    glfwSetErrorCallback(glfw_error_callback);

    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

    if(!visible)
    {
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    }

    glfwWindowHint(GLFW_RED_BITS    , mode->redBits    );
    glfwWindowHint(GLFW_GREEN_BITS  , mode->greenBits  );
    glfwWindowHint(GLFW_BLUE_BITS   , mode->blueBits   );
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    LISEM_DEBUG("Creating Window");


    m_width = wind_width;
    m_height = wind_height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(wind_width,wind_height,"LISEM",NULL,NULL);
    if (!window) {


        glfwTerminate();
        return 254;
    }
    glfwSetWindowUserPointer(window, this);

    GLFWimage image;
    image.width = pixmap.width();
    image.height = pixmap.height();
    image.pixels = pixmap.toImage().bits();
    glfwSetWindowIcon(window,1,&image);

    return 0;
}



void OpenGLCLManager::InitGLCL_int()
{
    glfwMakeContextCurrent(window);

    int suc = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if(!suc) {
        LISEM_ERROR("gladLoadGL failed!");
        LISEM_ERROR("Error Code " + QString::number(suc));
        return;
    }


    m_GLProgramList.clear();
    m_CLProgramList.clear();
    m_GLCLTextureList.clear();

    std::cout << glad_glGetString << std::endl;


    const GLubyte* vendor = glad_glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glad_glGetString(GL_RENDERER);

    LISEM_STATUS(QString("Found OpenGL Device ") +(const char*)(vendor) + "  "  + (const char*)(renderer));

    LISEM_PRINT_DEBUG();

    glad_glEnable(GL_BLEND);
    glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glad_glEnable(GL_MULTISAMPLE);

    glfwSetErrorCallback(glfw_error_callback);
    glfwSetKeyCallback(window,glfw_key_callback);
    glfwSetMouseButtonCallback(window,glfw_mouse_callback);
    glfwSetCursorPosCallback(window,glfw_mouse_move_callback);
    glfwSetScrollCallback(window,glfw_mouse_scroll_callback);
    glfwSetCursorEnterCallback(window,glfw_mouse_inout_callback);

    glfwSetFramebufferSizeCallback(window,glfw_framebuffer_size_callback);
    glfwSetDropCallback(window,glfw_drop_callback);
    glfwSetWindowCloseCallback(window, glfw_window_close_callback);
    glfwSetWindowContentScaleCallback(window,glfw_window_content_scale_callback);

    LoadPreferredCLContext();

    //init basic shader programs

    m_GLProgram_CopyText = new OpenGLProgram();
    m_GLProgram_CopyText->LoadProgramFromStrings(shader_texcopy_vert,shader_texcopy_frag);

    m_Quad = new OpenGLGeometry();
    m_Quad->CreateUnitQuad();

    m_GeometryPlane = new GL3DGeometry();
    m_GeometryPlane->CreateFromRegularUnitGrid(25);

    m_TextPainter = new OpenGLTextPainter();
    m_TextPainter->LoadDefaultFontsAndShaders();

    m_ShapePainter = new OpenGLShapePainter();
    m_ShapePainter->LoadShaders();

    m_TexturePainter = new OpenGLTexturePainter();
    m_TexturePainter->LoadShaders();

    m_3DPainter = new OpenGL3DPainter();
    m_3DPainter->LoadShaders();

    m_ScreenTarget = new OpenGLCLMSAARenderTarget();
    m_MSAATarget = new OpenGLCLMSAARenderTarget();

    CGlobalGLCLManager = this;


}



void OpenGLCLManager::LoadPreferredCLContext()
{


    LISEM_DEBUG("Get Context");

    LISEM_STATUS(QString("OpenGL %1.%2\n").arg(GLVersion.major).arg(GLVersion.minor));

    LISEM_DEBUG("Get Preferred Platform setting");

    QString preferred = "";//GetSettingsManager()->GetSetting("PreferredGPUDevice");
    QString devname = "None found";

    LISEM_DEBUG("Get Platforms");

    float max_score = 0.0;

    cl::Platform okPlatform;

    bool is_okdeviceset = false;
    QList<cl::Device> ok_cldevices;
    QList<QString> ok_devices;
    QList<float> ok_devicescores;
    bool devinterop;
    bool devccgl;
    bool devccnogl;
    cl_int errCode;
    try {
        ListPlatforms();

        //get preferred platform from settings


        //now iterate over all platforms and devices and see what we can get
        cl_int error = 0;
        try {
            // Get available platforms
            std::vector<cl::Platform> platforms;
            cl::Platform::get(&platforms);

            for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {

                cl::Platform lPlatform = *it;

                std::string platformname = it->getInfo<CL_PLATFORM_NAME>();

                std::vector<cl::Device> devices;
                lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
                for (unsigned d=0; d<devices.size(); ++d) {

                    QString devicename = QString(devices[d].getInfo<CL_DEVICE_NAME>().c_str());

                    {
                        int computeunits = devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
                        int computeclock = devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
                        int computepars = devices[d].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
                        int computetype = devices[d].getInfo<CL_DEVICE_TYPE>();
                        QString clversion = QString(devices[d].getInfo<CL_DEVICE_VERSION>().c_str());
                        unsigned long long computemem = devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
                        QString clcversion = QString(devices[d].getInfo<CL_DEVICE_OPENCL_C_VERSION>().c_str());

                        size_t maxwidth = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH>();
                        size_t maxheight = devices[d].getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT>();
                        int unified = devices[d].getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>();

                        const char *typestr;

                        switch(computetype) {
                        case CL_DEVICE_TYPE_CPU:
                            typestr = "CPU";
                            break;
                        case CL_DEVICE_TYPE_ACCELERATOR:
                            typestr = "accelerator";
                            break;
                        case CL_DEVICE_TYPE_GPU:
                            typestr = "GPU";
                            break;
                        default:
                            typestr = "<unknown>";
                        }
                        bool interop = checkExtnAvailability(devices[d],CL_GL_SHARING_EXT);

                        //if(interop)
                        {
                            bool is_preferred = false;
                            if((QString(platformname.c_str()) + "  " + devicename).compare(preferred) == 0)
                            {
                                is_preferred = true;
                                LISEM_DEBUG("Found Preferred Device " + QString(platformname.c_str()) + "  " + devicename);
                            }

                            ok_cldevices.append(devices[d]);
                            ok_devices.append(QString(platformname.c_str()) + "  " + devicename);

                            float score =(computetype == CL_DEVICE_TYPE_GPU? 25.0:0.0);
                            score += (QString(platformname.c_str()).startsWith("NVIDIA"))? 5 : 0.0;
                            score += (QString(platformname.c_str()).compare("AMD") == 0)? 3 : 0.0;
                            score -= unified * 15.0; //penalize in case of shared memory, probably intel igpu, which generally suck (their new brand of xe gpu maybe not?)
                            score += (is_preferred? 1000.0:0.0);
                            score += interop * 10.0;


                            ok_devicescores.append(score);

                            bool can_create = true;
                            bool can_create_nogl = true;
                            try
                            {
                                LISEM_DEBUG("Test CL Context "  + QString(platformname.c_str()) + "  " + devicename);

                                #ifdef OS_LNX
                                        cl_context_properties cps[] = {
                                            CL_GL_CONTEXT_KHR, (cl_context_properties)lisem_glfwGetGLXContext(window),
                                            CL_GLX_DISPLAY_KHR, (cl_context_properties)lisem_glfwGetX11Display(),
                                            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
                                            0
                                        };
                                #endif
                                #ifdef OS_WIN
                                        cl_context_properties cps[] = {
                                            CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
                                            CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
                                            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
                                            0
                                        };
                                #endif
                                devices[d];
                                cl::Context context_tmp = cl::Context(devices[d], cps);
                            } catch(cl::Error error)
                            {
                                can_create = false;
                            } catch(...)
                            {
                                can_create = false;
                            }

                            if(can_create)
                            {
                                LISEM_DEBUG("Platform Works");
                            }else
                            {
                                LISEM_DEBUG("Platform Doesnt work with default settings");

                                #ifdef OS_LNX
                                        cl_context_properties cps[] = {

                                            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
                                            0
                                        };
                                #endif
                                #ifdef OS_WIN
                                        cl_context_properties cps[] = {
                                            CL_CONTEXT_PLATFORM, (cl_context_properties)lPlatform(),
                                            0
                                        };
                                #endif

                                try
                                {
                                   cl::Context context_tmp = cl::Context(devices[d], cps);

                                } catch(cl::Error error)
                                {
                                    can_create_nogl = false;
                                } catch(...)
                                {
                                   can_create_nogl = false;
                                }
                                if(can_create_nogl)
                                {
                                    LISEM_DEBUG("Platform Works without OpenGL interop");
                                    if(interop)
                                    {
                                        LISEM_DEBUG("Try starting the software with the dedicated GPU enabled");
                                    }
                                }else
                                {
                                    LISEM_DEBUG("Platform doesnt work with minimal settings");
                                }
                            }


                            if(score > max_score && (can_create || can_create_nogl))
                            {
                                std::cout << "set best platform " <<  " " << platformname.c_str() << " " << devicename.toStdString() << " " << devices.size() << " " << d << " " << std::endl;
                                max_score= score;
                                m_device = devices[d];
                                is_okdeviceset = true;
                                okPlatform = lPlatform;
                                devname =QString(platformname.c_str()) + "  " + devicename;
                                devinterop = interop;
                                devccgl = can_create;
                                devccnogl= can_create_nogl;
                            }


                        }/*else
                        {
                            if((QString(platformname.c_str()) + "  " + devicename).compare(preferred) == 0)
                            {
                                LISEM_DEBUG("Preferred device does not have gl/cl interoperability");

                            }
                        }*/

                        break;
                    }


                }
            }
        } catch(cl::Error err) {
        }


        if(is_okdeviceset)
        {
            LISEM_DEBUG("Platform Found");
        }else
        {
            LISEM_ERROR("Can not found suitible platform for OpenGL/OpenCL interoperability");
            throw 1;
        }




        LISEM_DEBUG("Create CL Context " + devname);

        if(devccgl)
        {
            // Select the default platform and create a context using this platform and the GPU
            #ifdef OS_LNX
                    cl_context_properties cps[] = {
                        CL_GL_CONTEXT_KHR, (cl_context_properties)lisem_glfwGetGLXContext(window),
                        CL_GLX_DISPLAY_KHR, (cl_context_properties)lisem_glfwGetX11Display(),
                        CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                        0
                    };
            #endif
            #ifdef OS_WIN
                    cl_context_properties cps[] = {
                        CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
                        CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
                        CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                        0
                    };
            #endif

            context = cl::Context(m_device, cps);
        }else if(devccnogl)
        {
            #ifdef OS_LNX
                    cl_context_properties cps[] = {

                        CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                        0
                    };
            #endif
            #ifdef OS_WIN
                    cl_context_properties cps[] = {
                        CL_CONTEXT_PLATFORM, (cl_context_properties)okPlatform(),
                        0
                    };
            #endif


            context = cl::Context(m_device, cps);
        }



        // Create a command queue and use the first device
        q = cl::CommandQueue(context, m_device);

        std::cout << "Sucesfully created OpenCL Context" << std::endl;
        LISEM_DEBUG("Sucesfully created OpenCL Context");
        //CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR for gl/cl interop

        m_HasOpenCL = true;
    } catch(cl::Error error) {
        std::cout << error.what() << "(" << error.err() << ")" << std::endl;
        std::string val = cl::Program().getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);
        std::cout<<"Log:\n"<<val<<std::endl;
        std::cout << "Could not create OpenCL Context" << std::endl;
        LISEM_DEBUG("Could not create OpenCL Context");
        LISEM_DEBUG(QString(error.what()) + "(" + error.err() +")" );
        m_HasOpenCL = false;
    } catch(...)
    {
        LISEM_DEBUG("Could not create OpenCL Context (unknown error)");
        m_HasOpenCL = false;
    }


    LISEM_PRINT_DEBUG();

}



void OpenGLCLManager::GLCLLoop()
{

    LISEM_STATUS("Done initializing OpenGL and OpenCL, Starting Loop..");

    bool restart = false;


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();


        m_GLOutputUIMutex.lock();
        m_GLMutex.lock();

        //is there a call to recreate the window using a different context?
        //we can only do this if the model is not running, since its resources will be invalidated
        //this is guaranteed by the user interface
        //if a restart is requested, we throw a specific error code through the qt app to the sphazard execute function

        m_GLRestartMutex.lock();
        if(m_RequestGLRestart)
        {
            m_RequestGLRestart = false;
            glfwSetWindowShouldClose(window,GL_TRUE);
            restart = true;
        }
        m_GLRestartMutex.unlock();

        //make this thread current for opengl
        glfwMakeContextCurrent(window);

        int widtht = 0;
        int heightt = 0;
        glfwGetWindowSize(window,&widtht, &heightt);

        std::cout << "wh " << widtht << " " << heightt << " " << m_width << " " << m_height << std::endl;
        m_width = widtht;
        m_height = heightt;

        CreateMSAABuffer();

        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_MSAATarget->GetFrameBuffer());

        glad_glClearColor(0.961,0.963,0.966,1.0);
        glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glad_glDisable(GL_DEPTH_TEST);

        m_TexturePainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);
        m_TextPainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);
        m_ShapePainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);
        m_3DPainter->UpdateRenderTargetProperties(m_MSAATarget->GetFrameBuffer(),GL_GLOBAL.Width,GL_GLOBAL.Height);


        for(int i = 0; i < m_CallBackFrameList.length(); i++)
        {
            m_CallBackFrameList.at(i)();
        }


        // render msaa  render buffer to normal texture

        m_MSAATarget->BlitToTexture();


        //render texture to screen
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glad_glDrawBuffers(1, DrawBuffers);

        glad_glViewport(0,0,m_width,m_height);

        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_CopyText->m_program);
        // get uniform locations
        int mat_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"matrix");
        int tex_loc = glad_glGetUniformLocation(m_GLProgram_CopyText->m_program,"tex");
        // bind texture
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glUniform1i(tex_loc,0);
        glad_glBindTexture(GL_TEXTURE_2D,m_MSAATarget->GetTexture());
        // set project matrix
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
        // now render stuff
        glad_glBindVertexArray(m_Quad->m_vao);
        glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);


        //glad_glClearColor(0.961,0.963,0.966,1.0);
        //glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //swap opengl front and back buffer
        glfwSwapBuffers(window);

        //reset the current thread for opengl
        glfwMakeContextCurrent(NULL);


        //load and output all the errors that have occured in the past cycle.
        //GLenum err;
        //while((err = glGetError()) != GL_NO_ERROR)
        //{

        //    std::cout << "OpenGL Error " << err  << " " <<  gl_error_string(err) <<  std::endl;
        //    LISEM_ERROR("OpenGL Error " +QString::number(err) + gl_error_string(err));
        //}


        m_GLMutex.unlock();
        m_GLOutputUIMutex.unlock();

        if(restart)
        {
            break;
        }
    }


    if(restart)
    {

        QCoreApplication::exit(LISEM_CODE_RETURN_RESTART);
    }else
    {
        //now, since we quit this display, we also quit the other qt window

        QCoreApplication::exit(LISEM_CODE_RETURN_NORMAL);

    }




}
