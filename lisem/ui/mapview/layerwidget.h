#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H


#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QString>
#include "error.h"
#include "widgets/layerinfowidget.h"
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
#include <QListView>
#include <QItemDelegate>
#include <QPainter>
#include <QStringListModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QImage>
#include <QPixmap>
#include <QSlider>
#include "lsmio.h"
#include <QToolButton>
#include "glplot/worldwindow.h"
#include <QDialog>
#include <QPushButton>
#include <QSignalMapper>
#include <QColorDialog>
#include <QMenu>
#include "widgets/gradientselectionwidget.h"
#include "layer/uilayer.h"
#include "widgets/crsselectionwidget.h"
#include "layer/geo/uigeolayer.h"
#include "widgets/gradientcombobox.h"
#include "widgets/layerstylewidget.h"
#include "editorwidget.h"
#include "widgets/tableplot.h"
#include "layerparameterwidget.h"
#include "clabel.h"
#include "site.h"

class LayerWidget : public QWidget
{
        Q_OBJECT
        QHBoxLayout * m_Layout;
        QCheckBox * m_CheckBoxDraw;
        CLabel * m_Name;
        CLabel * m_File;
        QImage * m_Gradient;
        GradientComboBox * m_labelGradient;
        QSlider * m_Slider;
        QCheckBox * m_CheckBoxDEM;
        QCheckBox * m_CheckBoxHS;
        QCheckBox * m_CheckBoxLeg;
        QToolButton *DelButton;
        QDoubleSpinBox * m_SpinMin;
        QDoubleSpinBox * m_SpinMax;
        QToolButton * m_GetMinMaxButton;

        UILayer *m_maplayer = nullptr;
        WorldWindow * m_window;

        QAction *cInfoAct;
        QAction *cRemoveAct;
        QAction *cSaveAct;
        QAction *cDuplicateAct;
        QAction *cStyleAct;
        QAction *cStyleCAct;
        QAction *cStylePAct;
        QAction *cZoomAct;
        QAction *cParameterAct;
        QAction *cCRSAct;
        QAction *cCRSCAct;
        QAction *cCRSPAct;
        QAction *cCRSWAct;
        QAction *cTimeSeriesAct;
        QAction *cProfileAct;
        QAction *cSpectraAct;
        QAction *cEditAct;
        QAction *cStopEditAct;

        QTabWidget * m_ParentTabWidget;

        EditorWidget * m_EditorWidget = nullptr;

        UILayerEditor * editor;
        LayerStyleWidget *m_StyleWidget = nullptr;
        LayerParameterWidget *m_ParameterWidget = nullptr;

        TablePlotter * m_TimeSeriesPlotter = nullptr;
        TablePlotter * m_ProfilePlotter = nullptr;
        TablePlotter * m_SpectralPlotter = nullptr;

        QMutex  m_layerMutex;

public:

        inline LayerWidget( QWidget *parent, UILayer *ml,WorldWindow * w, QTabWidget * t): QWidget( parent)
        {

            m_ParentTabWidget = t;
            m_Layout = new QHBoxLayout();
            m_window = w;
            setLayout(m_Layout);

            m_Name = new CLabel("Name");
            m_Name->setMinimumSize(75,20);
            m_CheckBoxDraw = new QCheckBox("");
            m_File = new CLabel("File");
            m_File->setMinimumSize(100,20);
            m_Gradient = new QImage(125,25,QImage::Format_RGBA8888);


            m_Slider = new QSlider();
            m_Slider->setOrientation(Qt::Horizontal);
            m_Slider->setMinimum(0);
            m_Slider->setMaximum(100);
            m_Slider->setValue(100);
            m_Slider->setMaximumSize(125,25);
            m_Slider->setMinimumSize(25,10);

            QIcon *icon_delete = new QIcon();
            QString m_Dir = GetSite();
            icon_delete->addFile((m_Dir + LISEM_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);

            QIcon *icon_re = new QIcon();
            icon_re->addFile((m_Dir + LISEM_FOLDER_ASSETS + "refresh.png"), QSize(), QIcon::Normal, QIcon::Off);


            DelButton = new QToolButton();
            DelButton->setIcon(*icon_delete);
            DelButton->setIconSize(QSize(22,22));
            DelButton->resize(22,22);
            DelButton->setEnabled(ml->IsNative());

            m_GetMinMaxButton = new QToolButton();
            m_GetMinMaxButton->setIcon(*icon_re);
            m_GetMinMaxButton->setIconSize(QSize(22,22));
            m_GetMinMaxButton->resize(22,22);

            m_CheckBoxDEM = new QCheckBox("DEM");
            m_CheckBoxHS = new QCheckBox("HS");
            m_CheckBoxLeg = new QCheckBox("Leg");

            m_SpinMax = new QDoubleSpinBox();
            m_SpinMax->setValue(0.0);
            m_SpinMax->setDecimals(6);
            m_SpinMax->setMaximum(1e10);
            m_SpinMax->setMinimum(-1e10);

            m_SpinMin = new QDoubleSpinBox();
            m_SpinMin->setValue(0.0);
            m_SpinMin->setDecimals(6);
            m_SpinMin->setMaximum(1e10);
            m_SpinMin->setMinimum(-1e10);

            m_Layout->addWidget(m_CheckBoxDraw);
            m_Layout->addWidget(m_Name);
            m_Layout->addWidget(m_File);

            if(ml->GetStyle().IsSimpleStyleGradient())
            {
                m_labelGradient = new GradientComboBox();
                m_labelGradient->SetCurrentGradient(ml->GetStyle().m_ColorGradientb1);
                connect(m_labelGradient,SIGNAL(OnGradientChanged()),this,SLOT(OnGradientClicked()));
                m_Layout->addWidget(m_labelGradient);
            }
            if(ml->GetStyle().IsSimpleStyleRange())
            {
                m_Layout->addWidget(m_SpinMin);
                m_Layout->addWidget(m_SpinMax);
                m_Layout->addWidget(m_GetMinMaxButton);
                connect(m_GetMinMaxButton,SIGNAL(pressed()),this,SLOT(OnUpdateMinMax()));
            }
            m_Layout->addWidget(m_Slider);
            m_Layout->addWidget(DelButton);
            if(ml->CouldBeDEM())
            {
                m_Layout->addWidget(m_CheckBoxDEM);
                m_Layout->addWidget(m_CheckBoxHS);
            }
            if(ml->HasLegend())
            {
                m_Layout->addWidget(m_CheckBoxLeg);
            }

            m_Layout->setStretchFactor(m_CheckBoxDraw,2);
            m_Layout->setStretchFactor(m_Name,4);
            m_Layout->setStretchFactor(m_labelGradient,4);
            m_Layout->setStretchFactor(m_Slider,5);
            m_Layout->setStretchFactor(m_File,5);
            m_Layout->setStretchFactor(DelButton,1);
            m_Layout->setStretchFactor(m_CheckBoxDEM,1);
            m_Layout->setStretchFactor(m_CheckBoxLeg,1);
            m_Layout->setStretchFactor(m_SpinMin,1);
            m_Layout->setStretchFactor(m_SpinMax,1);

            connect(m_CheckBoxDraw,SIGNAL(clicked(bool)),this,SLOT(OnDrawChecked(bool)));
            connect(m_CheckBoxDEM,SIGNAL(clicked(bool)),this,SLOT(OnDEMChecked(bool)));
            connect(m_CheckBoxHS,SIGNAL(clicked(bool)),this,SLOT(OnHSChecked(bool)));
            connect(m_CheckBoxLeg,SIGNAL(clicked(bool)),this,SLOT(OnLegChecked(bool)));
            connect(m_Slider,SIGNAL(valueChanged(int)),this, SLOT( OnTransparancyChanged(int)));
            connect(DelButton,SIGNAL(pressed()),this,SLOT(OnDeleteButtonChecked()));//this->setMaximumSize(20000,40);

            connect(m_SpinMax,SIGNAL(valueChanged(double)),this,SLOT(OnMaxChanged(double)));
            connect(m_SpinMin,SIGNAL(valueChanged(double)),this,SLOT(OnMinChanged(double)));

            SetMapLayer(ml);


            cInfoAct = new QAction("Info",this);
            cRemoveAct = new QAction("Remove",this);
            cSaveAct = new QAction("Save As",this);
            cDuplicateAct = new QAction("Duplicate",this);
            cStyleAct = new QAction("Style",this);
            cStyleCAct = new QAction("Copy Style",this);
            cStylePAct = new QAction("Paste Style",this);
            cParameterAct = new QAction("Parameters",this);
            cZoomAct = new QAction("ZoomToExtent",this);
            cCRSAct = new QAction("Set CRS",this);
            cCRSCAct = new QAction("Copy CRS",this);
            cCRSPAct = new QAction("Paste CRS",this);
            cCRSWAct = new QAction("Set World CRS from Layer",this);
            cTimeSeriesAct = new QAction("Show TimeSeries",this);
            cProfileAct = new QAction("Show Profile",this);
            cSpectraAct = new QAction("Show Spectra",this);
            cEditAct = new QAction("Edit",this);
            cStopEditAct = new QAction("Stop Edit",this);

            connect(cInfoAct,SIGNAL(triggered()),this,SLOT(OnLayerInfo()));
            connect(cRemoveAct,SIGNAL(triggered()),this,SLOT(OnLayerRemove()));
            connect(cSaveAct ,SIGNAL(triggered()),this,SLOT(OnLayerSave()));
            connect(cDuplicateAct,SIGNAL(triggered()),this,SLOT(OnLayerDuplicate()));
            connect(cStyleAct,SIGNAL(triggered()),this,SLOT(OnLayerStyle()));
            connect(cStyleCAct,SIGNAL(triggered()),this,SLOT(OnLayerStyleC()));
            connect(cStylePAct,SIGNAL(triggered()),this,SLOT(OnLayerStyleP()));
            connect(cParameterAct,SIGNAL(triggered()),this,SLOT(OnLayerParameter()));
            connect(cZoomAct,SIGNAL(triggered()),this,SLOT(OnLayerZoom()));
            connect(cCRSAct,SIGNAL(triggered()),this,SLOT(OnLayerCRS()));
            connect(cCRSCAct,SIGNAL(triggered()),this,SLOT(OnLayerCRSC()));
            connect(cCRSPAct,SIGNAL(triggered()),this,SLOT(OnLayerCRSP()));
            connect(cCRSWAct,SIGNAL(triggered()),this,SLOT(OnLayerCRSW()));
            connect(cTimeSeriesAct,SIGNAL(triggered()),this,SLOT(OnTimeSeries()));
            connect(cProfileAct,SIGNAL(triggered()),this,SLOT(OnProfile()));
            connect(cSpectraAct,SIGNAL(triggered()),this,SLOT(OnSpectra()));
            connect(cEditAct,SIGNAL(triggered()),this,SLOT(OnLayerEdit()));
            connect(cStopEditAct,SIGNAL(triggered()),this,SLOT(OnStopLayerEdit()));
        }

        inline void Destroy()
        {
            delete cInfoAct;
            delete cRemoveAct;
            delete cSaveAct;
            delete cDuplicateAct;
            delete cStyleAct;
            delete cZoomAct;
            delete cCRSAct;
            delete cEditAct;
        }

        inline void SetMapLayer(UILayer *ml, bool update = true)
        {
            m_layerMutex.lock();

            if(ml == m_maplayer)
            {
                if(m_StyleWidget != nullptr)
                {
                    //if the styles are not functionally equivalent (map vs shape, different type of these, close the popup).
                    if(!ml->GetStyleRef()->FunctionalEquivalence(m_maplayer->GetStyleRef()))
                    {
                        m_StyleWidget->close();
                        delete m_StyleWidget;
                        m_StyleWidget = nullptr;
                    }else
                    {
                        m_StyleWidget->SetLayer(ml);
                    }
                }

            }


            m_maplayer = ml;


            //do checks for each widget if update is needed, to create smoother ui

            if(update)
            {
                if(m_Name->text() != ml->GetName())
                {
                    m_Name->setText(ml->GetName());
                }
                if(m_CheckBoxDraw->isChecked() != ml->IsDraw())
                {
                    m_CheckBoxDraw->blockSignals(true);
                    m_CheckBoxDraw->setChecked(ml->IsDraw());
                    m_CheckBoxDraw->blockSignals(false);
                }
                if(m_File->text() != ml->GetFilePath())
                {
                    m_File->setText(ml->GetFilePath());
                }

                //UpdateGradient();

                if(ml->GetStyle().IsSimpleStyleGradient())
                {
                    if(!(m_labelGradient->GetCurrentGradient() == ml->GetStyle().m_ColorGradientb1))
                    {
                        m_labelGradient->blockSignals(true);
                        m_labelGradient->SetCurrentGradient(ml->GetStyle().m_ColorGradientb1);
                        m_labelGradient->blockSignals(false);

                    }
                }

                if(ml->CouldBeDEM())
                {
                    if(m_CheckBoxHS->isChecked() != ml->IsDrawAsHillShade() || m_CheckBoxDEM->isChecked() != ml->IsDrawAsDEM())
                    {
                        m_CheckBoxDEM->blockSignals(true);
                        m_CheckBoxHS->blockSignals(true);
                        m_CheckBoxDEM->setChecked(ml->IsDrawAsDEM());
                        m_CheckBoxHS->setChecked(ml->IsDrawAsHillShade());
                        m_CheckBoxDEM->blockSignals(false);
                        m_CheckBoxHS->blockSignals(false);
                    }
                }

                if(m_Slider->value() != (int)((1.0f-ml->GetStyle().m_Transparancy)*100.0f))
                {
                    m_Slider->blockSignals(true);
                    m_Slider->setValue((1.0f-ml->GetStyle().m_Transparancy)*100.0f);
                    m_Slider->blockSignals(false);
                }
                if(ml->IsNative() || ml->IsBeingEdited())
                {
                    if(DelButton->isEnabled())
                    {
                        DelButton->setDisabled(true);
                    }
                }else
                {
                    if(!DelButton->isEnabled())
                    {
                        DelButton->setDisabled(false);
                    }
                }
            }

            m_layerMutex.unlock();
        }

        inline void UpdateGradient()
        {


        }

        inline UILayer *GetMapLayer()
        {
            return m_maplayer;
        }



        inline void contextMenuEvent(QContextMenuEvent *event) override
        {

            QClipboard *clipboard = QApplication::clipboard();
            QString originalText = clipboard->text();

            bool has_clipboard_crs = false;
            bool has_clipboard_style = false;
            if(originalText.startsWith("<OPENLISEM_HAZARD_CRSCOPYSTRING>"))
            {
                has_clipboard_crs = true;
            }
            if(originalText.startsWith("<OPENLISEM_HAZARD_STYLECOPYSTRING>"))
            {
                has_clipboard_style = true;
            }


            //check if clipboard text is recognized as CRS or Style Object

            m_layerMutex.lock();

            QMenu menu(this);

            menu.addAction(cInfoAct);

            if(m_maplayer->IsEditAble())
            {
                if(m_maplayer->IsBeingEdited())
                {
                    cRemoveAct->setEnabled(false);
                }else {
                   cRemoveAct->setEnabled(true);
                }
            }

            menu.addAction(cRemoveAct);
            if(m_maplayer->IsSaveAble())
            {
                menu.addAction(cSaveAct);
            }
            if(m_maplayer->IsLayerSaveAble())
            {
                menu.addAction(cDuplicateAct);
            }
            menu.addAction(cStyleAct);
            menu.addAction(cStyleCAct);
            if(has_clipboard_style)
            {
                menu.addAction(cStylePAct);
            }
            if(m_maplayer->HasParameters())
            {
                menu.addAction(cParameterAct);
            }
            if(m_maplayer->IsGeo())
            {
                menu.addAction(cZoomAct);
                menu.addAction(cCRSAct);
                menu.addAction(cCRSCAct);
                if(has_clipboard_crs)
                {
                    menu.addAction(cCRSPAct);
                }
                menu.addAction(cCRSWAct);
            }
            if(m_maplayer->IsTimeSeriesAble())
            {
                menu.addAction(cTimeSeriesAct);


            }
            if(m_maplayer->IsProfileAble())
            {
                menu.addAction(cProfileAct);
            }
            if(m_maplayer->IsSpectraAble())
            {
                menu.addAction(cSpectraAct);
            }
            if(m_maplayer->IsEditAble())
            {
                if(!m_maplayer->IsBeingEdited())
                {
                    menu.addAction(cEditAct);
                }else
                {
                    menu.addAction(cStopEditAct);
                }
            }

            m_layerMutex.unlock();

            menu.exec(event->globalPos());
        }

        inline void OnFocusLocationChanged(QList<LSMVector2> locs)
        {

            m_layerMutex.lock();

            if(m_TimeSeriesPlotter != nullptr)
            {
                //if(m_TimeSeriesPlotter->isVisible())
                {

                    if(m_maplayer != nullptr && m_TimeSeriesPlotter->isVisible())
                    {
                        LSMStyle s = m_maplayer->GetStyle();

                        MatrixTable * tbl = m_maplayer->GetTimeSeries(locs,m_window->GetCurrentProjection());

                        m_TimeSeriesPlotter->SetFromMatrixTable(tbl,true);
                    }

                }
            }

            if(m_SpectralPlotter != nullptr)
            {
                if(m_maplayer != nullptr&& m_SpectralPlotter->isVisible())
                {
                    MatrixTable * tbl = m_maplayer->GetSpectra(locs,m_window->GetCurrentProjection());

                    m_SpectralPlotter->SetFromMatrixTable(tbl,true);
                }

            }

            if(m_ProfilePlotter != nullptr)
            {
                if(m_maplayer != nullptr && m_ProfilePlotter->isVisible())
                {
                    MatrixTable * tbl = m_maplayer->GetProfile(locs,m_window->GetCurrentProjection());

                    m_ProfilePlotter->SetFromMatrixTable(tbl,true);
                }

            }

            m_layerMutex.unlock();

        }


public slots:

        inline void OnUpdateMinMax()
        {
            m_layerMutex.lock();


            m_SpinMax->blockSignals(true);
            m_SpinMin->blockSignals(true);

            double valmax = (m_maplayer->GetMaximumValue(0));
            double valmin = m_maplayer->GetMinimumValue(0);
            m_SpinMax->setValue(valmax);
            m_SpinMin->setValue(valmin);
            m_SpinMax->blockSignals(false);
            m_SpinMin->blockSignals(false);

            LSMStyle s = m_maplayer->GetStyle();
            s.m_Intervalb1.SetMin(valmin);
            s.m_Intervalb2.SetMax(valmax);
            m_maplayer->SetStyle(s);

            m_layerMutex.unlock();
        }

        inline void OnLayerInfo()
        {
            m_layerMutex.lock();

            LayerInfoWidget *w = new LayerInfoWidget(m_maplayer->GetInfo().GetMatrixTable());
            m_layerMutex.unlock();
            w->show();
        }

        inline void OnLayerRemove()
        {
            OnDeleteButtonChecked();
        }

        inline void OnLayerSave()
        {
            QList<QString> exts =m_maplayer->GetSaveExtHint();
            QString name =  m_maplayer->GetSaveNameHint();
            QString dir = m_maplayer->GetSaveDirHint();

            //get file name from user


            QString extss;
            for(int i = 0; i < exts.length(); i++)
            {
                extss += QString("*") + exts.at(i);
                if(i != exts.length())
                {
                    extss += ";";
                }
            }


            exts += ";*.*";
            QString path = QFileDialog::getOpenFileName(this,QString("Select a save file"),
                                                        m_maplayer->GetFilePath(),extss);


            if(!path.isEmpty())
            {

                //now ask layer to do the saving!

                bool success = m_maplayer->SaveLayerToFile(path);
                if(!success)
                {
                    QMessageBox msgBox;
                    msgBox.setText("An error occured when saving file to: " + path);
                    msgBox.exec();
                }
            }

        }

        inline void OnLayerDuplicate()
        {
            QJsonObject obj;

            QJsonArray layerJsonArray;


                    QString l_typename = m_maplayer->layertypeName();

                        //the layer type is registered

                    QJsonObject lobj;
                    m_maplayer->SaveLayer(&lobj);
                    lobj["TypeName"] = l_typename;
                    layerJsonArray.append(lobj);

            (obj)["layers"] = layerJsonArray;

            m_window->LoadFrom(&obj,false);

        }

        inline void OnLayerStyle()
        {
            m_layerMutex.lock();

            m_StyleWidget = new LayerStyleWidget(m_maplayer);

            m_StyleWidget->show();

            m_layerMutex.unlock();
        }

        inline void OnLayerStyleC()
        {

            QClipboard *clipboard = QApplication::clipboard();

            QString text = "<OPENLISEM_HAZARD_STYLECOPYSTRING>";
            QJsonObject obj;
            m_maplayer->GetStyleRef()->Save(&obj);

            QJsonDocument doc(obj);
            QString strJson(doc.toJson(QJsonDocument::Compact));

            clipboard->setText(text + strJson);

        }
        inline void OnLayerStyleP()
        {
            QClipboard *clipboard = QApplication::clipboard();
            QString text= clipboard->text();

            if(text.startsWith("<OPENLISEM_HAZARD_STYLECOPYSTRING>"))
            {
                text.remove("<OPENLISEM_HAZARD_STYLECOPYSTRING>");
                QJsonObject obj;

                QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());

                // check validity of the document
                if(!doc.isNull())
                {
                    if(doc.isObject())
                    {
                        obj = doc.object();
                        m_maplayer->GetStyleRef()->Restore(&obj);

                        //update widgets
                        this->SetMapLayer(m_maplayer);
                    }
                    else
                    {
                        //qDebug() << "Document is not an object" << endl;
                    }
                }
                else
                {
                    //qDebug() << "Invalid JSON...\n" << in << endl;
                }
            }




        }

