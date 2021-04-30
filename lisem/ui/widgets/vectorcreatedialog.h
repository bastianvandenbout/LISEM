#ifndef VECTORCREATEDIALOG_H
#define VECTORCREATEDIALOG_H

#include "QWidget"
#include "QGroupBox"
#include "QHBoxLayout"
#include "QDialog"
#include "styles/sphstyle.h"
#include "gradientcombobox.h"

#include "QToolButton"
#include "QCheckBox"
#include "linestylecombobox.h"
#include "fillstylecombobox.h"
#include "colorwheel.h"
#include "QScrollArea"
#include "QToolBox"
#include "QSpacerItem"
#include "widgets/labeledwidget.h"
#include "QDoubleSpinBox"
#include "spoilerwidget.h"
#include "QLineEdit"
#include "colorbutton.h"
#include "QRadioButton"
#include "tools/toolparameterwidget.h"
#include "QToolButton"
#include "iogdal.h"


typedef struct VectorConstructInfo
{

     int shapetype = 0;
    bool save = false;
    bool add_to_view = false;
    QString savename;
    QString projection;




} VectorConstructInfo;

class VectorCreateWidget : public QDialog
{
    Q_OBJECT;

    QVBoxLayout * m_Layout;
    QMap<QString,QWidget*> m_Widgets;



    GeoProjectionSelectWidget * m_CRSWidget;

    QCheckBox * m_AddToMapView;
    QCheckBox * m_Save;

    ShapeFileSaveSelectWidget * m_SaveWidget;

    QRadioButton *buttonc1;
    QRadioButton *buttonc2;
    QRadioButton *buttonc3;
    QRadioButton *buttonc4;
    QRadioButton *buttonc5;
    QRadioButton *buttonc6;

public:

    inline VectorCreateWidget(bool can_add_to_mapview = true) : QDialog()
    {

        m_Layout = new QVBoxLayout();


        m_AddToMapView = new QCheckBox("Add to View");
        m_Save = new QCheckBox("Save to File");

        if(can_add_to_mapview)
        {
            m_AddToMapView->setChecked(true);
            m_Save->setChecked(true);
        }else
        {
            m_AddToMapView->setChecked(false);
            m_Save->setChecked(true);
        }

        this->setLayout(m_Layout);

        buttonc1 = new QRadioButton("Point", this);
        buttonc2 = new QRadioButton("Line", this);
        buttonc3 = new QRadioButton("Polygon", this);
        buttonc4 = new QRadioButton("MultiPoint", this);
        buttonc5 = new QRadioButton("MultiLine", this);
        buttonc6 = new QRadioButton("MultiPolygon", this);

        m_Layout->addWidget(buttonc1);
        m_Layout->addWidget(buttonc2);
        m_Layout->addWidget(buttonc3);
        m_Layout->addWidget(buttonc4);
        m_Layout->addWidget(buttonc5);
        m_Layout->addWidget(buttonc6);

         if(can_add_to_mapview)
         {
            m_Layout->addWidget(m_AddToMapView);
            m_Layout->addWidget(m_Save);
         }

         m_SaveWidget = new ShapeFileSaveSelectWidget();
         m_Layout->addWidget(new QWidgetHDuo(new QLabel("Save To"),m_SaveWidget));


         m_CRSWidget = new GeoProjectionSelectWidget();
         m_Layout->addWidget(m_CRSWidget);


         buttonc1->setChecked(true);




         QWidget * buttons = new QWidget();
         QHBoxLayout *buttonsl = new QHBoxLayout();
         buttons->setLayout(buttonsl);

         QPushButton * okb = new QPushButton("Ok");
         QPushButton * cancelb = new QPushButton("Cancel");

         connect(okb, SIGNAL(clicked(bool)),this,SLOT(onOkClicked()));
         connect(cancelb, SIGNAL(clicked(bool)),this,SLOT(onCancelClicked()));

         buttonsl->addWidget(okb);
         buttonsl->addWidget(cancelb);

         m_Layout->addWidget(buttons);


         setFixedHeight(sizeHint().height());
         setFixedWidth(sizeHint().width());
    }

    inline VectorConstructInfo GetMapInfo()
    {
        VectorConstructInfo info;

        info.save = m_Save->isChecked();
        info.savename = m_SaveWidget->GetValueAsString();
        info.add_to_view = m_AddToMapView->isChecked();
        info.projection = m_CRSWidget->GetValueAsString();

        /*#define LISEM_SHAPE_POINT 1
        #define LISEM_SHAPE_LINE 2
        #define LISEM_SHAPE_POLYGON 4
        #define LISEM_SHAPE_MULTIPOINT 5
        #define LISEM_SHAPE_MULTILINE 6
        #define LISEM_SHAPE_MULTIPOLYGON 7*/

        if(buttonc1->isChecked())
        {
            info.shapetype = LISEM_SHAPE_POINT;
        }else if(buttonc2->isChecked())
        {
            info.shapetype = LISEM_SHAPE_LINE;
        }else if(buttonc3->isChecked())
        {
            info.shapetype = LISEM_SHAPE_POLYGON;
        }else if(buttonc4->isChecked())
        {
            info.shapetype = LISEM_SHAPE_MULTIPOINT;
        }else if(buttonc5->isChecked())
        {
            info.shapetype = LISEM_SHAPE_MULTILINE;
        }else if(buttonc6->isChecked())
        {
            info.shapetype = LISEM_SHAPE_MULTIPOLYGON;
        }


        return info;
    }

    static inline QString GetValueAsCodeString(VectorConstructInfo info)
    {
        return "";
    }


    static inline ShapeFile * CreateMap(VectorConstructInfo info)
    {

        ShapeFile * f = new ShapeFile();
        ShapeLayer * l = new ShapeLayer();

        l->SetType(info.shapetype);
        f->AddLayer(l);
        f->SetProjection(GeoProjection::FromString(info.projection));

        return f;
    }


public slots:

    inline void onOkClicked()
    {
        emit accept();
    }
    inline void onCancelClicked()
    {
        emit reject();
    }

};

#endif // VECTORCREATEDIALOG_H
