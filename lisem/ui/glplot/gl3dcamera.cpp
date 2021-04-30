#include "glplot/gl3dcamera.h"
#include "lisemmath.h"
#include "iostream"
#include "defines.h"

void GL3DCamera::ResizeViewPort(int w, int h)
{
    // setup viewport, projection etc.:


    aspect = float(w) / float(h ? h : 1);

    m_viewportSize = LSMVector2(float(w),float(h));

    m_Projection.SetIdentity();
    m_Projection.SetPerspective(m_FoV,aspect,m_ZNear,m_ZFar);
}

void GL3DCamera::SetCurrentMatrix()
{



    m_Projection.SetIdentity();
    m_Projection.SetPerspective(m_FoV,aspect,m_ZNear,m_ZFar);
    m_ProjectionNormalZ.SetIdentity();
    m_ProjectionNormalZ.SetPerspective(m_FoV,aspect,0.1,1000.0);

    LSMMatrix4x4 matrix1;
    LSMMatrix4x4 matrix2;
    matrix1.SetIdentity();
    matrix2.SetIdentity();
    LSMMatrix4x4 matrix3;
    matrix3.SetIdentity();

     this->m_CLookAtNoTranslationNormalZ.SetIdentity();
    this->m_CLookAtNoTranslation.SetIdentity();
    this->m_CameraMatrix.SetIdentity();
    this->m_CLookAtNoTranslationXZ.SetIdentity();

    m_CameraMatrix = m_Projection;
    m_CLookAtNoTranslation = m_Projection;
    m_CLookAtNoTranslationXZ = m_Projection;
    m_CLookAtNoTranslationNormalZ = m_ProjectionNormalZ;
    matrix2.SetLookAt(m_Position,m_Position+m_ViewDir,LSMVector3(0.0,1.0,0.0));
    matrix1.SetLookAt(LSMVector3(0,0,0),m_ViewDir,LSMVector3(0.0,1.0,0.0));
    matrix3.SetLookAt(LSMVector3(0,m_Position.y,0),LSMVector3(0,m_Position.y,0)+m_ViewDir,LSMVector3(0.0,1.0,0.0));

    m_CLookAtNoTranslationNormalZ = m_CLookAtNoTranslationNormalZ*matrix1;
    m_CLookAtNoTranslation = m_CLookAtNoTranslation*matrix1;
    m_CameraMatrix = m_CameraMatrix*matrix2;
    m_CLookAtNoTranslationXZ = m_CLookAtNoTranslationXZ*matrix3;

}

void GL3DCamera::SetRotation(double a, double b, double c)
{

}

void GL3DCamera::SetPosition(double a, double b, double c)
{
    this->m_Position.setX(a);
    this->m_Position.setY(b);
    this->m_Position.setZ(c);

}

void GL3DCamera::RotateX(double Angle)
{

    RotatedX += Angle;

    //Rotate viewdir around the right vector:
    m_ViewDir = LSMVector3(m_ViewDir*cos(Angle*LISEM_PIdiv180) + m_Up*sin(Angle*LISEM_PIdiv180)).Normalize();
    m_ViewDir = m_ViewDir.Normalize();
    //now compute the new UpVector (by cross product)
    m_Up = LSMVector3(0.0,1.0,0.0);//LSMVector3::CrossProduct(m_ViewDir, m_Right)*-1;

    //m_Right = LSMVector3::CrossProduct(m_ViewDir, m_Up);
    //m_Right.setY(0.0);
    //m_Right = m_Right.Normalize();
}

void GL3DCamera::RotateAroundRight(double Angle)
{

    RotatedX += Angle;

    //Rotate viewdir around the right vector:
    m_ViewDir = LSMVector3(m_ViewDir*cos(Angle*LISEM_PIdiv180) + m_Up*sin(Angle*LISEM_PIdiv180)).Normalize();

    //now compute the new UpVector (by cross product)
    m_Up = LSMVector3(0,1,0);//LSMVector3::CrossProduct(m_ViewDir, m_Right)*-1;

}


void GL3DCamera::RotateY(double Angle)
{
    RotatedY += Angle;

    //Rotate viewdir around the up vector:
    m_ViewDir =  LSMVector3(m_ViewDir*cos(Angle*LISEM_PIdiv180) - m_Right*sin(Angle*LISEM_PIdiv180)).Normalize();

    m_ViewDir = m_ViewDir.Normalize();
    //now compute the new RightVector (by cross product)
    m_Right = LSMVector3::CrossProduct(m_ViewDir, m_Up);
    m_Right.setY(0.0);
    m_Right = m_Right.Normalize();

}

void GL3DCamera::RotateZ(double Angle)
{
    RotatedZ += Angle;

    //Rotate viewdir around the right vector:
    m_Right =  LSMVector3(m_Right*cos(Angle*LISEM_PIdiv180) + m_Up*sin(Angle*LISEM_PIdiv180)).Normalize();

    //now compute the new UpVector (by cross product)
    m_Up = LSMVector3(0,1,0);//LSMVector3::CrossProduct(m_ViewDir, m_Right)*-1;
}

void GL3DCamera::Move(double a, double b, double c)
{
    this->m_Position.setX(m_Position.X() +a);
    this->m_Position.setY(m_Position.Y() +b);
    this->m_Position.setZ(m_Position.Z() +c);

}

void GL3DCamera::MoveForward(double Distance )
{
    m_Position = m_Position + (m_ViewDir*(-Distance));
}
void GL3DCamera::StrafeRight ( double Distance )
{
    m_Position = m_Position +(m_Right*Distance);
}

void GL3DCamera::MoveUpward( double Distance )
{
    m_Position = m_Position +(m_Up*Distance);
}

void GL3DCamera::LookAt(float x, float y, float z)
{
    LSMVector3 look= LSMVector3(x,y,z);
    m_ViewDir = (look-m_Position);
    m_ViewDir = m_ViewDir.Normalize();

}

void GL3DCamera::SetViewDir(float x, float y, float z)
{
    m_ViewDir = LSMVector3(x,y,z).Normalize();

}

void GL3DCamera::Zoom( double dangle )
{
    this->m_FoV += dangle;
}

void GL3DCamera::PlaceAndLookAtAuto(BoundingBox3D b)
{
    m_Position = LSMVector3(b.GetCenterX(), b.GetCenterZ() + b.GetSizeX(),b.GetCenterY());
    m_ViewDir = LSMVector3(0,-0.8,0.2).Normalize();
}


void GL3DCamera::PlaceAndLookAtAuto(BoundingBox b)
{
    m_Position = LSMVector3(b.GetCenterX(), b.GetSizeX(),b.GetCenterY());
    m_ViewDir = LSMVector3(0,-0.8,0.2).Normalize();
}
