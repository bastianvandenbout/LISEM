
#include "tableplot.h"
#include "qtplot/qcustomplot.h"
#include "spoilerwidget.h"
#include "iostream"
#include "widgets/layerinfowidget.h"
#include "QFileDialog"
#include "QtDataVisualization/Q3DScene"
#include "site.h"

TablePlotter::TablePlotter(MatrixTable * tbl_in, bool one_x_list_in, bool has_time_slider) : QDialog()
{
    m_Dir = GetSite();

    if(!one_x_list_in)
    {
        has_time_slider = false;
    }

    QVBoxLayout * vl = new QVBoxLayout();
    this->setLayout(vl);

    m_CustomPlot = new QCustomPlot();

    //m_CustomPlot->setBackground(QBrush(QColor(53,53,53)));
    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    m_CustomPlot->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    m_CustomPlot->xAxis2->setVisible(true);
    m_CustomPlot->yAxis2->setVisible(true);
    m_CustomPlot->xAxis2->setTickLabels(false);
    m_CustomPlot->yAxis2->setTickLabels(false);




    //set user interactions

    m_CustomPlot->setInteraction(QCP::iRangeDrag, true);
    m_CustomPlot->setInteraction(QCP::iRangeZoom, true);
    m_CustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                  QCP::iSelectLegend | QCP::iSelectPlottables);


    connect(m_CustomPlot,&QCustomPlot::plottableClick,this,&TablePlotter::graphClicked);
    connect(m_CustomPlot,&QCustomPlot::plottableDoubleClick,this,&TablePlotter::graphDClicked);
    //set menu bar

    QWidget * MenuWidgetExt = new QWidget();
    QHBoxLayout * mlExt = new QHBoxLayout(MenuWidgetExt);
    QWidget * MenuWidget = new QWidget();
    QHBoxLayout * ml = new QHBoxLayout(MenuWidget);
    MenuWidget->setLayout(ml);
    ml->setMargin(0);
    ml->setSpacing(0);
    mlExt->setMargin(0);
    mlExt->setSpacing(0);
    MenuWidgetExt->setLayout(mlExt);
    QMenuBar* mb = new QMenuBar();

    ml->setMenuBar(mb);

    QMenu * m = mb->addMenu("File");
    m->addAction("Open Data In Table View");
    m->addSeparator();
    m->addAction("Add Table File");
    m->addSeparator();
    m->addAction("Export to Image");
    m->addAction("Export to Table");
    m->addSeparator();
    m->addAction("Close");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    m = mb->addMenu("Elements");
    m->addAction("Set Plot Title");
    m->addAction("Set Horizontal Axis Label");
    m->addAction("Set Vertical Axis Label");
    m->addAction("Toggle Legend");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    m = mb->addMenu("Style");
    m->addAction("Set Uniform Plot Color");
    m->addAction("Set Default Plot Color");
    m->addAction("Set BackGround Color");
    m->addAction("Set Text Color");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    m = mb->addMenu("View");
    m->addAction("Zoom to all");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    //vl->addWidget(m_MainWidget);

    mlExt->addWidget(MenuWidget);
    vl->addWidget(MenuWidgetExt);

    vl->addWidget(m_CustomPlot);
    QWidget * infowidget = new QWidget();
    QHBoxLayout * m_infolayout = new QHBoxLayout();
    infowidget->setLayout(m_infolayout);
    m_SampleLabel = new QLabel("Current Location: ");

    m_infolayout->addWidget(m_SampleLabel);


    if(has_time_slider)
    {
        m_Slider = new QSlider();
        m_Slider->setOrientation(Qt::Horizontal);

        connect(m_Slider,&QSlider::valueChanged,this,&TablePlotter::OnTimeSliderValueChanged);

        m_infolayout->addWidget(m_Slider);
    }

    vl->addWidget(infowidget);

    Spoiler * spoilerdraw = new Spoiler("Draw Settings");

    QVBoxLayout * spoilerdrawl = new QVBoxLayout();
    m_ListWidget = new QListWidget();
    spoilerdrawl->addWidget(new QLabel("Graph Style List"));

    spoilerdrawl->addWidget(m_ListWidget);

    vl->addWidget(spoilerdraw);
    spoilerdraw->setContentLayout(*spoilerdrawl);


    Spoiler * spoiler = new Spoiler("Input Data");

    QVBoxLayout * spoilerl = new QVBoxLayout();


    vl->addWidget(spoiler);

    textedits = new QTabWidget();
    textedits->setTabsClosable(true);
    textedits->setMinimumSize(200,200);
    spoilerl->addWidget(textedits);
    spoiler->setContentLayout(*spoilerl);

    connect(textedits,&QTabWidget::tabCloseRequested,this,&TablePlotter::OnTabClose);
    this->setMinimumSize(QSize(600,300));


    m_CustomPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
    draggingLegend = false;

    connect(m_CustomPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveSignal(QMouseEvent*)));
    connect(m_CustomPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSignal(QMouseEvent*)));
    connect(m_CustomPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseSignal(QMouseEvent*)));
    connect(m_CustomPlot, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));


    SetFromMatrixTable(tbl_in,one_x_list_in);
}


