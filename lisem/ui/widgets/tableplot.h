#ifndef TABLEPLOT_H
#define TABLEPLOT_H

#include "QWidget"
#include "QLineEdit"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include "QLabel"
#include "QDialog"
#include "QPushButton"
#include "QTableView"
#include "tablemodel.h"
#include "QMenuBar"
#include "QCoreApplication"
#include "lsmio.h"
#include "matrixtable.h"
#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX
#endif
#include <glad/glad.h>

#include "widgets/labeledwidget.h"
#include "qtplot/qcustomplot.h"
#include "switchbutton.h"
#include "colorbutton.h"


class GraphStyleWidget : public QWidget
{
    Q_OBJECT;
    QHBoxLayout * m_Layout;
    QLineEdit* m_Name;
    QCustomPlot *m_Plot;
    SwitchButton * m_DrawButton;
    SwitchButton * m_FillButton;
    SwitchButton * m_LineButton;
    SwitchButton * m_MarkButton;

    int m_index = 0;
    MatrixTable* m_indextab = 0;

public:
    ColorButton * m_ColorButton;


public:
    inline GraphStyleWidget(QWidget * parent, int index, MatrixTable* index_tab, QCustomPlot *plot) : QWidget(parent)
    {
        m_indextab = index_tab;
        m_index= index;
        m_Plot = plot;
        m_Layout = new QHBoxLayout();
        setLayout(m_Layout);

        m_Name = new QLineEdit();
        m_Name->setText("Graph " + QString::number(index));

        m_DrawButton = new SwitchButton();
        m_DrawButton->setValue(true);

        m_Layout->addWidget(m_Name);
        m_Layout->addWidget(new QLabeledWidget("Draw",m_DrawButton));

        m_FillButton = new SwitchButton();
        m_FillButton->setValue(false);

        m_MarkButton = new SwitchButton();
        m_MarkButton->setValue(true);

        m_LineButton = new SwitchButton();
        m_LineButton->setValue(true);

        //pen style

        //point style


        //pen color
        m_ColorButton = new ColorButton();
        m_ColorButton->SetColor(m_Plot->graph(m_index)->pen().color());

        connect(m_ColorButton, &ColorButton::OnColorChanged,this,&GraphStyleWidget::OnColorChanged);

        m_Layout->addWidget(m_Name);
        m_Layout->addWidget(new QLabeledWidget("Draw",m_DrawButton));
        m_Layout->addWidget(new QLabeledWidget("Fill",m_FillButton));
        m_Layout->addWidget(new QLabeledWidget("Line",m_LineButton));
        m_Layout->addWidget(new QLabeledWidget("Marker",m_MarkButton));
        m_Layout->addWidget(m_ColorButton);


        m_Layout->setMargin(1);

        connect(m_Name,&QLineEdit::textChanged,this,&GraphStyleWidget::OnNameChanged);
        connect(m_DrawButton,&SwitchButton::valueChanged,this,&GraphStyleWidget::OnDrawChanged);
        connect(m_FillButton,&SwitchButton::valueChanged,this,&GraphStyleWidget::OnFillChanged);

        connect(m_LineButton,&SwitchButton::valueChanged,this,&GraphStyleWidget::OnLineChanged);
        connect(m_MarkButton,&SwitchButton::valueChanged,this,&GraphStyleWidget::OnMarkChanged);
    }
    inline MatrixTable * GetIndexTab()
    {
        return m_indextab;
    }

public slots:

    inline void OnColorChanged()
    {
        QPen p = m_Plot->graph(m_index)->pen();
        p.setColor(m_ColorButton->GetColorQ());
        m_Plot->graph(m_index)->setPen(p);
        m_Plot->replot();
    }

    inline void OnNameChanged(QString n)
    {
        m_Plot->graph(m_index)->setName(n);
        m_Plot->replot();
    }

    inline void OnDrawChanged(bool x)
    {
         m_Plot->graph(m_index)->setVisible(x);
         m_Plot->legend->item(m_index)->setVisible(x);
         m_Plot->replot();
    }
    inline void OnFillChanged(bool x)
    {
        if(x)
        {
            QBrush b = m_Plot->graph(m_index)->brush();
            b.setStyle(Qt::SolidPattern);
            QColor c=m_Plot->graph(m_index)->pen().color();
            c.setAlpha(100);
            b.setColor(c);
            m_Plot->graph(m_index)->setBrush(b);
            m_Plot->replot();
        }else
        {
            QBrush b = m_Plot->graph(m_index)->brush();
            b.setStyle(Qt::NoBrush);
            b.setColor(QColor(0,0,255,0));
            m_Plot->graph(m_index)->setBrush(b);
            m_Plot->replot();
        }

    }

    inline void OnLineChanged(bool x)
    {
        if(x)
        {
            m_Plot->graph(m_index)->setLineStyle(QCPGraph::lsLine);
            m_Plot->replot();
        }else
        {
            m_Plot->graph(m_index)->setLineStyle(QCPGraph::lsNone);
            m_Plot->replot();
        }

    }

    inline void OnMarkChanged(bool x)
    {
        if(x)
        {
            m_Plot->graph(m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
            m_Plot->replot();
        }else
        {
            m_Plot->graph(m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 4));
            m_Plot->replot();
        }

    }
};

class TablePlotter : public QDialog
{
    Q_OBJECT;

    QWidget * m_MainWidget = nullptr;
    QVBoxLayout * m_MainLayout = nullptr;

    QString m_Dir;

    QSlider * m_Slider = nullptr;

    double m_OldMinX = 0.0;
    double m_OldMaxX = 1.0;
    double m_OldMinY = 0.0;
    double m_OldMaxY = 1.0;

    QList<MatrixTable * >m_Data ;
    QCustomPlot * m_CustomPlot = nullptr;

    QLabel * m_SampleLabel = nullptr;
    QTabWidget * textedits = nullptr;

    QListWidget * m_ListWidget = nullptr;

    int m_Time = 0;
public:
    TablePlotter(MatrixTable * tbl_in , bool one_x_list = true,bool has_time_slider = false);
    inline ~TablePlotter()
    {
        for(int i = 0; i < m_Data.length(); i++)
        {
               delete m_Data.at(i);
        }
    }
public:

    void ClearMatrixTables();
    void RemoveMatrixTable(int index, bool tab_already_removed = false);
    void AddMatrixTable(MatrixTable * tbl_in , bool one_x_list = true);
    void SetFromMatrixTable(MatrixTable * tbl_in , bool one_x_list = true);
    void SetTime(int);
    int GetTime();

private:

    bool draggingLegend;
    QPointF dragLegendOrigin;

private slots:
  void mouseMoveSignal(QMouseEvent *event);
  void mousePressSignal(QMouseEvent *event);
  void mouseReleaseSignal(QMouseEvent *event);
  void beforeReplot();


public slots:
    void OnActionTriggered(QAction * a);
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent * me);
    void graphDClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent * me);
    void OnTimeSliderValueChanged(int);
    void OnTabClose(int);
signals:
    void OnTimeChanged();

};

#endif // TABLEPLOT_H
