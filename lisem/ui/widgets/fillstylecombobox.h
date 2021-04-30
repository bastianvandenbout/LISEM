#ifndef FILLSTYLECOMBOBOX_H
#define FILLSTYLECOMBOBOX_H


#include "QComboBox"
#include "QLabel"
#include "QAbstractItemView"
#include "QStyledItemDelegate"
#include "QItemDelegate"
#include "QPainter"
#include <QStandardItemModel>
#include <iostream>
#include "QStylePainter"
#include "styles/sphfillstyle.h"
#include "QColorDialog"
#include "QPixmap"

static inline void DrawFillStyleToRect(QPainter * p, QRect r, SPHFillStyle s)
{

    QRect rect = r;


    QPixmap pm(r.width(),r.height());

    QRect rnew = QRect(0,0,r.width(),r.height());

    QPainter ppm(&pm);

    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::HighQualityAntialiasing);
    ppm.fillRect(rnew,QColor(255,255,255,255));

    r.adjust(1,float(r.size().height()) * 0.1,-1,-float(r.size().height()) * 0.1);

    if(s.m_HasBackGroundColor && s.m_Color1.a > 0.0)
    {
        ppm.fillRect(rnew,QColor(s.m_Color1.r*255.0f,s.m_Color1.g*255.0f,s.m_Color1.b*255.0f,s.m_Color1.a*255.0f));
    }
    if(s.m_IsLines)
    {
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(s.m_LineSize);
        pen.setBrush(s.m_Color2.ToQColor());
        pen.setCapStyle(Qt::SquareCap);
        ppm.setPen(pen);
        ppm.setBrush(s.m_Color2.ToQColor());

        float sep =s.m_LineSeperation*5.0f;
        if(s.m_LineAngle <= 75)
        {
            int nlines =1.5 * rnew.width()/std::max(1.0f,sep);
            for(int i =0; i < nlines; i++)
            {
                //draw line
                ppm.drawLine(rnew.left() + i * sep,rnew.bottom(),rnew.left() + i * sep + sin(s.m_LineAngle * 3.14159/180.0) * rnew.height(),rnew.top());
            }
        }else if(s.m_LineAngle >= 105)
        {
            int nlines =1.5 * rnew.width()/std::max(1.0f,sep);
            for(int i =0; i < nlines; i++)
            {
                //draw line
                ppm.drawLine(rnew.left() + i * sep,rnew.top(),rnew.left() + i * sep + sin(s.m_LineAngle * 3.14159/180.0) * rnew.height(),rnew.bottom());
            }
        }else if(s.m_LineAngle < 90)
        {
            int nlines =1.5 * rnew.height()/std::max(1.0f,sep);
            for(int i =0; i < nlines; i++)
            {
                //draw line
                ppm.drawLine(rnew.left(),rnew.bottom()- i * sep,rnew.right(),rnew.bottom()- i * sep - cos(s.m_LineAngle * 3.14159/180.0) * rnew.width());
            }
        }else {
            int nlines =1.5 * rnew.height()/std::max(1.0f,sep);
            for(int i =0; i < nlines; i++)
            {
                //draw line
                ppm.drawLine(rnew.right(),rnew.bottom()- i * sep,rnew.left(),rnew.bottom()- i * sep + cos(s.m_LineAngle * 3.14159/180.0) * rnew.width());
            }
        }

    }
    if(s.m_IsShapes)
    {
        int n_shapes_x = rnew.width()/std::max(s.m_ShapeSeperation*5.0f,1.0f);
        int n_shapes_y = rnew.height()/std::max(s.m_ShapeSeperation*5.0f,1.0f);


        for(int i = 0; i < n_shapes_x; i++)
        {
            for(int j = 0; j < n_shapes_y; j++)
            {
                float x = i * s.m_ShapeSeperation*5.0;
                float y = j * s.m_ShapeSeperation*5.0;

                ppm.fillRect(QRect(x,y,s.m_ShapeSize*2.0,s.m_ShapeSize*2.0),s.m_Color2.ToQColor());
            }
        }
    }

    ppm.end();
    p->drawPixmap(r,pm,rnew);

}


class FillStyleData
{
public:
     inline FillStyleData()
     {

     }

     inline ~FillStyleData()
     {

     }
     SPHFillStyle lstyle;
     bool is_message;
     QString message;

};

