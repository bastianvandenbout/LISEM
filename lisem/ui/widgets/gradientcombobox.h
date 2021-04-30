#ifndef GRADIENTCOMBOBOX_H
#define GRADIENTCOMBOBOX_H


#include "QComboBox"
#include "color/lsmcolorgradient.h"
#include "QLabel"
#include "QAbstractItemView"
#include "QStyledItemDelegate"
#include "QItemDelegate"
#include "QPainter"
#include <QStandardItemModel>
#include <iostream>
#include "QStylePainter"
#include "gradientselectionwidget.h"
#include "settings/generalsettingsmanager.h"

class GradientData
{
public:
     inline GradientData()
     {

     }

     inline ~GradientData()
     {

     }
     LSMColorGradient grad;
     bool is_message;
     QString message;

};

Q_DECLARE_METATYPE(GradientData)

class GradientDelegate : public QItemDelegate
{

public:

    QList<GradientData> *m_List;
    LSMColorGradient * m_Current;
    inline GradientDelegate(QList<GradientData> *list, LSMColorGradient * current) : QItemDelegate()
    {
        m_List = list;
        m_Current = current;
    }

    inline void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {


        QString i = index.data().toString();

            if(i.length() < 3)
            {
                int in = i.toInt();
                QLinearGradient gradient(QPointF(option.rect.left()+1,option.rect.top()+1), QPointF(option.rect.right()-1,option.rect.bottom()-1)); // diagonal gradient from top-left to bottom-right
                LSMColorGradient grad;

                if(in > 0)
                {
                    GradientData d = m_List->at(in-1);
                    grad = d.grad;
                }else if(in == 0)
                {
                    grad = *m_Current;
                }

                for(int i = 0; i < grad.m_Gradient_Stops.length(); i++)
                {
                    if(grad.m_Gradient_Stops[i] > 1.00000f)
                    {
                        break;
                    }
                    gradient.setColorAt(grad.m_Gradient_Stops[i],QColor(255.0f*grad.m_Gradient_Colors[i].r,255.0f*grad.m_Gradient_Colors[i].g,255.0f*grad.m_Gradient_Colors[i].b,255.0f*grad.m_Gradient_Colors[i].a));
                }

                painter->fillRect(option.rect.adjusted(1,1,-1,-1),gradient);
            }else {
                QItemDelegate::paint(painter, option, index);
            }

    }

    inline QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override
    {
        return QSize(200,35);
    }

};

class GradientComboBox : public QComboBox
{

    Q_OBJECT;

public:

    LSMColorGradient m_CurrentGradient;
    GradientDialog * m_GradientDialog = nullptr;

    QList<GradientData> m_List;