void TablePlotter::mouseMoveSignal(QMouseEvent *event)
{
  if (draggingLegend)
  {
    QRectF rect = m_CustomPlot->axisRect()->insetLayout()->insetRect(0);
    // since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
    QPointF mousePoint((event->pos().x()-m_CustomPlot->axisRect()->left())/(double)m_CustomPlot->axisRect()->width(),
                       (event->pos().y()-m_CustomPlot->axisRect()->top())/(double)m_CustomPlot->axisRect()->height());
    rect.moveTopLeft(mousePoint-dragLegendOrigin);
    m_CustomPlot->axisRect()->insetLayout()->setInsetRect(0, rect);
    m_CustomPlot->replot();
  }

}

void TablePlotter::mousePressSignal(QMouseEvent *event)
{
  if (m_CustomPlot->legend->selectTest(event->pos(), false) > 0)
  {
    draggingLegend = true;
    // since insetRect is in axisRect coordinates (0..1), we transform the mouse position:
    QPointF mousePoint((event->pos().x()-m_CustomPlot->axisRect()->left())/(double)m_CustomPlot->axisRect()->width(),
                       (event->pos().y()-m_CustomPlot->axisRect()->top())/(double)m_CustomPlot->axisRect()->height());
    dragLegendOrigin = mousePoint-m_CustomPlot->axisRect()->insetLayout()->insetRect(0).topLeft();
  }
}

void TablePlotter::mouseReleaseSignal(QMouseEvent *event)
{
  Q_UNUSED(event)
  draggingLegend = false;
}

void TablePlotter::beforeReplot()
{
  // this is to prevent the legend from stretching if the plot is stretched.
  // Since we've set the inset placement to be ipFree, the width/height of the legend
  // is also defined in axisRect coordinates (0..1) and thus would stretch.
  // This is due to change in a future release (probably QCP 2.0) since it's basically a design mistake.
  m_CustomPlot->legend->setMaximumSize(m_CustomPlot->legend->minimumOuterSizeHint());
}




void TablePlotter::RemoveMatrixTable(int index, bool tab_already_removed)
{

    //remove text tabs
    if(!tab_already_removed)
    {
        textedits->removeTab(index);
    }
    //remove graphs from plot
    for(int i = 0; i < m_CustomPlot->graphCount(); i++)
    {
        //bool ok = false;
        MatrixTable* tab = (m_CustomPlot->graph(i)->property("sph_tab_index").value<MatrixTable*>());
        std::cout << tab << " " << m_Data.at(index) << " " << index << std::endl;
        if((tab == m_Data.at(index)))
        {
            m_CustomPlot->removeGraph(i);
        }
    }

    //remove graphs from list

    for(int i = m_ListWidget->count()-1; i >-1; i--)
    {
        QListWidgetItem* item = m_ListWidget->item(i);
        void* w = m_ListWidget->itemWidget(item);
        GraphStyleWidget * wi =(GraphStyleWidget *) w;
        if(wi->GetIndexTab() == m_Data.at(index))
        {
            QListWidgetItem* itemt = m_ListWidget->takeItem(i);
            if(itemt != nullptr)
            {
                delete itemt;
            }
        }
    }


    //remove table from internal list
    delete m_Data.at(index);
    m_Data.removeAt(index);

    m_CustomPlot->replot();

}

