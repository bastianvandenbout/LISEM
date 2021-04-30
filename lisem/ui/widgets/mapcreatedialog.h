#pragma once


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


typedef struct MapConstructInfo
{

    bool is_extent = true;
    double fill_value =0.0;
    int resolutionx = 1;
    int resolutiony = 1;
    double gridcellsizex = 1;
    double gridcellsizey = 1;
    double ulx = 0.0;
    double uly = 0.0;
    BoundingBox Extent;
    bool save = false;
    bool add_to_view = false;
    QString savename;
    QString projection;




} MapConstructInfo;

class MapCreateWidget : public QDialog
{
    Q_OBJECT;

    QVBoxLayout * m_Layout;
    QMap<QString,QWidget*> m_Widgets;

    QGroupBox *m_Constructor1;
    QGroupBox *m_Constructor2;

    QVBoxLayout * m_Constructorl1;
    QVBoxLayout * m_Constructorl2;

    BoundingBoxSelectWidget * m_BoundingBox;

    GeoProjectionSelectWidget * m_CRSWidget;
    QDoubleSpinBox * m_Value;

    QSpinBox * m_ResolutionX1;
    QSpinBox * m_ResolutionY1;

    QSpinBox * m_ResolutionX2;
    QSpinBox * m_ResolutionY2;

    QDoubleSpinBox * m_GridSizeX2;
    QDoubleSpinBox * m_GridSizeY2;

    QCheckBox * m_AddToMapView;
    QCheckBox * m_Save;


    PointSelectWidget * m_ULX;

    MapSaveSelectWidget * m_SaveWidget;

    QRadioButton *buttonc1;

public:

    inline MapCreateWidget(bool can_add_to_mapview = true) : QDialog()
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

        buttonc1 = new QRadioButton("Construct From Extent and Resolution", this);
        QRadioButton *buttonc2 = new QRadioButton("Construct From Pixel Size and Resolution", this);
        m_Layout->addWidget(buttonc1);
        m_Layout->addWidget(buttonc2);


         m_Constructorl1 = new QVBoxLayout();
         m_Constructorl2 = new QVBoxLayout();

         m_Constructor1 = new QGroupBox("Constructor");
         m_Constructor2 = new QGroupBox("Constructor");

         m_Constructor1->setLayout(m_Constructorl1);
         m_Constructor2->setLayout(m_Constructorl2);

         m_BoundingBox = new BoundingBoxSelectWidget();
         m_Constructorl1->addWidget(new QWidgetHDuo(new QLabel("Extent"),m_BoundingBox));



         if(can_add_to_mapview)
         {
            m_Layout->addWidget(m_AddToMapView);
            m_Layout->addWidget(m_Save);
         }

         m_SaveWidget = new MapSaveSelectWidget();
         m_Layout->addWidget(new QWidgetHDuo(new QLabel("Save To"),m_SaveWidget));


         m_ResolutionX1 = new QSpinBox();
         m_ResolutionY1 = new QSpinBox();

         m_ResolutionX2 = new QSpinBox();
         m_ResolutionY2 = new QSpinBox();

         m_ResolutionX1->setMinimum(1);
         m_ResolutionY1->setMinimum(1);
         m_ResolutionX2->setMinimum(1);
         m_ResolutionY2->setMinimum(1);

         m_ResolutionX1->setMaximum(10000000);
         m_ResolutionY1->setMaximum(10000000);
         m_ResolutionX2->setMaximum(10000000);
         m_ResolutionY2->setMaximum(10000000);

         m_GridSizeX2 = new QDoubleSpinBox();
         m_GridSizeY2 = new QDoubleSpinBox();

         m_GridSizeX2->setValue(1);
         m_GridSizeY2->setValue(1);

         m_GridSizeX2->setMinimum(1e-10);
         m_GridSizeY2->setMinimum(1e-10);

         m_ULX = new PointSelectWidget();

         m_Constructorl1->addWidget(new QWidgetHDuo(new QLabel("Horizontal Resolution"),m_ResolutionX1));
         m_Constructorl1->addWidget(new QWidgetHDuo(new QLabel("Vertical Resolution"),m_ResolutionY1));


         m_Constructorl2->addWidget(new QWidgetHDuo(new QLabel("Horizontal Resolution"),m_ResolutionX2));
         m_Constructorl2->addWidget(new QWidgetHDuo(new QLabel("Vertical Resolution"),m_ResolutionY2));

         m_Constructorl2->addWidget(new QWidgetHDuo(new QLabel("Grid Cell Size X"),m_GridSizeX2));
         m_Constructorl2->addWidget(new QWidgetHDuo(new QLabel("Grid Cell Size Y"),m_GridSizeY2));

         m_Constructorl2->addWidget(new QWidgetHDuo(new QLabel("Upper Left Coordinate"),m_ULX));




