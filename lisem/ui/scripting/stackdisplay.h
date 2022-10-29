#ifndef STACKDISPLAY_H
#define STACKDISPLAY_H

#include "QWidget"
#include "angelscript.h"
#include "QListView"
#include "QTableView"
#include "QTreeView"
#include "QSplitter"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>


#include <QVariant>
#include <QVector>
#include "QVBoxLayout"
#include "QMutex"
#include "QModelIndex"
#include "iostream"
#include "QListWidget"
#include "lsmscriptengine.h"
#include "geo/raster/map.h"
#include "QMenu"

class SDTreeItem
{
public:
    explicit SDTreeItem(const QVector<QVariant> &data, void * var = nullptr, std::vector<std::pair<QString,std::function<void(void*)>>> actions = {},SDTreeItem *parent = nullptr);
    ~SDTreeItem();

    SDTreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(SDTreeItem*item);
    SDTreeItem *parent();
    int childNumber() const;
    bool setData(int column, const QVariant &value);

public:
    void * m_Variable;
    std::vector<std::pair<QString,std::function<void(void*)>>> m_Actions;
    QVector<SDTreeItem*> childItems;
    QVector<QVariant> itemData;
    SDTreeItem *parentItem;
};



class SDTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SDTreeModel(
              QObject *parent = nullptr);
    ~SDTreeModel();

    void SetRoot(SDTreeItem * Root);

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    SDTreeItem *getItem(const QModelIndex &index) const;
private:
    void setupModelData(const QStringList &lines, SDTreeItem *parent);


    SDTreeItem *rootItem;
};



class StackDisplay : public QWidget
{
    Q_OBJECT

public:


    QSplitter * m_Splitter;
    QListWidget * m_StackView;
    QTreeView * m_VariableView;

    SDTreeModel * m_VariableTree;
    QStringList m_CallStackList;

    asIScriptModule * m_Module = nullptr;
    QMutex m_Mutex;
    SDTreeItem * m_RootItem = nullptr;
    int m_CurrentStack = -1;

    asIScriptContext * m_Context =nullptr;

