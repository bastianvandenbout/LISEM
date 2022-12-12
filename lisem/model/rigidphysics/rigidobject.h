#pragma once

#include "defines.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/solver/ChSolverAPGD.h"
#include "chrono/geometry/ChTriangleMeshSoup.h"
#include "chrono/physics/ChBody.h"

using namespace chrono;

#include "linear/lsm_matrix4x4.h"
#include "linear/lsm_vector3.h"
#include <omp.h>
#include "geo/raster/map.h"
#include <thread>
#include "chrono/physics/ChBodyEasy.h"
#include "openglgeometry.h"
#include "models/3dmodel.h"
#include <functional>
#include "chrono/core/ChMatrix33.h"
#include "models/lsmmesh.h"
#include "linear/lsm_vector4.h"
#include "boundingbox.h"



struct ChBodyDeleteFunctor {
    void operator()(ChBody* p) {

    }
};




class LISEM_API RigidPhysicsObject
{
    friend class RigidPhysicsWorld;

public:
    //the family is used for styling and efficiently displaying groups of objects using the OpenGL Instancing feature.
    //this easily allows for an oder of magnitude higher object count
    QString m_Family;
    LSMVector3 m_FamilyScale = LSMVector3(1.0,1.0,1.0);
    std::shared_ptr<ChMaterialSurfaceNSC> m_chMaterial;
    std::shared_ptr<ChBody> m_chBody;
    bool m_IsAdded = false;
    bool m_IsTerrain = false;
    bool m_IsConvex = true;
    int m_UniqueID = 0;

    bool m_IsSimpleHeight = false;
    bool m_IsSimpleEllipsoid = false;
    bool m_IsSimpleCube = false;
    bool m_IsSimpleCilinder = false;
    bool m_IsSimpleMesh = false;
    double m_Density = 1.0;
    LSMVector3 m_SimpleSize = LSMVector3(1.0,1.0,1.0);

    LSMVector4 m_Highlight = LSMVector4(0.0,0.0,0.0,0.0);


    double m_Volume = 0.0;

    ModelGeometry * m_TriangulatedModel= nullptr;


    void * m_CustomPtr = nullptr;

    std::function<void(void)> m_Frem;
public:
    inline RigidPhysicsObject()
    {

    }


    inline ~RigidPhysicsObject()
    {
    }


public:

    inline void int_sethighlight(LSMVector4 highlight)
    {
        m_Highlight = highlight;

    }
    inline LSMVector4 int_gethighlight()
    {
        return m_Highlight;
    }

    inline bool IsConvex()
    {
        return m_IsConvex;
    }
    inline void SetRemoveCall(std::function<void(void)> f)
    {
        m_Frem = f;

    }

    inline void RemoveRemoveCall()
    {
        m_Frem = std::function<void(void)>();
    }

    inline void DoRemoveCall()
    {
        if(m_Frem)
        {
            m_Frem();
        }
    }

    inline LSMVector3 GetFamilyScale()
    {
        return m_FamilyScale;
    }
    inline std::shared_ptr<ChBody> GetChBody()
    {
        return m_chBody;
    }

    inline QString GetFamily()
    {
        return m_Family;
    }

    inline bool IsTerrain()
    {
        return m_IsTerrain;
    }
    inline LSMVector3 GetPosition()
    {
        if(m_chBody)
        {
            ChVector< double > pos = m_chBody->GetPos();
            return LSMVector3(pos[0],pos[1],pos[2]);
        }

        return LSMVector3(0.0,0.0,0.0);
    }

    inline LSMMatrix4x4 GetRotationMatrix()
    {
        Quaternion rot = m_chBody->GetFrame_REF_to_abs().GetRot();
        double angle;
        Vector axisc;
        rot.Q_to_AngAxis(angle, axisc);
        LSMVector3 axis = LSMVector3(axisc[0],axisc[1],axisc[2]).Normalize();

        //ChMatrix33< double > rotmat = m_chBody->GetA();
        LSMMatrix4x4 ret = LSMMatrix4x4();

        ret.SetRotation(axis,angle);

        return ret;
    }

