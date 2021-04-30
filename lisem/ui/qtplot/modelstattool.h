#ifndef MODELSTATTOOL_H
#define MODELSTATTOOL_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QString>

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "scripting/codeeditor.h"
#include "QLineEdit"
#include "error.h"
#include "QTimer"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include "model.h"
#include "qcustomplot.h"
#include "glplot/worldwindow.h"

class ModelStatTool : public QWidget
{
        Q_OBJECT;

public:



    LISEMModel * m_Model;
    MODELTOINTERFACE * m_ModelData;
    QCustomPlot * m_CustomPlot;
    QSpinBox * m_SpinBox;

    QLabel * m_LabelArea;
    QLabel * m_LabelTime;
    QLabel * m_LabelRain;
    QLabel * m_LabelFlow;
    QLabel * m_LabelInfil;
    QLabel * m_LabelSurfStor;
    QLabel * m_LabelCanStor;
    QLabel * m_LabelChanFlow;
    QLabel * m_LabelOutFlow;
    QLabel * m_LabelFailure;

    inline ModelStatTool( LISEMModel * m, MODELTOINTERFACE *minterface, QWidget *parent = 0): QWidget( parent)
    {

        m_Model = m;
        m_ModelData = minterface;

        QHBoxLayout *gblayout_map  = new QHBoxLayout();
        this->setLayout(gblayout_map);
        QWidget * leftwidget = new QWidget(this);
        QVBoxLayout * leftlayout = new QVBoxLayout();

        leftwidget->setLayout(leftlayout);

        m_CustomPlot = new QCustomPlot(this);


        QLabel * Label_Title = new QLabel("Simulation Stats", this);
        Label_Title->setFont(QFont( "Helvetica",16));

        QGroupBox *window1 = new QGroupBox(leftwidget);
        QGroupBox *window2 = new QGroupBox(leftwidget);

        m_LabelArea = new QLabel("0");
        m_LabelTime = new QLabel("0");
        m_LabelRain = new QLabel("0");
        m_LabelFlow = new QLabel("0");
        m_LabelInfil = new QLabel("0");
        m_LabelSurfStor = new QLabel("0");
        m_LabelCanStor = new QLabel("0");
        m_LabelChanFlow = new QLabel("0");
        m_LabelOutFlow = new QLabel("0");
        m_LabelFailure = new QLabel("0");

        QFormLayout *layout = new QFormLayout();
        layout->addRow(new QLabel("Area (km2)"), m_LabelArea);
        layout->addRow(new QLabel("Time (min)"), m_LabelTime);
        layout->addRow(new QLabel("Rain (mm)"), m_LabelRain);
        layout->addRow(new QLabel("Flow (mm)"), m_LabelFlow);
        layout->addRow(new QLabel("Infil (mm)"), m_LabelInfil);
        layout->addRow(new QLabel("Surf. Stor. (mm)"), m_LabelSurfStor);
        layout->addRow(new QLabel("Can. Stor. (mm)"), m_LabelCanStor);
        layout->addRow(new QLabel("Chan. Flow (mm)"), m_LabelChanFlow);
        layout->addRow(new QLabel("Outflow (mm)"), m_LabelOutFlow);
        layout->addRow(new QLabel("Failure (mm)"), m_LabelFailure);

        window1->setLayout(layout);

        QFormLayout *layout2 = new QFormLayout(window2);
        m_SpinBox = new QSpinBox();
        m_SpinBox->setMinimum(0);
        m_SpinBox->setMaximum(0);

        layout2->addRow(new QLabel("Graph Settings"),new QLabel(""));
        layout2->addRow(new QLabel("Outlet"), m_SpinBox);

        connect(m_SpinBox,SIGNAL(valueChanged(int)),this,SLOT(OnSpinBoxIndexChanged(int)));

        window2->setLayout(layout2);

        leftlayout->addWidget(Label_Title);
        leftlayout->addWidget(window2);
        leftlayout->addWidget(window1);

        QSpacerItem *spacer = new QSpacerItem(40, 1000, QSizePolicy::Expanding, QSizePolicy::Minimum);
        leftlayout->addItem(spacer);

        gblayout_map->addWidget(leftwidget);
        gblayout_map->addWidget(m_CustomPlot);

        gblayout_map->setStretch(0,1);
        gblayout_map->setStretch(1,5);

        m_CustomPlot->xAxis->setLabel("Time (min)");
        m_CustomPlot->yAxis->setLabel("Value");
        m_CustomPlot->yAxis2->setLabel("Rain (mm/h)");

        m_CustomPlot->xAxis2->setVisible(true);
        m_CustomPlot->yAxis2->setVisible(true);
        m_CustomPlot->xAxis2->setTicks(false);
        m_CustomPlot->yAxis2->setTicks(true);
        m_CustomPlot->xAxis2->setTickLabels(false);
        m_CustomPlot->yAxis2->setTickLabels(true);

        m_CustomPlot->legend->setVisible(true);
        m_CustomPlot->legend->setBrush(QColor(255, 255, 255, 150));

    }



