

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

#include <mpi.h>

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
        LISEM_STATUS("Help info for lisem_app.exe");
        LISEM_STATUS("-h Help information");
        LISEM_STATUS("-r file.script - run script file with ui, script is first found in working directory, then in executable path");
        LISEM_STATUS("-c file.script - try to compile script file, script is first found in working directory, then in executable path");
        LISEM_STATUS("-d path/to/dir - set working directory, without this option, executable path is used");
        LISEM_STATUS("-m file.run - run a model by opening a .run file");
        LISEM_STATUS("-v data1.tif data2.shp ... - view a data files in the viewer window");
        LISEM_STATUS("-o 'a.map = sqrt(b.tif*c.map)' - carry out a calculation");
        LISEM_STATUS("-x - open with interface");
        LISEM_STATUS("-q - quite when done");
    }
    QString file = "";
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
                LISEM_STATUS("Could not find option for -c, no file to compile");
                return 0;
            }

            file = filename;
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

    QString dir = QDir::currentPath();
    bool has_dir = false;

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
    }

    if(quit_direct)
    {
        return 0;
    }






    LISEM_STATUS("");
    LISEM_DEBUG("Initiated Messages");
    LISEM_DEBUG("Updating console");

    //we need the path of the executable
    //to set some environment variable for gdal
    //Setting this needs to happen before initializing application
    //thus finally, we have to initiate QApplication twice
    m_App = new QApplication(argc, argv);

    QPixmap pixmap(QCoreApplication::applicationDirPath() + LISEM_FOLDER_ASSETS + "splash.png");
    QSplashScreen splash(pixmap);
    splash.show();


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

            std::cout << "test1 " << std::endl;
            InitUIShaders(m_OpenGLCLManager);

            std::cout << "test2 " << std::endl;

            m_OpenGLCLManager->SetCallBackFrame(&SPHazard::OnGLCLFrame,this);
            m_model->InitModel();

            std::cout << "test3 " << std::endl;

            m_WorldPainter = new WorldWindow(m_OpenGLCLManager);
            m_WorldPainter->SetModel(m_model);


            std::cout << "test4 " << std::endl;

            m_OpenGLCLManager->SetCallBackFrame(&WorldWindow::Draw,m_WorldPainter);


            std::cout << "test5 " << std::endl;

            glfwMakeContextCurrent(NULL);

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
                if(run_file)
                {

                }else if(compile_file)
                {

                }else if(run_model)
                {

                }else if(do_run)
                {

                }

            }else
            {
                std::thread t1 = std::thread((&OpenGLCLManager::GLCLLoop),m_OpenGLCLManager);

                m_InterfaceWindow->Create(m_ParameterManager, m_model,m_ScriptManager,m_WorldPainter);
                splash.finish(m_InterfaceWindow);
                m_InterfaceWindow->show();

                m_InterfaceWindow->SetScriptFunctions(m_ScriptManager);

                m_OpenGLCLManager->ProcessEvents(); //QTimer::singleShot(0,m_OpenGLCLManager,SLOT(ProcessEvents));
                std::cout << "execute qt app " << std::endl;

                //set instructions from command line to interface window

                m_InterfaceWindow->SetCommandLineCallBack(std::function<void(void)>([this,run_file,compile_file,model_run,calc_run,do_view,file,files,has_dir,dir,quit_post](){

                    std::cout << "command line callback" << std::endl;
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
                        m_InterfaceWindow->SetCommandLineRunOpenFiles(files,has_dir,dir,quit_post);
                    };
                }));



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

    return 0;
}


