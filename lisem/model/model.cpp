#include "model.h"
#include <thread>
#include <QThread>
#include "iogdal.h"
#include "lsmio.h"
#include "site.h"

LISEMModel * CLISEMModel = nullptr;


LISEMModel::LISEMModel(OpenGLCLManager * in_OpenGLCLManager, ParameterManager * pm)
{
    m_OpenGLCLManager = in_OpenGLCLManager;
    m_ParameterManager = pm;

}


void LISEMModel::InitModel()
{
     m_KernelDir = GetSite() +LISEM_FOLDER_KERNELS;

}


void LISEMModel::OnStep()
{


    m_ModelStateMutex.lock();


    if(m_StartRequested)
    {
        m_StartRequested = false;


        if(m_Started && m_Paused)
        {
           this->PauseCondition.wakeAll();
        }

        if(!m_Started)
        {
            m_Paused = false;
            m_Started = true;
            this->start();
        }
    }

    if(m_StopRequested)
    {
        if(m_Started && m_Paused)
        {
           this->PauseCondition.wakeAll();
        }
    }


    m_ModelStateMutex.unlock();



}

void LISEMModel::RequestStart(QList<QString> options)
{
    m_ModelStateMutex.lock();

    m_Options = QList<QString>(options);

    m_StartRequested = true;

    m_ModelStateMutex.unlock();

}



void LISEMModel::RequestPause()
{
    m_ModelStateMutex.lock();

    m_PauseRequested = true;

    m_ModelStateMutex.unlock();

}

void LISEMModel::RequestStop()
{
    m_ModelStateMutex.lock();

    m_StopRequested = true;

    m_ModelStateMutex.unlock();


}

void LISEMModel::SetMaskMap(QString path, int index)
{
    m_MaskMap = new cTMap(readRaster(path));



    m_MaskRMin = m_MaskMap->nrRows()+1;
    m_MaskRMax = -1;
    m_MaskCMin = m_MaskMap->nrCols()+1;
    m_MaskCMax = -1;

    bool found = false;
    FOR_ROW_COL(m_MaskMap)
    {
        if(pcr::isMV(m_MaskMap->data[r][c]) || !(((int)(0.5+m_MaskMap->data[r][c]) == index)))
        {
            m_MaskMap->Drc = 0;
        }else
        {
            m_MaskMap->Drc = 1;
            found = true;

                m_MaskRMin = std::min(r,m_MaskRMin);
                m_MaskRMax = std::max(r,m_MaskRMax);
                m_MaskCMin = std::min(c,m_MaskCMin);
                m_MaskCMax = std::max(c,m_MaskCMax);

        }
    }

    if(found == false)
    {
        LISEM_ERROR("ERROR: Subsection for simulation can not be found (index = " + QString::number(index) + ")");
        throw 1;
    }
    m_MaskMapSet = true;

}

cTMap *LISEMModel::GetMapSubSection(cTMap * m)
{

    //if we do not have a subsection, return the map
    if(m_MaskRMin < 0 || m_MaskRMax < 0 || m_MaskCMax < 0 || m_MaskCMin < 0 || m_SimulateSubSection == false)
    {
        return m;
    }

    //create new, smaller map, copy values and then delete input, and return the new one

    int size_r = std::max(1,m_MaskRMax - m_MaskRMin);
    int size_c = std::max(1,m_MaskCMax - m_MaskCMin);

    MaskedRaster<float> raster_data(size_r, size_c, m->data.north() + ((float)(m_MaskRMin)) * m->cellSizeY(), m->data.west() + ((float)(m_MaskCMin)) * m->cellSizeX(), m->data.cell_size(),m->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),m->projection(),"");

    FOR_ROW_COL_MV(map)
    {
        map->data[r][c] = m->data[r + m_MaskRMin][c + m_MaskCMin];
    }

    delete m;

    return map;
}


