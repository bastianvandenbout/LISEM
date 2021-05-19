#include "model.h"

#include "codeeditor.h"

#include <QtWidgets>

#include <QCompleter>
#include "sphsyntaxhighlighter.h"
#include "site.h"

CodeEditor::CodeEditor(QWidget *parent, ScriptManager * sm) : QPlainTextEdit(parent)
{
    m_ScriptManager = sm;
    lineNumberArea = new LineNumberArea(this);

    m_FindReplaceDialog = new FindReplaceDialog(this);
    m_FindReplaceDialog->setModal(false);
    m_FindReplaceDialog->setTextEdit(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateExtraSelection()));
    connect(this, SIGNAL(textChanged()),this,SLOT(OnTextChanged()));


    setMouseTracking(true);

    //connect(this, &QTextEdit::mouseMoveEvent,this,&CodeEditor::onMouseMoveEvent);
    //connect(this,&QTextEdit::selectionChanged,this,&CodeEditor::onSelectionChanged);

    m_File = "";
    m_FileName = "<new>";
    m_HasBeenEditedSinceSave = false;


    updateLineNumberAreaWidth(0);

    QString m_FontDir;
    m_FontDir = GetSite()+"/assets/";

    //try to find custom font
    int id = QFontDatabase::addApplicationFont(m_FontDir + "DejaVuSansMono.ttf");
    if(id != -1)
    {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont FiraCode(family);
        FiraCode.setWeight(QFont::Normal);
        FiraCode.setPointSize(10);
        FiraCode.setStyleHint(QFont::Monospace);
        this->setFont(FiraCode);

        const int tabStop = 4;  // 4 characters

        QFontMetrics metrics(FiraCode);
        setTabStopWidth(tabStop * metrics.width(' '));

    }else
    {
        LISEM_DEBUG("Could not find font " + m_FontDir + "firacode/FiraCode-Regular.ttf");
    }

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    Highlighter * SPHHighlighter = new Highlighter(this->document());

    //QPalette p = palette(); // define pallete for textEdit..
    //p.setColor(QPalette::Base, QColor(250,250,250)); // set color "Red" for textedit base
    //setPalette(p); // change textedit palette

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

    StartDrawUpdateOnLine();


    emit OnEditedSinceSave();
}

void CodeEditor::mousePressEvent ( QMouseEvent * event )
{


    QPlainTextEdit::mousePressEvent(event);

}

void CodeEditor::updateExtraSelection()
{
    QList<QTextEdit::ExtraSelection> extra;

    highlightCurrentLine(extra);
    highlightErrorLine(extra);
    highlightParenthesis(extra);

    QList<QTextEdit::ExtraSelection>& extraSelection = extra;

    setToolTipAtCursor(textCursor(),extraSelection);

    setExtraSelections(extra);
}

void CodeEditor::setToolTipAtCursor(QTextCursor TextCursor, QList<QTextEdit::ExtraSelection>& extraSelection)
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
            if(pcurrent > -1)
            {
                tooltip += "arg. " +QString::number(pcurrent+1) + "/" + QString::number(plength) + " " + fi.Parameters.at(pcurrent) + " (" +typen + ")\n";
            }else
            {
                tooltip += "arg. 0/0";
            }

            if(pcurrent > -1)
            {
                tooltip += fi.ParameterDescription.at(pcurrent);
            }


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

                    if(fi.Parameters.length() > i)
                    {
                        tooltip += m_ScriptManager->m_Engine->GetTypeName(typeId) + " " + fi.Parameters.at(i);
                    }

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


}

void CodeEditor::onSelectionChanged()
{
    auto selected = textCursor().selectedText();

    auto cursor = textCursor();

    // Cursor is null if setPlainText was called.
    if (cursor.isNull())
    {
        return;
    }

    /*cursor.movePosition(QTextCursor::MoveOperation::Left);
    cursor.select(QTextCursor::SelectionType::WordUnderCursor);

    QSignalBlocker blocker(this);
    m_framedAttribute->clear(cursor);

    if (selected.size() > 1 &&
        cursor.selectedText() == selected)
    {
        auto backup = textCursor();

        // Perform search selecting
        handleSelectionQuery(cursor);

        setTextCursor(backup);
    }*/
}

int CodeEditor::getIndentationSpaces()
{
    auto blockText = textCursor().block().text();

    int indentationLevel = 0;

    for (auto i = 0;
         i < blockText.size() && QString("\t ").contains(blockText[i]);
         ++i)
    {
        if (blockText[i] == ' ')
        {
            indentationLevel++;
        }
        else
        {
#if QT_VERSION >= 0x050A00
            indentationLevel += tabStopDistance() / fontMetrics().averageCharWidth();
#else
            indentationLevel += tabStopWidth() / fontMetrics().averageCharWidth();
#endif
        }
    }

    return indentationLevel;
}

