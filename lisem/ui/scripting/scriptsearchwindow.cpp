#include "scriptsearchwindow.h"
#include "scriptanalyzer.h"
#include "codeeditor.h"
#include "QTextBlock"

ScriptSearchWindow::ScriptSearchWindow() :QWidget()
{

    mt = new MatrixTable();
    model = new TableModel(mt);
    m_Table = new QTableView();

    proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel( model );
    m_Table->setModel( proxyModel );

    m_Table->horizontalHeader()->setSectionsMovable(true);
    m_Table->verticalHeader()->setSectionsMovable(true);

    m_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_Label = new QLabel();
    m_ButtonNext = new QToolButton();
    m_ButtonPrev = new QToolButton();
    m_LabelCount = new QLabel();
    m_SearchEdit = new QLineEdit();
    m_ReplaceEdit = new QLineEdit();
    m_Replace = new QToolButton();
    m_Replace1= new QToolButton();
    m_Search = new QToolButton();
    m_Close = new QToolButton();

    m_Label->setText("no search");

    m_Close->setToolTip("Close search widget");
    m_Search->setToolTip("Search currently opened document for entered text");
    m_Replace->setToolTip("Find and replace all instances of the search text");
    m_Replace1->setToolTip("Find and replace one instance of the search text");
    m_ButtonNext->setToolTip("Move text cursor to next search result");
    m_ButtonPrev->setToolTip("Move text cursor to previous search result");

    m_ButtonNext->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_NEXT)));
    m_ButtonPrev->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_PREVIOUS)));
    m_Search->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_SEARCH)));
    m_Replace->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_REPLACE)));
    m_Replace1->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_REPLACE1)));

    m_Close->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_CANCEL)));

    //create widgets and set empty data

    QWidget * bar = new QWidget();
    QHBoxLayout * barl = new QHBoxLayout();

    barl->setMargin(2);
    barl->setSpacing(2);
    barl->setMargin(2);
    barl->setSpacing(2);

    barl->addWidget(m_Label);
    barl->addWidget(m_SearchEdit);
    barl->addWidget(m_Search);
    barl->addWidget(m_ButtonPrev);
    barl->addWidget(m_ButtonNext);
    barl->addWidget(m_LabelCount);
    barl->addWidget(m_ReplaceEdit);
    barl->addWidget(m_Replace1);
    barl->addWidget(m_Replace);
    barl->addWidget(m_Close);
    bar->setLayout(barl);

    QVBoxLayout *lv = new QVBoxLayout();
    this->setLayout(lv);
    lv->setMargin(2);
    lv->setSpacing(2);
    lv->setMargin(2);
    lv->setSpacing(2);
    lv->addWidget(bar);
    lv->addWidget(m_Table);

    connect(m_Table, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
    connect(m_Table, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onTableDClicked(const QModelIndex &)));

    connect(m_SearchEdit, &QLineEdit::textChanged,this,&ScriptSearchWindow::OnSearchTextChanged);
    connect(m_ReplaceEdit, &QLineEdit::textChanged,this,&ScriptSearchWindow::OnReplaceTextChanged);

    connect(m_ButtonPrev,&QToolButton::clicked,[this](bool x)
    {
        this->FindPrevious();
    });

    connect(m_ButtonNext,&QToolButton::clicked,[this](bool x)
    {
        this->FindNext();

       ;
    });

    connect(m_Replace,&QToolButton::clicked,[this](bool x)
    {
        //sort all the item based on position

        if(m_Link == nullptr)
        {
            LinkCurrent();
            return;
        }
        QString replace = m_ReplaceEdit->text();
        std::vector<int> indices;
        for(int i = 0; i < m_SR_Position.size(); i++)
        {
            indices.push_back(i);
        }

        std::sort(indices.begin(),indices.end(),[this](const int &a, const int &b)
        {
            return m_SR_Position.at(a) > m_SR_Position.at(b);

        });



        int offset = 0;
        //replace each item
        for(int i = 0; i < m_SR_Position.size(); i++)
        {
            int index =indices.at(i);

            QTextCursor t = m_Link->textCursor();

            t.setPosition(m_SR_Position.at(index) + offset);
            t.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_SR_Length.at(index));

            //update added shift in location

            t.removeSelectedText();
            t.insertText(replace);
            offset += replace.size() - m_SR_Length.at(index);
        }

        m_Link->ShowOverlayMessage("Replaced " + QString::number(m_SR_Position.size()) + " occurences",2.0);

        m_Link->update();
        if(m_HasCallBackEditorOpen)
        {
            m_funceditoropen(m_Link);
        }

        //deactivate replace all ability
        this->DisableReplaceAll();
       ;
    });

    connect(m_Replace1,&QToolButton::clicked,[this](bool x)
    {
        //check if current selection is a search item

        if(m_Link == nullptr)
        {
            LinkCurrent();
            return;
        }


        QTextCursor t = m_Link->textCursor();
        int post = t.position();
        int lengt = t.selectionEnd() - t.selectionStart();

        bool found = false;
        for(int i = 0; i < m_SR_Position.size(); i++)
        {
            if(m_SR_Position.at(i) == post && m_SR_Length.at(i) == lengt)
            {
                found = true;
                break;
            }
        }

        std::cout << "found ?  " << found << " " << post << " " << lengt << " " << std::endl;
        if(!found)
        {
            this->FindNext();
        }else
        {
            QString replace = m_ReplaceEdit->text();
            std::vector<int> indices;
            for(int i = 0; i < m_SR_Position.size(); i++)
            {
                indices.push_back(i);
            }

            std::sort(indices.begin(),indices.end(),[this](const int &a, const int &b)
            {
                return m_SR_Position.at(a) > m_SR_Position.at(b);

            });

            int offset = 0;
            bool foundh = false;
            for(int i = 0; i < m_SR_Position.size(); i++)
            {
                int index =indices.at(i);
                if(m_SR_Position.at(index) == post && m_SR_Length.at(index) == lengt)
                {
                    foundh = true;
                    QTextCursor t = m_Link->textCursor();

                    t.setPosition(m_SR_Position.at(index) + offset);
                    t.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_SR_Length.at(index));

                    offset += replace.size() - m_SR_Length.at(index);

                    t.removeSelectedText();
                    t.insertText(replace);

                    m_Link->setTextCursor(t);

                }

                if(foundh)
                {
                    m_SR_Position.at(index) += offset;
                    m_SR_Col.at(index) += offset;
                }

            }


            m_Link->ShowOverlayMessage("Replaced 1 occurences",2.0);

            m_Link->update();

            this->DisableReplaceAll();

            if(m_HasCallBackEditorOpen)
            {
                m_funceditoropen(m_Link);
            }


        }


       ;
    });

    connect(m_Close,&QToolButton::clicked,[this](bool x)
    {
        this->Close();

       ;
    });


}
void ScriptSearchWindow::FindPrevious()
{

    if(m_Link == nullptr)
    {
        LinkCurrent();
    }
    if(m_Link == nullptr){
        return;
    }

    //get current cursor
    int pos = m_Link->textCursor().position();

    int posn = pos;
    int lenn = 0;
    int difm = INT_MAX;
    //next previous search in line
    for(int i = 0; i < m_SR_Position.size(); i++)
    {
        int dif = 0;
        if(m_SR_Position.at(i) < pos)
        {
            dif = pos - m_SR_Position.at(i);
        }else
        {
            dif = pos + m_Text.size() - m_SR_Position.at(i);
        }

        if(dif < difm)
        {
            difm = dif;
            posn = m_SR_Position.at(i);
            lenn = m_SR_Length.at(i);
        }
    }

    //go to it
    QTextCursor t = m_Link->textCursor();
    t.setPosition(posn);
    t.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,lenn);
    m_Link->setTextCursor(t);
   ;

    if(m_HasCallBackEditorOpen)
    {
        m_funceditoropen(m_Link);
    }

}

