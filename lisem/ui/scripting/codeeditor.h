
#ifndef CODEEDITOR_H
#define CODEEDITOR_H


#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif


#include <glad/glad.h>

#include <QPlainTextEdit>
#include <QObject>
#include <QFileInfo>
#include "lsmio.h"
#include <QDir>
#include "QCoreApplication"
#include "QFileDialog"
#include "scriptmanager.h"
#include "scriptcompleter.h"
#include "QMutex"
#include "QTimer"
#include "widgets/findreplacedialog.h"
#include "QMouseEvent"
#include "QTextStream"
#include "site.h"
#include "QStatusBar"
#include "QTimer"
#include "scriptsearchwindow.h"
#include "QMimeData"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

QT_BEGIN_NAMESPACE
class QCompleter;
QT_END_NAMESPACE

static QVector<QPair<QString, QString>> parentheses = {
    {"(", ")"},
    {"{", "}"},
    {"[", "]"},
    {"\"", "\""},
    {"'", "'"}
};

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:

    FindReplaceDialog * m_FindReplaceDialog;

    QMutex m_CurrentRunLineMutex;
    int m_CurrentRunLine = -1;

    int m_CurrentToolTipIndex = 0;

    QString m_File;
    QString m_FileName;
    bool m_HasBeenEditedSinceSave;

    QList<int> m_BreakPoints;
    QMutex m_BreakPointMutex;

    SPHScriptCompleter * m_Completer = nullptr;

    QList<ScriptFunctionInfo> funclist;
    QList<ScriptFunctionInfo> funcmemberlist;

    QString m_HomeDir;

    ScriptSearchWindow * m_Bar = nullptr;

    CodeEditor(QWidget *parent = 0, ScriptManager* sm = 0, ScriptSearchWindow * b = nullptr);

    void setCompleter(SPHScriptCompleter *m_Completer);
    QCompleter *completer() const;

    ScriptManager * m_ScriptManager;

    float m_CurrentZoom = 12;

    QString m_OverlayMessage = "";
    bool m_OverlayActive = false;
    bool m_OverlayFirst = true;
    float m_OverlayTime = 0.0;
    float m_OverlayTimep = 0.0;
    float m_OverlayAlpha = 1.0;
    QTimer m_OverlayTimer;
    bool m_OverlayDeactivated = false;



    bool m_RunCallBackSet = false;
    std::function<void(void)> m_RunCallBack;

    inline void setRunCallBack(std::function<void(void)> f)
    {
        m_RunCallBackSet = true;
        m_RunCallBack = f;
    }



    bool m_StopCallBackSet = false;
    std::function<void(void)> m_StopCallBack;

    inline void setStopCallBack(std::function<void(void)> f)
    {
        m_StopCallBackSet = true;
        m_StopCallBack = f;
    }

    bool m_CompileCallBackSet = false;
    std::function<void(void)> m_CompileCallBack;

    inline void setCompileCallBack(std::function<void(void)> f)
    {
        m_CompileCallBackSet = true;
        m_CompileCallBack = f;
    }





    bool m_ToolCallBackSet = false;
    std::function<void(QString)> m_ToolCallBack;

    inline void setToolCallBack(std::function<void(QString)> f)
    {
        m_ToolCallBackSet = true;
        m_ToolCallBack = f;
    }


    bool m_OpenCallBackSet = false;
    std::function<void(QString)> m_OpenCallBack;

    inline void setOpenCallBack(std::function<void(QString)> f)
    {
        m_OpenCallBackSet = true;
        m_OpenCallBack = f;
    }

    inline void ShowOverlayMessage(QString m, float time)
    {
        m_OverlayMessage = m;
        m_OverlayTime = time;
        m_OverlayTimep = 0.0;
        m_OverlayActive = true;
        m_OverlayFirst = true;
        m_OverlayDeactivated = false;
        m_OverlayAlpha = 1.0;
        m_OverlayTimer.stop();
        m_OverlayTimer.setSingleShot(true);
        m_OverlayTimer.setInterval(time * 1000.0);
        viewport()->update();

        QObject::connect(&m_OverlayTimer, &QTimer::timeout,[this]()
        {
            std::cout << "timer timeout 1 " << std::endl;
            this->UpdateOverlayMessage();
        });
        m_OverlayTimer.start();

    }
    inline void UpdateOverlayMessage()
    {
        if(m_OverlayDeactivated == false)
        {


        if(m_OverlayFirst)
        {
            m_OverlayTimep += m_OverlayTime;
            m_OverlayFirst = false;
        }else
        {
            m_OverlayTimep += 0.033;
        }

        if(m_OverlayTimep >= m_OverlayTime + 1.0)
        {
            std::cout << " overlay end "  << std::endl;

            m_OverlayDeactivated = true;
            m_OverlayActive = false;
            m_OverlayTimer.stop();
            m_OverlayTime = 0.0;
            m_OverlayTimep = 0.0;
            m_OverlayFirst = true;
        }else
        {
            m_OverlayTimer.setSingleShot(true);
            m_OverlayTimer.setInterval(33);
            std::cout << "set timer again " << m_OverlayTimep <<  m_OverlayTime + 1.0 << std::endl;
            QObject::connect(&m_OverlayTimer, &QTimer::timeout,[this]()
            {
                std::cout << "timer timeout" << std::endl;
                this->UpdateOverlayMessage();
            });
            m_OverlayTimer.start();

        }
        }
        viewport()->update();

    }

    inline QString GetFileDir()
    {
        return QFileInfo(m_File).absoluteDir().path();
    }

    int getIndentationSpaces();
    QChar CharUnderCursor(QTextCursor tc, int offset = 0) const;
    QChar CharUnderCursor(int offset = 0) const;
    QString wordUnderCursor() const;
    QString wordUnderCursor(QTextCursor tc) const;
