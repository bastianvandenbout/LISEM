#ifndef UIMODELPOINTLAYER_H
#define UIMODELPOINTLAYER_H

#include "layer/geo/uigeolayer.h"
#include "iostream"
#include "gl/openglcldatamanager.h"

class UIModelPointLayer : public UIGeoLayer
{
    OpenGLCLDataBuffer<float> *m_Data = nullptr;
    bool m_Created = false;
    float m_ShiftX = 0.0;
    float m_ShiftY = 0.0;
public:

    inline UIModelPointLayer(OpenGLCLDataBuffer<float> *data, float shiftx, float shifty) : UIGeoLayer(GeoProjection::GetGeneric(), BoundingBox(0,1,0,1), "Model Points Layer",false,"Model",false)
    {
        m_ShiftX = shiftx;
        m_ShiftY = shifty;
        m_Data = data;
        m_IsNative = true;
        m_IsModel = true;
        m_Created = true;
    }


    inline ~UIModelPointLayer()
    {

    }

    inline QString layertypeName()
    {
        return "ModelPointLayer";
    }


    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {


        if(m_Created)
        {
            if(IsDraw() && m_Data != nullptr)
            {

                LSMStyle s = GetStyle();

                glad_glDisable(GL_DEPTH_TEST);


                OpenGLProgram * program = GLProgram_particles;

                // bind shader
                glad_glUseProgram(program->m_program);

                int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
                int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
                int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
                int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");

                glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
                glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());

                float hmax = 1.0;
                float hmin = 0.0;
                if(!(s.m_Intervalb1.GetMax()  == 0.0f && s.m_Intervalb1.GetMin()  == 0.0f) && s.m_Intervalb1.GetMax()  > s.m_Intervalb1.GetMin() )
                {
                    hmax = s.m_Intervalb1.GetMax() ;
                    hmin = s.m_Intervalb1.GetMin() ;
                }

                glad_glUniform1f(h_max_loc,hmax);
                glad_glUniform1f(h_min_loc,hmin);

                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x- m_ShiftX);
                glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y- m_ShiftY);

                for(int k = 0; k <LISEM_GRADIENT_NCOLORS; k++)
                {
                    QString is = QString::number(k);
                    int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                    int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                    if(k < s.m_ColorGradientb1.m_Gradient_Stops.length())
                    {
                        glad_glUniform1f(colorstop_i_loc,s.m_ColorGradientb1.m_Gradient_Stops.at(k));
                        ColorF c = s.m_ColorGradientb1.m_Gradient_Colors.at(k);
                        glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                    }else {
                        glad_glUniform1f(colorstop_i_loc,1e30f);
                        glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                    }
                }

                // now render stuff
                glad_glPointSize(5);

                glad_glBindVertexArray(m_Data->m_vao);
                glad_glDrawArrays(GL_POINTS,0,m_Data->GetDrawDim());
                glad_glBindVertexArray(0);
            }
        }
    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {


        m_IsPrepared = true;
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



    inline LayerInfo GetInfo()
    {
        LayerInfo l;
        l.Add("Type","Model Point Layer");
        l.Add("FileName",m_File);
        l.Add("Name",m_Name);
        l.Add(UIGeoLayer::GetInfo());

        return l;
    }
};


#endif // UIMODELPOINTLAYER_H
