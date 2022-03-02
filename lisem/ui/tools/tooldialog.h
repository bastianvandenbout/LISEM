#pragma once

#include "QVBoxLayout"
#include "QWidget"
#include "scriptmanager.h"
#include "scripting/databasetool.h"
#include "mapview/mapviewtool.h"
#include "QLineEdit"
#include "QPushButton"
#include "QToolButton"
#include "QStringListModel"
#include "QItemDelegate"
#include "QLabel"
#include "QHBoxLayout"
#include "tooldialog.h"
#include "toolparameterwidget.h"
#include "QList"
#include "widgets/labeledwidget.h"
#include "qplaintextedit.h"
#include "site.h"

class ToolDialog : public QDialog
{

    QVBoxLayout *m_MainLayout;

    QLabel * m_FunctionNameBox;
    QGroupBox * m_FunctionDescriptionBox;
    QGroupBox * m_FunctionParametersBox;
    QGroupBox * m_FunctionOutputBox;
    QGroupBox * m_FunctionRunCodeBox;

    QVBoxLayout * m_FunctionDescriptionBoxL;
    QVBoxLayout * m_FunctionParametersBoxL;
    QVBoxLayout * m_FunctionOutputBoxL;
    QVBoxLayout * m_FunctionRunCodeBoxL;

    QWidget * m_ButtonWidget;
    QHBoxLayout * m_ButtonLayout;

    QList<ParameterSelectionWidget *> m_ParameterWidgets;

    QPlainTextEdit * m_Code;
    QPlainTextEdit * m_Messages;
    SwitchButton * m_CodeEdit;

    SwitchButton * m_SwitchSave = nullptr;
    QLineEdit * m_SaveFile = nullptr;
    SwitchButton * m_SwitchAdd = nullptr;

    ScriptFunctionInfo m_Function;

    ScriptManager * m_ScriptManager;