void ScriptSearchWindow::FindNext()
{
    if(m_Link == nullptr)
    {
        LinkCurrent();
    }
    if(m_Link == nullptr){
        return;
    }
    //get current cursor
    int pos = m_Link->textCursor().position();

    int posn = pos;
    int lenn = 0;
    int difm = INT_MAX;
    //next previous search in line
    for(int i = 0; i < m_SR_Position.size(); i++)
    {
        int dif = 0;
        if(m_SR_Position.at(i) > pos)
        {
            dif = m_SR_Position.at(i)- pos;
        }else
        {
            dif = m_SR_Position.at(i) + m_Text.size() - pos;
        }

        if(dif < difm)
        {
            difm = dif;
            posn = m_SR_Position.at(i);
            lenn = m_SR_Length.at(i);
        }
    }

    //go to it
    QTextCursor t = m_Link->textCursor();
    t.setPosition(posn);
    t.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,lenn);
    m_Link->setTextCursor(t);

    if(m_HasCallBackEditorOpen)
    {
        m_funceditoropen(m_Link);
    }
}
void ScriptSearchWindow::OnSearchTextChanged(QString s)
{

    if(m_Link != nullptr)
    {

        OpenAsSearch(m_Link,s,0);
    }else
    {
        LinkCurrent();
    }


}
void ScriptSearchWindow::OnReplaceTextChanged(QString s)
{

}