protected:
    void wheelEvent(QWheelEvent * event) override;
    void mousePressEvent ( QMouseEvent * event )  override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void paintEvent(QPaintEvent *event) override {
        QPlainTextEdit::paintEvent(event);
        this->overlayPaintEvent(event);
    }
public:
    QString textUnderCursor() const
    {
        QTextCursor tc = textCursor();
        tc.select(QTextCursor::WordUnderCursor);

        QString word = tc.selectedText();
        int selstart =  tc.anchor();
        tc.setPosition(selstart-1);
        tc.setPosition(selstart, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();

        tc.setPosition(selstart-1);
        tc.select(QTextCursor::WordUnderCursor);
        QString wordprev2 = tc.selectedText();
        if(word == ")")
        {
            word = wordprev2;
        }

        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }

        return word;
    }

    QString textUnderCursorLeft() const
    {
        QTextCursor tc = textCursor();
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);

        tc.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
        QString word = tc.selectedText();
        int selstart =  tc.anchor();
        tc.setPosition(selstart-1);
        tc.setPosition(selstart, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();

        tc.setPosition(selstart-1);
        tc.select(QTextCursor::WordUnderCursor);
        QString wordprev2 = tc.selectedText();
        if(word == ")")
        {
            word = wordprev2;
        }

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

    QString textUnderCursorLeft(QTextCursor tc) const
    {
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);

        tc.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
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

    QString textUnderCursorRight(QTextCursor tc) const
    {
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);
        int selstart =  tc.position();

        tc.setPosition(pos, QTextCursor::MoveMode::MoveAnchor);
        tc.setPosition(selstart, QTextCursor::MoveMode::KeepAnchor);
        QString word = tc.selectedText();

        return word;
    }

    QString textRightRightUnderCursor(QTextCursor tc) const
    {
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);
        int selstart = tc.position();
        tc.setPosition(selstart + 1);
        tc.select(QTextCursor::WordUnderCursor);
        selstart = tc.position();
        tc.setPosition(selstart + 1);
        tc.select(QTextCursor::WordUnderCursor);
        QString word = tc.selectedText();
        int selstart2 =  tc.anchor();
        tc.setPosition(selstart2-1);
        tc.setPosition(selstart2, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();
        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }
        return word;
    }

    QString textRightUnderCursor(QTextCursor tc) const
    {
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);
        int selstart = tc.position();
        tc.setPosition(selstart + 1);
        tc.select(QTextCursor::WordUnderCursor);
        QString word = tc.selectedText();
        int selstart2 =  tc.anchor();
        tc.setPosition(selstart2-1);
        tc.setPosition(selstart2, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();
        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }
        return word;
    }
    QString textLeftUnderCursor(QTextCursor tc) const
    {
        int pos = tc.anchor();
        tc.select(QTextCursor::WordUnderCursor);
        int selstart = tc.anchor();
        tc.setPosition(selstart - 2);
        tc.select(QTextCursor::WordUnderCursor);
        QString word = tc.selectedText();
        int selstart2 =  tc.anchor();
        tc.setPosition(selstart2-1);
        tc.setPosition(selstart2, QTextCursor::MoveMode::KeepAnchor);

        QString wordprev = tc.selectedText();
        if(wordprev.trimmed() == ".")
        {
            word.insert(0,",");
        }
        return word;


    }

    inline void insertFromMimeData(const QMimeData *source) override
    {
        if(source->hasUrls())
        {
            QList<QUrl> urls = source->urls();
            for(int i = 0; i < urls.size(); i++)
            {
                if(urls.at(i).isLocalFile())
                {
                    QString file = urls.at(i).toLocalFile();

                    if(m_OpenCallBackSet)
                    {
                        std::cout <<"open file from mime " << file.toStdString() << std::endl;
                        m_OpenCallBack(file);
                    }


                }
            }


        }else if(source->hasImage())
        {
            //should not have been accepted anyway

        }else
        {
            QPlainTextEdit::insertFromMimeData(source);

        }

    }