        inline void OnLayerParameter()
        {
            m_layerMutex.lock();

            if(!(m_ParameterWidget == nullptr))
            {
                delete m_ParameterWidget;
            }
            m_ParameterWidget = new LayerParameterWidget(m_maplayer);
            m_ParameterWidget->show();

            m_layerMutex.unlock();
        }


        inline void OnLayerZoom()
        {
            m_layerMutex.lock();

            if(m_maplayer->IsGeo())
            {
                UIGeoLayer *gl = ((UIGeoLayer *)(m_maplayer));
                m_window->LookAt(gl);
            }

            m_layerMutex.unlock();
        }

        inline void OnLayerCRSC()
        {
            QClipboard *clipboard = QApplication::clipboard();

            QString text = "<OPENLISEM_HAZARD_CRSCOPYSTRING>";
            if(m_maplayer->IsGeo())
            {
                UIGeoLayer* gl = (UIGeoLayer *)(m_maplayer);
                text += gl->GetProjection().GetWKT();
                clipboard->setText(text);

            }

        }

        inline void OnLayerCRSP()
        {
            QClipboard *clipboard = QApplication::clipboard();

            QString text = clipboard->text();
            if(text.startsWith("<OPENLISEM_HAZARD_CRSCOPYSTRING>"))
            {
                text.remove("<OPENLISEM_HAZARD_CRSCOPYSTRING>");
                if(m_maplayer->IsGeo())
                {
                    UIGeoLayer* gl = (UIGeoLayer *)(m_maplayer);
                    gl->SetProjection(GeoProjection::FromString(text));
                }
            }

        }