    inline GradientComboBox(QWidget * p = nullptr) : QComboBox(p)
    {
        m_CurrentGradient = GetDefaultGradient(LISEM_GRADIENT_UI);


        QList<GradientData> *gdlist = &m_List;


        {
            LSMColorGradient grad =GetDefaultGradient(0);
            GradientData d = GradientData();
            d.grad = grad;
            this->addItem(QString::number(0),QVariant(0));
        }

        for(int i = 0; i < LISEM_GRADIENT_NUM; i++)
        {
            LSMColorGradient grad =GetDefaultGradient(i);
            GradientData d = GradientData();
            d.grad = grad;
            gdlist->append(d);
            this->addItem(QString::number(i+1),QVariant(i));


        }

        GradientData d1 = GradientData();
        d1.is_message = true;
        d1.message = "Custom";
        gdlist->append(d1);
        this->addItem("Custom",QVariant(-1));
        GradientData d2 = GradientData();
        d2.is_message = true;
        d2.message = "Invert Selected";
        gdlist->append(d2);
        this->addItem("Invert Selected",QVariant(-1));
        GradientData d3 = GradientData();
        d3.is_message = true;
        d3.message = "Toggle Transparancy";
        gdlist->append(d3);
        this->addItem("Toggle Transparancy",QVariant(-1));
        GradientData d4 = GradientData();
        d4.is_message = true;
        d4.message = "Log Scale";
        gdlist->append(d4);
        this->addItem("Log Scale",QVariant(-1));
        GradientData d5 = GradientData();
        d5.is_message = true;
        d5.message = "Inv. Log Scale";
        gdlist->append(d5);
        this->addItem("Inv. Log Scale",QVariant(-1));

        GradientData d6 = GradientData();
        d6.is_message = true;
        d6.message = "Set as Default";
        gdlist->append(d6);
        this->addItem("Set as Default",QVariant(-1));


        this->setItemDelegate(new GradientDelegate(gdlist,&m_CurrentGradient));

        //setModel(model);

        connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(OnItemClicked(QString)));

    }

    inline void paintEvent(QPaintEvent *e)  override
    {
        QPainter painter(this);
        QStylePainter spainter(this);


        QStyleOptionComboBox opt;
        initStyleOption(&opt);

        spainter.drawComplexControl(QStyle::CC_ComboBox, opt);

        QRect rect = opt.rect.adjusted(1,1, -20, -1);
        QLinearGradient gradient(QPointF(rect.top(),rect.left()), QPointF(rect.right(),rect.bottom())); // diagonal gradient from top-left to bottom-right

        for(int i = 0; i < m_CurrentGradient.m_Gradient_Stops.length(); i++)
        {
            if(m_CurrentGradient.m_Gradient_Stops[i] > 1.00001f)
            {
                break;
            }
            gradient.setColorAt(m_CurrentGradient.m_Gradient_Stops[i],QColor(255.0f*m_CurrentGradient.m_Gradient_Colors[i].r,255.0f*m_CurrentGradient.m_Gradient_Colors[i].g,255.0f*m_CurrentGradient.m_Gradient_Colors[i].b,255.0f*m_CurrentGradient.m_Gradient_Colors[i].a));

        }
        painter.fillRect(rect,gradient);
    }


    inline void SetCurrentGradient(LSMColorGradient grad)
    {
        grad.MakeValid();
        m_CurrentGradient = grad;
        update();

    }

    inline LSMColorGradient GetCurrentGradient()
    {

        return m_CurrentGradient;
    }

public slots:

    inline void OnGradientEdited()
    {
        if(m_GradientDialog != nullptr)
        {
            m_CurrentGradient = LSMColorGradient(m_GradientDialog->stops,m_GradientDialog->colors);

        }

        emit OnGradientChanged();
    }



    inline void OnItemClicked(QString s)
    {

        if(s.length() < 3)
        {
            int in = s.toInt();
            if(in > 0)
            {
                GradientData d = m_List.at(in-1);
                m_CurrentGradient = d.grad;
                update();
            }
        }else {

            if(s.compare("Custom") == 0)
            {

                QList<float> stops;
                QList<ColorF> colors;

                for(int i = 0; i < m_CurrentGradient.m_Gradient_Stops.length(); i++)
                {
                    stops.append(m_CurrentGradient.m_Gradient_Stops[i]);
                    colors.append(m_CurrentGradient.m_Gradient_Colors[i]);
                }

                m_GradientDialog = new GradientDialog(stops,colors);

                connect(m_GradientDialog,&GradientDialog::OnGradientChanged,this,&GradientComboBox::OnGradientEdited);
                int res = m_GradientDialog->exec();


                if(res = QDialog::Accepted)
                {

                    m_CurrentGradient = LSMColorGradient(m_GradientDialog->stops,m_GradientDialog->colors);
                }else {
                    m_CurrentGradient = LSMColorGradient(m_GradientDialog->stops,m_GradientDialog->colors);
                }
                delete m_GradientDialog;
                m_GradientDialog = nullptr;
            }

            if(s.compare("Invert Selected") == 0)
            {
                m_CurrentGradient = m_CurrentGradient.Invert();
            }
            if(s.compare("Toggle Transparancy") == 0)
            {
                m_CurrentGradient = m_CurrentGradient.ToggleTransparancy();
            }
            if(s.compare("Inv. Log Scale") == 0)
            {
                m_CurrentGradient = m_CurrentGradient.ApplyInvLog();
            }
            if(s.compare("Log Scale") == 0)
            {
                m_CurrentGradient = m_CurrentGradient.ApplyLog();
            }
            if(s.compare("Set as Default") == 0)
            {
                QJsonObject jsonObj;
                m_CurrentGradient.Save(&jsonObj);

                QJsonDocument doc(jsonObj);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                GetSettingsManager()->SetSetting("USERGRADIENT",strJson);
            }
        }

        update();

        this->blockSignals(true);
        this->setCurrentIndex(-1);
        this->blockSignals(false);

        emit OnGradientChanged();
    }

signals:

    inline void OnGradientChanged();

};








#endif // GRADIENTCOMBOBOX_H