Q_DECLARE_METATYPE(FillStyleData)

class FillStyleDelegate : public QItemDelegate
{

public:

    QList<FillStyleData> m_List;
    inline FillStyleDelegate(QList<FillStyleData> list) : QItemDelegate()
    {
        m_List = list;
    }

    inline void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {


        QString i = index.data().toString();

            if(i.length() < 3)
            {
                int in = i.toInt();
                FillStyleData d = m_List.at(in);

                DrawFillStyleToRect(painter,option.rect,d.lstyle);

            }else {
                QItemDelegate::paint(painter, option, index);
            }

    }

    inline QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {
        return QSize(200,35);
    }

};

class FillStyleComboBox : public QComboBox
{

    Q_OBJECT;

public:

    SPHFillStyle m_CurrentLineStyle;

    QList<FillStyleData> m_List;

    inline FillStyleComboBox(QWidget * p = nullptr) : QComboBox(p)
    {
        m_CurrentLineStyle = GetDefaultFillStyle(LISEM_FILLSTYLE_BLACK);

        QList<FillStyleData> gdlist;
        for(int i = 0; i < LISEM_FILLSTYLE_NUM; i++)
        {
             SPHFillStyle grad =GetDefaultFillStyle(i);
            FillStyleData d = FillStyleData();
            d.lstyle = grad;
            gdlist.append(d);
            this->addItem(QString::number(i),QVariant(i));


        }

        FillStyleData d1 = FillStyleData();
        d1.is_message = true;
        d1.message = "Select Color1";
        gdlist.append(d1);
        this->addItem("Select Color1",QVariant(-1));
        FillStyleData d2 = FillStyleData();
        d2.is_message = true;
        d2.message = "Select Color2";
        gdlist.append(d2);
        this->addItem("Select Color2",QVariant(-1));
        FillStyleData d3 = FillStyleData();
        d3.is_message = true;
        d3.message = "Toggle Background";
        gdlist.append(d3);
        this->addItem("Toggle Background",QVariant(-1));


        this->setItemDelegate(new FillStyleDelegate(gdlist));

        //setModel(model);

        connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(OnItemClicked(QString)));

        m_List = gdlist;
    }

    inline void paintEvent(QPaintEvent *e)  override
    {
        QPainter painter(this);
        QStylePainter spainter(this);


        QStyleOptionComboBox opt;
        initStyleOption(&opt);

        spainter.drawComplexControl(QStyle::CC_ComboBox, opt);

        QRect rect = opt.rect.adjusted(1,1, -20, -1);
        DrawFillStyleToRect(&painter,rect,m_CurrentLineStyle);
    }


    inline void SetCurrentFillStyle(SPHFillStyle ls)
    {
        m_CurrentLineStyle = ls;
        update();

    }

    inline SPHFillStyle GetCurrentGradient()
    {

        return m_CurrentLineStyle;
    }

public slots:

    inline void OnItemClicked(QString s)
    {

        if(s.length() < 3)
        {
            int in = s.toInt();
            FillStyleData d = m_List.at(in);
            m_CurrentLineStyle = d.lstyle;
            update();
        }else {

            if(s.compare("Select Color1") == 0)
            {
                QColor color = QColorDialog::getColor(m_CurrentLineStyle.m_Color1.ToQColor(), nullptr,"Fill Main Color",QColorDialog::ShowAlphaChannel);
                if( color.isValid() )
                {
                    m_CurrentLineStyle.m_Color1.FromQColor(color);
                }
            }
            if(s.compare("Select Color2") == 0)
            {
                QColor color = QColorDialog::getColor(m_CurrentLineStyle.m_Color2.ToQColor(), nullptr,"Fill Secundary Color",QColorDialog::ShowAlphaChannel);
                if( color.isValid() )
                {
                    m_CurrentLineStyle.m_Color2.FromQColor(color);
                }
            }
            if(s.compare("Toggle Background") == 0)
            {
                    m_CurrentLineStyle.m_HasBackGroundColor = !m_CurrentLineStyle.m_HasBackGroundColor;
            }

        }
        update();

        emit OnGradientChanged();
    }

signals:

    inline void OnGradientChanged();

};



#endif // FILLSTYLECOMBOBOX_H
