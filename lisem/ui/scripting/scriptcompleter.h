#ifndef SCRIPTCOMPLETER_H
#define SCRIPTCOMPLETER_H

#include <QCompleter>
#include "QStringListModel"
#include "QStandardItemModel"
#include "QTableView"
#include "QHeaderView"

#include "QToolTip"
#include "QLabel"
#include "QDialog"
#include "QVBoxLayout"
#include "QEvent"
#include "QMouseEvent"
#include "QDir"
#include "lsmio.h"
#include "scriptmanager.h"
#include "extensionprovider.h"
#include "ScriptAnalyzer.h"
#include "iostream"

//table view for popup
class SPHScriptCompleterPopupView : public QTableView
{

    Q_OBJECT

    QDialog *popup;
     QLabel *popupLabel;
public:
     QStringList descriptions;

public:
    inline SPHScriptCompleterPopupView() : QTableView()
    {
        connect(selectionModel(),&QItemSelectionModel::selectionChanged,this,&SPHScriptCompleterPopupView::OnSelectionChanged);

        viewport()->installEventFilter(this);
        setMouseTracking(true);
        popup = new QDialog(this, Qt::Popup | Qt::ToolTip);

        QVBoxLayout *layout = new QVBoxLayout;
        popupLabel = new QLabel(popup);
        popupLabel->setWordWrap(true);
        layout->addWidget(popupLabel);
        popupLabel->setTextFormat(Qt::RichText);
        //popupLabel->setOpenExternalLinks(true);
        popup->setLayout(layout);
        popup->installEventFilter(this);
        hideColumn(1);

    }

    inline ~SPHScriptCompleterPopupView()
    {

        popup->hide();
    }

    bool eventFilter(QObject *watched, QEvent *event){
            if(viewport() == watched){
                if(event->type() == QEvent::MouseMove){
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                    QModelIndex index = indexAt(mouseEvent->pos());
                    if(index.isValid()){
                        showPopup(index);
                    }
                    else{
                        popup->hide();
                    }
                }
                else if(event->type() == QEvent::Leave){
                    popup->hide();
                }
            }
            else if(popup == watched){
                if(event->type() == QEvent::Leave){
                    popup->hide();
                }
            }
            return QTableView::eventFilter(watched, event);
        }

    void showPopup (const QModelIndex &index) const {
            if(index.column() == 0){
                int indexr = index.row();
                QRect r = visualRect(index);
                popup->move(viewport()->mapToGlobal(r.bottomRight()));
                popup->setFixedSize(300, popup->heightForWidth(300));

                popupLabel->setText(descriptions.at(indexr));
                popup->adjustSize();
                popup->show();
            }
            else {
                popup->hide();
            }
        }

    inline void UpdateToolTip()
    {
        /*QModelIndexList lrows = selectionModel()->selectedRows();


        std::cout << "update tooltip " << lrows.length() << std::endl;

        if(lrows.length() > 0)
        {
            QModelIndex index = model()->index(lrows.at(0).row(),1);
            if (!index.isValid())
            {
                QToolTip::hideText();
            }

           QString itemText = model()->data(index).toString();
            QString itemTooltip = model()->data(index, Qt::ToolTipRole).toString();

            QFontMetrics fm(font());
            int itemTextWidth = fm.width(itemText);
            QRect rect = visualRect(index);
            int rectWidth = rect.width();

            std::cout << "show text " << itemTooltip.toStdString() << std::endl;

           if ((itemTextWidth > rectWidth) && !itemTooltip.isEmpty())
            {


                QToolTip::showText(mapToGlobal(QPoint(0,0)), itemTooltip, this);
            }
            else
            {
                QToolTip::hideText();
            }

        }else {
            QToolTip::hideText();
        }*/

    }

public slots:

    inline void hideEvent(QHideEvent*e) override
    {
        popup->hide();
        QTableView::hideEvent(e);
    }

    inline void OnSelectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
    {

        UpdateToolTip();


    }
};


class SPHScriptCompleter : public QCompleter
{
    Q_OBJECT


private:
    QStringList m_list;
    QStringListModel m_model;
    QStandardItemModel *m_CompleterModel;

    SPHScriptCompleterPopupView * tableView;
    QString m_currentword;
    QStringList m_word;
    QStringList m_descriptions;
    QStringList m_wordu;
    QStringList m_descriptionsu;

    QStringList m_wordm;
    QStringList m_wordmc;
    QStringList m_descriptionsm;

