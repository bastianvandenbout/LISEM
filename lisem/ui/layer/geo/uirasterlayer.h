#pragma once

#include "layer/geo/uigeolayer.h"
#include "layer/editors/uirasterlayereditor.h"
#include "gl/openglcldatamanager.h"

class UIRasterLayerEditor;

class UIRasterLayer : public UIGeoLayer
{
private:


    OpenGLCLTexture * m_Texture = nullptr;
    bool m_createdTexture = false;
    cTMap * m_Map = nullptr;

    OpenGLProgram * m_Program = nullptr;

    bool m_MinMaxSet = false;
    float m_Val_Min = 0.0f;
    float m_Val_Max = 0.0f;
    float m_Val_Avg = 0.0f;

    QList<BoundingBox> m_DataEdits;

public:


    inline UIRasterLayer(cTMap * map, QString name, bool file = false, QString filepath = "", bool native = false) : UIGeoLayer(map->GetProjection(), map->GetBoundingBox(), name,file,filepath,false)
    {
        if(!native)
        {
            m_Editable = true;
        }
        m_IsNative = native;
        m_Map = map;
        m_CouldBeDEM = true;
        m_RequiresCRSGLTransform = true;
        m_Style.m_StyleSimpleGradient = true;
        m_Style.m_StyleSimpleRange = true;
        m_HasLegend = true;
        UpdateMinMax();

    }

    inline UIRasterLayer(cTMap * map, OpenGLCLTexture * m, QString name,  bool file = false, QString filepath = "",bool dynamic = false, bool native = false) : UIGeoLayer(map->GetProjection(),map->GetBoundingBox(), name,file,filepath,true)
    {
        if(!native)
        {
            m_Editable = true;
        }

        m_IsNative = native;
        m_Map = map;
        m_Texture = m;
        m_createdTexture = true;
        m_RequiresCRSGLTransform = true;
        m_Style.m_StyleSimpleGradient = true;
        m_HasLegend = true;
        UpdateMinMax();

    }

    inline ~UIRasterLayer()
    {

    }

    inline QString layertypeName()
    {
        return "RasterLayer";
    }

    inline QList<cTMap *> GetMaps() override
    {

        QList<cTMap *> l = QList<cTMap *>();
        l.append((m_Map));
        return l;
    }

    inline int GetMapCount() override
    {
       return 1;
    }




    inline void UpdateGLData()
    {

        for(int i = 0; i < m_DataEdits.length(); i++)
        {
            BoundingBox replacebb = m_DataEdits.at(i);

            int minr = replacebb.GetMinX();
            int minc = replacebb.GetMinY();
            int sizer = replacebb.GetSizeX();
            int sizec = replacebb.GetSizeY();

            minr = std::max(0,std::min(m_Map->nrRows()-1,minr));
            minc = std::max(0,std::min(m_Map->nrCols()-1,minc));
            sizer = std::max(0,std::min(m_Map->nrRows()-minr,sizer));
            sizec = std::max(0,std::min(m_Map->nrCols()-minc,sizec));


            MaskedRaster<float> datareplace = MaskedRaster<float>(sizer,sizec,0.0,0.0,1.0);

            for(int r = minr; r < minr + sizer; r++)
            {
                for(int c = minc; c < minc + sizec; c++)
                {
                    datareplace[r - minr][c - minc] = m_Map->data[r][c];
                }

            }
            glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
            glad_glTexSubImage2D(GL_TEXTURE_2D,0,minc,minr,sizec,sizer,GL_RED,GL_FLOAT,datareplace.data());
            glad_glBindTexture(GL_TEXTURE_2D,0);
        }

        m_DataEdits.clear();
    }

    inline void UpdateMinMax()
    {


        float hmax = -1e31f;
        float hmin = 1e31f;
        float havg = 0.0f;
        float n = 0.0;

        if(IsDynamic() || !( m_MinMaxSet))
        {

            FOR_ROW_COL_MV(m_Map)
            {
                if(std::isfinite(m_Map->data[r][c]) && !std::isnan((m_Map->data[r][c])))
                {
                    n++;
                    havg += (m_Map->data[r][c]);
                    hmax = std::max(hmax,m_Map->data[r][c]);
                    hmin = std::min(hmin,m_Map->data[r][c]);
                }
            }


            m_Val_Avg = havg/n;
            m_Val_Min = hmin;
            m_Val_Max = hmax;


            m_MinMaxSet = true;
        }
    }

