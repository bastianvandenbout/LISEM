#ifndef SPHAZARD_H
#define SPHAZARD_H

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX
#endif

#define __CL_ENABLE_EXCEPTIONS

#include "model.h"

#include "mlpack/prereqs.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <mlpack/core.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/layer/layer_types.hpp>
#include <mlpack/methods/ann/init_rules/random_init.hpp>
#include <mlpack/methods/ann/init_rules/const_init.hpp>
#include <mlpack/methods/ann/init_rules/nguyen_widrow_init.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/rnn.hpp>


#include <glad/glad.h>

#include "OpenCLUtil.h"
#include "cl.hpp"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"


#include <QScreen>
#include <QWindow>

#include <QDebug>
#include <QApplication>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include <thread>

#include "error.h"
#include "lsmio.h"
#include "openglclmanager.h"
#include "parameters/parametermanager.h"
#include "model.h"
#include "qtinterfacewindow.h"
#include "scriptmanager.h"
#include "glplot/worldwindow.h"

#include "geo/raster/map.h"

#include "audiomanager.h"

/////
/// \brief The SPHazard class
/// This class is the main entry point and controlling agent for
/// the SPHazard application.
/// To use, create a new instance, and run execute(..)
/// This wil start (depending on the options) the requested operations
/// This can include an interface and multiple windows, in which case
/// the class will take control of the event loop, and only return when
/// the user exits the application.
/////

class SPHazard : public QObject
{

    Q_OBJECT;


private:

    QApplication *m_App;

    QString Dir;

    QTInterfaceWindow * m_InterfaceWindow;
    //IOManager *m_IOManager;
    OpenGLCLManager *m_OpenGLCLManager;
    AudioManager * m_AudioManager;
    ParameterManager *m_ParameterManager;
    LISEMModel * m_model;
    ScriptManager * m_ScriptManager;
    WorldWindow * m_WorldPainter;

public:
    SPHazard();


    void processTimeStep();

    void renderFrame();

    void OnGLCLLoad();

    bool OnGLCLFrame();

    ////
    /// \brief execute
    /// \param argc
    /// \param argv
    /// \return
    ///



    int execute(int argc, char *argv[]);



};



#endif // SPHAZARD_H
