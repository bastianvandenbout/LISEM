#pragma once


#include "defines.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/solver/ChSolverAPGD.h"
#include "chrono/solver/ChSolverPSOR.h"
#include "chrono/solver/ChSolverBB.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"

using namespace chrono;


#include <omp.h>
#include "geo/raster/map.h"
#include <thread>
#include <QMutex>
#include "QJsonObject"
#include "QJsonDocument"
#include "QJsonArray"
#include "rigidobject.h"

typedef struct ObjAABB
{
    RigidPhysicsObject *obj;
    BoundingBox3D aabb;
    BoundingBox3D aabb_confined;
    double volume_per_unit_height;
    double volume;
    double area_proj_x;
    double area_proj_z;
    LSMVector2 linedir;
    LSMVector2 linecenter;

    inline ObjAABB() : aabb()
    {

    }

    inline ObjAABB(RigidPhysicsObject * o, BoundingBox3D bb) : aabb()
    {
        obj = o;
        aabb = bb;
    }

}ObjAABB;

class LISEM_API RigidPhysicsWorld
{
public:
    ChSystemNSC m_system;
    std::shared_ptr<ChSolverPSOR> m_Solver;

    QList<RigidPhysicsObject * > m_Objects;

    QMutex m_Mutex_internal;
    QMutex m_Mutex;
    bool m_HasChanges = false;

    bool fluids_set = false;
    bool fluids_twophase_set = false;

    cTMap * m_DEM = nullptr;
    cTMap * m_BlockX = nullptr;
    cTMap * m_BlockY = nullptr;
    cTMap * m_BlockFX = nullptr;
    cTMap * m_BlockFY = nullptr;
    cTMap * m_BlockCaptureX = nullptr;
    cTMap * m_BlockCaptureY = nullptr;
    cTMap * m_BlockLX1 = nullptr;
    cTMap * m_BlockLY1 = nullptr;
    cTMap * m_BlockLX2 = nullptr;
    cTMap * m_BlockLY2 = nullptr;
    cTMap * m_HCorrected = nullptr;
    cTMap * m_BlockLDD = nullptr;

    bool m_HasTerrain = false;

    QList<RigidPhysicsObject * > m_DEMObjects;

    cTMap * m_FlowBlockX1 = nullptr;
    cTMap * m_FlowBlockY1 = nullptr;

    bool m_DeleteAfter = false;
    cTMap * m_HF = nullptr;
    cTMap * m_HS = nullptr;
    cTMap * m_USX = nullptr;
    cTMap * m_USY = nullptr;
    cTMap * m_UFX = nullptr;
    cTMap * m_UFY = nullptr;
    cTMap * m_DS = nullptr;
    cTMap * m_IFAS = nullptr;
    cTMap * m_RSS = nullptr;


    cTMap * m_DEMN = nullptr;
    cTMap * m_HFN = nullptr;
    cTMap * m_HSN = nullptr;
    cTMap * m_USXN = nullptr;
    cTMap * m_USYN = nullptr;
    cTMap * m_UFXN = nullptr;
    cTMap * m_UFYN = nullptr;
    cTMap * m_DSN = nullptr;
    cTMap * m_IFASN = nullptr;
    cTMap * m_RSSN = nullptr;


public:

    inline RigidPhysicsWorld()
    {

        int xThreadCount = std::thread::hardware_concurrency();
        //m_Solver = chrono_types::make_shared<ChSolverBB>();
        m_Solver = chrono_types::make_shared<ChSolverPSOR>();
        m_Solver->SetMaxIterations(100);
        m_Solver->EnableWarmStart(true);
        m_system.Set_G_acc(ChVector<double>(0.0,-9.81,0.0));
        m_system.SetSolver(m_Solver);
        m_system.SetMaxPenetrationRecoverySpeed(1.0);
        //m_system.SetTimestepperType(ChTimestepper::Type::RUNGEKUTTA45);
    }

    inline ~RigidPhysicsWorld()
    {

    }

    inline bool HasChanges()
    {
        m_Mutex_internal.lock();
        bool ans = m_HasChanges;
        m_Mutex_internal.unlock();

        return ans;
    }

    inline void SetChanges(bool x)
    {
        m_Mutex_internal.lock();
        m_HasChanges = x;
        m_Mutex_internal.unlock();
    }

    LSMVector2 m_GeoOrigin;
    inline void SetSimpleGeoOrigin(LSMVector2 origin)
    {
        m_GeoOrigin = origin;
    }
    inline LSMVector2 GetSimpleGeoOrigin()
    {
        return m_GeoOrigin;
    }

    void SetTerrain(cTMap * DEM);

    void RunSingleStep(double dt, double t);
    void SetInteractTwoPhaseFlow(cTMap * HF,cTMap * UF,cTMap * VF,cTMap * HS,cTMap * US, cTMap * VS, cTMap * DS, bool delete_after = false)
    {
        if(m_DeleteAfter)
        {
            delete m_HF;
            delete m_UFX;
            delete m_UFY;
        }
        m_HF = HF;
        m_UFX = UF;
        m_UFY = VF;

        m_DeleteAfter = delete_after;
    }

    cTMap * GetUpdatedUFX()
    {
        return m_UFX;
    }

    cTMap * GetUpdatedUSX()
    {
        return m_UFX;
    }

    cTMap * GetUpdatedUFY()
    {
        return m_UFX;
    }

    cTMap * GetUpdatedUSY()
    {
        return m_UFX;
    }