cTMap *LISEMModel::GetBaseMap(QString path)
{

    m_BaseMap = GetMapSubSection(new cTMap(readRaster(path)));

    m_BaseWidth = m_BaseMap->nrCols();
    m_BaseHeight = m_BaseMap->nrRows();

    _dx = m_BaseMap->cellSize();

    return m_BaseMap;
}

cTMap *LISEMModel::GetMap(float value)
{
    cTMap *_M = new cTMap();

    _M->MakeMap(m_BaseMap, value);
    // use BaseMap as mask

    if (_M)
    {
        m_MapList.append(_M);
    }

    return(_M);


}
cTMap *LISEMModel::GetMapWithMultByName(QString Dir, QString name, double mult_unit)
{


    double mult = GetMapMult(name);

    cTMap * _M = GetMapSubSection(new cTMap(readRaster(Dir + GetOptionString(name))));

    if (_M)
    {
        FOR_ROW_COL_MV(_M)
        {
            _M->Drc = _M->Drc * mult * mult_unit;
        }

        m_MapList.append(_M);
    }else
    {
        LISEM_ERROR("ERROR in loading map " + Dir + name);
    }

    return(_M);


}

double LISEMModel::GetMapMult(QString name)
{
    for(int i = 0; i < m_Options.length(); i++)
    {
        if(m_Options.at(i).startsWith(name + "_cal_mult"))
        {
            QStringList split = m_Options.at(i).split("=");
            if(split.length() > 1)
            {
                bool ok = true;
                double val = split.at(1).toDouble(&ok);
                if(ok)
                {
                    std::cout << m_Options.at(i).toStdString() << "  " << val << std::endl;
                    return val;
                }else
                {
                    LISEM_ERROR("Could not read value for parameter " + name + " : " + m_Options.at(i));
                    return 1.0;
                }
            }else
            {
                LISEM_ERROR("Could not read value for parameter " + name + " : " + m_Options.at(i));
                return 1.0;
            }
        }
    }

    return 1.0;
}

cTMap *LISEMModel::GetMap(QString path)
{
    cTMap * _M = new cTMap(readRaster(path));

    if (_M)
    {
        m_MapList.append(_M);
    }

    return(_M);


}

void LISEMModel::DestroyMaps()
{
    delete m_BaseMap;
    m_BaseMap = NULL;
    if(m_MaskMap != nullptr)
    {
        delete m_MaskMap;
        m_MaskMap = nullptr;
        m_MaskMapSet= false;
    }
    m_MaskRMin = -1;
    m_MaskRMax = -1;
    m_MaskCMin = -1;
    m_MaskCMax = -1;

    for(int i = m_MapList.length()-1; i > -1; i--)
    {
        SAFE_DELETENS(m_MapList.at(i));
    }
    m_MapList.clear();
}


void LISEMModel::run()
{

    FinishMutex.lock();

    bool error = false;

    try {

        DoModelRun();


    } catch (cl::Error err) {

        std::cout << err.what() << "(" << err.err() << ")" << "  " << OCL_GetErrorString(err.err()) <<  std::endl;

         error = true;
         LISEM_ERROR("CL ERROR, QUITTING MODEL!");


    } catch (int cerror) {

        error = true;
        LISEM_ERROR("SPHAZARD ERROR, QUITTING MODEL!");

    } catch (std::exception &e) {

        error = true;
        LISEM_ERROR("STD ERROR, QUITTING MODEL!");

    } catch (...) {

        error = true;
        LISEM_ERROR("UNKNOWN ERROR (suspect memory limitation), QUITTING MODEL!");

    }

    if(m_Initializing)
    {
        m_Initializing = false;

        glfwMakeContextCurrent(NULL);
        m_OpenGLCLManager->m_GLMutex.unlock();
        m_OpenGLCLManager->m_GLOutputUIMutex.unlock();
    }

    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
    m_OpenGLCLManager->m_GLMutex.lock();
glfwMakeContextCurrent(m_OpenGLCLManager->window);

m_RequiresUIReset = true;
if(m_UIMapsSet)
{
    m_UIMapsSet = false;

}


    ModelRunDeleteData();

    //now free the opengl context and mutexes
    glfwMakeContextCurrent(NULL);
    m_OpenGLCLManager->m_GLMutex.unlock();
    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

    m_ModelStateMutex.lock();

    m_Paused = false;
    m_Started = false;

    m_ModelStateMutex.unlock();

    if(m_CallBack_ModelStopped_Set || error == true)
    {
        m_CallBack_ModelStopped(error);
    }


    //possibly a thread is waiting for this moment,
    //we release the finish mutex



    FinishMutex.unlock();
    FinishCondition.notify_all();


}

