#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "QWidget"
#include "layer/uilayereditor.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "widgets/labeledwidget.h"
#include "QGroupBox"
#include "QDoubleSpinBox"
#include "QSpinBox"
#include "QRadioButton"
#include "QCheckBox"
#include "QToolButton"
#include "QSpacerItem"
#include "QLineEdit"
#include "QFileDialog"
#include "widgets/colorwheel.h"
#include "QAction"
#include "QMenuBar"
#include "QObject"
#include "QMessageBox"
#include "widgets/layerinfowidget.h"
#include "vector"
#include "functional"
#include "site.h"
#include "widgets/shadereditor.h"

class EditorWidget : public QWidget
{
        Q_OBJECT;

    UILayerEditor * m_Editor;
    QVBoxLayout * m_Layout;

    LayerInfoWidget * m_AttributeDialog;

    QList<UIParameter> m_ParameterList;
    QList<QList<QWidget *>> m_WidgetList;

    QString m_Dir;

    QIcon *icon_stop;
    QIcon *icon_save;
    QIcon *icon_saveas;

    QAction *saveAct;
    QAction *saveasAct;
    QAction *stopAct;
    QAction *AttrAct;

    std::vector<std::pair<QString,std::vector<std::pair<QString,QRadioButton *>>>> m_RadioLists;

    typedef std::function<void(QString)> f_t;

    std::vector<std::pair<QString,f_t>> m_SetFunction;

    ShaderEditor * m_ShaderEditor = nullptr;

public:
    inline EditorWidget( UILayerEditor * l, QWidget *parent = 0): QWidget( parent)
    {
        std::cout << "start creating editor widget" <<std::endl;


        m_Editor = l;

        m_Dir = GetSite();

        m_Layout = new QVBoxLayout();
        this->setLayout(m_Layout);

        CreateMenuBar();

        CreateWidgetFromLayerEditor(l);


        std::cout << "done creating editor widget" <<std::endl;



    }

    inline void CreateMenuBar()
    {
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_stop = new QIcon();

        icon_stop->addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);

        saveAct = new QAction(*icon_save,tr("&Save"), this);
        //saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setStatusTip(tr("Save the document to disk"));
        connect(saveAct, &QAction::triggered, this, &EditorWidget::save);

        saveasAct = new QAction(*icon_saveas,tr("&Save As"), this);
        //saveasAct->setShortcuts(QKeySequence::SaveAs);
        saveasAct->setStatusTip(tr("Save the document to disk"));
        connect(saveasAct, &QAction::triggered, this, &EditorWidget::saveas);


        stopAct = new QAction(*icon_stop,tr("&Stop"), this);
        //stopAct->setShortcuts(QKeySequence::Paste);
        stopAct->setStatusTip(tr("Stop editing"));
        connect(stopAct, &QAction::triggered, this, &EditorWidget::stop);

        AttrAct = new QAction(tr("&Attributes"), this);
        //stopAct->setShortcuts(QKeySequence::Paste);
        stopAct->setStatusTip(tr("Edit Attributes"));
        connect(AttrAct, &QAction::triggered, this, &EditorWidget::attr);



        QMenuBar* mb = new QMenuBar();

        mb->addAction(saveAct);
        mb->addAction(saveasAct);
        mb->addSeparator();
        mb->addAction(AttrAct);
        mb->addSeparator();
        mb->addAction(stopAct);

        m_Layout->setMenuBar(mb);


    }


    inline void SetValueCallback(QString name, QString value)
    {

    }

    inline void SetOptionsCallback(QString name, QString value)
    {

    }

    void CreateWidgetFromLayerEditor(UILayerEditor *  l);

    inline void UpdateWidgetFromLayerEditor()
    {

    }

    inline void UpdateLayerEditorFromWidget()
    {


    }


public slots:

    inline void save()
    {

        if(m_Editor->GetCurrentFilePath().isEmpty())
        {

            QString path = QFileDialog::getOpenFileName(this,QString("Select a save file"),
                                                        m_Editor->GetCurrentFilePath(),QString("*.*"));
            m_Editor->OnSaveAs(path);

        }else {

            m_Editor->OnSave();
        }
    }

    inline void saveas()
    {
        QString path = QFileDialog::getOpenFileName(this,QString("Select a save file"),
                                                    m_Editor->GetCurrentFilePath(),QString("*.*"));

        if(!path.isEmpty())
        {
            m_Editor->OnSaveAs(path);
        }

    }

    inline void stop()
    {
        if(m_Editor->IsChanged())
        {
            if (QMessageBox::Save == QMessageBox::question(this, "Close and lose changes?", "Close?", QMessageBox::Close | QMessageBox::Save))
            {
                QString path = QFileDialog::getOpenFileName(this,QString("Select a save file"),
                                                            m_Editor->GetCurrentFilePath(),QString("*.*"));
                if(!path.isEmpty())
                {
                    m_Editor->OnSaveAs(path);
                }
            }

        }

        emit stopped();
    }

    inline void OnAttributeRowCallback(QString action,QList<int> rows, QList<QString> rowtitles)
    {

        if(m_Editor != nullptr)
        {
            m_Editor->OnAttributeRowCallBack(action,rows, rowtitles);
        }

    }

    inline void attr()
    {
        if(m_Editor != nullptr)
        {
            MatrixTable * t = m_Editor->GetAttributes();

            if(t != nullptr)
            {
                m_Editor->SetResponsive(false);
                QList<QString> rowoptions = m_Editor->GetAttributeRowCallbacks();

                m_AttributeDialog = new LayerInfoWidget(t,true,false,true,true,true,true,true);

                if(rowoptions.length() > 0)
                {
                    m_AttributeDialog->AddRowCallBack(rowoptions,&EditorWidget::OnAttributeRowCallback,this);

                }

                connect(m_AttributeDialog,&LayerInfoWidget::finished,this,&EditorWidget::attrdone2);
                connect(m_AttributeDialog,&LayerInfoWidget::accepted,this, &EditorWidget::attrdone);
                m_AttributeDialog->show();
                m_Editor->SetResponsive(true);
            }
        }
    }
    inline void attrdone2(int res)
    {
        m_Editor->OnAttributeEditStopped();
    }

    inline void attrdone()
    {
        //changes to the table are accepted
        //see if the table is valid

        MatrixTable * res = m_AttributeDialog->GetMatrixTable();

        int rows = res->GetNumberOfRows();

        //update shapefile
        m_Editor->SetAttributes(res);

        res->Empty();
        delete res;
    }
signals:

    void stopped();



};

#endif // EDITORWIDGET_H