void ScriptSearchWindow::Clear()
{

    {
        mt->Empty();
        delete mt;
        mt = new MatrixTable();
        model->SetAllData(mt);
        //proxyModel = new QSortFilterProxyModel();
        //proxyModel->setSourceModel( model );
        m_Table->setModel( proxyModel );


    }

}

void ScriptSearchWindow::OpenAsReferencesTo(CodeEditor* c, QString name, int pos)
{
    Link(c);
    ScriptAnalyzer s(c->m_ScriptManager);
    s.AnalyzeScript(c->document()->toPlainText());
    m_Text = c->document()->toPlainText();

    std::vector<ScriptReferenceResult> res = s.GetReferencesTo(name,pos);

    if(res.size() == 0)
    {
        c->ShowOverlayMessage("Found no references to  '" + name + QString("'"),2.0);


        c->SetHighlightSearch({},{},{});
        m_SR_Position.clear();
        m_SR_Line.clear();
        m_SR_Col.clear();
        m_SR_Length.clear();

    }else
    {
        c->ShowOverlayMessage("Found " + QString::number(res.size()) + " references to  '" + name + QString("'"),2.0);


        std::vector<int> s_line;
        std::vector<int> s_col;
        std::vector<int> s_len;

        MatrixTable * t = new MatrixTable();
        t->SetSize(res.size(),1);
        t->SetColumnTitle(0,QString("Text"));
        for(int i = 0; i < res.size(); i++)
        {
            s_line.push_back(res.at(i).row_n);
            s_col.push_back(res.at(i).col_n-1);
            s_len.push_back(name.size());

            m_SR_Position.push_back(res.at(i).pos);
            m_SR_Line.push_back(res.at(i).row_n);
            m_SR_Col.push_back(res.at(i).col_n);
            m_SR_Length.push_back(name.size());

            t->SetValue(i,0,res.at(i).line);
            t->SetRowTitle(i,QString::number(res.at(i).row_n));
        }

        c->SetHighlightSearch(s_line,s_col,s_len);


        this->Open();
        this->SetData(t);
        this->SetTitle("References to ");
        m_SearchEdit->blockSignals(true);
        m_SearchEdit->setText(name);
        m_SearchEdit->blockSignals(false);
        SetCallBackItemClicked([c,res](int row, int col, QString line, int clicks)
                    {


                        if(clicks == 2)
                        {

                            if(row < res.size())
                            {
                                ScriptReferenceResult resh = res.at(row);

                                QTextCursor text_cursor(c->document()->findBlockByNumber(resh.row_n));
                                text_cursor.select(QTextCursor::BlockUnderCursor);
                                c->setTextCursor(text_cursor);

                            }

                        }
                        ;
                    });
    }

    c->update();
    c->repaint();

    EnableReplaceAll();
}

void ScriptSearchWindow::OpenAsSearch(CodeEditor* c, QString name, int pos)
{
    Link(c);
    QString str = c->document()->toPlainText();
    m_Text = c->document()->toPlainText();

    QStringList Lines = str.split('\n');

    qsizetype j = 0;

    std::vector<int> poss = {};
    if(!name.isEmpty())
    {

        while ((j = str.indexOf(name, j)) != -1) {
                poss.push_back(j);
            ++j;
        }
    }

    if(poss.size() == 0)
    {
        c->ShowOverlayMessage("Found no occurences of '" + name + QString("'"),2.0);


        c->SetHighlightSearch({},{},{});

        m_SR_Position.clear();
        m_SR_Line.clear();
        m_SR_Col.clear();
        m_SR_Length.clear();
    }else
    {
        c->ShowOverlayMessage("Found " + QString::number(poss.size()) + " references to  '" + name + QString("'"),2.0);

        std::vector<int> s_line;
        std::vector<int> s_col;
        std::vector<int> s_len;

        MatrixTable * t = new MatrixTable();
        t->SetSize(poss.size(),1);
        t->SetColumnTitle(0,QString("Text"));
        for(int i = 0; i < poss.size(); i++)
        {

            //get row and column
            QTextBlock b = c->document()->findBlock(poss.at(i));
            t->SetValue(i,0,b.text());

            s_line.push_back(b.blockNumber());
            s_col.push_back(poss.at(i) - b.position());
            s_len.push_back(name.size());


            m_SR_Position.push_back(poss.at(i));
            m_SR_Line.push_back(b.blockNumber());
            m_SR_Col.push_back(poss.at(i) - b.position());
            m_SR_Length.push_back(name.size());


            t->SetRowTitle(i,QString::number(b.blockNumber()));
        }

        c->SetHighlightSearch(s_line,s_col,s_len);


        this->Open();
        this->SetData(t);
        this->SetTitle("References to ");
        m_SearchEdit->blockSignals(true);
        m_SearchEdit->setText(name);
        m_SearchEdit->blockSignals(false);
        SetCallBackItemClicked([c,poss](int row, int col, QString line, int clicks)
                    {


                        if(clicks == 2)
                        {

                            if(row < poss.size())
                            {
                                int resh = poss.at(row);

                                QTextCursor text_cursor(c->document()->findBlock(resh));
                                text_cursor.select(QTextCursor::BlockUnderCursor);
                                c->setTextCursor(text_cursor);

                            }

                        }
                        ;
                    });

    }

    c->update();
    c->repaint();
    EnableReplaceAll();
}