    void OnModelDataUpdate()
    {
        m_SpinBox->setMinimum(0);
        m_SpinBox->setMaximum(m_ModelData->outlets.length()-1);




        m_LabelArea->setText(QString::number(m_ModelData->area/(1000000.0f)));
        m_LabelTime->setText(QString::number(m_ModelData->t/60.0f));
        m_LabelRain->setText(QString::number(m_ModelData->rain_total));
        m_LabelFlow->setText(QString::number(m_ModelData->wh_total));
        m_LabelInfil->setText(QString::number(m_ModelData->infil_total));
        m_LabelSurfStor->setText(QString::number(m_ModelData->surfstor_total));
        m_LabelCanStor->setText(QString::number(m_ModelData->canstor_total));
        m_LabelChanFlow->setText(QString::number(m_ModelData->chwh_total));
        m_LabelOutFlow->setText(QString::number(m_ModelData->outflow_total));
        m_LabelFailure->setText(QString::number(m_ModelData->failure_total));


        DisplayData(m_SpinBox->value());

    }

    void DisplayData(int index)
    {

        index = std::min(std::max(0,index),m_ModelData->outlets.length());

        m_CustomPlot->clearGraphs();


        QVector<QCPGraphData> timeData(m_ModelData->H.at(index)->length());
        float hmax = 0.0;
        for(int i = 0; i < m_ModelData->H.at(index)->length(); i++)
        {
             timeData[i].key = ((float)(i)) * m_ModelData->dt/60.0f;
             timeData[i].value = m_ModelData->H.at(index)->at(i);
             hmax = std::max(hmax,m_ModelData->H.at(index)->at(i));
        }

        QVector<QCPGraphData> timeVData(m_ModelData->V.at(index)->length());
        float vmax = 0.0;
        for(int i = 0; i < m_ModelData->V.at(index)->length(); i++)
        {
             timeVData[i].key = ((float)(i)) * m_ModelData->dt/60.0f;
             timeVData[i].value = m_ModelData->V.at(index)->at(i);
             vmax = std::max(vmax,m_ModelData->V.at(index)->at(i));
        }

        QVector<QCPGraphData> timeQData(m_ModelData->Q.at(index)->length());
        float qmax = 0.0;
        for(int i = 0; i < m_ModelData->Q.at(index)->length(); i++)
        {
             timeQData[i].key = ((float)(i)) * m_ModelData->dt/60.0f;
             timeQData[i].value = m_ModelData->Q.at(index)->at(i);
             qmax = std::max(qmax,m_ModelData->Q.at(index)->at(i));
        }

        QVector<QCPGraphData> timeRData(m_ModelData->H.at(index)->length());
        float rmax = 0.0;
        for(int i = 0; i < m_ModelData->H.at(index)->length(); i++)
        {
             timeRData[i].key = ((float)(i)) * m_ModelData->dt/60.0f;
             timeRData[i].value = m_ModelData->rain.at(i);
             rmax = std::max(rmax,m_ModelData->rain.at(i));
        }

        m_CustomPlot->xAxis->setRange(0,((float)(m_ModelData->rain.length())*m_ModelData->dt)/60.0f);
        m_CustomPlot->yAxis->setRange(0,std::max(std::max(qmax,vmax),hmax));
        m_CustomPlot->yAxis2->setRange(0,rmax);

        m_CustomPlot->addGraph();
        QColor color(20+200/4.0*1.0,70*(1.6-1.0/4.0), 150, 150);
        m_CustomPlot->graph()->setLineStyle(QCPGraph::lsLine);
        m_CustomPlot->graph()->setPen(QPen(color.lighter(200)));
        m_CustomPlot->graph()->setBrush(QBrush(color));
        m_CustomPlot->graph()->data()->set(timeData);
        m_CustomPlot->graph()->setName("H (m)");

        m_CustomPlot->addGraph();
        QColor color2(20+200/4.0*3.0,70*(1.6-3.0/4.0), 150, 150);
        m_CustomPlot->graph()->setLineStyle(QCPGraph::lsLine);
        m_CustomPlot->graph()->setPen(QPen(color2.lighter(200)));
        m_CustomPlot->graph()->setBrush(QBrush(color2));
        m_CustomPlot->graph()->data()->set(timeVData);
        m_CustomPlot->graph()->setName("V (m/s)");

        m_CustomPlot->addGraph();
        QColor color3(20+200/4.0*2.0,70*(1.6-2.0/4.0), 150, 150);
        m_CustomPlot->graph()->setLineStyle(QCPGraph::lsLine);
        m_CustomPlot->graph()->setPen(QPen(color3.lighter(200)));
        m_CustomPlot->graph()->setBrush(QBrush(color3));
        m_CustomPlot->graph()->data()->set(timeQData);
        m_CustomPlot->graph()->setName("Q (m3/s)");

        m_CustomPlot->addGraph(m_CustomPlot->xAxis,m_CustomPlot->yAxis2);
        QColor color4(25,25,200, 150);
        m_CustomPlot->graph()->setPen(QPen(color4.lighter(200)));
        //m_CustomPlot->graph()->setBrush(QBrush(color4));
        m_CustomPlot->graph()->data()->set(timeRData);
        m_CustomPlot->graph()->setName("Rain (mm/h)");
        m_CustomPlot->replot();
    }


public slots:

    void OnSpinBoxIndexChanged(int index)
    {

        m_Model->m_ModelDataMutex.lock();

        DisplayData(index);

        m_Model->m_ModelDataMutex.unlock();

    }

};

#endif // MODELSTATTOOL_H
