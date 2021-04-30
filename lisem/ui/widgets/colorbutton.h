#ifndef COLORBUTTON_H
#define COLORBUTTON_H


#include "qlabel.h"
#include "linear/lsm_vector4.h"
#include "QMouseEvent"
#include "QColorDialog"
#include "QPainter"
#include "color/colorf.h"

class ColorButton : public QLabel {
    Q_OBJECT

    LSMVector4 m_Color;
public:
    inline explicit ColorButton(QWidget* parent = Q_NULLPTR) : QLabel(" ", parent)
    {
        this->setMinimumSize(20,20);
    }
    inline ~ColorButton()
    {

    }


    inline void paintEvent(QPaintEvent *pe) override
    {
        QRect r = contentsRect();
        r.adjust(margin(), margin(), -margin(), -margin());
        QRect rnew = QRect(0,0,r.width(),r.height());
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(QColor(m_Color.x * 255.0,m_Color.y * 255.0,m_Color.z * 255.0,255));
        painter.setBrush(brush);
        QRect rect = rnew.adjusted(1,1, -1, -1);
        //painter.drawRoundedRect(rect,4,4);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.fillRect(rnew,brush);
        painter.drawRect(rnew);
    }


    inline LSMVector4 GetColor()
    {
        return m_Color;
    }
    inline QColor GetColorQ()
    {
        return QColor(m_Color.x * 255.0,m_Color.y * 255.0,m_Color.z * 255.0,m_Color.w * 255.0);
    }

    inline ColorF GetColorF()
    {
        return ColorF(m_Color.x, m_Color.y,m_Color.z, m_Color.w);
    }

    inline void SetColor(QColor color)
    {
        m_Color.x = ((float)color.red())/255.0;
        m_Color.y = ((float)color.green())/255.0;
        m_Color.z = ((float)color.blue())/255.0;
        m_Color.w = ((float)color.alpha())/255.0;
        this->update();
    }


    inline void SetColor(LSMVector4 c)
    {
        m_Color = c;
        update();
    }

    inline void SetColor(ColorF c)
    {
        m_Color.x = c.r;
        m_Color.y = c.g;
        m_Color.z = c.b;
        m_Color.w = c.a;
        update();
    }


protected:
    inline void mousePressEvent(QMouseEvent* event)
    {
        if(event->buttons() & Qt::LeftButton)
        {
            //do color selection
            QColor color = QColorDialog::getColor(QColor(m_Color.x * 255.0,m_Color.y * 255.0,m_Color.z * 255.0,m_Color.w * 255.0), nullptr,"Choose Color",QColorDialog::ShowAlphaChannel);
            m_Color.x = ((float)color.red())/255.0;
            m_Color.y = ((float)color.green())/255.0;
            m_Color.z = ((float)color.blue())/255.0;
            m_Color.w = ((float)color.alpha())/255.0;
            this->update();

            emit OnColorChanged();
        }
    }


signals:

    inline void OnColorChanged();

};


#endif // COLORBUTTON_H
