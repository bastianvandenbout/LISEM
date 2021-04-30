#ifndef GL3DCAMERA_H
#define GL3DCAMERA_H

#include "linear/lsm_vector.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_matrix4x4.h"
#include "boundingbox.h"
#include "gl3delevationprovider.h"

class GL3DCamera
{
private:

    double m_ZNear;
    double m_ZFar;
    double m_FoV;


    LSMMatrix4x4 m_Projection;
    LSMMatrix4x4 m_ProjectionNormalZ;
    LSMMatrix4x4 m_CLookAtNoTranslationNormalZ;
    LSMMatrix4x4 m_CLookAtNoTranslation;
    LSMMatrix4x4 m_CLookAtNoTranslationXZ;
    LSMMatrix4x4 m_CLookAtNoTranslationInverse;
    LSMMatrix4x4 m_CameraMatrix;

    LSMVector3 m_Position;
    LSMVector3 m_ViewDir;
    LSMVector3 m_Up;
    LSMVector3 m_Right;

    double RotatedX;
    double RotatedY;
    double RotatedZ;
    double RotatedRight;

    double aspect;

    LSMVector2 m_viewportSize;


public:
    inline GL3DCamera()
    {
        m_ZNear = 0.1f;
        m_ZFar = 100000.0f;

        m_FoV = 75;

        m_Projection.SetIdentity();
        m_ProjectionNormalZ.SetIdentity();
        m_CLookAtNoTranslationNormalZ.SetIdentity();
        m_CLookAtNoTranslation.SetIdentity();
        m_CLookAtNoTranslationInverse.SetIdentity();
        m_CameraMatrix.SetIdentity();

        m_Position.setX(0);
        m_Position.setY(0);
        m_Position.setZ(0);
        m_ViewDir.setX(0);
        m_ViewDir.setY(0);
        m_ViewDir.setZ(1);
        m_Up.setX(0);
        m_Up.setY(1);
        m_Up.setZ(0);
        m_Right.setX(1);
        m_Right.setY(0);
        m_Right.setZ(0);

        RotatedX = 0;
        RotatedY = 0;
        RotatedZ = 0;
        RotatedRight = 0;
    }

    inline ~GL3DCamera()
    {

    }

    inline void SetZNear(float x)
    {
        m_ZNear = x;
    }
    inline void SetZFar(float x)
    {
        m_ZFar = x;
    }


    void ResizeViewPort(int w, int h);

    void SetCurrentMatrix();

    void SetRotation(double a, double b, double c);
    void SetPosition(double a, double b, double c);
    void Rotate(double a, double b, double c);
    void Move(double a, double b, double c);
    void RotateX(double Angle);
    void RotateAroundRight(double Angle);
    void RotateY(double Angle);
    void RotateZ(double Angle);
    void MoveForward(double Distance );
    void StrafeRight ( double Distance );
    void MoveUpward( double Distance );

    void SetViewDir(float x, float y, float z);
    void LookAt(float x, float y, float z);
    void PlaceAndLookAtAuto(BoundingBox b);
    void PlaceAndLookAtAuto(BoundingBox3D b);
    void Zoom(double dAngle);

    inline LSMVector3 GetPosition()
    {
        return m_Position;
    }

    inline LSMVector3 GetViewDir()
    {
        return m_ViewDir;
    }
    inline LSMVector3 GetUpDir()
    {
        return m_Up;
    }
    inline LSMVector3 GetRightDir()
    {
        return m_Right;
    }

    inline LSMMatrix4x4 GetProjectionMatrixNoTranslationXZ()
    {
        return m_CLookAtNoTranslationXZ;
    }

    inline LSMMatrix4x4 GetProjectionMatrix()
    {
        return m_CameraMatrix;
    }

    inline LSMMatrix4x4 GetProjectionMatrixNoTranslation()
    {
        return m_CLookAtNoTranslation;
    }
    inline LSMMatrix4x4 GetProjectionMatrixNoTranslationNormalZ()
    {
        return m_CLookAtNoTranslationNormalZ;
    }

    inline BoundingBox Get2DViewEquivalent(GL3DElevationProvider * p)
    {

        float cx = GetPosition().x;
        float cy = GetPosition().z;

        float elevation = p->GetElevationAtCenter(cx,cy);
        if(elevation < -1e25 || !std::isfinite(elevation))
        {
            elevation  = 0.0;
        }
        float sizex = std::max(elevation + 0.01f,(float)GetPosition().y)- elevation;
        float sizey = std::max(elevation + 0.01f,(float)GetPosition().y) -elevation;

        BoundingBox a;
        a.Set(cx - 0.5 * sizex,cx + 0.5 * sizex,cy - 0.5 * sizey, cy + 0.5 * sizey);

        return a;
    }


    inline LSMVector4 GetRayFromWindow(float rx, float ry)
    {

        LSMVector2 normalizedCoords = GetNormalizedDeviceCoordinates(rx,ry);
        LSMVector4 clipCoords = LSMVector4(normalizedCoords.x,normalizedCoords.y,-1.0f,1.0f);
        LSMVector4 eyeCoords = GetEyeCoords(clipCoords);
        return eyeCoords.normalized();
    }

    inline LSMVector4 GetEyeCoords(LSMVector4 clipcoords)
    {
        LSMMatrix4x4 invproj = m_CLookAtNoTranslation;
        invproj.Invert();
        return invproj*clipcoords;
    }

    inline LSMVector2 GetNormalizedDeviceCoordinates(float mouse_x, float mouse_y)
    {
        return LSMVector2(2.0f * mouse_x/m_viewportSize.x - 1.0f,2.0f * (m_viewportSize.y-mouse_y)/m_viewportSize.y - 1.0f);
    }

};

#endif // GL3DCAMERA_H