public:

    inline bool HasBreakPointAt(int line_number)
    {
        m_BreakPointMutex.lock();
        bool ret = m_BreakPoints.contains(line_number-2);
        m_BreakPointMutex.unlock();
        return ret;
    }

    inline void StartDrawUpdateOnLine()
    {
        QTimer::singleShot(20,this,&CodeEditor::UpdateLineDraw);
    }



    inline void SetHomeDir(QString dir)
    {
        m_HomeDir = dir;
        m_Completer->SetDirectory(dir);

    }


    inline void SetEmpty()
    {
        this->document()->setPlainText("void main()\n{\n    \n    \n    \n}\n");

        m_File = "";
        m_FileName = "<new>";

        m_HasBeenEditedSinceSave = false;

        emit OnEditedSinceSave();
    }

    inline void SetFile(QString file)
    {
        m_File = file;
        m_HasBeenEditedSinceSave = false;


        QFileInfo fileInfo(file);
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        m_FileName = filename;

        int ok = false;
        QStringList filetext = ReadFileAsText(file,false,&ok);

        QString plaintext;
        for(int i = 0; i < filetext.length(); i++)
        {
            QString temp = filetext.at(i);
            temp.replace(QString::fromUtf8("\u201D"),"\"");
            temp.replace(QString::fromUtf8("\u202D"),"\"");
            temp.replace(QString::fromUtf8("\u2003")," ");
            temp.replace(QString::fromUtf8("\u2002")," ");
            temp.replace(QString::fromUtf8("\u2001")," ");

            plaintext.append(temp);
        }

        this->document()->setPlainText(plaintext);

        m_HasBeenEditedSinceSave = false;

        emit OnEditedSinceSave();
    }
    inline void LoadFileDirect(QString path)
    {
        QFileInfo fileInfo(path);
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        SetFile(path);
    }
    inline void LoadFile(QString startdir)
    {
        QString openDir;
        if(startdir.length() == 0)
        {
           openDir = GetFIODir(LISEM_DIR_FIO_GENERAL);

        }

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Select Script File"),
                                            openDir,
                                            QString("*") + ".script" + QString(";*.*"));


        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GENERAL,QFileInfo(path).dir().absolutePath());
        }
        QFileInfo fileInfo(path);
        QString filename(fileInfo.fileName());
        QString filedir(fileInfo.dir().path());

        SetFile(path);


    }


    std::vector<int> m_ErrorLine;
    std::vector<int> m_ErrorCharacter;
    bool m_ErrorLineChanged = false;
    std::vector<int> m_HighlightLine;
    std::vector<int> m_HighlightCharacter;
    std::vector<int> m_HighlightLength;
    bool m_HighlightLineChanged = false;

    inline std::vector<int> GetHighlightSearchLines()
    {

        return m_HighlightLine;
    }

    inline void SetHighlightSearch(std::vector<int> lines, std::vector<int> positions, std::vector<int> lengths)
    {
        m_HighlightLine = lines;
        m_HighlightCharacter = positions;
        m_HighlightLength = lengths;
    }
    inline std::vector<int> GetHighlightErrorLines()
    {
        m_CurrentRunLineMutex.lock();
        std::vector<int> ret =  m_ErrorLine;
        m_CurrentRunLineMutex.unlock();
        return ret;
    }


    inline void SetHighlightErrorLocation(std::vector<int> line, std::vector<int> character)
    {

        m_CurrentRunLineMutex.lock();
        m_ErrorLine = line;
        m_ErrorLineChanged = true;
        m_ErrorCharacter = character;
        m_CurrentRunLineMutex.unlock();

    }

    inline void SetHighlightCurrentRunLine(int line)
    {
        m_CurrentRunLineMutex.lock();
        m_CurrentRunLine = line;
        m_CurrentRunLineMutex.unlock();

    }

    inline void SaveFileAs()
    {
         SaveFile(true);
    }

    inline void SaveFile(bool askname = false)
    {
        if(m_File.length() == 0)
        {
            askname = true;
        }
        QString fileName;
        if(askname)
        {
            QString selectedFilter;
            fileName = QFileDialog::getSaveFileName(this,
                                                            QString("Give a new script file name"),
                                                            GetFIODir(LISEM_DIR_FIO_CODE),
                                                            QString("LISEM Script Files (*") + ".script" + QString(");;All Files (*)"),
                                                            &selectedFilter);



            if (fileName.isEmpty())
            {

                return;
            }else
            {
                SetFIODir(LISEM_DIR_FIO_CODE,QFileInfo(m_File).dir().absolutePath());
                m_File = fileName;
                this->m_FileName = QFileInfo(m_File).fileName();
            }
        }else
        {
            fileName = m_File;
        }

        QFile fp(fileName);
        if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LISEM_DEBUG("Could not write file to disk: " + fileName);
            return;
        }

        QTextStream out(&fp);
        QString filetext =this->document()->toPlainText();
        filetext.replace(QString::fromUtf8("\u201D"),"\"");
        filetext.replace(QString::fromUtf8("\u202D"),"\"");
        filetext.replace(QString::fromUtf8("\u2003")," ");
        filetext.replace(QString::fromUtf8("\u2002")," ");
        filetext.replace(QString::fromUtf8("\u2001")," ");
        out << filetext;

        fp.close();
        m_HasBeenEditedSinceSave = false;
        emit OnEditedSinceSave();
    }


    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void overlayPaintEvent(QPaintEvent *event);

    int lineNumberAreaWidth();
    void setToolTipAtCursor(QTextCursor TextCursor, QList<QTextEdit::ExtraSelection>& extraSelection);

