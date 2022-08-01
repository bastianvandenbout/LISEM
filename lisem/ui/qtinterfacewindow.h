#ifndef QTINTERFACEWINDOW_H
#define QTINTERFACEWINDOW_H



#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX
#endif

#include "version.h"
#include "defines.h"
#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <QObject>

#include <QApplication>
#include <QtWidgets>
#include <QSystemTrayIcon>
#include <QPlainTextEdit>
#include <QSystemTrayIcon>
#include "parameters/parametermanager.h"
#include "model.h"
#include "scriptmanager.h"
#include "glplot/worldwindow.h"

#include "scripting/codeeditor.h"
#include "scripting/scripttool.h"
#include "scripting/databasetool.h"
#include "mapview/mapviewtool.h"
#include "console/consoletool.h"
#include "qtplot/modelstattool.h"
#include "modeltool.h"
#include "widgets/devicewidget.h"
#include "tools/toolbrowser.h"
#include "download/downloadtool.h"
#include "visualscripting/visualscripting.h"
#include "widgets/controlsdialog.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QPalette>
#include "QMessageBox"

class LISEM_API QTInterfaceWindow : public QMainWindow
{

    Q_OBJECT

public:

    QString m_Dir;

    WorldWindow * m_WorldWindow;
    ParameterManager * m_ParameterManager;
    LISEMModel * m_Model;
    ScriptManager * m_ScriptManager;

    QWidget * m_MainWidget;
    QWidget * m_ParameterWidget;

    ModelTool * m_ModelTool;
    ModelStatTool * m_ModelStatTool;
    MapViewTool * m_MapViewTool;
    ConsoleTool * m_ConsoleTool;
    ScriptTool * m_ScriptWidget;
    DatabaseTool* m_DatabaseWidget;
    VisualScriptingTool * m_VisualScripter;
    ToolBrowser * m_ToolBoxWidget;
    DownloadManagerWidget * m_DownloadWidget;

    QMenu *fileMenu;
    QTabWidget * TabWidget;
    QComboBox * m_DropDown;
    QToolButton *m_delButton;
    QVBoxLayout *m_Layout;
    QList<QVBoxLayout *> TabWidgetLayouts;

    QProgressBar * m_ProgressBar;

    QSignalMapper *m_SignalMapper_UI;

    QSignalMapper *m_SignalMapper_Bool;
    QSignalMapper *m_SignalMapper_Map;
    QSignalMapper *m_SignalMapper_Map2;
    QSignalMapper *m_SignalMapper_Dir;
    QSignalMapper *m_SignalMapper_File;
    QSignalMapper *m_SignalMapper_Float;
    QSignalMapper *m_SignalMapper_Int;
    QSignalMapper *m_SignalMapper_MultiCheck;

    QList<ParameterWidget> m_ParameterWidgetList;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_info;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveasAct;
    QAction *printAct;
    QAction *exitAct;

    QAction *startAct;
    QAction *pauseAct;
    QAction *stopAct;

    QString m_Path_Current;
    QString m_Path_runfiles;

    bool m_quit_allowed= false;

    bool m_darkui = false;
public:
    QTInterfaceWindow(QWidget * w= NULL ) : QMainWindow(w)
    {


    }

    int Create(ParameterManager * pm, LISEMModel * m, ScriptManager * sm, WorldWindow * w);


    inline void contextMenuEvent(QContextMenuEvent *event) override
    {
        //bad code, the QAction instance needs to be deleted

        /*QMenu menu(this);
        QAction *cInfoAct = new QAction("About",this);

        menu.addAction(cInfoAct);
        menu.exec(event->globalPos());*/
    }

    inline void SetScriptFunctions(ScriptManager * sm)
    {
        m_MapViewTool->SetScriptFunctions(sm);
        m_DownloadWidget->RegisterScriptFunctions(sm);


    }
    inline void Initialize(OpenGLCLManager * m, ScriptManager * sm)
    {
        m_ToolBoxWidget->InitializeFunctions(sm);
    }

    void CreateParameterWidgets();

    void UpdateParameters();
    void UpdateInterface();


    MODELTOINTERFACE m_ModelData;