    QString m_CurrentDir;

    QStringList m_classes;
    QStringList m_classesScript;
    QList<ScriptFunctionInfo> funclist;
    QList<ScriptFunctionInfo> funcmemberlist;
    ScriptManager * m_ScriptManager;
    LSMScriptEngine * m_ScriptEngine;


    QList<ScriptToken> tokens;
    QList<ScriptToken> ClassTokens;
    QList<ScriptToken> VariableTokens;
    QList<ScriptToken> FunctionTokens;
    int cursorlevel = 0;

public:
    inline SPHScriptCompleter(ScriptManager * engine) :
            QCompleter(), m_model()
    {

        m_ScriptManager = engine;
        m_ScriptEngine = m_ScriptManager->m_Engine;

        QStringList functionnames = m_ScriptManager->GetGlobalFunctionNames();

        funclist = m_ScriptManager->m_Engine->GetGlobalFunctionList();
        funcmemberlist = m_ScriptManager->m_Engine->GetMemberFunctionList();

        QStringList fnames;
        QStringList fdescrp;

        m_classes.push_back("int8");
        m_classes.push_back("int16");
        m_classes.push_back("int");
        m_classes.push_back("int64");
        m_classes.push_back("uint8");
        m_classes.push_back("uint16");
        m_classes.push_back("uint");
        m_classes.push_back("uint64");
        m_classes.push_back("float");
        m_classes.push_back("double");

        for(int i = 0; i < m_ScriptEngine->GetTypedefCount(); i++)
        {
            asITypeInfo * info = m_ScriptEngine->GetTypedefByIndex(i);
            if(info)
            {
                const char * name = info->GetName();
                if(name)
                {
                    QString nameq = QString(name);
                    m_classes.append(nameq);
                }
            }
        }
        for(int i = 0; i < funclist.size(); i++)
        {
            fnames.append(funclist.at(i).Function->GetName());
            QString full = m_ScriptManager->m_Engine->GetTypeName(funclist.at(i).Function->GetReturnTypeId()) + " " +funclist.at(i).Function->GetName();
            full.append("(");
            //if(funclist.at(i).Parameters.size() > 0)
            {
                for(int j = 0; j < funclist.at(i).Function->GetParamCount(); j++)
                {
                    int id = 0;
                    funclist.at(i).Function->GetParam(j,&id);
                    full.append(m_ScriptManager->m_Engine->GetTypeName(id) + " ");
                    if(funclist.at(i).Parameters.size() > j)
                    {
                        full.append(funclist.at(i).Parameters.at(j));
                    }

                    if(j != funclist.at(i).Function->GetParamCount()-1)
                    {
                        full.append(",");
                    }
                }

            }
            full.append(")");
            full.append("\n");
            full.append("\n");
            full.append(funclist.at(i).Description);

            fdescrp.append(full);

        }


        m_descriptions = fdescrp;
        m_word = fnames;
        m_list = fnames;


        for(int i = 0; i < funcmemberlist.size(); i++)
        {

            m_wordm.append(funcmemberlist.at(i).Function->GetName());
            m_wordmc.append(funcmemberlist.at(i).ObjectName);
            QString full = m_ScriptManager->m_Engine->GetTypeName(funcmemberlist.at(i).Function->GetReturnTypeId()) + " " +funcmemberlist.at(i).ObjectName + "::" + funcmemberlist.at(i).Function->GetName();
            full.append("(");
            //if(funclist.at(i).Parameters.size() > 0)
            {
                for(int j = 0; j < funcmemberlist.at(i).Function->GetParamCount(); j++)
                {
                    int id = 0;
                    funcmemberlist.at(i).Function->GetParam(j,&id);
                    full.append(m_ScriptManager->m_Engine->GetTypeName(id) + " ");
                    if(funcmemberlist.at(i).Parameters.size() > j)
                    {
                        full.append(funcmemberlist.at(i).Parameters.at(j));
                    }

                    if(j != funcmemberlist.at(i).Function->GetParamCount()-1)
                    {
                        full.append(",");
                    }
                }

            }
            full.append(")");
            full.append("\n");
            full.append("\n");
            full.append(funcmemberlist.at(i).Description);

            m_descriptionsm.append(full);

        }
        //m_wordu = words;
        //m_descriptionsu = descriptions;

        for(int i = 0; i < m_word.length(); i++)
        {
            //if(!m_wordu.contains(m_word.at(i)))
            {
                m_wordu.append(m_word.at(i));
                m_descriptionsu.append(m_descriptions.at(i));
            }
        }

        connect(this, SIGNAL(highlighted(QModelIndex)),this,SLOT(OnHighLighted(QModelIndex)));

        m_CompleterModel = new QStandardItemModel(m_list.length(),1);
        for(int i = 0; i < m_wordu.length(); i++)
        {
            m_CompleterModel->setItem(i,0,new QStandardItem(m_wordu.at(i)));
            //m_CompleterModel->setItem(i,1,new QStandardItem("Description: " + m_wordu.at(i)));
        }

        setModel(m_CompleterModel);
        setCompletionColumn(0);

        tableView = new SPHScriptCompleterPopupView();

        tableView->verticalHeader()->setVisible(false);
        tableView->horizontalHeader()->setVisible(false);
        tableView->setSelectionBehavior(QTableView::SelectRows);
        tableView->setShowGrid(false);

        tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->hideColumn(1);

        //tableView->resizeColumnsToContents();
        //tableView->resizeRowsToContents();
        setPopup(tableView);

    }