bool LISEMModel::IsRunning()
{
    m_ModelStateMutex.lock();

    bool isrunning = m_Started;

    m_ModelStateMutex.unlock();

    return isrunning;
}

void LISEMModel::Destroy()
{

    if(this->isRunning())
    {
        this->RequestStop();

        //this->terminate();
        this->wait();

        //while these are set for formality, there can be no new work done with this class as long as the mutex object are not reset.
        //since we quit the thread (possibly mid-execution) we could have left mutex objects locked leading to unpredictible behavior.
        m_Paused = false;
        m_Started = false;
    }


    if(m_ThreadReport.joinable())
    {
        m_ThreadReport.join();
    }

    if(m_UIMapsSet)
    {


        m_UIMapsSet = false;

        ModelRunDeleteData();



    }




}

/*void LISEMModel::SetScriptFunctions(ScriptManager * sm)
{
    //register a structure containing some model results and properties

    int r = sm->m_Engine->RegisterObjectType("ModelRes", sizeof(AS_MODELRESULT), asOBJ_VALUE | asOBJ_POD| asGetTypeTraits<AS_MODELRESULT>()); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string OutputDirectory", asOFFSET(AS_MODELRESULT,Dir_Output)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string MapsDirectory", asOFFSET(AS_MODELRESULT,Dir_Maps)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string TimeDirectory", asOFFSET(AS_MODELRESULT,Dir_Time)); assert( r >= 0 );


    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalFluidHeight", asOFFSET(AS_MODELRESULT,Name_HFFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalFluidVelocity", asOFFSET(AS_MODELRESULT,Name_VELFFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_MaxFluidHeight", asOFFSET(AS_MODELRESULT,Name_HFMAX)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_MaxFluidVelocity", asOFFSET(AS_MODELRESULT,Name_VFMAX)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_TotalInfiltration", asOFFSET(AS_MODELRESULT,Name_INFIL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_TotalSurfaceStorage", asOFFSET(AS_MODELRESULT,Name_SURFACESTORAGE)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_TotalCanopyStorage", asOFFSET(AS_MODELRESULT,Name_CANOPYSTORAGE)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelFluidHeight", asOFFSET(AS_MODELRESULT,Name_CHHFFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelFluidVelocity", asOFFSET(AS_MODELRESULT,Name_CHVELFFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalSolidHeight", asOFFSET(AS_MODELRESULT,Name_HSFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalSolidVelocity", asOFFSET(AS_MODELRESULT,Name_VELSFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_MaxSolidHeight", asOFFSET(AS_MODELRESULT,Name_HSMAX)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_MaxSolidVelocity", asOFFSET(AS_MODELRESULT,Name_VSMAX)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelSolidHeight", asOFFSET(AS_MODELRESULT,Name_CHHSFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelSolidVelocity", asOFFSET(AS_MODELRESULT,Name_CHVELSFINAL)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_SafetyFactor", asOFFSET(AS_MODELRESULT,Name_SF)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_SafetyFactorCompensation", asOFFSET(AS_MODELRESULT,Name_SFC)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_SlopeFailure", asOFFSET(AS_MODELRESULT,Name_FAILURE)); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectProperty("ModelRes", "string FileName_SFracMax", asOFFSET(AS_MODELRESULT,Name_SFracMax)); assert( r >= 0 );

    //register functions that allow the user to run the model from the scripting environment

    sm->m_Engine->RegisterGlobalFunction("ModelRes RunModel(string runfile, string additional_options = \"\")", asMETHODPR(LISEMModel,AS_StartModelFromRunFile,(QString,QString),AS_MODELRESULT),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //register object type
    sm->m_Engine->RegisterObjectType("RigidModel",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()

    //register constructors by using factory functions
    sm->m_Engine->RegisterObjectBehaviour("RigidModel",asBEHAVE_FACTORY,"RigidModel@ C0()",asFUNCTIONPR(AS_RigidWorldFactory,(),RigidPhysicsWorld * ),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    sm->m_Engine->RegisterObjectBehaviour("RigidModel",asBEHAVE_ADDREF,"void f()",asMETHOD(RigidPhysicsWorld,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    sm->m_Engine->RegisterObjectBehaviour("RigidModel",asBEHAVE_RELEASE,"void f()",asMETHOD(RigidPhysicsWorld,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    sm->m_Engine->RegisterObjectMethod("RigidModel", "RigidModel& opAssign(RigidModel &in m)", asMETHODPR(RigidPhysicsWorld,AS_Assign,(RigidPhysicsWorld *),RigidPhysicsWorld*), asCALL_THISCALL); assert( r >= 0 );

    //register object type
    sm->m_Engine->RegisterObjectType("RigidObject",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()

    //register constructors by using factory functions
    sm->m_Engine->RegisterObjectBehaviour("RigidObject",asBEHAVE_FACTORY,"RigidObject@ C0()",asFUNCTIONPR(AS_RigidObjectFactory,(),RigidPhysicsObject * ),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    sm->m_Engine->RegisterObjectBehaviour("RigidObject",asBEHAVE_ADDREF,"void f()",asMETHOD(RigidPhysicsObject,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    sm->m_Engine->RegisterObjectBehaviour("RigidObject",asBEHAVE_RELEASE,"void f()",asMETHOD(RigidPhysicsObject,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    sm->m_Engine->RegisterObjectMethod("RigidObject", "RigidObject& opAssign(RigidObject &in m)", asMETHODPR(RigidPhysicsObject,AS_Assign,(RigidPhysicsObject *),RigidPhysicsObject*), asCALL_THISCALL); assert( r >= 0 );
    //sm->m_Engine->RegisterObjectMethod("RigidObject", "vec3 Position()", asMETHODPR(RigidPhysicsObject,GetPosition,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );


    sm->m_Engine->RegisterGlobalFunction("RigidObject @RigidObjectBox(vec3 size= {1.0,1.0,1.0}, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsBox),  asCALL_CDECL); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("RigidObject @RigidObjectCilinder(double radius, double length, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsCilinder),  asCALL_CDECL); assert( r >= 0 );

    sm->m_Engine->RegisterObjectMethod("RigidModel","void AddObject(RigidObject &in)", asMETHOD(RigidPhysicsWorld,AS_AddObj),asCALL_THISCALL);
    sm->m_Engine->RegisterObjectMethod("RigidModel","int GetObjectCount()", asMETHOD(RigidPhysicsWorld,AS_GetObjCount),asCALL_THISCALL);
    sm->m_Engine->RegisterObjectMethod("RigidModel","RigidObject @GetObject(int n)", asMETHOD(RigidPhysicsWorld,AS_GetObj),asCALL_THISCALL);


    sm->m_Engine->RegisterGlobalFunction("ModelRes RunModel(string runfile, RigidModel &in rm, string additional_options = \"\")", asMETHODPR(LISEMModel,AS_StartModelFromRunFile,(QString,RigidPhysicsWorld *,QString),AS_MODELRESULT),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

}*/