    inline BoundingBox3D GetAABB()
    {
        ChVector< double > extentmin;
        ChVector< double > extentmax;

        m_chBody->GetTotalAABB(extentmin,extentmax);

        BoundingBox3D ret(extentmin[0],extentmax[0],extentmin[1],extentmax[1],extentmin[2],extentmax[2]);

        return ret;
    }

    inline void RecalculateVolume()
    {

    }
    inline void RecalculateSurfaceArea()
    {

    }

    inline double GetDragCoefficient(double reynollds)
    {




        return 0.0;
    }

    inline double GetSurfaceArea()
    {

        return 0.0;
    }

    inline double GetVolume()
    {
        return 0.0;
    }

    inline double GetProjectedSurfaceArea(LSMVector2 Velocity,BoundingBox extent, bool do_concave_detailed = false)
    {

        //project each triangle


        //for concave shape
        //

        return 0.0;
    }

    inline bool Contains(LSMVector3 pos)
    {
        //get the rotation and position of this body


        //transform pos to local coordinate system
        ChVector<double> relpos = m_chBody->Point_World2Body(ChVector<double>(pos.x,pos.y,pos.z));

        //ask triangulated model
        return this->m_TriangulatedModel->IsPointInside(LSMVector3(relpos[0],relpos[1],relpos[2]));
    }

    inline LSMVector3 GetRotation()
    {
        ChVector<double> vel = m_chBody->GetRot().Q_to_Euler123();
        return LSMVector3(vel[0],vel[1],vel[2]);
    }

    inline LSMVector3 GetRotationVelocity()
    {
        ChVector<double> vel = m_chBody->GetRot_dt().Q_to_Euler123();
        return LSMVector3(vel[0],vel[1],vel[2]);
    }

    inline LSMVector3 GetForces()
    {
        ChVector<double> vel = m_chBody->Get_accumulated_force();
        return LSMVector3(vel[0],vel[1],vel[2]);
    }

    inline LSMVector3 GetTorques()
    {
        ChVector<double> vel = m_chBody->Get_accumulated_torque();
        return LSMVector3(vel[0],vel[1],vel[2]);
    }

    inline LSMVector3 GetVelocity()
    {
        ChVector<double> vel = m_chBody->GetPos_dt();
        return LSMVector3(vel[0],vel[1],vel[2]);
    }
    inline LSMVector3 GetLocalLinearVelocity(LSMVector3 pos)
    {

        ChVector<double> relpos = m_chBody->Point_World2Body(ChVector<double>(pos.x,pos.y,pos.z));
        ChVector<double> absvel = m_chBody->PointSpeedLocalToParent(relpos);

        return LSMVector3(absvel[0],absvel[1],absvel[2]);

    }

    inline void ApplyForce(LSMVector3 pos, LSMVector3 f)
    {
        m_chBody->Accumulate_force(ChVector<double>(f.x,f.y,f.z),ChVector<double>(pos.x,pos.y,pos.z),false);
    }

    inline void ApplyTorque(LSMVector3 pos)
    {
        m_chBody->Accumulate_torque(ChVector<double>(pos.x,pos.y,pos.z),false);
    }

    inline void SetPosition(LSMVector3 pos)
    {
        m_chBody->SetPos(ChVector<double>(pos.x,pos.y,pos.z));
    }

    inline void SetVelocity(LSMVector3 vel)
    {
        m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
    }