         m_CRSWidget = new GeoProjectionSelectWidget();
         m_Layout->addWidget(m_CRSWidget);

         m_Value = new QDoubleSpinBox();
         m_Layout->addWidget(new QWidgetHDuo(new QLabel("Fill Value"),m_Value));


         m_Layout->addWidget(m_Constructor1);
         m_Layout->addWidget(m_Constructor2);


         connect(buttonc1, &QRadioButton::toggled,this,&MapCreateWidget::OnC1Toggled);
         connect(buttonc2, &QRadioButton::toggled,this,&MapCreateWidget::OnC2Toggled);

         buttonc1->setChecked(true);
         m_Constructor1->setVisible(true);
         m_Constructor2->setVisible(false);




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

    inline MapConstructInfo GetMapInfo()
    {
        MapConstructInfo info;

        info.Extent = m_BoundingBox->GetBoundingBox();
        info.ulx = m_ULX->GetPoint().X();
        info.uly = m_ULX->GetPoint().Y();
        info.save = m_Save->isChecked();
        info.savename = m_SaveWidget->GetValueAsString();
        info.is_extent = buttonc1->isChecked();
        info.fill_value = m_Value->value();
        info.add_to_view = m_AddToMapView->isChecked();
        info.resolutionx =buttonc1->isChecked()? m_ResolutionX1->value():m_ResolutionX2->value();
        info.resolutiony =buttonc1->isChecked()? m_ResolutionY1->value():m_ResolutionY2->value();
        info.gridcellsizex = m_GridSizeX2->value();
        info.gridcellsizey = m_GridSizeY2->value();
        info.projection = m_CRSWidget->GetValueAsString();
        return info;
    }

    static inline QString GetValueAsCodeString(MapConstructInfo info)
    {
        QString ret = "Map(";

        //int x, int y, float dx, float dy, float value, float north, float west

        if(info.is_extent)
        {
            int x = info.resolutionx;
            int y = info.resolutiony;

            float dx = info.Extent.GetSizeX()/(double(info.resolutionx));
            float dy = -info.Extent.GetSizeY()/(double(info.resolutiony));

            float value  = info.fill_value;

            float ulx = info.Extent.GetMinX();
            float uly = info.Extent.GetMinY();

            QString Projection = info.projection;

            ret += QString::number(x) + ",";
            ret += QString::number(y) + ",";
            ret += QString::number(dx,'g',10) + ",";
            ret += QString::number(dy,'g',10) + ",";
            ret += QString::number(uly,'g',10) + ",";
            ret += QString::number(ulx,'g',10) + ",";
            ret += "\"" + Projection + "\"";

        }else
        {

            int x = info.resolutionx;
            int y = info.resolutiony;

            float dx = info.gridcellsizex;
            float dy = info.gridcellsizey;

            float value  = info.fill_value;

            float ulx = info.ulx;
            float uly = info.uly;

            QString Projection = info.projection;


            ret += QString::number(x) + ",";
            ret += QString::number(y) + ",";
            ret += QString::number(dx,'g',10) + ",";
            ret += QString::number(dy,'g',10) + ",";
            ret += QString::number(uly,'g',10) + ",";
            ret += QString::number(ulx,'g',10) + ",";
            ret += "\"" + Projection + "\"";
        }


        return ret + ")";
    }


    static inline cTMap * CreateMap(MapConstructInfo info)
    {
        int nr_rows = info.resolutiony;
        int nr_cols = info.resolutionx;

        if(nr_rows < 1|| nr_cols < 1)
        {
            return nullptr;
        }

        double north = info.is_extent? info.Extent.GetMinY() : info.uly;
        double west = info.is_extent? info.Extent.GetMinX() : info.ulx;

        double cellsizex = info.is_extent? info.Extent.GetSizeX()/(double(info.resolutionx)):info.gridcellsizex;
        double cellsizey = info.is_extent? info.Extent.GetSizeY()/(double(info.resolutiony)):info.gridcellsizey;

        MaskedRaster<float> raster_data(nr_rows,nr_cols,north,west,cellsizex, cellsizey);

        cTMap *nmap = new cTMap(std::move(raster_data),info.projection,"");

        double val = info.fill_value;

        FOR_ROW_COL(nmap)
        {
            nmap->data[r][c] = val;
        }
        return nmap;
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



    inline void OnC1Toggled(bool checked)
    {
        m_Constructor1->setVisible(true);
        m_Constructor2->setVisible(false);

        setFixedHeight(sizeHint().height());
        setFixedWidth(sizeHint().width());
    }
    inline void OnC2Toggled(bool checked)
    {
        m_Constructor1->setVisible(false);
        m_Constructor2->setVisible(true);

        setFixedHeight(sizeHint().height());
        setFixedWidth(sizeHint().width());
    }
};

