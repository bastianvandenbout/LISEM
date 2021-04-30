#pragma once

#include "defines.h"
#include "layer/geo/uigeolayer.h"
#include "layer/editors/uivectorlayereditor.h"
#include "extensionprovider.h"
#include "linear/lsm_vector2.h"
#include "gl/openglcldatamanager.h"

class UIVectorLayerEditor;


class LISEM_API UIVectorLayer : public UIGeoLayer
{

public:
    ShapeFile * m_Shapes = nullptr;
private:
    QList<GLGeometryList *> m_GLGeometryList;
    QList<GLGeometryList *> m_GLTRGeometryList;
    QList<OpenGLGeometry *> m_GLPointSimpleList;
    QList<OpenGLGeometry *> m_GLPointFillList;
    QList<OpenGLGeometry *> m_GLPointLineList;
    QList<OpenGLGeometry *> m_GLLineLineList;
    QList<OpenGLGeometry *> m_GLPolygonFillList;
    QList<OpenGLGeometry *> m_GLPolygonLineList;

    bool m_UnitShapesPrepared = false;

    GLGeometryList * m_UnitPoint;
    OpenGLGeometry * m_GLUnitPointl;
    OpenGLGeometry * m_GLUnitPointf;
    GLGeometryList * m_UnitLine;
    OpenGLGeometry * m_GLUnitLinel;
    GLGeometryList * m_UnitSquare;
    OpenGLGeometry * m_GLUnitSquarel;
    OpenGLGeometry * m_GLUnitSquaref;

    bool m_createdGeometry = false;
    bool m_DrawPoints = false;
    bool m_ShapesChanged = false;
    OpenGLProgram * m_Program = nullptr;

    QString m_n_pf;
    QString m_n_pl;
    QString m_n_ll;
    QString m_n_pof;
    QString m_n_pol;

    LSMInterval m_in_pf;
    LSMInterval m_in_pl;
    LSMInterval m_in_ll;
    LSMInterval m_in_pof;
    LSMInterval m_in_pol;

public:

    inline UIVectorLayer()
    {

    }

    inline UIVectorLayer(ShapeFile * shapes, QString name, bool file = false, QString filepath = "", bool native = false) : UIGeoLayer()
    {
        Initialize(shapes,name,file,filepath,native);
    }

    inline void Initialize(ShapeFile * shapes, QString name, bool file = false, QString filepath = "", bool native = false)
    {
        UIGeoLayer::Initialize(shapes->GetProjection(),shapes->GetAndCalcBoundingBox(),name,file,filepath,false);
        m_IsLayerSaveAble = file;
        m_IsNative = native;
        m_Shapes = shapes;
        m_RequiresCRSGLTransform = false;
        m_Editable = true;
        m_ShapesChanged = true;
        m_Style.m_StyleSimpleGradient = false;
        m_Style.m_StyleSimpleRange = false;
        m_IsPrepared = false;
        m_HasLegend = true;

    }
    inline ~UIVectorLayer()
    {

    }