        inline void OnLayerCRSW()
        {
            if(m_maplayer->IsGeo())
            {
                UIGeoLayer* gl = (UIGeoLayer *)(m_maplayer);
                m_window->SetCurrentProjection(gl->GetProjection());
            }
        }

        inline void OnLayerCRS()
        {
            m_layerMutex.lock();

            if(m_maplayer->IsGeo())
            {
                m_window->m_UILayerMutex.lock();
                UIGeoLayer *gl = ((UIGeoLayer *)(m_maplayer));
                CRSSelectWidget * select = new CRSSelectWidget();
                select->SetCurrentCRS(gl->GetProjection());
                int res = select->exec();

                if(res == QDialog::Accepted){
                       GeoProjection p = select->GetCurrentProjection();

                       gl->SetProjection(p);

                }

                delete select;
                m_window->m_UILayerMutex.unlock();
            }

            m_layerMutex.unlock();
        }

        inline void OnLayerEdit()
        {
            m_layerMutex.lock();
            if(!m_maplayer->IsBeingEdited())
            {
                editor = m_maplayer->GetEditor();

                m_EditorWidget = new EditorWidget(editor);
                m_ParentTabWidget->addTab(m_EditorWidget,"Layer Editor");
                m_ParentTabWidget->setCurrentWidget(m_EditorWidget);

                connect(m_EditorWidget,&EditorWidget::stopped,this,&LayerWidget::OnStopLayerEdit);

                m_maplayer->SetBeingEdited(true);
                m_window->SetEditor(editor);
                m_Name->setText(m_maplayer->GetName() + " (Editing)");

            }

            m_layerMutex.unlock();
        }

