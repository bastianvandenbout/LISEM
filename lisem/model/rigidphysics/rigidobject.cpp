#include "rigidobject.h"



void RigidPhysicsObject::CopyFrom(RigidPhysicsObject * body)
{
    std::cout << " begin copy obj" << std::endl;

    //shape
    /*

    for(int i = 0; i < cm->GetNumShapes(); i++)
    {
        std::shared_ptr<collision::ChCollisionShape> s = cm->GetShape(i);

        chrono::collision::ChCollisionShape::Type t = s->GetType();
        std::shared_ptr<ChMaterialSurface> m = s->GetMaterial();
        chrono::ChCoordsys<double> pos = cm->GetShapePos(i);
        std::vector<double> dims = cm->GetShapeDimensions(i);

        if(t == chrono::collision::ChCollisionShape::SPHERE)
        {
        }else if(t == chrono::collision::ChCollisionShape::ELLIPSOID)
        {
        }else if(t == chrono::collision::ChCollisionShape::BOX)
        {
        }else if(t == chrono::collision::ChCollisionShape::CYLINDER)
        {
        }else if(t == chrono::collision::ChCollisionShape::CONVEXHULL)
        {
        }else if(t == chrono::collision::ChCollisionShape::TRIANGLEMESH)
        {
        }
    }*/
    //approx geometry


    //locations

    //velocities

    //density/friction properties

    std::shared_ptr<collision::ChCollisionModel>  cm = body->m_chBody->GetCollisionModel();
    m_chBody = chrono_types::make_shared<ChBody>();
    m_chBody->SetCollide(body->m_chBody->GetCollide());

    m_chBody->GetCollisionModel()->ClearModel();
    m_chBody->GetCollisionModel()->AddCopyOfAnotherModel(cm.get());
    m_chBody->GetCollisionModel()->BuildModel();



    m_SimpleSize = body->m_SimpleSize;
    m_Volume = body->m_Volume;
    m_IsTerrain = body->m_IsTerrain;
    m_IsSimpleHeight = body->m_IsSimpleHeight;
    m_IsSimpleEllipsoid = body->m_IsSimpleEllipsoid;
    m_IsSimpleCube = body->m_IsSimpleCube;
    m_IsSimpleCilinder = body->m_IsSimpleCilinder;
    m_IsSimpleMesh = body->m_IsSimpleMesh;

    m_Family = body->m_Family;
    m_FamilyScale = body->m_FamilyScale;
    this->m_IsConvex =  body->m_IsConvex;
    this->m_IsTerrain =  body->m_IsTerrain;
    m_chMaterial = chrono_types::make_shared<ChMaterialSurfaceNSC>();
    m_chMaterial->SetFriction(body->m_chMaterial->GetKfriction());
    m_chMaterial->SetDampingF(body->m_chMaterial->GetDampingF());
    m_chMaterial->SetCompliance(body->m_chMaterial->GetCompliance());
    m_chMaterial->SetComplianceT(body->m_chMaterial->GetComplianceT());
    m_chBody->GetCollisionModel()->SetAllShapesMaterial(m_chMaterial);

    m_chBody->SetPos(body->m_chBody->GetPos());
    m_chBody->SetPos_dt(body->m_chBody->GetPos_dt());
    m_chBody->SetRot(body->m_chBody->GetRot());
    m_chBody->SetRot_dt(body->m_chBody->GetRot_dt());
    m_chBody->SetMass(body->m_chBody->GetMass());
    m_chBody->SetDensity(body->GetDensity());
    m_Density = body->GetDensity();
    m_chBody->SetInertia(body->m_chBody->GetInertia());

    m_TriangulatedModel = body->m_TriangulatedModel->GetCopy();

    std::cout << " end copy obj" << std::endl;
}

