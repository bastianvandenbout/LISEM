#ifndef UISHADERLAYER_H
#define UISHADERLAYER_H


#include "layer/uilayer.h"
#include "openglclmanager.h"
#include "qcolor.h"
#include "model.h"
#include "geo/raster/map.h"
#include "openglcltexture.h"
#include "openglprogram.h"
#include "lsmio.h"
#include "lisemmath.h"
#include "QObject"
#include "openglcldatabuffer.h"
#include "color/colorf.h"
#include "boundingbox.h"
#include "linear/lsm_vector3.h"
#include "time.h"
#include "layer/editors/uishadereditor.h"
#include "gl/openglcldatamanager.h"
#include "linear/lsm_vector3.h"

class UIShaderLayerEditor;

class UIShaderLayer : public UIGeoLayer
{
private:

    bool m_createdTexture = false;
    bool m_createdShaders = false;
    bool m_createdRenderTargets = false;
    int m_RTSizeX = 512;
    int m_RTSizeY = 512;
    int m_Frame = -1;
    float m_TimeStart = 0.0;

    //can be customized by the user for input to the shaders
    std::vector<float> m_InputFloats = {0.0,0.0,0.0,0.0
                               ,0.0,0.0,0.0,0.0
                               ,0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0,
                               0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0
                             ,0.0,0.0,0.0,0.0};
    std::vector<float> m_InputFloats2 = {0.0,0.0,0.0,0.0
                               ,0.0,0.0,0.0,0.0
                               ,0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0,
                               0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0
                              ,0.0,0.0,0.0,0.0
                             ,0.0,0.0,0.0,0.0};

    //all float, just in case we need it, although slower...
    std::vector<OpenGLCLMSAARenderTarget *> m_Target;
    std::vector<OpenGLCLMSAARenderTarget *> m_TargetB;


    //how should this be drawn?
    bool m_AutoScale = false;
    bool m_Dynamic = false;

    bool m_Is2D;
    bool m_Is2DABS;
    bool m_Is3D;
    //these might be for 2D in crs coordinates, or 2d abs,which means screen relative coordinates
    BoundingBox m_BoundingBox;
    //the projection is only valid for m_Is2D is true and not absolut
    GeoProjection m_CRS;

    //these are for each buffer shader the textures (can be 4 textures for 1 image input)
    //when saving and restoring, we need to get the file names and band numbers from the rasters, otherwise we can not re-open it
    std::vector<std::vector<cTMap *>> textures;
    std::vector<std::vector<QString>> m_TextureLinks;

    //the output of each buffer is provided to all next ones
    std::vector<QString> m_Shaders;
    std::vector<OpenGLProgram*> m_Programs;

    std::vector<std::vector<OpenGLCLTexture*>> m_Textures;

    QMutex m_Mutex;
    double oldtlx = 0.0;
    double oldtly = 0.0;
    double oldbrx = 0.0;
    double oldbry = 0.0;

public:

    inline UIShaderLayer(std::vector<QString> buffershaders,std::vector<std::vector<cTMap *>> textures, ShapeFile * f,int resolutionx, int resolutiony, bool dynamic, bool scaling)
    {

    }

    inline UIShaderLayer(std::vector<QString> buffershaders,std::vector<std::vector<QString>> texture_files,std::vector<std::vector<cTMap *>> textures, BoundingBox bb,GeoProjection p,bool isabs, bool is3d,int resolutionx, int resolutiony, bool dynamic, bool scaling) : UIGeoLayer(p,bb,QString("Custom Shader"),false,QString(""),true)
    {
        m_BoundingBox = bb;
        UpdateRTSize(resolutionx,resolutiony);
        UpdateShader(buffershaders);
        UpdateImages(texture_files,textures);
        m_Dynamic = dynamic;
        m_AutoScale = scaling;
        m_Is2D = !is3d;
        m_Is2DABS = isabs;
        m_Is3D= is3d;
        m_CRS = p;

        m_Editable = true;
        m_IsDynamic = true;

    }

    inline ~UIShaderLayer()
    {

    }

    inline QString layertypeName()
    {
        return "ShaderLayer";
    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = true;
    }


    inline void setPosition(BoundingBox b, GeoProjection * p, bool is_rel)
    {
        m_BoundingBox = b;


        m_Is2DABS = is_rel;

        m_CRS = *p;



    }


    inline void SetAutoScale(bool x)
    {
        m_Mutex.lock();
        m_AutoScale = x;
        m_Mutex.unlock();
    }