    inline bool update(QString word, QString functype = "", QString script = "", int pos = 0)
    {
        //functype does not do anything yet!

        //this sets a couple of global variables, lists containing all function, variable and class tokens found in the script so far
        GetScriptFunctions(script, pos);

        if(functype == "")
        {
            // Do any filtering you like.
            // Here we just include all items that contain word.
            QStringList filtered;
            QStringList filteredd;

            for(int i = 0; i < m_wordu.length(); i++)
            {
                if(m_wordu.at(i).startsWith(word, caseSensitivity()))
                {
                    filtered.append(m_wordu.at(i));
                    filteredd.append(m_descriptionsu.at(i));
                }

            }

            tableView->setVisible(false);

            QStringList filesf;
            QStringList filesfd;

            if(!m_CurrentDir.isEmpty())
            {
                QDir directory(m_CurrentDir);
                QStringList files = directory.entryList(GetAllExtensionsFilter(),QDir::Files);



                for(int i = 0; i < files.length(); i++)
                {
                    if(files.at(i).startsWith(word, caseSensitivity()))
                    {
                        filesf.append(files.at(i));
                        filesfd.append(m_CurrentDir + QDir::separator() + files.at(i));

                    }
                }
            }

            int count = filtered.length() + filesf.length();

            QList<QString> filtered_other;
            QList<QString> filtered_otherd;
            //classes
            if(m_classes.size() != 0)
            {
                for(int i = 0; i < m_classes.size(); i++)
                {
                    if(m_classes.at(i).startsWith(word,caseSensitivity()))
                    {
                        count += 1;
                        filtered_other.push_back(m_classes.at(i));
                        filtered_otherd.push_back(m_classes.at(i) + " (class)");

                    }
                }
            }
            if(ClassTokens.size() != 0)
            {
                for(int i = 0; i < ClassTokens.size(); i++)
                {
                    if(ClassTokens.at(i).name.startsWith(word,caseSensitivity()))
                    {
                        count += 1;
                        filtered_other.push_back(m_classes.at(i));
                        filtered_otherd.push_back(m_classes.at(i) + " (class)");
                    }
                }
            }

            if(FunctionTokens.size() != 0)
            {

                for(int i = 0; i < FunctionTokens.size(); i++)
                {
                    if(FunctionTokens.at(i).name.startsWith(word,caseSensitivity()))
                    {
                        count += 1;
                        filtered_other.push_back(FunctionTokens.at(i).name);
                        filtered_otherd.push_back(FunctionTokens.at(i).text);
                    }
                }

            }

            if(VariableTokens.size() != 0)
            {

                for(int i = 0; i < VariableTokens.size(); i++)
                {
                    if(VariableTokens.at(i).name.startsWith(word,caseSensitivity()))
                    {
                        count += 1;
                        filtered_other.push_back(VariableTokens.at(i).name);
                        filtered_otherd.push_back(VariableTokens.at(i).text);
                    }
                }

            }


            m_CompleterModel->clear();
            m_CompleterModel->setColumnCount(1);
            m_CompleterModel->setRowCount(count);


            for(int i = 0; i < filtered.length(); i++)
            {
                m_CompleterModel->setItem(i,0,new QStandardItem(filtered.at(i)));

                //m_CompleterModel->setItem(i,1,new QStandardItem(filteredd.at(i)));
            }

            for(int i = 0; i < filesf.length(); i++)
            {
                m_CompleterModel->setItem(filtered.length() + i,0,new QStandardItem(filesf.at(i)));

                filteredd.append(filesfd.at(i));
            }

            for(int i = 0; i < filtered_other.length(); i++)
            {
                m_CompleterModel->setItem(filtered.length() + filesf.length() + i,0,new QStandardItem(filtered_other.at(i)));
                filteredd.append(filtered_other.at(i));

            }

            tableView->descriptions = filteredd;

            m_model.setStringList(filtered);
            m_currentword = word;

            tableView->setVisible(true);

            complete();
            this->setCompletionPrefix(word);

            if(filteredd.length() > 0)
            {
                return true;
            }else
            {
                return false;
            }

        }else//if(functype == "member")
        {


            tableView->setVisible(false);

            bool stringliteral = false;
            bool hasParenthesis = true;
            int nested = 0;
            QString identifier = "";
            QString classidentifier = "";
            for(int i = pos - word.length()-1; i > -1; i--)
            {
                if(i < script.length())
                {

                    if(script[i] == ')')
                    {
                        hasParenthesis = true;
                        nested += 1;
                    }else if(script[i] == "\"")
                    {
                        if(!hasParenthesis || nested == 0)
                        {
                            break;
                        }
                        stringliteral = !stringliteral;
                    }else if(script[i] == '(')
                    {
                        nested -= 1;
                        if(nested == 0)
                        {

                        }
                    }else
                    {
                        if(script[i].isLetterOrNumber())
                        {
                            identifier = script[i] + identifier;
                        }else
                        {
                            if(!hasParenthesis || nested == 0)
                            {
                                break;
                            }
                        }
                    }
                }else
                {
                    break;
                }
            }

            if(word.length() > 1)
            {
                word = word.right(word.length()-1);
            }else
            {
                word.clear();
            }

            if(identifier.isEmpty())
            {
                return false;
            }

            QString typeidentifier = "";

            //find the type of the identifier

            //is it a globally known function?

            bool found = false;
            for(int i = 0; i < funclist.size(); i++)
            {
                if(QString(funclist.at(i).Function->GetName()) == identifier)
                {
                    typeidentifier = m_ScriptManager->m_Engine->GetTypeName(funclist.at(i).Function->GetReturnTypeId());
                    found = true;
                    break;

                }
            }

            //is it a script-based function?
            if(!found)
            {
                for(int i = 0; i < FunctionTokens.size(); i++)
                {
                    if(FunctionTokens.at(i).name == identifier )
                    {
                        typeidentifier = FunctionTokens.at(i).Type;
                        found = true;
                        break;
                    }
                }

            }

            //is it a script-based variable?
            if(!found)
            {
                for(int i = 0; i < VariableTokens.size(); i++)
                {
                    if(VariableTokens.at(i).name == identifier )
                    {
                        typeidentifier = VariableTokens.at(i).Type;
                        found = true;
                        break;
                    }
                }

            }

            if(typeidentifier.isEmpty())
            {
                return false;
            }

            //detect and remove template arguments, as the functions are registered to the object id withou any specified template arguments
            if(typeidentifier.contains('<'))
            {
                QString temp;
                for(int i = 0; i < typeidentifier.length(); i++)
                {
                    if(typeidentifier[i] != '<')
                    {
                        temp = temp +typeidentifier[i];
                    }else
                    {
                        break;
                    }
                }
                typeidentifier = temp;
            }

            std::cout << "check member functions " << typeidentifier.toStdString() << std::endl;

            // Do any filtering you like.
            // Here we just include all items that contain word.
            QStringList filtered;
            QStringList filteredd;

            for(int i = 0; i < m_wordm.length(); i++)
            {
                if(m_wordm.at(i).startsWith(word, caseSensitivity()) || word.isEmpty())
                {
                    if(m_wordmc.at(i) == typeidentifier.trimmed())
                    {
                        filtered.append(m_wordm.at(i));
                        filteredd.append(m_descriptionsm.at(i));
                    }
                }

            }
            m_CompleterModel->clear();
            m_CompleterModel->setColumnCount(1);
            m_CompleterModel->setRowCount(filtered.size());

            for(int i = 0; i < filtered.length(); i++)
            {
                m_CompleterModel->setItem( i,0,new QStandardItem(filtered.at(i)));

            }

            tableView->descriptions = filteredd;

            m_model.setStringList(filtered);
            m_currentword = word;

            tableView->setVisible(true);

            complete();
            this->setCompletionPrefix(word);

            if(filtered.length() > 0)
            {
                return true;
            }else
            {
                return false;
            }


        }/*else if(functype == "namespace")
        {




        }*/

    }

