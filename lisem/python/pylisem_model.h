#pragma once


#include "error.h"

#include <utility>

#include <QtCore/QString>

#include "pybind11/pybind11.h"
#include "pybind11/operators.h"

#include "linear/lsm_vector.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"
#include "extensions/modelscripting.h"

namespace py = pybind11;


inline AS_MODELRESULT RequestStartAndWaitPy(QList<QString> options, RigidPhysicsWorld * world)
{
    std::cout << "Request start and wait "<< std::endl;
    LISEMModel * m = GetGLobalModel();
    if(m != nullptr)
    {
        std::cout << "Global model found" << std::endl;
        m->FinishMutex.lock();
        m->m_ModelStateMutex.lock();
        m->m_Options = options;
        m->m_RigidWorld = world;
        m->m_StartRequested = true;
        m->m_ModelStateMutex.unlock();

        std::cout << "requested start" << std::endl;
        m->OnStep();
        std::cout << "Onstep done" << std::endl;
        m->FinishCondition.wait(&m->FinishMutex);
        m->FinishMutex.unlock();
        AS_MODELRESULT res = m->FinishResult;

        return res;

    }else
    {

         OpenGLCLManager * m_OpenGLCLManager = new OpenGLCLManager();
         ParameterManager* m_ParameterManager = new ParameterManager();
        m = new LISEMModel(m_OpenGLCLManager,m_ParameterManager);
        int suc = m_OpenGLCLManager->CreateGLWindow(QPixmap());
        if(suc > 0)
        {
            LISEMS_ERROR("Could not create opengl/opencl context");
            throw 1;
        }
        suc = m_OpenGLCLManager->InitGLCL();
        if(suc > 0)
        {
            LISEMS_ERROR("Could not create opengl/opencl context");
            throw 1;
        }
        m->InitModel();


        m->FinishMutex.lock();
        m->m_ModelStateMutex.lock();
        m->m_Options = QList<QString>(options);
        m->m_RigidWorld = world;
        m->m_StartRequested = true;
        m->m_ModelStateMutex.unlock();
        m->FinishCondition.wait(&m->FinishMutex);
        m->FinishMutex.unlock();
        AS_MODELRESULT res = m->FinishResult;



        m->Destroy();
        m_ParameterManager->Destroy();
        m_OpenGLCLManager->Destroy();
        SAFE_DELETE(m_OpenGLCLManager);
        SAFE_DELETE(m_ParameterManager);
        SAFE_DELETE(m);

        return res;
    }



}

inline AS_MODELRESULT RequestStartAndWaitPy(QString file, QString add_options, RigidPhysicsWorld * world)
{

    std::cout << "load run file "<< std::endl;
    ParameterManager* m_ParameterManager = new ParameterManager();
    m_ParameterManager->InitParameters();
        AS_MODELRESULT res;
        QFileInfo f(file);
        if(f.exists())
        {
            return RequestStartAndWaitPy(m_ParameterManager->GetParameterListFromFile(file,add_options),world);
        }else {

            LISEM_ERROR("Run file could not be found " + file);
        }


        m_ParameterManager->Destroy();
        SAFE_DELETE(m_ParameterManager);
        return res;
}

inline AS_MODELRESULT StartModelFromRunFilePy(QString file,QString options)
{
    LISEMS_STATUS("Requesting model simulation with runfile : " + QString(file));
    return RequestStartAndWaitPy( QString(file),options, nullptr);
}
static void init_pylisem_model(py::module &m)
{
    py::class_<AS_MODELRESULT>(m,"ModelResult")
            .def("OutputDir",[](const AS_MODELRESULT & m){return m.Dir_Output;})
            .def("MapDir",[](const AS_MODELRESULT & m){return m.Dir_Maps;})
            .def("TimeDir",[](const AS_MODELRESULT & m){return m.Dir_Time;})
            .def("FinalFluidHeight",[](const AS_MODELRESULT & m){return m.Name_HFFINAL;})
            .def("FinalFluidVelocity",[](const AS_MODELRESULT & m){return m.Name_VELFFINAL;})
            .def("MaxFluidHeight",[](const AS_MODELRESULT & m){return m.Name_HFMAX;})
            .def("MaxFluidVelocity",[](const AS_MODELRESULT & m){return m.Name_VFMAX;})
            .def("TotalInfiltration",[](const AS_MODELRESULT & m){return m.Name_INFIL;})
            .def("TotalSurfaceStorage",[](const AS_MODELRESULT & m){return m.Name_SURFACESTORAGE;})
            .def("TotalCanopyStorage",[](const AS_MODELRESULT & m){return m.Name_CANOPYSTORAGE;})
            .def("FinalChannelFluidHeight",[](const AS_MODELRESULT & m){return m.Name_CHHFFINAL;})
            .def("FinalChannelFluidVelocity",[](const AS_MODELRESULT & m){return m.Name_CHVELFFINAL;})
            .def("FinalSolidHeight",[](const AS_MODELRESULT & m){return m.Name_HSFINAL;})
            .def("FinalSolidVelocity",[](const AS_MODELRESULT & m){return m.Name_VELSFINAL;})
            .def("MaxSolidHeight",[](const AS_MODELRESULT & m){return m.Name_HSMAX;})
            .def("MaxSolidVelocity",[](const AS_MODELRESULT & m){return m.Name_VSMAX;})
            .def("FinalChannelSolidHeight",[](const AS_MODELRESULT & m){return m.Name_CHHSFINAL;})
            .def("FinalChannelSolidVelocity",[](const AS_MODELRESULT & m){return m.Name_CHVELSFINAL;})
            .def("SafetyFactor",[](const AS_MODELRESULT & m){return m.Name_SF;})
            .def("SafetyFactorCompensation",[](const AS_MODELRESULT & m){return m.Name_SFC;})
            .def("SlopeFailure",[](const AS_MODELRESULT & m){return m.Name_FAILURE;})
            .def("SFracMax",[](const AS_MODELRESULT & m){return m.Name_SFracMax;});

            ;

    m.def("RunModel", py::overload_cast<QString,QString>(&StartModelFromRunFilePy), py::arg("Runfile"), py::arg("Options"));



}
