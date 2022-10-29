#pragma once
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
#include "QFont"


class QTreeWidgetLSM : public QTreeWidget
{

    Q_OBJECT;

public:
    inline QTreeWidgetLSM():QTreeWidget()
    {

    }

    inline void do_layout_update()
    {
        this->scheduleDelayedItemsLayout();
    }
};

class ToolWidget : public QWidget
{

        Q_OBJECT;
public:

    QVBoxLayout * m_MainLayout;

    ScriptFunctionInfo m_Function;
    ScriptManager * m_ScriptManager;
    QLabel * functionname;
    QLabel * description;
    inline ToolWidget(ScriptManager * sm, ScriptFunctionInfo f)
    {
        m_ScriptManager = sm;
        m_Function = f;

        m_MainLayout = new QVBoxLayout();
        m_MainLayout->setSizeConstraint( QLayout::SetFixedSize );
        this->setLayout(m_MainLayout);

        functionname = new QLabel(name());
        QFont font = functionname->font();
        font.setPointSize(14);
        functionname->setFont(font);

        description = new QLabel();
        description->setWordWrap(false);
        description->setText(f.Description);


        QString text_in = "Input: ";
        QString text_out = "Output: ";

        for(int i = 0; i < f.Function->GetParamCount(); i++)
        {
            QString parametername;
            if(i < f.Parameters.length())
            {
                 parametername = f.Parameters.at(i);
            }
            int typeId = 0;
            asDWORD flags = 0;
            const char * name;
            const char * def;
            f.Function->GetParam(i,&typeId,&flags,&name,&def);
            if(name != nullptr)
            {
                text_in +=" "+ (!parametername.isEmpty()? parametername : QString((name)));
            }

            text_in += " ( ";
            text_in += sm->m_Engine->GetTypeName(typeId);

            if(def != nullptr)
            {
                text_in +=  " ,default = " + QString((def));
            }
            text_in += ") ";
        }

        int typeId = f.Function->GetReturnTypeId();

        text_out += sm->m_Engine->GetTypeName(typeId);

        QLabel * functionin = new QLabel(text_in);
        QLabel * functionout = new QLabel(text_out);

        m_MainLayout->addWidget(functionname);
        m_MainLayout->addWidget(description);
        m_MainLayout->addWidget(functionin);
        m_MainLayout->addWidget(functionout);
        functionname->setMinimumSize(100,20);


    }

    bool m_IsHighLight = false;

    inline void setHighlight(bool hl)
    {
        if(hl && ! m_IsHighLight)
        {
            QFont font = functionname->font();
            font.setPointSize(18);
            functionname->setFont(font);
            description->setWordWrap(true);
            description->adjustSize();

        }else if(!hl && m_IsHighLight)
        {
            QFont font = functionname->font();
            font.setPointSize(14);
            functionname->setFont(font);
            description->setWordWrap(false);

            description->adjustSize();
        }
        m_IsHighLight = hl;

    }

    inline QString name()
    {
        if(m_Function.is_basetypeoperator)
        {
            QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);
            QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.basetypeoperatorname);

