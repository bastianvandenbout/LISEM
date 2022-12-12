
#include "layer/geo/uigeolayer.h"
#include "rigidphysics/rigidworld.h"
#include "rigidphysics/rigidobject.h"
#include "openglgeometry.h"
#include "models/3dmodel.h"
#include "geometry/triangleintersect.h"
#include "opengl3dobject.h"
#include "gl/openglcldatamanager.h"
class UIRigidObjectVisualizer
{

public:

    RigidPhysicsObject * m_RigidObject = nullptr;
    ModelGeometry * m_Model = nullptr;
    gl3dObject * m_Actor = nullptr;

    QString m_Family;
    bool m_ShouldBeRemoved = false;

    LSMVector3 m_Position = LSMVector3(0.0,0.0,0.0);
    LSMVector3 m_Scale = LSMVector3(1.0,1.0,1.0);
    LSMMatrix4x4 m_RotationMatrix; //defeault is identity matrix

    inline UIRigidObjectVisualizer(RigidPhysicsObject * o)
    {
        m_RigidObject = o;
        QString fam = o->GetFamily();
        if(!fam.isEmpty())
        {
            m_Family = fam;
            m_Scale = m_RigidObject->GetFamilyScale();

        }

        m_Model = o->GetModel();
    }


};
