#include "worldwindow.h"

bool WorldWindow::UpdateCamera()
{


    bool shift = false;
    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {
        shift = true;
    }

    //call all input based functions for the 2d and 3d camera controller
    float scw = std::max(1.0f,((float)(m_OpenGLCLManager->GL_GLOBAL.Width)));
    float sch = std::max(1.0f,(float)( m_OpenGLCLManager->GL_GLOBAL.Height));

    m_Camera2D->SetScreenSize(scw,sch);
    m_Camera3D->ResizeViewPort(scw,sch);
    m_Camera2DController->OnFrame(m_GLDT,m_OpenGLCLManager->window);
    m_Camera3DController->OnFrame(m_GLDT,m_OpenGLCLManager->window);

    if(std::abs(m_MouseState.Move_X) > 0 || std::abs(m_MouseState.Move_Y) > 0)
    {
        m_Camera2DController->OnMouseMove(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
        m_Camera3DController->OnMouseMove(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
    }
    if(std::abs(m_MouseState.MoveScroll_X) > 0 || std::abs(m_MouseState.MoveScroll_Y) > 0)
    {
        if(!shift)
        {
            m_Camera2DController->OnMouseScroll(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
            m_Camera3DController->OnMouseScroll(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
        }
    }

    float dragx = m_MouseState.Drag_x;
    float dragy = m_MouseState.Drag_y;

    if(dragx != 0.0f || dragy != 0.0f)
    {
        if(!shift)
        {
           m_Camera2DController->OnMouseDrag(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(dragx,dragy));
            m_Camera3DController->OnMouseDrag(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(dragx,dragy));
        }
    }

    for(int i = 0; i < m_MouseState.MouseButtonEvents.length(); i++)
    {
        if(!(m_MouseState.MouseButtonEventsShift.at(i)))
        {
            m_Camera2DController->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));
            m_Camera3DController->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));
        }
    }
    for(int i = 0; i < m_MouseState.KeyEvents.length(); i++)
    {
        if(!(m_MouseState.KeyEventShift.at(i)))
        {
            m_Camera2DController->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i));
            m_Camera3DController->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i));
        }
    }

    if(m_Draw3D)
    {
        if(m_DoSet3DViewFrom2DOnce)
        {
            m_DoSet3DViewFrom2DOnce = false;
            m_ElevationProvider->SetAllZero();
            m_Camera3D->PlaceAndLookAtAuto(m_Camera2D->GetBoundingBox());

        }else
        {
            m_Camera2D->LookAt(m_Camera3D->Get2DViewEquivalent(m_ElevationProvider));
        }

    }else {
        m_ElevationProvider->SetAllZero();
        m_Camera3D->PlaceAndLookAtAuto(m_Camera2D->GetBoundingBox());
    }


    m_DoSet3DViewFrom2DOnce = false;

    m_Camera3D->SetCurrentMatrix();

    return false;

}
