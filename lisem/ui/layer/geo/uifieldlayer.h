#ifndef UIFIELDLAYER_H
#define UIFIELDLAYER_H


#include "geo/raster/field.h"
#include "layer/uilayer.h"
#include "gl3dgeometry.h"
#include "models/3dmodel.h"
#include "opengl3dobject.h"
#include "gl/openglcldatamanager.h"
#include "openglcltexture3d.h"
#include "QMutex"

class UIFieldLayer : public UIGeoLayer
{
private:


    Field* m_Field;
    ModelGeometry * m_Cube;
    gl3dObject * m_Actor;
    OpenGLCLTexture3D * m_Texture;

    QMutex m_FieldMutex;
    bool m_UpdateGLField = false;
    bool m_IsSurface = false;
    double m_Hardness = 10.0;
    float m_Val_Max = 1.0;
    float m_Val_Avg = 0.5;
    float m_Val_Min = 0.0;
protected:


public:
    inline UIFieldLayer() : UIGeoLayer(GeoProjection(),BoundingBox(),"Field",false,"",false)
    {
         m_IsNative = false;
         m_IsUser = true;
         m_IsLayerSaveAble = true;


         m_Parameters->AddParameter("Hardness",m_Hardness,10.0,true,0.0,1000.0);
         m_Parameters->AddParameter("IsSurface",m_IsSurface,false);
    }

    inline UIFieldLayer(Field*g,QString name, bool is_file, QString file_name, bool native = false) : UIGeoLayer(GeoProjection(),BoundingBox(g->GetBoundingBox().GetMinX(),g->GetBoundingBox().GetMaxX(),g->GetBoundingBox().GetMinY(),g->GetBoundingBox().GetMaxY()),QString("3D Object"),false,QString(""),false)
    {
         g->GetMinMax(&m_Val_Min,&m_Val_Max, &m_Val_Avg);
         m_Field = g;
         m_IsNative = native;
         m_IsUser = true;
         m_IsLayerSaveAble = true;
         m_Style.m_StyleSimpleGradient = true;
         m_Style.m_StyleSimpleRange = true;
         m_HasLegend = true;


         m_Parameters->AddParameter("Hardness",m_Hardness,10.0,true,0.0,1000.0);
        m_Parameters->AddParameter("IsSurface",m_IsSurface,false);
    }

    inline ~UIFieldLayer()
    {

    }


    inline QString layertypeName()
    {
        return "UIFieldLayer";
    }

    inline bool IsLayerDirectReplaceable(Field * maps)
    {

        if(m_Field->nrLevels() != maps->nrLevels() || m_Field->nrCols() != maps->nrCols() || m_Field->nrRows() != maps->nrRows())
        {
            return false;
        }
        return true;

    }