    inline void SetInputFloats(int i, float a, float b, float c, float d)
    {
        m_Mutex.lock();

        if(i < 0)
        {

        }
        if(i < 8)
        {
            m_InputFloats.at(i*4 + 0) = a;
            m_InputFloats.at(i*4 + 1) = b;
            m_InputFloats.at(i*4 + 2) = c;
            m_InputFloats.at(i*4 + 3) = d;

        }
        if(i < 16)
        {
            m_InputFloats2.at((i-8)*4 + 0) = a;
            m_InputFloats2.at((i-8)*4 + 1) = b;
            m_InputFloats2.at((i-8)*4 + 2) = c;
            m_InputFloats2.at((i-8)*4 + 3) = d;

        }

        m_Mutex.unlock();


    }
    inline void SetInputImage(QString image, int in, int pass)
    {

        m_Mutex.lock();
        if(m_Shaders.size() > pass && in < 4)
        {
            //if needed, add empty lists to the TextureLinks array

            for(int  i = m_TextureLinks.size(); i < m_Shaders.size(); i++)
            {
                m_TextureLinks.push_back(std::vector<QString>());
            }
            //get the right sub-array



            //if needed, add empty items

            for(int i = m_TextureLinks.at(m_Shaders.size() - 1 - pass).size(); i < 4; i++)
            {
                m_TextureLinks.at(m_Shaders.size() - 1 - pass).push_back(QString());
            }


            //set our current request

            m_TextureLinks.at(m_Shaders.size() - 1 - pass).at(in) = image;

            m_createdTexture = false;
            m_IsPrepared = false;
        }

        m_Mutex.unlock();

    }

    inline void SetInputMap(cTMap * m, int in, int pass)
    {
        m_Mutex.lock();
        if(m_Shaders.size() > pass && in < 4)
        {
            //if needed, add empty lists to the TextureLinks array

            for(int  i = textures.size(); i < m_Shaders.size(); i++)
            {
                textures.push_back(std::vector<cTMap*>());
            }
            //get the right sub-array


            //if needed, add empty items

            for(int i = textures[m_Shaders.size() - 1 - pass].size(); i < 4; i++)
            {
                textures[m_Shaders.size() - 1 - pass].push_back(nullptr);
            }


            //set our current request

            if(textures[m_Shaders.size() - 1 - pass][in] != nullptr)
            {
                delete textures[m_Shaders.size() - 1 - pass][in];
            }
            textures.at(m_Shaders.size() - 1 - pass).at(in) = m;

            m_createdTexture = false;
            m_IsPrepared = false;
        }

        m_Mutex.unlock();


    }

    virtual inline bool IsMovable() override
    {
        return m_Is2D;
    }

    virtual inline bool IsScaleAble() override
    {
        return m_Is2D;
    }

    inline virtual LSMVector3 GetPosition() override
    {
        m_Mutex.lock();
        if(m_Is2D)
        {
            if(m_Is2DABS)
            {
                BoundingBox bbcrs = BoundingBox(0.0,1.0,0.0,1.0);
                float minx = oldtlx + m_BoundingBox.GetMinX() *(oldbrx-oldtlx);
                float maxx = oldtlx + m_BoundingBox.GetMaxX() *(oldbrx-oldtlx);
                float miny = oldtly + m_BoundingBox.GetMinY() *(oldbry-oldtly);
                float maxy = oldtly + m_BoundingBox.GetMaxY() *(oldbry-oldtly);

                bbcrs.Set(minx,maxx,miny,maxy);

                m_Mutex.unlock();
                return LSMVector3(bbcrs.GetCenterX(),0.0,bbcrs.GetCenterY());

            }else
            {


                m_Mutex.unlock();
                return LSMVector3(m_BoundingBox.GetCenterX(),0.0,m_BoundingBox.GetCenterY());
            }

        }else
        {

            m_Mutex.unlock();
            return m_Position;
        }
    }