void TablePlotter::ClearMatrixTables()
{

    for(int i = m_Data.length()-1; i > -1; i--)
    {
        RemoveMatrixTable(i);
    }
}

void TablePlotter::AddMatrixTable(MatrixTable * tbl, bool one_x_list)
{
    MatrixTable*data = tbl->Copy();
    m_Data.append(data);


    //set actual data for the plot
    int cols = tbl->GetNumberOfCols();
    int rows = tbl->GetNumberOfRows();

    double hmax = -1e31;
    double hmin = 1e31;
    double xmin = 1e31;
    double xmax = -1e31;

    int ngraphs = 0;

    std::ranlux48 gen;
    std::uniform_int_distribution<int>  uniform_0_255(0, 255);

    if(!one_x_list)
    {
        ngraphs = cols/2;

        for(int i = 0;i < ngraphs; i++)
        {
            int n_items = tbl->GetNumberOfRowsAtColumn(i * 2);
            QVector<QCPGraphData> timeData(n_items);

            for(int j = 0; j < n_items; j++)
            {
                double val_x = tbl->GetValueDouble(j,i*2);
                double val_y = tbl->GetValueDouble(j,i*2+1);
                QCPGraphData gd;
                gd.key = val_x;
                gd.value = val_y;

                 timeData.replace(j,gd);

                 hmax = std::max(hmax,val_y);
                 hmin = std::min(hmin,val_y);
                 xmin = std::min(xmin,val_x);
                 xmax = std::max(xmax,val_x);
            }

            if(n_items > 0)
            {
                m_CustomPlot->addGraph();
                int gindex = m_CustomPlot->graphCount()-1;


                QColor color;

                if( gindex == 0)
                {
                    color = QColor(164,3,31);
                }else if(gindex == 1)
                {
                    color = QColor(41,19,96);
                }else if(gindex == 2)
                {
                    color = QColor(48,66,37);
                }else if(gindex == 3)
                {
                    color = QColor(48,66,37);
                }else if(gindex == 4)
                {
                    color = QColor(48,66,37);
                }else
                {
                    color = QColor(uniform_0_255(gen),uniform_0_255(gen),uniform_0_255(gen));
                }
                m_CustomPlot->graph(gindex)->setLineStyle(QCPGraph::lsLine);
                m_CustomPlot->graph(gindex)->setPen(QPen(color.lighter(200)));
                //m_CustomPlot->graph()->setBrush(QBrush(color));
                m_CustomPlot->graph(gindex)->data()->set(timeData);
                m_CustomPlot->graph(gindex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
                m_CustomPlot->graph(gindex)->setName(tbl->GetColumnTitleQ(i*2+1));
                m_CustomPlot->graph(gindex)->setProperty("sph_tab_index",QVariant::fromValue(data));

                QListWidgetItem * item1 = new QListWidgetItem(m_ListWidget);
                GraphStyleWidget * itemw1 = new GraphStyleWidget(m_ListWidget,gindex,data,m_CustomPlot);
                m_ListWidget->addItem(item1);
                m_ListWidget->setItemWidget(item1,itemw1);
                item1->setSizeHint(QSize(140,35));

            }
        }
    }else
    {
        ngraphs = cols-1;

        for(int i = 0;i < ngraphs; i++)
        {
            int n_items = tbl->GetNumberOfRowsAtColumn(i * 2);
            QVector<QCPGraphData> timeData(n_items);

            for(int j = 0; j < n_items; j++)
            {
                double val_x = tbl->GetValueDouble(j,0);
                double val_y = tbl->GetValueDouble(j,i+1);
                QCPGraphData gd;
                gd.key = val_x;
                gd.value = val_y;

                 timeData.replace(j,gd);

                 hmax = std::max(hmax,val_y);
                 hmin = std::min(hmin,val_y);
                 xmin = std::min(xmin,val_x);
                 xmax = std::max(xmax,val_x);
            }

            if(n_items > 0)
            {
                m_CustomPlot->addGraph();

                int gindex = m_CustomPlot->graphCount()-1;


                QColor color;

                if( gindex == 0)
                {
                    color = QColor(164,3,31);
                }else if(gindex == 1)
                {
                    color = QColor(41,19,96);
                }else if(gindex == 2)
                {
                    color = QColor(48,66,37);
                }else if(gindex == 3)
                {
                    color = QColor(48,66,37);
                }else if(gindex == 4)
                {
                    color = QColor(48,66,37);
                }else
                {
                    color = QColor(uniform_0_255(gen),uniform_0_255(gen),uniform_0_255(gen));
                }

                m_CustomPlot->graph(gindex)->setLineStyle(QCPGraph::lsLine);
                m_CustomPlot->graph(gindex)->setPen(QPen(color.lighter(200)));
                //m_CustomPlot->graph()->setBrush(QBrush(color));
                m_CustomPlot->graph(gindex)->data()->set(timeData);
                m_CustomPlot->graph(gindex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
                m_CustomPlot->graph(gindex)->setName(tbl->GetColumnTitleQ(i*2+1));
                m_CustomPlot->graph(gindex)->setName("Graph " + QString::number(gindex));
                m_CustomPlot->graph(gindex)->setProperty("sph_tab_index",QVariant::fromValue(data));

                QListWidgetItem * item1 = new QListWidgetItem(m_ListWidget);
                GraphStyleWidget * itemw1 = new GraphStyleWidget(m_ListWidget,gindex,data,m_CustomPlot);
                m_ListWidget->addItem(item1);
                m_ListWidget->setItemWidget(item1,itemw1);
                item1->setSizeHint(QSize(140,35));
            }
        }
    }

    if(m_Slider != nullptr && data->GetNumberOfCols() > 0)
    {
        m_Slider->blockSignals(true);
        m_Slider->setMinimum(xmin);
        m_Slider->setMaximum(xmax);
        m_Slider->blockSignals(false);
    }else if(m_Slider != nullptr)
    {
        m_Slider->blockSignals(true);
         xmin = m_Slider->minimum();
         xmax = m_Slider->maximum();
         m_Slider->blockSignals(false);
    }



    m_OldMinX = xmin;
    m_OldMaxX = xmax;
    m_OldMinY = hmin;
    m_OldMaxY = hmax;
    m_CustomPlot->xAxis->setRange(xmin,xmax);
    m_CustomPlot->yAxis->setRange(hmin,hmax);



    QString text;
    for(int i = 0; i < data->GetNumberOfRows(); i++)
    {
        for(int j = 0; j < data->GetNumberOfCols(); j++)
        {
            text = text + data->GetValueStringQ(i,j);
            text = text + "\t";
        }

        text = text + "\n";
    }
    QTextEdit * te = new QTextEdit();
    te->setReadOnly(true);
    te->setMinimumSize(400,200);
    te->setText(text);

    textedits->addTab(te,"Table " + QString::number(m_Data.length()-1));

    m_CustomPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    m_CustomPlot->legend->setFont(legendFont);
    m_CustomPlot->legend->setSelectedFont(legendFont);
    m_CustomPlot->legend->setSelectableParts(QCPLegend::spItems);


    m_CustomPlot->replot();


}

void TablePlotter::SetFromMatrixTable(MatrixTable * tbl, bool one_x_list)
{
    ClearMatrixTables();
    AddMatrixTable(tbl,one_x_list);
}

void TablePlotter::OnActionTriggered(QAction * a)
{
    /*QMenu * m = mb->addMenu("File");
    m->addAction("Open Data In Table View");
    m->addSeparator();
    m->addAction("Add Table File");
    m->addSeparator();
    m->addAction("Export");
    m->addSeparator();
    m->addAction("Close");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    m = mb->addMenu("Elements");
    m->addAction("Set Plot Title");
    m->addAction("Set Horizontal Axis Label");
    m->addAction("Set Vertical Axis Label");
    m->addAction("Toggle Legend");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);

    m = mb->addMenu("Style");
    m->addAction("Set Uniform Color");
    m->addAction("Set BackGround Color");
    m->addAction("Set Text Color");
    connect(m,&QMenu::triggered,this,&TablePlotter::OnActionTriggered);*/


    QString name = a->text();

    if(name.compare("Open Data In Table View") == 0)
    {
        MatrixTable * t = m_Data.at(textedits->currentIndex());
        LayerInfoWidget *w = new LayerInfoWidget(t->Copy());
        w->show();
    }

    if(name.compare("Add Table File") == 0)
    {
        QList<QString> paths = QFileDialog::getOpenFileNames(this,	QString("Select a table file:"),
                                                    "",QString("*.tbl *.csv *.txt;;*.*"));

        for(int i = 0; i < paths.length(); i++)
        {
            QString path = paths.at(i);
            if(!path.isEmpty())
            {
                MatrixTable * t = new MatrixTable();
                t->LoadAsAutoFormat(path);
                this->AddMatrixTable(t);
                delete t;
           }
        }
    }

    if(name.compare("Set Plot Title") == 0)
    {
        QString currentTitle = m_CustomPlot->xAxis2->label();
        QString text = QInputDialog::getText(this,"Plot Title","Provide plot title",QLineEdit::Normal,currentTitle);
        m_CustomPlot->xAxis2->setLabel(text);
    }

    if(name.compare("Set Horizontal Axis Label") == 0)
    {
        QString currentTitle = m_CustomPlot->xAxis->label();
        QString text = QInputDialog::getText(this,"Plot Title","Provide horizontal axis label",QLineEdit::Normal,currentTitle);
        m_CustomPlot->xAxis->setLabel(text);
    }

    if(name.compare("Set Vertical Axis Label") == 0)
    {
        QString currentTitle = m_CustomPlot->yAxis->label();
        QString text = QInputDialog::getText(this,"Plot Title","Provide vertical axis label",QLineEdit::Normal,currentTitle);
        m_CustomPlot->yAxis->setLabel(text);
    }

    if(name.compare("Toggle Legend") == 0)
    {
        bool visible = m_CustomPlot->legend->visible();
        m_CustomPlot->legend->setVisible(!visible);
    }

    if(name.compare("Export to Table") == 0)
    {
        QString name = QFileDialog::getSaveFileName(this,"Save graph to Table","","*.tbl;*.csv;;*.*");

        if(!name.isEmpty())
        {
            MatrixTable * t = m_Data.at(textedits->currentIndex());
            t->SaveAsAutoFormat(name);
        }
    }

    if(name.compare("Export to Image") == 0)
    {
        QString name = QFileDialog::getSaveFileName(this,"Save graph to image","","*.jpg;*.png;;*.*");
        if(!name.isEmpty())
        {
            m_CustomPlot->grab().save(name);
        }

    }

    if(name.compare("Set Uniform Plot Color"))
    {

        QColor c = QColorDialog::getColor();

        if(c.isValid())
        {
            for(int i = 0; i < m_ListWidget->count(); ++i)
            {
                QListWidgetItem* item = m_ListWidget->item(i);
                GraphStyleWidget * widget = (GraphStyleWidget *) m_ListWidget->itemWidget(item);

                widget->m_ColorButton->SetColor(c);
                //Do stuff!
            }
        }

    }

    if(name.compare("Set Default Plot Color"))
    {

        for(int i = 0; i < m_ListWidget->count(); ++i)
        {
            QListWidgetItem* item = m_ListWidget->item(i);
            GraphStyleWidget * widget = (GraphStyleWidget *) m_ListWidget->itemWidget(item);


            std::ranlux48 gen;
            std::uniform_int_distribution<int>  uniform_0_255(0, 255);

            int gindex = i;
            QColor color;

            if( gindex == 0)
            {
                color = QColor(164,3,31);
            }else if(gindex == 1)
            {
                color = QColor(41,19,96);
            }else if(gindex == 2)
            {
                color = QColor(48,66,37);
            }else if(gindex == 3)
            {
                color = QColor(48,66,37);
            }else if(gindex == 4)
            {
                color = QColor(48,66,37);
            }else
            {
                color = QColor(uniform_0_255(gen),uniform_0_255(gen),uniform_0_255(gen));
            }

            widget->m_ColorButton->SetColor(color);
        }
    }

    if(name.compare("Set BackGround Color"))
    {
        QBrush B;
        QColor c = QColorDialog::getColor();
        if(c.isValid())
        {
            B.setColor(c);
            m_CustomPlot->setBackground(B);
        }

    }
    if(name.compare("Set Text Color"))
    {
        QColor c = QColorDialog::getColor();

        if(c.isValid())
        {
            m_CustomPlot->xAxis->setBasePen(QPen(c));
            m_CustomPlot->xAxis->setTickPen(QPen(c));
            m_CustomPlot->xAxis->setSubTickPen(QPen(c));
            m_CustomPlot->xAxis->setTickLabelColor(c);
            m_CustomPlot->xAxis->setLabelColor(c);

            m_CustomPlot->xAxis2->setBasePen(QPen(c));
            m_CustomPlot->xAxis2->setTickPen(QPen(c));
            m_CustomPlot->xAxis2->setSubTickPen(QPen(c));
            m_CustomPlot->xAxis2->setTickLabelColor(c);
            m_CustomPlot->xAxis2->setLabelColor(c);

            m_CustomPlot->yAxis->setBasePen(QPen(c));
            m_CustomPlot->yAxis->setTickPen(QPen(c));
            m_CustomPlot->yAxis->setSubTickPen(QPen(c));
            m_CustomPlot->yAxis->setTickLabelColor(c);
            m_CustomPlot->yAxis->setLabelColor(c);

            m_CustomPlot->yAxis2->setBasePen(QPen(c));
            m_CustomPlot->yAxis2->setTickPen(QPen(c));
            m_CustomPlot->yAxis2->setSubTickPen(QPen(c));
            m_CustomPlot->yAxis2->setTickLabelColor(c);
            m_CustomPlot->yAxis2->setLabelColor(c);
        }

    }
    if(name.compare("Zoom to all"))
    {
        m_CustomPlot->xAxis->setRange(m_OldMinX,m_OldMaxX);
        m_CustomPlot->yAxis->setRange(m_OldMinY,m_OldMaxY);

    }

    m_CustomPlot->replot();

}

void TablePlotter::graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent * me)
{
    std::cout << "graph clicked " << std::endl;
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  //double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  //QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  //ui->statusBar->showMessage(message, 2500);
}
void TablePlotter::graphDClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent * me)
{
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  //double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  //QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  //ui->statusBar->showMessage(message, 2500);
}

void TablePlotter::SetTime(int x)
{
    m_Time = x;
    if(m_Slider != nullptr)
    {
        m_Slider->setValue(x);
    }

}

void TablePlotter::OnTabClose(int index)
{
    RemoveMatrixTable(index);

}

int TablePlotter::GetTime()
{
    return m_Time;
}

void TablePlotter::OnTimeSliderValueChanged(int x)
{
    m_Time = x;
    emit OnTimeChanged();
}