        inline void OnStopLayerEdit()
        {
            m_layerMutex.lock();

            if(m_maplayer->IsBeingEdited())
            {
                m_ParentTabWidget->removeTab(m_ParentTabWidget->indexOf(m_EditorWidget));
                m_ParentTabWidget->setCurrentIndex(1);

                m_EditorWidget = nullptr;

                editor->SetRemove();

                m_maplayer->SetBeingEdited(false);

                m_Name->setText(m_maplayer->GetName());
            }

            m_layerMutex.unlock();
        }

        inline void OnGradientClicked()
        {
            m_layerMutex.lock();
            LSMStyle s = m_maplayer->GetStyle();
            s.m_ColorGradientb1 = m_labelGradient->GetCurrentGradient();
            m_maplayer->SetStyle(s);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();

        }
        inline void OnTransparancyChanged(int value)
        {
            m_layerMutex.lock();
            LSMStyle s = m_maplayer->GetStyle();
            s.SetTransparancy(1.0f - (value/100.0f));
            m_maplayer->SetStyle(s);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }

        inline void OnDrawChecked(bool checked)
        {
            m_layerMutex.lock();
            std::cout << "draw checked " << std::endl;
            m_maplayer->SetDraw(checked);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();

        }

        inline void OnLegChecked(bool checked)
        {
            m_layerMutex.lock();
            m_maplayer->SetDrawLegend(checked);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }
        inline void OnDEMChecked(bool checked)
        {
            m_layerMutex.lock();
            m_maplayer->SetDrawAsDEM(checked);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }
        inline void OnHSChecked(bool checked)
        {
            m_layerMutex.lock();
            m_maplayer->SetDrawAsHillShade(checked);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }


        inline void OnReferenceChecked(bool checked)
        {

        }

        inline void OnMinChanged(double v)
        {
            m_layerMutex.lock();
            LSMStyle s = m_maplayer->GetStyle();
            s.m_Intervalb1.SetMin(v);
            m_maplayer->SetStyle(s);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }

        inline void OnMaxChanged(double v)
        {
            m_layerMutex.lock();
            LSMStyle s = m_maplayer->GetStyle();
            s.m_Intervalb1.SetMax(v);
            m_maplayer->SetStyle(s);
            //m_window->SetUILayerAs(this->m_maplayer,false);
            m_layerMutex.unlock();
        }

        inline void OnDeleteButtonChecked()
        {

            //we do not allow removal of native (model) maps, since there is no way to add these again
            //if user does not want to display those, uncheck the draw checkbox
            if(!(m_maplayer->IsNative()))
            {
                m_window->RemoveUILayer(this->m_maplayer,true);
            }

        }

        inline void OnTimingChanged()
        {
            m_layerMutex.lock();
            if(m_maplayer != nullptr)
            {

                LSMStyle s = m_maplayer->GetStyle();

                if(m_StyleWidget != nullptr)
                {
                    m_StyleWidget->OnTimingChanged();
                }
                if(m_TimeSeriesPlotter != nullptr)
                {
                    m_TimeSeriesPlotter->SetTime(s.m_CurrentTime);
                }

                /*if(m_SpectralPlotter != nullptr)
                {
                    if(m_maplayer != nullptr)
                    {

                        QList<LSMVector2> locs = m_window->GetFocusLocations();

                        MatrixTable * tbl = m_maplayer->GetSpectra(locs,m_window->GetCurrentProjection());

                        m_SpectralPlotter->SetFromMatrixTable(tbl,true);
                    }

                }

                if(m_ProfilePlotter != nullptr)
                {
                    if(m_maplayer != nullptr)
                    {

                        QList<LSMVector2> locs = m_window->GetFocusLocations();

                        MatrixTable * tbl = m_maplayer->GetProfile(locs,m_window->GetCurrentProjection());

                        m_ProfilePlotter->SetFromMatrixTable(tbl,true);
                    }

                }*/


            }
            m_layerMutex.unlock();
        }

