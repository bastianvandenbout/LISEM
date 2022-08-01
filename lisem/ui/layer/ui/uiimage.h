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
    bool m_FlipV = false;

    int m_FillMode = 0;

    cTMap * m_R= nullptr;
    cTMap * m_G= nullptr;
    cTMap * m_B= nullptr;
    cTMap * m_A= nullptr;

    bool m_HasRotation = false;
    double m_Angle = 0.0;

    bool m_SetAdjustInitialSizeForAspectRatio = false;
public:

    inline UIImageLayer(QString file, BoundingBox position, GeoProjection p = GeoProjection::GetGeneric(), bool is_rel = true, int fillmode = 0)
    {
        m_File = file;
        m_BoundingBox = position;
        m_Is2D = true;
        m_Is2DABS =is_rel;
        m_CRS = p;
        m_FillMode = fillmode;
    }

    inline UIImageLayer(cTMap * r, cTMap * g, cTMap * b, cTMap * a, BoundingBox position, GeoProjection p = GeoProjection::GetGeneric(), bool is_rel = true, int fillmode = 0)
    {
        m_R = r;
        m_G = g;
        m_B = b;
        m_A = a;

        m_BoundingBox = position;
        m_Is2D = true;
        m_Is2DABS =is_rel;
        m_CRS = p;
        m_FillMode = fillmode;
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

    inline void setPositionRotation(BoundingBox pos,float angle, GeoProjection p, bool rel = false)
    {
        m_BoundingBox = pos;
        m_HasRotation = true;
        m_Angle = angle;
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

        if(m_HasRotation)
        {
            program = GLProgram_uigeoimagerotate;
        }
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

        //set rotation
        if(m_HasRotation)
        {
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_angle"),m_Angle);

        }


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

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ul_flipv"),m_FlipV?1:0);



        //set our last shader pass as our image input
        //if(m_Target.size() > 0)
        {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"tex1"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
        }

        //in either case (absolute or relative coordinates)
        //we have different fill modes we want to accomodate
        //0->stretch (default)
        //1->fit
        //2->fill

        //we can resolve this with a texture coordinate offset and scaling for either the x-axis or y-axis
        //first, get screen area pixel width and height, and texture width and height

        float imagescreenpixelsizex = std::max(1.0,bbscreen.GetSizeX() * s.scr_pixwidth);
        float imagescreenpixelsizey = std::max(1.0,bbscreen.GetSizeY() * s.scr_pixheight);
        float imagerealsizex = std::max(1.0,(double)m_Texture->GetWidth());
        float imagerealsizey = std::max(1.0,(double)m_Texture->GetHeight());


        //in case of stretch, do nothing

        float TexCoordXOffset = 0.0;
        float TexCoordXScale = 1.0;
        float TexCoordYOffset = 0.0;
        float TexCoordYScale = 1.0;

        //in case of fit

        if(m_FillMode == 1)
        {
            //if the width ratio is worse than the height ratio

            if(imagerealsizex/imagescreenpixelsizex > imagerealsizey/imagescreenpixelsizey)
            {
                float size_sb_x = imagescreenpixelsizex;
                float size_sb_y = imagescreenpixelsizex * imagerealsizey/imagerealsizex;
                TexCoordYOffset = -(0.5 * (imagescreenpixelsizey - size_sb_y))/imagescreenpixelsizey;
                TexCoordYScale = 1.0/(size_sb_y/imagescreenpixelsizey);

            //else if the height ratio is worse
            }else
            {
                float size_sb_y = imagescreenpixelsizey;
                float size_sb_x = imagescreenpixelsizey * imagerealsizex/imagerealsizey;
                TexCoordXOffset = -(0.5 * (imagescreenpixelsizex - size_sb_x))/imagescreenpixelsizex;
                TexCoordXScale = 1.0/(size_sb_x/imagescreenpixelsizex);
            }
        //in case of fill
        }else if(m_FillMode == 2)
        {

            //if the width ratio is worse than the height ratio

            if(imagerealsizex/imagescreenpixelsizex < imagerealsizey/imagescreenpixelsizey)
            {
                float size_sb_x = imagescreenpixelsizex;
                float size_sb_y = imagescreenpixelsizex * imagerealsizey/imagerealsizex;
                TexCoordYOffset = -(0.5 * (imagescreenpixelsizey - size_sb_y))/imagescreenpixelsizey;
                TexCoordYScale = 1.0/(size_sb_y/imagescreenpixelsizey);

            //else if the height ratio is worse
            }else
            {
                float size_sb_y = imagescreenpixelsizey;
                float size_sb_x = imagescreenpixelsizey * imagerealsizex/imagerealsizey;
                TexCoordXOffset = -(0.5 * (imagescreenpixelsizex - size_sb_x))/imagescreenpixelsizex;
                TexCoordXScale = 1.0/(size_sb_x/imagescreenpixelsizex);
            }
        }

        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_offsetx"),TexCoordXOffset);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_scalex"),TexCoordXScale);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_offsety"),TexCoordYOffset);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_scaley"),TexCoordYScale);



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


        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_offsetx"),0.0);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_scalex"),1.0);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_offsety"),0.0);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"fillmode_scaley"),1.0);
        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ul_flipv"),0);


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


         if(m_R != nullptr || m_G != nullptr || m_B != nullptr || m_A != nullptr)
         {
             cTMap * ref = nullptr;
             if(m_R != nullptr)
             {ref = m_R;
             }else if(m_G != nullptr)
             {ref = m_G;
             }else if(m_B != nullptr)
             {ref = m_B;
             }else if(m_A != nullptr)
             {ref = m_A;
             }

             if(m_R == nullptr)
             {
                 m_R = ref->GetCopy0();
             }
             if(m_G == nullptr)
             {
                 m_G = ref->GetCopy0();
             }
             if(m_B == nullptr)
             {
                 m_B = ref->GetCopy0();
             }
             if(m_A == nullptr)
             {
                 m_A = ref->GetCopy1();
             }
             std::vector<cTMap *> maps = {m_R,m_G,m_B,m_A};
             t->Create2DFromMaps(m->context,{maps[0],maps[1],maps[2],maps[3]});

         }else if(!file.isEmpty())
         {
             //is it a regular image

             if(file.endsWith(".jpg")|| file.endsWith(".jpeg")|| file.endsWith(".png")||file.endsWith(".bmp"))
             {

                 m_FlipV = true;

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
                         t->Create2DFromMaps(m->context,{maps[0],maps[0],maps[0]});

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