QChar CodeEditor::CharUnderCursor(QTextCursor tc, int offset) const
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


QChar CodeEditor::CharUnderCursor(int offset) const
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

QString CodeEditor::wordUnderCursor(QTextCursor tc) const
{
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}


QString CodeEditor::wordUnderCursor() const
{
    auto tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{

    bool m_replaceTab = true;
    bool m_autoIndentation = true;
    bool m_autoParentheses = true;
    QString m_tabReplace;
    m_tabReplace.clear();
    m_tabReplace.fill(' ', 4);

    #if QT_VERSION >= 0x050A00
      const int defaultIndent = tabStopDistance() / fontMetrics().averageCharWidth();
    #else
      const int defaultIndent = tabStopWidth() / fontMetrics().averageCharWidth();
    #endif



    if (m_Completer && m_Completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }


    if(QToolTip::isVisible())
    {

       if(e->key() == Qt::Key_PageUp)
       {
           m_CurrentToolTipIndex += 1;
           updateExtraSelection();
           return;
       }
       if(e->key() == Qt::Key_PageDown)
       {
           m_CurrentToolTipIndex -= 1;
           m_CurrentToolTipIndex = std::max(m_CurrentToolTipIndex,0);
           updateExtraSelection();
           return;
       }
    }else {

        m_CurrentToolTipIndex = 0;

    }



    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    bool isShortcutSearch = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_F); // CTRL+E
    bool isShortcutSave = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_S); // CTRL+E


    if(isShortcutSearch)
    {
        m_FindReplaceDialog->show();
    }
    if(isShortcutSave)
    {
        SaveFile();
    }


    if(!m_Completer || !isShortcut)
    {
        // do not process the shortcut when we have a completer



        int selectedLines = 0; //<--- this is it
        {
            QTextCursor cursor = textCursor();

            if(!cursor.selection().isEmpty())
            {
                QString str = cursor.selection().toPlainText();
                selectedLines = str.count("\n")+1;
            }
        }


        int indentationLevel = getIndentationSpaces();

        std::cout << "selected lines " << std::endl;
        if(selectedLines > 1)
        {
            // Shortcut for moving line to left
            if (m_replaceTab && e->key() == Qt::Key_Tab) {
              indentationLevel = std::min(indentationLevel, m_tabReplace.size());


              QTextCursor cursor = textCursor();
              cursor.position();
              QString str = cursor.selection().toPlainText();
              str.replace("\n","\n"+ m_tabReplace);
              str.prepend(m_tabReplace);
              cursor.insertText(str);
              cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
              setTextCursor(cursor);

              return;
            }


            // Shortcut for moving line to left
            if (m_replaceTab && e->key() == Qt::Key_Backtab) {
              indentationLevel = std::min(indentationLevel, m_tabReplace.size());

              QTextCursor cursor = textCursor();
              QString str = cursor.selection().toPlainText();
              str.replace("\n"+ m_tabReplace,"\n");
              if(str.startsWith(m_tabReplace))
              {
                  str = str.mid(m_tabReplace.length(),str.length() - m_tabReplace.length());
              }
              cursor.removeSelectedText();
              cursor.insertText(str);
              cursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor,str.length());
              setTextCursor(cursor);
              return;
            }
        }


        if (m_replaceTab && e->key() == Qt::Key_Tab &&
            e->modifiers() == Qt::NoModifier) {
          insertPlainText(m_tabReplace);
          return;
        }

        #if QT_VERSION >= 0x050A00
            int tabCounts =
                indentationLevel * fontMetrics().averageCharWidth() / tabStopDistance();
        #else
            int tabCounts =
                indentationLevel * fontMetrics().averageCharWidth() / tabStopWidth();
        #endif




        if (m_autoIndentation &&
               (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) &&
                CharUnderCursor() == '}' && CharUnderCursor(-1) == '{')
            {
              int charsBack = 0;
              insertPlainText("\n");

              if (m_replaceTab)
                insertPlainText(QString(indentationLevel + defaultIndent, ' '));
              else
                insertPlainText(QString(tabCounts + 1, '\t'));

              insertPlainText("\n");
              charsBack++;

              if (m_replaceTab)
              {
                insertPlainText(QString(indentationLevel, ' '));
                charsBack += indentationLevel;
              }
              else
              {
                insertPlainText(QString(tabCounts, '\t'));
                charsBack += tabCounts;
              }

              while (charsBack--)
                moveCursor(QTextCursor::MoveOperation::Left);
              return;
            }

            // Shortcut for moving line to left
            if (m_replaceTab && e->key() == Qt::Key_Backtab) {
              indentationLevel = std::min(indentationLevel, m_tabReplace.size());

              auto cursor = textCursor();

              cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
              cursor.movePosition(QTextCursor::MoveOperation::Right,
                                  QTextCursor::MoveMode::KeepAnchor, indentationLevel);

              cursor.removeSelectedText();
              return;
            }


             QPlainTextEdit::keyPressEvent(e);

            if (m_autoIndentation && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)) {
                  if (m_replaceTab)
                    insertPlainText(QString(indentationLevel, ' '));
                  else
                    insertPlainText(QString(tabCounts, '\t'));
                }


            if (m_autoParentheses)
            {
              for (auto&& el : parentheses)
              {
                        // Inserting closed brace
                        if (el.first == e->text())
                        {
                          insertPlainText(el.second);
                          moveCursor(QTextCursor::MoveOperation::Left);
                          break;
                        }

                        // If it's close brace - check parentheses
                        if (el.second == e->text())
                        {
                            auto symbol = CharUnderCursor();

                            if (symbol == el.second)
                            {
                                textCursor().deletePreviousChar();
                                moveCursor(QTextCursor::MoveOperation::Right);
                            }

                            break;
                        }
                    }
              }



    }



    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_Completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word (without . since we use this in variable names when directly calculating with files)
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    QString fulltext = this->document()->toPlainText();
    int cursorpos = textCursor().position();
    bool ismember = false;
    if(fulltext[cursorpos-completionPrefix.length()] == ".")
    {
        ismember = true;

    }else
    {
        ismember = false;
    }

    std::cout << "test " << completionPrefix.toStdString() << std::endl;

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| (completionPrefix.length() < 3 && !ismember)
                      || eow.contains(e->text().right(1)))) {

        m_Completer->popup()->hide();
        return;
    }

    bool has = false;
    if(ismember)
    {
         has = m_Completer->update(completionPrefix,"member",fulltext,cursorpos);
    }else
    {
        has = m_Completer->update(completionPrefix,"",fulltext,cursorpos);
    }

    std::cout << "has " << has << std::endl;

    //if (completionPrefix != m_Completer->completionPrefix()) {

    if(has)
    {
        m_Completer->popup()->setCurrentIndex(m_Completer->completionModel()->index(0, 0));

        std::cout << 5 << std::endl;

        QRect cr = cursorRect();
        cr.setWidth(m_Completer->popup()->sizeHintForColumn(0)
                    + m_Completer->popup()->verticalScrollBar()->sizeHint().width());



        std::cout << 5 << std::endl;
        m_Completer->complete(cr); // popup it up!

        std::cout << 5 << std::endl;
        m_Completer->ShowPopup0();

        std::cout << 5 << std::endl;

    }
    //}


}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (m_Completer)
        m_Completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}
