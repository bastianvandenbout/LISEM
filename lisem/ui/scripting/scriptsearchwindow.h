#ifndef SCRIPTSEARCHWINDOW_H
#define SCRIPTSEARCHWINDOW_H

#include "qtablewidget.h"
#include "widgets/tablemodel.h"
#include "qsortfilterproxymodel.h"
#include "QLineEdit"
#include "QLabel"
#include "QToolButton"
#include "QTableView"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "resourcemanager.h"
#include "QHeaderView"

class CodeEditor;

class ScriptSearchWindow : public QWidget
{
    Q_OBJECT

    MatrixTable * mt;
    TableModel *model;
    QSortFilterProxyModel * proxyModel;
    QTableView * m_Table;

    QToolButton * m_ButtonNext;
    QToolButton * m_ButtonPrev;
    QLabel * m_LabelCount;
    QLineEdit * m_SearchEdit;
    QLineEdit * m_ReplaceEdit;
    QToolButton * m_Replace;
    QToolButton * m_Replace1;
    QToolButton * m_Search;
    QLabel * m_Label;
    QToolButton * m_Close;

    CodeEditor * m_Link = nullptr;
    QString * m_CurrentName;
    int m_CurrentPos = -1;

    std::vector<int> m_SR_Position;
    std::vector<int> m_SR_Line;
    std::vector<int> m_SR_Col;
    std::vector<int> m_SR_Length;

    QString m_Text;
    bool m_CanReplaceAll = false;

public:
    ScriptSearchWindow();

    void Clear();
    void OpenAsReferencesTo(CodeEditor* c, QString name, int pos);
    void OpenAsSearch(CodeEditor* c, QString name, int pos);
    void OpenAsReplace(CodeEditor* c, QString name, int pos);
    void OpenAsReplaceFile(CodeEditor* c, QString name, int pos);
    void Link(CodeEditor*);
    CodeEditor * GetLink();
    void UnLink();
    void Open();
    void Close();
    void SetTitle(QString title);
    void SetData(MatrixTable * T);
    void FindNext();
    void FindPrevious();
    void LinkCurrent();

    bool m_HasCallBackEditorOpen = false;
    std::function<void(CodeEditor*)> m_funceditoropen; //arguments are item row, item column, text in line, type of click
    bool m_HasCallBackEditorCurrent = false;
    std::function<CodeEditor*(void)> m_funceditorcurrent; //arguments are item row, item column, text in line, type of click

    inline void SetCallBackEditorOpen(std::function<void(CodeEditor*)> f)
    {

        m_HasCallBackOpen = true;
        m_funceditoropen = f;
    }

    inline void SetCallBackEditorCurrent(std::function<CodeEditor*(void)> f)
    {
        m_HasCallBackEditorCurrent = true;
        m_funceditorcurrent = f;
    }

    bool m_HasCallBackClose = false;
    std::function<void(void)> m_funcclose; //arguments are item row, item column, text in line, type of click
    bool m_HasCallBackOpen = false;
    std::function<void(void)> m_funcopen; //arguments are item row, item column, text in line, type of click

    inline void DisableReplaceAll()
    {
        m_CanReplaceAll = false;

        m_Replace->setEnabled(false);
    }
    inline void EnableReplaceAll()
    {
        m_CanReplaceAll = true;
        m_Replace->setEnabled(true);
    }
    inline void SetCallBackOpen(std::function<void(void)> f)
    {
        m_HasCallBackOpen = true;
        m_funcopen = f;

    }
    inline void SetCallBackClose(std::function<void(void)> f)
    {
        m_HasCallBackClose = true;
        m_funcclose= f;

    }
    bool m_HasCallBackClicked = false;
    std::function<void(int,int,QString,int)> m_func; //arguments are item row, item column, text in line, type of click

    inline void SetCallBackItemClicked(std::function<void(int,int,QString,int)> func)
    {
        m_HasCallBackClicked = true;
        m_func = func;
    }


public slots:

    void OnSearchTextChanged(QString s);
    void OnReplaceTextChanged(QString s);


    inline void onTableClicked(const QModelIndex &index)
    {
        if(m_HasCallBackEditorOpen)
        {
            m_funceditoropen(m_Link);
        }

        if (index.isValid()) {
            QString cellText = index.data().toString();
            std::cout << "table clicked: " << cellText.toStdString() << std::endl;

            if(m_HasCallBackClicked)
            {
                m_func(index.row(),index.column(),cellText,1);
            }
        }
    }
    inline void onTableDClicked(const QModelIndex &index)
    {
        if(m_HasCallBackEditorOpen)
        {
            m_funceditoropen(m_Link);
        }
        if (index.isValid()) {
            QString cellText = index.data().toString();
            std::cout << "table dclicked: " << cellText.toStdString() << std::endl;

            if(m_HasCallBackClicked)
            {
                m_func(index.row(),index.column(),cellText,2);
            }
        }
    }


    void onLinkDestroyed();

};




#endif // SCRIPTSEARCHWINDOW_H
