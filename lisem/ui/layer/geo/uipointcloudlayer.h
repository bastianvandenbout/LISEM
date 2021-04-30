#ifndef UIPOINTCLOUDLAYER_H
#define UIPOINTCLOUDLAYER_H

#include "layer/geo/uigeolayer.h"
#include "geo/pointcloud/pointcloud.h"
#include "openglgeometry.h"
#include "gl/openglcldatamanager.h"

class UIPointCloudLayer : public UIGeoLayer
{

private:

    bool m_Created = false;
    PointCloud * m_PointCloud = nullptr;
    OpenGLGeometry * m_Geometry = nullptr;

    //local copy we keep for visual changes and for CRS transforms
    std::vector<float> m_PosX;
    std::vector<float> m_PosY;
    std::vector<float> m_PosZ;
    std::vector<float> m_ColorR;
    std::vector<float> m_ColorG;
    std::vector<float> m_ColorB;

public:

    inline UIPointCloudLayer()
    {

    }

    inline UIPointCloudLayer(PointCloud * p, QString name, bool file = false, QString filepath = "",bool native = false) : UIGeoLayer()
    {

        Initialize(p,name,file,filepath,native);


    }

    inline void Initialize(PointCloud * p, QString name, bool file = false, QString filepath = "",bool native = false)
    {

        UIGeoLayer::Initialize(p->GetProjection(), p->GetBoundingBox(), name,file,filepath,false);

        m_IsLayerSaveAble = true;
        m_IsNative = native;
        m_IsModel = false;
        m_Created = true;


        m_PointCloud = p;
    }

    inline void SaveLayer(QJsonObject * obj) override
    {
        if(!m_IsNative && m_IsFile)
        {
            LSMSerialize::FromString(obj,"FilePath",m_File);
            LSMSerialize::FromString(obj,"Name",m_Name);
            LSMSerialize::FromString(obj,"PointCloudFile",m_File);
            QJsonObject obj_style;
            m_Style.Save(&obj_style);

            obj->insert("Style",obj_style);

            QString proj;
            LSMSerialize::ToString(obj,"CRS",proj);
            m_Projection.FromString(proj);

        }
    }

    inline void RestoreLayer(QJsonObject * obj)
    {

        QString path;
        QString name;
        LSMSerialize::ToString(obj,"ShapeFile",path);
        LSMSerialize::ToString(obj,"Name",name);

        PointCloud * _M;
        bool error = false;
        try
        {
            _M = ReadPointCloud(path);
        }
        catch (int e)
        {
           error = true;
        }

        if(!error)
        {
            Initialize(_M,name,true,path,false);

            QJsonValue sval = (*obj)["Style"];
            if(sval.isObject() && !sval.isUndefined())
            {
                LSMStyle st;
                QJsonObject o =sval.toObject();
                st.Restore(&o);
                this->m_Style.CopyFrom(st);
            }

            QString proj;
            LSMSerialize::ToString(obj,"CRS",proj);
            GeoProjection p;
            p.FromString(proj);
            this->SetProjection(p);
        }else
        {
            m_Exists = false;
        }
    }



    inline ~UIPointCloudLayer()
    {

    }

    inline QString layertypeName()
    {
        return "PointCloudLayer";
    }


    inline void OnDraw(OpenGLCLManager * m,GeoWindowState s) override
    {



    }

    inline void OnDraw3D(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {
        //if((state.is_3d))
        {
            glad_glDepthMask(GL_TRUE);
            glad_glEnable(GL_DEPTH_TEST);

            LSMStyle s = GetStyle();


            OpenGLProgram * program = GLProgram_pointcloud3d;

            // bind shader
            glad_glUseProgram(program->m_program);

            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");

            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,state.Camera3D->GetProjectionMatrix().GetMatrixDataPtr());

            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());
            glad_glUniform1f(h_max_loc,1.0f);
            glad_glUniform1f(h_min_loc,0.0f);


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
            glad_glPointSize(3);

            glad_glBindVertexArray(m_Geometry->m_vao);
            glad_glDrawArrays(GL_POINTS,0,m_Geometry->m_indexlength);
            glad_glBindVertexArray(0);
        }



    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {
        if(!(state.is_3d))
        {

            LSMStyle s = GetStyle();

            glad_glDisable(GL_DEPTH_TEST);


            OpenGLProgram * program = GLProgram_pointcloud;

            // bind shader
            glad_glUseProgram(program->m_program);

            int h_max_loc = glad_glGetUniformLocation(program->m_program,"h_max");
            int h_min_loc = glad_glGetUniformLocation(program->m_program,"h_min");
            int mat_loc = glad_glGetUniformLocation(program->m_program,"matrix");
            int alpha_loc = glad_glGetUniformLocation(program->m_program,"alpha");

            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);
            glad_glUniform1f(alpha_loc,1.0f-s.GetTransparancy());
            glad_glUniform1f(h_max_loc,1.0f);
            glad_glUniform1f(h_min_loc,0.0f);

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);

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
            glad_glPointSize(3);

            glad_glBindVertexArray(m_Geometry->m_vao);
            glad_glDrawArrays(GL_POINTS,0,m_Geometry->m_indexlength);
            glad_glBindVertexArray(0);
        }

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_PosX.resize(m_PointCloud->GetPointCount());
        m_PosY.resize(m_PointCloud->GetPointCount());
        m_PosZ.resize(m_PointCloud->GetPointCount());

        m_ColorR.resize(m_PointCloud->GetPointCount());
        m_ColorG.resize(m_PointCloud->GetPointCount());
        m_ColorB.resize(m_PointCloud->GetPointCount());

        for(int i = 0; i < m_PointCloud->GetPointCount(); i++)
        {
            m_PosX[i] = m_PointCloud->m_PosX[i];
            m_PosY[i] = m_PointCloud->m_PosY[i];
            m_PosZ[i] = m_PointCloud->m_PosZ[i];
            if(m_PointCloud->HasColor())
            {
                m_ColorR[i] = m_PointCloud->m_ColorR[i];
                m_ColorG[i] = m_PointCloud->m_ColorG[i];
                m_ColorB[i] = m_PointCloud->m_ColorB[i];
            }
        }


        m_Geometry = new OpenGLGeometry();
        m_Geometry->CreateFrom3DPointsWithColor(m_PosX.data(),m_PosY.data(),m_PosZ.data(),m_ColorR.data(),m_ColorG.data(),m_ColorB.data(),m_PointCloud->GetPointCount(),nullptr,nullptr);

        m_IsPrepared = true;
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState s, float posy_start = 0.0, float posy_end = 0.0)
    {



    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        //if the conversion to the new crs is not a generic one


        //convert all points to new CRS



    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {

        //delete geometry

        m_Geometry->Destroy();
        delete m_Geometry;
        m_Geometry = nullptr;
    }

    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {


        return LayerProbeResult();
    }

    inline LayerInfo GetInfo()
    {
        return LayerInfo();
    }
};


#endif // UIPOINTCLOUDLAYER_H
