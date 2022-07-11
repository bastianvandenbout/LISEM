#ifndef UIIMAGE_H
#define UIIMAGE_H


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
#include "gl/openglcldatamanager.h"


class UIImageLayer : public UIGeoLayer
{
private:

    bool m_Is2D;
    bool m_Is2DABS;

    //these might be for 2D in crs coordinates, or 2d abs,which means screen relative coordinates
    BoundingBox m_BoundingBox;

    OpenGLCLTexture * m_Texture = nullptr;
    QString m_File;

    //the projection is only valid for m_Is2D is true and not absolut
    GeoProjection m_CRS;

    QMutex m_Mutex;
    double oldtlx = 0.0;
    double oldtly = 0.0;
    double oldbrx = 0.0;
    double oldbry = 0.0;

    cTMap * m_R= nullptr;
    cTMap * m_G= nullptr;
    cTMap * m_B= nullptr;
    cTMap * m_A= nullptr;

    bool m_SetAdjustInitialSizeForAspectRatio = false;
public:

    inline UIImageLayer(QString file, BoundingBox position, GeoProjection p = GeoProjection::GetGeneric(), bool is_rel = true)
    {
        m_File = file;
        m_BoundingBox = position;
        m_Is2D = true;
        m_Is2DABS =is_rel;
        m_CRS = p;
    }

    inline UIImageLayer(cTMap * r, cTMap * g, cTMap * b, cTMap * a, BoundingBox position, GeoProjection p = GeoProjection::GetGeneric(), bool is_rel = true)
    {
        m_R = r;
        m_G = g;
        m_B = b;
        m_A = a;

        m_BoundingBox = position;
        m_Is2D = true;
        m_Is2DABS =is_rel;
        m_CRS = p;

    }

    inline QString layertypeName()
    {
        return "ImageLayer";
    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
          m_Exists = true;
    }

    inline void SetAdjustInitialSizeForAspectRatio()
    {
        m_SetAdjustInitialSizeForAspectRatio = true;
    }

    inline void setPosition(BoundingBox pos, GeoProjection p, bool rel = false)
    {
        m_BoundingBox = pos;
        m_Is2D = true;
        m_Is2DABS = rel;
        m_CRS = p;
    }

    inline void setImage(QString s)
    {
        m_File = s;
    }


    inline void setImage(cTMap * r, cTMap * g, cTMap * b, cTMap * a)
    {
        m_R = r;
        m_G = g;
        m_B = b;
        m_A = a;
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

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_Mutex.lock();

        oldtlx = s.tlx;
        oldtly =s.tly;
        oldbrx = s.brx;
        oldbry = s.bry;

        //set shader uniform values
        OpenGLProgram * program = GLProgram_uigeoimage;

        // bind shader
        glad_glUseProgram(program->m_program);

        BoundingBox bbscreen;
        BoundingBox bbcrs;

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
            float minx = (m_BoundingBox.GetMinX() - s.tlx)/(s.tlx-s.brx);
            float maxx = (m_BoundingBox.GetMaxX() - s.tlx)/(s.tlx-s.brx);
            float miny = (m_BoundingBox.GetMinY() - s.tly)/(s.tly-s.bry);
            float maxy = (m_BoundingBox.GetMaxY() - s.tly)/(s.tly-s.bry);

            bbcrs =m_BoundingBox;
            bbscreen.Set(minx,maxx,miny,maxy);
        }



        BoundingBox bbscreenreal;
        BoundingBox bbcrsreal = bbcrs;

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


        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),m_Texture->GetWidth());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),m_Texture->GetHeight());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),bbcrsreal.GetSizeX()/m_Texture->GetWidth());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),bbcrsreal.GetSizeY()/m_Texture->GetHeight());
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),s.scr_pixwidth);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),s.scr_pixheight);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),s.width);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),s.height);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),s.scr_ratio);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),s.translation_x);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),s.translation_y);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),s.scr_width);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),s.scr_height);


        //set our last shader pass as our image input
        //if(m_Target.size() > 0)
        {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex1"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
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



        m_Mutex.unlock();


    }
    inline LayerInfo GetInfo()
    {
        m_Mutex.lock();
        LayerInfo l;
        l.Add("Type","Image Layer");

        m_Mutex.unlock();

        return l;
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
         m_Mutex.lock();


         OpenGLCLTexture * t = new OpenGLCLTexture();
         QString file =m_File;

         if(!file.isEmpty())
         {
             //is it a regular image

             if(file.endsWith(".jpg")|| file.endsWith(".jpeg")|| file.endsWith(".png")||file.endsWith(".bmp"))
             {
                 if(t->Create2DFromFile(file) == 1)
                 {
                     LISEM_ERROR("Could not open file as image: "  + file);
                 }

                 // or a geo-image
             }else
             {
                 try
                 {
                     std::vector<cTMap *> maps = LoadMapBandList(file);
                     if(maps.size() >4)
                     {
                         t->Create2DFromMaps(m->context,{maps[0],maps[1],maps[2],maps[3]});
                     }else if(maps.size() == 1)
                     {
                         t->Create2DFromMaps(m->context,{maps[0],maps[1],maps[2]});

                     }else if(maps.size() > 0)
                     {
                         t->Create2DFromMaps(m->context,maps);
                     }
                 }catch(...)
                 {
                      t = nullptr;
                      LISEM_ERROR("Could not open file as image: "  + file);
                 }
             }


         }

         if(t != nullptr)
         {
             if(m_SetAdjustInitialSizeForAspectRatio)
             {

                 m_BoundingBox.CorrectAspectRatio(((float)(t->GetWidth()))/std::max(1.0f,(float)t->GetHeight()));


                 m_SetAdjustInitialSizeForAspectRatio = false;
             }
         }

         m_Texture =t;

         m_IsPrepared = true;
         m_Mutex.unlock();
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {

        if(m_Texture != nullptr)
        {
            m_Texture->Destroy();
            delete m_Texture;
            m_Texture = nullptr;
        }
    }

};


#endif // UIIMAGE_H
