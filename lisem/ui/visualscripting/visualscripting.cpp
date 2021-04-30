#include "visualscripting.h"


#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>
#include <nodes/internal/NodeGraphicsObject.hpp>
#include <nodes/ConnectionStyle>


#include "widgets/spoilerwidget.h"
#include "scripting/scripttool.h"
#include "mapfunctionmodel.h"
#include "variablefunctionmodel.h"

#include "programflowmodel.h"
#include "conditionalfunctionmodel.h"
#include "loopfunctionmodel.h"
#include "numbersourcemodel.h"
#include "QThread.h"
#include "site.h"

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::TypeConverterId;

VisualScriptingTool::VisualScriptingTool(ScriptManager * sm,QWidget * parent): QWidget(parent)
{

    setMouseTracking(true);
    popup = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout;
    popupLabel = new QLabel(popup);
    popupLabel->setWordWrap(true);
    layout->addWidget(popupLabel);
    popupLabel->setTextFormat(Qt::RichText);
    //popupLabel->setOpenExternalLinks(true);
    popup->setLayout(layout);
    popup->installEventFilter(this);

    m_Dir = GetSite();

    QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
    {
      "ConnectionStyle": {
        "UseDataDefinedColors": true
      }
    }
    )");


    m_CodeIsRunning.store(false);
    m_CodeIsPaused.store(false);
    m_CodeIsStopRequested.store(false);
    m_CodeIsPauseRequested.store(false);


    QList<QString> m_Types;

    m_Types.append("Map");
    m_Types.append("GeoProjection");
    m_Types.append("Shapes");
    m_Types.append("Table");
    m_Types.append("PointCloud");
    m_Types.append("MapArray");
    m_Types.append("BoundingBox");
    m_Types.append("Point");
    m_Types.append("bool");
    m_Types.append("double");
    m_Types.append("float");
    m_Types.append("int");
    m_Types.append("string");


    m_ScriptManager = sm;

    QWidget *mainWidget = this;

    menuBar    = new QMenuBar();
    compileAction = menuBar->addAction("Compile");
    menuBar->addSeparator();
    runAction = menuBar->addAction("Run");
    pauseAction = menuBar->addAction("Pause");
    stopAction = menuBar->addAction("Stop");
    menuBar->addSeparator();
    saveAction = menuBar->addAction("Save");
    saveasAction = menuBar->addAction("Save as");
    loadAction = menuBar->addAction("Load");
    newAction = menuBar->addAction("New");
    menuBar->addSeparator();
    previewAction = menuBar->addAction("Preview On/Off");

    connect(compileAction,&QAction::triggered,this,&VisualScriptingTool::OnCompile);
    connect(runAction,&QAction::triggered,this,&VisualScriptingTool::OnRun);
    connect(pauseAction,&QAction::triggered,this,&VisualScriptingTool::OnPause);
    connect(stopAction,&QAction::triggered,this,&VisualScriptingTool::OnStop);
    connect(saveAction,&QAction::triggered,this,&VisualScriptingTool::OnSave);
    connect(saveasAction,&QAction::triggered,this,&VisualScriptingTool::OnSaveas);
    connect(loadAction,&QAction::triggered,this,&VisualScriptingTool::OnLoad);
    connect(newAction,&QAction::triggered,this,&VisualScriptingTool::OnNew);
    connect(previewAction,&QAction::triggered,this,&VisualScriptingTool::OnPreviewToggle);


    m_LabelRunning = new QLabel();
    QMovie *movie = new QMovie(m_Dir + LISEM_FOLDER_ASSETS + "loader_1.gif");
    m_LabelRunning ->setMovie(movie);
    movie->start();
    m_LabelRunning->setVisible(false);
    layout->addWidget(m_LabelRunning);
    runAction->setEnabled(true);
    pauseAction->setEnabled(false);
    stopAction->setEnabled(false);

    connect(this,&VisualScriptingTool::int_update_nodedone,this,&VisualScriptingTool::OnNodeDone,Qt::ConnectionType::QueuedConnection);


    connect(this,&VisualScriptingTool::int_update_buttons_stop,this,&VisualScriptingTool::int_buttons_onstop,Qt::ConnectionType::QueuedConnection);
    connect(this,&VisualScriptingTool::int_update_buttons_start,this,&VisualScriptingTool::int_buttons_onstart,Qt::ConnectionType::QueuedConnection);
    connect(this,&VisualScriptingTool::int_update_buttons_pause,this,&VisualScriptingTool::int_buttons_onpause,Qt::ConnectionType::QueuedConnection);

    connect(this,&VisualScriptingTool::int_emit_show_dialog,this,&VisualScriptingTool::int_show_dialog,Qt::ConnectionType::QueuedConnection);
    connect(this,&VisualScriptingTool::int_emit_show_temporary_dialog,this,&VisualScriptingTool::int_show_temporary_dialog,Qt::ConnectionType::QueuedConnection);
    connect(this,&VisualScriptingTool::int_emit_hide_dialog,this,&VisualScriptingTool::int_hide_dialog,Qt::ConnectionType::QueuedConnection);

    connect(this,&VisualScriptingTool::int_emit_previewdataset,this,&VisualScriptingTool::SetPreviewData,Qt::ConnectionType::QueuedConnection);
    connect(this,&VisualScriptingTool::int_emit_signal_node_preview,this,&VisualScriptingTool::call_signal_node_preview,Qt::ConnectionType::QueuedConnection);



    QVBoxLayout *l = new QVBoxLayout();
    this->setLayout(l);

    l->addWidget(menuBar);
    m_Scene = new FlowScene();


    m_Scene->connect(m_Scene,&FlowScene::nodePlaced,[this](QtNodes::Node & n){std::cout << "x" << std::endl; OnNodePlaced(n);});
    m_Scene->connect(m_Scene,&FlowScene::nodeDeleted,this,&VisualScriptingTool::OnNodeDeleted);
    m_Scene->connect(m_Scene,&FlowScene::connectionCreated,this,&VisualScriptingTool::connectionCreated);
    m_Scene->connect(m_Scene,&FlowScene::connectionDeleted,this,&VisualScriptingTool::connectionDeleted);


    //get all the global functions into the registry
    std::shared_ptr<DataModelRegistry> ret = std::make_shared<DataModelRegistry>();

    QList<ScriptFunctionInfo> items = sm->m_Engine->GetGlobalFunctionList();

    for(int i = 0; i < items.length(); i++)
    {
        QString name = QString(items.at(i).Function->GetName());

       ScriptFunctionInfo func = items.at(i);
       std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
               [func,name,sm]()
       {
           return std::make_unique<MapFunctionModel>(func,sm);
       };
       ret->registerModel<MapFunctionModel>("Global Functions",lambda);

    }

    //set all the member functions
    QList<ScriptFunctionInfo> itemsm = sm->m_Engine->GetMemberFunctionList();
    for(int i = 0; i < itemsm.length(); i++)
    {
        QString name = QString(itemsm.at(i).Function->GetName());
        if(!(name.startsWith("op") && name.endsWith("Assign")))
        {

            if(!m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(name).isEmpty())
            {
                ScriptFunctionInfo func = itemsm.at(i);
                std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
                        [func,name,sm]()
                {
                    return std::make_unique<MapFunctionModel>(func,sm);
                };
                ret->registerModel<MapFunctionModel>("Operators",lambda);
            }else
            {
                ScriptFunctionInfo func = itemsm.at(i);
                std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
                        [func,name,sm]()
                {
                    return std::make_unique<MapFunctionModel>(func,sm);
                };
                ret->registerModel<MapFunctionModel>("Member Functions",lambda);
            }

        }

    }

    //add operators for number types

    QStringList op_number_u = m_ScriptManager->m_Engine->GetNumberUnaryOperators();
    QStringList op_number_b = m_ScriptManager->m_Engine->GetNumberBinaryOperators();
    QStringList op_number_c = m_ScriptManager->m_Engine->GetNumberCompareOperators();

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

               std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
                       [func,sm]()
               {
                   return std::make_unique<MapFunctionModel>(func,sm);
               };
               ret->registerModel<MapFunctionModel>("Operators",lambda);
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

               std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
                       [func,sm]()
               {
                   return std::make_unique<MapFunctionModel>(func,sm);
               };
               ret->registerModel<MapFunctionModel>("Operators",lambda);
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

               std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
                       [func,sm]()
               {
                   return std::make_unique<MapFunctionModel>(func,sm);
               };
               ret->registerModel<MapFunctionModel>("Operators",lambda);


        }
    }



    //add operators for bool types

    QStringList op_bool_u = m_ScriptManager->m_Engine->GetBoolUnaryOperators();
    QStringList op_bool_b = m_ScriptManager->m_Engine->GetBoolBinaryOperators();


    for(int i = 0; i < op_bool_b.size(); i++)
    {
       QString object1 = "bool";
       QString object2 = "bool";

       ScriptFunctionInfo func;
       func.is_basetypeoperator = true;
       func.ObjectName = object1;
       func.basetypeoperatorname = op_number_c.at(i);
       func.basetypeoperatorreturntype = "bool";
       func.basetypeoperatorparametertype = "bool";

       std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
               [func,sm]()
       {
           return std::make_unique<MapFunctionModel>(func,sm);
       };
       ret->registerModel<MapFunctionModel>("Operators",lambda);
    }

    for(int i = 0; i < op_bool_u.size(); i++)
    {
        QString object1 = "bool";

       ScriptFunctionInfo func;
       func.is_basetypeoperator = true;
       func.ObjectName = object1;
       func.basetypeoperatorname = op_number_u.at(i);
       func.basetypeoperatorreturntype = object1;
       func.basetypeoperatorparametertype = QString();

       std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
               [func,sm]()
       {
           return std::make_unique<MapFunctionModel>(func,sm);
       };
       ret->registerModel<MapFunctionModel>("Operators",lambda);
    }


    //set source functions into the registry
    QList<ScriptFunctionInfo> itemsc = sm->m_Engine->GetConstructorFunctionList();
    for(int i = 0; i < itemsc.length(); i++)
    {
       QString name = QString(itemsc.at(i).Function->GetName());

       ScriptFunctionInfo func = itemsc.at(i);
       std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
               [func,name,sm]()
       {
           return std::make_unique<MapFunctionModel>(func,sm);
       };
       ret->registerModel<MapFunctionModel>("Constructors",lambda);
    }


    std::function<void(VariableFunctionModel *)> lambdaupdate =
            [l,this](VariableFunctionModel *nm)
    {

        std::vector<QtNodes::Node*> nodes=  m_Scene->allNodes();
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node*n = nodes.at(i);
            //check if this is the node we are talking about
            if(n->nodeDataModel() == nm)
            {
                QPointF point = n->nodeGraphicsObject().pos();

                QList<QString> Types;

                Types.append("Map");
                Types.append("GeoProjection");
                Types.append("Shapes");
                Types.append("Table");
                Types.append("PointCloud");
                Types.append("MapArray");
                Types.append("BoundingBox");
                Types.append("Point");
                Types.append("bool");
                Types.append("double");
                Types.append("float");
                Types.append("int");
                Types.append("string");


                std::unique_ptr<QtNodes::NodeDataModel> p = std::make_unique<VariableFunctionModel>(Types,nm->m_FuncReset,nm->m_IndexType);
                QtNodes::Node &added = m_Scene->createNode(std::move(p));
                added.nodeGraphicsObject().setPos(point);

                m_Scene->removeNode(*n);

                break;
            }
        }
    };

    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda =
            [m_Types,lambdaupdate]()
    {
        return std::make_unique<VariableFunctionModel>(m_Types,lambdaupdate);
    };
    ret->registerModel<VariableFunctionModel>("Variable",lambda);


    std::function<void(ConditionalFunctionModel *)> lambdaupdate2 =
            [l,this](ConditionalFunctionModel *nm)
    {

        std::vector<QtNodes::Node*> nodes=  m_Scene->allNodes();
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node*n = nodes.at(i);
            //check if this is the node we are talking about
            if(n->nodeDataModel() == nm)
            {
                QPointF point = n->nodeGraphicsObject().pos();

                QList<QString> Types;

                Types.append("Map");
                Types.append("GeoProjection");
                Types.append("Shapes");
                Types.append("Table");
                Types.append("PointCloud");
                Types.append("MapArray");
                Types.append("BoundingBox");
                Types.append("Point");
                Types.append("bool");
                Types.append("double");
                Types.append("float");
                Types.append("int");
                Types.append("string");

                std::unique_ptr<QtNodes::NodeDataModel> p = std::make_unique<ConditionalFunctionModel>(Types,nm->m_FuncReset,nm->m_IndexType);
                QtNodes::Node &added = m_Scene->createNode(std::move(p));
                added.nodeGraphicsObject().setPos(point);

                m_Scene->removeNode(*n);

                break;
            }
        }
    };


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda2 =
            [m_Types,lambdaupdate2]()
    {
        return std::make_unique<ConditionalFunctionModel>(m_Types,lambdaupdate2);
    };
    ret->registerModel<ConditionalFunctionModel>("Program Flow",lambda2);


    std::function<void(WhileFunctionModel *)> lambdaupdate3 =
            [l,this](WhileFunctionModel *nm)
    {

        std::vector<QtNodes::Node*> nodes=  m_Scene->allNodes();
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node*n = nodes.at(i);
            //check if this is the node we are talking about
            if(n->nodeDataModel() == nm)
            {
                QPointF point = n->nodeGraphicsObject().pos();

                QList<QString> Types;

                Types.append("Map");
                Types.append("GeoProjection");
                Types.append("Shapes");
                Types.append("Table");
                Types.append("PointCloud");
                Types.append("MapArray");
                Types.append("BoundingBox");
                Types.append("Point");
                Types.append("bool");
                Types.append("double");
                Types.append("float");
                Types.append("int");
                Types.append("string");



                std::unique_ptr<QtNodes::NodeDataModel> p = std::make_unique<WhileFunctionModel>(Types,nm->m_FuncReset,nm->m_Count,nm->m_IndexTypes);
                QtNodes::Node &added = m_Scene->createNode(std::move(p));
                added.nodeGraphicsObject().setPos(point);

                m_Scene->removeNode(*n);

                break;
            }
        }
    };



    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda3 =
            [m_Types,lambdaupdate3]()
    {
        return std::make_unique<WhileFunctionModel>(m_Types,lambdaupdate3,0,QList<int>());
    };
    ret->registerModel<WhileFunctionModel>("Program Flow",lambda3);


    std::function<void(LoopFunctionModel *)> lambdaupdate4 =
            [l,this](LoopFunctionModel *nm)
    {

        std::vector<QtNodes::Node*> nodes=  m_Scene->allNodes();
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node*n = nodes.at(i);
            //check if this is the node we are talking about
            if(n->nodeDataModel() == nm)
            {
                QPointF point = n->nodeGraphicsObject().pos();

                QList<QString> Types;

                Types.append("Map");
                Types.append("GeoProjection");
                Types.append("Shapes");
                Types.append("Table");
                Types.append("PointCloud");
                Types.append("MapArray");
                Types.append("BoundingBox");
                Types.append("Point");
                Types.append("bool");
                Types.append("double");
                Types.append("float");
                Types.append("int");
                Types.append("string");


                std::unique_ptr<QtNodes::NodeDataModel> p = std::make_unique<LoopFunctionModel>(Types,nm->m_FuncReset,nm->m_Count,nm->m_IndexTypes, nm->m_Iterations);

                QtNodes::Node &added = m_Scene->createNode(std::move(p));
                added.nodeGraphicsObject().setPos(point);
                m_Scene->removeNode(*n);
                break;
            }
        }
    };


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda4 =
            [m_Types,lambdaupdate4]()
    {
        return std::make_unique<LoopFunctionModel>(m_Types,lambdaupdate4,0,QList<int>(),1);
    };
    ret->registerModel<LoopFunctionModel>("Program Flow",lambda4);


    //source models


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda5 =
            []()
    {
        return std::make_unique<StringSourceModel>();
    };
    ret->registerModel<StringSourceModel>("Source",lambda5);



    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda6 =
            []()
    {
        return std::make_unique<IntSourceModel>();
    };
    ret->registerModel<IntSourceModel>("Source",lambda6);


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda7 =
            []()
    {
        return std::make_unique<DoubleSourceModel>();
    };
    ret->registerModel<DoubleSourceModel>("Source",lambda7);


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda8 =
            []()
    {
        return std::make_unique<BoolSourceModel>();
    };
    ret->registerModel<BoolSourceModel>("Source",lambda8);



    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda9 =
            []()
    {
        return std::make_unique<MapSourceModel>();
    };
    ret->registerModel<MapSourceModel>("Source",lambda9);

    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda10 =
            []()
    {
        return std::make_unique<ShapesSourceModel>();
    };
    ret->registerModel<ShapesSourceModel>("Source",lambda10);

    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda11 =
            []()
    {
        return std::make_unique<TableSourceModel>();
    };
    ret->registerModel<TableSourceModel>("Source",lambda11);

    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda12 =
            []()
    {
        return std::make_unique<PointCloudSourceModel>();
    };
    ret->registerModel<PointCloudSourceModel>("Source",lambda12);


    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda13 =
            []()
    {
        return std::make_unique<MapArraySourceModel>();
    };
    ret->registerModel<MapArraySourceModel>("Source",lambda13);

    std::function<QtNodes::DataModelRegistry::RegistryItemPtr()> lambda14 =
            []()
    {
        return std::make_unique<GeoProjectionSourceModel>();
    };
    ret->registerModel<GeoProjectionSourceModel>("Source",lambda14);

    ret->registerTypeConverter(std::make_pair(boolData().type(),
                                              intData().type()),
                               QtNodes::TypeConverter{BoolToIntConverter()});

    ret->registerTypeConverter(std::make_pair(doubleData().type(),
                                              intData().type()),
                               QtNodes::TypeConverter{DoubleToIntConverter()});

    ret->registerTypeConverter(std::make_pair(floatData().type(),
                                              intData().type()),
                               QtNodes::TypeConverter{FloatToIntConverter()});

    ret->registerTypeConverter(std::make_pair(intData().type(),
                                              floatData().type()),
                               QtNodes::TypeConverter{IntToFloatConverter()});

    ret->registerTypeConverter(std::make_pair(doubleData().type(),
                                              floatData().type()),
                               QtNodes::TypeConverter{DoubleToFloatConverter()});

    ret->registerTypeConverter(std::make_pair(floatData().type(),
                                              doubleData().type()),
                               QtNodes::TypeConverter{FloatToDoubleConverter()});

    ret->registerTypeConverter(std::make_pair(intData().type(),
                                              doubleData().type()),
                               QtNodes::TypeConverter{IntToDoubleConverter()});

    std::unordered_map<QString, std::function<std::unique_ptr<QtNodes::NodeDataModel>()>> map = ret->registeredModelCreators();

    std::vector<QString> keys;
    keys.reserve(map.size());

    for(auto kv : map) {
        keys.push_back(kv.first);
        auto x = kv.second();
        //to check if we got everything
    }

    m_Scene->setRegistry(ret);
    m_FlowView = new FlowView(m_Scene);
    l->addWidget(m_FlowView);

    l->addWidget(popup);

    Spoiler * spoil = new Spoiler();
    QVBoxLayout * spoill = new QVBoxLayout();
    m_ScriptTool = new CodeEditor(nullptr,m_ScriptManager);
    m_ScriptTool->setMinimumSize(600,400);
    spoill->addWidget(m_ScriptTool);
    spoil->setContentLayout(*spoill);

    l->addWidget(spoil);

    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    popup->setVisible(false);


    std::cout << "set up script auto-preview " << std::endl;


    m_QuitThread.store(false);

    QtConcurrent::run([this](){

        std::cout << "start loop "<< std::endl;
        while(true)
        {
            bool quit = m_QuitThread.load();

            if(quit)
            {
                std::cout << "QUIT VISUALSCRIPT LOOP"<< std::endl;
                return;
            }

            m_PreviewMutex.lock();

            bool has_work = m_CurrentPreviewNodes.length() > 0;

            QThread::msleep(50);
            m_PreviewCodeReset = false;
            if(has_work)
            {

                m_CurrentPreviewNode = m_CurrentPreviewNodes.at(0);
                m_CurrentPreviewArgument = m_CurrentPreviewArguments.at(0);
                m_CurrentPreviewNodes.removeFirst();
                m_CurrentPreviewArguments.removeFirst();
                m_PreviewMutex.unlock();

                while(has_work)
                {
                    std::cout << "do work" << std::endl;

                    //do work
                    FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(m_CurrentPreviewNode->nodeDataModel());

                    RunSinglePreviewNode(nmodel,m_CurrentPreviewArgument);
                    std::cout << "did node" << std::endl;

                    int_emit_signal_node_preview();

                    m_PreviewMutex.lock();
                    //check if we can still put the output into the preview display of the node!

                    has_work = m_CurrentPreviewNodes.length() > 0;
                    m_PreviewCodeReset = false;
                    if(has_work)
                    {
                        m_CurrentPreviewNode = m_CurrentPreviewNodes.at(0);
                        m_CurrentPreviewArgument = m_CurrentPreviewArguments.at(0);
                        m_CurrentPreviewNodes.removeFirst();
                        m_CurrentPreviewArguments.removeFirst();
                    }else
                    {
                        m_PreviewMutex.unlock();
                        break;
                    }
                    m_PreviewMutex.unlock();
                }
            }else
            {
                m_PreviewMutex.unlock();
            }
        }
    });
}