void CodeEditor::setCompleter(SPHScriptCompleter *completer)
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
                     this, &CodeEditor::insertCompletion);
}

QCompleter *CodeEditor::completer() const
{
    return m_Completer;
}
void CodeEditor::insertCompletion(const QString& completion)
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

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 +  fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightErrorLine(QList<QTextEdit::ExtraSelection>& extraSelection)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(m_ErrorLine > -1)
     {
        //if (!isReadOnly())
        {
            QTextEdit::ExtraSelection selection;

            QColor lineColor = QColor(Qt::red).lighter(160);

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.setPosition(this->document()->findBlockByLineNumber(m_ErrorLine -1).position());
            selection.cursor.clearSelection();
            selection.cursor.select(QTextCursor::LineUnderCursor);
            extraSelection.append(selection);

        }
    }


}


void CodeEditor::highlightCurrentLine(QList<QTextEdit::ExtraSelection>& extraSelection)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    //if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selection.cursor.select(QTextCursor::LineUnderCursor);
        extraSelection.append(selection);

    }

}

void CodeEditor::highlightParenthesis(QList<QTextEdit::ExtraSelection>& extraSelection)
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



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    int lawidth = lineNumberAreaWidth();

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();


    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);


            bool thislinecurrent = false;
            bool thislinebreak = false;

            m_BreakPointMutex.lock();
            m_CurrentRunLineMutex.lock();

            if(m_BreakPoints.contains(blockNumber-1))
            {
                thislinebreak = true;
            }

            if((blockNumber + 1) == m_CurrentRunLine)
            {
                thislinecurrent = true;
            }


            m_CurrentRunLineMutex.unlock();
            m_BreakPointMutex.unlock();

            if(thislinebreak)
            {
                painter.setBrush(Qt::red);
                painter.drawEllipse(lawidth/2, top ,lawidth,fontMetrics().height());
            }

            if(thislinecurrent)
            {
                 painter.setBrush(Qt::yellow);
                 painter.drawEllipse(lawidth/2, top ,lawidth,fontMetrics().height());
            }
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);


        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
