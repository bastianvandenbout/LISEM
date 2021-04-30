#pragma once


#include "QWidget"
#include "QGroupBox"
#include "QHBoxLayout"
#include "QDialog"
#include "styles/sphstyle.h"
#include "widgets/gradientcombobox.h"

#include "layer/uilayer.h"

#include "layer/layerparameters.h"

#include "QToolButton"
#include "QCheckBox"
#include "widgets/linestylecombobox.h"
#include "widgets/fillstylecombobox.h"
#include "widgets/colorwheel.h"
#include "QScrollArea"
#include "QToolBox"
#include "QSpacerItem"
#include "widgets/labeledwidget.h"
#include "QDoubleSpinBox"
#include "widgets/spoilerwidget.h"
#include "QLineEdit"
#include "widgets/colorbutton.h"

class LayerParameterWidget : public QDialog
{
    Q_OBJECT;

    UILayer *  m_Layer;
    LayerParameters *m_params = nullptr;
    QVBoxLayout * m_Layout;
    QMap<QString,QWidget*> m_Widgets;
public:

    inline LayerParameterWidget(UILayer * l) : QDialog()
    {
        m_Layer = l;
        this->setMinimumSize(QSize(400,400));
        m_Layout = new QVBoxLayout();

        SetupWidgetFromLayer(m_Layer);


        this->setLayout(m_Layout);


    }

    inline void SetupWidgetFromLayer(UILayer * l)
    {
        //deal with callbacks
        if(m_params != nullptr)
        {
            m_params->RemoveGeneralCallBacks();
        }
        m_Widgets.clear();
         m_params = m_Layer->GetParameters();

         m_params->SetGeneralCallBacks(
                     [](){


         },
                     [](QString, void*){



         }
         );


         //now clean up the widget

         clearLayout(m_Layout);


         //build up the widget

         QList<QString> names = m_params->GetParameterNames();
         QList<int> types = m_params->GetParameterTypes();
         QList<QString> values = m_params->GetParameterValues();

         for(int i = 0; i < names.length(); i++)
         {
             QString name = names.at(i);

             QWidget * temp = new QWidget();
             QHBoxLayout * templ = new QHBoxLayout();
             temp->setLayout(templ);
             QWidget * w;

             QLabel * label = new QLabel(names.at(i));
             templ->addWidget(label);
             templ->addItem(new QSpacerItem(9999,20));

             int type = types.at(i);

             if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_BOOL)
             {
                 QCheckBox *b = new QCheckBox();
                 b->setChecked(values.at(i).toInt() > 0);
                 connect(b,&QCheckBox::pressed,[name,b,this](){this->m_params->SetParameter(name,b->isChecked()?"1":"0");});
                 w = b;

             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_COUNT)
             {
                 QSpinBox * sb = new QSpinBox();
                 sb->setRange(-10000000000,1000000000);
                 sb->setValue(values.at(i).toInt());
                 connect(sb,qOverload<int>(&QSpinBox::valueChanged),[name,sb,this](int val){this->m_params->SetParameter(name,QString::number(val));});
                 w = sb;

             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_NUMBER)
             {
                 QDoubleSpinBox * sb = new QDoubleSpinBox();
                 sb->setRange(-10000000000.0,1000000000.0);
                 connect(sb,qOverload<double>(&QDoubleSpinBox::valueChanged),[name,sb,this](double val){this->m_params->SetParameter(name,QString::number(val));});
                 sb->setValue(values.at(i).toDouble());
                 w = sb;

             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_STRING)
             {
                QLineEdit * le = new QLineEdit();
                le->setText(values.at(i));
                le->setMinimumSize(250,20);
                connect(le,&QLineEdit::textChanged,[name,le,this](const QString&val){this->m_params->SetParameter(name,val);});
                w = le;

             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_COLOR)
             {
                 ColorButton * cb = new ColorButton();
                 ColorF c;
                 c.fromString(values.at(i));
                 cb->SetColor(c);
                 //connect this to the Parameters
                 connect(cb,&ColorButton::OnColorChanged,this,[name,cb,this](){this->m_params->SetParameter(name,cb->GetColorF().toString());});
                 w = cb;

             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_VEC3)
             {


             }else if(type == LayerParameters::LISEM_LAYERPARAMETER_TYPE_TEXTURE)
             {

                 QLineEdit * le = new QLineEdit();
                 le->setMinimumSize(250,20);

                 //load button



                 w = le;

             }else if(LayerParameters::LISEM_LAYERPARAMETER_TYPE_3DOBJECT)
             {
                 QLineEdit * le = new QLineEdit();
                 le->setMinimumSize(250,20);

                 //load button

                 w = le;


             }else {

                 //do nothing
             }


             templ->addWidget(w);
             m_Layout->addWidget(temp);
             m_Widgets[names.at(i)] = (QWidget*)w;

         }

        m_Layout->addItem(new QSpacerItem(20,9999));





    }

    void clearLayout(QLayout *layout) {
        QLayoutItem *item;
        while((item = layout->takeAt(0))) {
            if (item->layout()) {
                clearLayout(item->layout());
                delete item->layout();
            }
            if (item->widget()) {
               delete item->widget();
            }
            QSpacerItem *SpacerItem = nullptr;
            if(SpacerItem = item->spacerItem())
            { delete SpacerItem; }
            delete item;
        }
    }

    inline ~LayerParameterWidget()
    {

        if(m_params != nullptr)
        {
            m_params->RemoveGeneralCallBacks();

        }
        m_Widgets.clear();
    }

};