void VisualScriptingTool::signal_node_preview()
{
    std::cout << "analyze node for previews " << std::endl;
    m_PreviewMutex.lock();

    std::vector<QtNodes::Node*> nodes = m_Scene->allNodes();
    std::unordered_map<QUuid, std::shared_ptr<QtNodes::Connection>> connections = m_Scene->connections();

    //check iteratively if we need to clear nodes

    bool changed = true;
    while(changed)
    {
        changed = false;

        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node * n = nodes.at(i);

            FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());

            if(!nmodel->IsPreviewAvailable() || !nmodel->IsReturner())
            {
                continue;
            }

            if(nmodel->GetInternalDataChangeFlag() && nmodel->IsPreviewAvailable())
            {
                std::cout << "clear node internal data change" << nmodel->name().toStdString() << std::endl;
                nmodel->ClearPreview();
                continue;
            }


            QList<int> ports;
            QList<QtNodes::Node*> providers;
            //can we fill all ports with available data?
            for(auto kv : connections)
            {
                std::shared_ptr<QtNodes::Connection> conn = kv.second;
                QtNodes::Node* n_out = conn->getNode(QtNodes::PortType::Out);

                if(conn->getNode(QtNodes::PortType::In) == n)
                {
                    int index = conn->getPortIndex(QtNodes::PortType::In);
                    ports.push_back(index);
                    providers.push_back(n_out);
                }
            }

            bool has= nmodel->IsInternalDataForPreviewValid();

            if(has)
            {
                for(int j = 0; j < nmodel->nPorts(QtNodes::PortType::In); j++)
                {

                    int index = ports.indexOf(j);
                    if(index < 0)
                    {
                        std::cout << "index not found, set has to false"<< std::endl;
                        has = false; break;
                    }
                    {
                        FunctionalNodeModel* noutmodel = dynamic_cast<FunctionalNodeModel*>(providers.at(index)->nodeDataModel());
                        //we require that preview nodes have a single output
                        if(!(noutmodel->IsPreviewAvailable() && noutmodel->IsReturner() && (!noutmodel->IsPreviewError())&&(!noutmodel->GetExternalDataChangeFlag())))
                        {
                            std::cout << "setting has to false at " << noutmodel->name().toStdString() << "  " << noutmodel->IsPreviewAvailable() << noutmodel->IsReturner() << noutmodel->IsPreviewError() << noutmodel->GetExternalDataChangeFlag() << std::endl;
                            has=false;

                        }
                        if(nmodel->IsPreviewError() && (noutmodel->GetInternalDataChangeFlag() || noutmodel->GetExternalDataChangeFlag()))
                        {
                            nmodel->ClearError();

                        }
                    }
                }
            }

            //if not, make sure to remove any preview data that was there.

            std::cout << "node has first check ? " << has << std::endl;

            if(has == false)
            {
                if(nmodel->IsPreviewAvailable())
                {
                    std::cout << "clear node " << nmodel->name().toStdString() << std::endl;
                    nmodel->ClearPreview();
                    changed = true;

                }
            }
        }
    }



    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);
        FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());

        nmodel->SetInternalDataChangeFlag(false);
        nmodel->SetExternalDataChangeFlag(false);
    }

    //add a single node (if we have one that can be done) to the list

    if(m_CurrentPreviewNodes.length() == 0)
    {
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node * n = nodes.at(i);
            FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());

            std::cout << "node i " << nmodel->name().toStdString() << " "<< i << " " <<nmodel->IsPreviewAvailable()<< nmodel->IsReturner() << nmodel->IsInternalDataForPreviewValid()<<std::endl;
            if(nmodel->IsPreviewAvailable() || !nmodel->IsReturner() || nmodel->IsPreviewError())
            {
                continue;
            }

            QList<int> ports;
            QList<QtNodes::Node*> providers;
            //can we fill all ports with available data?
            for(auto kv : connections)
            {

                std::shared_ptr<QtNodes::Connection> conn = kv.second;
                QtNodes::Node* n_out = conn->getNode(QtNodes::PortType::Out);

                if(conn->getNode(QtNodes::PortType::In) == n)
                {
                    int index = conn->getPortIndex(QtNodes::PortType::In);
                    ports.push_back(index);
                    providers.push_back(n_out);
                }

            }

            bool has= nmodel->IsInternalDataForPreviewValid();
            if(has)
            {
                for(int j = 0; j < nmodel->nPorts(QtNodes::PortType::In); j++)
                {

                    int index = ports.indexOf(j);
                    if(index < 0)
                    {
                        std::cout << "port not found " << std::endl;
                        has = false; break;
                    }
                    {

                        FunctionalNodeModel* noutmodel = dynamic_cast<FunctionalNodeModel*>(providers.at(index)->nodeDataModel());
                        //we require that preview nodes have a single output
                        if((!noutmodel->IsPreviewAvailable()) || noutmodel->IsPreviewError())
                        {
                            std::cout << "provider is not available "<< std::endl;
                            has=false;break;

                        }
                    }
                }
            }

            std::cout << "does it has? " << nmodel->name().toStdString() << " "<<ports.size() << " "<< providers.size() << " " << has << std::endl;

            //if all is ready, add it to the que

            if(has == true)
            {
                std::cout << "add model " << std::endl;
                //get parameters

                QList<PreviewArgument> args;

                for(int j = 0; j < nmodel->nPorts(QtNodes::PortType::In); j++)
                {
                    std::cout << "check port " << j << std::endl;

                    int index = ports.indexOf(j);
                    if(index < 0)
                    {
                        has = false; break;
                    }
                    {
                        FunctionalNodeModel* noutmodel = dynamic_cast<FunctionalNodeModel*>(providers.at(index)->nodeDataModel());
                        //we require that preview nodes have a single output

                        args.append(noutmodel->GetPreviewData());
                    }
                }

                std::cout << "checked ports " << has << std::endl;

                if(has)
                {

                     m_CurrentPreviewNodes.append(n);
                     m_CurrentPreviewArguments.append(args);

                     nmodel->SetPreviewIsLoading();

                }

                break;
            }
        }
    }

    m_PreviewMutex.unlock();

}