void ScriptSearchWindow::OpenAsReplaceFile(CodeEditor* c, QString name, int pos)
{

    OpenAsSearch(c,name,pos);


}

void ScriptSearchWindow::OpenAsReplace(CodeEditor* c, QString name, int pos)
{
    Link(c);
    ScriptAnalyzer s(c->m_ScriptManager);
    s.AnalyzeScript(c->document()->toPlainText());
    m_Text = c->document()->toPlainText();
    std::vector<ScriptReferenceResult> res = s.GetReferencesTo(name,pos);

    if(res.size() == 0)
    {
        c->ShowOverlayMessage("Found no references to  '" + name + QString("'"),2.0);



        c->SetHighlightSearch({},{},{});
        m_SR_Position.clear();
        m_SR_Line.clear();
        m_SR_Col.clear();
        m_SR_Length.clear();
    }else
    {
        c->ShowOverlayMessage("Found " + QString::number(res.size()) + " references to  '" + name + QString("'"),2.0);

        std::vector<int> s_line;
        std::vector<int> s_col;
        std::vector<int> s_len;

        MatrixTable * t = new MatrixTable();
        t->SetSize(res.size(),1);
        t->SetColumnTitle(0,QString("Text"));
        for(int i = 0; i < res.size(); i++)
        {
            s_line.push_back(res.at(i).row_n);
            s_col.push_back(res.at(i).col_n-1);
            s_len.push_back(name.size());


            m_SR_Position.push_back(res.at(i).pos);
            m_SR_Line.push_back(res.at(i).row_n);
            m_SR_Col.push_back(res.at(i).col_n);
            m_SR_Length.push_back(name.size());

            t->SetValue(i,0,res.at(i).line);
            t->SetRowTitle(i,QString::number(res.at(i).row_n));
        }

        c->SetHighlightSearch(s_line,s_col,s_len);

        this->Open();
        this->SetData(t);
        this->SetTitle("References to ");
        m_SearchEdit->blockSignals(true);
        m_SearchEdit->setText(name);
        m_SearchEdit->blockSignals(false);
        SetCallBackItemClicked([c,res](int row, int col, QString line, int clicks)
                    {


                        if(clicks == 2)
                        {

                            if(row < res.size())
                            {
                                ScriptReferenceResult resh = res.at(row);

                                QTextCursor text_cursor(c->document()->findBlockByNumber(resh.row_n));
                                text_cursor.select(QTextCursor::BlockUnderCursor);
                                c->setTextCursor(text_cursor);

                            }

                        }
                        ;
                    });
    }

    c->update();
    c->repaint();


    EnableReplaceAll();
}
void ScriptSearchWindow::Link(CodeEditor *c)
{

    m_Link = c;
    connect(m_Link,&CodeEditor::destroyed,this,&ScriptSearchWindow::onLinkDestroyed);

}
void ScriptSearchWindow::onLinkDestroyed()
{
    UnLink();

}
CodeEditor * ScriptSearchWindow::GetLink()
{
    return m_Link;
}
void ScriptSearchWindow::UnLink()
{

    m_Link = nullptr;
    DisableReplaceAll();
}

void ScriptSearchWindow::LinkCurrent()
{

    if(m_HasCallBackEditorCurrent)
    {
        CodeEditor * c = m_funceditorcurrent();
        if(c != nullptr)
        {
            Link(c);
        }
    }

}

void ScriptSearchWindow::Open()
{
    if(m_HasCallBackOpen )
    {
        m_funcopen();
    }
}

void ScriptSearchWindow::Close()
{
    if(m_HasCallBackClose )
    {
        m_funcclose();
    }
}



void ScriptSearchWindow::SetTitle(QString title)
{
    m_Label->setText(title);
}
void ScriptSearchWindow::SetData(MatrixTable * T)
{
    //mt->Empty();
    //delete mt;
    mt = T->Copy();

    model->SetAllData(mt);
    //proxyModel = new QSortFilterProxyModel();
    //proxyModel->setSourceModel( model );
    m_Table->setModel( proxyModel );
    m_Table->verticalHeader()->setDefaultSectionSize(10);
    m_Table->resizeRowsToContents();
    m_Table->setColumnWidth(0,2000);

}