    void AddObject(RigidPhysicsObject * object,bool has_mutex = false, bool add_geo = false);
    void RemoveObject(RigidPhysicsObject * object,bool has_mutex = false);
    int GetObjectCount(bool has_mutex);
    void LockMutex();
    void UnLockMutex();

    inline void Destroy()
    {


            m_Mutex.lock();
            m_Mutex_internal.lock();


        //delete all rigid body objects that are not part of a script

        for(int i = 0; i < m_Objects.length(); i++)
        {
            RigidPhysicsObject * object = m_Objects.at(i);
            if(object->AS_IsFromScript)
            {
                RemoveObject(object,true);
                object->AS_ReleaseRef();

            }else
            {
                RemoveObject(object,true);
                delete object;
            }
        }

        m_Objects.clear();
        m_DEMObjects.clear();

        m_HasChanges = true;

        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }

    QList<RigidPhysicsObject * > GetObjectList();

    inline void ExportToFile(QString path)
    {
        QJsonObject sceneJson;
        //gravity


        //all objects
        QJsonArray layerJsonArray;



        //done
        QJsonDocument document(sceneJson);


    }

    inline void ImportFromFile(QString path)
    {




    }

    void CopyFrom(RigidPhysicsWorld *w);
public:

    QString    AS_FileName          = "";
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    bool           AS_IsFromScript = false;
    RigidPhysicsWorld *     AS_Assign   (RigidPhysicsWorld *other);
    cTMap * AS_GetFlowBlockX();
    cTMap * AS_GetFlowBlockY();
    cTMap * AS_GetFlowBlockFX();
    cTMap * AS_GetFlowBlockFY();
    cTMap * AS_GetFlowBlockCX();
    cTMap * AS_GetFlowBlockCY();
    cTMap * AS_GetFlowHCorrect();

    void AS_Step(float dt);
    void AS_SetElevation(cTMap * dem);
    void AS_SetFlow(cTMap * h , cTMap * ux, cTMap * uy ,cTMap * dens);
    std::vector<cTMap *> AS_GetFlowCoupling();

    //add or remove objects
    void AS_AddObj(RigidPhysicsObject*obj);
    int AS_GetObjCount();
    RigidPhysicsObject* AS_GetObj(int i);

    //get object properties



};


inline RigidPhysicsWorld * RigidPhysicsWorld::AS_Assign(RigidPhysicsWorld *other)
{
    this->CopyFrom(other);
    AS_IsFromScript = true;
    for(int i = 0; i < m_Objects.length(); i++)
    {
        m_Objects.at(i)->AS_IsFromScript = true;
    }

    return this;
}


inline void RigidPhysicsWorld::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void RigidPhysicsWorld::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        this->Destroy();
        delete this;
    }

}

/*inline RigidPhysicsWorld * RigidPhysicsWorld::AS_Assign(RigidPhysicsWorld*other)
{


    return this;
}*/

inline static RigidPhysicsWorld * AS_RigidWorldFactory()
{
    return new RigidPhysicsWorld();


}



inline void RigidPhysicsWorld::AS_AddObj(RigidPhysicsObject*obj)
{
    std::cout << "add obj " << std::endl;
    obj->AS_IsFromScript = true;

    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }
    std::cout << "add obj 2" << std::endl;
    if(!m_Objects.contains(obj))
    {
        AddObject(obj,true, true);
    }
    std::cout << "add obj done2" << std::endl;
    {
        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }
    std::cout << "add obj done" << std::endl;
}

inline int RigidPhysicsWorld::AS_GetObjCount()
{


    return GetObjectCount(false);
}

inline RigidPhysicsObject* RigidPhysicsWorld::AS_GetObj(int i)
{

    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }


    if(i < 0 || i > m_Objects.length())
    {

        LISEMS_ERROR("RigidWorld::GetObject incorrect object index, out of bounds " + QString::number(i));
        throw 1;
    }

    RigidPhysicsObject* obj = m_Objects.at(i);

    obj->AS_IsFromScript = true;
    obj->AS_AddRef();

    {
        m_Mutex_internal.unlock();
        m_Mutex.unlock();
    }

    return obj;

}

inline static std::vector<cTMap *> AS_GetFlowCoupling(RigidPhysicsWorld* w)
{
    std::vector<cTMap *> ret;

    if(w->m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    ret.push_back(w->m_BlockX->GetCopy());
    ret.push_back(w->m_BlockY->GetCopy());
    ret.push_back(w->m_BlockFX->GetCopy());
    ret.push_back(w->m_BlockFY->GetCopy());
    ret.push_back(w->m_BlockCaptureX->GetCopy());
    ret.push_back(w->m_BlockCaptureY->GetCopy());

    return ret;

}

inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockX()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockX->GetCopy();
}



inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockY()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockY->GetCopy();
}

inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockFX()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockFX->GetCopy();
}


inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockFY()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockFY->GetCopy();
}

inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockCX()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockCaptureX->GetCopy();
}


inline cTMap * RigidPhysicsWorld::AS_GetFlowBlockCY()
{

    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_BlockCaptureY->GetCopy();
}

inline cTMap * RigidPhysicsWorld::AS_GetFlowHCorrect()
{
    if(m_BlockX == nullptr)
    {
        LISEMS_ERROR("Could not get coupling feedback as no flow has been set, and no step ran");
        throw 1;
    }
    return m_HCorrected->GetCopy();
}

#include "geo/raster/field.h"


LISEM_API std::vector<Field*> RigidWorldToField(RigidPhysicsWorld* world, Field * ref);
LISEM_API void RigidWorldApplyPressureField(RigidPhysicsWorld* world, Field * Block, Field * P);