        inline void OnProfile()
        {
            QList<LSMVector2> locs = m_window->GetFocusLocations();

            m_layerMutex.lock();
            if(m_maplayer != nullptr)
            {
                MatrixTable * tbl = m_maplayer->GetProfile(locs,m_window->GetCurrentProjection());

                m_layerMutex.unlock();
                m_ProfilePlotter = new TablePlotter(tbl,true,false);
                m_ProfilePlotter->show();

            }else
            {
                 m_layerMutex.unlock();
            }


        }

        inline void OnSpectra()
        {

            QList<LSMVector2> locs = m_window->GetFocusLocations();

            m_layerMutex.lock();
            if(m_maplayer != nullptr)
            {
                MatrixTable * tbl = m_maplayer->GetSpectra(locs,m_window->GetCurrentProjection());

                m_layerMutex.unlock();
                m_SpectralPlotter = new TablePlotter(tbl,true,false);
                m_SpectralPlotter->show();


            }else
            {
                m_layerMutex.unlock();
            }

        }

        inline void OnTimeSeries()
        {

            QList<LSMVector2> locs = m_window->GetFocusLocations();

            m_layerMutex.lock();

            if(m_maplayer != nullptr)
            {
                MatrixTable * tbl = m_maplayer->GetTimeSeries(locs,m_window->GetCurrentProjection());

                m_layerMutex.unlock();
                m_TimeSeriesPlotter = new TablePlotter(tbl,true,true);
                m_TimeSeriesPlotter->show();
            }else
            {
                 m_layerMutex.unlock();
            }

            connect(m_TimeSeriesPlotter, &TablePlotter::OnTimeChanged, this, &LayerWidget::OnGraphTimingChanged);

        }


        inline void OnGraphTimingChanged()
        {
            if(m_TimeSeriesPlotter != nullptr)
            {
                m_layerMutex.lock();
                LSMStyle *s = m_maplayer->GetStyleRef();
                m_layerMutex.unlock();
                s->m_CurrentTime = m_TimeSeriesPlotter->GetTime();
            }

            if(m_StyleWidget != nullptr)
            {
                m_StyleWidget->OnTimingChanged();
            }

        }

};

#endif // LAYERWIDGET_H