signals:
    void OnEditedSinceSave();

protected:
    void resizeEvent(QResizeEvent *event) override;

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

public slots:

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightErrorLine(QList<QTextEdit::ExtraSelection>& extraSelection);
    void highlightCurrentLine(QList<QTextEdit::ExtraSelection>& extraSelection);
    void highlightParenthesis(QList<QTextEdit::ExtraSelection>& extraSelection);
    void updateLineNumberArea(const QRect &, int);

    inline void OnTextChanged()
    {
        if(!m_HasBeenEditedSinceSave)
        {
            m_HasBeenEditedSinceSave = true;
            emit OnEditedSinceSave();
        }
    }

    void insertCompletion(const QString& completion);
public slots:
    void updateExtraSelection();
    void onSelectionChanged();

    inline void UpdateLineDraw()
    {
        m_CurrentRunLineMutex.lock();
        bool dot = false;
        if((m_CurrentRunLine > -1) || (m_ErrorLineChanged == true))
        {
            dot = true;
        }
        m_ErrorLineChanged = false;

        m_CurrentRunLineMutex.unlock();
        if(dot)
        {
            QTimer::singleShot(20,this,&CodeEditor::UpdateLineDraw);
        }
        this->lineNumberArea->repaint();

    }

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }


    inline void mousePressEvent ( QMouseEvent * event )  override
    {

        if((event->button() == Qt::MouseButton::LeftButton) && (event->pos().x() < codeEditor->lineNumberAreaWidth()))
        {
            int line_number = codeEditor->cursorForPosition(event->pos()).blockNumber();

            codeEditor->m_BreakPointMutex.lock();
            bool ret = codeEditor->m_BreakPoints.contains(line_number-1);

            if(ret == false)
            {
                //std::cout << "add breakpoint " << line_number-1 << std::endl;
                codeEditor->m_BreakPoints.append(line_number-1);
            }else
            {
                codeEditor->m_BreakPoints.removeAll(line_number-1);
            }
            codeEditor->m_BreakPointMutex.unlock();

        }
        this->repaint();
    }


private:
    CodeEditor *codeEditor;
};


#endif
