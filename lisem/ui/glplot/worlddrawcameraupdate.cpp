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


    m_CameraLimitMutex.lock();

    if(m_CameraLock2D && !m_Draw3D)
    {
        BoundingBox bb = m_Camera2D->GetBoundingBox();
        //check if size is smaller then required scale

        double size_min = std::min(bb.GetSizeX(),bb.GetSizeY());

        double req_min = m_Camera2DLockScale * std::min(m_Camera2DLock.GetSizeX(),m_Camera2DLock.GetSizeY());

        if(size_min < req_min)
        {
            bb.Scale(req_min/std::max(1e-12,size_min));
        }

        if(bb.GetSizeX() > m_Camera2DLock.GetSizeX())
        {
            bb.Scale(m_Camera2DLock.GetSizeX()/std::max(1e-12,bb.GetSizeX()));
        }

        if(bb.GetSizeY() > m_Camera2DLock.GetSizeY())
        {
            bb.Scale(m_Camera2DLock.GetSizeY()/std::max(1e-12,bb.GetSizeY()));
        }


        //if it is outside the bounds, move it in

        double move_x1 = std::max(0.0,m_Camera2DLock.GetMinX() - bb.GetMinX());
        double move_x2 = std::min(0.0,m_Camera2DLock.GetMaxX() - bb.GetMaxX());
        double move_y1 = std::max(0.0,m_Camera2DLock.GetMinY() - bb.GetMinY());
        double move_y2 = std::min(0.0,m_Camera2DLock.GetMaxY() - bb.GetMaxY());

        bb.Move(LSMVector2(move_x1 + move_x2,move_y1 + move_y2));

        m_Camera2D->LookAt(bb);
    }

    if(m_CameraLock3D && m_Draw3D)
    {
        LSMVector3 pos = m_Camera3D->GetPosition();
        pos.x = std::max(m_Camera3DLock.GetMinX(),std::min(m_Camera3DLock.GetMaxX(),(double)pos.x));
        pos.x = std::max(m_Camera3DLock.GetMinY(),std::min(m_Camera3DLock.GetMaxY(),(double)pos.y));
        pos.x = std::max(m_Camera3DLock.GetMinZ(),std::min(m_Camera3DLock.GetMaxZ(),(double)pos.z));
        m_Camera3D->SetPosition(pos.x,pos.y,pos.z);
    }


    if(m_CameraFixed2D&& !m_Draw3D)
    {
        m_Camera2D->LookAt(m_Camera2DLock);
    }


    if(m_CameraFixed3D && m_Draw3D)
    {
        m_Camera3D->SetPosition(m_Camera3DFixPos.x,m_Camera3DFixPos.y,m_Camera3DFixPos.z);
        m_Camera3D->SetViewDir(m_Camera3DFixView.x,m_Camera3DFixView.y,m_Camera3DFixView.z);
    }
    m_DoSet3DViewFrom2DOnce = false;

    m_Camera3D->SetCurrentMatrix();


    m_CameraLimitMutex.unlock();

    return false;

}

/*QMutex m_CameraLimitMutex;
bool m_CameraLock2D = false;
bool m_CameraLock3D = false;
bool m_CameraFixed2D = false;
bool m_CameraFixed3D = false;
int m_CameraLastLimit = -1;
BoundingBox m_Camera2DLock;
double m_Camera2DLockScale = 1.0;
BoundingBox3D m_Camera3DLock;
LSMVector3 m_Camera3DFixPos;
LSMVector3 m_Camera3DFixView;
BoundingBox m_Camera2DFixView;*/


void WorldWindow::SetFixedCamera2D(BoundingBox view)
{
    m_CameraLimitMutex.lock();

    m_CameraFixed2D = true;
    m_CameraLastLimit = 0;
    m_Camera2DFixView = view;
    m_CameraLimitMutex.unlock();

}
void WorldWindow::SetFixedCamera3D(LSMVector3 position, LSMVector3 view)
{
    m_CameraLimitMutex.lock();

    m_CameraFixed3D = true;
    m_CameraLastLimit = 1;
    m_Camera3DFixView = view;
    m_Camera3DFixPos = position;
    m_CameraLimitMutex.unlock();


}
void WorldWindow::SetLimitedPosition2D(BoundingBox view, double minscale )
{
    m_CameraLimitMutex.lock();

    m_CameraLock2D = true;
    m_CameraLastLimit = 2;
    m_Camera2DLockScale = minscale;
    m_Camera2DLock = view;
    m_CameraLimitMutex.unlock();

}
void WorldWindow::SetLimitedPosition3D(BoundingBox3D view)
{
    m_CameraLimitMutex.lock();

    m_CameraLock3D = true;
    m_CameraLastLimit = 3;
    m_Camera3DLock = view;
    m_CameraLimitMutex.unlock();

}
void WorldWindow::SetFreeCamera(bool x)
{
    m_CameraLimitMutex.lock();
    if(!x)
    {
        if(m_CameraLastLimit == 0)
        {
            m_CameraFixed2D = true;
        }
        if(m_CameraLastLimit == 1)
        {
            m_CameraFixed3D = true;
        }
        if(m_CameraLastLimit == 2)
        {
            m_CameraLock2D = true;
        }
        if(m_CameraLastLimit == 3)
        {
            m_CameraLock3D = true;
        }


    }else
    {
        m_CameraLock2D = false;
        m_CameraLock3D = false;
        m_CameraFixed2D = false;
        m_CameraFixed3D = false;
    }
    m_CameraLimitMutex.unlock();

}