    QString m_SaveName;
    QString m_SaveDir;
    QString m_SaveText;
    bool m_SaveRaster = false;
    bool m_SaveTable = false;
    bool m_SaveShapeFile = false;
public:
    inline ToolDialog(ScriptManager * sm, MapViewTool * mvt, ScriptFunctionInfo f) : QDialog()
    {
        QScrollArea *scrollArea = new QScrollArea( this );
        QWidget * temp = new QWidget();
        scrollArea->setWidget(temp);
        scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
        scrollArea->setWidgetResizable( true );

        QVBoxLayout * ltemp = new QVBoxLayout();
        this->setLayout(ltemp);
        ltemp->addWidget(scrollArea);

        m_ScriptManager = sm;
        m_MainLayout = new QVBoxLayout();
        temp->setLayout(m_MainLayout);

        m_Function = f;

        m_FunctionNameBox = new QLabel(f.Function->GetName());
        QFont font = m_FunctionNameBox->font();
        font.setPointSize(15);
        m_FunctionNameBox->setFont(font);
        m_FunctionDescriptionBox = new QGroupBox("Description");
        m_FunctionParametersBox = new QGroupBox("Input Parameters");
        m_FunctionOutputBox = new QGroupBox("Output");
        m_FunctionRunCodeBox = new QGroupBox("Run Code");

        m_FunctionDescriptionBoxL = new QVBoxLayout();
        m_FunctionParametersBoxL = new QVBoxLayout();
        m_FunctionOutputBoxL = new QVBoxLayout();
        m_FunctionRunCodeBoxL = new QVBoxLayout();

        QLabel * descriptionlabel = new QLabel();

        m_FunctionDescriptionBox->setLayout(m_FunctionDescriptionBoxL);
        m_FunctionParametersBox->setLayout(m_FunctionParametersBoxL);
        m_FunctionOutputBox->setLayout(m_FunctionOutputBoxL);
        m_FunctionRunCodeBox->setLayout(m_FunctionRunCodeBoxL);


        m_FunctionDescriptionBoxL->addWidget(descriptionlabel);
        descriptionlabel->setText(m_Function.Description);
        descriptionlabel->setWordWrap(true);

        int returntypeId = f.Function->GetReturnTypeId();
        QString returnt= sm->m_Engine->GetTypeName(returntypeId);

        //supported output types
        bool can_run = true;
        if(!(returnt.compare("Map") == 0 || returnt.compare("Shapes") == 0 ||  returnt.compare("Table") == 0))
        {
            can_run = false;
        }
        for(int i = 0; i < f.Function->GetParamCount(); i++)
        {
            int typeId = 0;
            asDWORD flags = 0;
            const char * name;
            const char * def;
            f.Function->GetParam(i,&typeId,&flags,&name,&def);

            ParameterSelectionWidget *  w = GetSelectionWidgetFromType(sm->m_Engine,typeId,def);

            if(w == nullptr)
            {

                can_run = false;

                delete w;
                break;
            }

            delete w;
        }

        //first get all the input and output widgets, if they all can be made, this means we can provide the functionality to run it






        ///input widgets
        for(int i = 0; i < f.Function->GetParamCount(); i++)
        {
            QLabel * parlabel = new QLabel();
            QLabel * pardescriptlabel = new QLabel();
            pardescriptlabel->setWordWrap(true);
            QString labeltext = "";

            QString parametername;
            QString parameterdescription;
            if(i < f.Parameters.length())
            {
                 parametername = f.Parameters.at(i);
            }
            if(i < f.ParameterDescription.length())
            {
                parameterdescription = f.ParameterDescription.at(i);
            }

            int typeId = 0;
            asDWORD flags = 0;
            const char * name;
            const char * def;
            f.Function->GetParam(i,&typeId,&flags,&name,&def);
            if(name != nullptr)
            {
                labeltext +=" "+ (!parametername.isEmpty()? parametername : QString((name)));
            }
            asITypeInfo * ti = sm->m_Engine->GetTypeInfoById(typeId);
            if(ti != nullptr)
            {

                labeltext +=  " ( " + QString(ti->GetName());
            }else
            {
                labeltext += " ( ";
                labeltext += sm->m_Engine->GetBaseTypeName(typeId);
            }
            if(def != nullptr)
            {
                labeltext +=  " ,default = " + QString((def));
            }
            labeltext += ") ";

            parlabel->setText(labeltext);
            parlabel->setStyleSheet("font-weight: bold; color: black");
            pardescriptlabel->setText(parameterdescription);
            pardescriptlabel->setStyleSheet("color: grey");

            if(can_run)
            {
                ParameterSelectionWidget *  w = GetSelectionWidgetFromType(sm->m_Engine,typeId,def);
                if(!(w == nullptr))
                {
                    m_FunctionParametersBoxL->addWidget(parlabel);
                    m_FunctionParametersBoxL->addWidget(pardescriptlabel);
                    m_FunctionParametersBoxL->addWidget(w);
                    QFrame *line;
                    line = new QFrame();
                    line->setFrameShape(QFrame::HLine);
                    line->setFrameShadow(QFrame::Sunken);
                    m_FunctionParametersBoxL->addWidget(line);
                    connect(w,&ParameterSelectionWidget::OnValueChanged,this,&ToolDialog::onParameterChanged);
                }
                m_ParameterWidgets.append(w);
            }
        }

        //output widgets


        if(can_run)
        {
            m_FunctionOutputBoxL->addWidget(new QLabel(returnt));

            if(returnt.compare("Map") == 0 || returnt.compare("Shapes") == 0 )
            {
                m_SwitchAdd = new SwitchButton();
                connect(m_SwitchAdd,&SwitchButton::valueChanged,this,&ToolDialog::AddChanged);
                m_FunctionOutputBoxL->addWidget(new QLabeledWidget("Add File To Map View ",m_SwitchAdd));
            }
            if(returnt.compare("Map") == 0 || returnt.compare("Shapes") == 0 ||  returnt.compare("Table") == 0 )
            {
                m_SwitchSave = new SwitchButton();
                connect(m_SwitchSave,&SwitchButton::valueChanged,this,&ToolDialog::SaveChanged);
                m_FunctionOutputBoxL->addWidget(new QLabeledWidget("Save To File ",m_SwitchSave));
                m_SaveFile = new QLineEdit();

                QString m_Dir = GetSite();
                QIcon * icon_save = new QIcon();
                icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton * m_SaveButton = new QToolButton();
                m_SaveButton->setIcon(*icon_save);

                connect(m_SaveButton,&QToolButton::pressed,this,&ToolDialog::OnSavePressed);
                connect(m_SaveFile,&QLineEdit::textChanged,this,&ToolDialog::SaveFileChanged);
                m_FunctionOutputBoxL->addWidget(new QWidgetHDuo(m_SaveFile,m_SaveButton));

                if(returnt.compare("Map") == 0)
                {
                    m_SaveRaster = true;
                }
                if(returnt.compare("Table"))
                {
                    m_SaveTable = true;
                }
                if(returnt.compare("ShapeFile"))
                {
                    m_SaveShapeFile = true;
                }
            }
        }

        if(can_run)
        {
            //code widgets
            m_CodeEdit = new SwitchButton();
            m_Code = new QPlainTextEdit();
            m_Code->setEnabled(false);
            connect(m_CodeEdit,&SwitchButton::valueChanged,this,&ToolDialog::CodeEditChanged);
            m_FunctionRunCodeBoxL->addWidget(new QLabeledWidget("Edit Code ",m_CodeEdit));
            m_FunctionRunCodeBoxL->addWidget(m_Code);

            m_Messages = new QPlainTextEdit();
            m_Messages->setEnabled(false);
            m_FunctionRunCodeBoxL->addWidget(m_Messages);

            ///run or cancel buttons
            m_ButtonWidget = new QWidget();
            m_ButtonLayout = new QHBoxLayout();
            m_ButtonWidget->setLayout(m_ButtonLayout);

            QPushButton * okb = new QPushButton("Run");
            QPushButton * cancelb = new QPushButton("Cancel");

            connect(okb, &QPushButton::clicked,this,&ToolDialog::onOkClicked);
            connect(cancelb, &QPushButton::clicked,this,&ToolDialog::onCancelClicked);

            m_ButtonLayout->addWidget(okb);
            m_ButtonLayout->addWidget(cancelb);

        }

        //add everything to layout
        m_MainLayout->addWidget(m_FunctionNameBox);
        m_MainLayout->addWidget(m_FunctionDescriptionBox);
        m_MainLayout->addWidget(m_FunctionParametersBox);
        if(can_run)
        {
            m_MainLayout->addWidget(m_FunctionOutputBox);
            m_MainLayout->addWidget(m_FunctionRunCodeBox);
            m_MainLayout->addWidget(m_ButtonWidget);
        }

        this->setMinimumSize(QSize(400,600));

        if(can_run)
        {
            onParameterChanged();

            QTimer::singleShot(30,this,&ToolDialog::UpdateConsole);
        }
    }

public slots:

    void UpdateConsole()
    {
        QList<LeveledMessage> list = GetMessagesSTD();

        for(int i = 0; i < list.length(); i++)
        {
            QString line = list.at(i).Message;

            QString alertHtml = "<font color=\"DeepPink\">";
            QString notifyHtml = "<font color=#404040>";
            QString infoHtml = "<font color=#003399>";
            QString errorHtml = "<font color=\"Red\">";
            QString scriptHtml = "<font color=\"Indigo\">";
            QString endHtml = "</font>";

            switch(list.at(i).Level)
            {
                case LISEM_MESSAGE_ERROR: line = errorHtml.append(line); break;
                case LISEM_MESSAGE_WARNING: line = alertHtml.append(line); break;
                case LISEM_MESSAGE_STATUS: line = notifyHtml.append(line); break;
                case LISEM_MESSAGE_DEBUG: line = infoHtml.append(line); break;
                case LISEM_MESSAGE_SCRIPT: line = infoHtml.append(line); break;
                default: line = notifyHtml.append(line); break;
            }

            line = line.append(endHtml);
            m_Messages->appendHtml(line);
        }

        QTimer::singleShot(30,this,&ToolDialog::UpdateConsole);

    }

    inline void CodeEditChanged(bool x)
    {
        m_Code->setEnabled(x);
    }