    inline void SetRotation(LSMVector3 rotation)
    {
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        m_chBody->SetRot(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));

    }
    inline void SetRotationalVelocity(LSMVector3 rotation)
    {
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        m_chBody->SetRot_dt(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));
    }


    inline double GetConfinedVolume(BoundingBox & region, float h, float dhdx, float dhdz)
    {

        return 0.0;
    }

    inline bool CollidesWithCell(BoundingBox & region)
    {
        ChVector<> bbmin;
        ChVector<> bbmax;
        m_chBody->GetTotalAABB(bbmin,bbmax);

        BoundingBox b2(bbmin.x(),bbmax.x(),bbmin.z(),bbmax.z());

        return b2.Overlaps(region);
    }
    inline void * GetCustomPtr()
    {
        return m_CustomPtr;
    }

    inline void SetCustomPtr(void * p)
    {
        m_CustomPtr = p;
    }

    inline ModelGeometry * GetModel()
    {
        return m_TriangulatedModel;
    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsSphere(double radius, double density, LSMVector3 position = LSMVector3(0.0,0.0,0.0), LSMVector3 rotation = LSMVector3(0.0,0.0,0.0), LSMVector3 vel = LSMVector3(0.0,0.0,0.0), LSMVector3 rotvel = LSMVector3(0.0,0.0,0.0), double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "", bool is_static = false)
    {

        return  RigidPhysicsObject_AsEllipsoid(LSMVector3(radius,radius,radius),density,position,rotation,vel,rotvel,friction,compliance,complianceT,DampingF,family,is_static);

    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsEllipsoid(LSMVector3 radius, double density, LSMVector3 position = LSMVector3(0.0,0.0,0.0), LSMVector3 rotation = LSMVector3(0.0,0.0,0.0), LSMVector3 vel = LSMVector3(0.0,0.0,0.0), LSMVector3 rotvel = LSMVector3(0.0,0.0,0.0), double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "", bool is_static = false)
    {


        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->m_Density= density;
        ret->m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
        ret->m_chMaterial->SetFriction(friction);
        ret->m_chMaterial->SetCompliance(compliance);
        ret->m_chMaterial->SetComplianceT(complianceT);
        ret->m_chMaterial->SetDampingF(DampingF);
        ret->m_chBody = std::dynamic_pointer_cast<ChBody>(std::shared_ptr<ChBodyEasyEllipsoid>(new ChBodyEasyEllipsoid(ChVector<double>(radius.x,radius.y,radius.z),density,true,true,ret->m_chMaterial), [=](ChBodyEasyEllipsoid* b)
        {

            std::cout << "Deleting body Sphere\n" << b << std::endl;
            //delete b;
        }));
        ret->m_Family = family;
        ret->m_Volume = (4.0/3.0)*M_PI * radius.x * radius.y * radius.z;
        if(!ret->m_Family.isEmpty())
        {
            //save the scaling of this particular thing, as opposed to a unit mesh
            ret->m_FamilyScale = radius;
        }
        if(is_static)
        {
            ret->m_chBody->SetBodyFixed(true);
        }
        ret->m_chBody->SetPos(ChVector<double>(position.x,position.y,position.z));
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        ret->m_chBody->SetRot(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));
        ret->m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
        ModelGeometry *model = new ModelGeometry();
        LSMMesh m1;
        m1.SetAsEllipsoidTruncated(1,1,-1,1,0.0,10,10);
        m1.CalculateNormalsAndTangents();
        m1.Scale(radius.x,radius.y,radius.z);
        model->AddMesh(m1);
        ret->m_TriangulatedModel = model;
        ret->m_IsSimpleEllipsoid = true;
        ret->m_SimpleSize = radius;
        return ret;

    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsCilinder(double radius,double length, double density, LSMVector3 position = LSMVector3(0.0,0.0,0.0), LSMVector3 rotation = LSMVector3(0.0,0.0,0.0), LSMVector3 vel = LSMVector3(0.0,0.0,0.0), LSMVector3 rotvel = LSMVector3(0.0,0.0,0.0), double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "", bool is_static = false)
    {
        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->m_Density= density;
        ret->m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
        ret->m_chMaterial->SetFriction(friction);
        ret->m_chMaterial->SetCompliance(compliance);
        ret->m_chMaterial->SetComplianceT(complianceT);
        ret->m_chMaterial->SetDampingF(DampingF);
        ret->m_chBody = std::dynamic_pointer_cast<ChBody>(std::shared_ptr<ChBodyEasyCylinder>(new ChBodyEasyCylinder(radius,length,density,true,true,ret->m_chMaterial), [=](ChBodyEasyCylinder* b)
        {

            std::cout << "Deleting body cilinder\n" << b << std::endl;
            //delete b;
        }));
        ret->m_Family = family;
        ret->m_Volume = M_PI * std::pow(radius,2.0) * length;
        if(!ret->m_Family.isEmpty())
        {
            //save the scaling of this particular thing, as opposed to a unit mesh
            ret->m_FamilyScale = LSMVector3(radius,length,radius);
        }
        if(is_static)
        {
            ret->m_chBody->SetBodyFixed(true);
        }
        ret->m_chBody->SetPos(ChVector<double>(position.x,position.y,position.z));
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        ret->m_chBody->SetRot(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));
        ret->m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
        ModelGeometry *model = new ModelGeometry();
        LSMMesh m1;
        m1.SetAsCone(radius,radius,length/2.0,16);
        model->AddMesh(m1);
        ret->m_TriangulatedModel = model;
        ret->m_IsSimpleCilinder = true;
        ret->m_SimpleSize = LSMVector3(radius,length,0.0);
        return ret;

    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsBox(LSMVector3 Size, double density, LSMVector3 position = LSMVector3(0.0,0.0,0.0), LSMVector3 rotation = LSMVector3(0.0,0.0,0.0),LSMVector3 vel = LSMVector3(0.0,0.0,0.0), LSMVector3 rotvel = LSMVector3(0.0,0.0,0.0), double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "", bool is_static = false)
    {
        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->m_Density= density;
        ret->m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
        ret->m_chMaterial->SetFriction(friction);
        ret->m_chMaterial->SetCompliance(compliance);
        ret->m_chMaterial->SetComplianceT(complianceT);
        ret->m_chMaterial->SetDampingF(DampingF);
        ret->m_chBody = std::dynamic_pointer_cast<ChBody>(std::shared_ptr<ChBodyEasyBox>(new ChBodyEasyBox(Size.x,Size.y,Size.z,density,true,true,ret->m_chMaterial), [=](ChBodyEasyBox* b)
        {
            //delete b;
            std::cout << "Deleting body box\n" << b << std::endl;
            //delete b;
        }));

        ret->m_Family = family;
        ret->m_Volume = Size.x * Size.y * Size.z;
        if(!ret->m_Family.isEmpty())
        {
            //save the scaling of this particular thing, as opposed to a unit mesh
            ret->m_FamilyScale = Size;
        }
        if(is_static)
        {
            ret->m_chBody->SetBodyFixed(true);
        }
        ret->m_chBody->SetPos(ChVector<double>(position.x,position.y,position.z));
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        ret->m_chBody->SetRot(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));
        ret->m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
        ModelGeometry *model = new ModelGeometry();
        LSMMesh m1;
        m1.SetAsCube(Size);
        model->AddMesh(m1);
        ret->m_TriangulatedModel = model;
        ret->m_IsSimpleCube = true;
        ret->m_SimpleSize = Size;
        return ret;

    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsASMesh(ModelGeometry * gmodel,double density, LSMVector3 position = LSMVector3(0.0,0.0,0.0), LSMVector3 rotation = LSMVector3(0.0,0.0,0.0),LSMVector3 vel = LSMVector3(0.0,0.0,0.0), LSMVector3 rotvel = LSMVector3(0.0,0.0,0.0), double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "", bool is_static = false)
    {
        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->m_Density= density;
        ret->m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
        ret->m_chMaterial->SetFriction(friction);
        ret->m_chMaterial->SetCompliance(compliance);
        ret->m_chMaterial->SetComplianceT(complianceT);
        ret->m_chMaterial->SetDampingF(DampingF);
        ret->m_chBody = std::shared_ptr<ChBody>(new ChBody(), [=](ChBody* b)
        {
            std::cout << "Deleting body\n" << b << std::endl;
            //delete b;
            //delete b;

        });

        ret->m_chBody->SetCollide(true);
        std::shared_ptr<collision::ChCollisionModel> cmodel = ret->m_chBody->GetCollisionModel();

        bool is_closed = true;

        cmodel->ClearModel();
        if(!is_closed)
        {
            //dynamic shape requires mass and inertia, which can be estimated from a connected closed mesh only
            //for other mesh types this requires custom mass and inertia components, which is not supported in this helper function
            is_static = true;

            std::shared_ptr<geometry::ChTriangleMeshSoup> chgeom = chrono_types::make_shared<geometry::ChTriangleMeshSoup>();


            QList<LSMMesh*> meshes = gmodel->GetMeshes();
            //for each mesh
            for(int i = 0; i < meshes.length(); i++)
            {
                LSMMesh * m = meshes.at(i);
                m->CalcIsConvex();

                int triangleCount = m->indices.size()/3;


                //for each triangle
                for(int a = 0; a < triangleCount; a++)
                {
                    long i1 = m->indices.at(a * 3 + 0);
                    long i2 = m->indices.at(a * 3 + 1);
                    long i3 = m->indices.at(a * 3 + 2);

                    const LSMVector3& v1 = m->vertices[i1].position();
                    const LSMVector3& v2 = m->vertices[i2].position();
                    const LSMVector3& v3 = m->vertices[i3].position();

                    chgeom->addTriangle(ChVector<double>(v1.x,v1.y,v1.z),ChVector<double>(v2.x,v2.y,v2.z),ChVector<double>(v3.x,v3.y,v3.z));
                }
            }

            cmodel->AddTriangleMesh(ret->m_chMaterial,chgeom,true,false);
        }else
        {
            std::shared_ptr<geometry::ChTriangleMeshConnected> chgeom = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();


            QList<LSMMesh*> meshes = gmodel->GetMeshes();
            //for each mesh
            for(int i = 0; i < meshes.length(); i++)
            {
                LSMMesh * m = meshes.at(i);

                int triangleCount = m->indices.size()/3;


                //for each triangle
                for(int a = 0; a < triangleCount; a++)
                {
                    long i1 = m->indices.at(a * 3 + 0);
                    long i2 = m->indices.at(a * 3 + 1);
                    long i3 = m->indices.at(a * 3 + 2);

                    const LSMVector3& v1 = m->vertices[i1].position();
                    const LSMVector3& v2 = m->vertices[i2].position();
                    const LSMVector3& v3 = m->vertices[i3].position();

                    chgeom->addTriangle(ChVector<double>(v1.x,v1.y,v1.z),ChVector<double>(v2.x,v2.y,v2.z),ChVector<double>(v3.x,v3.y,v3.z));
                }
            }

            cmodel->AddTriangleMesh(ret->m_chMaterial,chgeom,true,false);
        }

        ret->m_Family = family;
        cmodel->BuildModel();

        ret->m_chBody->SetCollisionModel(cmodel);
        ret->m_chBody->SetCollide(true);
        ret->m_chBody->SetDensity((float)density);
        ret->m_chBody->SetMass(1.0);
        if(is_static)
        {
            ret->m_chBody->SetBodyFixed(true);
        }
        ret->m_chBody->SetInertiaXX(ChVector<>((1.0 / 12.0) * 1.0  ,
                                               (1.0 / 12.0) * 1.0  ,
                                               (1.0 / 12.0) * 1.0  ));

        ret->m_chBody->SetPos(ChVector<double>(position.x,position.y,position.z));
        LSMVector4 rotq = LSMVector4::QFromEulerAngles(rotation.x,rotation.y,rotation.z);
        ret->m_chBody->SetRot(ChQuaternion<double>(rotq.x,rotq.y,rotq.z,rotq.w));
        ret->m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
        ret->m_TriangulatedModel = gmodel->GetCopy();

        ret->m_IsSimpleMesh = true;

        return ret;

    }

    static inline RigidPhysicsObject * RigidPhysicsObject_AsMesh(ModelGeometry * gmodel,bool is_closed, bool is_static, double density = 1.0f, double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "")
    {
        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->m_Density= density;
        ret->m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
        ret->m_chMaterial->SetFriction(friction);
        ret->m_chMaterial->SetCompliance(compliance);
        ret->m_chMaterial->SetComplianceT(complianceT);
        ret->m_chMaterial->SetDampingF(DampingF);
        ret->m_chBody = std::shared_ptr<ChBody>(new ChBody(), [=](ChBody* b)
        {
            std::cout << "Deleting body\n" << b << std::endl;
            //delete b;
            //delete b;

        });

        ret->m_chBody->SetCollide(true);
        std::shared_ptr<collision::ChCollisionModel> cmodel = ret->m_chBody->GetCollisionModel();


        cmodel->ClearModel();
        if(!is_closed)
        {
            //dynamic shape requires mass and inertia, which can be estimated from a connected closed mesh only
            //for other mesh types this requires custom mass and inertia components, which is not supported in this helper function
            is_static = true;

            std::shared_ptr<geometry::ChTriangleMeshSoup> chgeom = chrono_types::make_shared<geometry::ChTriangleMeshSoup>();


            QList<LSMMesh*> meshes = gmodel->GetMeshes();
            //for each mesh
            for(int i = 0; i < meshes.length(); i++)
            {
                LSMMesh * m = meshes.at(i);
                m->CalcIsConvex();

                int triangleCount = m->indices.size()/3;


                //for each triangle
                for(int a = 0; a < triangleCount; a++)
                {
                    long i1 = m->indices.at(a * 3 + 0);
                    long i2 = m->indices.at(a * 3 + 1);
                    long i3 = m->indices.at(a * 3 + 2);

                    const LSMVector3& v1 = m->vertices[i1].position();
                    const LSMVector3& v2 = m->vertices[i2].position();
                    const LSMVector3& v3 = m->vertices[i3].position();

                    chgeom->addTriangle(ChVector<double>(v1.x,v1.y,v1.z),ChVector<double>(v2.x,v2.y,v2.z),ChVector<double>(v3.x,v3.y,v3.z));
                }
            }

            cmodel->AddTriangleMesh(ret->m_chMaterial,chgeom,true,false);
        }else
        {
            std::shared_ptr<geometry::ChTriangleMeshConnected> chgeom = chrono_types::make_shared<geometry::ChTriangleMeshConnected>();


            QList<LSMMesh*> meshes = gmodel->GetMeshes();
            //for each mesh
            for(int i = 0; i < meshes.length(); i++)
            {
                LSMMesh * m = meshes.at(i);

                int triangleCount = m->indices.size()/3;


                //for each triangle
                for(int a = 0; a < triangleCount; a++)
                {
                    long i1 = m->indices.at(a * 3 + 0);
                    long i2 = m->indices.at(a * 3 + 1);
                    long i3 = m->indices.at(a * 3 + 2);

                    const LSMVector3& v1 = m->vertices[i1].position();
                    const LSMVector3& v2 = m->vertices[i2].position();
                    const LSMVector3& v3 = m->vertices[i3].position();

                    chgeom->addTriangle(ChVector<double>(v1.x,v1.y,v1.z),ChVector<double>(v2.x,v2.y,v2.z),ChVector<double>(v3.x,v3.y,v3.z));
                }
            }

            cmodel->AddTriangleMesh(ret->m_chMaterial,chgeom,true,false);
        }

        ret->m_Family = family;
        cmodel->BuildModel();
        ret->m_chBody->SetCollisionModel(cmodel);
        ret->m_chBody->SetCollide(true);
        ret->m_chBody->SetDensity((float)density);
        //ret->m_chBody->SetPos_dt(ChVector<double>(vel.x,vel.y,vel.z));
        ret->m_chBody->SetMass(1.0);
        if(is_static)
        {
            ret->m_chBody->SetBodyFixed(true);
        }
        ret->m_chBody->SetInertiaXX(ChVector<>((1.0 / 12.0) * 1.0  ,
                                               (1.0 / 12.0) * 1.0  ,
                                               (1.0 / 12.0) * 1.0  ));
        ret->m_TriangulatedModel = gmodel;
        ret->m_IsSimpleMesh = true;

        return ret;
    }


    static inline QList<RigidPhysicsObject *> RigidPhysicsObject_AsHeightField(cTMap * DEM, bool add_northwest = false, double friction = 0.4, double compliance = 0.0, double complianceT = 0.0, double DampingF = 0.2, QString family = "")
    {


        QList<RigidPhysicsObject *> retlist;

        //divide the full raster into smaller objects containing a 8x8 grid
        //8x8 grids means 64 * 2 = 128 triangles per mesh
        //this seams a reasonable trade-off between number of objects and number of triangles per object
        //if the raster size is not an 8-multiple, there is some half-empty objects at the edges

        ModelGeometry * geom;

        int npp = 8;

        int nr_obj_r = std::max(1,(DEM->nrRows()/npp)+1);
        int nr_obj_c = std::max(1,(DEM->nrCols()/npp)+1);

        double north = add_northwest? DEM->north():0.0;
        double west = add_northwest? DEM->west():0.0;

        for(int objr = 0; objr < nr_obj_r; objr++)
        {
            for(int objc = 0; objc < nr_obj_c; objc++)
            {


                ModelGeometry * model = new ModelGeometry();

                std::vector<Vertex> vertexdata;
                std::vector<unsigned int> indexdata;


                for(int r2 = 0; r2 < npp+1; r2++)
                {
                    for(int c2 = 0; c2 < npp+1; c2++)
                    {
                        int r = objr*npp + r2;
                        int c = objc*npp + c2;

                        r= std::max(0,std::min(DEM->nrRows()-1,r));
                        c= std::max(0,std::min(DEM->nrCols()-1,c));

                        float uvx = float(c)/float(DEM->nrCols());
                        float uvy = float(r)/float(DEM->nrRows());

                        Vertex v1;
                        v1.setPosition(LSMVector3(west+ DEM->cellSizeX()*c, DEM->data[r][c], north + DEM->cellSizeY()*r));
                        v1.setUV(LSMVector2(uvx,uvy));

                        vertexdata.push_back(v1);
                    }
                }

                for(int r2 = 0; r2 < npp; r2++)
                {
                    for(int c2 = 0; c2 < npp; c2++)
                    {
                        //triangle 1
                        unsigned int i1 = r2 * (npp+1) + c2;
                        unsigned int i2 = r2 * (npp+1) + (c2+1);
                        unsigned int i3 = (r2+1) * (npp+1) + c2;

                        //triangle 2
                        unsigned int i4 = r2 * (npp+1) + (c2+1);
                        unsigned int i5 = (r2 +1)* (npp+1) + (c2+1);
                        unsigned int i6 = (r2+1) * (npp+1) + c2;


                        indexdata.push_back(i1);
                        indexdata.push_back(i2);
                        indexdata.push_back(i3);
                        indexdata.push_back(i4);
                        indexdata.push_back(i5);
                        indexdata.push_back(i6);
                    }
                }


                LSMMesh m = LSMMesh(vertexdata,indexdata);
                m.CalculateNormals();
                m.SetIsConvex(false);
                model->AddMesh(std::move(m));


                RigidPhysicsObject * ret = RigidPhysicsObject_AsMesh(model,false,true);

                ret->m_IsTerrain = true;
                ret->m_IsSimpleHeight = true;
                retlist.append(ret);
            }
        }

        return retlist;

    }

    inline double GetDensity()
    {
        return m_Density;
    }

    inline double GetMass()
    {
        return GetVolume() * GetDensity();
    }

    inline LSMMatrix3x3 GetInertia()
    {

        ChMatrix33<> inertia = m_chBody->GetInertia();

        LSMMatrix3x3 ret;
        ret.data[0][0] = inertia(0,0);
        ret.data[0][1] = inertia(0,1);
        ret.data[0][2] = inertia(0,2);
        ret.data[1][0] = inertia(1,0);
        ret.data[1][1] = inertia(1,1);
        ret.data[1][2] = inertia(1,2);
        ret.data[2][0] = inertia(2,0);
        ret.data[2][1] = inertia(2,1);
        ret.data[2][2] = inertia(2,2);

        return ret;


    }

    void CopyFrom(RigidPhysicsObject * body);

    inline RigidPhysicsObject * GetCopy()
    {
        std::cout << "get copy" << std::endl;
        RigidPhysicsObject * ret = new RigidPhysicsObject();
        ret->CopyFrom(this);
        std::cout << "end get copy "<< std::endl;
        return ret;
    }

public:

    QString    AS_FileName          = "";
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    bool AS_IsFromScript = false;
    bool        AS_IsScript();
    RigidPhysicsObject *     AS_Assign   (RigidPhysicsObject *other);


    //add or remove objects


    //get object properties



};

inline bool RigidPhysicsObject::AS_IsScript()
{
    return AS_IsFromScript;
}
inline void RigidPhysicsObject::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline RigidPhysicsObject* RigidPhysicsObject::AS_Assign(RigidPhysicsObject * other)
{

    std::cout << "start assign "<< std::endl;
    //copy all the contents from the other object to this
    this->CopyFrom(other);

    std::cout << " end assign" << std::endl;

    return this;
}

inline void RigidPhysicsObject::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        delete this;
    }

}

/*inline RigidPhysicsWorld * RigidPhysicsWorld::AS_Assign(RigidPhysicsWorld*other)
{


    return this;
}*/

inline static RigidPhysicsObject * AS_RigidObjectFactory()
{
    return new RigidPhysicsObject();


}