    inline virtual LSMVector3 GetScale() override
    {
        m_Mutex.lock();
        if(m_Is2D)
        {
            if(m_Is2DABS)
            {
                LSMVector3 returnv  = LSMVector3(m_BoundingBox.GetSizeX() * (oldbrx - oldtlx),0.0,m_BoundingBox.GetSizeY()*(oldbry - oldtly));
                m_Mutex.unlock();
                return returnv;
            }else
            {
                LSMVector3 returnv  = LSMVector3(m_BoundingBox.GetSizeX(),0.0,m_BoundingBox.GetSizeY());;
                m_Mutex.unlock();
                return returnv;

            }

        }else
        {

            m_Mutex.unlock();
            return m_Scale;
        }
    }
    inline virtual void Scale(LSMVector3 s) override
    {
        m_Mutex.lock();

        m_BoundingBox.ScaleX(s.x);
        m_BoundingBox.ScaleY(s.z);

        m_Mutex.unlock();


    }
    inline virtual void Move(LSMVector3 m) override
    {
        m_Mutex.lock();

        std::cout << " move shader  " << std::endl;
        if(m_Is2D)
        {
            if(m_Is2DABS)
            {

                float movex = m.x/std::fabs((oldtlx-oldbrx));
                float movey = m.z/std::fabs((oldtly-oldbry));

                m_BoundingBox.Move(LSMVector2(movex,movey));

            }else
            {

                    m_BoundingBox.Move(LSMVector2(m.x,m.z));
                }

        }else
        {
        }
        m_Mutex.unlock();
    }



    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }


    inline void OnDraw3DPostProcess(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

    }

    inline void SwapTargets()
    {
        for(int i = 0; i < m_Target.size(); i++)
        {
            OpenGLCLMSAARenderTarget * temp = m_Target.at(i);
            m_Target[i] = m_TargetB.at(i);
            m_TargetB[i] = temp;
        }
    }

    inline double seconds_since_local_midnight() {
        time_t now;
        if (time(&now) == -1) {
          return -1;
        }
        struct tm timestamp;
        if (localtime_s(&timestamp,&now) == 0) { // C23
          return -1;
        }
        timestamp.tm_isdst = -1; // Important
        timestamp.tm_hour = 0;
        timestamp.tm_min = 0;
        timestamp.tm_sec = 0;
        time_t midnight = mktime(&timestamp);
        if (midnight == -1) {
          return -1;
        }
        return difftime(now, midnight);
      }


    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_Mutex.lock();

        oldtlx = s.tlx;
        oldtly =s.tly;
        oldbrx = s.brx;
        oldbry = s.bry;

        m_Frame++;
        if(m_Frame == 0)
        {
            m_TimeStart = s.m_time;
        }

        if(m_Is2D)
        {
            BoundingBox bbscreen;
            BoundingBox bbcrs;

            BoundingBox bbscreenreal;
            BoundingBox bbcrsreal;

            //the projection is only valid for m_Is2D is true and not absolut
            GeoProjection m_CRS;

            //what is the area of the shader that we draw?

            if(this->m_Is2DABS)
            {
                float minx = s.tlx + m_BoundingBox.GetMinX() *(s.brx-s.tlx);
                float maxx = s.tlx + m_BoundingBox.GetMaxX() *(s.brx-s.tlx);
                float miny = s.tly + m_BoundingBox.GetMinY() *(s.bry-s.tly);
                float maxy = s.tly + m_BoundingBox.GetMaxY() *(s.bry-s.tly);

                bbscreen =m_BoundingBox;
                bbcrs.Set(minx,maxx,miny,maxy);
            }else
            {
                float minx = (m_BoundingBox.GetMinX() - s.tlx)/(s.brx-s.tlx);
                float maxx = (m_BoundingBox.GetMaxX() - s.tlx)/(s.brx-s.tlx);
                float miny = (m_BoundingBox.GetMinY() - s.tly)/(s.bry-s.tly);
                float maxy = (m_BoundingBox.GetMaxY() - s.tly)/(s.bry-s.tly);

                bbcrs =m_BoundingBox;
                bbscreen.Set(minx,maxx,miny,maxy);
            }

            LSMVector3 as_ulc = LSMVector3(0.0,0.0,0.0);
            LSMVector3 as_brc = LSMVector3(1.0,1.0,1.0);

            if(this->m_AutoScale)
            {
                bbscreenreal = bbscreen;
                bbcrsreal = bbcrs;
                if((bbscreen.GetMinX() < 0.0 && bbscreen.GetMaxX() <0.0) || (bbscreen.GetMinX() > 1.0 && bbscreen.GetMaxX() >1.0) || (bbscreen.GetMinY() < 0.0 && bbscreen.GetMaxY() <0.0) ||(bbscreen.GetMinY() >1.0 && bbscreen.GetMaxY() >1.0))
                {
                    std::cout << "set0" << std::endl;
                    bbscreenreal.Set(0,0,0,0);
                    bbcrs.Set(0,0,0,0);
                }else
                {
                    if(bbscreen.GetMinX() < 0.0)
                    {
                        bbscreenreal.Set(0.0,bbscreen.GetMaxX(),bbscreen.GetMinY(),bbscreen.GetMaxY());
                    }
                    if(bbscreen.GetMaxX() > 1.0)
                    {
                        bbscreenreal.Set(bbscreen.GetMinX(),1.0,bbscreen.GetMinY(),bbscreen.GetMaxY());
                    }
                    if(bbscreen.GetMinY() < 0.0)
                    {
                        bbscreenreal.Set(bbscreen.GetMinX(),bbscreen.GetMaxX(),0.0,bbscreen.GetMaxY());
                    }
                    if(bbscreen.GetMaxY() > 1.0)
                    {
                        bbscreenreal.Set(bbscreen.GetMinX(),bbscreen.GetMaxX(),bbscreen.GetMinY(),1.0);
                    }

                    float minx = s.tlx + bbscreenreal.GetMinX() *(s.brx-s.tlx);
                    float maxx = s.tlx + bbscreenreal.GetMaxX() *(s.brx-s.tlx);
                    float miny = s.tly + bbscreenreal.GetMinY() *(s.bry-s.tly);
                    float maxy = s.tly + bbscreenreal.GetMaxY() *(s.bry-s.tly);

                    bbcrsreal.Set(minx,maxx,miny,maxy);

                }

                as_ulc.x = (bbscreenreal.GetMinX() - bbscreen.GetMinX())/std::max(1e-12,bbscreen.GetSizeX());
                as_ulc.y = (bbscreenreal.GetMinY() - bbscreen.GetMinY())/std::max(1e-12,bbscreen.GetSizeY());
                as_brc.x = (bbscreenreal.GetMaxX() - bbscreen.GetMinX())/std::max(1e-12,bbscreen.GetSizeX());
                as_brc.y = (bbscreenreal.GetMaxY() - bbscreen.GetMinY())/std::max(1e-12,bbscreen.GetSizeY());

            }else
            {
                bbcrsreal = bbcrs;
                bbscreenreal = bbscreen;
            }

            GLint drawFboId = 0, readFboId = 0;
            glad_glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
            glad_glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

            //first, we draw our stuff to the buffers
            for(int i = 0; i < m_Shaders.size(); i++)
            {

                if(!(m_Programs.size() >i))
                {
                    break;
                }

                glad_glBindFramebuffer(GL_FRAMEBUFFER, m_Target.at(i)->GetFrameBuffer());
                glad_glViewport(0,0,m_Target.at(i)->GetWidth(),m_Target.at(i)->GetHeight());
                glad_glClearColor(0.5,0.5,0.5,1.0);
                glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                OpenGLProgram * program = m_Programs.at(i);
                //set up shader and uniforms

                // bind shader
                glad_glUseProgram(program->m_program);

                int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");


                std::cout << 1<< std::endl;
                // set project matrix

                glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                glad_glUniform1i(islegend_loc,0);
                glad_glUniform1i(istransformed_loc,1);
                glad_glUniform1f(alpha_loc,1.0f-GetStyle().GetTransparancy());

                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"texcoord_ulc"),as_ulc.x,as_ulc.y);
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"texcoord_brc"),as_brc.x,as_brc.y);

                //uniform vec2 iScreenResolution; //resolution of full opengl screen of LISEM
                //uniform vec2 iResolution; //resolution of shader buffers
                //uniform float iTime; //time in seconds since shader was added
                //uniform int iFrame; //frame number since this shader was added
                //uniform vec2 iChannelResolution[4]; //resolution of input images
                //uniform vec4 iMouse; //xy->mouse coords (relative to shader area), zw->left and right mouse button pressed
                //uniform sampler2D iChannel0; //input images, have resolution iChannelResolution[i]
                //uniform sampler2D iChannel1;
                //uniform sampler2D iChannel2;
                //uniform sampler2D iChannel3;
                //uniform sampler2D iChannelB0; //input buffers as obtained from previous frame
                //uniform sampler2D iChannelB1;
                //uniform sampler2D iChannelB2;
                //uniform sampler2D iChannelB3;
                //uniform vec4 iDate; //year,month,day,time in seconds
                //uniform vec4 iScreenCoords; //the upper left and bottom right coordinates in screen relative numbers (0-1)
                //uniform vec4 iCRSCoords; //the upper left and bottom right coordinates in CRS coordinates
                //uniform highp vec3 CameraPosition; //only valid in 3D
                //uniform highp mat4 CMatrix;//only valid in 3D
                //uniform highp vec3 iSunDir; //only valid in 3D
                //uniform highp sampler2D ScreenColor; //from previous full-screen frame, has resolution iScreenResolution
                //uniform highp sampler2D ScreenPosX; //from previous full-screen frame, has resolution iScreenResolution
                //uniform highp sampler2D ScreenPosY; //from previous full-screen frame, has resolution iScreenResolution
                //uniform highp sampler2D ScreenPosZ; //from previous full-screen frame, has resolution iScreenResolution
                //uniform highp sampler2D ScreenNormal; //from previous full-screen frame, has resolution iScreenResolution

                //common
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"iScreenResolution"),s.scr_pixwidth,s.scr_pixheight);
                glad_glUniform2f(glad_glGetUniformLocation(program->m_program,"iResolution"),m_RTSizeX,m_RTSizeY);
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iFrame"),m_Frame);
                glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time - m_TimeStart);


                //date
                std::time_t t = std::time(nullptr);
                std::tm *const pTInfo = std::localtime(&t);
                int year = 1900 + pTInfo->tm_year;
                int month = pTInfo->tm_mon + 1;
                int day = pTInfo->tm_mday;
                float seconds = seconds_since_local_midnight();

                glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"iDate"),year,month,day,seconds);

                //mouse props
                glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"iMouse"),s.MousePosX,s.MousePosY,s.MouseLeftButton?1.0:0.0,s.MouseRightButton?1.0:0.0);

                //camera stuff
                //glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());
                glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());


                //coordinates
                glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"iScreenCoords"),bbscreen.GetMinX(),bbscreen.GetMaxX(),bbscreen.GetMinY(),bbscreen.GetMaxY());
                glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"iCRSCoords"),bbcrs.GetMinX(),bbcrs.GetMaxX(),bbcrs.GetMinY(),bbcrs.GetMaxY());


                //textures

                std::vector<const char *> names = {"iChannel0","iChannel1","iChannel2","iChannel3"};
                std::vector<const char *> namesb = {"iChannelB0","iChannelB1","iChannelB2","iChannelB3"};


                int n_set_texture = 0;
                for(int j= 0; j < m_Textures.at(i).size(); j++)
                {
                    if(m_Textures.at(i).at(j) != nullptr)
                    {
                        //maximum support now for 4 textures
                        if(j > 3)
                        {
                            break;
                        }
                        n_set_texture ++;
                        // bind texture
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,names[j]),j);
                        glad_glActiveTexture(GL_TEXTURE0+ j);
                        glad_glBindTexture(GL_TEXTURE_2D,m_Textures.at(i).at(j)->m_texgl);
                    }
                }

                int n_buffer = std::min(4,(int)m_Shaders.size());
                for(int j = 0; j < n_buffer; j++)
                {
                    //maximum support now for 4 buffers
                    if(j > 3)
                    {
                        break;
                    }
                    int index = std::min((int)m_TargetB.size()-1,std::max(0,(int)m_Shaders.size() - 4) + j);

                    // bind texture
                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,namesb[j]),j + n_set_texture);
                    glad_glActiveTexture(GL_TEXTURE0+ j + n_set_texture);
                    glad_glBindTexture(GL_TEXTURE_2D,m_TargetB.at(index)->GetTexture());
                }

                // now render stuff
                glad_glBindVertexArray(m->m_Quad->m_vao);
                glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                glad_glBindVertexArray(0);

            }


            //blit to textuer, only required in case of msaa
            for(int i = 0; i < m_Target.size(); i++)
            {
                m_Target.at(i)->BlitToTexture();

            }


            GLuint FB = s.GL_FrameBuffer->GetFrameBuffer();
            glad_glBindFramebuffer(GL_FRAMEBUFFER, FB);

            glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);


            //glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER_BINDING, drawFboId);
            //glad_glBindFramebuffer(GL_READ_FRAMEBUFFER_BINDING, readFboId);

            //draw the final buffer to the screen
            //two options here: either a crs-matching or crs-not caring, or a different crs
            //in the last case, we need real-time reprojection


            std::cout << "draw to screen " << std::endl;

            //set shader uniform values
            OpenGLProgram * program = GLProgram_uigeoimage;

            // bind shader
            glad_glUseProgram(program->m_program);
            /*
             *
uniform mat4 matrix;

uniform float ul_pixx;
uniform float ul_pixy;
uniform float ul_pixdx;
uniform float ul_pixdy;

uniform float ul_sizex;
uniform float ul_sizey;
uniform float ul_transx;
uniform float ul_transy;

uniform float ug_windowpixsizex;
uniform float ug_windowpixsizey;
uniform float ug_windowsizex;
uniform float ug_windowsizey;
uniform float ug_windowhwration;
uniform float ug_windowtransx;
uniform float ug_windowtransy;

uniform float tr_sizex;
uniform float tr_sizey;
uniform float tr_transx;
uniform float tr_transy;


uniform int is_transformedf;

//actual map values
uniform sampler2D tex1;

//transformation properties (texture contains x and y coordinates)
uniform sampler2D texX;
uniform sampler2D texY;

uniform float alpha = 1.0;
;*/

            std::cout << "abs2d "<< m_Is2DABS << " " <<  bbcrsreal.GetSizeX() << " " << bbcrsreal.GetSizeY() << " " << bbcrsreal.GetCenterX() << " " << bbcrsreal.GetCenterY() << std::endl;

            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
            int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
            int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
            int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
            int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),bbcrsreal.GetSizeX());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),bbcrsreal.GetSizeY());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),bbcrsreal.GetCenterX());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),bbcrsreal.GetCenterY());

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),m_RTSizeX);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),m_RTSizeY);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),bbcrsreal.GetSizeX()/m_RTSizeX);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),bbcrsreal.GetSizeY()/m_RTSizeY);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),s.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),s.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),s.width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),s.height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),s.scr_ratio);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),s.translation_x);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),s.translation_y);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),s.scr_width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),s.scr_height);


            std::cout << "set buffer as texture " << std::endl;

            //set our last shader pass as our image input
            if(m_Target.size() > 0)
            {
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex1"),0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,m_Target.at(m_Target.size()-1)->GetTexture());
            }

            //uniform float ul_transy;
            if(m_Is2DABS)
            {
                //we draw a 2d in absolute screen relative coordinates (0,0)-(1,1)
                //crs does not matter for drawing area, but matters in the shader (what is 1 unit?)
                //we provide the uniform constants to deal with that

                glad_glUniform1i(istransformed_loc,0);
                glad_glUniform1i(istransformedf_loc,0);

            }else
            {
                //we draw a 2d square in crs coordinates.
                //if the crs is generic or equal, we can draw a sub-screen square, otherwise we need the GLTransform

                WorldGLTransform * gltransform = tm->Get(s.projection,this->GetProjection());

                if(gltransform != nullptr)
                {
                    if(!gltransform->IsGeneric())
                    {
                        BoundingBox b = gltransform->GetBoundingBox();

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),bbcrsreal.GetSizeX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),bbcrsreal.GetSizeY());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),bbcrsreal.GetCenterX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),bbcrsreal.GetCenterY());

                        glad_glUniform1i(istransformed_loc,1);
                        glad_glUniform1i(istransformedf_loc,1);

                        glad_glUniform1i(tex_x_loc,1);
                        glad_glActiveTexture(GL_TEXTURE1);
                        glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureX()->m_texgl);

                        glad_glUniform1i(tex_y_loc,2);
                        glad_glActiveTexture(GL_TEXTURE2);
                        glad_glBindTexture(GL_TEXTURE_2D,gltransform->GetTextureY()->m_texgl);
                    }else {

                        glad_glUniform1i(istransformed_loc,0);
                        glad_glUniform1i(istransformedf_loc,0);
                    }

                }else
                {
                    glad_glUniform1i(istransformed_loc,0);
                    glad_glUniform1i(istransformedf_loc,0);
                }


            }



            std::cout << "draw " << std::endl;

            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            glad_glUniform1f(alpha_loc,1.0f-GetStyle().GetTransparancy());

            // now render stuff
            glad_glBindVertexArray(m->m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);


            std::cout << "done " << std::endl;
        }


        glad_glViewport(0,0,s.scr_pixwidth,s.scr_pixheight);

        m_Mutex.unlock();

        SwapTargets();

    }

    inline LayerInfo GetInfo()
    {
        m_Mutex.lock();
        LayerInfo l;
        l.Add("Type","Custom Shader Layer");

        m_Mutex.unlock();

        return l;
    }

    inline void DestroyShaders()
    {
        for(int i=  0; i < m_Programs.size(); i++)
        {
            OpenGLProgram* Program = m_Programs.at(i);
            Program->Destroy();
            delete Program;
        }
        m_Programs.clear();
    }
    inline void DestroyTextures()
    {
        for(int i  =0;i < m_Textures.size(); i++)
        {
            for(int j =0;j < m_Textures.at(i).size(); j++)
            {
                OpenGLCLTexture * t = m_Textures.at(i).at(j);
                if(t != nullptr)
                {
                    t->Destroy();
                    delete t;
                }
            }
        }
        m_Textures.clear();

    }
    inline void DestroyBuffers()
    {
        for(int i = 0; i < m_Target.size(); i++)
        {
            m_Target.at(i)->Destroy();
        }
        m_Target.clear();
        for(int i = 0; i < m_TargetB.size(); i++)
        {
            m_TargetB.at(i)->Destroy();
        }
        m_TargetB.clear();

    }

    bool m_CallBackCompileErrorSet = false;
    std::function<void(std::vector<int>,std::vector<std::vector<int>>,std::vector<std::vector<QString>>)> m_CallBackCompileError;
    template<typename _Callable1, typename... _Args1>
    inline void SetCompileErrorCallback(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackCompileErrorSet = true;
        m_CallBackCompileError = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
    }


    std::vector<int> error_pass;
    std::vector<std::vector<int>> error_line;
    std::vector<std::vector<QString>> error_message;

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        std::cout << "onprepare" << std::endl;

        error_pass.clear();
        error_line.clear();
        error_message.clear();

        m_Mutex.lock();

        bool m_shadersucces = true;

        if(m_IsPrepared == false)
        {

            if(!m_createdShaders)
            {
                std::vector<OpenGLProgram*> programs_temp;
                for(int i =0;i< m_Shaders.size();i++)
                {
                    std::cout << "compile shader " << std::endl;
                    std::cout << m_Shaders.at(i).toStdString()<< std::endl;
                    OpenGLProgram* Program = new OpenGLProgram();
                    QString shaderi = QString("#version 330 \nin highp vec2 texcoord;\nout vec4 fragColor; ")
                    + "\n"
                    + " uniform vec2 iScreenResolution; \n"
                    + "uniform vec2 iResolution; \n"
                    + "uniform float iTime; \n"
                    + "uniform int iFrame;\n"
                    + "uniform vec2 iChannelResolution[4];\n"
                    + "uniform vec4 iMouse;\n"
                    + "uniform sampler2D iChannel0;\n"
                    + "uniform sampler2D iChannel1;\n"
                    + "uniform sampler2D iChannel2;\n"
                    + "uniform sampler2D iChannel3;\n"
                    + "uniform sampler2D iChannelB0;\n"
                    + "uniform sampler2D iChannelB1;\n"
                    + "uniform sampler2D iChannelB2;\n"
                    + "uniform sampler2D iChannelB3;\n"
                    + "uniform vec4 iDate;\n"
                    + "uniform vec4 iScreenCoords;\n"
                    + "uniform vec4 iCRSCoords;\n"
                    + "uniform highp vec3 CameraPosition;\n"
                    + "uniform highp mat4 CMatrix;\n"
                    + "uniform highp vec3 iSunDir;\n"
                    + "uniform highp sampler2D ScreenColor;\n"
                    + "uniform highp sampler2D ScreenPosX;\n"
                    + "uniform highp sampler2D ScreenPosY;\n"
                    + "uniform highp sampler2D ScreenPosZ;\n"
                    + "uniform highp sampler2D ScreenNormal;\n"
                    + "\n"
                    + "\n"
                    + m_Shaders.at(i);

                    try
                    {
                        Program->LoadProgramcfs((KernelDir + "UIShaderLayerDraw.vert").toStdString().c_str(), shaderi.toStdString().c_str());
                    }catch(...)
                    {
                        LISEMS_ERROR("Could not compile shader");
                        std::vector<int> errorlines = Program->GetErrorLines();
                        std::vector<QString> errormessages = Program->GetErrorMessages();

                        error_pass.push_back(i);
                        error_line.push_back(errorlines);
                        error_message.push_back(errormessages);

                        m_shadersucces = false;
                    }

                    programs_temp.push_back(Program);

                }

                if(m_shadersucces == true)
                {
                    std::cout << "compile done" << std::endl;
                        DestroyShaders();
                        std::cout << "set shader programs " << std::endl;
                        for(int i = 0; i < programs_temp.size(); i++)
                        {
                            m_Programs.push_back(programs_temp.at(i));
                        }
                }
             }

            if(m_shadersucces == true)
            {
                if(!m_createdTexture)
                {
                    //std::vector<std::vector<std::vector<cTMap *>>> textures;
                    //std::vector<QString> m_TextureLinks;
                    //std::vector<std::vector<OpenGLCLTexture*>> m_Textures;

                    std::cout << "destroy texts" <<std::endl;
                    DestroyTextures();
                    std::vector<std::vector<OpenGLCLTexture *>> texs;

                    std::cout << "destroy done" << std::endl;

                    for(int i = 0; i < m_Shaders.size(); i++)
                    {


                        int size_files = 0;
                        int size_texts = 0;

                        if(textures.size() > i)
                        {
                            size_texts = textures.at(i).size();
                        }
                        if(m_TextureLinks.size() > i)
                        {
                            size_files = m_TextureLinks.at(i).size();
                        }

                        //how many can we have at most?
                        int count_check= std::max(4,std::min((int)size_texts,(int)size_files));
                        std::vector<OpenGLCLTexture *> texs2;

                        std::cout << "size texture files "<< size_files << std::endl;

                        for(int j = 0; j < count_check; j++)
                        {

                            std::cout << "check image " << j << std::endl;

                            bool done_j = false;

                            //can we find a file link
                            if(size_texts > j)
                            {
                                if(textures.at(i).at(j) != nullptr)
                                {
                                    std::cout << "load text from map" << std::endl;
                                    done_j = true;

                                    OpenGLCLTexture * t = new OpenGLCLTexture();
                                    t->Create2DFromMap(m->context,&(textures.at(i).at(j)->data));
                                    texs2.push_back(t);
                                }

                            }

                            if(!done_j)
                            {
                                //can we find a memory texture
                                if(size_files > j)
                                {
                                    if(!m_TextureLinks.at(i).at(j).isEmpty())
                                    {

                                        OpenGLCLTexture * t = new OpenGLCLTexture();
                                        QString file =m_TextureLinks.at(i).at(j);

                                        std::cout << "load text from file "<< file.toStdString() <<  std::endl;

                                        //is it a regular image

                                        if(file.endsWith(".jpg")|| file.endsWith(".jpeg")|| file.endsWith(".png")||file.endsWith(".bmp"))
                                        {
                                            t->Create2DFromFile(file);

                                            // or a geo-image
                                        }else
                                        {
                                            try
                                            {
                                                std::vector<cTMap *> maps = LoadMapBandList(file);
                                                std::cout << "loaded maps "<< maps.size() << std::endl;
                                                if(maps.size() >4)
                                                {
                                                    t->Create2DFromMaps(m->context,{maps[0],maps[1],maps[2],maps[3]});
                                                }else if(maps.size() > 0)
                                                {
                                                    t->Create2DFromMaps(m->context,maps);
                                                }
                                            }catch(...)
                                            {
                                                 t = nullptr;
                                            }
                                        }


                                        texs2.push_back(t);
                                    }
                                }

                                //otherwise add a nullptr

                                if(!done_j)
                                {
                                    texs2.push_back(nullptr);

                                }
                            }



                            std::cout << "text done" << std::endl;


                        }
                        texs.push_back(texs2);





                    }
                    std::cout << "textst  done " << std::endl;
                     m_Textures = texs;
                }


                if(!m_createdRenderTargets)
                {
                    DestroyBuffers();


                    for(int i = 0; i < m_Shaders.size(); i++)
                    {
                        OpenGLCLMSAARenderTarget * target = new OpenGLCLMSAARenderTarget();
                        target->Create(m_RTSizeX,m_RTSizeY,1,GL_RGBA32F,GL_RGBA32F,GL_RGBA,GL_FLOAT);
                        m_Target.push_back(target);
                    }
                    for(int i = 0; i < m_Shaders.size(); i++)
                    {
                        OpenGLCLMSAARenderTarget * target = new OpenGLCLMSAARenderTarget();
                        target->Create(m_RTSizeX,m_RTSizeY,1,GL_RGBA32F,GL_RGBA32F,GL_RGBA,GL_FLOAT);
                        m_TargetB.push_back(target);
                    }
                    m_createdRenderTargets= true;
                }
            }
        }

        std::cout <<" prepare done" << std::endl;
        if(m_CallBackCompileErrorSet && m_shadersucces == false)
        {
            m_CallBackCompileError(error_pass,error_line,error_message);
        }

        m_IsPrepared = true;
        m_createdTexture = true;
        m_createdShaders = true;
        m_Mutex.unlock();

    }



    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_Mutex.lock();
        //destroy and delete all
        DestroyShaders();
        DestroyBuffers();
        DestroyTextures();
        for(int i =0;i< textures.size();i++)
        {
            for(int j =0;j< textures.at(i).size();j++)
            {

                if(textures.at(i).at(j) != nullptr)
                {

                    delete textures.at(i).at(j);
                    textures.at(i).at(j) = nullptr;
                }

            }
        }
        m_Mutex.unlock();
    }

    inline void UpdateShader(std::vector<QString> buffershaders)
    {
        m_Mutex.lock();
        m_Shaders = buffershaders;
        m_createdShaders= false;
        m_IsPrepared = false;
        m_Mutex.unlock();
    }


    inline void UpdateRTSize(int x, int y)
    {
        m_Mutex.lock();
        m_RTSizeX = x;
        m_RTSizeY = y;
        m_createdRenderTargets = false;
        m_IsPrepared = false;
        m_Mutex.unlock();
    }


    inline void UpdateImages(std::vector<std::vector<QString>> images, std::vector<std::vector<cTMap *>> rasters)
    {
        m_Mutex.lock();
        for(int i =0;i< textures.size();i++)
        {
            for(int j =0;j< textures.at(i).size();j++)
            {

                delete textures.at(i).at(j);

            }
        }
        textures.clear();
        m_TextureLinks = images;
        textures = rasters;

        m_createdTexture = false;
        m_IsPrepared = false;

        m_Mutex.unlock();
    }

    inline std::vector<QString> GetShaderTexts()
    {
        std::vector<QString> ret;
        m_Mutex.lock();

        ret = m_Shaders;

        m_Mutex.unlock();
        return ret;
    }

    inline std::vector<QString> GetImages()
    {
        return {};

    }
    inline std::vector<std::vector<cTMap*>> GetImagesMem()
    {

        return {};
    }

    UILayerEditor* GetEditor();


};

#endif // UISHADERLAYER_H