            if(m_Function.basetypeoperatorparametertype.isEmpty())
            {
                  return op + " " + m_Function.ObjectName + " (" + opname + ")";
            }else {
                  return m_Function.ObjectName + " " +  op + " " +m_Function.basetypeoperatorparametertype +  " (" + opname + ")";
              }

        }else {
            QString parameters;
            parameters += "(";

            int paramcount = m_Function.Function->GetParamCount();

            for(int i = 0; i < paramcount; i++)
            {
                int id = 0;
                m_Function.Function->GetParam(i,&id);
                parameters += m_ScriptManager->m_Engine->GetTypeName(id);
                if(i != paramcount -1)
                {
                    parameters += ",";
                }
            }
            parameters += ")";

            if(m_Function.is_constructor)
            {
                return QString(m_Function.ObjectName)+"::"+ parameters;
            }else if(m_Function.is_member)
            {
                QString op;
                QString opname;
                bool opinv;

                if(QString(m_Function.Function->GetName()).compare("eq") == 0)
                {
                    op = "==";
                    opname = "equal to";
                    opinv = false;

                }else if(QString(m_Function.Function->GetName()).compare("eq_r") == 0)
                {
                    op = "==";
                    opname = "equal to";
                    opinv = true;

                }else if(QString(m_Function.Function->GetName()).compare("neq") == 0)
                {
                    op = "!=";
                    opname = "not equal to";
                    opinv = false;

                }else if(QString(m_Function.Function->GetName()).compare("neq_r") == 0)
                {
                      op = "!=";
                      opname = "not equal to";
                      opinv = true;

                }else {

                    op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                    opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                    opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());
                }

                if(!op.isEmpty())
                {

                    QString object2name;
                    if(paramcount > 0)
                    {
                        int id = 0;
                        m_Function.Function->GetParam(0,&id);
                        object2name = m_ScriptManager->m_Engine->GetTypeName(id);
                    }

                    if(opinv)
                    {
                        return m_Function.ObjectName + " " + op + " " + object2name + "(" + opname + ")";
                    }else {
                        return object2name + " " + op + " " +m_Function.ObjectName +  "(" + opname + ")";
                      }
                }else {
                    return m_Function.ObjectName + QString("::") + QString(m_Function.Function->GetName()) + parameters;
                }
            }else {
                return QString(m_Function.Function->GetName())+ parameters;
           }
        }
    }
};


class ToolBrowser : public QWidget
{
        Q_OBJECT;

    ScriptManager * m_ScriptManager;
    DatabaseTool * m_ScriptTool;
    MapViewTool * m_MapViewTool;
    QStringListModel * m_ToolModel;




    QLineEdit * m_SearchEdit;
    QTreeWidgetLSM * m_ToolList;
    QVBoxLayout * m_MainLayout;

    QList<QTreeWidgetItem *> m_ListItems;
    QList<ToolWidget * > m_ListWidgets;
    QList<ScriptFunctionInfo> m_ListFunctions;
    QList<QString> m_Titles;
    QList<QString> m_TitlesS;
public:

    inline ToolBrowser(ScriptManager * sm, DatabaseTool * dbt, MapViewTool * mvt)
    {
        m_MapViewTool = mvt;
        m_ScriptManager = sm;
        m_ScriptTool = dbt;

        m_MainLayout = new QVBoxLayout();
        this->setLayout(m_MainLayout);

        m_SearchEdit = new QLineEdit();

        m_ToolList = new QTreeWidgetLSM();


        m_ToolModel = new QStringListModel();



        m_MainLayout->addWidget(m_SearchEdit);
        m_MainLayout->addWidget(m_ToolList);

        m_ToolList->setAlternatingRowColors(true);

        connect(m_SearchEdit, SIGNAL(textChanged(QString)),this,SLOT(OnSearchChanged(QString)));
        connect(m_ToolList,&QTreeWidget::itemDoubleClicked,this,&ToolBrowser::ItemDoubleClicked);


    }

    inline void SetSearch(QString s)
    {
        m_SearchEdit->setText(s);
    }