    inline void OnModelStepDone()
    {

        //this is the moment to exchange any data between the main interface and the model while it is running
        m_Model->m_ModelDataMutex.lock();



        m_Model->m_InterfaceData.m_CurrentUIMap = m_ModelData.m_CurrentUIMap;

        //from model to interface
        m_Model->m_InterfaceData.m_UIMapNames =m_Model->m_UIMapNames;
        m_Model->m_InterfaceData.val_max = m_ModelData.val_max;
        m_Model->m_InterfaceData.val_min = m_ModelData.val_min;

        m_ModelData = m_Model->m_InterfaceData;

        //for pointers we must make a duplicate manually
        if(m_ModelData.H.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.H.append(new QList<float>());
            }

        }
        if(m_ModelData.V.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.V.append(new QList<float>());
            }
        }
        if(m_ModelData.Q.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.Q.append(new QList<float>());
            }
        }
        for(int i = 0; i < m_ModelData.outlets.length(); i++)
        {
            m_ModelData.H.at(i)->fromStdList(m_Model->m_InterfaceData.H.at(i)->toStdList());
            m_ModelData.V.at(i)->fromStdList(m_Model->m_InterfaceData.V.at(i)->toStdList());
            m_ModelData.Q.at(i)->fromStdList(m_Model->m_InterfaceData.Q.at(i)->toStdList());
        }


        //from interface to model
        if(m_Model->m_InterfaceData.m_mapnameschanged == true)
        {
            m_Model->m_InterfaceData.m_mapnameschanged = false;
        }


        m_Model->m_ModelDataMutex.unlock();

        QTimer::singleShot(0,this,SLOT(UpdateInterfaceFromModelData()));

    }

    inline void closeEvent(QCloseEvent *event) override
    {
        //first ask the user (if required) if they need to save something

        //is model running
        bool model = m_Model->IsRunning();
        //get list of unsaved files from script tool
        QList<QString> unsaved_script = m_DatabaseWidget->GetUnsavedFileNames();
        //is script running
        bool is_script_running = m_ScriptManager->IsScriptRunning();
        //get  unsaved file name from visual script tool
        bool is_visual_script_running = m_VisualScripter->IsScriptRunning();
        bool is_visual_script_unsaved = m_VisualScripter->IsScriptUnsaved();

        //get unsaved stuff from editor
        bool mapedits = m_MapViewTool->IsEditsUnsaved();

        //ask user
        if((m_quit_allowed == false) )
        {
            if( (model || is_script_running || is_visual_script_running || is_visual_script_unsaved || (unsaved_script.size() > 0)|| mapedits))
            {
                m_quit_allowed = true;

                std::cout << "request close8 " << model <<  is_script_running <<  is_visual_script_running <<  is_visual_script_unsaved <<  (unsaved_script.size() > 0)<<  mapedits <<  std::endl;

                QString text = "";
                if(mapedits)
                {
                    text += "Unsaved changes to layer editor\n";
                }
                if(unsaved_script.size() > 0)
                {
                    text += "Unsaved changes to " + QString::number(unsaved_script.size()) + " scripts\n";
                }
                if(model)
                {
                    text += "Running simulation\n";
                }
                if(is_script_running)
                {
                    if(is_visual_script_running)
                    {
                        text += "Visual script running\n";
                    }else
                    {
                        text += "Script running\n";
                    }
                }
                if(is_visual_script_unsaved)
                {
                    text += "Unsaved changes to visual script\n";
                }
                int ret = QMessageBox::warning(
                                    m_MainWidget,
                                    "OpenLISEM Hazard",
                                    "Quit without saving changes?",
                                     QMessageBox::Discard | QMessageBox::Cancel);//QMessageBox::Save |

                        switch (ret) {
                        case QMessageBox::Save:

                            //save all the stuff

                            break;
                        case QMessageBox::Discard:
                            break;
                        case QMessageBox::Cancel:
                        default:
                            event->ignore();
                            return;
                        }



            }

            //they they want to quit, inform the main loop

            m_WorldWindow->SignalClose();

            event->ignore();

        }


    }

    bool has_callback_commandline = false;
    std::function<void(void)> m_CallBack_CommandLine;

    template<typename _Callable, typename... _Args>
    inline void  SetCommandLineCallBack(_Callable&& __f, _Args&&... __args)
    {
        m_CallBack_CommandLine = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        has_callback_commandline = true;
    }

    inline bool SetCommandLineRun(QString file, bool has_dir, QString dir, bool quit)
    {

        m_DatabaseWidget->SetWorkingDir(dir);

        m_DatabaseWidget->m_FileEditor->OpenAndRunCode(file,std::function<void(void)>([this](){ if(has_callback_commandline)
            {
                m_CallBack_CommandLine();
            };
                ;}));


        return false;

    }
    inline bool SetCommandLineCompile(QString file, bool has_dir, QString dir, bool quit)
    {
        m_DatabaseWidget->SetWorkingDir(dir);

        m_DatabaseWidget->m_FileEditor->OpenAndCompileCode(file,std::function<void(void)>([this](){ if(has_callback_commandline)
            {
                m_CallBack_CommandLine();
            };
                ;}));


        return false;
    }

    inline bool SetCommandLineRunModel(QString file, bool has_dir, QString dir, bool quit)
    {
        this->m_ModelTool->InterfaceOpenRunFile(dir + file);
        this->m_ModelTool->start();

        return false;
    }

    inline bool SetCommandLineRunCalc(QString calc, bool has_dir, QString dir, bool quit)
    {
        m_DatabaseWidget->SetWorkingDir(dir);

        m_DatabaseWidget->OnConsoleCommand(calc);

        return false;
    }

    inline bool SetCommandLineOpenFiles(QList<QString> files, bool has_dir, QString dir, bool quit)
    {
        for(int i = 0; i < files.length(); i++)
        {
            OnFileOpenRequest(dir + files.at(i),LISEM_FILE_TYPE_UNKNOWN);
        }

        return false;
    }

    bool OnFileOpenRequest(QString path, int type);

    bool first_onshow = true;
