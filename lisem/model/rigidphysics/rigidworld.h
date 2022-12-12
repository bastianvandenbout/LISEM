#pragma once


#include "defines.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/solver/ChSolverAPGD.h"
#include "chrono/solver/ChSolverPSOR.h"
#include "chrono/solver/ChSolverBB.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"

using namespace chrono;

QString base64_encode(QString string);
QString base64_decode(QString string);


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

    void SetObjectHighlight(RigidPhysicsObject*obj, LSMVector4 highlight,bool has_mutex = false);
    void AddObject(RigidPhysicsObject * object,bool has_mutex = false, bool add_geo = false);
    void RemoveObject(RigidPhysicsObject * object,bool has_mutex = false);
    int GetObjectCount(bool has_mutex, bool include_terrain = true);
    void LockMutex();
    void UnLockMutex();

    inline BoundingBox3D GetBoundingBox()
    {

        m_Mutex.lock();
        m_Mutex_internal.lock();

        BoundingBox3D res;
        if(m_Objects.length() == 0)
        {
            res = BoundingBox3D(-1.0,1.0,-1.0,1.0,-1.0,1.0);
        }
        for(int i = 0; i < m_Objects.length(); i++)
        {
            RigidPhysicsObject * object = m_Objects.at(i);
            BoundingBox3D b = object->GetAABB();

            if(i == 0)
            {
                res = b;
            }else
            {
                res.Merge(b);
            }

        }

        res.Move(LSMVector3(GetSimpleGeoOrigin().x,0.0,GetSimpleGeoOrigin().y));



        m_Mutex.unlock();
        m_Mutex_internal.unlock();

        return res;

    }


    inline void Empty()
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

    template <typename B>
    inline bool JSonGetValueBool(B jsondoc, QString name, bool backup = false)
    {
        QJsonValue val = jsondoc.value(name);
        if(val.isUndefined() || val.isNull())
        {
            return backup;
        }else
        {
            double ret = val.toBool(false);
            return ret;
        }
    }

    template <typename B>
    inline double JSonGetValueFloat(B jsondoc, QString name, double backup = 0.0)
    {
        QJsonValue val = jsondoc.value(name);
        if(val.isUndefined() || val.isNull())
        {
            return backup;
        }else
        {
            double ret = val.toDouble(backup);
            return ret;
        }
    }
    template <typename B>
    inline int JSonGetValueInt(B jsondoc, QString name, int backup = 0)
    {
        QJsonValue val = jsondoc.value(name);
        if(val.isUndefined() || val.isNull())
        {
            return backup;
        }else
        {
            int ret = val.toInt(backup);
            return ret;
        }
    }
    template <typename B>
    inline QString JSonGetValueString(B jsondoc, QString name, QString backup = "")
    {
        QJsonValue val = jsondoc.value(name);
        if(val.isUndefined() || val.isNull())
        {
            return backup;
        }else
        {
            QString ret = QString(val.toString());
            return ret;
        }
    }
    inline bool ExportToFile(QString path)
    {
        QJsonObject sceneJson;

        sceneJson.insert("object_type","LISEM_RIGID_WORLD");


        //gravity
        sceneJson.insert("Gravity_x",QJsonValue(m_system.Get_G_acc().x()));
        sceneJson.insert("Gravity_y",QJsonValue(m_system.Get_G_acc().y()));
        sceneJson.insert("Gravity_z",QJsonValue(m_system.Get_G_acc().z()));

        sceneJson.insert("geo_x",QJsonValue(GetSimpleGeoOrigin().x));
        sceneJson.insert("geo_y",QJsonValue(GetSimpleGeoOrigin().y));

        for(int i = 0; i < m_Objects.length(); i++)
        {
            QJsonObject jsob;

            //store object pos
            RigidPhysicsObject * obj = m_Objects.at(i);
            jsob.insert("pos_x",obj->GetPosition().x);
            jsob.insert("pos_y",obj->GetPosition().y);
            jsob.insert("pos_z",obj->GetPosition().z);

            //store object pos_vel
            jsob.insert("dpos_x",obj->GetVelocity().x);
            jsob.insert("dpos_y",obj->GetVelocity().y);
            jsob.insert("dpos_z",obj->GetVelocity().z);

            //store object rot
            jsob.insert("rot_x",obj->GetRotation().x);
            jsob.insert("rot_y",obj->GetRotation().y);
            jsob.insert("rot_z",obj->GetRotation().z);

            //store object rot_vel
            jsob.insert("drot_x",obj->GetRotationVelocity().x);
            jsob.insert("drot_y",obj->GetRotationVelocity().y);
            jsob.insert("drot_z",obj->GetRotationVelocity().z);

            //store object params (friction,density etc..)

            jsob.insert("density",obj->GetDensity());
            jsob.insert("staticfriction",obj->m_chMaterial->GetSfriction());
            jsob.insert("kineticfriction",obj->m_chMaterial->GetKfriction());
            jsob.insert("rollingfriction",obj->m_chMaterial->GetRollingFriction());
            jsob.insert("spinningfriction",obj->m_chMaterial->GetSpinningFriction());
            jsob.insert("restitution",obj->m_chMaterial->GetRestitution());
            jsob.insert("compliance",obj->m_chMaterial->GetCompliance());
            jsob.insert("compliancet",obj->m_chMaterial->GetComplianceT());
            jsob.insert("rollingcompliance",obj->m_chMaterial->GetComplianceRolling());
            jsob.insert("spinningcompliance",obj->m_chMaterial->GetComplianceSpinning());
            jsob.insert("dampingf",obj->m_chMaterial->GetDampingF());
            jsob.insert("cohesion",obj->m_chMaterial->GetCohesion());

            jsob.insert("fixed",obj->GetChBody()->GetBodyFixed());

            jsob.insert("mass",obj->GetMass());
            jsob.insert("convex",obj->m_IsConvex);
            jsob.insert("volume",obj->GetVolume());

            LSMMatrix3x3 inertia = obj->GetInertia();
            jsob.insert("inertia_xx",inertia.data[0][0]);
            jsob.insert("inertia_yy",inertia.data[1][1]);
            jsob.insert("inertia_zz",inertia.data[2][2]);
            jsob.insert("inertia_xy",inertia.data[1][2]);
            jsob.insert("inertia_yz",inertia.data[2][1]);
            jsob.insert("inertia_xz",inertia.data[0][2]);

            jsob.insert("is_simple_height", obj->m_IsSimpleHeight);
            jsob.insert("is_simple_ellipsoid",obj->m_IsSimpleEllipsoid);
            jsob.insert("is_simple_cube",obj->m_IsSimpleCube);
            jsob.insert("is_simple_cilinder", obj->m_IsSimpleCilinder);
            jsob.insert("is_mesh", obj->m_IsSimpleMesh);

            jsob.insert("simple_size_x", obj->m_SimpleSize.x);
            jsob.insert("simple_size_y", obj->m_SimpleSize.y);
            jsob.insert("simple_size_z", obj->m_SimpleSize.z);

            jsob.insert("familyname", obj->m_Family);

            //store if object is static
            //store object mesh
            //is it a simple mesh? Ellipsoid/Cilinder/Box/
            //store simple mesh dimensions
            //otherwise store number of indices and vertices
            //store index data
                //QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<char *>(&val), sizeof(float));
            //store vertex data

            if(obj->m_IsSimpleMesh)
            {

                ModelGeometry * model = obj->GetModel();
                int meshcount = model->GetMeshCount();
                for(int j = 0; j < meshcount; j++)
                {
                    QJsonObject mobj;


                    LSMMesh * mesh = model->GetMesh(j);


                    mobj.insert("facecount",mesh->GetFaceCount());
                    mobj.insert("vertexcount",mesh->GetVertexCount());

                    //cast data to chars and store as bytes, much more efficient in case of large number of vertices
                    QByteArray byteArray = QByteArray::fromRawData((char *)mesh->GetVertexPtr(0), sizeof(Vertex) *mesh->GetVertexCount());

                    mobj.insert("vertexdata",QString(byteArray.toBase64()));

                    std::vector<int> faces;
                    for(int f = 0; f < mesh->GetFaceCount(); f++)
                    {
                        faces.push_back(mesh->GetFace(f).i_1);
                        faces.push_back(mesh->GetFace(f).i_2);
                        faces.push_back(mesh->GetFace(f).i_3);

                    }
                    QByteArray byteArray2 = QByteArray::fromRawData((char *)(&faces[0]), sizeof(int) *mesh->GetFaceCount() * 3);

                    mobj.insert("indexdata",QString(byteArray2.toBase64()));
                    jsob.insert("mesh_"+ QString::number(j),mobj);
                }
            }
            sceneJson.insert("obj_"+QString::number(i),jsob);
        }

        for(int i = 0; i < m_DEMObjects.length(); i++)
        {
            QJsonObject jsob;

            //store object pos
            RigidPhysicsObject * obj = m_DEMObjects.at(i);
            jsob.insert("pos_x",obj->GetPosition().x);
            jsob.insert("pos_y",obj->GetPosition().y);
            jsob.insert("pos_z",obj->GetPosition().z);

            //store object pos_vel
            jsob.insert("dpos_x",obj->GetVelocity().x);
            jsob.insert("dpos_y",obj->GetVelocity().y);
            jsob.insert("dpos_z",obj->GetVelocity().z);

            //store object rot
            jsob.insert("rot_x",obj->GetRotation().x);
            jsob.insert("rot_y",obj->GetRotation().y);
            jsob.insert("rot_z",obj->GetRotation().z);

            //store object rot_vel
            jsob.insert("drot_x",obj->GetRotationVelocity().x);
            jsob.insert("drot_y",obj->GetRotationVelocity().y);
            jsob.insert("drot_z",obj->GetRotationVelocity().z);

            //store object params (friction,density etc..)


            jsob.insert("density",obj->GetDensity());
            jsob.insert("staticfriction",obj->m_chMaterial->GetSfriction());
            jsob.insert("kineticfriction",obj->m_chMaterial->GetKfriction());
            jsob.insert("rollingfriction",obj->m_chMaterial->GetRollingFriction());
            jsob.insert("spinningfriction",obj->m_chMaterial->GetSpinningFriction());
            jsob.insert("restitution",obj->m_chMaterial->GetRestitution());
            jsob.insert("compliance",obj->m_chMaterial->GetCompliance());
            jsob.insert("compliancet",obj->m_chMaterial->GetComplianceT());
            jsob.insert("rollingcompliance",obj->m_chMaterial->GetComplianceRolling());
            jsob.insert("spinningcompliance",obj->m_chMaterial->GetComplianceSpinning());
            jsob.insert("dampingf",obj->m_chMaterial->GetDampingF());
            jsob.insert("cohesion",obj->m_chMaterial->GetCohesion());

            jsob.insert("fixed",obj->GetChBody()->GetBodyFixed());

            jsob.insert("mass",obj->GetMass());
            jsob.insert("convex",obj->m_IsConvex);
            jsob.insert("volume",obj->GetVolume());

            LSMMatrix3x3 inertia = obj->GetInertia();
            jsob.insert("inertia_xx",inertia.data[0][0]);
            jsob.insert("inertia_yy",inertia.data[1][1]);
            jsob.insert("inertia_zz",inertia.data[2][2]);
            jsob.insert("inertia_xy",inertia.data[1][2]);
            jsob.insert("inertia_yz",inertia.data[2][1]);
            jsob.insert("inertia_xz",inertia.data[0][2]);

            jsob.insert("is_simple_height", obj->m_IsSimpleHeight);
            jsob.insert("is_simple_ellipsoid",obj->m_IsSimpleEllipsoid);
            jsob.insert("is_simple_cube",obj->m_IsSimpleCube);
            jsob.insert("is_simple_cilinder", obj->m_IsSimpleCilinder);
            jsob.insert("is_mesh", obj->m_IsSimpleMesh);

            jsob.insert("simple_size_x", obj->m_SimpleSize.x);
            jsob.insert("simple_size_y", obj->m_SimpleSize.y);
            jsob.insert("simple_size_z", obj->m_SimpleSize.z);

            jsob.insert("familyname", obj->m_Family);

            //store if object is static
            //store object mesh
            //is it a simple mesh? Ellipsoid/Cilinder/Box/
            //store simple mesh dimensions
            //otherwise store number of indices and vertices
            //store index data
                //QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<char *>(&val), sizeof(float));
            //store vertex data

            if(obj->m_IsSimpleMesh)
            {

                ModelGeometry * model = obj->GetModel();
                int meshcount = model->GetMeshCount();
                for(int j = 0; j < meshcount; j++)
                {
                    QJsonObject mobj;


                    LSMMesh * mesh = model->GetMesh(j);


                    mobj.insert("facecount",mesh->GetFaceCount());
                    mobj.insert("vertexcount",mesh->GetVertexCount());

                    //cast data to chars and store as bytes, much more efficient in case of large number of vertices
                    QByteArray byteArray = QByteArray::fromRawData((char *)mesh->GetVertexPtr(0), sizeof(Vertex) *mesh->GetVertexCount());

                    mobj.insert("vertexdata",QString(byteArray.toBase64()));

                    std::vector<int> faces;
                    for(int f = 0; f < mesh->GetFaceCount(); f++)
                    {
                        faces.push_back(mesh->GetFace(f).i_1);
                        faces.push_back(mesh->GetFace(f).i_2);
                        faces.push_back(mesh->GetFace(f).i_3);

                    }
                    QByteArray byteArray2 = QByteArray::fromRawData((char *)(&faces[0]), sizeof(int) *mesh->GetFaceCount() * 3);

                    mobj.insert("indexdata",QString(byteArray2.toBase64()));
                    jsob.insert("mesh_"+ QString::number(j),mobj);
                }
            }
            sceneJson.insert("demobj_"+QString::number(i),jsob);
        }


        //constraints?



        //done
        QJsonDocument document(sceneJson);


        QByteArray doc =  document.toJson();

        QFile file(path);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(doc);
            return true;
        }else
        {
            return false;
        }


    }

    inline bool ImportFromFile(QString path)
    {
        this->Empty();
        std::vector<RigidPhysicsObject*> objects;
        std::vector<RigidPhysicsObject*> demobjects;

        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
          return false;

        QByteArray wholeFile = file.readAll();

        QJsonObject jsonDocument = QJsonDocument::fromJson(wholeFile).object();

        QJsonValue val_objtype = jsonDocument.value("object_type");

        if(!val_objtype.isUndefined() && val_objtype.isString())
        {
            if(val_objtype.toString() != "LISEM_RIGID_WORLD")
            {
                LISEM_ERROR("Could not load rigid world file, object type in json incorrect");
                return false;
            }
        }else{
            LISEM_ERROR("Could not load rigid world file, object type in json incorrect");
            return false;

        }

        double grav_x = JSonGetValueFloat(jsonDocument,"Gravity_x",0.0);
        double grav_y = JSonGetValueFloat(jsonDocument,"Gravity_y",-9.81);
        double grav_z = JSonGetValueFloat(jsonDocument,"Gravity_z",0.0);

        double geo_x = JSonGetValueFloat(jsonDocument,"geo_x",0.0);
        double geo_y = JSonGetValueFloat(jsonDocument,"geo_y",0.0);

        m_system.Set_G_acc(ChVector<double>(grav_x,grav_y,grav_z));
        m_GeoOrigin.x = geo_x;
        m_GeoOrigin.y = geo_y;

        int i = 0;
        bool found = true;
        while(found)
        {

            i++;
            QJsonValue valobji = jsonDocument.value("obj_"+QString::number(i));
            if( !valobji.isUndefined())
            {
                found= true;

                if(valobji.isObject())
                {
                    std::cout << "object found " << std::endl;
                    QJsonObject jsob = valobji.toObject();
                    double pos_x = JSonGetValueFloat(jsob,"pos_x",0.0);
                    double pos_y = JSonGetValueFloat(jsob,"pos_y",0.0);
                    double pos_z = JSonGetValueFloat(jsob,"pos_z",0.0);
                    double dpos_x = JSonGetValueFloat(jsob,"dpos_x",0.0);
                    double dpos_y = JSonGetValueFloat(jsob,"dpos_y",0.0);
                    double dpos_z = JSonGetValueFloat(jsob,"dpos_z",0.0);
                    double rot_x = JSonGetValueFloat(jsob,"rot_x",0.0);
                    double rot_y = JSonGetValueFloat(jsob,"rot_y",0.0);
                    double rot_z = JSonGetValueFloat(jsob,"rot_z",0.0);
                    double drot_x = JSonGetValueFloat(jsob,"drot_x",0.0);
                    double drot_y = JSonGetValueFloat(jsob,"drot_y",0.0);
                    double drot_z = JSonGetValueFloat(jsob,"drot_z",0.0);


                    double staticfriction = JSonGetValueFloat(jsob,"staticfriction",0.0);
                    double kineticfriction = JSonGetValueFloat(jsob,"kineticfriction",0.0);
                    double rollingfriction = JSonGetValueFloat(jsob,"rollingfriction",0.0);
                    double spinningfriction = JSonGetValueFloat(jsob,"spinningfriction",0.0);
                    double restitution = JSonGetValueFloat(jsob,"restitution",0.0);
                    double compliance = JSonGetValueFloat(jsob,"compliance",0.0);
                    double compliancet = JSonGetValueFloat(jsob,"compliancet",0.0);
                    double rollingcompliance = JSonGetValueFloat(jsob,"rollingcompliance",0.0);
                    double spinningcompliance = JSonGetValueFloat(jsob,"spinningcompliance",0.0);
                    double dampingf = JSonGetValueFloat(jsob,"dampingf",0.0);
                    double cohesion = JSonGetValueFloat(jsob,"cohesion",0.0);
                    bool fixed = JSonGetValueBool(jsob,"fixed",false);

                    double density = JSonGetValueFloat(jsob,"density",1.0);
                    double mass= JSonGetValueFloat(jsob,"mass",0.0);
                    bool convex = JSonGetValueBool(jsob,"convex",false);
                    double volume = JSonGetValueFloat(jsob,"volume",0.0);
                    double inertia_xx = JSonGetValueFloat(jsob,"inertia_xx",0.0);
                    double inertia_yy = JSonGetValueFloat(jsob,"inertia_yy",0.0);
                    double inertia_zz = JSonGetValueFloat(jsob,"inertia_zz",0.0);
                    double inertia_xy = JSonGetValueFloat(jsob,"inertia_xy",0.0);
                    double inertia_yz = JSonGetValueFloat(jsob,"inertia_yz",0.0);
                    double inertia_xz = JSonGetValueFloat(jsob,"inertia_xz",0.0);
                    bool is_simple_height = JSonGetValueBool(jsob,"is_simple_height",false);
                    bool is_simple_ellipsoid = JSonGetValueBool(jsob,"is_simple_ellipsoid",false);
                    bool is_simple_cube = JSonGetValueBool(jsob,"is_simple_cube",false);
                    bool is_simple_cilinder = JSonGetValueBool(jsob,"is_simple_cilinder",false);
                    bool is_mesh = JSonGetValueBool(jsob,"is_mesh",false);
                    double simple_size_x = JSonGetValueFloat(jsob,"simple_size_x",0.0);
                    double simple_size_y = JSonGetValueFloat(jsob,"simple_size_y",0.0);
                    double simple_size_z = JSonGetValueFloat(jsob,"simple_size_z",0.0);

                    QString fam_name = JSonGetValueString(jsob,"fam_name","");


                    RigidPhysicsObject * obji = nullptr;

                    if(is_mesh)
                    {
                        std::cout << "detect mesh " << std::endl;

                        ModelGeometry * geom = new ModelGeometry();

                        bool foundm = false;
                        int j = 0;
                        while(foundm)
                        {
                            j++;
                            QJsonValue valobjmi = jsonDocument.value("obj_"+QString::number(i));
                            if( !valobjmi.isUndefined())
                            {
                                found= true;


                                if(valobjmi.isObject())
                                {

                                    QJsonObject jsobm = valobjmi.toObject();
                                    int vertexcount = JSonGetValueInt(jsobm,"vertexcount",0);
                                    int facecount = JSonGetValueInt(jsobm,"facecount",0);

                                    QByteArray vertexdata = QByteArray::fromBase64(QByteArray(jsobm.value("vertexdata").toString().toStdString().c_str()));
                                    QByteArray indexdata = QByteArray::fromBase64(QByteArray(jsobm.value("indexdata").toString().toStdString().c_str()));

                                    std::vector<Vertex> vertices;
                                    std::vector<unsigned int> indices;

                                    //get the data from the QByteArray
                                    const char * vdata = vertexdata.constData();
                                    int num_vertexdata = vertexdata.size()/sizeof(Vertex);

                                    const char * idata = indexdata.constData();
                                    int num_indexdata = indexdata.size()/sizeof(Vertex);

                                    for(int k = 0; k < num_vertexdata; k++)
                                    {
                                        Vertex vk = (((Vertex*)vdata)[k]);
                                        vertices.push_back(vk);
                                    }
                                    for(int k = 0; k < num_indexdata; k++)
                                    {
                                        unsigned int vk = (((int*)idata)[k]);
                                        indices.push_back(vk);
                                    }

                                    LSMMesh m(vertices,indices);
                                    geom->AddMesh(m);

                                }else
                                {
                                    found = false;
                                }
                            }




                        }
                        obji = RigidPhysicsObject::RigidPhysicsObject_AsASMesh(geom,density,
                                                                     LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                               LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                               staticfriction,compliance,compliancet,dampingf,fam_name,fixed);



                    }else if(is_simple_cube)
                    {
                        obji = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(simple_size_x,simple_size_y,simple_size_z),density,
                                                                     LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                     LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                     staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                    }else if(is_simple_cilinder)
                    {
                        obji = RigidPhysicsObject::RigidPhysicsObject_AsCilinder(simple_size_x,simple_size_y,density,
                                                                     LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                     LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                     staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                    }else if(is_simple_ellipsoid)
                    {
                        obji = RigidPhysicsObject::RigidPhysicsObject_AsEllipsoid(LSMVector3(simple_size_x,simple_size_y,simple_size_z),density,
                                                                     LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                     LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                     staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                    }else
                    {


                    }

                    if(obji != nullptr)
                    {
                        //set inertia, mass, volume, material properties

                        obji->GetChBody()->SetMass(mass);
                        ChMatrix33<> inertia;// = m_chBody->GetInertia();
                        inertia(0,0);
                        inertia(0,1);
                        inertia(0,2);
                        inertia(1,0);
                        inertia(1,1);
                        inertia(1,2);
                        inertia(2,0);
                        inertia(2,1);
                        inertia(2,2);

                        obji->GetChBody()->SetInertia(inertia);
                        std::shared_ptr<chrono::collision::ChCollisionModel> model = obji->GetChBody()->GetCollisionModel();

                        for(int l = 0; l < model->GetNumShapes(); l++)
                        {
                            std::shared_ptr< chrono::collision::ChCollisionShape > shape = model->GetShape(l);
                            std::shared_ptr<chrono::ChMaterialSurface> material = shape->GetMaterial();


                        }
                        objects.push_back(obji);
                    }

                }

            }else
            {
                found = false;
            }
        }


        i = 0;
        found = true;
        while(found)
        {

            i++;
            QJsonValue valobji = jsonDocument.value("demobj_"+QString::number(i));
            if( !valobji.isUndefined())
            {
                found= true;

                if(valobji.isObject())
                {
                    QJsonObject jsob = valobji.toObject();
                    double pos_x = JSonGetValueFloat(jsob,"pos_x",0.0);
                    double pos_y = JSonGetValueFloat(jsob,"pos_y",0.0);
                    double pos_z = JSonGetValueFloat(jsob,"pos_z",0.0);
                    double dpos_x = JSonGetValueFloat(jsob,"dpos_x",0.0);
                    double dpos_y = JSonGetValueFloat(jsob,"dpos_y",0.0);
                    double dpos_z = JSonGetValueFloat(jsob,"dpos_z",0.0);
                    double rot_x = JSonGetValueFloat(jsob,"rot_x",0.0);
                    double rot_y = JSonGetValueFloat(jsob,"rot_y",0.0);
                    double rot_z = JSonGetValueFloat(jsob,"rot_z",0.0);
                    double drot_x = JSonGetValueFloat(jsob,"drot_x",0.0);
                    double drot_y = JSonGetValueFloat(jsob,"drot_y",0.0);
                    double drot_z = JSonGetValueFloat(jsob,"drot_z",0.0);

                    double staticfriction = JSonGetValueFloat(jsob,"staticfriction",0.0);
                    double kineticfriction = JSonGetValueFloat(jsob,"kineticfriction",0.0);
                    double rollingfriction = JSonGetValueFloat(jsob,"rollingfriction",0.0);
                    double spinningfriction = JSonGetValueFloat(jsob,"spinningfriction",0.0);
                    double restitution = JSonGetValueFloat(jsob,"restitution",0.0);
                    double compliance = JSonGetValueFloat(jsob,"compliance",0.0);
                    double compliancet = JSonGetValueFloat(jsob,"compliancet",0.0);
                    double rollingcompliance = JSonGetValueFloat(jsob,"rollingcompliance",0.0);
                    double spinningcompliance = JSonGetValueFloat(jsob,"spinningcompliance",0.0);
                    double dampingf = JSonGetValueFloat(jsob,"dampingf",0.0);
                    double cohesion = JSonGetValueFloat(jsob,"cohesion",0.0);
                    bool fixed = JSonGetValueBool(jsob,"fixed",false);

                    double density = JSonGetValueFloat(jsob,"density",1.0);
                    double mass= JSonGetValueFloat(jsob,"mass",0.0);
                    bool convex = JSonGetValueBool(jsob,"convex",false);
                    double volume = JSonGetValueFloat(jsob,"volume",0.0);
                    double inertia_xx = JSonGetValueFloat(jsob,"inertia_xx",0.0);
                    double inertia_yy = JSonGetValueFloat(jsob,"inertia_yy",0.0);
                    double inertia_zz = JSonGetValueFloat(jsob,"inertia_zz",0.0);
                    double inertia_xy = JSonGetValueFloat(jsob,"inertia_xy",0.0);
                    double inertia_yz = JSonGetValueFloat(jsob,"inertia_yz",0.0);
                    double inertia_xz = JSonGetValueFloat(jsob,"inertia_xz",0.0);
                    bool is_simple_height = JSonGetValueBool(jsob,"is_simple_height",false);
                    bool is_simple_ellipsoid = JSonGetValueBool(jsob,"is_simple_ellipsoid",false);
                    bool is_simple_cube = JSonGetValueBool(jsob,"is_simple_cube",false);
                    bool is_simple_cilinder = JSonGetValueBool(jsob,"is_simple_cilinder",false);
                    bool is_mesh = JSonGetValueBool(jsob,"is_mesh",false);
                    double simple_size_x = JSonGetValueFloat(jsob,"simple_size_x",0.0);
                    double simple_size_y = JSonGetValueFloat(jsob,"simple_size_y",0.0);
                    double simple_size_z = JSonGetValueFloat(jsob,"simple_size_z",0.0);

                    QString fam_name = JSonGetValueString(jsob,"fam_name","");


                    RigidPhysicsObject * obji = nullptr;

                    if(is_mesh)
                    {

                        ModelGeometry * geom = new ModelGeometry();

                        bool foundm = false;
                        int j = 0;
                        while(foundm)
                        {
                            j++;
                            QJsonValue valobjmi = jsonDocument.value("obj_"+QString::number(i));
                            if( !valobjmi.isUndefined())
                            {
                                found= true;


                                if(valobjmi.isObject())
                                {

                                    QJsonObject jsobm = valobjmi.toObject();
                                    int vertexcount = JSonGetValueInt(jsobm,"vertexcount",0);
                                    int facecount = JSonGetValueInt(jsobm,"facecount",0);

                                    QByteArray vertexdata = QByteArray::fromBase64(QByteArray(jsobm.value("vertexdata").toString().toStdString().c_str()));
                                    QByteArray indexdata = QByteArray::fromBase64(QByteArray(jsobm.value("indexdata").toString().toStdString().c_str()));

                                    std::vector<Vertex> vertices;
                                    std::vector<unsigned int> indices;

                                    //get the data from the QByteArray
                                    const char * vdata = vertexdata.constData();
                                    int num_vertexdata = vertexdata.size()/sizeof(Vertex);

                                    const char * idata = indexdata.constData();
                                    int num_indexdata = indexdata.size()/sizeof(Vertex);

                                    for(int k = 0; k < num_vertexdata; k++)
                                    {
                                        Vertex vk = (((Vertex*)vdata)[k]);
                                        vertices.push_back(vk);
                                    }
                                    for(int k = 0; k < num_indexdata; k++)
                                    {
                                        unsigned int vk = (((int*)idata)[k]);
                                        indices.push_back(vk);
                                    }

                                    LSMMesh m(vertices,indices);
                                    geom->AddMesh(m);

                                }else
                                {
                                    found = false;
                                }
                            }

                            obji = RigidPhysicsObject::RigidPhysicsObject_AsASMesh(geom,density,
                                                                         LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                                   LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                                   staticfriction,compliance,compliancet,dampingf,fam_name,fixed);




                        }

                        }else if(is_simple_cube)
                        {
                            obji = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(simple_size_x,simple_size_y,simple_size_z),density,
                                                                         LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                         LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                         staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                        }else if(is_simple_cilinder)
                        {
                            obji = RigidPhysicsObject::RigidPhysicsObject_AsCilinder(simple_size_x,simple_size_y,density,
                                                                         LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                         LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                         staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                        }else if(is_simple_ellipsoid)
                        {
                            obji = RigidPhysicsObject::RigidPhysicsObject_AsEllipsoid(LSMVector3(simple_size_x,simple_size_y,simple_size_z),density,
                                                                         LSMVector3(pos_x,pos_y,pos_z),LSMVector3(rot_x,rot_y,rot_z),
                                                                         LSMVector3(dpos_x,dpos_y,dpos_z),LSMVector3(drot_x,drot_y,drot_z),
                                                                         staticfriction,compliance,compliancet,dampingf,fam_name,fixed);

                        }else
                        {


                        }

                        if(obji != nullptr)
                        {
                            //set inertia, mass, volume, material properties

                            obji->m_IsTerrain = is_simple_height;
                            obji->GetChBody()->SetMass(mass);
                            ChMatrix33<> inertia;// = m_chBody->GetInertia();
                            inertia(0,0);
                            inertia(0,1);
                            inertia(0,2);
                            inertia(1,0);
                            inertia(1,1);
                            inertia(1,2);
                            inertia(2,0);
                            inertia(2,1);
                            inertia(2,2);

                            obji->GetChBody()->SetInertia(inertia);
                            std::shared_ptr<chrono::collision::ChCollisionModel> model = obji->GetChBody()->GetCollisionModel();

                            for(int l = 0; l < model->GetNumShapes(); l++)
                            {
                                std::shared_ptr< chrono::collision::ChCollisionShape > shape = model->GetShape(l);
                                std::shared_ptr<chrono::ChMaterialSurface> material = shape->GetMaterial();


                            }
                            demobjects.push_back(obji);
                        }

                }

            }else
            {
                found = false;
            }
        }

        m_Mutex.lock();
        m_Mutex_internal.lock();

        for(int i = 0; i < objects.size(); i++)
        {
            std::cout << "add object to world " << std::endl;
            AddObject(objects.at(i),true,false);
            if(objects.at(i)->IsTerrain())
            {
                m_DEMObjects.push_back(objects.at(i));
            }
        }

        m_Mutex_internal.unlock();
        m_Mutex.unlock();

        return true;

    }

    void CopyFrom(RigidPhysicsWorld *w);
public:

    QString    AS_FileName          = "";
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    bool           AS_IsFromScript = false;
    RigidPhysicsWorld *     AS_Assign   (RigidPhysicsWorld *other);
    bool           AS_writeonassign     = false;
    std::function<void(RigidPhysicsWorld *,QString)> AS_writefunc;
    std::function<RigidPhysicsWorld *(QString)> AS_readfunc;
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
    if(AS_writeonassign)
    {
        AS_writefunc(other,AS_FileName);
    }
    this->AS_writeonassign = false;
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


    return GetObjectCount(false,false);
}

inline RigidPhysicsObject* RigidPhysicsWorld::AS_GetObj(int index)
{

    {
        m_Mutex.lock();
        m_Mutex_internal.lock();
    }


    if(index < 0 || index > m_Objects.length())
    {

        LISEMS_ERROR("RigidWorld::GetObject incorrect object index, out of bounds " + QString::number(index));
        throw 1;
    }


    RigidPhysicsObject* obj = nullptr;;

    int n = 0;
    for(int i = 0; i < m_Objects.size(); i++)
    {
        if(!m_Objects.at(i)->IsTerrain())
        {
            if(n == index)
            {
                obj = m_Objects.at(i);
                break;
            }
            n++;
        }
    }

    if(obj  == nullptr)
    {
        LISEMS_ERROR("RigidWorld::GetObject incorrect object index, out of bounds " + QString::number(index));
        throw 1;
    }

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

#include "lsmio.h"

inline static RigidPhysicsWorld * AS_LoadRigidWorldFromFileAbsPath(const QString & name)
{
    RigidPhysicsWorld * ret = new RigidPhysicsWorld();
    bool succ = ret->ImportFromFile(name);
    if(!succ)
    {
        ret->Destroy();
        delete ret;
        ret = nullptr;
    }
    if(ret == nullptr)
    {
        LISEMS_ERROR("Could not read Point Cloud from file: " + name);
        LISEM_ERROR("Could not read Point Cloud from file: " + name);
        throw 1;
    }else {
        return ret;
    }
}

inline static RigidPhysicsWorld * AS_LoadRigidWorldFromFile(const QString & name)
{
    return AS_LoadRigidWorldFromFileAbsPath(AS_DIR + name);
}

inline static void AS_SaveRigidWorldToFileAbsPath(RigidPhysicsWorld * pc, const QString & name)
{

    //call writing function for point cloud
    pc->ExportToFile(name);

}
inline static void AS_SaveRigidWorldToFile(RigidPhysicsWorld * pc,const QString & name)
{
    pc->ExportToFile(AS_DIR + name);
}

inline static RigidPhysicsWorld * AS_SaveThisRigidWorldToFile(const QString & path)
{
    RigidPhysicsWorld * _M = new RigidPhysicsWorld();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    _M->AS_IsFromScript = true;

    std::function<void(RigidPhysicsWorld *,QString)> writecall;
    writecall = [](RigidPhysicsWorld * m, QString p){AS_SaveRigidWorldToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<RigidPhysicsWorld *(QString)> readcall;
    readcall = [](QString p){return AS_LoadRigidWorldFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}


#include "geo/raster/field.h"


LISEM_API std::vector<Field*> RigidWorldToField(RigidPhysicsWorld* world, Field * ref);
LISEM_API void RigidWorldApplyPressureField(RigidPhysicsWorld* world, Field * Block, Field * P);