    inline void InitializeFunctions(ScriptManager * sm)
    {
        QList<ScriptFunctionInfo> items = sm->m_Engine->GetGlobalFunctionList();
        QList<ScriptFunctionInfo> itemsm = sm->m_Engine->GetMemberFunctionList();
        QStringList op_number_u = m_ScriptManager->m_Engine->GetNumberUnaryOperators();
        QStringList op_number_b = m_ScriptManager->m_Engine->GetNumberBinaryOperators();
        QStringList op_number_c = m_ScriptManager->m_Engine->GetNumberCompareOperators();

        QMap<QString, QTreeWidgetItem*> topLevelItems;

        QStringList categories = {"Global","Operators"};
        categories.append(sm->m_Engine->GetObjectNameList());

        auto skipText = QStringLiteral("skip me");

        QTreeWidgetItem *BaseItem = new QTreeWidgetItem(m_ToolList);
        BaseItem->setText(0, "Functions");
        BaseItem->setData(0, Qt::UserRole, skipText);
        m_ToolList->setHeaderItem(BaseItem);

        //add base items
        for(int i = 0; i < categories.length(); i++)
        {
          QTreeWidgetItem *item = new QTreeWidgetItem(m_ToolList);
          item->setText(0, categories.at(i));
          item->setData(0, Qt::UserRole, skipText);
          topLevelItems[categories.at(i)] = item;
        }

        //add specific items



        for(int i = 0; i < items.length(); i++)
        {
            ScriptFunctionInfo func = items.at(i);

            //QListWidgetItem * item = new QListWidgetItem();
            ToolWidget * w = new ToolWidget(sm,func);

            auto parent = topLevelItems["Global"];
            auto item   = new QTreeWidgetItem(parent);
            item->setText(0, "");//QString(func.Function->GetDeclaration(true,false,true)));
            item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

            m_ListWidgets.append(w);
            m_ListItems.append(item);
            m_ListFunctions.append(func);
            m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
            m_TitlesS.append(QString(func.Function->GetName()));

            m_ToolList->setItemWidget(item,0,w);
        }

        for(int i = 0; i < itemsm.length(); i++)
        {
            ScriptFunctionInfo func = itemsm.at(i);

            //QListWidgetItem * item = new QListWidgetItem();
            ToolWidget * w = new ToolWidget(sm,func);

            auto parent = topLevelItems[func.ObjectName];
            auto item   = new QTreeWidgetItem(parent);
            item->setText(0, "");//QString(func.Function->GetDeclaration(true,false,true)));
            item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

            m_ListWidgets.append(w);
            m_ListItems.append(item);
            m_ListFunctions.append(func);
            m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
            m_TitlesS.append(QString(func.Function->GetName()));
            m_ToolList->setItemWidget(item,0,w);
        }

        QStringList numbertypes = {"int","float","double"};
        for(int i = 0; i < op_number_b.size(); i++)
        {
            for(int j = 0; j < numbertypes.size(); j++)
            {
                for(int k = 0; k < numbertypes.size(); k++)
                {
                    QString object1 = numbertypes.at(j);
                    QString object2 = numbertypes.at(k);

                   ScriptFunctionInfo func;
                   func.is_basetypeoperator = true;
                   func.ObjectName = object1;
                   func.basetypeoperatorname = op_number_b.at(i);
                   func.basetypeoperatorreturntype = k > j? object2: object1;
                   func.basetypeoperatorparametertype = object2;

                   ToolWidget * w = new ToolWidget(sm,func);


                   auto parent = topLevelItems["Operators"];
                   auto item   = new QTreeWidgetItem(parent);
                   item->setText(0, QString(func.Function->GetDeclaration(true,false,true)));
                   item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

                   m_ListWidgets.append(w);
                   m_ListItems.append(item);
                   m_ListFunctions.append(func);
                   m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
                   m_TitlesS.append(QString(func.Function->GetName()));
                   m_ToolList->setItemWidget(item,0,w);

                }

            }
        }

        for(int i = 0; i < op_number_c.size(); i++)
        {
            for(int j = 0; j < numbertypes.size(); j++)
            {
                for(int k = 0; k < numbertypes.size(); k++)
                {
                    QString object1 = numbertypes.at(j);
                    QString object2 = numbertypes.at(k);

                   ScriptFunctionInfo func;
                   func.is_basetypeoperator = true;
                   func.ObjectName = object1;
                   func.basetypeoperatorname = op_number_c.at(i);
                   func.basetypeoperatorreturntype = k > j? object2: object1;
                   func.basetypeoperatorparametertype = "bool";

                   ToolWidget * w = new ToolWidget(sm,func);

                   auto parent = topLevelItems["Operators"];
                   auto item   = new QTreeWidgetItem(parent);
                   item->setText(0, QString(func.Function->GetDeclaration(true,false,true)));
                   item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

                   m_ListWidgets.append(w);
                   m_ListItems.append(item);
                   m_ListFunctions.append(func);
                   m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
                   m_TitlesS.append(QString(func.Function->GetName()));
                   m_ToolList->setItemWidget(item,0,w);

                }

            }
        }

        for(int i = 0; i < op_number_u.size(); i++)
        {
            for(int j = 0; j < numbertypes.size(); j++)
            {
                    QString object1 = numbertypes.at(j);

                   ScriptFunctionInfo func;
                   func.is_basetypeoperator = true;
                   func.ObjectName = object1;
                   func.basetypeoperatorname = op_number_u.at(i);
                   func.basetypeoperatorreturntype = object1;
                   func.basetypeoperatorparametertype = QString();


                   ToolWidget * w = new ToolWidget(sm,func);

                   auto parent = topLevelItems["Operators"];
                   auto item   = new QTreeWidgetItem(parent);
                   item->setText(0, QString(func.Function->GetDeclaration(true,false,true)));
                   item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

                   m_ListWidgets.append(w);
                   m_ListItems.append(item);
                   m_ListFunctions.append(func);
                   m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
                   m_TitlesS.append(QString(func.Function->GetName()));
                   m_ToolList->setItemWidget(item,0,w);

            }
        }


        //set source functions into the registry
        QList<ScriptFunctionInfo> itemsc = sm->m_Engine->GetConstructorFunctionList();
        for(int i = 0; i < itemsc.length(); i++)
        {
            QString name = QString(itemsc.at(i).Function->GetName());

           ScriptFunctionInfo func = itemsc.at(i);

           ToolWidget * w = new ToolWidget(sm,func);

           auto parent = topLevelItems[func.ObjectName];
           auto item   = new QTreeWidgetItem(parent);
           item->setText(0, QString(func.Function->GetDeclaration(true,false,true)));
           item->setData(0, Qt::UserRole, QString(func.Function->GetDeclaration(true,false,true)));

           m_ListWidgets.append(w);
           m_ListItems.append(item);
           m_ListFunctions.append(func);
           m_Titles.append(QString(func.Function->GetDeclaration(true,false,true)));
           m_TitlesS.append(QString(func.Function->GetName()));
           m_ToolList->setItemWidget(item,0,w);

        }


        m_ToolList->expandAll();

    }

public slots:


