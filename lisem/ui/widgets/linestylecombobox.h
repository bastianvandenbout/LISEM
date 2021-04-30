#ifndef LINESTYLECOMBOBOX_H
#define LINESTYLECOMBOBOX_H

#include "QComboBox"
#include "QLabel"
#include "QAbstractItemView"
#include "QStyledItemDelegate"
#include "QItemDelegate"
#include "QPainter"
#include <QStandardItemModel>
#include <iostream>
#include "QStylePainter"
#include "styles/sphlinestyle.h"
#include "QColorDialog"

static inline void DrawLineStyleToRect(QPainter * p, QRect r, LSMLineStyle s)
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
    if(s.m_HasPattern && s.m_Color2.a > 0.0)
    {
        QVector<qreal> Pattern;
        for(int i = 0; i < s.pattern.length(); i++)
        {
            Pattern.append(s.pattern.at(i) * 2.5);
        }
        if(Pattern.length() % 2 == 1)
        {
            Pattern.append(1);
        }
        QPen pen;
        pen.setStyle(Qt::CustomDashLine);
        pen.setDashPattern(Pattern);
        pen.setWidth(10.0);
        pen.setBrush(QColor(s.m_Color2.r*255.0f,s.m_Color2.g*255.0f,s.m_Color2.b*255.0f,s.m_Color2.a*255.0f));
        pen.setCapStyle(Qt::SquareCap);
        ppm.setPen(pen);
        ppm.setBrush(QColor(s.m_Color2.r*255.0f,s.m_Color2.g*255.0f,s.m_Color2.b*255.0f,s.m_Color2.a*255.0f));
        ppm.drawLine(rnew.left(),rnew.center().y(),rnew.right(),rnew.center().y());
    }
    ppm.end();
    p->drawPixmap(r,pm,rnew);


}


class LineStyleData
{
public:
     inline LineStyleData()
     {

     }

     inline ~LineStyleData()
     {

     }
     LSMLineStyle lstyle;
     bool is_message;
     QString message;

};

Q_DECLARE_METATYPE(LineStyleData)

class LineStyleDelegate : public QItemDelegate
{

public:

    QList<LineStyleData> m_List;
    inline LineStyleDelegate(QList<LineStyleData> list) : QItemDelegate()
    {
        m_List = list;
    }

    inline void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {


        QString i = index.data().toString();

            if(i.length() < 3)
            {
                int in = i.toInt();
                LineStyleData d = m_List.at(in);

                DrawLineStyleToRect(painter,option.rect,d.lstyle);

            }else {
                QItemDelegate::paint(painter, option, index);
            }

    }

    inline QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {
        return QSize(200,35);
    }

};

class LineStyleComboBox : public QComboBox
{

    Q_OBJECT;

public:

    LSMLineStyle m_CurrentLineStyle;

    QList<LineStyleData> m_List;

    inline LineStyleComboBox(QWidget * p = nullptr) : QComboBox(p)
    {
        m_CurrentLineStyle = GetDefaultLineStyle(LISEM_LINESTYLE_BLACK);

        QList<LineStyleData> gdlist;
        for(int i = 0; i < LISEM_LINESTYLE_NUM; i++)
        {
             LSMLineStyle grad =GetDefaultLineStyle(i);
            LineStyleData d = LineStyleData();
            d.lstyle = grad;
            gdlist.append(d);
            this->addItem(QString::number(i),QVariant(i));


        }

        LineStyleData d1 = LineStyleData();
        d1.is_message = true;
        d1.message = "Select Color1";
        gdlist.append(d1);
        this->addItem("Select Color1",QVariant(-1));
        LineStyleData d2 = LineStyleData();
        d2.is_message = true;
        d2.message = "Select Color2";
        gdlist.append(d2);
        this->addItem("Select Color2",QVariant(-1));
        LineStyleData d3 = LineStyleData();
        d3.is_message = true;
        d3.message = "Toggle Background";
        gdlist.append(d3);
        this->addItem("Toggle Background",QVariant(-1));


        this->setItemDelegate(new LineStyleDelegate(gdlist));

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
        DrawLineStyleToRect(&painter,rect,m_CurrentLineStyle);
    }


    inline void SetCurrentLineStyle(LSMLineStyle ls)
    {
        m_CurrentLineStyle = ls;
        update();

    }

    inline LSMLineStyle GetCurrentGradient()
    {

        return m_CurrentLineStyle;
    }

public slots:

    inline void OnItemClicked(QString s)
    {

        if(s.length() < 3)
        {
            int in = s.toInt();
            LineStyleData d = m_List.at(in);
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

#endif // LINESTYLECOMBOBOX_H