public slots:

    inline void showEvent(QShowEvent* event) override
    {
        if(first_onshow)
        {
            first_onshow = false;
            if(has_callback_commandline)
            {
                m_CallBack_CommandLine();
            }

        }
        QWidget::showEvent(event);

        //check command line task and potentially execute it

    }


    void UpdateInterfaceFromModelData();

    void newFile();
    void open();
    void save();
    void saveas();
    void ExportRunfile(bool saveas = false);
    void ImportRunfile(QString name= QString(""));
    void ExportRunfileList();
    void OnRunfileIndexChanged(int i);
    void OnRunFileDeleteFromList();
    void print();
    void cut();
    void copy();
    void paste();
    inline void SignalFunction_UIButton(QString arg)
    {


    }

    void start();
    void pause();
    void stop();
    void OnModelPaused();
    void OnModelStopped(bool);
    void ModelPaused();
    void ModelStopped(bool has_error);


    void SignalFunction_UI(QString);


    void SignalFunction_Bool(int index);
    void SignalFunction_Map(int index);
    void SignalFunction_Dir(int index);
    void SignalFunction_File(int index);
    void SignalFunction_Float(int index);
    void SignalFunction_Int(int index);
    void SignalFunction_MultiCheck(int index);


    inline void Device()
    {

        DeviceWidget *w = new DeviceWidget(m_WorldWindow->m_OpenGLCLManager);
        w->show();

    }

    inline void Info()
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Thank you for using LISEM ") + LISEM_VERSION_STRING + "! \n For more information visit www.lisemmodel.com. \n This software is published under the GNU public licence version 3, see also the details below.");
        msgBox.setInformativeText("Info");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        QString message = "This software is an open-source project under the GNU public licence version 3. Copyright (C) 2020 B. van den Bout. This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program; if not, see http://www.gnu.org/licenses/gpl-3.0.en.html";

        message += "\n" ;
        message += "For more information, see www.lisemmodel.com";
        message += "\n" ;
        message += "This project utilizes various other open-source libraries: \n";
        message += "QT \n";
        message += "GDAL \n";
        message += "OPENGL \n";
        message += "MingW64 \n";
        message += "MSYS2 \n";
        message += "OpenCL \n";
        message += "FFMPEG \n";
        message += "Project CHRONO \n";
        message += "AV Video \n";
        message += "Angelscript \n";
        message += "GLFW \n";
        message += "Freetype \n";
        message += "Clipper \n";
        message += "QTPlot \n";
        message += "EIGEN \n";
        message += "MLPACK \n";


        msgBox.setDetailedText( message);
        int ret = msgBox.exec();
    }

    inline void ControlScheme()
    {
        ControlDialog *cd = new ControlDialog();
        cd->show();
        /*QMessageBox msgBox;
        msgBox.setText("Control scheme for OpenLISEM Hazard");
        msgBox.setInformativeText("Info");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setDetailedText("Use the <shift> button to toggle between moving and selecting/dragging within the map viewer. \n When in 3D mode, use w/a/s/d to move the position of the camera, and drag the display to change view direction." );
        int ret = msgBox.exec();*/

    }

    inline void Font()
    {
        QFont current = QApplication::font();

        bool ok;
        QFont font = QFontDialog::getFont(
                        &ok, current, this);
        if (ok) {
                    QApplication::setFont(font);
        } else {
        }
    }




    inline void ToggleTheme()
    {
        if(!m_darkui)
        {
            // modify palette to dark
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window,QColor(53,53,53));
            darkPalette.setColor(QPalette::WindowText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
            darkPalette.setColor(QPalette::Base,QColor(42,42,42));
            darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
            darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
            darkPalette.setColor(QPalette::ToolTipText,Qt::white);
            darkPalette.setColor(QPalette::Text,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
            darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
            darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
            darkPalette.setColor(QPalette::Button,QColor(53,53,53));
            darkPalette.setColor(QPalette::ButtonText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
            darkPalette.setColor(QPalette::BrightText,Qt::red);
            darkPalette.setColor(QPalette::Link,QColor(42,130,218));
            darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
            darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
            darkPalette.setColor(QPalette::HighlightedText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

            qApp->setPalette(darkPalette);

        }else
        {
            qApp->setPalette(this->style()->standardPalette());
        }

        m_darkui = !m_darkui;
    }
};

#endif // QTINTERFACEWINDOW_H