    inline void DirectReplace(Field * maps)
    {
        m_FieldMutex.lock();

        if(m_IsPrepared)
        {
            maps->GetMinMax(&m_Val_Min,&m_Val_Max, &m_Val_Avg);


            for(int i = 0; i < maps->nrLevels(); i++)
            {
                for(int r = 0; r < maps->nrRows(); r++)
                {
                    for(int c = 0; c < maps->nrCols(); c++)
                    {
                        m_Field->at(i)->data[r][c] = maps->ValueAt(i,r,c);
                    }
                }
            }

        }else
        {
            maps->GetMinMax(&m_Val_Min,&m_Val_Max, &m_Val_Avg);

            for(int i = 0; i < maps->nrLevels(); i++)
            {
                for(int r = 0; r < maps->nrRows(); r++)
                {
                    for(int c = 0; c < maps->nrCols(); c++)
                    {
                        m_Field->at(i)->data[r][c] = maps->ValueAt(i,r,c);
                    }
                }
            }
        }


        m_UpdateGLField  = true;


        m_FieldMutex.unlock();

    }

    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnPostDraw3D(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {

    }

    inline void OnPreDraw3D(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_FieldMutex.lock();
        //create cube geometry
        m_Cube = new ModelGeometry();
        LSMMesh me;
        me.SetAsCube(LSMVector3(1.0,1.0,1.0),LSMVector3(0.5,0.5,0.5));
        m_Cube->AddMesh(me);

        m_Actor = new gl3dObject(m,m_Cube);

        m_Texture = new OpenGLCLTexture3D();
        m_Texture->CreateFromField(m_Field);

        m_UpdateGLField = false;

        m_IsPrepared = true;

        m_FieldMutex.unlock();
    }

    inline void OnDraw(OpenGLCLManager *m, GeoWindowState s) override
    {

        if(m_IsPrepared)
        {
            m_FieldMutex.lock();

            m_Texture->UpdateDataFromField(m_Field);


            m_UpdateGLField = false;

            m_FieldMutex.unlock();

        }


    }

    inline void OnDraw3DTransparentLayer(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


        m_FieldMutex.lock();

        m_Parameters->UpdateParameters();

        std::vector<gl3dMesh *> meshes = m_Actor->GetMeshes();

        glad_glDepthMask(GL_FALSE);
        glad_glDisable(GL_DEPTH_TEST);
        glad_glEnable(GL_CULL_FACE);
        glad_glCullFace(GL_FRONT);

        for(int i = 0; i < meshes.size(); i++)
        {

            gl3dMesh * mesh = meshes.at(i);

            LSMVector3 camerapos = s.Camera3D->GetPosition();
            LSMVector3 objpos = LSMVector3(this->m_Field->GetWest(),this->m_Field->GetBottom(),this->m_Field->GetNorth());
            LSMMatrix4x4 objrotm = LSMMatrix4x4();
            LSMMatrix4x4 objscale = LSMMatrix4x4();
            objscale.SetScaling(m_Field->GetSizeX(),m_Field->GetSizeZ(),m_Field->GetSizeY());


            LSMMatrix4x4 objtrans;
            objtrans.Translate(LSMVector3(objpos.x - camerapos.x,objpos.y,objpos.z - camerapos.z));
            objrotm = objtrans*objrotm *objscale;

            s.GL_3DFrameBuffer->SetAsTarget();

            //check if camera is inside the object


            bool cam_in = false;



            //set shader uniform values
            OpenGLProgram * program = GLProgram_uifield;


            // bind shader
            glad_glUseProgram(program->m_program);

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"OMatrix"),1,GL_FALSE,(float*)(&objrotm));//.GetMatrixDataPtr());

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjTranslate"),objpos.x - camerapos.x,objpos.y,objpos.z  - camerapos.z);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjScale"),m_Field->GetSizeX(),m_Field->GetSizeZ(),m_Field->GetSizeY());

            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isSurface"),m_IsSurface? 1:0);


            LSMStyle style = GetStyle();

            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
            glad_glUniform1f(alpha_loc,1.0f-style.GetTransparancy());

            int hard_loc = glad_glGetUniformLocation(program->m_program,"hardness");
            glad_glUniform1f(hard_loc,m_Hardness);

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < style.m_ColorGradientb1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,style.m_ColorGradientb1.m_Gradient_Stops.at(i));
                    ColorF c = style.m_ColorGradientb1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }


            float hmax = -1e31f;
            float hmin = 1e31f;


            hmax = m_Val_Max;
            hmin = m_Val_Min;

            if(!(style.m_Intervalb1.GetMax()  == 0.0f && style.m_Intervalb1.GetMin()  == 0.0f) && style.m_Intervalb1.GetMax()  > style.m_Intervalb1.GetMin() )
            {
                hmax = style.m_Intervalb1.GetMax() ;
                hmin = style.m_Intervalb1.GetMin() ;
            }
            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");


            glad_glUniform1f(h_max_loc,hmax);
            glad_glUniform1f(h_min_loc,hmin);


            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_3D,m_Texture->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iTex3D"),0);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenColor"),3);
            glad_glActiveTexture(GL_TEXTURE3);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(0));

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosX"),4);
            glad_glActiveTexture(GL_TEXTURE4);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(1));

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosY"),5);
            glad_glActiveTexture(GL_TEXTURE5);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(2));

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"ScreenPosZ"),6);
            glad_glActiveTexture(GL_TEXTURE6);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(3));



            // now render stuff
            glad_glBindVertexArray(mesh->VAO);
            glad_glDrawElements(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);

        }

        glad_glDisable(GL_CULL_FACE);
        glad_glCullFace(GL_FRONT_AND_BACK);


        glad_glDepthMask(GL_TRUE);
        glad_glEnable(GL_DEPTH_TEST);


        m_FieldMutex.unlock();

    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


        m_FieldMutex.lock();


        LSMVector3 ulc = LSMVector3(this->m_Field->GetWest(),this->m_Field->GetBottom(),this->m_Field->GetNorth());
        LSMVector3 brc = ulc + LSMVector3(m_Field->GetSizeX(),m_Field->GetSizeZ(),m_Field->GetSizeY());

        float ULCX = s.scr_width * ( ulc.x - s.tlx)/s.width;
        float ULCY = (s.scr_height * (ulc.z - s.tly)/s.height) ;
        float BRCX = s.scr_width * ( brc.x - s.tlx)/s.width;
        float BRCY = (s.scr_height * ( brc.z - s.tly)/s.height) ;

        float width_black = std::max(1.0,2.0 * s.ui_scale2d3d);

        //draw black dot on cursor location
        m->m_ShapePainter->DrawLine(ULCX,ULCY,BRCX,ULCY,width_black,LSMVector4(0.0,0.0,0.0,0.5));
        m->m_ShapePainter->DrawLine(ULCX,ULCY,ULCX,BRCY,width_black,LSMVector4(0.0,0.0,0.0,0.5));
        m->m_ShapePainter->DrawLine(BRCX,BRCY,BRCX,ULCY,width_black,LSMVector4(0.0,0.0,0.0,0.5));
        m->m_ShapePainter->DrawLine(BRCX,BRCY,ULCX,BRCY,width_black,LSMVector4(0.0,0.0,0.0,0.5));


        m_FieldMutex.unlock();


    }

    inline void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {


        /*m_FieldMutex.lock();

        m_Parameters->UpdateParameters();

        std::vector<gl3dMesh *> meshes = m_Actor->GetMeshes();

        for(int i = 0; i < meshes.size(); i++)
        {

            gl3dMesh * mesh = meshes.at(i);

            LSMVector3 camerapos = s.Camera3D->GetPosition();
            LSMVector3 objpos = LSMVector3(this->m_Field->GetWest(),this->m_Field->GetBottom(),this->m_Field->GetNorth());
            LSMMatrix4x4 objrotm = LSMMatrix4x4();
            LSMMatrix4x4 objscale = LSMMatrix4x4();
            objscale.SetScaling(m_Field->GetSizeX(),m_Field->GetSizeZ(),m_Field->GetSizeY());


            LSMMatrix4x4 objtrans;
            objtrans.Translate(LSMVector3(objpos.x - camerapos.x,objpos.y,objpos.z - camerapos.z));
            objrotm = objtrans*objrotm *objscale;

            s.GL_3DFrameBuffer->SetAsTarget();

            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);

            //set shader uniform values
            OpenGLProgram * program = GLProgram_uifield;


            // bind shader
            glad_glUseProgram(program->m_program);

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"OMatrix"),1,GL_FALSE,(float*)(&objrotm));//.GetMatrixDataPtr());

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjTranslate"),objpos.x - camerapos.x,objpos.y,objpos.z  - camerapos.z);
            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjScale"),m_Field->GetSizeX(),m_Field->GetSizeZ(),m_Field->GetSizeY());

            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isSurface"),m_IsSurface? 1:0);

            LSMStyle s = GetStyle();

            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

            int hard_loc = glad_glGetUniformLocation(program->m_program,"hardness");
            glad_glUniform1f(hard_loc,m_Hardness);

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < s.m_ColorGradientb1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(i));
                    ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }


            float hmax = -1e31f;
            float hmin = 1e31f;


            hmax = m_Val_Max;
            hmin = m_Val_Min;

            if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
            {
                hmax = s.m_Intervalb1.GetMax() ;
                hmin = s.m_Intervalb1.GetMin() ;
            }
            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");


            glad_glUniform1f(h_max_loc,hmax);
            glad_glUniform1f(h_min_loc,hmin);


            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_3D,m_Texture->m_texgl);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iTex3D"),0);

            // now render stuff
            glad_glBindVertexArray(mesh->VAO);
            glad_glDrawElements(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);

        }




        m_FieldMutex.unlock();*/


    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_Texture->Destroy();
        delete m_Texture;
        m_IsPrepared = false;
    }


    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm)
    {


        m_IsCRSChanged = false;

    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline double GetMinimumValue(int i) override
    {
        return m_Val_Min;
    }

    inline double GetAverageValue(int i) override
    {
        return m_Val_Avg;
    }

    inline double GetMaximumValue(int i) override
    {
        return m_Val_Max;
    }

    inline float MinimumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        //as fraction of the vertical screen resolution

        //space = 0.5*text

        //space
        ////Text
        //space
        ////Gradient-Text
        ////Gradient
        ////Gradient-Text
        //space

        //need at least 5.5 * text height

        return s.ui_textscale * 12.0 * 5.5/s.scr_height; // 5.5 times the text width
    }

    inline float MaximumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        //as fraction of the vertical screen resolution

        return 1.0; //full height
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s, float posy_start = 0.0, float posy_end = 0.0)
    {
        if(s.legendindex < s.legendtotal)
        {
            float width = s.scr_pixwidth;
            float height = s.scr_pixheight;

            LSMVector4 LineColor = LSMVector4(0.4,0.4,0.4,1.0);
            float linethickness = std::max(1.0f,1.0f*s.ui_scale);
            float framewidth = 0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
            float ui_ticktextscale = s.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

            float legendwidth =s.ui_scalem*0.085 * width ;
            float legendspacing = s.ui_scalem*0.015* width;

            float legendsq_top = framewidth +posy_start;//framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
            float legendsq_bottom = framewidth +posy_end - 2.0 *12.0 * ui_ticktextscale;//legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
            float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
            float legendsq_right = width - legendspacing;

            float space_text = std::max(50.0f,(legendsq_right  - legendsq_left) * 0.60f);

            float legendcsq_right = std::max(legendsq_left + 10.0f,legendsq_left + (legendsq_right - legendsq_left)-space_text);

            m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);


            //Gradient
            m->m_ShapePainter->DrawSquare(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,LSMVector4(0.1,0.1,0.1,1));

            m->m_ShapePainter->DrawSquareGradient(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,&(GetStyleRef()->m_ColorGradientb1));

            //title

            m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_bottom + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);


            //ticks

            LSMVector4 TickColor = LSMVector4(0.2,0.2,0.2,1.0);
            LSMVector4 TickSmallColor = LSMVector4(0.5,0.5,0.5,1.0);
            float tickwidth = 2.0f*s.ui_scale;
            float ticksmallwidth = std::max(2.0f,1.5f * ui_ticktextscale);
            float ticklength = 7.0f*s.ui_scale;
            float ticksmalllength = 4.0f* s.ui_scale;
            float tickdist = 2.0f *s.ui_scale;

            int n_ticks  = 10;

            n_ticks = std::min(10,std::max(2,int(std::fabs(legendsq_top-legendsq_bottom)/(2.0 * 12.0*ui_ticktextscale))));

            double hmax = 0.0;
            double hmin = 0.0;

            hmax = m_Val_Max;
            hmin = m_Val_Min;

            if(!(GetStyle().m_Intervalb1.GetMax()  == 0.0f && GetStyle().m_Intervalb1.GetMin()  == 0.0f) && GetStyle().m_Intervalb1.GetMax() > GetStyle().m_Intervalb1.GetMin() )
            {
                hmax = GetStyle().m_Intervalb1.GetMax() ;
                hmin = GetStyle().m_Intervalb1.GetMin() ;
            }

            for(int i = 0; i < n_ticks + 1; i++)
            {
                double val = hmax + float(i) * (hmin -hmax)/float(n_ticks);
                double y = legendsq_bottom +float(i) * float(legendsq_top -legendsq_bottom)/float(n_ticks);
                m->m_ShapePainter->DrawLine(legendcsq_right,y,legendcsq_right+ticklength + tickdist,y,tickwidth,TickColor);
                m->m_TextPainter->DrawString(QString::number(val),NULL,legendcsq_right + tickdist+ticklength+1,y + (i == n_ticks? 0.0:-9.0f* ui_ticktextscale),LSMVector4(0.0,0.0,0.0,1.0),9 * ui_ticktextscale);

            }
        }
    }

};


#endif // UIFIELDLAYER_H