    inline void SaveLayer(QJsonObject * obj) override
    {
        if(!m_IsNative && m_IsFile)
        {
            LSMSerialize::FromString(obj,"FilePath",m_File);
            LSMSerialize::FromString(obj,"Name",m_Name);
            LSMSerialize::FromString(obj,"ShapeFile",m_File);
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

        ShapeFile * _M;
        bool error = false;
        try
        {
            _M = LoadVector(path);
        }
        catch (int e)
        {
           error = true;
        }

        if(!error && _M != nullptr)
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

    inline QString layertypeName()
    {
        return "VectorLayer";
    }

    inline ShapeFile * GetShapeFile()
    {
        return m_Shapes;
    }


    inline QList<ShapeFile*> GetShapeFiles() override
    {
        QList<ShapeFile *> l = QList<ShapeFile *>();
        l.append(m_Shapes);
        return l;

    }

    inline bool IsSaveAble() override
    {
        return true;
    }
    inline virtual bool SaveLayerToFile(QString filepath)
    {
        SaveVector(m_Shapes,filepath);

        return true;
    }

    inline QList<QString> GetSaveExtHint()
    {
        return GetShapeExtensions();
    }






    inline void SetDrawPoints(bool x)
    {
        m_DrawPoints = x;
    }

    inline void OnDraw(OpenGLCLManager * m, GeoWindowState state) override
    {
        if(!GetCurrentAttributeNames(m_Style.m_PointFillStyle.m_Color1Parameter,m_Style.m_PointLineStyle.m_Color1Parameter,m_Style.m_LineLineStyle.m_Color1Parameter,m_Style.m_PolygonFillStyle.m_Color1Parameter,m_Style.m_PolygonLineStyle.m_Color1Parameter))
        {
            std::cout << "attributes changed " << std::endl;
            m_ShapesChanged = true;
        }
        if(m_ShapesChanged)
        {
            DestroyGLBuffers();
            DestroyShapeData();
            GetShapeData();
            TransformShapeData(state);
            CreateGLBuffers();
            m_ShapesChanged = false;
        }
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState state, WorldGLTransformManager * tm) override
    {

        LSMStyle s = GetStyle();


        glad_glDisable(GL_DEPTH_TEST);

        for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
        {
            GLGeometryList *GLGeometryRaw = m_GLGeometryList.at(i);

            QList<OpenGLGeometry *> glgeomlist = {m_GLPointFillList.at(i),m_GLPointLineList.at(i),m_GLLineLineList.at(i),m_GLPolygonLineList.at(i),m_GLPolygonFillList.at(i)};
            QList<OpenGLProgram *> programlist = {GLProgram_uivectorp,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorp};
            QList<bool> is_line_list = {false,true,true,true,false};
            QList<bool> is_point_part_list = {true,true,false,false,false};
            QList<LSMLineStyle> ls_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
            QList<SPHFillStyle> fs_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};

            QList<LSMInterval> intervals = {m_in_pf, m_in_pl, m_in_ll, m_in_pol, m_in_pof};

            for(int j = glgeomlist.length()-1; j > -1; j--)
            {
                LSMLineStyle ls = ls_list.at(j);
                SPHFillStyle fs = fs_list.at(j);
                bool is_line = is_line_list.at(j);
                bool is_point_part = is_point_part_list.at(j);
                OpenGLGeometry * GLGeometry = glgeomlist.at(j);
                if(GLGeometry != nullptr)
                {
                    OpenGLProgram * program = programlist.at(j);
                    LSMInterval interval = intervals.at(j);

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

                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_legend"),0);

                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizex"),state.scr_pixwidth);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowpixsizey"),state.scr_pixheight);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizex"),state.width);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowsizey"),state.height);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowhwration"),state.scr_ratio);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransx"),state.translation_x);
                    glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"ug_windowtransy"),state.translation_y);

                    if(is_line)
                    {
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),ls.m_Color1.r,ls.m_Color1.g,ls.m_Color1.b,ls.m_HasBackGroundColor? ls.m_Color1.a : 0.0f);
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),ls.m_Color2.r,ls.m_Color2.g,ls.m_Color2.b,ls.m_Color2.a);
                        float*pat = new float[16];
                        for(int k = 0; k < 16; k++)
                        {
                            pat[k] = 0.0f;
                        }
                        float pat_total = 0.0f;
                        for(int k = 0; k < ls.pattern.length(); k++)
                        {
                            if(ls.pattern.at(k) > 0)
                            {
                                pat_total += ls.pattern.at(k);
                                pat[k] = pat_total;
                            }
                            if(k == 15)
                            {
                                break;
                            }
                        }
                        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"pattern"),1,GL_FALSE,pat);

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"patterntotal"),pat_total);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),ls.m_LineWMax * state.ui_scale2d3d);
                        delete[] pat;

                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_AttributeColor"),ls.m_Color1Parameter.isEmpty()?0:1);

                        float amin = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMin(): interval.GetMin();
                        float amax = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMax(): interval.GetMax();

                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMin"),amin);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMax"),amax);

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                            if(i < ls.m_ColorGradient1.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,ls.m_ColorGradient1.m_Gradient_Stops.at(i));
                                ColorF c = ls.m_ColorGradient1.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                            if(i < ls.m_ColorGradient2.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,ls.m_ColorGradient2.m_Gradient_Stops.at(i));
                                ColorF c = ls.m_ColorGradient2.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }

                    }else {

                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),fs.m_Color1.r,fs.m_Color1.g,fs.m_Color1.b,fs.m_HasBackGroundColor? fs.m_Color1.a : 0.0f);
                        glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),fs.m_Color2.r,fs.m_Color2.g,fs.m_Color2.b,(fs.m_IsLines || fs.m_IsShapes)?fs.m_Color2.a : 0.0f);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),fs.m_LineSize * state.ui_scale2d3d);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineangle"),fs.m_LineAngle * state.ui_scale2d3d);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineseperation"),fs.m_LineSeperation * state.ui_scale2d3d);
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"type"),fs.m_IsLines?1:0);

                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_AttributeColor"),fs.m_Color1Parameter.isEmpty()?0:1);

                        float amin = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMin(): interval.GetMin();
                        float amax = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMax(): interval.GetMax();


                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMin"),amin);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMax"),amax);

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                            if(i < fs.m_ColorGradient1.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,fs.m_ColorGradient1.m_Gradient_Stops.at(i));
                                ColorF c = fs.m_ColorGradient1.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }

                        for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
                        {
                            QString is = QString::number(i);
                            int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                            int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                            if(i < fs.m_ColorGradient2.m_Gradient_Stops.length())
                            {
                                glad_glUniform1f(colorstop_i_loc,fs.m_ColorGradient2.m_Gradient_Stops.at(i));
                                ColorF c = fs.m_ColorGradient2.m_Gradient_Colors.at(i);
                                glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                            }else {
                                glad_glUniform1f(colorstop_i_loc,1e30f);
                                glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                            }
                        }
                    }

                    glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);

                    if(is_point_part)
                    {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"relative_size"),s.m_PointSize);
                    }else {
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
                    }

                    //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

                    glad_glBindVertexArray(GLGeometry->m_vao);
                    glad_glDrawElements(GL_TRIANGLES,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
                    glad_glBindVertexArray(0);

                    //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                }
            }
        }


        if(m_DrawPoints)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {

                    {

                        OpenGLGeometry * GLGeometry = m_GLPointSimpleList.at(i);
                        if(GLGeometry != nullptr)
                        {
                            OpenGLProgram * program = GLProgram_uipointsimple;

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

                            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),1.0,0.0,0.0,1.0);
                            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),1.0,0.0,0.0,1.0);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),1 * state.ui_scale2d3d);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineangle"),1 * state.ui_scale2d3d);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineseperation"),1 * state.ui_scale2d3d);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"type"),0);

                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);

                            glad_glPointSize(4);
                            glad_glBindVertexArray(GLGeometry->m_vao);
                            glad_glDrawElements(GL_POINTS,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
                            glad_glBindVertexArray(0);
                        }

                    }
                }
        }

    }

    inline void CreateUnitShapes(OpenGLCLManager * m)
    {
        if(m_UnitShapesPrepared == false)
        {
            /*GLGeometryList * m_UnitPoint;
            OpenGLGeometry * m_GLUnitPointl;
            OpenGLGeometry * m_GLUnitPointf;
            GLGeometryList * m_UnitLine;
            GLGeometryList * m_GLUnitLinel;
            GLGeometryList * m_UnitSquare;
            GLGeometryList * m_GLUnitSquarel;
            GLGeometryList * m_GLUnitSquaref;*/

            {
                ShapeLayer * point = new ShapeLayer();
                Feature * pf = new Feature();
                LSMPoint * ps = new LSMPoint(0.5,0.5);
                pf->AddShape(ps);
                point->AddFeature(pf);
                m_UnitPoint = point->GetGLGeometryList();
                point->Destroy();
                delete point;
            }

            {
                ShapeLayer * point = new ShapeLayer();
                Feature * pf = new Feature();
                LSMPolygon * ps = new LSMPolygon();
                Ring * r = new Ring();
                r->AddVertex(0.0,0.0);
                r->AddVertex(0.0,1.0);
                r->AddVertex(1.0,1.0);
                r->AddVertex(1.0,0.0);
                r->AddVertex(0.0,0.0);
                ps->SetExtRing(r);
                pf->AddShape(ps);
                point->AddFeature(pf);
                m_UnitSquare = point->GetGLGeometryList();
                point->Destroy();
                delete point;
            }

            {
                ShapeLayer * point = new ShapeLayer();
                Feature * pf = new Feature();
                LSMLine * ps = new LSMLine();
                ps->AddVertex(0.0,0.5);
                ps->AddVertex(1.0,0.5);
                pf->AddShape(ps);
                point->AddFeature(pf);
                m_UnitLine = point->GetGLGeometryList();
                point->Destroy();
                delete point;
            }

            GLGeometryList * GLGeometry = m_UnitPoint;


                OpenGLGeometry * geom = new OpenGLGeometry();
                geom->CreateFromLinesWithOrigin(GLGeometry->Point_Lines_x.data(),GLGeometry->Point_Lines_y.data(),GLGeometry->Point_Lines_connect.data(),GLGeometry->Point_Lines_rx.data(),GLGeometry->Point_Lines_ry.data(),GLGeometry->Point_Lines_x.size(),1,true,GLGeometry->Point_Lines_attr.data(),GLGeometry->Point_Lines_index.data());
                m_GLUnitPointl = geom;

                geom = new OpenGLGeometry();
                geom->CreateFromTrianglesWithOrigin(GLGeometry->Point_Triangles_x.data(),GLGeometry->Point_Triangles_y.data(),GLGeometry->Point_Triangles_rx.data(),GLGeometry->Point_Triangles_ry.data(),GLGeometry->Point_Triangles_x.size(),true, GLGeometry->Point_Triangles_attr.data(),GLGeometry->Point_Triangles_index.data());
                m_GLUnitPointf = geom;

                GLGeometry = m_UnitLine;

                geom = new OpenGLGeometry();
                geom->CreateFromLines(GLGeometry->Line_Lines_x.data(),GLGeometry->Line_Lines_y.data(),GLGeometry->Line_Lines_connect.data(),GLGeometry->Line_Lines_x.size(),1,true,GLGeometry->Line_Lines_attr.data(),GLGeometry->Line_Lines_index.data());
                m_GLUnitLinel = geom;


                GLGeometry = m_UnitSquare;

                geom = new OpenGLGeometry();
                geom->CreateFromLines(GLGeometry->Polygon_Lines_x.data(),GLGeometry->Polygon_Lines_y.data(),GLGeometry->Polygon_Lines_connect.data(),GLGeometry->Polygon_Lines_x.size(),1,true,GLGeometry->Polygon_Lines_attr.data(),GLGeometry->Polygon_Lines_index.data());
                m_GLUnitSquarel = geom;

                geom = new OpenGLGeometry();
                geom->CreateFromTriangles(GLGeometry->Polygon_Triangles_x.data(),GLGeometry->Polygon_Triangles_y.data(),GLGeometry->Polygon_Triangles_x.size(),true, GLGeometry->Polygon_Triangles_attr.data(),GLGeometry->Polygon_Triangles_index.data());
                m_GLUnitSquaref = geom;

                m_UnitShapesPrepared = true;
        }

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        CreateUnitShapes(m);

        m_IsPrepared = true;
    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        UIGeoLayer::OnCRSChanged(m,s,tm);

        if(m_ShapesChanged)
        {
            DestroyGLBuffers();
            DestroyShapeData();
            GetShapeData();
            TransformShapeData(s);
            CreateGLBuffers();
            m_ShapesChanged = false;
        }

        TransformShapeData(s);
        DestroyGLBuffers();
        CreateGLBuffers();
        m_IsCRSChanged = false;
    }

    inline void DestroyShapeData()
    {
        if(m_Shapes!= nullptr)
        {
            for(int i = 0; i < m_GLGeometryList.length(); i++)
            {
                if(m_GLGeometryList.at(i) != nullptr)
                {
                    delete m_GLGeometryList.at(i);
                }
            }
            m_GLGeometryList.clear();
            for(int i = 0; i < m_GLTRGeometryList.length(); i++)
            {
                if(m_GLTRGeometryList.at(i) != nullptr)
                {
                    delete m_GLTRGeometryList.at(i);
                }
            }
            m_GLTRGeometryList.clear();
        }
    }

    inline void GetShapeData()
    {
        if(m_Shapes!= nullptr)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {
                ShapeLayer * l = m_Shapes->GetLayer(i);
                GLGeometryList *GLGeometry = l->GetGLGeometryList();
                GLGeometryList *GLTRGeometry = GLGeometry->CopyGeomOnly();
                m_GLGeometryList.append(GLGeometry);
                m_GLTRGeometryList.append(GLTRGeometry);
            }
        }
    }

    inline void TransformShapeData(GeoWindowState s)
    {


            //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(GetProjection(),s.projection);

            //if(!transformer->IsGeneric())
            {
                for(int i = 0; i < m_GLTRGeometryList.length(); i++)
                {

                    m_GLTRGeometryList.at(i)->CopyGeomOnlyFrom(m_GLGeometryList.at(i));
                    m_GLTRGeometryList.at(i)->Transform(m_Transformerinv);
                }
            }
    }

    inline void DestroyGLBuffers()
    {
        for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
        {
            GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);

            if(m_GLPointSimpleList.at(i) != nullptr)
            {
                m_GLPointSimpleList.at(i)->Destroy();
                delete m_GLPointSimpleList.at(i);
            }
            m_GLPointSimpleList.clear();
            if(m_GLPointLineList.at(i) != nullptr)
            {
                m_GLPointLineList.at(i)->Destroy();
                delete m_GLPointLineList.at(i);
            }
            m_GLPointLineList.clear();
            if(m_GLPointFillList.at(i) != nullptr)
            {
                m_GLPointFillList.at(i)->Destroy();
                delete m_GLPointFillList.at(i);
            }
            m_GLPointFillList.clear();
            if(m_GLLineLineList.at(i) != nullptr)
            {
                m_GLLineLineList.at(i)->Destroy();
                delete m_GLLineLineList.at(i);
            }
            m_GLLineLineList.clear();
            if(m_GLPolygonLineList.at(i) != nullptr)
            {
                m_GLPolygonLineList.at(i)->Destroy();
                delete m_GLPolygonLineList.at(i);
            }
            m_GLPolygonLineList.clear();
            if(m_GLPolygonFillList.at(i) != nullptr)
            {
                m_GLPolygonFillList.at(i)->Destroy();
                delete m_GLPolygonFillList.at(i);
            }
            m_GLPolygonFillList.clear();
        }
        m_createdGeometry = false;
    }

    inline void SetCurrentAttributeNames(QString n_pf, QString n_pl, QString n_ll, QString n_pof, QString n_pol)
    {
        m_n_pf = n_pf;
        m_n_pl = n_pl;
        m_n_ll = n_ll;
        m_n_pof = n_pof;
        m_n_pol = n_pol;
    }

    inline void SetCurrentAttributeIntervals(LSMInterval n_pf,LSMInterval n_pl,LSMInterval n_ll,LSMInterval n_pof,LSMInterval n_pol)
    {
        m_in_pf = n_pf;
        m_in_pl = n_pl;
        m_in_ll = n_ll;
        m_in_pof = n_pof;
        m_in_pol = n_pol;
    }

    inline bool GetCurrentAttributeNames(QString n_pf, QString n_pl, QString n_ll, QString n_pof, QString n_pol)
    {
        return ((m_n_pf == n_pf) && (m_n_pl == n_pl) && (m_n_ll == n_ll) && (m_n_pof == n_pof) && (m_n_pol == n_pol));
    }
    inline void CreateGLBuffers()
    {
        if(!m_createdGeometry)
        {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {
                GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);
                m_Shapes->GetLayer(i)->SetGeometryLayerAttributes(GLGeometry,m_Style.m_PointFillStyle.m_Color1Parameter,m_Style.m_PointLineStyle.m_Color1Parameter,m_Style.m_LineLineStyle.m_Color1Parameter,m_Style.m_PolygonFillStyle.m_Color1Parameter,m_Style.m_PolygonLineStyle.m_Color1Parameter);

                SetCurrentAttributeNames(m_Style.m_PointFillStyle.m_Color1Parameter,m_Style.m_PointLineStyle.m_Color1Parameter,m_Style.m_LineLineStyle.m_Color1Parameter,m_Style.m_PolygonFillStyle.m_Color1Parameter,m_Style.m_PolygonLineStyle.m_Color1Parameter);
                SetCurrentAttributeIntervals(
                            m_Shapes->GetLayer(i)->GetAttributeInterval(m_Style.m_PointFillStyle.m_Color1Parameter),
                            m_Shapes->GetLayer(i)->GetAttributeInterval(m_Style.m_PointLineStyle.m_Color1Parameter),
                            m_Shapes->GetLayer(i)->GetAttributeInterval(m_Style.m_LineLineStyle.m_Color1Parameter),
                            m_Shapes->GetLayer(i)->GetAttributeInterval(m_Style.m_PolygonFillStyle.m_Color1Parameter),
                            m_Shapes->GetLayer(i)->GetAttributeInterval(m_Style.m_PolygonLineStyle.m_Color1Parameter)
                            );

                if(GLGeometry->VertexPoint_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromPoints(GLGeometry->VertexPoint_x.data(),GLGeometry->VertexPoint_y.data(),GLGeometry->VertexPoint_x.size(),true,GLGeometry->Vertex_Point_attr.data(),GLGeometry->Vertex_Point_index.data());
                    m_GLPointSimpleList.append(geom);
                }else {
                    m_GLPointSimpleList.append(nullptr);
                }

                if(GLGeometry->Point_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLinesWithOrigin(GLGeometry->Point_Lines_x.data(),GLGeometry->Point_Lines_y.data(),GLGeometry->Point_Lines_connect.data(),GLGeometry->Point_Lines_rx.data(),GLGeometry->Point_Lines_ry.data(),GLGeometry->Point_Lines_x.size(),1,true,GLGeometry->Point_Lines_attr.data(),GLGeometry->Point_Lines_index.data());
                    m_GLPointLineList.append(geom);
                }else {
                    m_GLPointLineList.append(nullptr);
                }
                if(GLGeometry->Point_Triangles_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromTrianglesWithOrigin(GLGeometry->Point_Triangles_x.data(),GLGeometry->Point_Triangles_y.data(),GLGeometry->Point_Triangles_rx.data(),GLGeometry->Point_Triangles_ry.data(),GLGeometry->Point_Triangles_x.size(),true, GLGeometry->Point_Triangles_attr.data(),GLGeometry->Point_Triangles_index.data());
                    m_GLPointFillList.append(geom);

                }else {
                    m_GLPointFillList.append(nullptr);
                }
                if(GLGeometry->Line_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLines(GLGeometry->Line_Lines_x.data(),GLGeometry->Line_Lines_y.data(),GLGeometry->Line_Lines_connect.data(),GLGeometry->Line_Lines_x.size(),1,true,GLGeometry->Line_Lines_attr.data(),GLGeometry->Line_Lines_index.data());
                    m_GLLineLineList.append(geom);
                }else {
                    m_GLLineLineList.append(nullptr);
                }
                if(GLGeometry->Polygon_Lines_x.size() > 0)
                {
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromLines(GLGeometry->Polygon_Lines_x.data(),GLGeometry->Polygon_Lines_y.data(),GLGeometry->Polygon_Lines_connect.data(),GLGeometry->Polygon_Lines_x.size(),1,true,GLGeometry->Polygon_Lines_attr.data(),GLGeometry->Polygon_Lines_index.data());
                    m_GLPolygonLineList.append(geom);
                }else {
                    m_GLPolygonLineList.append(nullptr);
                }
                if(GLGeometry->Polygon_Triangles_x.size() > 0)
                {
                    std::cout << "create polygon triangles" << std::endl;
                    OpenGLGeometry * geom = new OpenGLGeometry();
                    geom->CreateFromTriangles(GLGeometry->Polygon_Triangles_x.data(),GLGeometry->Polygon_Triangles_y.data(),GLGeometry->Polygon_Triangles_x.size(),true, GLGeometry->Polygon_Triangles_attr.data(),GLGeometry->Polygon_Triangles_index.data());
                    m_GLPolygonFillList.append(geom);
                }else {
                    m_GLPolygonFillList.append(nullptr);
                }
            }
        }else {
            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {

                GLGeometryList *GLGeometry = m_GLTRGeometryList.at(i);
                m_Shapes->GetLayer(i)->SetGeometryLayerAttributes(GLGeometry,m_Style.m_PointFillStyle.m_Color1Parameter,m_Style.m_PointLineStyle.m_Color1Parameter,m_Style.m_LineLineStyle.m_Color1Parameter,m_Style.m_PolygonFillStyle.m_Color1Parameter,m_Style.m_PolygonLineStyle.m_Color1Parameter);
                SetCurrentAttributeNames(m_Style.m_PointFillStyle.m_Color1Parameter,m_Style.m_PointLineStyle.m_Color1Parameter,m_Style.m_LineLineStyle.m_Color1Parameter,m_Style.m_PolygonFillStyle.m_Color1Parameter,m_Style.m_PolygonLineStyle.m_Color1Parameter);

                if(m_GLPointSimpleList.at(i) != nullptr)
                {
                    m_GLPointSimpleList.at(i)->FillFromPoints(GLGeometry->VertexPoint_x.data(),GLGeometry->VertexPoint_y.data(),GLGeometry->VertexPoint_x.size(),true,GLGeometry->Vertex_Point_attr.data(),GLGeometry->Vertex_Point_index.data());
                }
                if(m_GLPointLineList.at(i) != nullptr)
                {
                    m_GLPointLineList.at(i)->FillFromLinesWithOrigin(GLGeometry->Point_Lines_x.data(),GLGeometry->Point_Lines_y.data(),GLGeometry->Point_Lines_connect.data(),GLGeometry->Point_Lines_rx.data(),GLGeometry->Point_Lines_ry.data(),GLGeometry->Point_Lines_x.size(),1,true,GLGeometry->Point_Lines_attr.data(),GLGeometry->Point_Lines_index.data());
                }
                if(m_GLPointFillList.at(i) != nullptr)
                {
                    m_GLPointFillList.at(i)->FillFromTrianglesWithOrigin(GLGeometry->Point_Triangles_x.data(),GLGeometry->Point_Triangles_y.data(),GLGeometry->Point_Triangles_rx.data(),GLGeometry->Point_Triangles_ry.data(),GLGeometry->Point_Triangles_x.size(),true, GLGeometry->Point_Triangles_attr.data(),GLGeometry->Point_Triangles_index.data());
                }
                if(m_GLLineLineList.at(i) != nullptr)
                {
                    m_GLLineLineList.at(i)->FillFromLines(GLGeometry->Line_Lines_x.data(),GLGeometry->Line_Lines_y.data(),GLGeometry->Line_Lines_connect.data(),GLGeometry->Line_Lines_x.size(),1,true,GLGeometry->Line_Lines_attr.data(),GLGeometry->Line_Lines_index.data());
                }
                if(m_GLPolygonLineList.at(i) != nullptr)
                {
                    std::cout << "refill polygon lines " << std::endl;
                    m_GLPolygonLineList.at(i)->FillFromLines(GLGeometry->Polygon_Lines_x.data(),GLGeometry->Polygon_Lines_y.data(),GLGeometry->Polygon_Lines_connect.data(),GLGeometry->Polygon_Lines_x.size(),1,true,GLGeometry->Polygon_Lines_attr.data(),GLGeometry->Polygon_Lines_index.data());
                }
                if(m_GLPolygonFillList.at(i) != nullptr)
                {
                    std::cout << "refill polygon triangles " << std::endl;
                    m_GLPolygonFillList.at(i)->FillFromTriangles(GLGeometry->Polygon_Triangles_x.data(),GLGeometry->Polygon_Triangles_y.data(),GLGeometry->Polygon_Triangles_x.size(),true, GLGeometry->Polygon_Triangles_attr.data(),GLGeometry->Polygon_Triangles_index.data());
                }

            }
        }


        m_createdGeometry = true;
    }

    inline void DrawFeature(Feature * s,GeoWindowState state, OpenGLCLManager * m, LSMVector4 color = LSMVector4(1.0,1.0,1.0,0.3), LSMVector2 offset = LSMVector2(0.0,0.0))
    {
        if(s == nullptr)
        {
            return;
        }

        for(int i =0; i < s->GetShapeCount(); i++)
        {
            DrawShape(s->GetShape(i),state,m,color, offset);
        }
    }

    inline void DrawShape(LSMShape * s,GeoWindowState state,OpenGLCLManager * m,LSMVector4 color = LSMVector4(1.0,1.0,1.0,0.3), LSMVector2 offset = LSMVector2(0.0,0.0))
    {
        if(s == nullptr)
        {
            return;
        }

        GLGeometryList * gl = new GLGeometryList();
        Feature::AddShapeToGlGeometryList(gl,0,s);
        gl->ApplyOffset(offset);

        //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(GetProjection(),state.projection);

        gl->Transform(m_Transformerinv);
        gl->TransformToScreen(BoundingBox(state.tlx,state.brx,state.tly,state.bry),state.scr_pixwidth,state.scr_pixheight);

        m->m_ShapePainter->DrawPolygon(gl->Polygon_Triangles_x.data(),gl->Polygon_Triangles_y.data(),gl->Polygon_Triangles_x.size(),color);
        m->m_ShapePainter->DrawLines(gl->Polygon_Lines_x.data(),gl->Polygon_Lines_y.data(),5,gl->Polygon_Lines_x.size(),color);
        m->m_ShapePainter->DrawLines(gl->Line_Lines_x.data(),gl->Line_Lines_y.data(),5,gl->Line_Lines_x.size(),color);

        for(int i = 0; i < gl->PointPoint_x.size(); i++)
        {
            m->m_ShapePainter->DrawSquare(gl->PointPoint_x.at(i) -5,gl->PointPoint_y.at(i) -5,10,10,color);
        }
        delete gl;

    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {
        DestroyShapeData();
        DestroyGLBuffers();

        m_IsPrepared = false;
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }

    inline void DrawUnitShapeF(OpenGLCLManager * m, GeoWindowState state, OpenGLGeometry * shape, float ulx, float uly, float sx, float sy, LSMStyle &style, SPHFillStyle &fillstyle,float attr_val, float attr_min = 0.0, float attr_max = 1.0)
    {


        LSMStyle s = style;


        glad_glDisable(GL_DEPTH_TEST);

        //LSMLineStyle ls = ls_list.at(j);
        SPHFillStyle fs = fillstyle;
        bool is_point_part = false;
        OpenGLGeometry * GLGeometry = shape;
        OpenGLProgram * program  = GLProgram_uivectorp;
        LSMInterval interval = LSMInterval(0.0,1.0);//not used in shader

        // bind shader
        glad_glUseProgram(program->m_program);


        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_legend"),1);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_ulx"),ulx);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_uly"),uly);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_sx"),sx);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_sy"),sy);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_attr"),attr_val);

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


            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),fs.m_Color1.r,fs.m_Color1.g,fs.m_Color1.b,fs.m_HasBackGroundColor? fs.m_Color1.a : 0.0f);
            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),fs.m_Color2.r,fs.m_Color2.g,fs.m_Color2.b,(fs.m_IsLines || fs.m_IsShapes)?fs.m_Color2.a : 0.0f);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),fs.m_LineSize * state.ui_scale2d3d);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineangle"),fs.m_LineAngle * state.ui_scale2d3d);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"lineseperation"),fs.m_LineSeperation * state.ui_scale2d3d);
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"type"),fs.m_IsLines?1:0);

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_AttributeColor"),fs.m_Color1Parameter.isEmpty()?0:1);

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMin"),attr_min);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMax"),attr_max);

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < fs.m_ColorGradient1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,fs.m_ColorGradient1.m_Gradient_Stops.at(i));
                    ColorF c = fs.m_ColorGradient1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                if(i < fs.m_ColorGradient2.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,fs.m_ColorGradient2.m_Gradient_Stops.at(i));
                    ColorF c = fs.m_ColorGradient2.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }


        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);

        if(is_point_part)
        {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"relative_size"),s.m_PointSize);
        }else {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
        }

        //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        glad_glBindVertexArray(GLGeometry->m_vao);
        glad_glDrawElements(GL_TRIANGLES,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);

        //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );*/

    }

    inline void DrawUnitShapeL(OpenGLCLManager * m, GeoWindowState state, OpenGLGeometry * shape, float ulx, float uly, float sx, float sy, LSMStyle &style, LSMLineStyle & linestyle, float attr_val, float attr_min = 0.0, float attr_max = 1.0)
    {
        LSMStyle s = style;


        glad_glDisable(GL_DEPTH_TEST);

        //LSMLineStyle ls = ls_list.at(j);
        LSMLineStyle ls = linestyle;
        bool is_line = true;
        bool is_point_part = false;
        OpenGLGeometry * GLGeometry = shape;
        OpenGLProgram * program = GLProgram_uivectorl;
        LSMInterval interval = LSMInterval(0.0,1.0);

        // bind shader
        glad_glUseProgram(program->m_program);

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_legend"),1);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_ulx"),ulx);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_uly"),uly);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_sx"),sx);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_sy"),sy);
        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"legend_attr"),attr_val);

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

            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color1"),ls.m_Color1.r,ls.m_Color1.g,ls.m_Color1.b,ls.m_HasBackGroundColor? ls.m_Color1.a : 0.0f);
            glad_glUniform4f(glad_glGetUniformLocation(program->m_program,"Color2"),ls.m_Color2.r,ls.m_Color2.g,ls.m_Color2.b,ls.m_Color2.a);
            float*pat = new float[16];
            for(int k = 0; k < 16; k++)
            {
                pat[k] = 0.0f;
            }
            float pat_total = 0.0f;
            for(int k = 0; k < ls.pattern.length(); k++)
            {
                if(ls.pattern.at(k) > 0)
                {
                    pat_total += ls.pattern.at(k);
                    pat[k] = pat_total;
                }
                if(k == 15)
                {
                    break;
                }
            }
            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"pattern"),1,GL_FALSE,pat);

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"patterntotal"),pat_total);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"linewidth"),ls.m_LineWMax * state.ui_scale2d3d);
            delete[] pat;

            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_AttributeColor"),ls.m_Color1Parameter.isEmpty()?0:1);

            //float amin = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMin(): interval.GetMin();
            //float amax = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMax(): interval.GetMax();

            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMin"),attr_min);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"AttributeColorMax"),attr_max);

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop_c"+ is).toStdString().c_str());

                if(i < ls.m_ColorGradient1.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,ls.m_ColorGradient1.m_Gradient_Stops.at(i));
                    ColorF c = ls.m_ColorGradient1.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }

            for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
            {
                QString is = QString::number(i);
                int colorstop_i_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_"+ is).toStdString().c_str());
                int colorstop_ci_loc = glad_glGetUniformLocation(program->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

                if(i < ls.m_ColorGradient2.m_Gradient_Stops.length())
                {
                    glad_glUniform1f(colorstop_i_loc,ls.m_ColorGradient2.m_Gradient_Stops.at(i));
                    ColorF c = ls.m_ColorGradient2.m_Gradient_Colors.at(i);
                    glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
                }else {
                    glad_glUniform1f(colorstop_i_loc,1e30f);
                    glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
                }
            }

        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);

        if(is_point_part)
        {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),is_point_part?1:0);
            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"relative_size"),s.m_PointSize);
        }else {
            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"is_relative"),0);
        }

        //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        glad_glBindVertexArray(GLGeometry->m_vao);
        glad_glDrawElements(GL_TRIANGLES,GLGeometry->m_indexlength,GL_UNSIGNED_INT,0);
        glad_glBindVertexArray(0);

        //glad_glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );*/
    }

    inline float MinimumLegendHeight(OpenGLCLManager * m, GeoWindowState state) override
    {
        //as fraction of the vertical screen resolution

        //space = 0.5*text

        //for each layer
        //space
        ////Text
        //half space
        ////icon-Text
        //half-space
        ////icon-Text
        //half-space
        ////icon-Text
        //space

        //need at least 2.5 * text height + 1.25 * number of items (5 for a gradient, 1 otherwise)

        float nitems = 0.0;
        float layers = 0;

        LSMStyle s = GetStyle();

        //check all the layers, and their elements
        for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
        {
            bool layer_has_elements = false;
            GLGeometryList *GLGeometryRaw = m_GLGeometryList.at(i);

            QList<OpenGLGeometry *> glgeomlist = {m_GLPointFillList.at(i),m_GLPointLineList.at(i),m_GLLineLineList.at(i),m_GLPolygonLineList.at(i),m_GLPolygonFillList.at(i)};
            QList<OpenGLProgram *> programlist = {GLProgram_uivectorp,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorp};
            QList<bool> is_line_list = {false,true,true,true,false};
            QList<bool> is_point_part_list = {true,true,false,false,false};
            QList<bool> is_endof_element = {false,true,true,false,true};
            QList<LSMLineStyle> ls_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
            QList<SPHFillStyle> fs_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};
            QList<LSMLineStyle> lso_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
            QList<SPHFillStyle> fso_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};

            QList<LSMInterval> intervals = {m_in_pf, m_in_pl, m_in_ll, m_in_pol, m_in_pof};

            for(int j = glgeomlist.length()-1; j > -1; j--)
            {
                LSMLineStyle ls = ls_list.at(j);
                SPHFillStyle fs = fs_list.at(j);

                LSMLineStyle ls_other = lso_list.at(j);
                SPHFillStyle fs_other = fso_list.at(j);
                bool has_other = (j != 2);
                bool is_line = is_line_list.at(j);
                bool is_endof = is_endof_element.at(j);
                bool is_point_part = is_point_part_list.at(j);
                OpenGLGeometry * GLGeometry = glgeomlist.at(j);

                if(GLGeometry != nullptr)
                {
                    //this geometry is not null, so this means this kind of shape exists


                    if(is_line)
                    {
                        if(is_endof)
                        {
                            if(ls.m_Color1Parameter.isEmpty() && (!has_other || fs_other.m_Color1Parameter.isEmpty()))
                            {
                                nitems ++;
                            }else
                            {
                                nitems += 5;
                            }
                        }
                    }else
                    {
                        if(is_endof)
                        {
                            if(fs.m_Color1Parameter.isEmpty()&& (!has_other || ls_other.m_Color1Parameter.isEmpty()))
                            {
                                nitems ++;
                            }else
                            {
                                nitems += 5;
                            }
                        }
                    }

                    if(!layer_has_elements )
                    {
                        layer_has_elements = true;
                        layers ++;
                    }
                }
            }
        }


        return state.ui_textscale *12.0 * (2.25 * layers  + 1.25 * nitems + 1.5)/state.scr_height; // 5.5 times the text width
    }

    inline float MaximumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        //as fraction of the vertical screen resolution

        return MinimumLegendHeight(m,s); //same as minimum height
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState state, float posy_start = 0.0, float posy_end = 0.0)
    {

        float width = state.scr_pixwidth;
        float height = state.scr_pixheight;

        LSMVector4 LineColor = LSMVector4(0.4,0.4,0.4,1.0);
        float linethickness = std::max(1.0f,1.0f*state.ui_scale);
        float framewidth = 0.65*state.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
        float ui_ticktextscale = state.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

        float legendwidth =state.ui_scalem*0.085 * width ;
        float legendspacing = state.ui_scalem*0.015* width;


        float legendsq_top = framewidth +posy_end - 2.0 *12.0 * ui_ticktextscale;//framewidth + (float(s.legendindex)/float(s.legendtotal)) * (height - 2.0 * framewidth) +  (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.025;
        float legendsq_bottom = framewidth +posy_start;//legendsq_top + (1.0/float(s.legendtotal)) * (height - 2.0 * framewidth) * 0.975 - 15*s.ui_scale;
        float legendsq_left = width - framewidth - 0.65 * legendwidth + legendspacing;
        float legendsq_right = width - legendspacing;

        //m->m_ShapePainter->DrawSquareLine(legendsq_left,legendsq_bottom, legendsq_right -legendsq_left,legendsq_top-legendsq_bottom,linethickness,LineColor);

        m->m_TextPainter->DrawString(this->m_Name,NULL,legendsq_left - legendwidth * 0.2 ,legendsq_top + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);

        float actposy = legendsq_top - 1.5*12 * ui_ticktextscale;

        //if(state.legendindex < state.legendtotal)
        {
            LSMStyle s = GetStyle();


            glad_glDisable(GL_DEPTH_TEST);

            for(int i = 0; i < m_Shapes->GetLayerCount(); i++)
            {
                GLGeometryList *GLGeometryRaw = m_GLGeometryList.at(i);

                QList<OpenGLGeometry *> glgeomactlist = {m_GLPointFillList.at(i),m_GLPointLineList.at(i),m_GLLineLineList.at(i),m_GLPolygonLineList.at(i),m_GLPolygonFillList.at(i)};
                QList<OpenGLGeometry *> glgeomlist = {m_GLUnitPointf,m_GLUnitPointl,m_GLUnitLinel,m_GLUnitSquarel,m_GLUnitSquaref};
                QList<OpenGLProgram *> programlist = {GLProgram_uivectorp,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorl,GLProgram_uivectorp};
                QList<bool> is_line_list = {false,true,true,true,false};
                QList<bool> is_point_part_list = {true,true,false,false,false};
                QList<LSMLineStyle> ls_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
                QList<SPHFillStyle> fs_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};
                QList<LSMLineStyle> lso_list = {s.m_PointLineStyle,s.m_PointLineStyle,s.m_LineLineStyle,s.m_PolygonLineStyle,s.m_PolygonLineStyle};
                QList<SPHFillStyle> fso_list = {s.m_PointFillStyle,s.m_PointFillStyle,s.m_PointFillStyle,s.m_PolygonFillStyle,s.m_PolygonFillStyle};
                QList<bool> is_endof_element = {false,true,true,false,true};
                QList<bool> is_new_element = {false,true,true,false,true};

                QList<LSMInterval> intervals = {m_in_pf, m_in_pl, m_in_ll, m_in_pol, m_in_pof};
                QList<LSMInterval> intervalsother = {m_in_pl, m_in_pf, m_in_ll, m_in_pof, m_in_pol};

                for(int j = 0; j < glgeomlist.length(); j++)
                {
                    LSMLineStyle ls = ls_list.at(j);
                    SPHFillStyle fs = fs_list.at(j);
                    bool is_line = is_line_list.at(j);
                    bool is_endof =is_endof_element.at(j);
                    bool is_point_part = is_point_part_list.at(j);
                    OpenGLGeometry * GLGeometrySF = glgeomactlist.at(j);
                    OpenGLGeometry * GLGeometry = glgeomlist.at(j);
                    bool has_other = (j != 2);
                    LSMLineStyle ls_other = lso_list.at(j);
                    SPHFillStyle fs_other = fso_list.at(j);
                    LSMInterval interval = intervals.at(j);
                    LSMInterval intervalother = intervalsother.at(j);


                    if(GLGeometrySF != nullptr)
                    {
                        //draw the relevant element for the legend

                        if(is_line)
                        {

                            float amin = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMin(): interval.GetMin();
                            float amax = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMax(): interval.GetMax();

                            float amino = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMin(): intervalother.GetMin();
                            float amaxo = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMax(): intervalother.GetMax();

                            if(ls.m_Color1Parameter.isEmpty() && (!has_other || fs_other.m_Color1Parameter.isEmpty()))
                            {
                                DrawUnitShapeL(m,state,GLGeometry,legendsq_left,actposy,20,12 *ui_ticktextscale ,m_Style,ls,amin + (float)(i)*(amax-amin)/4.0, amin,amax);
                                if(is_endof)
                                {
                                    actposy -= 12 *ui_ticktextscale * 1.25;
                                }
                            }else
                            {
                                //gradient of fill style, draw 5 items with same style
                                if(ls.m_Color1Parameter.isEmpty())
                                {
                                    float actposytemp = actposy;

                                    for(int j = 0; j < 5; j++)
                                    {
                                        DrawUnitShapeL(m,state,GLGeometry,legendsq_left,actposytemp,20,12 *ui_ticktextscale ,m_Style,ls,interval.GetMin() + (float)(j)*(interval.GetMax() - interval.GetMin())/4.0, amin,amax);
                                        m->m_TextPainter->DrawString(QString::number(amino + (float)(j)*(amaxo - amino)/4.0),NULL,legendsq_left + 25 ,actposytemp + 2 + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);

                                        actposytemp -= 12.0 *ui_ticktextscale * 1.25;
                                    }

                                    if(is_endof)
                                    {
                                        actposy -= 5.0 * (12 *ui_ticktextscale * 1.25);
                                    }
                                //gradient of line style, draw 5 items with increasing attribute style
                                }else
                                {
                                    //get attribute range
                                    float actposytemp = actposy;

                                    for(int j = 0; j < 5; j++)
                                    {
                                        DrawUnitShapeL(m,state,GLGeometry,legendsq_left,actposytemp,20,12 *ui_ticktextscale ,m_Style,ls,amin + (float)(j)*(amax - amin)/4.0, amin,amax);
                                        m->m_TextPainter->DrawString(QString::number(interval.GetMin() + (float)(j)*(interval.GetMax() - interval.GetMin())/5.0),NULL,legendsq_left + 25 ,actposytemp + 0.5*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);

                                        actposytemp -= 12.0 *ui_ticktextscale * 1.25;
                                    }

                                    if(is_endof)
                                    {
                                        actposy -= 5.0 * (12 *ui_ticktextscale * 1.25);
                                    }
                                }

                            }
                        }else
                        {
                            float amin = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMin(): interval.GetMin();
                            float amax = !(fs.m_Color1Interval.GetMin() == 0.0f && fs.m_Color1Interval.GetMax() == 0.0f)? fs.m_Color1Interval.GetMax(): interval.GetMax();

                            float amino = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMin(): intervalother.GetMin();
                            float amaxo = !(ls.m_Color1Interval.GetMin() == 0.0f && ls.m_Color1Interval.GetMax() == 0.0f)? ls.m_Color1Interval.GetMax(): intervalother.GetMax();

                            if(fs.m_Color1Parameter.isEmpty() && (!has_other || ls_other.m_Color1Parameter.isEmpty()))
                            {
                                DrawUnitShapeF(m,state,GLGeometry,legendsq_left,actposy,20,12 *ui_ticktextscale,m_Style,fs,amin + (float)(j)*(amax-amin)/4.0,amin,amax);
                                if(is_endof)
                                {
                                    actposy -= 12 *ui_ticktextscale * 1.25;
                                }
                            }else
                            {
                                //gradient of fill style, draw 5 items with same style
                                //if(fs.m_Color1Parameter.isEmpty())
                                {
                                    float actposytemp = actposy;

                                    for(int j = 0; j < 5; j++)
                                    {
                                        DrawUnitShapeF(m,state,GLGeometry,legendsq_left,actposytemp,20,12 *ui_ticktextscale,m_Style,fs,amin + (float)(j)*(amax-amin)/4.0, amin,amax);
                                        actposytemp -= 12.0 *ui_ticktextscale * 1.25;
                                    }

                                    if(is_endof)
                                    {
                                        actposy -= 5.0 * (12 *ui_ticktextscale * 1.25);
                                    }
                                }

                            }
                        }


                    }
                }

                //finished layer
                actposy -= 12 *ui_ticktextscale * 0.5;
            }
        }
    }


    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        LayerProbeResult p;
        p.hit = false;
        //convert location to local coordinate system
        //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(proj,GetProjection());

        LSMVector2 locn = m_Transformer->Transform(Pos);


        //then if it is within the bounding box of the raster
        BoundingBox b = GetBoundingBox();
        if(b.Contains(locn.x,locn.y))
        {
            QList<Feature *> features = m_Shapes->GetFeaturesAt(locn, tolerence);

            if(features.length() > 0)
            {
                MatrixTable *t = m_Shapes->GetFeatureAttributes(features.at(0));
                p.hit = true;
                for(int i = 0; i < t->GetNumberOfRows(); i++)
                {
                    p.AttrNames.append(t->GetValueStringQ(i,0));
                    p.AttrValues.append(t->GetValueStringQ(i,1));
                }
                delete t;
            }
        }

        return p;
    }

    inline void NotifyDataChange()
    {
        m_ShapesChanged = true;
    }


    inline virtual QList<QString> GetAllAttributes()
    {
        QList<QString> names;
        if(m_Shapes->GetLayerCount() > 0)
        {
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                names.append(m_Shapes->GetLayer(0)->GetAttributeName(i));
            }

        }
        return names;
    }

    inline virtual QList<QString> GetNumberAttributes()
    {
        QList<QString> names;
        if(m_Shapes->GetLayerCount() > 0)
        {
            for(int i = 0; i < m_Shapes->GetLayer(0)->GetNumberOfAttributes(); i++)
            {
                if(m_Shapes->GetLayer(0)->GetAttributeList(i)->isNumberType())
                {
                    names.append(m_Shapes->GetLayer(0)->GetAttributeName(i));
                }

            }

        }
        return names;
    }

    UILayerEditor* GetEditor();
};
