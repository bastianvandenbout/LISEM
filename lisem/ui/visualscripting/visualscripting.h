#pragma once


#include "QWidget"
#include "QApplication"
#include "QMenuBar"
#include "QVBoxLayout"
#include "QTimer"
#include "QtConcurrent/QtConcurrent"

#define NODE_EDITOR_SHARED


#include "scriptmanager.h"
#include "scripting/scripttool.h"
#include "scripting/codeeditor.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <atomic>
#include "functionalnodemodel.h"

class VisualScriptingTool : public QWidget
{
        Q_OBJECT;

public:

    QMutex m_LineInfoMutex;
    bool is_compilesucceed = false;
    bool is_compilechanged = false;
    QList<QString> m_Codes;
    QList<QtNodes::Node*> m_CodeLineNodes;
    int m_CurrentLine = -1; //-1 indicates nothing to highlight

    QTimer * m_Timer = nullptr;
    QWidget *popup;
    QLabel *popupLabel;
    QString popupMessage;



    QString m_Dir;

    QWaitCondition m_PauseWaitCondition;
    QMutex m_PauseMutex;

    std::atomic<bool> m_CodeIsRunning;
    std::atomic<bool> m_CodeIsPaused;
    std::atomic<bool> m_CodeIsStopRequested;
    std::atomic<bool> m_CodeIsPauseRequested;


    ScriptManager * m_ScriptManager;
    CodeEditor * m_ScriptTool;

    QString m_Path_Current;
    QString m_Path_OpenFiles;

    QtNodes::FlowView *m_FlowView;
    QtNodes::FlowScene *m_Scene;

    QMenuBar * menuBar;
    QAction *  compileAction;
    QAction * runAction;
    QAction * pauseAction;
    QAction * stopAction;
    QAction * saveAction;
    QAction * saveasAction;
    QAction * loadAction;
    QAction * newAction;
    QAction * previewAction;

    QLabel * m_LabelRunning;




    std::atomic<bool> m_QuitThread;
    QTimer m_PreviewTimer;
    bool m_DoPreview;
    bool m_PreviewCodeReset = false;
    QMutex m_PreviewMutex;
    QtNodes::Node* m_CurrentPreviewNode;
    QList<PreviewArgument> m_CurrentPreviewArgument;
    QList<QtNodes::Node*> m_CurrentPreviewNodes;
    QList<QList<PreviewArgument>> m_CurrentPreviewArguments;

    QMutex m_PreviewDataMutex;
    QList<FunctionalNodeModel*> m_PreviewDataModel;
    QList<PreviewArgument> m_PreviewData;

    VisualScriptingTool( ScriptManager * sm,QWidget * parent = nullptr);

    inline ~VisualScriptingTool()
    {

    }

    inline bool IsScriptUnsaved()
    {
        m_LineInfoMutex.lock();
        bool ret = is_compilechanged;
        m_LineInfoMutex.unlock();

        return ret;
    }

    void RunSinglePreviewNode( FunctionalNodeModel*, QList<PreviewArgument> args);

    inline bool IsScriptRunning()
    {
        return m_CodeIsRunning.load();
    }
    QList<QString> ConvertToScript();

    inline void int_emitupdatebuttons_start()
    {
        emit int_update_buttons_start();
    }
    inline void int_emitupdatebuttons_pause()
    {
        emit int_update_buttons_pause();
    }
    inline void int_emitupdatebuttons_stop()
    {
        emit int_update_buttons_stop();
    }

    inline void setCurrentLine(int l)
    {
        emit int_update_nodedone(l);
    }

    void signal_node_preview();

signals:

    void int_update_nodedone(int);

    void int_update_buttons_start();
    void int_update_buttons_pause();
    void int_update_buttons_stop();

    void int_emit_show_dialog();
    void int_emit_show_temporary_dialog();
    void int_emit_hide_dialog();

    void int_emit_previewdataset();

    void int_emit_signal_node_preview();
public slots:

    inline void call_signal_node_preview()
    {
        signal_node_preview();
    }
    void SetPreviewData();
    void OnNodeDone(int l);

    void OnNodePlaced(QtNodes::Node & n);
    void OnNodeDeleted(QtNodes::Node & n);
    void connectionCreated(QtNodes::Connection const & c);
    void connectionDeleted(QtNodes::Connection const & c);

    inline void int_show_dialog()
    {
        popupLabel->setText(popupMessage);
        popup->adjustSize();
        popup->setVisible(true);
    }

    inline void int_show_temporary_dialog()
    {
         if(m_Timer != nullptr)
         {
             delete m_Timer;
             m_Timer  = nullptr;
         }
         m_Timer = new QTimer();
         m_Timer->start(3500);
         QObject::connect(m_Timer, SIGNAL(timeout()), this, SLOT(int_show_temporary_dialog_ontimer()));
         popupLabel->setText(popupMessage);
         popup->adjustSize();
         popup->setVisible(true);
    }

    inline void int_show_temporary_dialog_ontimer()
    {
        if(m_Timer != nullptr)
        {
            delete m_Timer;
            m_Timer = nullptr;
        }

        popup->setVisible(false);
    }

    inline void int_hide_dialog()
    {
        popup->setVisible(false);
    }

    inline void int_buttons_onpause()
    {
        runAction->setEnabled(true);
        pauseAction->setEnabled(false);
        stopAction->setEnabled(true);
        m_LabelRunning->setVisible(true);
    }
    inline void int_buttons_onstop()
    {
        runAction->setEnabled(true);
        pauseAction->setEnabled(false);
        stopAction->setEnabled(false);
        m_LabelRunning->setVisible(false);
    }
    inline void int_buttons_onstart()
    {
        runAction->setEnabled(false);
        pauseAction->setEnabled(true);
        stopAction->setEnabled(true);
        m_LabelRunning->setVisible(true);
    }

public slots:

    void OnSave(bool);
    void OnSaveas(bool);
    void OnLoad(bool);
    void OnNew(bool);
    void OnRun(bool);
    void OnPause(bool);
    void OnStop(bool);
    void OnCompile(bool);
    void OnPreviewToggle(bool);


};