void VisualScriptingTool::RunSinglePreviewNode( FunctionalNodeModel*model , QList<PreviewArgument> args)
{
    if(args.size() == model->nPorts(QtNodes::PortType::In))
    {

        std::vector<QString> ins;
        std::vector<QString> outs;
        outs.push_back("vout");

        std::vector<QString> ins_types;

        for(int i = 0;i < model->nPorts(QtNodes::PortType::In); i++)
        {

            ins.push_back("vin_" +QString::number(i));
            ins_types.push_back(m_ScriptManager->m_Engine->GetTypeNameFromUIName(model->dataType(QtNodes::PortType::In,i).name));
        }

        //get code
        QList<QString> code = model->GetCodeLine(ins,outs,m_ScriptManager->m_Engine);
        QString script;
        for(int i = 0; i < code.length(); i++)
        {
            script += "\n";
            script += code.at(i);
        }

        QString rtype = m_ScriptManager->m_Engine->GetTypeNameFromUIName(model->dataType(QtNodes::PortType::Out,0).name);
        QString funcCode = rtype + " GetNodeVal(";

        for(int i = 0;i < model->nPorts(QtNodes::PortType::In); i++)
        {

            funcCode += ins_types.at(i);
            funcCode += " vin_" +QString::number(i);
            funcCode += " ";
            if(model->nPorts(QtNodes::PortType::In)-1 != i)
            {
                funcCode += ",";
            }
        }

        funcCode += ")";
        QString decl = funcCode;

        funcCode += "{\n";
        funcCode += script;
        funcCode += "\nreturn vout;\n}";
        script = funcCode;

        std::cout << "running script for node !!!!!!!!!!!!!!! " << std::endl;
        std::cout << script.toStdString() << std::endl;

        //get all parameters

        //set up script
        SPHScript * s = m_ScriptManager->CompileScriptNode(script);

        if(s->IsCompiled())
        {

            asIScriptContext *ctx = m_ScriptManager->m_Engine->CreateContext();
            asIScriptFunction *func = s->scriptbuilder->GetModule()->GetFunctionByDecl(QString(decl).toStdString().c_str());
            if(func != nullptr)
            {
                ctx->Prepare(func);


                 void *ret = nullptr;

                 QList<std::shared_ptr<void>> ovars;

                 bool nullf = false;

                 for(int i = 0;i < model->nPorts(QtNodes::PortType::In); i++)
                 {
                     QString type = ins_types.at(i);

                     if(type == "bool")
                     {
                        ctx->SetArgByte(i,args.at(i).xb);
                     }else if(type == "int")
                     {
                        ctx->SetArgDWord(i,args.at(i).xi);
                     }else if(type == "float")
                     {
                        ctx->SetArgFloat(i,args.at(i).xd);
                     }else if(type == "double")
                     {
                        ctx->SetArgDouble(i,args.at(i).xd);
                     }else
                     {
                         ovars.append(args.at(i).xm);
                         std::cout <<  "object arg " << args.at(i).xm.get() << std::endl;
                         ctx->SetArgObject(i,args.at(i).xm.get());
                         if(args.at(i).xm.get() == nullptr)
                         {
                             std::cout << "argument is null"<<std::endl;
                             nullf = true;
                         }
                     }

                 }

                 if(!nullf)
                 {

                     //set up callbacks


                     try
                     {
                         std::cout << "run start "<< std::endl;
                         int r = ctx->Execute();
                         std::cout << "run end "<< std::endl;

                         if( r == asEXECUTION_FINISHED )
                         {

                             std::cout << "run finished "<< std::endl;

                         }else
                         {
                             std::cout << "error during node calculation " << std::endl;
                         }
                     }catch(...)
                     {
                          std::cout << "exception during node calculation" << std::endl;
                         LISEM_ERROR("Exception thrown during script run");

                     }


                     std::cout << "calc2 finished " << ret  <<  std::endl;


                         PreviewArgument p;
                         p.type = rtype;

                         if(rtype == "bool")
                         {
                             p.isvalid= true;
                             p.xb = ctx->GetReturnByte();

                         }else if(rtype == "int")
                         {
                             p.isvalid= true;
                             p.xi = ctx->GetReturnDWord();
                         }else if(rtype == "float")
                         {
                             p.isvalid= true;
                             p.xd = ctx->GetReturnFloat();
                         }else if(rtype == "double")
                         {
                             p.isvalid= true;
                             p.xd = ctx->GetReturnDouble();
                         }else if(rtype == "Map")
                         {
                             ret =  ctx->GetReturnObject();
                             std::cout << "get return object " << ret << std::endl;

                             if(ret != nullptr)
                             {
                                 p.isvalid= true;
                                 //get result
                                 p.xm = std::static_pointer_cast<void>(std::make_shared<cTMap>(std::move(*((cTMap *)ctx->GetReturnObject()))));
                             }else
                             {
                                nullf = true;
                             }
                         }

                         if(!nullf)
                         {
                             std::cout << "preview data ready"<< std::endl;
                             m_PreviewDataMutex.lock();
                             m_PreviewDataModel.append(model);
                             m_PreviewData.append(p);

                             m_PreviewDataMutex.unlock();

                             std::cout << "done "<< std::endl;

                             emit int_emit_previewdataset();

                             ctx->Release();
                             ovars.clear();
                             return;
                         }else
                         {
                             std::cout << "null returned " << std::endl;
                         }

                 }else
                 {
                     std::cout << "null arg encoutnered " << std::endl;
                 }


                ovars.clear();
            }else
            {
                std::cout << "function declaration not found " << std::endl;
            }


            ctx->Release();

        }else
        {
            std::cout << "compilation error" << std::endl;
        }
    }

    //check if we can still set the result
    std::cout << "preview data not ready "<< std::endl;

    PreviewArgument p;
    p.isvalid= false;
    m_PreviewDataMutex.lock();
    m_PreviewDataModel.append(model);
    m_PreviewData.append(p);

    m_PreviewDataMutex.unlock();

    std::cout << "done "<< std::endl;

    emit int_emit_previewdataset();

}