    inline QStringList GetVariableNamesFromScript()
    {

        return {};
    }

    inline QStringList GetPotentialFileNames()
    {

        return {};
    }

    inline void GetCurrentLevel(QString qscript, int pos)
    {


    }

    inline void PosToLine(QString qscript, int pos)
    {


    }

    inline void GetScriptFunctions(QString qscript, int cpos)
    {
        m_classesScript.clear();
        std::string modifiedScript = qscript.toStdString();
        std::string currentClass = "";
        std::string currentNamespace = "";
        unsigned int pos = 0;
        int nested = 0;
        int nested_total = 0;

        bool canbestatement = true;
        bool is_enum = false;

        cursorlevel = 0;

        tokens.clear();
        ClassTokens.clear();
        VariableTokens.clear();
        FunctionTokens.clear();

        while( pos < modifiedScript.size() )
        {
            asUINT len = 0;
            asETokenClass t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

            std::string token;
            token.assign(&modifiedScript[pos], len);

            ScriptToken token_this;
            token_this.tokenclass = t;
            token_this.pos = pos;
            token_this.length = len;
            token_this.text = QString(token.c_str());

            tokens.append(token_this);

            if( pos >  cpos)
            {
                cursorlevel = nested_total;
            }

            if(nested == 0)
            {
                // Check if class
                if( currentClass == "" && (modifiedScript.substr(pos,len) == "class" || modifiedScript.substr(pos,len) == "interface" ) )
                {
                    // Get the identifier after "class"
                    do
                    {
                        pos += len;
                        if( pos >= modifiedScript.size() )
                        {
                            t = asTC_UNKNOWN;
                            break;
                        }
                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                    } while(t == asTC_COMMENT || t == asTC_WHITESPACE);

                    if( t == asTC_IDENTIFIER )
                    {
                        currentClass = modifiedScript.substr(pos,len);

                        ScriptToken token_class;
                        token_class.is_class = true;
                        token_class.name = QString(modifiedScript.substr(pos,len).c_str());
                        token_class.nspace = QString(currentNamespace.c_str());
                        token_class.nesting = nested_total;
                        token_class.pos = token_this.pos;
                        token_class.length = pos + len - token_this.pos;

                        ClassTokens.append(token_class);

                        m_classesScript.append(QString(currentClass.c_str()));

                        // Search until first { or ; is encountered
                        while( pos < modifiedScript.length() )
                        {
                            m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                            // If start of class section encountered stop
                            if( modifiedScript[pos] == '{' )
                            {
                                nested_total += 1;
                                pos += len;
                                break;
                            }
                            else if (modifiedScript[pos] == ';')
                            {
                                // The class declaration has ended and there are no children
                                currentClass = "";
                                pos += len;
                                break;
                            }

                            // Check next symbol
                            pos += len;
                        }
                    }

                    continue;
                }

                // Check if end of class
                if( currentClass != "" && modifiedScript[pos] == '}' )
                {
                    currentClass = "";
                    nested_total -= 1;
                    pos += len;
                    continue;
                }


                // Check if namespace
                if( modifiedScript.substr(pos,len) == "namespace" )
                {
                    // Get the identifier after "namespace"
                    do
                    {
                        pos += len;
                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                    } while(t == asTC_COMMENT || t == asTC_WHITESPACE);

                    if( currentNamespace != "" )
                        currentNamespace += "::";
                    currentNamespace += modifiedScript.substr(pos,len);

                    // Search until first { is encountered
                    while( pos < modifiedScript.length() )
                    {
                        m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                        // If start of namespace section encountered stop
                        if( modifiedScript[pos] == '{' )
                        {
                            nested_total += 1;
                            pos += len;
                            break;
                        }

                        // Check next symbol
                        pos += len;
                    }

                    continue;
                }

                // Check if end of namespace
                if( currentNamespace != "" && modifiedScript[pos] == '}' )
                {
                    size_t found = currentNamespace.rfind( "::" );
                    if( found != std::string::npos )
                    {
                        currentNamespace.erase( found );
                    }
                    else
                    {
                        currentNamespace = "";
                    }
                    nested_total -= 1;
                    pos += len;
                    continue;
                }
            }

            //detect the potential begin of a new statement, so we can indicate we are looking for a new function or variable
            if(modifiedScript[pos] == '}')
            {
                nested += 1;
                nested_total += 1;
                pos += len;
                continue;
            }

            //detect the potential begin of a new statement, so we can indicate we are looking for a new function or variable
            if(modifiedScript[pos] == '}')
            {
                nested -= 1;
                nested_total -= 1;
                nested = std::max(nested,0);
                nested_total = std::max(nested_total,0);
                pos += len;
                continue;
            }

            // Skip white spaces, comments, and leading decorators
            if( t == asTC_KEYWORD && !(token == "void" || token == "int" || token == "double" ||token == "float" || token == "bool"||token == "uint" || token == "int8" ||token == "uint8" ||token == "int16" ||token == "uint16" ||token == "int64" ||token == "uint64"))
            {
                //any reserved words, or [,],,,/,.,<,>,@,+,-,*,^,<<,>>,=,==

                if( token == "interface" || token == "class" ||
                        token == "enum" || token == "private" ||
                        token == "protected" ||
                        token == "shared" || token == "external" ||
                        token == "final" || token == "abstract" )
                {



                }

                //we detected classes and interfaces already, the other ones do not matter now
                if( token == "enum" )
                {
                    bool did_identifier = false;
                    // Search until first { or ; is encountered
                    while( pos < modifiedScript.length() )
                    {
                        int t_inner = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                        if(t_inner == asTC_IDENTIFIER)
                        {
                            if(!did_identifier)
                            {
                                std::string token_enum;
                                token_enum.assign(&modifiedScript[pos], len);
                                ScriptToken token_var;
                                token_var.is_variable = true;
                                token_var.name = QString(token_enum.c_str());
                                token_var.text = QString(token_enum.c_str());
                                token_var.nspace = QString(currentNamespace.c_str());
                                token_var.nesting = nested_total;
                                token_var.pos = token_this.pos;
                                token_var.length = pos + len - token_this.pos;

                                //if non-global nesting is 0, it means it is an object member
                                if(nested == 0)
                                {
                                    token_var.memberclass = QString(currentClass.c_str());
                                }

                                VariableTokens.append(token_var);

                                //enums are recognizeable variables
                                did_identifier = true;
                            }
                        }else
                        // If start of class section encountered stop
                        if( modifiedScript[pos] == ',' )
                        {
                            nested_total += 1;
                            pos += len;
                            did_identifier = false;
                            continue;
                        }
                        else
                        //end of enum block
                        if (modifiedScript[pos] == '}')
                        {
                            // The class declaration has ended and there are no children
                            currentClass = "";
                            pos += len;
                            break;
                        }

                        // Check next symbol
                        pos += len;
                    }


                }


                //for and while
                if( token == "for" || token == "while" || token == "if" || token == "else")
                {

                    int nested_inner = 0;

                    for(; pos < (int)modifiedScript.size();)
                    {

                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                        token.assign(&modifiedScript[pos], len);
                        if(token == "(")
                        {
                            nested_inner ++;
                        }
                        if(token == ")")
                        {
                            nested_inner --;

                        }else if(token == "{")
                        {
                            if(nested_inner == 0)
                            {
                                nested += 1;
                                nested_total += 1;

                                pos += len;
                                break;
                            }
                        }else if(token == ";")
                        {
                            if(nested_inner == 0)
                            {
                                //means either its do{}while(); or for() ...;
                                //in both cases, we already dealt with the nesting scope
                                pos += len;
                                break;
                            }
                        }

                        pos += len;
                    }
                }


            }

            // get some kind of statement (can be a variable or a function)
            if( t == asTC_IDENTIFIER || (token == "void" || token == "int" || token == "double" ||token == "float" || token == "bool"||token == "uint" || token == "int8" ||token == "uint8" ||token == "int16" ||token == "uint16" ||token == "int64" ||token == "uint64"))
            {

                pos = pos + len;

                //first check if there is a dot after this identifier. if so, we have a write to an object member or a script-recognized file
                if(pos < (int)modifiedScript.size())
                {
                    if(modifiedScript[pos] == '.')
                    {
                        pos=pos + 1;
                        if(pos < (int)modifiedScript.size())
                        {
                            t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                            std::string token_temp;
                            token_temp.assign(&modifiedScript[pos], len);


                            token = token + "."+ token_temp;
                            pos += len;
                        }
                    }
                }

                bool hasParenthesis = false;
                int nestedParenthesis = 0;
                bool hasOtherParenthesis = false;
                int nestedOtherParenthesis = 0;
                std::string name;
                bool name_found = false;
                std::string decleration = token;
                std::string decleration_noargs = token;

                //detect if function or otherwise variable?
                for(; pos < (int)modifiedScript.size();)
                {

                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                    token.assign(&modifiedScript[pos], len);

                    if (t == asTC_KEYWORD)
                    {
                        if(!(hasParenthesis || (token == "(") || (token == ")")))
                        {
                            decleration_noargs += token;
                        }
                        decleration += token;
                        if (token == "{" && nestedParenthesis == 0)
                        {
                            if (hasParenthesis)
                            {
                                if(name_found)
                                {
                                    // We've found the end of a function signature

                                    ScriptToken token_func;
                                    token_func.is_function = true;
                                    token_func.name = QString(name.c_str());
                                    token_func.text = QString(decleration.c_str());
                                    token_func.nspace = QString(currentNamespace.c_str());
                                    token_func.nesting = nested_total;
                                    token_func.pos = token_this.pos;
                                    token_func.length = pos + len - token_this.pos;

                                    QString type = QString(decleration_noargs.c_str()).trimmed();
                                    token_func.Type = type.left(std::max(0,(int) (type.length() - name.length()))).trimmed();

                                    std::cout << "function " << name <<  "   " <<  decleration_noargs << " " << token_func.Type.toStdString() <<  std::endl;


                                    //if non-global nesting is 0, it means it is an object member
                                    if(nested == 0)
                                    {
                                        token_func.memberclass = QString(currentClass.c_str());
                                    }

                                    FunctionTokens.append(token_func);

                                    nested += 1;
                                    nested_total += 1;

                                }else if(token == "= ")
                                {
                                    int nested_inner = 0;
                                    for(; pos < (int)modifiedScript.size();)
                                    {
                                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                        token.assign(&modifiedScript[pos], len);
                                        if(token == "(")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == ")")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "[")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "]")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "{")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "}")
                                        {
                                            nested_inner --;

                                        }
                                        else if(token == ";")
                                        {
                                            if(nested_inner == 0)
                                            {
                                                pos += len;
                                                break;
                                            }
                                        }
                                        pos += len;
                                    }
                                    break;
                                }else
                                {
                                    pos += len;
                                    break;
                                }

                            }
                            else
                            {
                                // We've found a virtual property. Just keep the name

                            }
                            pos += len;
                            break;
                        }
                        if ((token == "=" && !hasParenthesis) || token == ";")
                        {
                            if (hasParenthesis)
                            {
                                if(name_found)
                                {
                                    // The declaration is ambigous. It can be a variable with initialization, or a function prototype
                                    // assume it is a variable

                                    ScriptToken token_var;
                                    token_var.is_variable = true;
                                    token_var.name = QString(name.c_str());
                                    token_var.text = QString(decleration.c_str());
                                    token_var.nspace = QString(currentNamespace.c_str());
                                    token_var.nesting = nested_total;
                                    token_var.pos = token_this.pos;
                                    token_var.length = pos + len - token_this.pos;
                                    QString type = QString(decleration_noargs.c_str()).trimmed();
                                    type = type.left(type.length()-1).trimmed();
                                    token_var.Type = type.left(std::max(0,(int) (type.length() - name.length())));


                                }

                                pos += len;
                                break;
                            }
                            else
                            {
                                //variable
                                // Substitute the declaration with just the name
                                if(name_found)
                                {
                                    int nested_inner = 0;

                                    ScriptToken token_var;
                                    token_var.is_variable = true;
                                    token_var.name = QString(name.c_str());
                                    token_var.text = QString(decleration.c_str());
                                    token_var.nspace = QString(currentNamespace.c_str());
                                    token_var.nesting = nested_total;
                                    token_var.pos = token_this.pos;
                                    token_var.length = pos + len - token_this.pos;
                                    QString type = QString(decleration.c_str()).trimmed();
                                    type = type.left(type.length()-1).trimmed();
                                    token_var.Type = type.left(std::max(0,(int) (type.length() - name.length())));

                                    //if non-global nesting is 0, it means it is an object member
                                    if(nested == 0)
                                    {
                                        token_var.memberclass = QString(currentClass.c_str());
                                    }

                                    VariableTokens.append(token_var);

                                    for(; pos < (int)modifiedScript.size();)
                                    {

                                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                        token.assign(&modifiedScript[pos], len);
                                        if(token == "(")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == ")")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "[")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "]")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "{")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "}")
                                        {
                                            nested_inner --;

                                        }
                                        else if(token == ";")
                                        {
                                            if(nested_inner == 0)
                                            {
                                                pos += len;
                                                break;
                                            }
                                        }
                                        pos += len;
                                    }

                                    break;
                                }else if(token == "=")
                                {
                                    int nested_inner = 0;
                                    for(; pos < (int)modifiedScript.size();)
                                    {
                                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                        token.assign(&modifiedScript[pos], len);
                                        if(token == "(")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == ")")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "[")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "]")
                                        {
                                            nested_inner --;

                                        }
                                        if(token == "{")
                                        {
                                            nested_inner ++;
                                        }
                                        if(token == "}")
                                        {
                                            nested_inner --;

                                        }
                                        else if(token == ";")
                                        {
                                            if(nested_inner == 0)
                                            {
                                                pos += len;
                                                break;
                                            }
                                        }
                                        pos += len;
                                    }
                                    break;
                                }
                            }
                        }else if(token == "=")
                        {
                            int nested_inner = 0;
                            for(; pos < (int)modifiedScript.size();)
                            {

                                t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                token.assign(&modifiedScript[pos], len);
                                if(token == "(")
                                {
                                    nested_inner ++;
                                }
                                if(token == ")")
                                {
                                    nested_inner --;

                                }
                                if(token == "[")
                                {
                                    nested_inner ++;
                                }
                                if(token == "]")
                                {
                                    nested_inner --;

                                }
                                if(token == "{")
                                {
                                    nested_inner ++;
                                }
                                if(token == "}")
                                {
                                    nested_inner --;

                                }
                                else if(token == ";")
                                {
                                    if(nested_inner == 0)
                                    {
                                        pos += len;
                                        break;
                                    }
                                }
                                pos += len;
                            }

                            break;
                        }
                        else if (token == "(")
                        {
                            nestedParenthesis++;

                            // This is the first parenthesis we encounter. If the parenthesis isn't followed
                            // by a statement block, then this is a variable declaration, in which case we
                            // should only store the type and name of the variable, not the initialization parameters.
                            hasParenthesis = true;
                        }else if(token == "[")
                        {
                            nestedOtherParenthesis++;
                            hasOtherParenthesis = true;

                        }else if(token == "]")
                        {
                            nestedOtherParenthesis--;
                        }else if(token == "<")
                        {
                            nestedOtherParenthesis++;
                            hasOtherParenthesis = true;

                        }else if(token == ">")
                        {
                            nestedOtherParenthesis--;
                        }
                        else if (token == ")")
                        {
                            nestedParenthesis--;
                        }

                    }
                    else if( t == asTC_IDENTIFIER )
                    {
                        if(!(hasParenthesis || (nestedOtherParenthesis>0)))
                        {
                            name_found = true;
                            name = token;

                        }
                        if(!hasParenthesis)
                        {
                            decleration_noargs += token;

                        }
                        decleration += token;
                    }else if(!(t == asTC_COMMENT) && token != "\n")
                    {
                        decleration += token;
                        if(!hasParenthesis)
                        {
                            decleration_noargs += token;
                        }
                    }

                    // Skip trailing decorators
                    //if( !hasParenthesis || nestedParenthesis > 0 || t != asTC_IDENTIFIER || (token != "final" && token != "override") )
                    //    declaration += token;

                    pos += len;
                }
                //finally unknown
            }

            pos += len;


        }







    }


    inline QStringList GetPotentialGlobalFunctions()
    {

        return {};
    }

    inline QStringList GetPotentialMemberFunction()
    {

        return {};
    }


    inline void GetScriptVariables()
    {


    }


    inline QStringList GetVariableTypeBeforeCursor()
    {
        //a variable returned from a function

            //returned from a global function

            //returned from a member function

        //an existing variable


        //a compound statement (or r-value variable)




        return {};
    }


    inline void ShowPopup0()
    {

        tableView->showPopup(tableView->model()->index(0,0));
    }

    inline QString word()
    {
        return m_currentword;
    }

    inline void SetDirectory(QString dir)
    {
        m_CurrentDir = dir;
    }
public slots:
    inline void OnHighLighted(QModelIndex index)
    {
        tableView->showPopup(index);
    }

};


#endif // SCRIPTCOMPLETER_H
