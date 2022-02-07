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

    inline void CreateWidgetFromLayerEditor(UILayerEditor *  l)
    {



        m_ParameterList = l->GetParameters();

        QGroupBox * CurrentGroupBox= nullptr;
        QVBoxLayout * CurrentLayout = m_Layout;
        for(int i = 0; i < m_ParameterList.length(); i++)
        {
            UIParameter p = m_ParameterList.at(i);
            if(p.Type == UI_PARAMETER_TYPE_GROUP)
            {
                CurrentGroupBox = new QGroupBox(p.Name);
                CurrentLayout->addWidget(CurrentGroupBox);
                CurrentLayout = new QVBoxLayout();
                CurrentGroupBox->setLayout(CurrentLayout);
                CurrentGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum));

            }else if(p.Type == UI_PARAMETER_TYPE_LABEL)
            {
                QLabel * l = new QLabel(p.Name);
                CurrentLayout->addWidget(l);

            }else if(p.Type == UI_PARAMETER_TYPE_FLOAT)
            {
                QDoubleSpinBox * l = new QDoubleSpinBox();
                if(!(p.min == p.max))
                {
                    l->setRange(p.min,p.max);
                }
                l->setDecimals(6);
                l->setValue(p.Value.toDouble());
                CurrentLayout->addWidget(new QLabeledWidget(p.Name,l));
                QString name = p.Name;
                connect(l,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[this,l,name](double){
                    QString name2 = name;
                    this->m_Editor->ChangeParameter(name,QString::number(l->value()),

                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );

                            ;});



            }else if(p.Type == UI_PARAMETER_TYPE_DOUBLE)
            {
                QDoubleSpinBox * l = new QDoubleSpinBox();
                if(! (p.min == p.max))
                {
                    l->setRange(p.min,p.max);
                }
                l->setDecimals(12);
                l->setValue(p.Value.toDouble());
                CurrentLayout->addWidget(new QLabeledWidget(p.Name,l));
                QString name = p.Name;
                connect(l,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[this,l,name](double){
                    QString name2 = name;
                    this->m_Editor->ChangeParameter(name,QString::number(l->value()),
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );

                            ;});

            }else if(p.Type == UI_PARAMETER_TYPE_BOOL)
            {
                QCheckBox * l = new QCheckBox();
                l->setText(p.Name);
                CurrentLayout->addWidget(l);
                QString name = p.Name;
                l->setChecked(p.Value.compare("1") == 0);
                connect(l,static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::toggled),[this,l,name](bool x){
                    QString name2 = name;

                    this->m_Editor->ChangeParameter(name,x?"1":"0",
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );

                    ;});


            }else if(p.Type == UI_PARAMETER_TYPE_FILE)
            {
                QLineEdit * l = new QLineEdit();
                l->setText(p.Value);
                CurrentLayout->addWidget(new QLabeledWidget(p.Name,l));

                QIcon icon;
                icon.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);


                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);

                CurrentLayout->addWidget(new QLabeledWidget(p.Name,new QWidgetHDuo(l,addButton)));
                QString name = p.Name;
                connect(addButton,static_cast<void (QToolButton::*)()>(&QToolButton::pressed),
                        [this,l, name]()
                {
                    QString path = QFileDialog::getOpenFileName(this,QString("Select a File for " + name),
                                                                l->text(),QString("*.*"));
                    // open file dialog


                    if (!path.isEmpty())// && QFileInfo(path).exists())
                    {
                        l->setText(path);
                    }
                });
                connect(l,static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textChanged),
                        [this,l,name](const QString &)
                {
                    QString name2 = name;
                    this->m_Editor->ChangeParameter(name,l->text(),
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );
                });
            }else if(p.Type == UI_PARAMETER_TYPE_STRING)
            {
                QLineEdit * l = new QLineEdit();
                l->setText(p.Value);
                CurrentLayout->addWidget(new QLabeledWidget(p.Name,l));
                QString name = p.Name;
                connect(l,static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textChanged),[this,l,name](const QString &){
                    QString name2 = name;

                    this->m_Editor->ChangeParameter(name,l->text(),
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );
                            ;});

            }else if(p.Type == UI_PARAMETER_TYPE_DIR)
            {
                QLineEdit * l = new QLineEdit();
                l->setText(p.Value);



                QIcon icon;
                icon.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);

                CurrentLayout->addWidget(new QLabeledWidget(p.Name,new QWidgetHDuo(l,addButton)));
                QString name = p.Name;
                connect(addButton,static_cast<void (QToolButton::*)()>(&QToolButton::pressed),
                        [this,l,name]()
                {
                    QString path = QFileDialog::getExistingDirectory(this, QString("Select a directory for " + name),
                                                                             l->text(),
                                                                             QFileDialog::ShowDirsOnly
                                                                             | QFileDialog::DontResolveSymlinks);
                    // open file dialog


                    if (!path.isEmpty())// && QFileInfo(path).exists())
                    {
                        l->setText(path);
                    }
                });
                connect(l,static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textChanged),
                        [this,l,name](const QString &)
                {
                    QString name2 = name;
                    this->m_Editor->ChangeParameter(name,l->text(),
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );
                });


            }else if(p.Type == UI_PARAMETER_TYPE_RADIO)
            {

                QStringList list = p.Description.split("|");
                QWidget * w = new QWidget();
                QHBoxLayout * la = new QHBoxLayout();
                w->setLayout(la);

                for(int j = 0; j < list.length(); j++)
                {
                    QRadioButton * rb = new QRadioButton(list.at(j));
                    if(list.at(j).compare(p.Value) == 0)
                    {
                        rb->setChecked(true);
                    }
                    la->addWidget(rb);
                    QString t = list.at(j);
                    QString name = p.Name;

                    connect(rb,static_cast<void (QRadioButton::*)(bool)>(&QRadioButton::toggled),
                            [this,t,name](bool x)
                    {
                        if(x)
                        {
                            QString name2 = name;
                            this->m_Editor->ChangeParameter(name,t,
                                                            [this,name2](QString name, QString value){
                                                                SetValueCallback(name,value);
                                                            ;},
                                                            [this,name2](QString name, QString value){
                                                                SetOptionsCallback(name,value);
                                                                ;}
                                                            );
                        }
                    });
                }

                CurrentLayout->addWidget(new QLabeledWidget(p.Name,w));

            }else if(p.Type == UI_PARAMETER_TYPE_COLOR)
            {

                ColorWheel *l = new ColorWheel();
                l->setMaximumSize(150,150);

                CurrentLayout->addWidget(new QLabeledWidget(p.Name,l));
                QString name = p.Name;

                QColor cn;
                QStringList list = p.Description.split("|");
                if(list.length() > 0)
                {
                    cn.setRedF(list.at(0).toDouble());
                }if(list.length() > 1)
                {
                    cn.setGreenF(list.at(1).toDouble());
                }if(list.length() > 2)
                {
                    cn.setBlueF(list.at(2).toDouble());
                }

                connect(l,static_cast<void (ColorWheel::*)(const QColor &)>(&ColorWheel::colorChange),
                        [this,l,name](const QColor & c)
                {
                    QString name2 = name;
                    this->m_Editor->ChangeParameter(name,QString::number(c.redF()) + "|" + QString::number(c.greenF()) + "|" + QString::number(c.blueF()),
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );
                });

            }else if(p.Type == UI_PARAMETER_TYPE_BUTTON)
            {
                QToolButton * l = new QToolButton();
                l->setText(p.Name);
                CurrentLayout->addWidget(l);
                QString name = p.Name;
                connect(l,static_cast<void (QToolButton::*)()>(&QToolButton::pressed),
                        [this,l,name]()
                {
                    QString name2 = name;

                    this->m_Editor->ChangeParameter(name," ",
                                                    [this,name2](QString name, QString value){
                                                        SetValueCallback(name,value);
                                                    ;},
                                                    [this,name2](QString name, QString value){
                                                        SetOptionsCallback(name,value);
                                                        ;}
                                                    );
                });

            }


        }

    }

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