void VisualScriptingTool::OnNodePlaced(QtNodes::Node & n)
{
    FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n.nodeDataModel());

    connect(nmodel,&FunctionalNodeModel::OnInternalDataChanged,[this,nmodel]()
    {

        nmodel->ClearPreview();
        nmodel->SetInternalDataChangeFlag(true);
        signal_node_preview();
    });

    m_LineInfoMutex.lock();
    m_Codes.clear();
    m_CodeLineNodes.clear();
    is_compilechanged = true;
    m_LineInfoMutex.unlock();

    signal_node_preview();
}

void VisualScriptingTool::OnNodeDeleted(QtNodes::Node & n)
{
    m_LineInfoMutex.lock();
    m_Codes.clear();
    m_CodeLineNodes.clear();
    is_compilechanged = true;
    m_LineInfoMutex.unlock();

    signal_node_preview();
}

void VisualScriptingTool::connectionCreated(QtNodes::Connection const & c)
{
    m_LineInfoMutex.lock();
    m_Codes.clear();
    m_CodeLineNodes.clear();
    is_compilechanged = true;
    m_LineInfoMutex.unlock();

    signal_node_preview();
}

void VisualScriptingTool::connectionDeleted(QtNodes::Connection const & c)
{
    m_LineInfoMutex.lock();
    m_Codes.clear();
    m_CodeLineNodes.clear();
    is_compilechanged = true;
    m_LineInfoMutex.unlock();

    signal_node_preview();
}


void VisualScriptingTool::OnNodeDone(int l){
    m_LineInfoMutex.lock();

    //-1 indicates we are done with running script, set all as done
    if( l == -1)
    {
        //m_Codes
        for(int i = 0; i < m_CodeLineNodes.length(); i++)
        {
            QtNodes::Node * n =m_CodeLineNodes.at(i);
            if(n != nullptr)
            {
                if(!(n->nodeDataModel()->validationState() == QtNodes::NodeValidationState::Warning))
                {
                    ((FunctionalNodeModel* )n->nodeDataModel())->SetValidationState(QtNodes::NodeValidationState::Warning);
                    ((FunctionalNodeModel* )n->nodeDataModel())->SetValidationMessage("Done");
                    n->nodeGraphicsObject().update(n->nodeGraphicsObject().boundingRect());

                }
            }

        }
    }
    m_CurrentLine = l;

    //is the number in a valid range? if so, set the node that corresponds to that number as done
    if(m_CurrentLine< m_CodeLineNodes.length() && l > -1)
    {
        QtNodes::Node * n = m_CodeLineNodes.at(m_CurrentLine);

        if(n != nullptr)
        {
            ((FunctionalNodeModel* )n->nodeDataModel())->SetValidationState(QtNodes::NodeValidationState::Warning);
            ((FunctionalNodeModel* )n->nodeDataModel())->SetValidationMessage("Done");
            n->nodeGraphicsObject().update(n->nodeGraphicsObject().boundingRect());
        }
    }
    m_LineInfoMutex.unlock();

    //update widget
    //depending on the script, this might be asked for quite frequently
    //perhaps better to do this only if a certain amount of time has passed?
    m_FlowView->invalidateScene();
    m_FlowView->update();

    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget* w, widgets)
    {
         w->repaint();
    }
    m_FlowView->repaint();
}

inline void VisualScriptingTool::SetPreviewData()
{
    m_PreviewDataMutex.lock();

    std::vector<QtNodes::Node*> nodes = m_Scene->allNodes();

    for(int i = 0; i < m_PreviewDataModel.size(); i++)
    {
        //check if model is still active
        FunctionalNodeModel* model = m_PreviewDataModel.at(i);
        PreviewArgument arg = m_PreviewData.at(i);

        for(int j = 0; j < nodes.size(); j++)
        {
            FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(nodes.at(j)->nodeDataModel());
            if(nmodel == model)
            {
                std::cout << "found and setting preview data" << std::endl;
                model->SetPreviewData(arg);
                break;
            }
        }

    }

    m_PreviewData.clear();
    m_PreviewDataModel.clear();

    m_PreviewDataMutex.unlock();

    signal_node_preview();

    m_FlowView->invalidateScene();
    m_FlowView->update();

    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget* w, widgets)
    {
         w->repaint();
    }
    m_FlowView->repaint();
}

