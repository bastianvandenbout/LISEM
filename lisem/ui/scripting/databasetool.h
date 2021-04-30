#ifndef DATABASETOOL_H
#define DATABASETOOL_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include "functional"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "codeeditor.h"
#include "QLineEdit"
#include "QIcon"
#include "lsmio.h"
#include "QCoreApplication"
#include "QToolButton"
#include "QComboBox"
#include "scripttool.h"
#include <QSplitter>
#include <iostream>
#include <QTimer>
#include <iostream>
#include <QFileIconProvider>
#include "widgets/layerinfowidget.h"
#include "widgets/elidedlabel.h"
#include "extensionprovider.h"
#include "QTextBlock"
#include "QScrollBar"
#include "site.h"

class SPHFileSystemModel : public QFileSystemModel
{

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_delete;
    QIcon *icon_info;
    QIcon *icon_map;
    QIcon *icon_script;
    QIcon *icon_runfile;
    QIcon *icon_diropen;
    QIcon *icon_vector;
    QIcon *icon_pointcloud;
    QIcon *icon_table;
    QIcon *icon_debug;

    QString m_Dir;
public:

    inline SPHFileSystemModel() : QFileSystemModel()
    {
        m_Dir = GetSite();

        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();
        icon_delete = new QIcon();
        icon_map = new QIcon();
        icon_script = new QIcon();
        icon_runfile = new QIcon();
        icon_diropen = new QIcon();
        icon_table = new QIcon();
        icon_vector = new QIcon();
        icon_pointcloud = new QIcon();
        //icon_debug = new QIcon();

        icon_start->addFile((m_Dir + LISEM_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + LISEM_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + LISEM_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_delete->addFile((m_Dir + LISEM_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_map->addFile((m_Dir + LISEM_FOLDER_ASSETS + "map.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_script->addFile((m_Dir + LISEM_FOLDER_ASSETS + "onewfile.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_runfile->addFile((m_Dir + LISEM_FOLDER_ASSETS + "iconfinder_Report_132648.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_diropen->addFile((m_Dir + LISEM_FOLDER_ASSETS + "diropen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_table->addFile((m_Dir + LISEM_FOLDER_ASSETS + "table.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_vector->addFile((m_Dir + LISEM_FOLDER_ASSETS + "vector.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pointcloud->addFile((m_Dir + LISEM_FOLDER_ASSETS + "pointcloud.png"), QSize(), QIcon::Normal, QIcon::Off);
        //icon_debug->addFile((m_Dir + LISEM_FOLDER_ASSETS + "debug.png"), QSize(), QIcon::Normal, QIcon::Off);
    }

    inline QVariant data( const QModelIndex& index, int role ) const override
    {

        if( role == Qt::DecorationRole && index.column() == 0 )
        {
            QFileInfo info = SPHFileSystemModel::fileInfo(index);

            if(info.isFile())
            {
                if(IsMapFile(info.filePath()) )
                {
                    return *icon_map;//I pick the icon depending on the extension
                }else if(info.suffix() == "run")
                {
                    return *icon_runfile;
                }
                else if(IsScriptFile(info.filePath()) )
                {
                    return *icon_script;
                }else if(IsShapeFile(info.filePath()) )
                {
                    return *icon_vector;
                }else if(IsPointCloudFile(info.filePath()))
                {
                    return *icon_pointcloud;
                }else if(IsTableFile(info.filePath()) )
                {
                    return *icon_table;
                }
            }
        }
        return QFileSystemModel::data(index, role);
    }


};


class LSMLineEdit : public QPlainTextEdit
{
        Q_OBJECT;
public:


    QList<QString> m_CommandHistory;
    int m_CommandHistoryIndex = -1;
    bool m_AllowCommands = true;

    ScriptManager * m_ScriptManager;

    SPHScriptCompleter * m_Completer = nullptr;
    QList<ScriptFunctionInfo> funclist;
    QList<ScriptFunctionInfo> funcmemberlist;


    int m_CurrentToolTipIndex = 0;

    inline LSMLineEdit(ScriptManager * s, QWidget * p) : QPlainTextEdit(p)
    {

        m_ScriptManager = s;

        QFontMetrics metrics(font());
        int lineHeight = metrics.lineSpacing();
        setFixedHeight(std::max(30,lineHeight));

        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setLineWrapMode(QPlainTextEdit::NoWrap);

        setMouseTracking(true);

            connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateExtraSelection()));

        if(m_ScriptManager != nullptr)
        {
            QStringList functionnames = m_ScriptManager->GetGlobalFunctionNames();

            funclist = m_ScriptManager->m_Engine->GetGlobalFunctionList();
            funcmemberlist = m_ScriptManager->m_Engine->GetMemberFunctionList();

            m_Completer = new SPHScriptCompleter(m_ScriptManager);
            m_Completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
            m_Completer->setCompletionMode(QCompleter::PopupCompletion);
            m_Completer->setWrapAround(true);
            this->setCompleter(m_Completer);

        }


        connect(this,SIGNAL(returnPressed()),this,SLOT(OnConsoleEnterPressed()));
    }

    inline void setCompleter(SPHScriptCompleter *completer)
    {
        if (m_Completer)
        {
            QObject::disconnect(m_Completer, 0, this, 0);
        }

        m_Completer = completer;

        if (!m_Completer)
            return;

        m_Completer->setWidget(this);
        m_Completer->setCompletionMode(QCompleter::PopupCompletion);
        m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
        QObject::connect(m_Completer, QOverload<const QString &>::of(&QCompleter::activated),
                         this, &LSMLineEdit::insertCompletion);
    }

    void insertCompletion(const QString& completion)
    {
        if (m_Completer->widget() != this)
        {
            return;
        }

        QTextCursor tc = textCursor();
        int extra = completion.length() - m_Completer->completionPrefix().length();
        tc.movePosition(QTextCursor::Left);
        tc.movePosition(QTextCursor::EndOfWord);
        tc.insertText(completion.right(extra));
        setTextCursor(tc);
    }

    inline void SetDir(QString dir)
    {
        if(m_Completer != nullptr)
        {
            m_Completer->SetDirectory(dir);
        }
    }

    QString textUnderCursor() const
    {
        QTextCursor tc = textCursor();
        tc.select(QTextCursor::WordUnderCursor);

        QString word = tc.selectedText();
        int selstart =  tc.anchor();
        tc.setPosition(selstart-1);
        tc.setPosition(selstart, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();
        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }
        return word;
    }

    QString textUnderCursor(QTextCursor tc) const
    {
        tc.select(QTextCursor::WordUnderCursor);

        QString word = tc.selectedText();
        int selstart =  tc.anchor();
        tc.setPosition(selstart-1);
        tc.setPosition(selstart, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();
        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }
        return word;
    }

    inline void keyPressEvent(QKeyEvent *event) override
    {

        if (m_Completer && m_Completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
           switch (event->key()) {
           case Qt::Key_Enter:
           case Qt::Key_Return:
           case Qt::Key_Escape:
           case Qt::Key_Tab:
           case Qt::Key_Backtab:
                event->ignore();
                return; // let the completer do default behavior
           default:
               break;
           }
        }


        if(QToolTip::isVisible())
        {

           if(event->key() == Qt::Key_PageUp)
           {
               m_CurrentToolTipIndex += 1;
               updateExtraSelection();
               return;
           }
           if(event->key() == Qt::Key_PageDown)
           {
               m_CurrentToolTipIndex -= 1;
               m_CurrentToolTipIndex = std::max(m_CurrentToolTipIndex,0);
               updateExtraSelection();
               return;
           }
        }else {

            m_CurrentToolTipIndex = 0;

        }

        bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E

        if(!m_Completer || !isShortcut)
        {

            if((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down))
            {
                int m_CommandHistoryIndexO =  m_CommandHistoryIndex;

                if((event->key() == Qt::Key_Up) )
                {

                     m_CommandHistoryIndex += 1;
                }
                else if(event->key() == Qt::Key_Down)
                {
                    m_CommandHistoryIndex -= 1;
                }


                //limit to -1 (new command) and length of the history
                 m_CommandHistoryIndex = std::min(m_CommandHistory.length()-1,std::max(-1, m_CommandHistoryIndex));

                 //if there is a difference in index, we know we must change the text
                 if(m_CommandHistoryIndexO != m_CommandHistoryIndex)
                 {
                       if(m_CommandHistoryIndex == -1)
                       {
                            this->document()->setPlainText("");
                       }else
                       {
                            this->document()->setPlainText(m_CommandHistory.at(m_CommandHistoryIndex));
                       }

                 }




            }else {

                if( (event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
                {
                    OnConsoleEnterPressed();
                }else
                {
                    QPlainTextEdit::keyPressEvent(event);
                }

            }
        }



        const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
        if (!m_Completer || (ctrlOrShift && event->text().isEmpty()))
        {
            return;
        }

        static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word
        bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
        QString completionPrefix = textUnderCursor();

        if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3
                          || eow.contains(event->text().right(1)))) {

            m_Completer->popup()->hide();
            return;
        }


        m_Completer->update(completionPrefix);
        //if (completionPrefix != m_Completer->completionPrefix()) {

            m_Completer->popup()->setCurrentIndex(m_Completer->completionModel()->index(0, 0));
        //}
        QRect cr = cursorRect();
        cr.setWidth(m_Completer->popup()->sizeHintForColumn(0)
                    + m_Completer->popup()->verticalScrollBar()->sizeHint().width());



        m_Completer->complete(cr); // popup it up!


        m_Completer->ShowPopup0();

    }
    QString wordUnderCursor(QTextCursor tc) const
    {
        tc.select(QTextCursor::WordUnderCursor);
        return tc.selectedText();
    }


    QString wordUnderCursor() const
    {
        auto tc = textCursor();
        tc.select(QTextCursor::WordUnderCursor);
        return tc.selectedText();
    }

    QChar CharUnderCursor(int offset = 0) const
    {
        auto block = textCursor().blockNumber();
        auto index = textCursor().positionInBlock();
        auto text = document()->findBlockByNumber(block).text();

        index += offset;

        if (index < 0 || index >= text.size())
        {
            return {};
        }

        return text[index];
    }

    QChar CharUnderCursor(QTextCursor tc, int offset = 0) const
    {
        auto block = tc.blockNumber();
        auto index = tc.positionInBlock();
        auto text = document()->findBlockByNumber(block).text();

        index += offset;

        if (index < 0 || index >= text.size())
        {
            return {};
        }

        return text[index];
    }

    void highlightParenthesis(QList<QTextEdit::ExtraSelection>& extraSelection)
    {
        auto currentSymbol = CharUnderCursor();
        auto prevSymbol = CharUnderCursor(-1);

        for (auto& pair : parentheses)
        {
            int direction;

            QChar counterSymbol;
            QChar activeSymbol;
            auto position = textCursor().position();

            if (pair.first == currentSymbol)
            {
                direction = 1;
                counterSymbol = pair.second[0];
                activeSymbol = currentSymbol;
            }
            else if (pair.second == prevSymbol)
            {
                direction = -1;
                counterSymbol = pair.first[0];
                activeSymbol = prevSymbol;
                position--;
            }
            else
            {
                continue;
            }

            auto counter = 1;

            while (counter != 0 &&
                   position > 0 &&
                   position < (document()->characterCount() - 1))
            {
                // Moving position
                position += direction;

                auto character = document()->characterAt(position);
                // Checking symbol under position
                if (character == activeSymbol)
                {
                    ++counter;
                }
                else if (character == counterSymbol)
                {
                    --counter;
                }
            }

            QTextCharFormat format = QTextCharFormat();
            format.setBackground(Qt::yellow);

            // Found
            if (counter == 0)
            {
                QTextEdit::ExtraSelection selection{};

                auto directionEnum =
                     direction < 0 ?
                     QTextCursor::MoveOperation::Left
                     :
                     QTextCursor::MoveOperation::Right;

                selection.format = format;
                selection.cursor = textCursor();
                selection.cursor.clearSelection();
                selection.cursor.movePosition(
                    directionEnum,
                    QTextCursor::MoveMode::MoveAnchor,
                    std::abs(textCursor().position() - position)
                );

                selection.cursor.movePosition(
                    QTextCursor::MoveOperation::Right,
                    QTextCursor::MoveMode::KeepAnchor,
                    1
                );

                extraSelection.append(selection);

                selection.cursor = textCursor();
                selection.cursor.clearSelection();
                selection.cursor.movePosition(
                    directionEnum,
                    QTextCursor::MoveMode::KeepAnchor,
                    1
                );

                extraSelection.append(selection);
            }

            break;
        }
    }


    void setToolTipAtCursor(QTextCursor TextCursor, QList<QTextEdit::ExtraSelection>& extraSelection)
    {
        QChar currentSymbol = '(';
        QChar prevSymbol = ')';

        QChar rcurrentSymbol = CharUnderCursor(TextCursor);
        QChar rprevSymbol = CharUnderCursor(TextCursor,-1);

        bool found_start = false;
        bool found_end = false;
        int pos_start = -1;
        int pos_end = -1;
        int comma_before = 0;
        int comma_after = 0;

        for (auto& pair : parentheses)
        {
            for(int o = 0; o < 2; o++)
            {
                int direction;
                int count_comma = 0;

                QChar counterSymbol;
                QChar activeSymbol;
                auto position = TextCursor.position();

                bool include_self = false;

                if(o < 1)
                {
                    if (pair.first == currentSymbol)
                    {
                        direction = 1;
                        counterSymbol = pair.second[0];
                        activeSymbol = currentSymbol;
                        if(prevSymbol == rprevSymbol)
                        {
                            include_self = true;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }else
                {
                    if (pair.second == prevSymbol)
                    {
                        direction = -1;
                        counterSymbol = pair.first[0];
                        activeSymbol = prevSymbol;
                        position--;

                        if(currentSymbol == rcurrentSymbol)
                        {
                            include_self = true;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                //position += direction;

                auto counter = 1;
                bool stringliteral = false;

                while (counter != 0 &&
                       position > 0 &&
                       position < (document()->characterCount() - 1))
                {


                    auto character = document()->characterAt(position);
                    // Checking symbol under position
                    if (character == activeSymbol)
                    {
                        ++counter;
                    }
                    else if (character == counterSymbol)
                    {
                        --counter;
                    }else if(character == "\"")
                    {
                        stringliteral = !stringliteral;
                    }else if(character == ',')
                    {
                        if(!stringliteral)
                        {
                            count_comma++;
                        }
                    }

                    if(counter != 0)
                    {
                        // Moving position
                        position += direction;
                    }
                }

                QTextCharFormat format = QTextCharFormat();
                format.setBackground(Qt::green);

                // Found
                if (counter == 0)
                {
                    if(o == 0)
                    {
                        found_end = true;
                        pos_end = position;
                        comma_after = count_comma;
                    }else {
                        found_start = true;
                        pos_start = position;
                        comma_before = count_comma;
                    }
                    QTextEdit::ExtraSelection selection{};

                    auto directionEnum =
                         direction < 0 ?
                         QTextCursor::MoveOperation::Left
                         :
                         QTextCursor::MoveOperation::Right;

                    selection.format = format;
                    selection.cursor = TextCursor;
                    selection.cursor.clearSelection();
                    selection.cursor.movePosition(
                        directionEnum,
                        QTextCursor::MoveMode::MoveAnchor,
                        std::abs(TextCursor.position() - position)
                    );

                    selection.cursor.movePosition(
                        QTextCursor::MoveOperation::Right,
                        QTextCursor::MoveMode::KeepAnchor,
                        1
                    );

                    extraSelection.append(selection);

                    if(include_self)
                    {
                        selection.cursor = TextCursor;
                        selection.cursor.clearSelection();
                        selection.cursor.movePosition(
                            directionEnum,
                            QTextCursor::MoveMode::KeepAnchor,
                            1
                        );

                        extraSelection.append(selection);
                    }
                }
            }
            break;
        }


        //if we found the end and the beginning, try to get a function name
        QTextCursor cursor =TextCursor;
        cursor.movePosition(QTextCursor::MoveOperation::Left,QTextCursor::MoveMode::MoveAnchor,std::abs(TextCursor.position() - pos_start)+1);
        cursor.select(QTextCursor::WordUnderCursor);
        QString functionname = cursor.selectedText();
        std::cout << TextCursor.position() << " " << pos_start << " "<< cursor.selectedText().toStdString() << std::endl;

        QString curseractualtext = textUnderCursor(TextCursor);
        bool found_wordmatch = false;
        for(int i = 0; i < funclist.length(); i++)
        {
            ScriptFunctionInfo fi = funclist.at(i);
            if(QString(fi.Function->GetName()).compare(curseractualtext) == 0)
            {
                found_wordmatch = true;
            }
        }

        if(!(found_end && found_start) || found_wordmatch)
        {
            functionname = textUnderCursor(TextCursor);
            comma_after = 0;
            comma_before = 0;
        }


        QList<ScriptFunctionInfo> fmatches;
        bool found_match = false;
        bool found_nargs = 0;
        //now get the best matches for a function
        //based on argument count, tries to get all overloaded function defenitions with the closes matching number of arguments
        for(int i = 0; i < funclist.length(); i++)
        {
            ScriptFunctionInfo fi = funclist.at(i);
            if(QString(fi.Function->GetName()).compare(functionname) == 0)
            {
                //perfect match
                if(fi.Parameters.length() == (comma_after+comma_before + 1))
                {
                    if(!found_match)
                    {
                        fmatches.clear();
                    }
                    found_match = true;

                    fmatches.append(fi);
                }else
                {
                    if(!found_match)
                    {
                        if(fmatches.length() == 0)
                        {
                            fmatches.append(fi);
                            found_nargs = fi.Parameters.length();
                        }else
                        {
                            ScriptFunctionInfo fi2 =fmatches.at(0);

                            if(fi.Parameters.length() == fi2.Parameters.length())
                            {
                                fmatches.append(fi);
                            }else if((fi.Parameters.length() >  (comma_after+comma_before + 1)) && (fi2.Parameters.length() <  (comma_after+comma_before + 1)))
                            {
                                fmatches.clear();
                                fmatches.append(fi);
                                found_nargs = fi.Parameters.length();
                            }else
                            {
                                if(std::fabs(fi.Parameters.length() -  (comma_after+comma_before + 1)) < std::fabs(fi2.Parameters.length() -  (comma_after+comma_before + 1)))
                                {
                                    fmatches.clear();
                                    fmatches.append(fi);
                                    found_nargs = fi.Parameters.length();
                                }
                            }
                        }
                    }
                }

            }
        }

        if(fmatches.size() > 0 && (found_end && found_start))
        {

            std::cout << "found matches and function" << std::endl;


            int match = m_CurrentToolTipIndex % fmatches.size();

            ScriptFunctionInfo fi = fmatches.at(match);
            //set the tooltip text
            QString tooltip = "(" +QString::number(match+1) +"/" + QString::number(fmatches.size()) + ") " + m_ScriptManager->m_Engine->GetTypeName(fi.Function->GetReturnTypeId()) + " " + fi.Function->GetName() + "(";

            int plength = fi.Parameters.length();
            if(plength == fi.Function->GetParamCount())
            {
                int pcurrent = std::min(comma_before,plength-1);
                int ibegin = 0;
                int iend = plength;

                bool plimitbegin = false;
                bool plimitend = false;

                if(plength > 5)
                {

                    ibegin = pcurrent - 1;
                    if(ibegin < 0)
                    {
                        ibegin = 0;
                    }else if(ibegin > 0)
                    {
                        plimitbegin = true;
                    }
                    iend = ibegin + 3;
                    if(iend > plength)
                    {
                        iend = plength;
                    }else if(ibegin < plength)
                    {
                        plimitend = true;
                    }
                }

                if(plimitbegin)
                {
                    tooltip += "...";
                }

                std::cout << "middle "<< ibegin << " "<< iend <<  " " << fi.ParameterTypes.length() << " " << fi.Parameters.length() <<  std::endl;
                for(int i = ibegin; i < iend; i++)
                {
                    int typeId = 0;
                    asDWORD flags = 0;
                    const char * name;
                    const char * def;
                    fi.Function->GetParam(i,&typeId,&flags,&name,&def);
                    tooltip +=  m_ScriptManager->m_Engine->GetTypeName(typeId) + " " + fi.Parameters.at(i);

                    if(i != iend-1)
                    {
                        tooltip += ",";
                    }
                }

                std::cout << "end "<< std::endl;

                if(plimitend)
                {
                    tooltip += "...";
                }
                tooltip += ")";

                QString typen;
                if(pcurrent < fi.Function->GetParamCount())
                {
                    int typeId = 0;
                    asDWORD flags = 0;
                    const char * name;
                    const char * def;
                    fi.Function->GetParam(pcurrent,&typeId,&flags,&name,&def);

                     typen = m_ScriptManager->m_Engine->GetTypeName(typeId);
                }else
                {
                    typen = "";
                }


                tooltip += "\n \n";
                tooltip += "arg. " +QString::number(pcurrent+1) + "/" + QString::number(plength) + " " + fi.Parameters.at(pcurrent) + " (" +typen + ")\n";

                tooltip += fi.ParameterDescription.at(pcurrent);

                //show tooltip
                QToolTip::showText(viewport()->mapToGlobal(QPoint(cursorRect(TextCursor).right(),cursorRect(TextCursor).bottom())), QString(tooltip));

            }

        }else
        {
            if(fmatches.size() > 0)
            {
                int match = m_CurrentToolTipIndex % fmatches.size();


                ScriptFunctionInfo fi = fmatches.at(match);
                //set the tooltip text
                QString tooltip = "(" +QString::number(match+1) +"/" + QString::number(fmatches.size()) + ") " +m_ScriptManager->m_Engine->GetTypeName(fi.Function->GetReturnTypeId()) + " " + fi.Function->GetName() + "(";

                int plength = fi.Parameters.length();
                if(plength == fi.Function->GetParamCount())
                {

                        int ibegin = 0;
                    int iend = plength;

                    bool plimitbegin = false;
                    bool plimitend = false;

                    for(int i = ibegin; i < iend; i++)
                    {
                        int typeId = 0;
                        asDWORD flags = 0;
                        const char * name;
                        const char * def;
                        fi.Function->GetParam(i,&typeId,&flags,&name,&def);

                        tooltip += m_ScriptManager->m_Engine->GetTypeName(typeId) + " " + fi.Parameters.at(i);

                        if(i != iend-1)
                        {
                            tooltip += ",";
                        }
                    }
                    if(plimitend)
                    {
                        tooltip += "...";
                    }
                    tooltip += ")";
                    //show tooltip


                    QToolTip::showText(viewport()->mapToGlobal(QPoint(cursorRect(TextCursor).right(),cursorRect(TextCursor).bottom())), QString(tooltip));
                }
            }else
            {
                QToolTip::hideText();
            }

        }


        std::cout << "done"<< std::endl;

    }

    inline void AllowCommands(bool x)
    {
        m_AllowCommands = x;
    }

protected:
    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::ToolTip)
        {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            QTextCursor cursor = cursorForPosition(helpEvent->pos());
            QList<QTextEdit::ExtraSelection> selects;
            cursor.select(QTextCursor::WordUnderCursor);
            if (!cursor.selectedText().isEmpty())
            {
                m_CurrentToolTipIndex = 0;
                setToolTipAtCursor(cursorForPosition(helpEvent->pos()),selects);
            }
            else
            {
                QToolTip::hideText();
            }
            return true;
        }
        return QPlainTextEdit::event(event);
    }
public:
signals:

    void OnCommandGiven(QString c);


public slots:

    void updateExtraSelection()
    {
        QList<QTextEdit::ExtraSelection> extra;

        highlightParenthesis(extra);

        QList<QTextEdit::ExtraSelection>& extraSelection = extra;

        setToolTipAtCursor(textCursor(),extraSelection);

        setExtraSelections(extra);
    }

    inline void OnConsoleEnterPressed()
    {
        if(m_AllowCommands)
        {

            QString command = this->document()->toRawText();

            if(!command.isEmpty())
            {

                m_CommandHistory.prepend(command);
                m_CommandHistoryIndex = -1;
                OnCommandGiven(command);

            }
        }

    }



};


class DatabaseTool : public QWidget
{
        Q_OBJECT;

public:

    QMutex * m_ConsoleMutex;
    bool m_ConsoleRunning = false;

    QSplitter *m_MainWidget;
    QPlainTextEdit *m_FileConsole;
    LSMLineEdit * m_FileConsoleLineEdit;
    ScriptTool * m_FileEditor;
    QTabWidget * m_TabFiles;
    QWidget * m_MenuWidget;
    QHBoxLayout * m_MenuLayout;
    QComboBox * m_DirBox;

    QString m_Dir;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_delete;
    QIcon *icon_info;
    QIcon *icon_map;
    QIcon *icon_script;
    QIcon *icon_runfile;
    QIcon *icon_diropen;

    ScriptManager *m_ScriptManager;

    QFileSystemModel *model;
    QTreeView *m_BrowseTree;
    SPHFileSystemModel *model2;
    QTreeView *m_BrowseTree2;

    ElidedLabel * m_DirLabel;

    QString m_Path_Work;
    QString m_Path_Current;
    QString m_Path_OpenFiles;

    inline DatabaseTool( ScriptManager * sm,QWidget *parent = 0, const char *name = 0 ): QWidget( parent)
    {

        m_ScriptManager = sm;
        m_Dir = GetSite();

        m_ConsoleMutex = new QMutex();

        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();
        icon_delete = new QIcon();
        icon_map = new QIcon();
        icon_script = new QIcon();
        icon_runfile = new QIcon();
        icon_diropen = new QIcon();

        icon_start->addFile((m_Dir + LISEM_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + LISEM_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + LISEM_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + LISEM_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + LISEM_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_delete->addFile((m_Dir + LISEM_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_map->addFile((m_Dir + LISEM_FOLDER_ASSETS + "map.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_script->addFile((m_Dir + LISEM_FOLDER_ASSETS + "onewfile.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_runfile->addFile((m_Dir + LISEM_FOLDER_ASSETS + "iconfinder_Report_132648.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_diropen->addFile((m_Dir + LISEM_FOLDER_ASSETS + "diropen.png"), QSize(), QIcon::Normal, QIcon::Off);


        model = new QFileSystemModel();
        model->setRootPath("");
        model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileIconProvider * iconp =  model->iconProvider();
        m_BrowseTree = new QTreeView();
        QString rootPath = "";//GetSite();
        m_BrowseTree->setModel(model);
           if (!rootPath.isEmpty()) {
                const QModelIndex rootIndex = model->index(QDir::cleanPath(rootPath));
                if (rootIndex.isValid())
                    m_BrowseTree->setRootIndex(rootIndex);
            }


       m_BrowseTree->setAnimated(false);
       m_BrowseTree->setIndentation(20);
       m_BrowseTree->setSortingEnabled(true);
       m_BrowseTree->hideColumn(1);
       m_BrowseTree->hideColumn(2);
       m_BrowseTree->hideColumn(3);
       m_BrowseTree->setDropIndicatorShown(true);
       m_BrowseTree->setAcceptDrops(true);
       m_BrowseTree->viewport()->setAcceptDrops(true);

       model2 = new SPHFileSystemModel();
       model2->setRootPath("C:/");
       model2->setFilter(QDir::Files | QDir::NoDotAndDotDot);

       QFileIconProvider * iconp2 =  model->iconProvider();

       m_BrowseTree2 = new QTreeView();
       m_BrowseTree2->setDragEnabled(true);
       m_BrowseTree2->setDropIndicatorShown(true);
       m_BrowseTree2->setAcceptDrops(true);
       m_BrowseTree2->setColumnWidth(0,300);
       m_BrowseTree2->viewport()->setAcceptDrops(true);
       QString rootPath2 = GetSite();
       m_BrowseTree2->setModel(model2);
          if (!rootPath2.isEmpty()) {
               const QModelIndex rootIndex2 = model2->index(QDir::cleanPath(rootPath));
               if (rootIndex2.isValid())
                   m_BrowseTree2->setRootIndex(rootIndex2);
           }


      m_BrowseTree2->setAnimated(false);
      m_BrowseTree2->setIndentation(20);
      m_BrowseTree2->setSortingEnabled(true);


     m_FileConsole = new QPlainTextEdit(this);
     m_FileConsoleLineEdit = new LSMLineEdit(m_ScriptManager,this);

     m_TabFiles = new QTabWidget();
     m_TabFiles->setTabsClosable(true);
     m_TabFiles->setMovable(true);

     m_FileEditor = new ScriptTool(sm);


     m_MenuWidget = new QWidget(this);
     m_MenuLayout = new QHBoxLayout(m_MenuWidget);
     m_MenuLayout->setMargin(0);


     QToolButton *DeleteButton = new QToolButton(this);
     DeleteButton->setIcon(*icon_delete);
     DeleteButton->setIconSize(QSize(22,22));
     DeleteButton->setMaximumSize(QSize(22,22));
     DeleteButton->resize(22,22);
     DeleteButton->setEnabled(true);
     DeleteButton->setToolTip("Delete file or folder");

     QToolButton *NewFButton = new QToolButton(this);
     NewFButton->setIcon(*icon_new);
     NewFButton->setIconSize(QSize(22,22));
     NewFButton->setMaximumSize(QSize(22,22));
     NewFButton->resize(22,22);
     NewFButton->setEnabled(true);
     NewFButton->setToolTip("Create new directory");

     QToolButton *OpenButton = new QToolButton(this);
     OpenButton->setIcon(*icon_open);
     OpenButton->setIconSize(QSize(22,22));
     OpenButton->setMaximumSize(QSize(22,22));
     OpenButton->resize(22,22);
     OpenButton->setEnabled(true);
     OpenButton->setToolTip("Open file");

     QToolButton *DisplayButton = new QToolButton(this);
     DisplayButton->setIcon(*icon_map);
     DisplayButton->setIconSize(QSize(22,22));
     DisplayButton->setMaximumSize(QSize(22,22));
     DisplayButton->resize(22,22);
     DisplayButton->setEnabled(true);
     DisplayButton->setToolTip("Open in map viewer");

     QToolButton *OpenScriptButton = new QToolButton(this);
     OpenScriptButton->setIcon(*icon_script);
     OpenScriptButton->setIconSize(QSize(22,22));
     OpenScriptButton->setMaximumSize(QSize(22,22));
     OpenScriptButton->resize(22,22);
     OpenScriptButton->setEnabled(true);
     OpenScriptButton->setToolTip("Open script file");

     QToolButton *OpenRunfileButton = new QToolButton(this);
     OpenRunfileButton->setIcon(*icon_runfile);
     OpenRunfileButton->setIconSize(QSize(22,22));
     OpenRunfileButton->setMaximumSize(QSize(22,22));
     OpenRunfileButton->resize(22,22);
     OpenRunfileButton->setEnabled(true);
     OpenRunfileButton->setToolTip("Open run file");

     QToolButton *InfoButton = new QToolButton(this);
     InfoButton->setIcon(*icon_info);
     InfoButton->setIconSize(QSize(22,22));
     InfoButton->setMaximumSize(QSize(22,22));
     InfoButton->resize(22,22);
     InfoButton->setEnabled(true);
     InfoButton->setToolTip("Print file info");

     QToolButton *DirOpenButton = new QToolButton(this);
     DirOpenButton->setIcon(*icon_diropen);
     DirOpenButton->setIconSize(QSize(22,22));
     DirOpenButton->setMaximumSize(QSize(22,22));
     DirOpenButton->resize(22,22);
     DirOpenButton->setEnabled(true);
     DirOpenButton->setToolTip("Set current directory as working directory");


     m_DirLabel = new ElidedLabel();

     m_MenuLayout->setMargin(0);
     m_MenuLayout->addWidget(DeleteButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(NewFButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(DisplayButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenScriptButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenRunfileButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(InfoButton,0,Qt::AlignLeft);
     m_MenuLayout->addItem(new QSpacerItem(20,20));
     m_MenuLayout->addWidget(DirOpenButton);
     m_MenuLayout->addWidget(m_DirLabel);

     m_MenuLayout->setMargin(2);
     m_MenuLayout->setSpacing(2);

     m_MenuLayout->addStretch();

     QSplitter * m_BrowseWidget = new QSplitter();
     QHBoxLayout * m_BrowseHLayout = new QHBoxLayout();
     //m_BrowseWidget->setLayout(m_BrowseHLayout);
     QWidget * mapVtab = new QWidget();
     QHBoxLayout *maplayout = new QHBoxLayout();
     QVBoxLayout *mapvlayout = new QVBoxLayout();

     m_MainWidget = new QSplitter();
     this->setLayout(maplayout);
     maplayout->addWidget(m_MainWidget);

     maplayout->setMargin(2);
     maplayout->setSpacing(2);
     mapvlayout->setMargin(2);
     mapvlayout->setSpacing(2);
     m_BrowseHLayout->setMargin(2);
     m_BrowseHLayout->setSpacing(2);

     m_MainWidget->addWidget(m_FileEditor);
     m_MainWidget->setCollapsible(m_MainWidget->indexOf(m_FileEditor),false);
     m_MainWidget->setStretchFactor(m_MainWidget->indexOf(m_FileEditor),3);

     m_MainWidget->addWidget(mapVtab);
     m_MainWidget->setCollapsible(m_MainWidget->indexOf(mapVtab),false);
     m_MainWidget->setStretchFactor(m_MainWidget->indexOf(mapVtab),1);
     mapVtab->setLayout(mapvlayout);

     QSplitter * vsplit = new QSplitter();
     vsplit->setOrientation(Qt::Vertical);

     QWidget * vwidg1 = new QWidget();
     QVBoxLayout * vwidg1l = new QVBoxLayout();
     vwidg1->setLayout(vwidg1l);
     vwidg1l->setMargin(2);
     vwidg1l->setSpacing(2);
     vwidg1l->setMargin(2);
     vwidg1l->setSpacing(2);

     QWidget * vwidg2 = new QWidget();
     QVBoxLayout * vwidg2l = new QVBoxLayout();
     vwidg2->setLayout(vwidg2l);
     vwidg2l->setMargin(2);
     vwidg2l->setSpacing(2);
     vwidg2l->setMargin(2);
     vwidg2l->setSpacing(2);


     vwidg1l->addWidget(m_BrowseWidget);
     m_BrowseWidget->addWidget(m_BrowseTree);
     m_BrowseWidget->addWidget(m_BrowseTree2);
     vwidg2l->addWidget(m_MenuWidget);
     vwidg2l->addWidget(m_FileConsole);
     vwidg2l->addWidget(m_FileConsoleLineEdit);

     vsplit->addWidget(vwidg1);
     vsplit->addWidget(vwidg2);

     mapvlayout->addWidget(vsplit);


     connect(m_BrowseTree,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnFileModelClicked(const QModelIndex &)));

     connect(m_BrowseTree2,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnFileModel2Clicked(const QModelIndex &)));
     connect(m_BrowseTree2,SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(OnFileModel2DClicked(const QModelIndex &)));

     connect(DeleteButton,SIGNAL(pressed()),this,SLOT(OnDelete()));
     connect(NewFButton,SIGNAL(pressed()),this,SLOT(OnNewFolder()));
     connect(OpenButton,SIGNAL(pressed()),this,SLOT(OnOpen()));
     connect(DisplayButton,SIGNAL(pressed()),this,SLOT(OnDisplay()));
     connect(OpenScriptButton,SIGNAL(pressed()),this,SLOT(OnOpenScript()));
     connect(OpenRunfileButton,SIGNAL(pressed()),this,SLOT(OnOpenRunFile()));
     connect(InfoButton,SIGNAL(pressed()),this,SLOT(OnInfo()));
     connect(DirOpenButton,SIGNAL(pressed()),this,SLOT(OnDirOpen()));


     connect(m_FileConsoleLineEdit,SIGNAL(OnCommandGiven(QString)),this,SLOT(OnConsoleCommand(QString)));
     QTimer::singleShot(0,this,SLOT(UpdateConsole()));

     m_Path_Current = GetSite()+"/";
     m_Path_OpenFiles = m_Path_Current + "openloc.ini";


     QFile fin(m_Path_OpenFiles);
     if (!fin.open(QFile::ReadOnly | QFile::Text)) {

         if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
         {
             QTextStream out(&fin);
             out << QString("[Open Location]\n");
         }
         fin.close();
     }else
     {
         while (!fin.atEnd())
         {
             QString S = fin.readLine().trimmed();
             QFile fin_temp(S);
             if(!S.startsWith("["))
             {
                 if (QDir(S).exists())
                 {
                     QModelIndex index = model->index(QDir::cleanPath(S));
                     m_BrowseTree->setCurrentIndex(index);
                     SetRootFolder(S);
                     m_Path_Work = S;

                     m_DirLabel->setText(S);
                     m_FileEditor->SetHomeDir(S);
                     m_FileConsoleLineEdit->SetDir(S);
                 }
             }
         }
     }

     ExportOpenLoc();

    }

    inline void ExportOpenLoc()
    {
        QFile fin(m_Path_OpenFiles);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            out << QString("[Runfile List]\n");

            out << m_DirLabel->text() << "\n";

        }
        fin.close();

    }

    inline ~DatabaseTool()
    {

        delete m_ConsoleMutex;
    }

    std::function<bool(QString, int)> m_FileCallBack;
    bool m_FileCallBackSet = false;

    template<typename _Callable, typename... _Args>
    inline void SetCallBackFileOpened(_Callable&& __f, _Args&&... __args)
    {
        //now we store this function pointer and call it later when a file is openened
        m_FileCallBack = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1,std::placeholders::_2);
        m_FileCallBackSet = true;
    }

public slots:

    void UpdateConsole()
    {
        QList<LeveledMessage> list = GetMessagesS();

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
            m_FileConsole->appendHtml(line);
        }

        QTimer::singleShot(0.01,this,SLOT(UpdateConsole()));

    }

    inline void OnFileModelClicked(const QModelIndex &index)
    {
        SetRootFolder(model->filePath(index));
    }

    inline void SetRootFolder(QString rootPath2)
    {
        delete model2;
        model2 = new SPHFileSystemModel();
        model2->setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QFileIconProvider * iconp2 =  model->iconProvider();

        model2->setRootPath(rootPath2);
        m_BrowseTree2->setModel(model2);

        if (!rootPath2.isEmpty()) {
             const QModelIndex rootIndex2 = model2->index(QDir::cleanPath(rootPath2));
             if (rootIndex2.isValid())

                 m_BrowseTree2->setRootIndex(rootIndex2);
         }

        m_BrowseTree2->update();
        m_BrowseTree2->setColumnWidth(0,300);

    }

    inline void OnFileModel2DClicked(const QModelIndex &index)
    {
        QString filepath = model->filePath(index);

        QFileInfo f = QFileInfo(filepath);
        if(f.exists())
        {
            //check extension to determine automatic usage of the file
            //if nothing found, use the default os file opening routine

            if(filepath.length() > 3)
            {
                QString ext = filepath.right(4);

                if((ext.compare(".script") == 0) || (ext.compare(".run") == 0))
                {
                    //raster extension, try to open with map display
                    m_FileEditor->OpenCode(filepath);

                }else if(ext.compare(".tbl") == 0 || ext.compare(".csv") == 0)
                {
                    //table extension
                    MatrixTable * t = new MatrixTable();
                    t->LoadAsAutoFormat(filepath);

                    LayerInfoWidget *w = new LayerInfoWidget(t);
                    w->show();

                }
            }
            if(m_FileCallBackSet)
            {
                bool used = m_FileCallBack(filepath, LISEM_FILE_TYPE_UNKNOWN);
            }

        }else {

            LISEMS_ERROR("Could not open file: " +filepath);
        }


    }

    inline void OnFileModel2Clicked(const QModelIndex &index)
    {





    }

    inline void OnDelete()
    {
        if(m_BrowseTree->hasFocus())
        {
            QModelIndex index = m_BrowseTree->currentIndex();
            QString file = model->filePath(index);

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete folder" , "Delete " + file + "?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {


                QDir d(file);
                if(d.exists())
                {
                    bool succes = d.removeRecursively();
                    if(!succes)
                    {
                        QMessageBox::information(this,"Error","Error when deleting file: " + file);

                    }
                }else
                {
                    QMessageBox::information(this,"Error","Error when deleting file: " + file);

                }

            } else {


            }
        }else if(m_BrowseTree2->hasFocus())
        {
            QModelIndex index = m_BrowseTree2->currentIndex();
            QString file = model2->filePath(index);

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete file" , "Delete " + file + "?",
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {


                QFile f(file);
                if(f.exists())
                {
                    f.remove();
                }else {
                    QMessageBox::information(this,"Error","Error when deleting file: " + file);

                }
            } else {

            }

        }else {
            QMessageBox::information(this,"Error","No file selected!");

        }

    }

    void OnNewFolder();

    inline void OnDirOpen()
    {
        m_Path_Work =model->filePath(m_BrowseTree->currentIndex());
        m_DirLabel->setText(m_Path_Work);

        ExportOpenLoc();

        m_FileEditor->SetHomeDir(model->filePath(m_BrowseTree->currentIndex()));
        m_FileConsoleLineEdit->SetDir(model->filePath(m_BrowseTree->currentIndex()));

    }

    inline void OnOpen()
    {
        if(m_BrowseTree2->hasFocus())
        {
             QModelIndex index = m_BrowseTree2->currentIndex();
             QString file = model2->filePath(index);


        }

    }

    inline void OnOpenScript()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);
        if(m_FileCallBackSet)
        {
            bool used = m_FileCallBack(file, LISEM_FILE_TYPE_SCRIPT);
        }

    }

    inline void OnDisplay()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);
        if(m_FileCallBackSet)
        {
            bool used = m_FileCallBack(file, LISEM_FILE_TYPE_MAP);
        }


    }

    inline void OnOpenRunFile()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);
        if(m_FileCallBackSet)
        {
            bool used = m_FileCallBack(file, LISEM_FILE_TYPE_RUN);
        }
    }

    inline qint64 dirSize(QString dirPath) {
        qint64 size = 0;
        QDir dir(dirPath);
        //calculate total size of current directories' files
        QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
        for(QString filePath : dir.entryList(fileFilters)) {
            QFileInfo fi(dir, filePath);
            size+= fi.size();
        }
        //add size of child directories recursively
        QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
        for(QString childDirPath : dir.entryList(dirFilters))
            size+= dirSize(dirPath + QDir::separator() + childDirPath);
        return size;
    }

    inline QString formatSize(qint64 size) {
        QStringList units = {"Bytes", "KB", "MB", "GB", "TB", "PB"};
        int i;
        double outputSize = size;
        for(i=0; i<units.size()-1; i++) {
            if(outputSize<1024) break;
            outputSize= outputSize/1024;
        }
        return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
    }

    inline void OnInfo()
    {
        if(m_BrowseTree->hasFocus())
        {
            QModelIndex index = m_BrowseTree->currentIndex();
            QString file = model->filePath(index);


            QDir d(file);
            if(d.exists())
            {
                LISEMS_STATUS("Directory: " + file);
                LISEMS_STATUS("Directory file count: " + QString::number(d.count()));
                LISEMS_STATUS("Directory size: " + formatSize(dirSize(file)));

            }else
            {
                LISEMS_STATUS("Unsupported directory");
            }

        }else
        {
            QModelIndex index = m_BrowseTree2->currentIndex();
            QString file = model2->filePath(index);


            if(file.length() > 3)
            {
                QString ext = file.right(4);


                LISEMS_STATUS("File: " + file);
                LISEMS_STATUS("File size: " + formatSize(QFile(file).size()));

                if(IsMapFile(file))
                {
                    bool gdal_can_open = rasterCanBeOpenedForReading(file);
                    if(gdal_can_open)
                    {
                        LISEMS_STATUS("Map file format (GDAL)");
                        cTMapProps p = readRasterProps(file);
                        LISEMS_STATUS("sx:    " +QString::number(p.sizex));
                        LISEMS_STATUS("sy:    " +QString::number(p.sizey));
                        LISEMS_STATUS("csx:   " +QString::number(p.cellsizex));
                        LISEMS_STATUS("csy:   " +QString::number(p.cellsizey));
                        LISEMS_STATUS("ulx:   " +QString::number(p.ulx));
                        LISEMS_STATUS("uly:   " +QString::number(p.uly));
                        LISEMS_STATUS("bands: " +QString::number(p.bands));
                        LISEMS_STATUS("CRS:   " +GeoProjection::FromString(p.projection).GetName());
                    }else
                    {
                        LISEMS_STATUS("Map file format (unsupported!)");
                    }


                }else if(IsShapeFile(file))
                {
                    ShapeFile * s = LoadVector(file);
                    int lc = s->GetLayerCount();
                    int sc = s->GetShapeCount();
                    QString pn = s->GetProjection().GetName();
                    if(lc > 0)
                    {
                        LISEMS_STATUS("Vector file format (GDAL)");
                        LISEMS_STATUS("layers:    " +QString::number(lc));
                        LISEMS_STATUS("Features:  " +QString::number(sc));
                        LISEMS_STATUS("CRS:       " +(pn));
                    }else
                    {
                        LISEMS_STATUS("Vector file format (unsupported!)");
                    }

                    s->Destroy();
                    delete s;

                }else if(IsTableFile(file))
                {
                   MatrixTable * t = new MatrixTable();
                   bool loaded = t->LoadAsAutoFormat(file);

                   if(loaded > 0)
                   {
                       LISEMS_STATUS("Table file format");
                       LISEMS_STATUS("rows:    " +QString::number(t->GetNumberOfRows()));
                       LISEMS_STATUS("columns:  " +QString::number(t->GetNumberOfCols()));
                   }else
                   {
                       LISEMS_STATUS("Table file format (unsupported!)");
                   }
                   t->Empty();
                   delete t;

                }else
                {
                    LISEMS_STATUS("Unsupported file format");
                }
            }else
            {
                LISEMS_STATUS("Unsupported file format");
            }
        }
    }

    inline void OnScriptDone()
    {
        LISEMS_DEBUG("Done");
    }

    inline void OnScriptCallBackPrint(QString s)
    {

    }

    inline void OnConsoleCommand(QString command)
    {
        m_ConsoleMutex->lock();
        m_ConsoleRunning = true;
        m_FileConsoleLineEdit->AllowCommands(false);
        m_ConsoleMutex->unlock();

        m_FileConsole->appendPlainText(command);
        m_FileConsoleLineEdit->clear();

        SPHScript *s = new SPHScript();
        s->SetCode(command);
        s->SetSingleLine(true);
        s->SetPreProcess(true);
        s->SetHomeDir(m_Path_Work+"/");

        s->SetCallBackPrint(std::function<void(DatabaseTool *,SPHScript*,QString)>([](DatabaseTool *,SPHScript*,QString) ->
                                                                    void{


            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackDone(std::function<void(DatabaseTool *,SPHScript*,bool x)>([](DatabaseTool *dt,SPHScript*, bool finished) ->
                                                                    void{

            dt->m_ConsoleMutex->lock();
            dt->m_FileConsoleLineEdit->AllowCommands(true);
            dt->m_ConsoleRunning = true;
            dt->m_ConsoleMutex->unlock();
            LISEMS_DEBUG("Done");
            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackCompilerError(std::function<void(DatabaseTool *,SPHScript*,const asSMessageInfo *msg)>([](DatabaseTool *,SPHScript*,const asSMessageInfo *msg) ->
                                                                    void{
            LISEMS_ERROR("Error in script execution");
            const char *type = "ERR ";
            if( msg->type == asMSGTYPE_WARNING )
            {
                type = "WARN";
            }
            else if( msg->type == asMSGTYPE_INFORMATION )
            {
                type = "INFO";
            }
            LISEMS_ERROR(QString(msg->section) + " (" + QString(msg->row) + ", " + QString(msg->col) + ") : " + QString(type) + " : " + QString(msg->message));
            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackException(std::function<void(DatabaseTool *,SPHScript*,asIScriptContext *ctx)>([](DatabaseTool *,SPHScript*,asIScriptContext *ctx) ->
                                                                    void{

            LISEMS_ERROR("Exception encountered when running script");

            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackLine(std::function<void(DatabaseTool *,SPHScript*,asIScriptContext *ctx)>([](DatabaseTool *,SPHScript*,asIScriptContext *ctx) ->
                           void{


            ;
                           }),this,s,std::placeholders::_1);

        m_ScriptManager->CompileScript_Generic(s);
        if(s->IsCompiled())
        {
            m_ScriptManager->RunScript(s);
        }else
        {
            LISEMS_STATUS("Compilation Error");
            m_ConsoleMutex->lock();
            m_ConsoleRunning = true;
            m_FileConsoleLineEdit->AllowCommands(true);
            m_ConsoleMutex->unlock();
        }
    }

    inline QList<QString> GetUnsavedFileNames()
    {
        return this->m_FileEditor->GetUnsavedFileNames();
    }

    inline void SaveUnsavedFiles()
    {
        m_FileEditor->SaveUnsavedFiles();
    }

};

#endif // DATABASETOOL_H
