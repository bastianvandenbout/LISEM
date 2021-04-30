#ifndef GL2DCAMERACONTROLLER_H
#define GL2DCAMERACONTROLLER_H

#include "linear/lsm_vector2.h"
#include "gl2dcamera.h"
#include "GLFW/glfw3.h"
#include "math.h"
#include <algorithm>

class GL2DCameraController
{

private:

    GL2DCamera * m_Camera;


public:
    inline GL2DCameraController(GL2DCamera * c)
    {

        m_Camera = c;

    }

    inline void OnFrame(float dt, GLFWwindow * w)
    {



    }


    inline void OnMouseDrag(LSMVector2 pos, LSMVector2 mov)
    {
        float dragx = mov.X();
        float dragy = mov.Y();

        //CRS to pixel conversion value
        float CRStoPixX = m_Camera->GetSize().X()/(m_Camera->GetScreenSize().X());
        float CRStoPixY = m_Camera->GetSize().Y()/(m_Camera->GetScreenSize().Y());

        //then based on the CRS width and height and pixel width and heigt
        //estimate CRS drag

        float CRSDragX = dragx *CRStoPixX;
        float CRSDragY = dragy *CRStoPixY;

        m_Camera->Move(CRSDragX,CRSDragY);
    }

    inline void OnMouseMove(LSMVector2 pos, LSMVector2 mov)
    {

    }

    inline void OnMouseScroll(LSMVector2 pos, LSMVector2 mov)
    {
        m_Camera->Resize(std::min(1000000000000.0,std::max(0.00000001, std::pow(0.95,mov.Y()))));
    }

    inline void OnMousePressed(int button, int pressed)
    {

    }

    inline void OnMouseLeave(bool leave)
    {

    }

    inline void OnKeyPressed(int key, bool pressed)
    {

    }
};


#endif // GL2DCAMERACONTROLLER_H