    void Draw_Raster(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm,bool raw_value, float scale = 1.0f)
    {
        bool is_hs = false;
        if(!raw_value)
        {
            is_hs = m_DrawAsHillShade;
        }

        if(m_createdTexture)
        {
            if(IsDraw() && m_Map != nullptr)
            {

                WorldGLTransform * gltransform = tm->Get(state.projection,this->GetProjection());

                //Get style
                LSMStyle s = GetStyle();
                BoundingBox bb = GetBoundingBox();

                float hmax = -1e31f;
                float hmin = 1e31f;
                float havg = 0.0f;
                float n = 0.0;

                UpdateGLData();

                UpdateMinMax();

                hmax = m_Val_Max;
                hmin = m_Val_Min;

                if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                {
                    hmax = s.m_Intervalb1.GetMax() ;
                    hmin = s.m_Intervalb1.GetMin() ;
                }

                //layer geometry
                float l_width = ((float)(m_Map->nrCols()))*m_Map->data.cell_sizeX();
                float l_height = ((float)(m_Map->nrRows()))*m_Map->data.cell_sizeY();
                float l_cx = m_Map->data.west() + 0.5f * l_width;
                float l_cy = m_Map->data.north()+ 0.5f * l_height;

                //set shader uniform values
                OpenGLProgram * program = GLProgram_uimap;

                // bind shader
                glad_glUseProgram(program->m_program);
                // get uniform locations

                int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                int tex_loc = glad_glGetUniformLocation(program->m_program,"tex");
                int tex_x_loc = glad_glGetUniformLocation(program->m_program,"texX");
                int tex_y_loc = glad_glGetUniformLocation(program->m_program,"texY");
                int islegend_loc = glad_glGetUniformLocation(program->m_program,"is_legend");
                int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");
                int istransformed_loc = glad_glGetUniformLocation(program->m_program,"is_transformed");
                int istransformedf_loc = glad_glGetUniformLocation(program->m_program,"is_transformedf");
                int israw_loc = glad_glGetUniformLocation(program->m_program,"is_raw");
                int ishs_loc = glad_glGetUniformLocation(program->m_program,"is_hs");

                glad_glUniform1i(ishs_loc,is_hs? 1:0);
                glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_ldd"),m_Map->AS_IsLDD? 1:0);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizex"),l_width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_sizey"),l_height);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixx"),(float)(m_Map->nrCols()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixy"),(float)(m_Map->nrRows()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdx"),(float)(m_Map->cellSizeX()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_pixdy"),(float)(m_Map->cellSizeY()));
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transx"),l_cx);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ul_transy"),l_cy);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrwidth"),state.scr_width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_scrheight"),state.scr_height);

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


                // bind texture
                glad_glUniform1i(tex_loc,0);
                glad_glActiveTexture(GL_TEXTURE0);
                glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);

                if(gltransform != nullptr)
                {
                    if(!gltransform->IsGeneric())
                    {
                        BoundingBox b = gltransform->GetBoundingBox();

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizex"),b.GetSizeX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_sizey"),b.GetSizeY());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transx"),b.GetCenterX());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"tr_transy"),b.GetCenterY());

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

                if(raw_value)
                {
                    glad_glUniform1i(tex_loc,0);
                    glad_glActiveTexture(GL_TEXTURE0);
                    glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
                    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"zscale"),scale);

                    glad_glUniform1i(israw_loc,1);
                }else {
                    glad_glUniform1i(israw_loc,0);
                }


                glad_glUniform1f(h_max_loc,hmax);
                glad_glUniform1f(h_min_loc,hmin);

                // set project matrix
                glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                glad_glUniform1i(islegend_loc,0);
                glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                // now render stuff
                glad_glBindVertexArray(m->m_Quad->m_vao);
                glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
                glad_glBindVertexArray(0);

                if(raw_value)
                {
                    glad_glBindTexture(GL_TEXTURE_2D,m_Texture->m_texgl);
                    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                }
            }
        }

    }

    virtual void OnDrawGeoElevation(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        Draw_Raster(m,s,tm,true,s.projection.GetUnitZMultiplier());
    }



    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        Draw_Raster(m,s,tm,false);
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        if(!m_createdTexture)
        {
            if(m_Map != nullptr)
            {
                OpenGLCLTexture * _T = m->GetMCLGLTexture(&(this->m_Map->data),false,false,true);
                m_Texture = _T;
                m_createdTexture = true;
            }

        }

        m_IsPrepared = true;
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s)
    {
        if(s.legendindex < s.legendtotal)
        {
            float width = s.scr_pixwidth;
            float height = s.scr_pixheight;

            LSMVector4 LineColor = LSMVector4(0.4,0.4,0.4,1.0);
            float linethickness = std::max(1.0f,1.0f*s.ui_scale);
            float framewidth = 0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
            float ui_ticktextscale = s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

            float legendwidth =s.ui_scalem*0.085 * width ;
            float legendspacing = s.ui_scalem*0.015* width;

            float legendsq_top = framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
            float legendsq_bottom = legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
            float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
            float legendsq_right = width - legendspacing;

            float space_text = std::max(50.0f,(legendsq_right  - legendsq_left) * 0.60f);

            float legendcsq_right = std::max(legendsq_left + 10.0f,legendsq_left + (legendsq_right - legendsq_left)-space_text);

            m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);

            //Gradient
            m->m_ShapePainter->DrawSquare(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,LSMVector4(0.1,0.1,0.1,1));

            m->m_ShapePainter->DrawSquareGradient(legendsq_left,legendsq_bottom, legendcsq_right -legendsq_left,legendsq_top-legendsq_bottom,&(GetStyleRef()->m_ColorGradientb1));

            //title

            m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_bottom + 3 +  2 * s.ui_scale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);


            //ticks

            LSMVector4 TickColor = LSMVector4(0.2,0.2,0.2,1.0);
            LSMVector4 TickSmallColor = LSMVector4(0.5,0.5,0.5,1.0);
            float tickwidth = 2.0f*s.ui_scale;
            float ticksmallwidth = std::max(2.0f,1.5f * ui_ticktextscale);
            float ticklength = 7.0f*s.ui_scale;
            float ticksmalllength = 4.0f* s.ui_scale;
            float tickdist = 2.0f *s.ui_scale;

            int n_ticks  = 10;

            n_ticks = std::min(10,std::max(2,int(0.2 * std::fabs(legendsq_top-legendsq_bottom)/9.0 * ui_ticktextscale)));

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

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        m_IsCRSChanged = false;
    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
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

    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        //convert location to local coordinate system
        GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        LSMVector2 locn = transformer->Transform(Pos);
        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();
        if(b.Contains(locn.x,locn.y))
        {
            p.hit = true;

            float rel_fracx = (locn.x - b.GetMinX())/b.GetSizeX();
            float rel_fracy = 1.0-(locn.y - b.GetMinY())/b.GetSizeY();

            int r = (float)(m_Map->nrRows()) * rel_fracy;
            int c = (float)(m_Map->nrCols()) * rel_fracx;
            float valraw = m_Map->data[r][c];

            QString value = QString::number(valraw);
            if(pcr::isMV(valraw))
            {
                value = "Missing Value";
            }

            //finally get a cell location
            p.AttrNames.append(QString("Raster Value"));
            p.AttrValues.append(value);
        }

        return p;
    }

    inline LayerInfo GetInfo()
    {
        LayerInfo l;
        l.Add("Type","Multi-Band Raster Layer");
        l.Add("FileName",m_File);
        l.Add("Name",m_Name);
        l.Add(UIGeoLayer::GetInfo());
        l.Add("Nr of Rows", QString::number(m_Map->nrRows()));
        l.Add("Nr of Rows", QString::number(m_Map->nrRows()));
        l.Add("Nr of Cols", QString::number(m_Map->nrCols()));
        l.Add("CellSize x", QString::number(m_Map->cellSizeX()));
        l.Add("CellSize y", QString::number(m_Map->cellSizeY()));

        return l;
    }

    inline cTMap * GetMap()
    {
        return m_Map;
    }
    inline OpenGLCLTexture * GetMainTexture()
    {
        return m_Texture;
    }

    inline void NotifyDataChange(BoundingBox b)
    {
        //b should contain row and column bounds to replace
        m_DataEdits.append(b);
        m_MinMaxSet = false;

    }
    UILayerEditor* GetEditor();


};
