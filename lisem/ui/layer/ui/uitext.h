#ifndef UITEXT_H
#define UITEXT_H

#include "defines.h"
#include "layer/geo/uigeolayer.h"
#include "layer/editors/uivectorlayereditor.h"
#include "extensionprovider.h"
#include "linear/lsm_vector2.h"
#include "gl/openglcldatamanager.h"

class LISEM_API UITextLayer : public UIGeoLayer
{

private:

    QString m_Text = "";
    QString m_FontName = "";
    float m_Size = 11;
    bool m_Bold = false;
    bool m_Italic = false;
    bool m_Underlined = false;

    bool m_DrawOnTop = false;
    bool m_PositionIsScreenSpace = false;
    LSMVector3 m_Position;

public:
    inline UITextLayer()
    {

    }

    inline UITextLayer(QString text, QString font, LSMVector3 Position, GeoProjection Projection, float size, bool bold, bool italic, bool underline) : UIGeoLayer()
    {
        Initialize(text,font,Position,Projection,size,bold,italic,underline);
    }
    inline void Initialize(QString text, QString font, LSMVector3 Position, GeoProjection Projection, float size, bool bold, bool italic, bool underline)
    {
        UIGeoLayer::Initialize( Projection,BoundingBox(Position),text,false,"",false);

        m_Text= text;
        m_FontName = font;
        m_Position = Position;

    }


    /*inline void SaveLayer(QJsonObject * obj) override
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
    }*/


    inline virtual void OnPostDraw3DUI(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {

        /*if(s.GL_FrameBuffer->GetWidth() == 0 ||  s.GL_FrameBuffer->GetHeight() == 0)
        {
            return;
        }

        bool draw_ui = s.draw_ui;
        if(s.is_3d && !do_if_3d)
        {

            draw_ui = false;
        }else
        {
            if(s.is_3d)
            {
                if(s.ScreenPosX == nullptr || s.ScreenPosY == nullptr || s.ScreenPosZ == nullptr )
                {
                    return;

                }
            }
        }



        double unitsm = s.projection.GetUnitMultiplier();
        bool projected = s.projection.IsProjected();
        QString units = s.projection.GetUnitName();

        float width = s.scr_pixwidth;
        float height = s.scr_pixheight;

        float framewidth = s.ui_framewidth;//0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
        float ui_ticktextscale = s.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

        if(!s.is_3d)
        {

        }
        if(draw_ui)
        {

            //m->m_TextPainter->DrawString(QString::number(xtick),NULL,xpix -20.0f,ybottom-tickdist-ticklength-1-1.15f*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);



        }*/
    }


    inline QString layertypeName()
    {
        return "TextLayer";
    }

};


#endif // UITEXT_H