    inline void onParameterChanged()
    {
        if(m_CodeEdit->value() == false)
        {
            QString text;

            int typeId = m_Function.Function->GetReturnTypeId();
            asITypeInfo * ti = m_ScriptManager->m_Engine->GetTypeInfoById(typeId);
            QString returnt;
            if(ti != nullptr)
            {
                returnt += ti->GetName();
            }else
            {
                returnt += m_ScriptManager->m_Engine->GetBaseTypeName(typeId);
            }

            text += returnt;
            text += " result = ";
            text += m_Function.Function->GetName();
            text += "(";

            for(int i = 0; i < m_ParameterWidgets.length(); i++)
            {
                if(i != 0)
                {
                      text += ",";
                }
                if(m_ParameterWidgets.at(i) != nullptr)
                {
                    text += m_ParameterWidgets.at(i)->GetValueAsString();
                }
            }
            text += ");";
            if(m_SwitchAdd != nullptr)
            {
                if(m_SwitchAdd->value())
                {
                    text += "\n";
                    text += "AddViewLayer(result,\"";
                    text += m_Function.Function->GetName();
                    text += "\");";

                }
            }
            if(m_SwitchSave != nullptr && m_SaveFile != nullptr)
            {
                if(m_SwitchSave->value())
                {
                    text += "\n";
                    text += "SaveMapAbsPath(result,\"";
                    text += m_SaveFile->text();
                    text += "\");";

                }
            }
            m_Code->setPlainText(text);

        }
    }

    inline void AddChanged(bool x)
    {

        onParameterChanged();

    }
    inline void SaveChanged(bool x)
    {

        onParameterChanged();
    }

    inline void OnScriptDone()
    {
        LISEMS_DEBUG("Done");
    }

    inline void onOkClicked(bool)
    {
        //run the actual code

        SPHScript *s = m_ScriptManager->CompileScript(m_Code->toPlainText(),true,true,"");
        m_ScriptManager->RunScript(s,&ToolDialog::OnScriptDone,this);

        //close after doing run?
        //we wouldnt be able to see wether it succeeded
        //or quickly redo with different parameters
        //emit accept();
    }
    inline void onCancelClicked(bool)
    {
        //cancel and close

        emit reject();
    }

    inline void OnSavePressed()
    {
        QString ext = "*.*;;";
        QString name = "file";

        if(m_SaveRaster)
        {
            name = "raster";
            ext = "*.map *.tif *.csf;;*.*";
        }else if(m_SaveShapeFile)
        {
            name = "ShapeFile";
            ext = "*.shp;;*.*";
        }else if(m_SaveTable)
        {
            name = "Table";
            ext = "*.tbl *.csv;;*.*";
        }

        QString path = QFileDialog::getSaveFileName(this,	QString("Select the map:"),
                                                    GetFIODir(LISEM_DIR_FIO_GENERAL),QString());
        // open file dialog


        if (!path.isEmpty())// && QFileInfo(path).exists())
        {

           SetFIODir(LISEM_DIR_FIO_GENERAL,QFileInfo(path).dir().absolutePath());


           QString f = QFileInfo(path).fileName();
           QString p = QFileInfo(path).dir().path();
           m_SaveDir = p;
           m_SaveName = path;

           m_SaveText = "";

           if(m_SaveRaster)
           {
               m_SaveText += "SaveMapAbsPath(result,\"";
               m_SaveText += m_SaveName;
               m_SaveText += "\");";
           }else if(m_SaveShapeFile)
           {
               m_SaveText += "SaveShapesAbsPath(result,\"";
               m_SaveText += m_SaveName;
               m_SaveText += "\");";
           }else if(m_SaveTable)
           {
               m_SaveText += "SaveTableAbsPath(result,\"";
               m_SaveText += m_SaveName;
               m_SaveText += "\");";
           }

           if(m_SaveFile != nullptr)
           {
               m_SaveFile->setText(m_SaveName);
           }
        }
        this->onParameterChanged();
    }

    void SaveFileChanged(QString)
    {
        this->onParameterChanged();
    }
};