    inline void OnSearchChanged(QString t)
    {
        if(t.isEmpty())
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                m_ToolList->setItemHidden(m_ListItems.at(i),false);
                m_ListWidgets.at(i)->setHighlight(false);
            }
        }else
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                bool find = m_Titles.at(i).contains(t,Qt::CaseSensitivity::CaseInsensitive);

                m_ToolList->setItemHidden(m_ListItems.at(i),!find);

                if(m_TitlesS.at(i).compare(t,Qt::CaseSensitivity::CaseInsensitive) == 0)
                {
                    m_ListItems.at(i)->setHidden(true);

                    m_ListWidgets.at(i)->setHighlight(true);
                    m_ListItems.at(i)->setSizeHint(0,QSize(1000.0,m_ListWidgets.at(i)->description->size().height()));

                    m_ListItems.at(i)->setHidden(false);


                }else
                {
                    m_ListItems.at(i)->setSizeHint(0,QSize(-1,-1));
                    m_ListWidgets.at(i)->setHighlight(false);
                }
            }
        }

        this->m_ToolList->update();
        this->m_ToolList->repaint();
        this->m_ToolList->do_layout_update();
    }

    inline void ItemDoubleClicked(QTreeWidgetItem * i)
    {
        int index = m_ListItems.indexOf(i);
        if(index > -1)
        {
            ToolDialog * dl = new ToolDialog(m_ScriptManager,m_MapViewTool,m_ListFunctions.at(index));
            dl->exec();
        }

    }



};

