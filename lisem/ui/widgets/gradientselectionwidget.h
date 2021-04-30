#ifndef GRADIENTSELECTIONWIDGET_H
#define GRADIENTSELECTIONWIDGET_H


#include "QWidget"
#include "QLineEdit"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include "QLabel"
#include "QDialog"
#include "QDoubleSpinBox"
#include "QPushButton"
#include "color/colorf.h"
#include "QPainter"
#include "QColorDialog"
#include "iostream"
#include "color/lsmcolorgradient.h"
#include "math.h"

class GradientWidget: public QWidget
{
    Q_OBJECT;

public:

    QList<ColorF> colors;
    QList<float> stops;

    int dragindex;
    bool m_drag;
    float m_dragXOrg;
    float m_dragYOrg;
    float m_dragX;
    float m_dragY;

    int selectorwidth = 3;

    inline GradientWidget(QList<float> in_stops, QList<ColorF> in_colors,QWidget * p = nullptr) : QWidget(p)
    {
        colors = in_colors;
        stops = in_stops;

    }


    inline void paintEvent(QPaintEvent *pe) override
    {

        QRect rectall = contentsRect();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);

        QRect rect = rectall.adjusted(1,1, -1, -1);
        QLinearGradient gradient(QPointF(rect.top(),rect.left()), QPointF(rect.right(),rect.bottom())); // diagonal gradient from top-left to bottom-right

        for(int i = 0; i < stops.length(); i++)
        {
            if(stops.at(i) > 1.00001f)
            {
                break;
            }
            gradient.setColorAt(stops[i],QColor(255.0f*colors[i].r,255.0f*colors[i].g,255.0f*colors[i].b,255.0f*colors[i].a));

        }

        painter.fillRect(rect,gradient);

        painter.drawRect(rect);

        for(int i = 0; i < stops.length(); i++)
        {
            if(stops.at(i) > 1.00001f)
            {
                break;
            }

            float center = float(rect.left()) + stops.at(i) * float(rect.right() - rect.left());

            painter.setPen(QPen(Qt::white, 3));
            painter.setBrush(QColor(255.0f*colors[i].r,255.0f*colors[i].g,255.0f*colors[i].b,255.0f*colors[i].a));
            painter.drawRoundedRect(QRect(QPoint(center - selectorwidth ,rect.top()+10),QPoint(center + selectorwidth ,rect.bottom()-10)),selectorwidth ,selectorwidth );

            painter.setBrush(QColor(0,0,0,0));


            painter.setPen(QPen(Qt::black,2));
            if(dragindex == i)
            {
                painter.setPen(QPen(Qt::black,3));
            }
            painter.setBrush(QColor(0,0,0,0));
            painter.drawRoundedRect(QRect(QPoint(center - selectorwidth ,rect.top()+10 ),QPoint(center + 5,rect.bottom()-10)),selectorwidth ,selectorwidth );




        }
    }

    inline int GetIndexOfStopAt(QPointF pos)
    {
        QRect rectall = contentsRect();
        QRect rect = rectall.adjusted(1,1, -1, -1);
        for(int i = 0; i < stops.length(); i++)
        {
            if(stops.at(i) > 1.0000f)
            {
                break;
            }

            float center = float(rect.left()) + stops.at(i) * float(rect.right() - rect.left());

            if(QRect(QPoint(center - selectorwidth ,rect.top()+10 ),QPoint(center + selectorwidth ,rect.bottom()-10)).contains(pos.toPoint()))
            {
                return i;
            }
        }
        return -1;
    }

    inline void	mouseDoubleClickEvent(QMouseEvent *event) override
    {

        QPointF pos = event->localPos();
        float x = event->pos().x();
        float y = event->pos().y();
        QRect rectall = contentsRect();
        QRect rect = rectall.adjusted(1,1, -1, -1);
        float stop = x/(float(rect.right()-rect.left()));

        if(m_drag == false)
        {
            bool placed = false;
            for(int i = -1; i < stops.length()-1; i++)
            {
                if(stops.at(i) > 1.00001f)
                {
                    break;
                }
                if(stop < stops.at(i+1))
                {
                    placed = true;
                    stops.insert(i+1,stop);
                    colors.insert(i+1,LSMColorGradient(stops,colors).ColorAt(stop));
                    break;
                }
            }
            if(!placed)
            {
                stops.append(stop);
                colors.append(LSMColorGradient(stops,colors).ColorAt(stop));
            }

            emit OnGradientChanged();
        }

        update();

    }
    inline void	mouseMoveEvent(QMouseEvent *event) override
    {
        QPointF pos = event->localPos();
        float x = event->pos().x();
        float y = event->pos().y();

        QRect rectall = contentsRect();
        QRect rect = rectall.adjusted(1,1, -1, -1);
        if(m_drag)
        {
            if(rect.contains(pos.toPoint()))
            {
                float dx = x -m_dragX;
                float dy = y -m_dragY;

                m_dragY = y;
                m_dragX = x;

                float stop_new = std::max(0.0f,std::min(1.0f,stops.at(dragindex) + dx/(float(rect.right()-rect.left()))));
                ColorF color = colors.at(dragindex);

                stops.removeAt(dragindex);
                colors.removeAt(dragindex);

                bool found = false;
                for(int i = 0; i < stops.length(); i++)
                {
                    if(!(stop_new > stops.at(i)))
                    {
                        found = true;
                        stops.insert(i,stop_new);
                        colors.insert(i,color);
                        dragindex = i;
                        break;
                    }
                }
                if(!found)
                {
                    dragindex = stops.length();

                    stops.append(stop_new);
                    colors.append(color);
                }

                emit OnGradientChanged();

            }else {
            }
        }

        this->update();
    }
    inline void	mousePressEvent(QMouseEvent *event) override
    {

        QPointF pos = event->localPos();
        float x = event->pos().x();
        float y = event->pos().y();

        m_dragXOrg = x;
        m_dragYOrg = y;

        if(event->buttons() == Qt::RightButton)
        {
            int index = GetIndexOfStopAt(pos);
            if(m_drag == false)
            {
                if(index > -1 && stops.length() > 1)
                {
                    stops.removeAt(index);
                    colors.removeAt(index);

                    emit OnGradientChanged();
                }

                dragindex = -1;
            }
        }
        if(event->buttons() == Qt::LeftButton)
        {

            int index = GetIndexOfStopAt(pos);

            if(index > -1)
            {
                dragindex = index;
                m_dragX = x;
                m_dragY = y;
                m_drag = true;
            }
        }
        this->update();
    }
    inline void	mouseReleaseEvent(QMouseEvent *event) override
    {
        if(event->button() == Qt::LeftButton)
        {
            QPointF pos = event->localPos();
            float x = event->pos().x();
            float y = event->pos().y();
            int index = GetIndexOfStopAt(pos);



            if(index > -1)
            {

                dragindex = index;
                if(std::sqrt((m_dragXOrg - x)*(m_dragXOrg - x) + (m_dragYOrg - y)*(m_dragYOrg - y)) < 2.0)
                {
                    QColor color = QColorDialog::getColor(QColor(255.0f*colors[dragindex].r,255.0f*colors[dragindex].g,255.0f*colors[dragindex].b,255.0f*colors[dragindex].a), nullptr,"Gradient Color",QColorDialog::ShowAlphaChannel);
                    if( color.isValid() )
                    {
                        colors.replace(dragindex,ColorF(color.redF(),color.greenF(),color.blueF(),color.alphaF()));
                    }
                }
                emit OnGradientChanged();
            }
        }


        m_drag = false;
    }



    inline QSize sizeHint() const override
    {
        return QSize(300,50);

    }


