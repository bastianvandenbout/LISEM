#include "editorwidget.h"
#include "layer/geo/uishaderlayer.h"
#include "layer/editors/uishadereditor.h"

void EditorWidget::CreateWidgetFromLayerEditor(UILayerEditor *  l)
{
    m_ParameterList = l->GetParameters();

    if(m_ParameterList.size() == 1 && m_ParameterList.at(0).Type == UI_PARAMETER_TYPE_SHADER)
    {

        //create ui shader editor

        //first create tab-based script editor

        m_ShaderEditor = new ShaderEditor();
        m_Layout->addWidget(m_ShaderEditor);


        UIShaderLayerEditor* shadereditorlayer = dynamic_cast<UIShaderLayerEditor*>(l);

        m_ShaderEditor->SetEditorLayer(shadereditorlayer);

        //callback shader update/run/compile

        m_ShaderEditor->SetShaderUpdateCallback(&UIShaderLayerEditor::OnUpdate,shadereditorlayer);
        m_ShaderEditor->SetShaderRunCallback(&UIShaderLayerEditor::OnRun,shadereditorlayer);
        m_ShaderEditor->SetShaderPauseCallback(&UIShaderLayerEditor::OnPause,shadereditorlayer);
        m_ShaderEditor->SetShaderCoordinatesCallback(&UIShaderLayerEditor::OnCoordinates,shadereditorlayer);

        //callback errors
        shadereditorlayer->SetCompileErrorCallback(&ShaderEditor::OnShaderCompileError,m_ShaderEditor);


    }else
    {

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


}