void VisualScriptingTool::OnSave(bool)
{
    m_Scene->save();
}
void VisualScriptingTool::OnSaveas(bool)
{
    m_Scene->saveToMemory();
}
void VisualScriptingTool::OnLoad(bool)
{

    m_Scene->load();

}
void VisualScriptingTool::OnNew(bool)
{
    m_Scene->clearScene();
}
void VisualScriptingTool::OnPreviewToggle(bool)
{

}
void VisualScriptingTool::OnRun(bool)
{

    m_LineInfoMutex.lock();
    if(!is_compilesucceed)
    {
        QMessageBox::warning(
            this,
            tr("OpenLISEM Hazard"),
            tr("First compile your code") );

        m_LineInfoMutex.unlock();
        return;
    }
    m_LineInfoMutex.unlock();


    //get wether a code is running

    bool is_running = m_CodeIsRunning.load();

    if(is_running)
    {
        m_PauseMutex.lock();
        if(m_CodeIsPaused.load())
        {
            m_PauseMutex.unlock();
            //re-start code
            m_PauseWaitCondition.notify_all();
        }else
        {
            QMessageBox::warning(
                this,
                tr("OpenLISEM Hazard"),
                tr("Still running code, wait for it to finish") );

            m_PauseMutex.unlock();
        }



    }else
    {

        CodeEditor * ce = m_ScriptTool;
        QString command = ce->document()->toPlainText();

        SPHScript *s = new SPHScript();
        s->SetCode(command);
        s->SetSingleLine(true);
        s->SetPreProcess(true);
        s->SetHomeDir("");

        s->SetCallBackPrint(std::function<void(VisualScriptingTool *,SPHScript*,QString)>([](VisualScriptingTool *,SPHScript*,QString) ->
                                                                    void{


            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackDone(std::function<void(VisualScriptingTool *,SPHScript*,bool x)>([ce](VisualScriptingTool *st,SPHScript*, bool finished) ->
                                                                    void{


            st->m_CodeIsPaused.store(false);
            st->m_CodeIsStopRequested.store(false);
            st->m_CodeIsPauseRequested.store(false);
            st->m_CodeIsRunning.store(false);

            st->int_emitupdatebuttons_stop();

           if(finished)
           {
               st->popupMessage = "Done";
           }
            st->int_emit_show_temporary_dialog();
            ce->SetHighlightCurrentRunLine(-1);
            st->setCurrentLine(-1);

            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackCompilerError(std::function<void(VisualScriptingTool *,SPHScript*,const asSMessageInfo *msg)>([](VisualScriptingTool *,SPHScript*,const asSMessageInfo *msg) ->
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

        s->SetCallBackException(std::function<void(VisualScriptingTool *,SPHScript*,asIScriptContext *ctx)>([](VisualScriptingTool *st,SPHScript*,asIScriptContext *ctx) ->
                                                                    void{

            st->popupMessage = "Exception encountered when running script";

            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackLine(std::function<void(VisualScriptingTool *,SPHScript*,asIScriptContext *ctx)>([ce](VisualScriptingTool * st,SPHScript*,asIScriptContext *ctx) ->
                           void{
            ce->SetHighlightCurrentRunLine(ctx->GetLineNumber());
            st->setCurrentLine(ctx->GetLineNumber());

            if(st->m_CodeIsPauseRequested.load())
            {
                //pause by setting up waitcondition
                st->m_PauseMutex.lock();
                st->m_CodeIsPaused.store(true);
                LISEMS_STATUS("Paused script execution");
                st->int_emitupdatebuttons_pause();
                st->m_PauseWaitCondition.wait(&(st->m_PauseMutex));
                st->m_CodeIsPauseRequested.store(false);
                st->m_CodeIsPaused.store(false);
                st->int_emitupdatebuttons_start();
                st->m_PauseMutex.unlock();
            }

            if(st->m_CodeIsStopRequested.load())
            {
                //abort execution
                ctx->Abort();
                ctx->Suspend();

                LISEMS_STATUS("Stopped script execution");
            }

            ;
                           }),this,s,std::placeholders::_1);

        m_ScriptManager->CompileScript_Generic(s);

        if(s->IsCompiled())
        {
            m_CodeIsPaused.store(false);
            m_CodeIsStopRequested.store(false);
            m_CodeIsPauseRequested.store(false);
            m_CodeIsRunning.store(true);
            ce->SetHighlightCurrentRunLine(0);

            int_emitupdatebuttons_start();
            popupMessage = "running";
            emit int_emit_show_dialog();

            m_ScriptManager->RunScript(s);
        }else
        {

            LISEMS_STATUS("Compilation Error");
            m_CodeIsPaused.store(false);
            m_CodeIsStopRequested.store(false);
            m_CodeIsPauseRequested.store(false);
            m_CodeIsRunning.store(false);
        }


    }


}
void VisualScriptingTool::OnPause(bool)
{

    if(m_CodeIsRunning.load())
    {
        if(!m_CodeIsPaused.load())
        {
            m_CodeIsPauseRequested.store(true);
        }
    }

}
void VisualScriptingTool::OnStop(bool)
{
    if(m_CodeIsRunning.load())
    {
        m_CodeIsStopRequested.store(true);

        m_PauseMutex.lock();
        if(m_CodeIsPaused.load())
        {
            m_PauseMutex.unlock();
            //re-start code
            m_PauseWaitCondition.notify_all();
        }else
        {
            m_PauseMutex.unlock();
        }
    }


}
void VisualScriptingTool::OnCompile(bool)
{

    if(m_CodeIsRunning.load())
    {

        QMessageBox::warning(
            this,
            tr("OpenLISEM Hazard"),
            tr("Still running code, wait for it to finish before compiling changes") );

    }else
    {
        QList<QString> code = ConvertToScript();
        //code is same as m_Code, which is set by function
        m_FlowView->invalidateScene();
        m_FlowView->update();

        QString codetext = "";

        for(int i = 0; i < code.length(); i++)
        {

            codetext +=  "\n";
            codetext += code.at(i);
        }

        m_ScriptTool->clear();
        m_ScriptTool->insertPlainText(codetext);

    }

    m_FlowView->repaint();
}

#define LISEM_VISUALSCRIPT_DEBUG

#ifdef LISEM_VISUALSCRIPT_DEBUG
#define SPHVSDB(x) x
#else
#define SPHVSDB(x) ;
#endif

QList<QString> VisualScriptingTool::ConvertToScript()
{
    int error_missingconnection = -1;
    int error_unconfinedloop = -1;
    int error_invalidvalue = -1;


    QList<QString> Codes;
    QList<QtNodes::Node*> CodeLineNodes;

    m_LineInfoMutex.lock();

    is_compilesucceed = false;
    is_compilechanged = false;
    m_Codes.clear();
    m_CodeLineNodes.clear();

    SPHVSDB(std::cout <<std::endl <<  "////////////////////////////---------do compile "<< std::endl;)

    std::vector<QtNodes::Node*> nodes = m_Scene->allNodes();
    std::unordered_map<QUuid, std::shared_ptr<QtNodes::Connection>> connections = m_Scene->connections();

    std::vector<std::vector<std::shared_ptr<QtNodes::Connection>>> in_conn;
    std::vector<std::vector<std::shared_ptr<QtNodes::Connection>>> out_conn;

    SPHVSDB(std::cout <<std::endl <<  "/////reset node messages "<< std::endl;)

    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);

        FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());
        nmodel->SetValidationState(QtNodes::NodeValidationState::Valid);
        nmodel->SetValidationMessage("");

    }

    SPHVSDB(std::cout <<  "//initialize connections "<< std::endl;)

    for(int i = 0; i < nodes.size(); i++) {
        in_conn.insert(in_conn.begin(),std::vector<std::shared_ptr<QtNodes::Connection>>());
        out_conn.insert(out_conn.begin(),std::vector<std::shared_ptr<QtNodes::Connection>>());
    }



    SPHVSDB(std::cout <<  "//sort connections by node"<< std::endl;)

    //sort the nodes with their connections
    for(auto kv : connections) {

        std::shared_ptr<QtNodes::Connection> conn = kv.second;
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node * n = nodes.at(i);
            if(conn->getNode(QtNodes::PortType::Out) == n)
            {
                out_conn.at(i).insert(out_conn.at(i).begin(),conn);
            }
            if(conn->getNode(QtNodes::PortType::In) == n)
            {
                in_conn.at(i).insert(in_conn.at(i).begin(),conn);
            }
        }
    }

    SPHVSDB(std::cout <<  "//initialize properties "<< std::endl;)


    //indices for the nodes
    std::unordered_map<QtNodes::Node*,int> node_indices;


    //first resolve structure
    QList<bool> NodeIsLoop;
    QList<bool> NodePartOfLoop;
    //fill with defeault values
    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);

        node_indices.insert(std::pair<QtNodes::Node*,int>(n,i));

        NodeIsLoop.append(false);
        NodePartOfLoop.append(false);
    }

    SPHVSDB(std::cout <<"// sort connections by input port index "<< std::endl;)


    bool found_missing = false;
    QtNodes::Node * nerror = nullptr;

    //find all the loop nodes
    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);
        QtNodes::NodeDataModel * dm = n->nodeDataModel();

        //check if all nececary connections are filled

        //to do this, we only get all unique ports !!NOT NEEDED, INPUT CAN NOT BE DOUBLE!!


        /*std::list<int> ports;

        for(int j = 0; j < in_conn.at(i).size(); j++)
        {
            ports.insert(ports.begin(),in_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::In));
        }
        ports.sort();
        ports.unique();*/

        int n_ports_in = dm->nPorts(QtNodes::PortType::In);

        if(!(n_ports_in == in_conn.at(i).size()))
        {
            if(!(n->nodeDataModel()->name() == "Variable"))
            {

                SPHVSDB(std::cout << "ERROR node input missing"<< std::endl;)
                //return is done after the sorting, so that we can indicate all nodes that miss stuff

                FunctionalNodeModel* nmodel = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());
                nmodel->SetValidationState(QtNodes::NodeValidationState::Error);
                nmodel->SetValidationMessage("Required inputs are missing!");

                nerror = n;
                found_missing = true;
            }
        }


        //sort the inputs, as they must be the same number as the optional number of inputs
        std::vector<std::shared_ptr<QtNodes::Connection>> in_sorted;
        for(int j =0; j < in_conn.at(i).size(); j++)
        {
            for(int k = 0; k < in_conn.at(i).size(); k++)
            {
                if(in_conn.at(i).at(k)->getPortIndex(QtNodes::PortType::In) == j)
                {
                    in_sorted.insert(in_sorted.end(),in_conn.at(i).at(k));
                }
            }
        }
        in_conn[i] = in_sorted;

    }

    if(found_missing)
    {
        SPHVSDB(std::cout << "ERROR node input missing"<< std::endl;)

        //error!!!
        //non-filled input port at node [n]

        popupMessage = "ERROR node input missing";
        if(nerror != nullptr)
        {
            QRectF r = (nerror->nodeGraphicsObject()).boundingRect();
            r.setWidth(r.width() * 3.0);
            r.setHeight(r.height() * 3.0);
            this->m_FlowView->fitInView(r,Qt::AspectRatioMode::KeepAspectRatioByExpanding);
        }
        emit int_emit_show_temporary_dialog();
        m_LineInfoMutex.unlock(); return Codes;


    }

    SPHVSDB(std::cout <<  "/////////-- find all the loop nodes and their children "<< std::endl;)


    //find all the loop nodes
    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);
        if(n->nodeDataModel()->name() == "Loop" || n->nodeDataModel()->name() == "While")
        {
             NodeIsLoop[i] = true;

             QList<QtNodes::Node *> subnodes;
             //fill with initial nodes that follow from loop
             for(int j = 0; j < out_conn.at(i).size(); j++)
             {
                 //we only, for now, follow the odd-numbered outputs that are for the loop itself
                 bool is_second =  (out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::Out) %2 == 1);

                 if(is_second && out_conn.at(i).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                 {
                    subnodes.insert(subnodes.begin(),out_conn.at(i).at(j)->getNode(QtNodes::PortType::In));
                 }
             }
             //check all sub-nodes, including loops but excluding sub-loop nodes
             while(subnodes.length() > 0)
             {
                 QtNodes::Node * n2 = subnodes.at(0);

                 int index = node_indices[n2];
                 //if we return to our own loop, then all is fine
                 if(!(n2 == n))
                 {
                     if(NodePartOfLoop[index] == true)
                     {
                        SPHVSDB(std::cout << "ERROR infinite loop" << std::endl;)

                        m_LineInfoMutex.unlock(); return Codes;
                        //error!!!
                        //there is an infinite loop
                        //node n2 has already been seen in a loop, and is now used again


                     }else {
                         NodePartOfLoop[index] = true;
                     }
                 }else
                 {
                 }

                 //this one ends in our loop node, so there is closure
                 if(n2 == n)
                 {
                     subnodes.removeAt(0);
                     continue;
                 }

                 //if subnode is another loop, continue directly with its output
                 if(n2->nodeDataModel()->name() == "Loop" || n2->nodeDataModel()->name() == "While")
                 {

                     for(int j = 0; j < out_conn.at(index).size(); j++)
                     {
                         //we only, for now, follow the even-numbered outputs that are output
                         bool is_second = (out_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::Out) %2 == 0);

                         if(is_second && out_conn.at(index).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                         {
                            //add this to the subnodes
                            subnodes.insert(subnodes.end(),out_conn.at(index).at(j)->getNode(QtNodes::PortType::In));
                         }
                     }

                     subnodes.removeAt(0);
                     continue;
                 }else if(n2->nodeDataModel()->name() == "Variable")
                 {
                     //variables are seperate note type,
                     //we dont follow these, unless required
                     //this could be an update or initalization
                     //initialization means creation, inside a loop is a weird use-case
                     //allow it for now

                     subnodes.removeAt(0);
                     continue;
                 }else
                 {
                     //if subnode is another type of node, continue following its output

                     for(int j = 0; j < out_conn.at(index).size(); j++)
                     {
                         //we follow all outputs for a normal node
                         if(out_conn.at(index).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                         {
                            //add this to the subnodes
                            subnodes.insert(subnodes.end(),out_conn.at(index).at(j)->getNode(QtNodes::PortType::In));
                         }
                     }

                     subnodes.removeAt(0);
                     continue;
                 }
             }
        }

        //are not yet of interest, if they are in a a loop, they must both stay within the loop, but this is automatically checked
        if(n->nodeDataModel()->name() == "If,Else")
        {

        }
        //are also not yet of interest, these nodes are not followed directly, but only called in case of a requirement
        if(n->nodeDataModel()->name() == "Variable")
        {

        }
    }


    SPHVSDB(std::cout <<  "/////////////----- start doing the code conversion"<< std::endl;)



    ////we have for each node ensured:
    //input is filled
    //which ones are loops?
    //is it within a loop
    //loops are closed
    //what is the loop parent

    ////now for the next part
    //iteratively progressing through the compilation
    //checking wether all inputs are finished
    //adding code lines


    //CONVERSION CODE

    std::vector<QtNodes::Node*> nodestbd = nodes;


    std::vector<std::vector<QString>> in_conn_codenames;
    std::vector<std::vector<QString>> out_conn_codenames;
    std::vector<std::vector<bool>> in_conn_done;

    QList<bool> NodeFinished;
    QList<bool> NodeIsVariable;
    QList<QString> NodeVariableName;
    QList<QString> NodeLoopConditionName;
    QList<QString> NodeLoopIteratorName;
    QList<bool> NodeIsVariableIsAssigned;


    QList<QtNodes::Node*> UpperLoopNode;


    //fill with defeault values

    SPHVSDB(std::cout <<  "// initialize default properties"<< std::endl;)


    int variable_count = 0;
    int var_inout_count = 0;

    for(int i = 0; i < nodes.size(); i++)
    {
        in_conn_codenames.insert(in_conn_codenames.begin(),std::vector<QString>());
        out_conn_codenames.insert(out_conn_codenames.begin(),std::vector<QString>());

    }
    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);

        NodeFinished.append(false);
        NodeLoopConditionName.append("");
        NodeLoopIteratorName.append("");

        UpperLoopNode.append(nullptr);

        for(int j = 0; j < in_conn.at(i).size(); j++)
        {
            in_conn_codenames.at(i).insert(in_conn_codenames.at(i).begin(),"");
        }
        for(int j = 0; j < out_conn.at(i).size(); j++)
        {
            out_conn_codenames.at(i).insert(out_conn_codenames.at(i).begin(),"");
        }
        if(n->nodeDataModel()->name() == "Variable")
        {
            NodeIsVariable.append(true);

            NodeVariableName.append("variable_" +QString::number(variable_count));

            variable_count++;
        }else {
            NodeIsVariable.append(false);
            NodeVariableName.append("");
        }

        NodeIsVariableIsAssigned.append(false);

    }


    SPHVSDB(std::cout <<  "// write intro"<< std::endl;)


    Codes.append(QString(R"(//SPHazard generated script )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"(//This is a comment, provided to understand the code )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"( )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"(//Declaring variables )"));
    CodeLineNodes.append(nullptr);


    SPHVSDB(std::cout <<  "//////-- declare all variables"<< std::endl;)


    //declare variables
    for(int i = 0; i < nodes.size(); i++)
    {
        QtNodes::Node * n = nodes.at(i);

        NodeFinished.append(false);

        if(n->nodeDataModel()->name() == "Variable")
        {

            VariableFunctionModel * varmodel = dynamic_cast<VariableFunctionModel*>(n->nodeDataModel());

            if(varmodel)
            {
                QtNodes::NodeDataType type = varmodel->dataType(QtNodes::PortType::Out,0);
                QString codename = m_ScriptManager->m_Engine->GetTypeNameFromUIName(type.name);
                if(codename.isEmpty())
                {
                    SPHVSDB(std::cout << "ERROR can not get variable type" << std::endl;)
                    m_LineInfoMutex.unlock(); return Codes;
                    //error!!!
                }

                //declare variable
                Codes.append(codename + " " + NodeVariableName[i] + ";");
                CodeLineNodes.append(n);

                //set the variable name for any following nodes
                for(int j = 0; j < out_conn.at(i).size(); j++)
                {
                    QtNodes::Node * n2 =out_conn.at(i).at(j)->getNode(QtNodes::PortType::In);
                    if(n2 != nullptr )
                    {

                       {
                           int index = node_indices[n2];
                           int portindex_in = out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::In);
                           int portindex_out = out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::Out);

                           in_conn_codenames.at(index).at(portindex_in) = NodeVariableName[i];
                       }
                    }
                }


                NodeFinished[i] = true;

            }else
            {
                SPHVSDB(std::cout << "ERROR from variable not being variable" << std::endl;)
                m_LineInfoMutex.unlock(); return Codes;
                //error!!!
            }
        }


    }

    SPHVSDB(std::cout <<  "//////-- start calculation section"<< std::endl;)


    //start calculation writing

    Codes.append(QString(R"( )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"( )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"(//Calculation section )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"(//iteratively processing all nodes for which all input is prepared )"));
    CodeLineNodes.append(nullptr);
    Codes.append(QString(R"( )"));
    CodeLineNodes.append(nullptr);


    bool is_finished = false;

    SPHVSDB(std::cout <<  "//////-- start main loop"<< std::endl;)

    while(!is_finished)
    {
        SPHVSDB(std::cout <<  "//////-- iteration in main loop"<< std::endl;)

        //while there is a node that does not require additional input

        bool finished_nonloop = false;

        //first check for non-loop nodes
        for(int i = 0; i < nodes.size(); i++)
        {
            QtNodes::Node * n = nodes.at(i);

            SPHVSDB(std::cout <<  "is this node finished?  " << n->nodeDataModel()->name().toStdString() <<  "  " <<NodeIsLoop[i] << " " << NodeFinished[i] << "  " << NodePartOfLoop[i] << std::endl;)


            if(NodeIsLoop[i] == false && NodePartOfLoop[i] == false && NodeFinished[i] == false)
            {
                SPHVSDB(std::cout <<  "//check node with name " << n->nodeDataModel()->name().toStdString() << std::endl;)

                //check if there is missing input
                bool missing = false;

                for(int j = 0; j < in_conn.at(i).size(); j++)
                {

                    QtNodes::Node* n_source = (in_conn.at(i).at(j)->getNode(QtNodes::PortType::Out));
                    int index = node_indices[n_source];
                    if(!NodeFinished[index])
                    {
                        missing = true;
                        break;
                    }
                }

                //if none is missing, finish this node
                if(!missing)
                {
                    finished_nonloop = true;
                    NodeFinished[i] = true;

                    SPHVSDB(std::cout <<  "//solve node with name " << n->nodeDataModel()->name().toStdString() << std::endl;)


                    //variables
                    //get the name of all the input variables
                    //in_conn_codenames should be filled with these
                    //might not be sorted? yes they are, at the start
                    std::vector<QString> inputnames = in_conn_codenames.at(i);

                    SPHVSDB(std::cout <<  "prepare output names " << std::endl;)

                    //now prepare the output names
                    std::vector<QString> outputnames;

                    //function
                    FunctionalNodeModel* funcnode = dynamic_cast<FunctionalNodeModel*>(n->nodeDataModel());

                    for(int j = 0; j < n->nodeDataModel()->nPorts(QtNodes::PortType::Out); j++)
                    {
                        if((funcnode->FirstOutputIsFirstInput()) && j == 0)
                        {
                            outputnames.insert(outputnames.end(),inputnames.at(0));

                        }else
                        {
                            outputnames.insert(outputnames.end(),"var_" +QString::number(var_inout_count));
                            var_inout_count ++;
                        }
                    }

                    SPHVSDB(std::cout <<  "get function code" << std::endl;)

                    //save the results into the new variables

                    //node must both declare and assign the outputnames
                    QList<QString> node_code = funcnode->GetCodeLine(inputnames,outputnames, m_ScriptManager->m_Engine);

                    //add code lines
                    SPHVSDB(std::cout <<  "add code" << std::endl;)
                    for(int j = 0; j < node_code.size(); j++)
                    {
                        QString node_code_line = node_code.at(j);
                        Codes.append(node_code_line);
                        CodeLineNodes.append(n);
                    }

                    SPHVSDB(std::cout <<  "set dependent variables" << std::endl;)

                    //variable check
                    //check if next nodes are variables, if so, set these
                    for(int j = 0; j < out_conn.at(i).size(); j++)
                    {
                        QtNodes::Node * n2 =out_conn.at(i).at(j)->getNode(QtNodes::PortType::In);
                        if(n2 != nullptr )
                        {
                           if(n2->nodeDataModel()->name() == "Variable")
                           {
                               int index = node_indices[n2];
                               int portindex_out = out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::Out);
                               QString varname = outputnames.at(portindex_out);

                               NodeIsVariableIsAssigned[index] = true;
                               Codes.append(NodeVariableName[index] + " = " + varname + ";");
                               CodeLineNodes.append(n2);
                           }
                        }
                    }

                    SPHVSDB(std::cout <<  "output propagation" << std::endl;)

                    //output propagation
                    //set input names for new nodes
                    for(int j = 0; j < out_conn.at(i).size(); j++)
                    {
                        QtNodes::Node * n2 =out_conn.at(i).at(j)->getNode(QtNodes::PortType::In);
                        if(n2 != nullptr )
                        {
                           {
                               SPHVSDB(std::cout << "propagate to " << n2->nodeDataModel()->name().toStdString() << n2 << std::endl;)

                               int index = node_indices[n2];
                               int portindex_in = out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::In);
                               int portindex_out = out_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::Out);

                               QString varname = outputnames.at(portindex_out);
                               SPHVSDB(std::cout << "propagate to ports " << portindex_in << " " <<  portindex_out << " " << varname.toStdString() << " " << out_conn_codenames.at(i).size() << " " << j << std::endl;)

                               out_conn_codenames.at(i).at(j) = varname;


                               for(int k = 0; k < in_conn.at(index).size(); k++)
                               {
                                    if(in_conn.at(index).at(k)->getPortIndex(QtNodes::PortType::In) == portindex_in)
                                    {
                                        in_conn_codenames.at(index).at(k) = varname;
                                        break;
                                    }
                               }
                           }
                        }
                    }
                    SPHVSDB(std::cout <<  "done" << std::endl;)

                }
            }
        }

        bool finished_loop = false;


        //in case of loop, we require the whole loop and its subloops to be independent of data
        //we have ensured this by first checking all possible non-loop nodes. If there are no more non-loops available to finish, we started doing loops
        if(!finished_nonloop)
        {


            //now check for loop nodes
            for(int i = 0; i < nodes.size(); i++)
            {
                //if there is a loop that does not require additional input

                QtNodes::Node * n_check = nodes.at(i);

                SPHVSDB(std::cout <<  "is this loop node finished?  " << n_check->nodeDataModel()->name().toStdString() <<  "  " <<NodeIsLoop[i] << " " << NodeFinished[i] << std::endl;)


                if(NodeIsLoop[i] == true && NodeFinished[i] == false)
                {

                    SPHVSDB(std::cout <<  "//check loop with name " << n_check->nodeDataModel()->name().toStdString() << std::endl;)


                    //is all directly required input there
                    //check if there is missing input
                    bool missing = false;

                    for(int j = 0; j < in_conn.at(i).size(); j++)
                    {
                        SPHVSDB(std::cout << "check loop input " << j << " of " << in_conn.at(i).size() << std::endl;)

                        //we only, for now, require the even-numbered inputs which are not part of the loop
                        bool is_second = ((in_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::In) %2) == 0);
                        //for aloop, do not select the last input (this input is the condition for the loop)
                        bool is_last = false;
                        {
                            if(n_check->nodeDataModel()->nPorts(QtNodes::PortType::Out)-1 ==in_conn.at(i).at(j)->getPortIndex(QtNodes::PortType::Out))
                            {
                                is_last = true;
                            }
                        }

                        if(is_second && !is_last && in_conn.at(i).at(j)->getNode(QtNodes::PortType::Out) != nullptr )
                        {
                            SPHVSDB(std::cout << "check loop input source " << std::endl;)

                            QtNodes::Node* n_source = (in_conn.at(i).at(j)->getNode(QtNodes::PortType::Out));
                            int index = node_indices[n_source];
                            if(!NodeFinished[index])
                            {
                                missing = true;
                                break;
                            }
                        }

                    }


                    //if none is missing, start the loop
                    if(!missing)
                    {
                        SPHVSDB(std::cout <<  "//////solve loop with name " << n_check->nodeDataModel()->name().toStdString() << n_check<< std::endl;)

                        QList<QtNodes::Node * > n_loops;
                        n_loops.append(nodes.at(i));
                        bool loop_is_new = true;

                        QList<QList<QtNodes::Node *>> subnodes;

                        QList<QList<QString>> n_loop_variables;
                        QList<QList<QString>> n_loop_variables_types;
                        QList<QList<QString>> n_loop_variables_initnames;

                        while(n_loops.length() > 0)
                        {
                            SPHVSDB(std::cout << "////start loop follow " << n_loops.length() << std::endl;)

                            SPHVSDB(std::cout << "//nested loop list length " << n_loops.length() << std::endl;)


                            QtNodes::Node * n_loop = n_loops.at(n_loops.length()-1);
                            int index_loop = node_indices[n_loop];

                            //starting the loop
                            if(loop_is_new)
                            {
                                SPHVSDB(std::cout <<  "//add loop code " << std::endl;)

                                subnodes.append(QList<QtNodes::Node*>());

                                n_loop_variables.append(QList<QString>());
                                n_loop_variables_types.append(QList<QString>());
                                n_loop_variables_initnames.append(QList<QString>());


                                //declare loop variables

                                //initialize variables

                                //get number of variables and types

                                SPHVSDB(std::cout <<  "variable decleration " << std::endl;)

                                for(int k = 0; k < n_loop->nodeDataModel()->nPorts(QtNodes::PortType::Out); k = k+2)
                                {
                                    n_loop_variables[n_loop_variables.length()-1].append("var_" +QString::number(var_inout_count));
                                    var_inout_count ++;
                                    n_loop_variables_types[n_loop_variables.length()-1].append(m_ScriptManager->m_Engine->GetTypeNameFromUIName(n_loop->nodeDataModel()->dataType(QtNodes::PortType::Out,k).name));

                                }

                                for(int k = 0; k < n_loop->nodeDataModel()->nPorts(QtNodes::PortType::Out); k = k+2)
                                {
                                    if(k != n_loop->nodeDataModel()->nPorts(QtNodes::PortType::Out) -1)
                                    {
                                        n_loop_variables_initnames[n_loop_variables.length()-1].append(in_conn_codenames.at(index_loop).at(k));
                                    }
                                }

                                //declare and initialize all of them before starting the loop

                                SPHVSDB(std::cout <<  "variable initialization " << std::endl;)
                                for(int k = 0; k < n_loop_variables[n_loop_variables.length()-1].length(); k++)
                                {
                                    Codes.append(n_loop_variables_types[n_loop_variables.length()-1].at(k) + " " + n_loop_variables[n_loop_variables.length()-1].at(k) + " = " + n_loop_variables_initnames[n_loop_variables.length()-1].at(k) + ";");
                                    CodeLineNodes.append(n_loop);
                                }

                                //propagate output
                                SPHVSDB(std::cout <<  "propagate output " << std::endl;)
                                //all outputs
                                for(int j = 0; j < out_conn.at(i).size(); j++)
                                {
                                    QtNodes::Node * n3 =out_conn.at(index_loop).at(j)->getNode(QtNodes::PortType::In);
                                    if(n3 != nullptr )
                                    {
                                       {
                                           int index2 = node_indices[n3];
                                           int portindex_in = out_conn.at(index_loop).at(j)->getPortIndex(QtNodes::PortType::In);
                                           int portindex_out = out_conn.at(index_loop).at(j)->getPortIndex(QtNodes::PortType::Out);
                                           QString varname = n_loop_variables[n_loop_variables.length()-1].at(portindex_in/2);
                                           out_conn_codenames.at(index_loop).at(j) = varname;
                                           for(int k = 0; k < in_conn.at(index2).size(); k++)
                                           {
                                                if(in_conn.at(index2).at(k)->getPortIndex(QtNodes::PortType::In) == portindex_in)
                                                {
                                                    in_conn_codenames.at(index2).at(k) = varname;
                                                    break;
                                                }
                                           }
                                       }
                                    }
                                }


                                //name of final input in the loop (either condition or number of iterations
                                QString var_in_loop_condition_name = in_conn_codenames.at(index_loop).at(in_conn_codenames.at(index_loop).size()-1);

                                //start loop

                                NodeLoopConditionName[index_loop] = "var_" +QString::number(var_inout_count);
                                var_inout_count++;


                                SPHVSDB(std::cout <<  "start loop code" << std::endl;)

                                //if loop
                                if(n_loop->nodeDataModel()->name() == "Loop")
                                {
                                    QString nlctype = "int";

                                    Codes.append(QString(""));
                                    CodeLineNodes.append(nullptr);
                                    Codes.append(QString("for(int " + NodeLoopConditionName[index_loop] + " = 0; " + NodeLoopConditionName[index_loop] + " < " + var_in_loop_condition_name + "; " +  NodeLoopConditionName[index_loop] + "++)"));
                                    CodeLineNodes.append(n_loop);
                                    Codes.append(QString("{"));
                                    CodeLineNodes.append(n_loop);
                                    Codes.append(QString(""));
                                    CodeLineNodes.append(nullptr);

                                //else while loop
                                }else
                                {
                                    QString nlctype = "bool";

                                    Codes.append(QString(""));
                                    CodeLineNodes.append(nullptr);
                                    Codes.append(QString("while(" + var_in_loop_condition_name  + ")"));
                                    CodeLineNodes.append(n_loop);
                                    Codes.append(QString("{"));
                                    CodeLineNodes.append(n_loop);
                                    Codes.append(QString(""));
                                    CodeLineNodes.append(nullptr);
                                }



                                SPHVSDB(std::cout <<  "initialize loop following" << std::endl;)

                                //follow loop nodes

                                //fill with initial nodes that follow from loop
                                for(int j = 0; j < out_conn.at(index_loop).size(); j++)
                                {
                                    //we only, for now, follow the odd-numbered outputs that are for the loop itself
                                    bool is_second =  ((out_conn.at(index_loop).at(j)->getPortIndex(QtNodes::PortType::Out) %2) == 1);

                                    if(is_second && out_conn.at(index_loop).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                                    {
                                       subnodes[subnodes.length()-1].insert(subnodes[subnodes.length()-1].begin(),out_conn.at(index_loop).at(j)->getNode(QtNodes::PortType::In));
                                    }
                                }

                                loop_is_new = false;

                            }


                            //check all sub-nodes, including loops but excluding sub-loop nodes
                            while(subnodes[subnodes.length()-1].length() > 0)
                            {

                                bool finished_nonloop_inloop = false;


                                bool did_node = false;

                                for(int l = 0; l < subnodes[subnodes.length()-1].length(); l++)
                                {
                                    QtNodes::Node * n2 = subnodes[subnodes.length()-1].at(l);


                                    int index = node_indices[n2];

                                    SPHVSDB(std::cout << "check node " << n2->nodeDataModel()->name().toStdString() << n2 << " "<< NodeFinished[index] << std::endl;)

                                    //the node is already finished?????
                                    if(NodeFinished[index] == true)
                                    {
                                       std::cout << "ERROR DUE TO LOOP NODE BEING RE-USED" << std::endl;
                                       m_LineInfoMutex.unlock(); return Codes;
                                       //error!!!
                                       //there is an infinite loop
                                       //node n2 has already been seen in a loop, and is now used again


                                    }

                                    //this one ends in our loop node, so there is closure
                                    if(n2 == n_loops.at(n_loops.length()-1))
                                    {
                                        subnodes[subnodes.length()-1].removeAt(0);
                                        finished_nonloop_inloop = true;
                                        continue;
                                    }

                                    //if subnode is another loop, continue directly with its output
                                    if(n2->nodeDataModel()->name() == "Loop" || n2->nodeDataModel()->name() == "While")
                                    {
                                    }else if(n2->nodeDataModel()->name() == "Variable")
                                    {
                                        //if it is a variable, the setting occurs from the sending node
                                        subnodes[subnodes.length()-1].removeAt(0);
                                        finished_nonloop_inloop = true;
                                    }else
                                    {
                                        SPHVSDB(std::cout <<  "//check node with name " << n2->nodeDataModel()->name().toStdString() << n2 <<  std::endl;)


                                        //is all directly required input there
                                        //check if there is missing input
                                        bool missing2 = false;

                                        for(int j = 0; j < in_conn.at(index).size(); j++)
                                        {

                                            QtNodes::Node* n_source = (in_conn.at(index).at(j)->getNode(QtNodes::PortType::Out));

                                            //our parent loop node is not finished, but the output is oke
                                            if(!(n_source == n_loops.at(n_loops.length() -1)))
                                            {

                                                int index2 = node_indices[n_source];
                                                if(!NodeFinished[index2])
                                                {
                                                    missing = true;
                                                    break;
                                                }
                                            }else {

                                            }
                                        }


                                        //if non is missing
                                        if(!missing2)
                                        {

                                            //if subnode is another type of node, continue to process it and following its output


                                            //important check
                                            //for a given node, is all the input either part of the loop itself or a variable
                                            //otherwise, there is a conflict in the flow scheme
                                            //(a sub-loop node connects to a post-loop node or there is mixing of loops)
                                            //two conditions for this check
                                            //do the inputs come from some node with the same parent node
                                            //otherwise, are they a variable

                                            bool loop_mix_found = false;

                                            for(int k = 0; k < in_conn.at(index).size(); k++)
                                            {
                                                //get source node
                                                QtNodes::Node * n3 = in_conn.at(index).at(k)->getNode(QtNodes::PortType::Out);
                                                if(n3 != nullptr)
                                                {
                                                    SPHVSDB(std::cout << "checking if connected source node is part of same loop " <<std::endl;)
                                                    //check if it is either outside of loop, or in same loop
                                                    int index2 = node_indices[n3];

                                                    SPHVSDB(std::cout << "checking for : " << n3->nodeDataModel()->name().toStdString() << n3 << "  upper loop node" << UpperLoopNode[index2] << " current " << n_loop << std::endl;)
                                                    if(!((UpperLoopNode[index2] == nullptr) || (UpperLoopNode[index2] == n_loop) || n3 == n_loop))
                                                    {
                                                          loop_mix_found = true;
                                                          break;
                                                    }
                                                }
                                            }

                                            if(loop_mix_found)
                                            {
                                                SPHVSDB(std::cout << "ERROR loop node mixing is not allowed" << std::endl;)

                                                m_LineInfoMutex.unlock(); return Codes;
                                                //error!!!
                                            }


                                            //set this node as finished
                                            UpperLoopNode[index] = n_loop;
                                            NodeFinished[index] = true;

                                            SPHVSDB(std::cout <<  "//solve node with name " << n2->nodeDataModel()->name().toStdString() << std::endl;)


                                            //variables
                                            //get the name of all the input variables
                                            //in_conn_codenames should be filled with these
                                            //furthermore, should automatically be sorted according to the input ports
                                            std::vector<QString> inputnames = in_conn_codenames.at(index);

                                            SPHVSDB(std::cout << "prepare input and output names" << std::endl;)
                                            //now prepare the output names
                                            std::vector<QString> outputnames;

                                            //function
                                            FunctionalNodeModel* funcnode = dynamic_cast<FunctionalNodeModel*>(n2->nodeDataModel());

                                            for(int j = 0; j < n2->nodeDataModel()->nPorts(QtNodes::PortType::Out); j++)
                                            {
                                                if((funcnode->FirstOutputIsFirstInput()) && j == 0)
                                                {
                                                    outputnames.insert(outputnames.end(),inputnames.at(0));

                                                }else
                                                {
                                                    outputnames.insert(outputnames.end(),"var_" +QString::number(var_inout_count));
                                                    var_inout_count ++;
                                                }

                                            }


                                            SPHVSDB(std::cout << "get function code" << std::endl;)


                                            //save the results into the new variables
                                            QList<QString> node_code = funcnode->GetCodeLine(inputnames,outputnames, m_ScriptManager->m_Engine);

                                            //add code lines
                                            SPHVSDB(std::cout << "add function lines" << std::endl;)
                                            for(int j = 0; j < node_code.size(); j++)
                                            {
                                                QString node_code_line = node_code.at(j);
                                                Codes.append(node_code_line);
                                                CodeLineNodes.append(n2);
                                            }

                                            SPHVSDB(std::cout << "check variables" << std::endl;)
                                            //variable check
                                            //check if next nodes are variables, if so, set these
                                            for(int j = 0; j < out_conn.at(i).size(); j++)
                                            {
                                                QtNodes::Node * n3 =out_conn.at(i).at(j)->getNode(QtNodes::PortType::In);
                                                if(n3 != nullptr )
                                                {
                                                   if(n3->nodeDataModel()->name() == "Variable")
                                                   {
                                                       int index2 = node_indices[n3];
                                                       int portindex_out = out_conn.at(index2).at(j)->getPortIndex(QtNodes::PortType::Out);
                                                       QString varname = outputnames.at(portindex_out);

                                                       NodeIsVariableIsAssigned[index2] = true;
                                                       Codes.append(NodeVariableName[index2] + " = " + varname + ";");
                                                       CodeLineNodes.append(n3);
                                                   }
                                                }
                                            }

                                            SPHVSDB(std::cout << "propagate output" << std::endl;)
                                            //output propagation
                                            //set input names for new nodes
                                            for(int j = 0; j < out_conn.at(index).size(); j++)
                                            {
                                                QtNodes::Node * n3 =out_conn.at(index).at(j)->getNode(QtNodes::PortType::In);
                                                if(n3 != nullptr )
                                                {
                                                   {
                                                       SPHVSDB(std::cout << "propagate to " << n3->nodeDataModel()->name().toStdString() << n3 << std::endl;)

                                                       int index2 = node_indices[n3];
                                                       int portindex_in = out_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::In);
                                                       int portindex_out = out_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::Out);
                                                       QString varname = outputnames.at(portindex_out);

                                                       out_conn_codenames.at(index).at(j) = varname;

                                                       for(int k = 0; k < in_conn.at(index2).size(); k++)
                                                       {
                                                            if(in_conn.at(index2).at(k)->getPortIndex(QtNodes::PortType::In) == portindex_in)
                                                            {
                                                                in_conn_codenames.at(index2).at(k) = varname;
                                                                break;
                                                            }
                                                       }
                                                   }
                                                }
                                            }


                                            //add any followup nodes to the subnodes list

                                            SPHVSDB(std::cout << "add followup nodes" << std::endl;)

                                            for(int j = 0; j < out_conn.at(index).size(); j++)
                                            {
                                                //we follow all outputs for a normal node
                                                if(out_conn.at(index).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                                                {
                                                   //add this to the subnodes
                                                   subnodes[subnodes.length()-1].insert(subnodes[subnodes.length()-1].end(),out_conn.at(index).at(j)->getNode(QtNodes::PortType::In));
                                                }
                                            }


                                            SPHVSDB(std::cout << "finished this node" << std::endl;)
                                            //finished the node, so remove
                                            subnodes[subnodes.length()-1].removeAt(0);
                                            finished_nonloop_inloop = true;
                                        }
                                    }

                                }


                                //if we did not finish a non-loop, try to start a new sub-loop

                                if(finished_nonloop_inloop == false)
                                {

                                    for(int l = 0; l < subnodes[subnodes.length()-1].length(); l++)
                                    {
                                        QtNodes::Node * n2 = subnodes[subnodes.length()-1].at(l);

                                        int index = node_indices[n2];

                                        //if subnode is another loop, continue directly with its output
                                        if(n2->nodeDataModel()->name() == "Loop" || n2->nodeDataModel()->name() == "While")
                                        {

                                            SPHVSDB(std::cout <<  "//check loop with name " << n2->nodeDataModel()->name().toStdString() << " " << n2 <<  std::endl;)


                                            //check if anything is missing
                                            bool missing2 = false;


                                            for(int j = 0; j < in_conn.at(index).size(); j++)
                                            {

                                                //we only, for now, require the even-numbered inputs which are not part of the loop
                                                bool is_second = ((in_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::In) %2) == 0);
                                                //for aloop, do not select the last input (this input is the condition for the loop)
                                                bool is_last = false;
                                                {
                                                    if(n2->nodeDataModel()->nPorts(QtNodes::PortType::Out)-1 ==in_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::Out))
                                                    {
                                                        is_last = true;
                                                    }
                                                }

                                                if(is_second && !is_last && in_conn.at(index).at(j)->getNode(QtNodes::PortType::Out) != nullptr )
                                                {
                                                    QtNodes::Node* n_source = (in_conn.at(index).at(j)->getNode(QtNodes::PortType::Out));
                                                    int index2 = node_indices[n_source];
                                                    if(!NodeFinished[index2])
                                                    {
                                                        missing = true;
                                                        break;
                                                    }
                                                }

                                            }

                                            //if non are missing
                                            if(!missing2)
                                            {
                                                SPHVSDB(std::cout <<  "//go into loop with name " << n2->nodeDataModel()->name().toStdString() << " " << n2 <<  std::endl;)


                                                //add its children to the subnodes
                                                for(int j = 0; j < out_conn.at(index).size(); j++)
                                                {
                                                    //we only, for now, follow the even-numbered outputs that are output
                                                    bool is_second = ((out_conn.at(index).at(j)->getPortIndex(QtNodes::PortType::Out) %2) == 0);

                                                    if(is_second && out_conn.at(index).at(j)->getNode(QtNodes::PortType::In) != nullptr )
                                                    {
                                                       //add this to the subnodes
                                                       subnodes[subnodes.length()-1].insert(subnodes[subnodes.length()-1].end(),out_conn.at(index).at(j)->getNode(QtNodes::PortType::In));
                                                    }
                                                }

                                                subnodes[subnodes.length()-1].removeAt(0);

                                                //initiate a new while loop starting at this loop parent
                                                UpperLoopNode[index] = n_loop;
                                                loop_is_new = true;
                                                n_loops.append(n2);

                                            }
                                        }

                                        if(loop_is_new)
                                        {
                                            break;
                                        }
                                    }

                                }

                                if(loop_is_new)
                                {
                                    break;
                                }

                                if(finished_nonloop_inloop == false)
                                {


                                    {
                                        if(subnodes[subnodes.length()-1].length() > 0)
                                        {
                                            SPHVSDB(std::cout << "not all dependencies can be resolved";)

                                            m_LineInfoMutex.unlock(); return Codes;
                                            //error!!!
                                            //if we couldnt finish a node, and the list is not empty
                                            //there needs to be an error, we are stuck

                                        }
                                    }
                                }

                            }

                            //did we not->exit the while loop to start following an inner loop?
                            if(!loop_is_new)
                            {
                                SPHVSDB(std::cout <<  "//finish loop with name " << n_loop->nodeDataModel()->name().toStdString() << n_loop << std::endl;)

                                //we finished the outer loop, subnodes for this loop are empty

                                SPHVSDB(std::cout << "check if loop can be finished " << std::endl;)
                                //check if now actually we can finish the loop
                                bool loop_input_full = true;
                                for(int j = 0; j < in_conn.at(index_loop).size(); j++)
                                {
                                    //now we check all
                                    {
                                        QtNodes::Node* n_source = (out_conn.at(index_loop).at(j)->getNode(QtNodes::PortType::Out));
                                        int index2 = node_indices[n_source];
                                        if(!NodeFinished[index2])
                                        {
                                            loop_input_full = true;
                                            break;
                                        }
                                    }
                                }
                                //if not, an error must be thrown
                                if(!loop_input_full)
                                {
                                    SPHVSDB(std::cout << "ERROR finished loop is not a closed system"<< std::endl;)


                                    m_LineInfoMutex.unlock(); return Codes;
                                    //error!!!
                                }


                                SPHVSDB(std::cout << "update variables " << std::endl;)

                                //update variable values

                                for(int k = 0; k < n_loop_variables[n_loop_variables.length()-1].length(); k++)
                                {
                                    Codes.append(n_loop_variables[n_loop_variables.length()-1].at(k) + " = " + in_conn_codenames.at(index_loop).at(k*2+1) + ";");

                                    if(in_conn_codenames.at(index_loop).at(k*2+1).isEmpty())
                                    {
                                        SPHVSDB(std::cout << "ERROR not all loop variables can be updated"<< std::endl;)

                                        m_LineInfoMutex.unlock(); return Codes;

                                    }
                                    CodeLineNodes.append(n_loop);
                                }

                                SPHVSDB(std::cout << "close loop " << std::endl;)


                                //closing the loop

                                Codes.append(QString(""));
                                CodeLineNodes.append(nullptr);
                                Codes.append(QString("}"));
                                CodeLineNodes.append(n_loop);
                                Codes.append(QString(""));
                                CodeLineNodes.append(nullptr);

                                //propagation of output is already done when loop is initiated

                                SPHVSDB(std::cout << "remove stuff " << std::endl;)

                                n_loop_variables.removeLast();
                                n_loop_variables_types.removeLast();
                                n_loop_variables_initnames.removeLast();

                                subnodes.removeLast();
                                n_loops.removeLast();

                                SPHVSDB(std::cout << "remove stuff " << std::endl;)

                                NodeFinished[index_loop] = true;

                                SPHVSDB(std::cout << "done" << std::endl;)

                            //if we did exit the while loop to start following an inner loop, do nothing
                            //we wont get here, since loop_is_new does a continue earlier
                            }else{}


                        }

                        finished_loop = true;

                    }
                }
            }
        }



        //are we finished yet?

        is_finished = true;
        for(int i = 0; i < nodes.size(); i++)
        {
            if(NodeFinished[i] == false)
            {
                is_finished = false;
            }
        }

        //check if we did add any at all, if not, we are stuck and need to report an error message

        if(!finished_loop && !finished_nonloop && is_finished == false)
        {
            SPHVSDB(std::cout << "ERROR could not resolve all dependencies " << std::endl;)
            m_LineInfoMutex.unlock(); return Codes;
            //error
        }

    }

    SPHVSDB(std::cout << "/////////////---Finished all " << std::endl;)

    //finalize the return values

    popupMessage = "Sucesfully compiled code!";
    emit int_emit_show_temporary_dialog();

    is_compilesucceed = true;
    m_Codes = Codes;
    m_CodeLineNodes = CodeLineNodes;

    m_LineInfoMutex.unlock(); return Codes;

}


