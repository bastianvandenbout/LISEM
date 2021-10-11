#ifndef CONSOLETOOL_H
#define CONSOLETOOL_H


#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QString>

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "model.h"

#include "scripting/codeeditor.h"


#include "QLineEdit"
#include "QTimer"


#include "error.h"


#include "scriptmanager.h"

#include "mlpack/core.hpp"




class ConsoleTool : public QWidget
{
    Q_OBJECT;

public:



    QPlainTextEdit *m_Console;
    QLineEdit * m_ConsoleLineEdit;
    ScriptManager * m_ScriptManager;

    inline ConsoleTool( QWidget *parent = 0, const char *name = 0, ScriptManager * sm = NULL): QWidget( parent)
    {


        m_ScriptManager = sm;

        m_Console = new QPlainTextEdit(this);
        m_ConsoleLineEdit = new QLineEdit(this);
        m_Console->setWordWrapMode(QTextOption::NoWrap);
        m_Console->setMaximumHeight(1900);
        m_Console->clear();

        QSizePolicy sizePolicy;
        sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
        sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
        m_Console->setSizePolicy(sizePolicy);

        connect(m_ConsoleLineEdit,SIGNAL(returnPressed()),this,SLOT(OnConsoleEnterPressed()));
        QTimer::singleShot(30,this,SLOT(UpdateConsole()));

        QVBoxLayout *gblayout = new QVBoxLayout;
        this->setLayout(gblayout);
        gblayout->addWidget(m_Console);
        gblayout->addWidget(m_ConsoleLineEdit);
        gblayout->stretch(0);




    }

    inline ~ConsoleTool()
    {



    }



public slots:

    void UpdateConsole()
    {
        QList<LeveledMessage> list = GetMessages();

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
            m_Console->appendHtml(line);
        }

        QTimer::singleShot(30,this,SLOT(UpdateConsole()));

    }

    void OnConsoleEnterPressed()
    {

        QString command = m_ConsoleLineEdit->text();
        m_Console->appendPlainText(m_ConsoleLineEdit->text());
        m_ConsoleLineEdit->setText("");

        m_ScriptManager->RunCommand(command);

    }


};

#endif // CONSOLETOOL_H
