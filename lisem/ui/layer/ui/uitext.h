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
    bool m_SizeRel = false;
    LSMVector3 m_Position;
    LSMVector4 m_Color = LSMVector4(0.0,0.0,0.0,1.0);

    double oldtlx = 0.0;
    double oldtly = 0.0;
    double oldbrx = 0.0;
    double oldbry = 0.0;
    double oldresx = 1.0;
    double oldresy = 1.0;

public:
    inline UITextLayer()
    {

    }

    inline UITextLayer(QString text, QString font, LSMVector3 Position, GeoProjection Projection, float size, bool bold, bool italic, bool underline, bool coord_abs = false, bool draw_ontop = false,bool size_rel = false, bool box = false, LSMVector4 box_color = LSMVector4(0.0,0.0,0.0,0.0)) : UIGeoLayer()
    {
        Initialize(text,font,Position,Projection,size,bold,italic,underline,coord_abs,draw_ontop, size_rel,box,box_color);
    }
    inline void Initialize(QString text, QString font, LSMVector3 Position, GeoProjection Projection, float size, bool bold, bool italic, bool underline, bool coord_abs = false, bool draw_ontop = false,bool size_rel = false, bool box = false, LSMVector4 box_color = LSMVector4(0.0,0.0,0.0,0.0))
    {
        UIGeoLayer::Initialize( Projection,BoundingBox(Position),text,false,"",false);

        m_Size = size;
        m_Text= text;
        m_FontName = font;
        m_Position = Position;

    }
    inline void SetText(QString t)
    {
        m_Text = t;
    }

    inline void SetTextColor(LSMVector4 v)
    {
        m_Color = v;

    }
    inline void SetTextPosition(LSMVector3 position, GeoProjection *p)
    {
        m_Position = position;
        SetProjection(*p);
    }
    inline void SetTextSize(float s)
    {
        m_Size = s;
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

    inline virtual void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


        oldtlx = s.tlx;
        oldtly = s.tly;
        oldbrx = s.brx;
        oldbry = s.bry;

        oldresx = s.scr_width;
        oldresy = s.scr_width;


        //get screen pixel position
        //get screen pixel size

        float xpix = s.scr_pixwidth* (float)((m_Position.x - s.tlx)/(s.brx-s.tlx));
        float ypix = s.scr_pixheight* (float)((m_Position.z - s.tly)/(s.bry-s.tly));


        m->m_TextPainter->DrawString(m_Text,NULL,xpix,ypix,LSMVector4(0.0,0.0,0.0,1.0),m_Size);



    }

    inline virtual void OnPostDraw3DUI(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {


        oldtlx = s.tlx;
        oldtly = s.tly;
        oldbrx = s.brx;
        oldbry = s.bry;


        oldresx = s.scr_width;
        oldresy = s.scr_width;

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



    virtual inline bool IsMovable() override
    {
        return true;
    }

    virtual inline bool IsScaleAble() override
    {
        return true;
    }

    inline virtual LSMVector3 GetPosition() override
    {

        if(!m_PositionIsScreenSpace)
        {
            return m_Position;

        }else
        {
            float posx = oldbrx + m_Position.x *(oldbrx-oldtlx);
            float posy = oldbry + m_Position.y *(oldbry-oldtly);
            return LSMVector3(posx,0.0,posy);
        }

    }

    inline virtual LSMVector3 GetScale() override
    {
        float size = m_Text.size() * m_Size * (oldbrx-oldtlx)/oldresx;
        if(m_PositionIsScreenSpace)
        {
            return LSMVector3(size,1.0,size);

        }else
        {
            return LSMVector3(size,1.0,size);

        }
    }

    inline virtual void Scale(LSMVector3 s) override
    {

        m_Size = m_Size * std::sqrt(s.x*s.z);

    }
    inline virtual void Move(LSMVector3 m) override
    {
        if(!m_PositionIsScreenSpace)
        {
            m_Position = m_Position + m;

        }else
        {
            float movex = m.x/std::max(1e-12,(oldtlx-oldbrx));
            float movey = m.y/std::max(1e-12,(oldtly-oldbry));

            m_Position = m_Position + LSMVector3(movex,0.0,movey);
        }
    }
};


#endif // UITEXT_H
