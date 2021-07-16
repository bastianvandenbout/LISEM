

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

void SPHazard::OnGLCLFrame()
{

    processTimeStep();

    renderFrame();

}

int SPHazard::execute(int argc, char *argv[])
{




    InitMessages();
    InitMessagesS();
    LISEM_STATUS("============================");
    LISEM_STATUS("=      SPHazard Model       ");
    LISEM_STATUS("= Opening main structure    ");
    LISEM_STATUS("=                           ");
    LISEM_STATUS("============================");
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
                        tr("OpenLISEM Hazard"),
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
            std::thread t1 = std::thread((&OpenGLCLManager::GLCLLoop),m_OpenGLCLManager);

            m_InterfaceWindow->Create(m_ParameterManager, m_model,m_ScriptManager,m_WorldPainter);
            splash.finish(m_InterfaceWindow);
            m_InterfaceWindow->show();

            m_InterfaceWindow->SetScriptFunctions(m_ScriptManager);

            m_OpenGLCLManager->ProcessEvents(); //QTimer::singleShot(0,m_OpenGLCLManager,SLOT(ProcessEvents));
            int qreturn = m_App->exec();

            m_App->processEvents();

            if(qreturn == LISEM_CODE_RETURN_RESTART)
            {
                restart = true;
            }
            glfwSetWindowShouldClose(m_OpenGLCLManager->window,GL_TRUE);

            t1.join();


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


