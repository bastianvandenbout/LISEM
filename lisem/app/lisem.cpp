

#include "lisem.h"
#include "error.h"
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "downloadmanager.h"
#include "QPixmap"
#include "QSplashScreen"
#include "QMessageBox"
#include "resourcemanager.h"
#include "gl/openglcldatamanager.h"
#include <QDesktopServices>
#include <QSysInfo>
#include <QUrl>
#include <QProcess>
#include "omp.h"
#include "QThread"
#include "QString"
#include <stdlib.h>
#include <mpi.h>
#include <cstdlib>
#include <QtConcurrent/QtConcurrent>
#include "site.h"

typedef unsigned int uint;






SPHazard::SPHazard()
{


}

void SPHazard::processTimeStep()
{
    m_model->OnStep();
}

void SPHazard::renderFrame()
{
    m_model->OnDraw();
}

void SPHazard::OnGLCLLoad()
{




}

bool SPHazard::OnGLCLFrame()
{

    processTimeStep();

    renderFrame();

    //return true only if the model has altered data due to a new timestep
    return false;
}

int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    {
        n += 1;
    }
    return n;
}


char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}
QList<QString> getCmdOptions(char ** begin, char ** end, const std::string & option)
{
    QList<QString> options;
    char ** itr = std::find(begin, end, option);
    while(itr != end && ++itr != end)
    {

        QString her = *itr;

        if(her.startsWith("-"))
        {
            break;
        }
        options.append(her);

    }
    return options;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int SPHazard::execute(int argc, char *argv[])
{

    bool use_commandline  = false;

    if(cmdOptionExists(argv, argv+argc, "-r")|| cmdOptionExists(argv, argv+argc, "-v") || cmdOptionExists(argv, argv+argc, "-o") || cmdOptionExists(argv, argv+argc, "-h") || cmdOptionExists(argv, argv+argc, "-c") || cmdOptionExists(argv, argv+argc, "-m") || cmdOptionExists(argv, argv+argc, "-d"))
    {
        use_commandline = true;
    }

    bool quit_direct = false;
    bool quit_post = false;



    InitMessages(use_commandline);
    InitMessagesS(use_commandline);
    LISEM_STATUS("============================");
    LISEM_STATUS("=        LISEM Model        ");
    LISEM_STATUS("=   see www.lisemmodel.com  ");
    LISEM_STATUS("=   ITC - Twente University ");
    LISEM_STATUS("============================");
    LISEM_STATUS("");
    LISEM_STATUS("System info:");
    LISEM_STATUS(QString("OS:   ") + (QSysInfo::kernelType() + " version: "+ QSysInfo::productVersion()));
    LISEM_STATUS(QString("PPN:  ") + QSysInfo::prettyProductName());
    LISEM_STATUS(QString("ARCH: ") + QSysInfo::currentCpuArchitecture());
    LISEM_STATUS(QString("OMP:  ") + QString::number(omp_thread_count()));
    LISEM_STATUS(QString("ITC:  ") + QString::number(QThread::idealThreadCount()));
    LISEM_STATUS("");

    //allow for usage from command line

    if(cmdOptionExists(argv, argv+argc, "-h"))
    {
        quit_direct = true;
        LISEM_STATUS("Help info for lisem_app.exe");
        LISEM_STATUS("-h Help information");
        LISEM_STATUS("-r file.script - run script file with ui, script is first found in working directory, then in executable path");
        LISEM_STATUS("-c file.script - try to compile script file, script is first found in working directory, then in executable path");
        LISEM_STATUS("-d path/to/dir - set working directory, without this option, executable working directory is used");
        LISEM_STATUS("-m file.run - run a model by opening a .run file");
        LISEM_STATUS("-mo 'option1=value|option2=value2|...'");
        LISEM_STATUS("-v data1.tif data2.shp ... - view a data files in the viewer window");
        LISEM_STATUS("-o 'a.map = sqrt(b.tif*c.map)' - carry out a calculation");
        LISEM_STATUS("-x - open with interface");
        LISEM_STATUS("-q - quite when done (when using interface)");


        m_App = new QApplication(argc, argv);
        m_App->quit();

        int initialized = false;
        MPI_Initialized(&initialized);
        if(initialized)
        {
            MPI_Finalize();
        }

        std::exit( EXIT_SUCCESS );

        return 0;

    }

    QString dir = QDir::currentPath();
    bool has_dir = false;

    QString file = "";
    QString modeloptions = "";
    bool run_file =false;
    if(cmdOptionExists(argv, argv+argc, "-r"))
    {
        run_file = true;
        char * filename = getCmdOption(argv, argv + argc, "-r");
        if(!filename)
        {
            LISEM_STATUS("Could not find option for -r, no file to run");
            return 0;
        }

        file = filename;
    }
    bool compile_file = false;

    if(cmdOptionExists(argv, argv+argc, "-c"))
    {
        if(run_file)
        {
            LISEM_STATUS("Already asked to run a file, so not compiling another");
        }else
        {
            compile_file = true;
            char * filename = getCmdOption(argv, argv + argc, "-c");

            if(!filename)
            {
                LISEM_STATUS("Could not find option for -c, no file to compile");
                return 0;
            }

            file = filename;
            if(!QFileInfo(file).isAbsolute())
            {
                file = QFileInfo(dir + "/"+ file).absoluteFilePath();
            }
        }
    }



    bool do_interface = false;
    if(cmdOptionExists(argv, argv+argc, "-x"))
    {
        do_interface = true;
    }

    bool do_calc = false;
    if(cmdOptionExists(argv, argv+argc, "-o"))
    {
        if(run_file || compile_file)
        {
            LISEM_STATUS("Already asked to run/compile a file or model, so not running calc");
        }else
        {
            do_calc = true;
            char * filename = getCmdOption(argv, argv + argc, "-o");

            if(!filename)
            {
                LISEM_STATUS("Could not find option for -o, no string to calculate from");
                return 0;
            }

            //is actually a calculation string
            file = filename;

        }
    }

    bool run_model = false;
    if(cmdOptionExists(argv, argv+argc, "-m"))
    {
        if(run_file || compile_file||do_calc)
        {
            LISEM_STATUS("Already asked to run/compile a file, so not running model");
        }else
        {
            run_model = true;
            char * filename = getCmdOption(argv, argv + argc, "-m");

            if(!filename)
            {
                LISEM_STATUS("Could not find option for -m, no run-file provided");
                return 0;
            }

            file = filename;
            if(!QFileInfo(file).isAbsolute())
            {
                file = QFileInfo(dir + "/"+ file).absoluteFilePath();
            }
        }

        if(cmdOptionExists(argv, argv+argc, "-mo"))
        {
            char * options = getCmdOption(argv, argv + argc, "-o");

            if(!options)
            {
                LISEMS_STATUS("Empty model options");
            }
            modeloptions = options;
        }
    }


    if(cmdOptionExists(argv, argv+argc, "-d"))
    {
        has_dir = true;
        char * filename = getCmdOption(argv, argv + argc, "-d");
        if(!filename)
        {
            LISEM_STATUS("Could not find option for -d, no directory to set");
            return 0;
        }
        dir = filename;
        if(!QDir(dir).isAbsolute())
        {
            dir = QDir::cleanPath(QDir(dir).absoluteFilePath(filename));
        }
    }


    QList<QString> files;
    bool do_view = false;
    if(cmdOptionExists(argv, argv+argc, "-v"))
    {
        if(run_file || compile_file||run_model||do_calc)
        {
            LISEM_STATUS("Already asked to run/compile/calc a file or model, so not opening data");
        }else
        {
            do_view = true;
            QList<QString> filename = getCmdOptions(argv, argv + argc, "-v");

            if(filename.length()== 0)
            {
                LISEM_STATUS("Could not find option for -v, no data to open");
                return 0;
            }

            files = filename;
            //if(has_dir)
            {
                for(int i= 0; i < files.size(); i++)
                {
                    if(!QDir(files[i]).isAbsolute())
                    {
                        files[i] = file = QFileInfo(dir + "/"+ file[i]).absoluteFilePath();
                    }
                }
            }

        }
    }

    if(do_view)
    {
        do_interface = true;
    }
    if(cmdOptionExists(argv, argv+argc, "-q"))
    {
        quit_post = true;
        // Do stuff
    }



    if(quit_direct)
    {
        int initialized = false;
        MPI_Initialized(&initialized);
        if(initialized)
        {
            MPI_Finalize();
        }

        return 0;
    }

    if(!use_commandline)
    {
        do_interface = true;
    }




    LISEM_STATUS("");
    LISEM_DEBUG("Initiated Messages");
    LISEM_DEBUG("Updating console");

    //we need the path of the executable
    //to set some environment variable for gdal
    //Setting this needs to happen before initializing application
    //thus finally, we have to initiate QApplication twice
    m_App = new QApplication(argc, argv);

    QSplashScreen *splash;

    if(do_interface)
    {
        QPixmap pixmap(QCoreApplication::applicationDirPath() + LISEM_FOLDER_ASSETS + "splash.png");
        splash = new QSplashScreen(pixmap);
        splash->show();
    }

    //path of the executable
    Dir = QCoreApplication::applicationDirPath();
    m_App->quit();

    SetSite(Dir);
    InitializeGeneralSettingsManager(Dir);
    InitializeResourceManager();

    PreInitializeGDAL(Dir);
    bool restart = true;
    bool first = true;

    while(restart)
    {

        m_App = new QApplication(argc, argv);


        //some things need to be initialized only once (c-style libraries in particular)
        if(first)
        {
            InitializeGDAL();
            first = false;
        }
        //set window icon
        QIcon icon(QIcon(Dir + "/assets/" + "logo.png"));
        m_App->setWindowIcon(icon);
        QPixmap iconp = icon.pixmap(48,48);


        //load all the managers
        //there is no specific order required, except
        //that some require others as input
        m_ScriptManager = new ScriptManager();


        //m_IOManager = new IOManager();
        m_OpenGLCLManager = new OpenGLCLManager();
        m_AudioManager = new AudioManager();
        m_ParameterManager = new ParameterManager();
        m_model = new LISEMModel(m_OpenGLCLManager,m_ParameterManager);
        //m_model->InitModel();
        SetGlobalModel(m_model);
        m_InterfaceWindow = new QTInterfaceWindow();

        InitDownloadManager();
        InitScriptManager(m_ScriptManager);

        QLocale loc = QLocale::system(); // current locale
        loc.setNumberOptions(QLocale::c().numberOptions()); // borrow number options from the "C" locale
        QLocale::setDefault(loc);

        LISEM_STATUS("OPENING MODEL");

        restart = false;

        bool init = false;
        try
        {

            m_ScriptManager->Initialize();
            m_ParameterManager->InitParameters();

            if(do_interface)
            {

            }
            if(do_interface)
            {

                int suc = m_OpenGLCLManager->CreateGLWindow(iconp);
                if(suc > 0)
                {
                    if(suc == 254)
                    {
                        QMessageBox::warning(
                            nullptr,
                            tr("LISEM"),
                            tr("Could not create OpenGL Context with OpenGL 4.0 support!") );
                    }
                    throw 1;
                }

                suc = m_OpenGLCLManager->InitGLCL();

                if(suc > 0)
                {
                    throw 1;
                }
            }else if(run_model)
            {
                int suc = m_OpenGLCLManager->CreateGLWindow(iconp, false);
                if(suc > 0)
                {
                    if(suc == 254)
                    {
                        QMessageBox::warning(
                            nullptr,
                            tr("LISEM"),
                            tr("Could not create OpenGL Context with OpenGL 4.0 support!") );
                    }
                    throw 1;
                }

                suc = m_OpenGLCLManager->InitGLCL();

                if(suc > 0)
                {
                    throw 1;
                }
                glfwMakeContextCurrent(NULL);

            }
            if(do_interface)
            {
                InitUIShaders(m_OpenGLCLManager);


                m_OpenGLCLManager->SetCallBackFrame(&SPHazard::OnGLCLFrame,this);


                m_AudioManager->SetupDefaultOutputDevice();

            }


            m_model->InitModel();

            if(do_interface)
            {

                m_WorldPainter = new WorldWindow(m_OpenGLCLManager);
                m_WorldPainter->SetModel(m_model);

                m_OpenGLCLManager->SetCallBackFrame(&WorldWindow::Draw,m_WorldPainter);
                glfwMakeContextCurrent(NULL);

            }


            init = true;

        }
        catch (int error)
        {

            QList<LeveledMessage> list = GetMessages();
            QString errorstext;
            for(int i = 0; i < list.length(); i++)
            {
                std::cout << list.at(i).Message.toStdString() << std::endl;
                errorstext += list.at(i).Message + "\n";
            }

            LISEM_ERROR("Error in initialization of main application framework. Code " + QString::number(error));

            //report all messages to file
            if(!WriteMessagesToFile(list,Dir + "/errors.txt"))
            {
                QMessageBox msgBox;
                msgBox.setText("Fatal error! \n Could not write messages to file!");
                msgBox.setDetailedText(errorstext);
                msgBox.exec();
            }else {
                QMessageBox msgBox;
                msgBox.setText("Fatal error! \n Error messages written to " + Dir + "/errors.txt");
                msgBox.setDetailedText(errorstext);
                msgBox.exec();
            }


        }
        catch (...)
        {
            QList<LeveledMessage> list = GetMessages();

            QString errorstext;
            for(int i = 0; i < list.length(); i++)
            {
                std::cout << list.at(i).Message.toStdString() << std::endl;
                errorstext += list.at(i).Message + "\n";
            }

            LISEM_ERROR("Unknown error in initialization of main application framework.");

            //report all messages to file
            if(!WriteMessagesToFile(list,Dir + "/errors.txt"))
            {
                QMessageBox msgBox;
                msgBox.setText("Fatal error! \n Could not write messages to file!");
                msgBox.setDetailedText(errorstext);
                msgBox.exec();
            }else {
                QMessageBox msgBox;
                msgBox.setText("Fatal error! \n Error messages written to " + Dir + "/errors.txt");
                msgBox.setDetailedText(errorstext);
                msgBox.exec();
            }

        }

        if(init)
        {
            if(use_commandline && !do_interface)
            {


                //these all share some code
                if(run_file || compile_file || do_calc)
                {

                    //get script to do stuff with

                    bool file_read = false;

                    QString scripttext;
                    if(run_file || compile_file)
                    {

                        file_read = true;



                        QFile fin(file);
                        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

                            file_read = false;
                        }else
                        {
                            while (!fin.atEnd())
                            {
                                QString S;

                                    S = fin.readLine();


                                scripttext += "\n" + S;
                            }
                        }

                    }else if(do_calc)
                    {
                        file_read = true;
                        scripttext = file;
                    }

                    SPHScript *s = new SPHScript();
                    s->SetCode(scripttext);
                    if(do_calc)
                    {
                        s->SetSingleLine(true);
                    }else
                    {
                        s->SetSingleLine(false);
                    }
                    s->SetPreProcess(true);
                    s->SetHomeDir(dir + "/");


                    if(file_read)
                    {

                        //now set up a compile/run request
                        s->SetCallBackPrint(std::function<void(SPHScript*,QString)>([](SPHScript*,QString) ->
                                                                                    void{


                            ;
                                                                                    }),s,std::placeholders::_1);

                        s->SetCallBackDone(std::function<void(bool x)>([]( bool finished) ->
                                                                                    void{


                            LISEMS_DEBUG("Done");
                            ;
                                                                                    }),std::placeholders::_1);

                        s->SetCallBackCompilerError(std::function<void(SPHScript*,const asSMessageInfo *msg)>([](SPHScript*,const asSMessageInfo *msg) ->
                                                                                    void{
                            const char *type = "Error: ";
                            if( msg->type == asMSGTYPE_WARNING )
                            {
                                type = "Warning: ";
                            }
                            else if( msg->type == asMSGTYPE_INFORMATION )
                            {
                                type = "Info: ";
                            }
                            LISEMS_ERROR(QString(type) + " Line: (" + QString::number(msg->row) + " (" + QString::number(msg->col) + ") " + " : " + QString(msg->message));
                            ;



                                                                                    }),s,std::placeholders::_1);

                        s->SetCallBackException(std::function<void(SPHScript*,asIScriptContext *ctx)>([](SPHScript*,asIScriptContext *ctx) ->
                                                                                    void{

                            LISEMS_ERROR("Exception encountered when running script");

                                                    try
                                                      {
                                                        // Retrow the original exception so we can catch it again
                                                        throw;
                                                      }
                                                      catch( std::exception &e )
                                                      {
                                                        // Tell the VM the type of exception that occurred
                                                       LISEMS_ERROR("std::exception " +QString(e.what()));
                                                        //ctx->SetException(e.what());
                                                      }catch(int e)
                                                    {
                                                       LISEMS_ERROR("int exception " +QString::number(e));
                                                     }
                                                      catch(...)
                                                      {
                                                        LISEMS_ERROR("Unknown exception");
                                                        // The callback must not allow any exception to be thrown, but it is not necessary
                                                        // to explicitly set an exception string if the default exception string is sufficient
                                                      }



                            ;
                                                                                    }),s,std::placeholders::_1);

                        s->SetCallBackLine(std::function<void(SPHScript*,asIScriptContext *ctx)>([](SPHScript*,asIScriptContext *ctx) ->
                                           void{


                            ;
                                           }),s,std::placeholders::_1);

                        //compile it

                        m_ScriptManager->CompileScript_Generic(s);

                        //output errors
                        if(s->IsCompiled())
                        {

                        }else
                        {


                        }

                        if(s->IsCompiled() && !compile_file)
                        {
                            //if requested, run it


                            m_ScriptManager->RunScript(s);

                            //done


                        }
                    }


                }else if(run_model)
                {
                    //this is tricky, we need to make sure the model is not going to call the interface window, since it isnt set up yet,
                    //but instead we want to report to the command prompt how our progress is

                    //

                    m_OpenGLCLManager->ProcessEvents();

                    ParameterManager* m_ParameterManager = new ParameterManager();
                    m_ParameterManager->InitParameters();



                    LISEMModel * m = GetGLobalModel();
                    if(m != nullptr)
                    {
                        LISEM_STATUS("Starting LISEM Model");
                        LISEM_STATUS("Load run-file: " +file);
                        m->FinishMutex.lock();
                        m->m_ModelStateMutex.lock();
                        m->m_Options = m_ParameterManager->GetParameterListFromFile(file,modeloptions);
                        //m->m_RigidWorld = world;
                        m->m_StartRequested = true;
                        m->m_ModelStateMutex.unlock();

                        m->OnStep();

                        m->FinishCondition.wait(&m->FinishMutex);
                        m->FinishMutex.unlock();
                        m->SetCallBackTimeStep([](int step, int step_max,bool start, bool stop){


                            if(start)
                            {
                                LISEM_STATUS("LISEM Model started");
                                LISEM_STATUS("0 %");
                            }else if(stop)
                            {

                                LISEM_STATUS("100 %");
                            }else
                            {
                                LISEM_STATUS(QString::number(float(step)/float(step_max)) + " %");
                            }


                        });


                        AS_MODELRESULT res = m->FinishResult;


                        LISEM_STATUS("Finished running LISEM model");

                    }

                    glfwSetWindowShouldClose(m_OpenGLCLManager->window,GL_TRUE);




                }

            }else
            {
                std::thread t1 = std::thread((&OpenGLCLManager::GLCLLoop),m_OpenGLCLManager);

                m_InterfaceWindow->Create(m_ParameterManager, m_model,m_ScriptManager,m_WorldPainter);
                splash->finish(m_InterfaceWindow);
                m_InterfaceWindow->show();

                m_InterfaceWindow->SetScriptFunctions(m_ScriptManager);
                m_InterfaceWindow->Initialize(m_OpenGLCLManager,m_ScriptManager);

                m_OpenGLCLManager->ProcessEvents(); //QTimer::singleShot(0,m_OpenGLCLManager,SLOT(ProcessEvents));


                //set instructions from command line to interface window

                /*m_InterfaceWindow->SetCommandLineCallBack(std::function<void(void)>([this,run_file,compile_file,run_model,do_calc,do_view,file,files,has_dir,dir,quit_post](){

                    */std::cout << "command line callback" << std::endl;
                    if(run_file)
                    {
                        m_InterfaceWindow->SetCommandLineRun(file,has_dir,dir,quit_post);
                    }else if(compile_file)
                    {
                        m_InterfaceWindow->SetCommandLineCompile(file,has_dir,dir,quit_post);
                    }else if(run_model)
                    {
                        m_InterfaceWindow->SetCommandLineRunModel(file,has_dir,dir,quit_post);
                    }else if(do_calc)
                    {
                        m_InterfaceWindow->SetCommandLineRunCalc(file,has_dir,dir,quit_post);
                    }else if(do_view)
                    {
                        m_InterfaceWindow->SetCommandLineOpenFiles(files,has_dir,dir,quit_post);
                    };
                //}));



                //int qreturn  = 0;
                //std::this_thread::sleep_for(std::chrono::seconds(100));
                int qreturn = m_App->exec();

                m_App->processEvents();

                if(qreturn == LISEM_CODE_RETURN_RESTART)
                {
                    restart = true;
                }
                glfwSetWindowShouldClose(m_OpenGLCLManager->window,GL_TRUE);

                t1.join();

            }


        }

        LISEM_STATUS("Closing resources");

        if(init)
        {

            LISEM_STATUS("Closing window");

            m_model->Destroy();


            //m_InterfaceWindow->DestroyLater();
            m_InterfaceWindow->m_VisualScripter->m_QuitThread.store(true);
            m_App->closeAllWindows();
            m_InterfaceWindow->close();

            //m_IOManager->Destroy();
            m_ParameterManager->Destroy();
            m_OpenGLCLManager->Destroy();

        }

        //safe delete the main managers

        DestroyDownloadManager();
        DestroyScriptManager();

        DestroyUIShaders(m_OpenGLCLManager);

        //SAFE_DELETE(m_IOManager);
        SAFE_DELETE(m_OpenGLCLManager);
        SAFE_DELETE(m_ParameterManager);
        SAFE_DELETE(m_model);

        //we do not have to delete the m_InterfaceWindow
        //qt frees up all widgets related to the main windows and any children.
        //However, it does not set our own pointer to NULL, so we would delete memory that does not exist any longer.
        m_InterfaceWindow = NULL;
    }

    DestroyGeneralSettingsManager();

    int initialized = false;
    MPI_Initialized(&initialized);
    if(initialized)
    {
        MPI_Finalize();
    }


    std::exit( EXIT_SUCCESS );
    return 0;
}


