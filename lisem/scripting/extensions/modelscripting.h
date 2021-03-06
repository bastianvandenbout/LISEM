#ifndef MODELSCRIPTING_H
#define MODELSCRIPTING_H

#include "error.h"
#include "model.h"
#include "scriptmanager.h"
#include "openglclmanager.h"
#include "QPixmap"
#include "raster/rasterseismic.h"
#include "extensions/scriptarrayhelpers.h"

//AS_MODELRESULT RequestStartAndWait(QList<QString> options, RigidPhysicsWorld * world = nullptr);
//AS_MODELRESULT RequestStartAndWait(QString file, QString add_options, RigidPhysicsWorld * world = nullptr);
//AS_MODELRESULT StartModelFromRunFile(QString dir,QString file, QString add_options);
//AS_MODELRESULT StartModelFromRunFile(QString dir,QString file);
//AS_MODELRESULT StartModelFromRunFile(QString dir,QString file, RigidPhysicsWorld * world, QString add_options);


inline AS_MODELRESULT RequestStartAndWait(QList<QString> options, RigidPhysicsWorld * world)
{
    std::cout << "Request start and wait "<< std::endl;
    LISEMModel * m = GetGLobalModel();
    if(m != nullptr)
    {
        m->FinishMutex.lock();
        m->m_ModelStateMutex.lock();
        m->m_Options = QList<QString>(options);
        m->m_RigidWorld = world;
        m->m_StartRequested = true;
        m->m_ModelStateMutex.unlock();

        m->FinishCondition.wait(&m->FinishMutex);
        m->FinishMutex.unlock();
        AS_MODELRESULT res = m->FinishResult;
        return res;

    }else
    {

         OpenGLCLManager * m_OpenGLCLManager = new OpenGLCLManager();
         ParameterManager* m_ParameterManager = new ParameterManager();
        m_ParameterManager->InitParameters();
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

inline AS_MODELRESULT RequestStartAndWait(QString file, QString add_options, RigidPhysicsWorld * world)
{

    ParameterManager* m_ParameterManager = new ParameterManager();
    m_ParameterManager->InitParameters();

        AS_MODELRESULT res;
        QFileInfo f(file);
        if(f.exists())
        {
            return RequestStartAndWait(m_ParameterManager->GetParameterListFromFile(file,add_options),world);
        }else {

            LISEM_ERROR("Run file could not be found " + file);
        }


        m_ParameterManager->Destroy();
        SAFE_DELETE(m_ParameterManager);
        return res;
}


inline AS_MODELRESULT StartModelFromRunFile(QString path,QString file, QString add_options)
{
    LISEMS_STATUS("Requesting model simulation with runfile : " + file);
    return RequestStartAndWait( path +QString(file),QString(add_options),nullptr);
}

inline AS_MODELRESULT StartModelFromRunFile(QString file,QString options)
{
    LISEMS_STATUS("Requesting model simulation with runfile : " + QString(file));
    return RequestStartAndWait( QString(file),options, nullptr);
}

inline AS_MODELRESULT StartModelFromRunFile(QString file, RigidPhysicsWorld * rm, QString add_options)
{
    rm->AS_AddRef();
    LISEMS_STATUS("Requesting model simulation with runfile : " + QString(file));
    return RequestStartAndWait( QString(file),"",rm);

}


inline void RegisterModelScriptFunctions(LSMScriptEngine *sm)
{
    //register a structure containing some model results and properties

    int r = sm->RegisterObjectType("ModelRes", sizeof(AS_MODELRESULT), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS| asGetTypeTraits<AS_MODELRESULT>()); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string OutputDirectory", asOFFSET(AS_MODELRESULT,Dir_Output)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string MapsDirectory", asOFFSET(AS_MODELRESULT,Dir_Maps)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string TimeDirectory", asOFFSET(AS_MODELRESULT,Dir_Time)); assert( r >= 0 );


    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalFluidHeight", asOFFSET(AS_MODELRESULT,Name_HFFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalFluidVelocity", asOFFSET(AS_MODELRESULT,Name_VELFFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_MaxFluidHeight", asOFFSET(AS_MODELRESULT,Name_HFMAX)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_MaxFluidVelocity", asOFFSET(AS_MODELRESULT,Name_VFMAX)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_TotalInfiltration", asOFFSET(AS_MODELRESULT,Name_INFIL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_TotalSurfaceStorage", asOFFSET(AS_MODELRESULT,Name_SURFACESTORAGE)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_TotalCanopyStorage", asOFFSET(AS_MODELRESULT,Name_CANOPYSTORAGE)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelFluidHeight", asOFFSET(AS_MODELRESULT,Name_CHHFFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelFluidVelocity", asOFFSET(AS_MODELRESULT,Name_CHVELFFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalSolidHeight", asOFFSET(AS_MODELRESULT,Name_HSFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalSolidVelocity", asOFFSET(AS_MODELRESULT,Name_VELSFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_MaxSolidHeight", asOFFSET(AS_MODELRESULT,Name_HSMAX)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_MaxSolidVelocity", asOFFSET(AS_MODELRESULT,Name_VSMAX)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelSolidHeight", asOFFSET(AS_MODELRESULT,Name_CHHSFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_FinalChannelSolidVelocity", asOFFSET(AS_MODELRESULT,Name_CHVELSFINAL)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_SafetyFactor", asOFFSET(AS_MODELRESULT,Name_SF)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_SafetyFactorCompensation", asOFFSET(AS_MODELRESULT,Name_SFC)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_SlopeFailure", asOFFSET(AS_MODELRESULT,Name_FAILURE)); assert( r >= 0 );
    r = sm->RegisterObjectProperty("ModelRes", "string FileName_SFracMax", asOFFSET(AS_MODELRESULT,Name_SFracMax)); assert( r >= 0 );

    //register object type
    sm->RegisterObjectType("RigidModel",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()

    //register constructors by using factory functions
    sm->RegisterObjectBehaviour("RigidModel",asBEHAVE_FACTORY,"RigidModel@ C0()",asFUNCTIONPR(AS_RigidWorldFactory,(),RigidPhysicsWorld * ),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    sm->RegisterObjectBehaviour("RigidModel",asBEHAVE_ADDREF,"void f()",asMETHOD(RigidPhysicsWorld,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectBehaviour("RigidModel",asBEHAVE_RELEASE,"void f()",asMETHOD(RigidPhysicsWorld,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    sm->RegisterObjectMethod("RigidModel", "RigidModel& opAssign(RigidModel &in m)", asMETHODPR(RigidPhysicsWorld,AS_Assign,(RigidPhysicsWorld *),RigidPhysicsWorld*), asCALL_THISCALL); assert( r >= 0 );

    //register object type
    sm->RegisterObjectType("RigidObject",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()

    //register constructors by using factory functions
    sm->RegisterObjectBehaviour("RigidObject",asBEHAVE_FACTORY,"RigidObject@ C0()",asFUNCTIONPR(AS_RigidObjectFactory,(),RigidPhysicsObject * ),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    sm->RegisterObjectBehaviour("RigidObject",asBEHAVE_ADDREF,"void f()",asMETHOD(RigidPhysicsObject,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectBehaviour("RigidObject",asBEHAVE_RELEASE,"void f()",asMETHOD(RigidPhysicsObject,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    sm->RegisterObjectMethod("RigidObject", "RigidObject& opAssign(RigidObject &in m)", asMETHODPR(RigidPhysicsObject,AS_Assign,(RigidPhysicsObject *),RigidPhysicsObject*), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 Position()", asMETHODPR(RigidPhysicsObject,GetPosition,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 Rotation()", asMETHODPR(RigidPhysicsObject,GetRotation,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 Velocity()", asMETHODPR(RigidPhysicsObject,GetVelocity,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 RotationalVelocity()", asMETHODPR(RigidPhysicsObject,GetRotationVelocity,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 GetLocalLinearVelocity(vec3 worldpos)", asMETHODPR(RigidPhysicsObject,GetLocalLinearVelocity,(LSMVector3),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 GetForces()", asMETHODPR(RigidPhysicsObject,GetForces,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "vec3 GetTorques()", asMETHODPR(RigidPhysicsObject,GetTorques,(),LSMVector3), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void ApplyForce(vec3 p, vec3 f)", asMETHODPR(RigidPhysicsObject,ApplyForce,(LSMVector3, LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void ApplyTorques(vec3 f)", asMETHODPR(RigidPhysicsObject,ApplyTorque,(LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void SetPosition(vec3 f)", asMETHODPR(RigidPhysicsObject,SetPosition,(LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void SetVelocity(vec3 f)", asMETHODPR(RigidPhysicsObject,SetRotation,(LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void SetRotation(vec3 f)", asMETHODPR(RigidPhysicsObject,SetVelocity,(LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("RigidObject", "void SetRotationalVelocity(vec3 f)", asMETHODPR(RigidPhysicsObject,SetRotationalVelocity,(LSMVector3),void), asCALL_THISCALL); assert( r >= 0 );

    sm->RegisterGlobalFunction("RigidObject @RigidObjectSphere(float size, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsSphere),  asCALL_CDECL); assert( r >= 0 );
    sm->RegisterGlobalFunction("RigidObject @RigidObjectEllipsoid(vec3 size= {1.0,1.0,1.0}, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsEllipsoid),  asCALL_CDECL); assert( r >= 0 );
    sm->RegisterGlobalFunction("RigidObject @RigidObjectDEM(Map &in dem, bool relative = false,double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsHeightField),  asCALL_CDECL); assert( r >= 0 );
    sm->RegisterGlobalFunction("RigidObject @RigidObjectBox(vec3 size= {1.0,1.0,1.0}, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsBox),  asCALL_CDECL); assert( r >= 0 );
    sm->RegisterGlobalFunction("RigidObject @RigidObjectCilinder(double radius, double length, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsCilinder),  asCALL_CDECL); assert( r >= 0 );
    sm->RegisterGlobalFunction("RigidObject @RigidObjectMesh(Object &in model, double density = 1000.0, vec3 position = {0.0,0.0,0.0}, vec3 rotation = {0.0,0.0,0.0}, vec3 vel = {0.0,0.0,0.0}, vec3 rotvel = {0.0,0.0,0.0},double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double damplingF = 0.2, string family = \"\", bool static = false)", asFUNCTION(RigidPhysicsObject::RigidPhysicsObject_AsASMesh),  asCALL_CDECL); assert( r >= 0 );

    sm->RegisterObjectMethod("RigidModel","void AddObject(RigidObject &in)", asMETHOD(RigidPhysicsWorld,AS_AddObj),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","int GetObjectCount()", asMETHOD(RigidPhysicsWorld,AS_GetObjCount),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","RigidObject @GetObject(int n)", asMETHOD(RigidPhysicsWorld,AS_GetObj),asCALL_THISCALL);

    sm->RegisterObjectMethod("RigidModel","void Step(float dt)", asMETHOD(RigidPhysicsWorld,AS_Step),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","void SetElevation(Map &in m)", asMETHOD(RigidPhysicsWorld,AS_SetElevation),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","void SetFlow(Map &in h, Map &in ux,Map &in uy, Map &in density)", asMETHOD(RigidPhysicsWorld,AS_SetFlow),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockX()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockX),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockY()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockY),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockFX()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockFX),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockFY()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockFY),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockCX()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockCX),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowBlockCY()", asMETHOD(RigidPhysicsWorld,AS_GetFlowBlockCY),asCALL_THISCALL);
    sm->RegisterObjectMethod("RigidModel","Map @GetFlowHCorrect()", asMETHOD(RigidPhysicsWorld,AS_GetFlowHCorrect),asCALL_THISCALL);

    //register functions that allow the user to run the model from the scripting environment

    sm->RegisterGlobalFunction("ModelRes RunModel(string runfile, string additional_options = \"\")", asFUNCTIONPR(StartModelFromRunFile,(QString,QString),AS_MODELRESULT),  asCALL_CDECL); assert( r >= 0 );


    sm->RegisterGlobalFunction("ModelRes RunModel(string runfile, RigidModel &in rm, string additional_options = \"\")", asFUNCTIONPR(StartModelFromRunFile,(QString,RigidPhysicsWorld *,QString),AS_MODELRESULT),  asCALL_CDECL); assert( r >= 0 );

    sm->RegisterGlobalSTDFunction("array<Field> @RigidWorldToField(const RigidModel &in rm, Field &in ref)", GetFuncConvert(RigidWorldToField)); assert( r >= 0 );
    sm->RegisterGlobalSTDFunction("void RigidWorldApplyPressure(const RigidModel &in rm, Field &in Block, Field &in Pressure)", GetFuncConvert(RigidWorldApplyPressureField)); assert( r >= 0 );





    //register object type
    sm->RegisterObjectType("SeismicModel",0,asOBJ_REF );// | asGetTypeTraits<cTMap>()

    //register constructors by using factory functions
    sm->RegisterObjectBehaviour("SeismicModel",asBEHAVE_FACTORY,"SeismicModel@ C0()",asFUNCTIONPR(AS_SeismicModelFactory,(),SeismicModel * ),asCALL_CDECL); assert( r >= 0 );

    //register reference counting for garbage collecting
    sm->RegisterObjectBehaviour("SeismicModel",asBEHAVE_ADDREF,"void f()",asMETHOD(SeismicModel,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectBehaviour("SeismicModel",asBEHAVE_RELEASE,"void f()",asMETHOD(SeismicModel,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );

    sm->RegisterObjectMethod("SeismicModel", "SeismicModel& opAssign(SeismicModel &in m)", asMETHODPR(SeismicModel,AS_Assign,(SeismicModel *),SeismicModel*), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "void Step(float dt)", asMETHODPR(SeismicModel,Step,(float),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "void AddVelocityModel(const Field &in vp, const Field &in vs, const Field &in dens)", asMETHODPR(SeismicModel,AddVelocityModel,(Field *, Field*, Field*),void), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "void SetTopography(const Map &in dem)", asMETHODPR(SeismicModel,SetTopography,(cTMap*),void), asCALL_THISCALL); assert( r >= 0 );

    sm->RegisterObjectMethod("SeismicModel", "void AddPointSource(float x, float y, float z, float Mxx, float Myy, float Mzz, float Mxy, float Mxz, float Myz, float t, float freq, string type = \"C6SmoothBump\")", asMETHOD(SeismicModel,AddSourcePoint), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "Field @FillField(Field &in F, string param = \"acc\", bool nearest=false)", asMETHOD(SeismicModel,FillField), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "void SetCellSize(float size)", asMETHOD(SeismicModel,SetCellSize), asCALL_THISCALL); assert( r >= 0 );
    sm->RegisterObjectMethod("SeismicModel", "void SetZMaxFrac(float size)", asMETHOD(SeismicModel,SetZMaxFrac), asCALL_THISCALL); assert( r >= 0 );

}



#endif // MODELSCRIPTING_H