signals:

    void OnGradientChanged();

};

class GradientDialog: public QDialog
{
        Q_OBJECT;

public:
    QList<ColorF> colors;
    QList<float> stops;

    QList<ColorF> colors_org;
    QList<float> stops_org;

    int length = 0;

    GradientWidget * m_GradientWidget;

    inline GradientDialog(QList<float> in_stops, QList<ColorF> in_colors, QWidget * parent = 0, Qt::WindowFlags f = 0) : QDialog(parent,f)
    {

        if(in_stops.length() < 2)
        {
            in_stops.clear();
            in_colors.clear();
            in_stops.append(0.0f);
            in_stops.append(1.0f);
            in_colors.append(ColorF(0.0,0.0,0.0,1.0));
            in_colors.append(ColorF(1.0,1.0,1.0,1.0));
        }

        stops = in_stops;
        colors = in_colors;
        length = stops.length();

        stops_org = in_stops;
        colors_org = in_colors;


        QVBoxLayout* mainGrid = new QVBoxLayout;


        m_GradientWidget = new GradientWidget(in_stops,in_colors);
        m_GradientWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
        connect(m_GradientWidget, &GradientWidget::OnGradientChanged, this, &GradientDialog::OnGradientEdited);

        QWidget * buttons = new QWidget();
        QHBoxLayout *buttonsl = new QHBoxLayout();
        buttons->setLayout(buttonsl);

        QPushButton * okb = new QPushButton("Ok");
        QPushButton * cancelb = new QPushButton("Cancel");

        connect(okb, SIGNAL(clicked(bool)),this,SLOT(onOkClicked()));
        connect(cancelb, SIGNAL(clicked(bool)),this,SLOT(onCancelClicked()));

        buttonsl->addWidget(okb);
        buttonsl->addWidget(cancelb);

        mainGrid->addWidget(m_GradientWidget);
        mainGrid->addWidget(buttons);

        this->setLayout(mainGrid);

    }

    inline ~GradientDialog()
    {

    }

public slots:

    inline void onOkClicked()
    {
        stops = m_GradientWidget->stops;
        colors = m_GradientWidget->colors;
        emit accept();
    }
    inline void onCancelClicked()
    {
        stops = stops_org;
        colors = colors_org;
        emit reject();
    }

    inline void OnGradientEdited()
    {
        stops = m_GradientWidget->stops;
        colors = m_GradientWidget->colors;

        emit OnGradientChanged();
    }

signals:

    void OnGradientChanged();

};
#endif // GRADIENTSELECTIONWIDGET_H