    inline StackDisplay(QWidget * parent = 0) : QWidget(parent)
    {

        m_Splitter = new QSplitter();
        m_Splitter->setOrientation(Qt::Vertical);

        //upper (callstack display)
        m_StackView = new QListWidget();
        connect(m_StackView,&QListView::clicked,[this](const QModelIndex &index)
        {
            if(m_Context != nullptr)
            {
                SetFromContext(m_Context, index.row(), false);
            }

        });

        //lower (variable display)
        //changes contents when a callstack level is selected from the upper debug display
        m_VariableView = new QTreeView();
        m_VariableView->setContextMenuPolicy(Qt::CustomContextMenu);
        m_VariableTree = new SDTreeModel();

        connect(m_VariableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

        QVector<QVariant> headers = {"Variable","Type","Value"};
        SDTreeItem * root = new SDTreeItem(headers);

        m_VariableTree->SetRoot(root);

        m_VariableView->setModel(m_VariableTree);


        m_Splitter->addWidget(m_StackView);
        m_Splitter->addWidget(m_VariableView);

        QVBoxLayout * m_Layout = new QVBoxLayout();
        m_Layout->addWidget(m_Splitter);
        this->setLayout(m_Layout);

        connect(this,&StackDisplay::OnDataChanged,this,&StackDisplay::OnUpdateVariableTree,Qt::QueuedConnection);


    }


    inline void SetFromContext(asIScriptContext *ctx, int stack, bool reset)
    {
        m_Mutex.lock();

        m_Context = ctx;
        if(reset)
        {

            m_CallStackList.clear();
            m_Module = nullptr;


            m_CallStackList.append("global");

            // Show the call stack
            for( asUINT n = 0; n < ctx->GetCallstackSize(); n++ )
            {
              asIScriptFunction *func;
              const char *scriptSection;
              int line, column;
              func = ctx->GetFunction(n);
              if(m_Module == nullptr)
              {
                  m_Module = func->GetModule();
              }
              line = ctx->GetLineNumber(n, &column, &scriptSection);

              m_CallStackList.append("l:"+ QString::number(line)+ " c:" + QString::number(column)+ " s:" + scriptSection + "   " + QString(func->GetDeclaration()));
            }

            m_StackView->clear();
            m_StackView->addItems(m_CallStackList);
        }

        int do_global = false;
        if(stack == 0)
        {
            do_global = true;



        }
        stack = stack - 1;
        if((m_CurrentStack != stack) || reset)
        {

            //special stack (global variables)
            if(stack == -1)
            {


                m_CurrentStack = stack;
                //get the module that this context is within
                QVector<QVariant> headers = {"Variable","Type","Value"};
                SDTreeItem * root = new SDTreeItem(headers);

                std::vector<SDTreeItem*> Roots;

                asIScriptEngine *engine = ctx->GetEngine();



                if(m_Module != nullptr)
                {
                    AddGlobalScriptItemsToTree(Roots,nullptr,0,nullptr,m_Module,engine,nullptr);

                }


                for(int i = 0; i < Roots.size(); i++)
                {
                    root->insertChildren(Roots.at(i));
                }
                m_RootItem = root;
                emit OnDataChanged();
            }else if(stack > -1 && stack < ctx->GetCallstackSize())//m_CallStackList.length())
            {

                m_CurrentStack = stack;
                int stackLevel = stack;
                asIScriptEngine *engine = ctx->GetEngine();


                std::vector<SDTreeItem*> Roots;
                AddScriptItemsToTree(Roots,nullptr,0,nullptr,ctx,stackLevel,engine,nullptr);

                QVector<QVariant> headers = {"Variable","Type","Value"};
                SDTreeItem * root = new SDTreeItem(headers);

                for(int i = 0; i < Roots.size(); i++)
                {
                    root->insertChildren(Roots.at(i));
                }
                m_RootItem = root;
                emit OnDataChanged();

            }else {

            }

        }

        m_Mutex.unlock();

    }

    inline void AddScriptItemsToTree(std::vector<SDTreeItem*> &Roots, SDTreeItem * parent, int p_typeId, void*p_varPointer, asIScriptContext *ctx, int stacklevel, asIScriptEngine *engine, QVector<void*> *data_done)
    {


        if(parent == nullptr)
        {
            QVector<void*> data_doner;

            //get all base items
            int typeId = ctx->GetThisTypeId(stacklevel);

            void *varPointer = ctx->GetThisPointer(stacklevel);
            if( typeId )
            {

                SDTreeItem * item = GetTreeItem(typeId, varPointer,ctx,stacklevel,engine, "this");
                data_doner.push_back(varPointer);
                Roots.push_back(item);
            }


            int numVars = ctx->GetVarCount(stacklevel);
            for( int n = 0; n < numVars; n++ )
            {

              int typeId = ctx->GetVarTypeId(n, stacklevel);
              void *varPointer = ctx->GetAddressOfVar(n, stacklevel);

              QString name = QString(ctx->GetVarDeclaration(n,stacklevel));


              if( typeId )
              {

                  SDTreeItem * item = GetTreeItem(typeId, varPointer,ctx,stacklevel,engine,name);

                  data_doner.push_back(varPointer);
                  Roots.push_back(item);

                  if( typeId & asTYPEID_SCRIPTOBJECT )
                  {


                        AddScriptItemsToTree(Roots,item,typeId,varPointer,ctx,stacklevel,engine,&data_doner);

                  }

              }
            }

        }else {

            //get all sub items for parentasITypeInfo
            asITypeInfo* info = engine->GetTypeInfoById(p_typeId);
            if(info != nullptr)
            {
                int propcount =  info->GetPropertyCount();


                for(int i = 0; i < propcount; i++)
                {
                    const char ** name;
                    int typeId;
                    bool is_private;
                    bool is_protected;
                    int offset;
                    bool is_reference;
                    info->GetProperty(i,name,&typeId,&is_private,&is_protected,&offset,&is_reference);

                    SDTreeItem * item = GetTreeItem(typeId,(void*)((char*)p_varPointer + offset),ctx,stacklevel,engine,"prop");
                    parent->insertChildren(item);

                    if( typeId )
                    {

                        if(!(data_done->contains((void*)((char*)p_varPointer + offset))))
                        {
                            data_done->push_back((void*)((char*)p_varPointer + offset));

                            if( typeId & asTYPEID_SCRIPTOBJECT )
                            {
                                  AddScriptItemsToTree(Roots,item,typeId,(void*)((char*)p_varPointer + offset),ctx,stacklevel,engine,data_done);
                            }
                        }


                    }
                }
            }
        }
    }

    inline void AddGlobalScriptItemsToTree(std::vector<SDTreeItem*> &Roots, SDTreeItem * parent, int p_typeId, void*p_varPointer, asIScriptModule *ctx, asIScriptEngine *engine, QVector<void*> *data_done)
    {


        if(parent == nullptr)
        {
            QVector<void*> data_doner;




            int numVars = ctx->GetGlobalVarCount();
            for( int n = 0; n < numVars; n++ )
            {

              int typeId = -1;
              const char *x;
              int rtid =ctx->GetGlobalVar(n,&x,0,&typeId);
              if(typeId == -1 || rtid < 0)
              {
                  continue;
              }

              void *varPointer = ctx->GetAddressOfGlobalVar(n);

              QString name = QString(ctx->GetGlobalVarDeclaration(n));


              if( typeId )
              {

                  SDTreeItem * item = GetTreeItem(typeId, varPointer,nullptr,-1,engine,name);

                  data_doner.push_back(varPointer);
                  Roots.push_back(item);

                  if( typeId & asTYPEID_SCRIPTOBJECT )
                  {


                        AddGlobalScriptItemsToTree(Roots,item,typeId,varPointer,ctx,engine,&data_doner);

                  }

              }
            }

        }else {

            //get all sub items for parentasITypeInfo
            asITypeInfo* info = engine->GetTypeInfoById(p_typeId);
            if(info != nullptr)
            {
                int propcount =  info->GetPropertyCount();


                for(int i = 0; i < propcount; i++)
                {
                    const char ** name;
                    int typeId;
                    bool is_private;
                    bool is_protected;
                    int offset;
                    bool is_reference;
                    info->GetProperty(i,name,&typeId,&is_private,&is_protected,&offset,&is_reference);

                    SDTreeItem * item = GetTreeItem(typeId,(void*)((char*)p_varPointer + offset),nullptr,-1,engine,"prop");
                    parent->insertChildren(item);

                    if( typeId )
                    {

                        if(!(data_done->contains((void*)((char*)p_varPointer + offset))))
                        {
                            data_done->push_back((void*)((char*)p_varPointer + offset));

                            if( typeId & asTYPEID_SCRIPTOBJECT )
                            {
                                  AddGlobalScriptItemsToTree(Roots,item,typeId,(void*)((char*)p_varPointer + offset),ctx,engine,data_done);
                            }
                        }


                    }
                }
            }
        }
    }

    inline SDTreeItem * GetTreeItem(int typeId,void *varPointer,asIScriptContext *ctx, int stacklevel, asIScriptEngine *engine, QString declarename)
    {


        LSMScriptEngine * sphengine = (LSMScriptEngine*)(engine->GetUserData());

        QString name = sphengine->GetTypeName(typeId);

        QString value = "{...}";


        int typeId_l = typeId;
        asITypeInfo * info = sphengine->GetTypeInfoById(typeId);
        if(info)
        {
            asITypeInfo * info_b = sphengine->GetTypeInfoByName(sphengine->GetTypeInfoById(typeId)->GetName());

            if(info_b)
            {
                typeId_l = info_b->GetTypeId();
            }else {

                typeId_l = typeId;
            }
        }

        value = sphengine->GetTypeDebugValue(typeId,varPointer);

        if(value == "{...}")
        {
            QString attempt2 = sphengine->GetTypeDebugValue(typeId_l,varPointer);
            if(attempt2 != "{...}")
            {
                value = attempt2;
            }
        }


        QVector<QVariant> data;
        data.push_back(declarename);
        data.push_back(name);
        data.push_back(value);


        std::cout << "b4 " << declarename.toStdString() << "  " << name.toStdString() << " " << value.toStdString() << std::endl;



        SDTreeItem * ret = new SDTreeItem(data,varPointer,sphengine->GetTypeDebugActions(typeId));

            if(sphengine->GetTypeIsDebugList(typeId_l))
            {
                std::vector<int> x =  sphengine->GetTypeDebugListDims(typeId_l,varPointer);
                int type_id_l = sphengine->GetTypeDebugListType(typeId_l,varPointer);

                if(x.size() > 0 )
                {
                    if(x.at(0) > 0)
                    {
                        std::vector<int> index;
                        std::vector<int> index_max;

                        int max_listitems = std::max(2.0,std::pow(100.0,1.0/((double)(x.size()))));
                        int n_total = 0;
                        for(int i = 0; i < x.size(); i++)
                        {
                            index.push_back(0);
                            n_total = n_total * x.at(i);
                            index_max.push_back(std::min(max_listitems-1,x.at(i)-1));
                        }

                        bool is_next = true;
                        int n = 0;


                        while(is_next)
                        {
                            //get element

                            void * var_index = sphengine->GetTypeDebugListGet(typeId_l,varPointer,index);

                            QString name_l = sphengine->GetTypeName(type_id_l);//declarename;

                            for(int i = 0; i < index.size(); i++)
                            {
                                name_l += "[" + QString::number(index.at(i)) + "]";
                            }

                            SDTreeItem * item_l = GetTreeItem(type_id_l,var_index,ctx, stacklevel, engine, name_l);
                            ret->insertChildren(item_l);

                            n++;

                            //increment
                            index.at(0) += 1;

                            //check if we are above bounds for all
                            for(int i = 0; i < index.size(); i++)
                            {
                                if(index.at(i) > index_max.at(i))
                                {
                                    index.at(i) = 0;
                                    if(i != index.size()-1)
                                    {
                                        index.at(i+1) += 1;
                                    }else {
                                        //finally the last dimension reaches its final index, so break out of the while loop
                                        is_next = false;
                                        break;
                                    }
                                }
                            }
                        }

                        if(n_total - n > 0)
                        {
                            QString name_l = sphengine->GetTypeName(type_id_l);//declarename;

                            SDTreeItem * item_lrem = new SDTreeItem({"...["+ QString::number(n_total - n) + " items]",name_l,""});
                            ret->insertChildren(item_lrem);
                        }
                    }

                }
            }


        return ret;
    }

    inline void Clear()
    {

        m_Mutex.lock();


        m_StackView->clear();
        QVector<QVariant> headers = {"Variable","Type","Value"};
        SDTreeItem * root = new SDTreeItem(headers);

        m_RootItem = root;

        emit OnDataChanged();

        m_Mutex.unlock();

    }


    inline void RegisterDataOpenBehavior()
    {

    }

    inline void RegisterDataSaveBehavior()
    {


    }

    inline void PrintCallstack(asIScriptContext *ctx)
    {

    }

    inline void PrintVariables(asIScriptContext *ctx, asUINT stackLevel)
    {

    }

private slots:

    inline void OnUpdateVariableTree()
    {

        m_Mutex.lock();

        SDTreeModel * old = m_VariableTree;
        m_VariableTree = new SDTreeModel();
        m_VariableTree->SetRoot(m_RootItem);
        m_VariableView->setModel(m_VariableTree);

        //old->deleteLater();
        m_Mutex.unlock();


    }


    inline void onCustomContextMenu(const QPoint &p)
    {

        QModelIndex index = m_VariableView->indexAt(p);
        if (index.isValid()) {

            SDTreeItem* item = m_VariableTree->getItem(index);

            if(item->m_Actions.size()> 0)
            {

                QMenu menu;


                for(int i = 0; i < item->m_Actions.size(); i++)
                {
                    menu.addAction(item->m_Actions.at(i).first);
                }
                QAction* selAct = menu.exec(m_VariableView->mapToGlobal(p));
                if(0 != selAct){

                    for(int i = 0; i < item->m_Actions.size(); i++)
                    {
                        if(selAct->text() == item->m_Actions.at(i).first)
                        {
                            item->m_Actions.at(i).second(item->m_Variable);
                        }

                    }
                }
            }
        }
    }

signals:

    void OnDataChanged();

};




#endif // STACKDISPLAY_H
