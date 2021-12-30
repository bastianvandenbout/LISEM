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
    LSMVector3 Position;

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
        UIGeoLayer::Initialize( Projection,shapes->GetAndCalcBoundingBox(),name,file,filepath,false);

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

    inline QString layertypeName()
    {
        return "TextLayer";
    }

};


#endif // UITEXT_H
