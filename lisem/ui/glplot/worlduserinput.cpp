#include "worldwindow.h"
#include "QClipboard"
void WorldWindow::OnMouseScroll(double scrollx ,double scrolly)
{

    MouseStateMutex.lock();

    if(m_MouseState.movescroll_first)
    {
        m_MouseState.ScrollOld_x = scrollx;
        m_MouseState.ScrollOld_y = scrolly;
        m_MouseState.movescroll_first = false;
    }

    m_MouseState.Scroll_x = scrollx;
    m_MouseState.Scroll_y = scrolly;

    m_MouseState.scroll += scrolly;

    m_MouseState.MoveScroll_X += scrollx;
    m_MouseState.MoveScroll_Y += scrolly;

    MouseStateMutex.unlock();

}
void WorldWindow::OnMouseMove(double posx,double posy)
{

    bool shift = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS;

    if(!GetShiftAllowd())
    {
        shift= false;
    }

    MouseStateMutex.lock();
    if(m_MouseState.move_first == true)
    {
        m_MouseState.PosDO_x = posx;
        m_MouseState.PosDO_y = posy;
        m_MouseState.PosDOO_x = posx;
        m_MouseState.PosDOO_y = posy;
        m_MouseState.move_first = false;
    }

    m_MouseState.Pos_x = posx;
    m_MouseState.Pos_y = posy;

    double dx = m_MouseState.Pos_x - m_MouseState.PosDO_x;
    double dy = m_MouseState.Pos_y - m_MouseState.PosDO_y;

    m_MouseState.Move_X -=dx;
    m_MouseState.Move_Y +=dy;

    if(m_MouseState.dragging)
    {

        m_MouseState.Drag_x -=dx;
        m_MouseState.Drag_y +=dy;
        m_MouseState.PosDO_x = m_MouseState.Pos_x;
        m_MouseState.PosDO_y = m_MouseState.Pos_y;
    }else {

        m_MouseState.PosDOO_x = m_MouseState.Pos_x;
        m_MouseState.PosDOO_y = m_MouseState.Pos_y;
    }

    if(m_SunDrag)
    {

        m_SunDir = m_Camera3D->GetRayFromWindow(m_MouseState.Pos_x,m_MouseState.Pos_y).xyz();
        m_SunDir.y = std::max(0.0f,m_SunDir.y);
        m_SunDir = m_SunDir.Normalize();
    }

    float drag_x = m_MouseState.Pos_x - m_MouseState.PosDOO_x;
    float drag_y = m_MouseState.Pos_y - m_MouseState.PosDOO_y;
    float drag_s = std::sqrt(drag_x*drag_x + drag_y*drag_y);


    if(drag_s > 4 && m_DraggingFocusSquare == false && shift && m_LayerEditor == nullptr)
    {
        //add focus area
        m_FocusMutex.lock();
        m_FocusSquare.clear();
        m_DraggingFocusSquare = true;
        m_FocusSquareStart = LSMVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ);
        m_FocusSquare.append(BoundingBox(m_FocusSquareStart.x,m_CurrentWindowState.MouseGeoPosX,m_FocusSquareStart.y,m_CurrentWindowState.MouseGeoPosZ));
        m_FocusMutex.unlock();

        emit OnFocusLocationChanged();

    }else if(m_DraggingFocusSquare && m_LayerEditor == nullptr)
    {
        if(!shift)
        {
            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;

            m_FocusMutex.unlock();

            emit OnFocusLocationChanged();

        }else if(m_FocusSquare.length() > 0 && m_LayerEditor == nullptr)
        {
            BoundingBox b = m_FocusSquare.last();
            //b.MergeWith(LSMVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ));
            b = BoundingBox(m_FocusSquareStart.x,m_CurrentWindowState.MouseGeoPosX,m_FocusSquareStart.y,m_CurrentWindowState.MouseGeoPosZ);
            m_FocusSquare.replace(m_FocusSquare.length()-1,b);
        }

    }
    MouseStateMutex.unlock();

}
void WorldWindow::OnMouseInOut(bool enter)
{

    MouseStateMutex.lock();

    m_MouseState.in = enter;

    MouseStateMutex.unlock();
}
void WorldWindow::OnMouseKey( int key, int action, int mods)
{

    MouseStateMutex.lock();

    m_SM.lock();
    bool shift = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS;
    bool c = glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_C) ==GLFW_PRESS;

    if(!GetShiftAllowd())
    {
        shift = false;
    }
    if(shift)
    {
        m_MouseState.MouseButtonEventsShift.append(true);
        m_MouseState.MouseButtonSEventsShift.append(true);

    }else {
        m_MouseState.MouseButtonEventsShift.append(false);
        m_MouseState.MouseButtonSEventsShift.append(false);
    }


    m_MouseState.MouseButtonSPosX.append(m_MouseState.Pos_x/m_CurrentWindowState.scr_width);
    m_MouseState.MouseButtonSPosY.append(m_MouseState.Pos_y/m_CurrentWindowState.scr_height);

    m_MouseState.MouseButtonSEvents.append(key);
    m_MouseState.MouseButtonKeySAction.append(action);

    m_SM.unlock();

    m_MouseState.MouseButtonEvents.append(key);
    m_MouseState.MouseButtonKeyAction.append(action);

    if (key == GLFW_MOUSE_BUTTON_1  && action == GLFW_PRESS) {
        m_MouseState.dragging = true;
        m_MouseState.move_first = true;

        m_MouseState.Button_Left_Pressed = true;
        if(shift)
        {
            LSMVector4 ray = this->m_Camera3D->GetRayFromWindow(m_MouseState.Pos_x,m_MouseState.Pos_y);
            if(m_SunDir.Angle(ray.xyz()) < 0.05)
            {
                m_SunDrag = true;
            }
        }

    }else if(key == GLFW_MOUSE_BUTTON_1  && action == GLFW_RELEASE) {
        m_MouseState.dragging = false;
        m_SunDrag = false;

        m_MouseState.Button_Left_Pressed = false;
        if(shift && m_LayerEditor == nullptr)
        {
            float drag_x = m_MouseState.Pos_x - m_MouseState.PosDOO_x;
            float drag_y = m_MouseState.Pos_y - m_MouseState.PosDOO_y;
            float drag_s = std::sqrt(drag_x*drag_x + drag_y*drag_y);

            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();

            if(drag_s < 4)
            {

                //add focus point
                m_FocusMutex.lock();
                m_FocusLocations.append(LSMVector2(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosZ));
                m_FocusMutex.unlock();

                if(c)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    clipboard->setText("Point(" + QString::number(m_CurrentWindowState.MouseGeoPosX) + ","+QString::number(m_CurrentWindowState.MouseGeoPosZ)+ ")");
                }

            }else {


                if(c)
                {
                    QClipboard *clipboard = QApplication::clipboard();
                    if(m_FocusSquare.length() > 0)
                    {
                        BoundingBox b = m_FocusSquare.last();
                        clipboard->setText("Region(" + QString::number(b.GetMinX()) + ","+QString::number(b.GetMaxX())+","+QString::number(b.GetMinY()) + ","+QString::number(b.GetMaxY())+ ")");
                    }

                }
            }

            emit OnFocusLocationChanged();
        }else
        {

            m_FocusMutex.lock();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();
        }
    }
    if (key == GLFW_MOUSE_BUTTON_3  && action == GLFW_PRESS) {


        m_MouseState.Button_Middle_Pressed = true;

    }else if (key == GLFW_MOUSE_BUTTON_3  && action == GLFW_RELEASE) {


        m_MouseState.Button_Middle_Pressed = false;
    }

    if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_PRESS) {


        m_MouseState.Button_Right_Pressed = true;

    }else if (key == GLFW_MOUSE_BUTTON_2  && action == GLFW_RELEASE) {


        m_MouseState.Button_Right_Pressed = false;

        if(shift)
        {
            m_FocusMutex.lock();
            m_FocusSquare.clear();
            m_FocusLocations.clear();
            m_DraggingFocusSquare = false;
            m_FocusMutex.unlock();

            emit OnFocusLocationChanged();
        }
    }

    MouseStateMutex.unlock();
}

void WorldWindow::OnFileDrop(QString f)
{
    std::cout << "fopen1 request " << f.toStdString() << std::endl;

    emit int_emit_fildropcallback(f);
}

void WorldWindow::OnFrameBufferSize(int w,int h)
{

    MouseStateMutex.lock();

    MouseStateMutex.unlock();
}
void WorldWindow::OnKey(int key, int action, int mods)
{
    MouseStateMutex.lock();

    if(!GetShiftAllowd())
    {
       if( mods == GLFW_MOD_SHIFT)
       {
            mods = 0x0000;
       }
    }


    m_SM.lock();

    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {

        m_MouseState.KeyEventShift.append(true);
    }else {

        m_MouseState.KeyEventShift.append(false);
    }


    m_MouseState.KeyEvents.append(key);
    m_MouseState.KeyAction.append(action);
    m_MouseState.KeyMods.append(mods);

    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {

        m_MouseState.KeySEventShift.append(true);
    }else {

        m_MouseState.KeySEventShift.append(false);
    }


    m_MouseState.KeySEvents.append(key);
    m_MouseState.KeySAction.append(action);
    m_MouseState.KeySMods.append(mods);


    m_SM.unlock();

    if( key == GLFW_KEY_P && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS)
    {
        m_OpenGLCLManager->m_DoFinalRender = !m_OpenGLCLManager->m_DoFinalRender;
    }
    if (key == GLFW_KEY_T && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS) {

        std::cout << "set draw arrows " << m_DrawArrows << std::endl;
        if(m_DrawArrows && m_GizmoMode != GIZMO_MOVE)
        {
            m_DrawArrows = true;
        }else
        {
            m_DrawArrows = !m_DrawArrows;
        }
        m_GizmoMode = GIZMO_MOVE;

    }
    if (key == GLFW_KEY_R && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS) {

        if(m_DrawArrows && m_GizmoMode != GIZMO_ROTATE)
        {
            m_DrawArrows = true;

        }else
        {
            m_DrawArrows = !m_DrawArrows;
        }
        std::cout << "set draw arrows " << m_DrawArrows << std::endl;

        m_GizmoMode = GIZMO_ROTATE;

    }
    if (key == GLFW_KEY_S && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS) {

        std::cout << "set draw arrows " << m_DrawArrows << std::endl;
        if(m_DrawArrows && m_GizmoMode != GIZMO_SCALE)
        {
            m_DrawArrows = true;

        }else
        {
            m_DrawArrows = !m_DrawArrows;
        }

        m_GizmoMode = GIZMO_SCALE;

    }


    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {



    }

    MouseStateMutex.unlock();
}

void WorldWindow::InputToLayers()
{

    //first check if we have to add or remove some focus point/area

    //if there is no editor


    //if there has been a mouse button release with left shift press

    //if there was a significant move, make it a square

    //otherwise make it a point

    //if there has been a right mouse button release with left shift press


    //input to layers

    bool shift = false;
    if(glfwGetKey(m_OpenGLCLManager->window,GLFW_KEY_LEFT_SHIFT) ==GLFW_PRESS)
    {
        shift = true;
    }

    //first get geo location
    bool GeoHit = m_CurrentWindowState.MouseHit;
    LSMVector3 GeoLoc = LSMVector3(m_CurrentWindowState.MouseGeoPosX,m_CurrentWindowState.MouseGeoPosY,m_CurrentWindowState.MouseGeoPosZ);


    for(int j = 0; j < m_UILayerList.length(); j++)
    {
        UILayer * l = m_UILayerList.at(j);
        l->OnFrame(m_GLDT,m_OpenGLCLManager->window);
        if(std::abs(m_MouseState.Move_X) > 0 || std::abs(m_MouseState.Move_Y) > 0)
        {
            l->OnMouseMove(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
            if(GeoHit)
            {
                l->OnGeoMouseMove(m_CurrentWindowState,GeoLoc,GeoLoc-m_Prev_GeoLoc,m_MouseState.dragging);
            }
        }
        if(std::abs(m_MouseState.MoveScroll_X) > 0 || std::abs(m_MouseState.MoveScroll_Y) > 0)
        {
            if(shift)
            {
                l->OnMouseScroll(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
            }
        }
        float dragx = m_MouseState.Drag_x;
        float dragy = m_MouseState.Drag_y;

        if(dragx != 0.0f || dragy != 0.0f)
        {

            if(shift)
            {
                l->OnMouseDrag(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(dragx,dragy));
            }
        }
        for(int i = 0; i < m_MouseState.MouseButtonEvents.length(); i++)
        {
            if((m_MouseState.MouseButtonEventsShift.at(i)))
            {
                l->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));
                if(GeoHit)
                {
                    l->OnGeoMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i),m_CurrentWindowState,GeoLoc);
                }
            }
        }
       for(int i = 0; i < m_MouseState.KeyEvents.length(); i++)
        {
            //if((m_MouseState.KeyEventShift.at(i)))
            {

                l->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i),m_MouseState.KeyMods.at(i));
            }
        }
    }

    //input to editor and current tool

    UILayerEditor * l = m_LayerEditor;
    if(l != nullptr)
    {
        l->OnFrame(m_GLDT,m_OpenGLCLManager->window);
        if(std::abs(m_MouseState.Move_X) > 0 || std::abs(m_MouseState.Move_Y) > 0)
        {
            l->OnMouseMove(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(m_MouseState.Move_X,m_MouseState.Move_Y));
            if(GeoHit)
            {
                l->OnGeoMouseMove(m_CurrentWindowState,GeoLoc,GeoLoc-m_Prev_GeoLoc,m_MouseState.dragging);
            }
        }
        if(std::abs(m_MouseState.MoveScroll_X) > 0 || std::abs(m_MouseState.MoveScroll_Y) > 0)
        {
            if(shift)
            {
                l->OnMouseScroll(LSMVector2(m_MouseState.Scroll_x,m_MouseState.Scroll_y),LSMVector2(m_MouseState.MoveScroll_X,m_MouseState.MoveScroll_Y));
            }
        }
        float dragx = m_MouseState.Drag_x;
        float dragy = m_MouseState.Drag_y;

        if(dragx != 0.0f || dragy != 0.0f)
        {
            if(shift)
            {

                l->OnMouseDrag(LSMVector2(m_MouseState.Pos_x,m_MouseState.Pos_y),LSMVector2(dragx,dragy));
            }
        }
        for(int i = 0; i < m_MouseState.MouseButtonEvents.length(); i++)
        {
            if((m_MouseState.MouseButtonEventsShift.at(i)))
            {
                l->OnMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i));

                if(GeoHit)
                {
                    l->OnGeoMousePressed(m_MouseState.MouseButtonEvents.at(i),m_MouseState.MouseButtonKeyAction.at(i),m_CurrentWindowState,GeoLoc);
                }
            }
        }
        for(int i = 0; i < m_MouseState.KeyEvents.length(); i++)
        {
            if((m_MouseState.KeyEventShift.at(i)))
            {
                l->OnKeyPressed(m_MouseState.KeyEvents.at(i),m_MouseState.KeyAction.at(i),m_MouseState.KeyMods.at(i));
            }
        }
    }


    //now do any drag element detection



    for(int j = 0;j < m_MouseState.MouseButtonEvents.length(); j++)
    {
        for(int i = m_DragElements.size()-1; i > -1; i--)
        {
            DragElement e = m_DragElements.at(i);

            if(shift)
            {
                if(m_MouseState.MouseButtonEvents.at(j) == GLFW_MOUSE_BUTTON_1 && m_MouseState.MouseButtonKeyAction.at(j) == GLFW_PRESS)
                {
                    std::cout << "check collide " << std::endl;
                    if(e.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y))
                    {
                        std::cout << "selected drag element "<< std::endl;
                        m_SelectedDragElement = e;
                        m_SelectedDragElement.exists = true;
                        break;
                    }
                }

            }

        }
        if(m_MouseState.MouseButtonEvents.at(j) == GLFW_MOUSE_BUTTON_1 && m_MouseState.MouseButtonKeyAction.at(j) == GLFW_RELEASE)
        {
            m_SelectedDragElement.Layer = nullptr;
            m_SelectedDragElement.exists = false;
        }
    }

    if(m_SelectedDragElement.exists && shift)
    {
        //apply drag movement to drag element

        float dragx = m_MouseState.Drag_x;
        float dragy = m_MouseState.Drag_y;

        if(dragx != 0.0f || dragy != 0.0f)
        {
            std::cout << "drag element " << dragx << " " << dragy << std::endl;
            m_SelectedDragElement.Drag(dragx,dragy);
        }

    }

    m_Prev_GeoLoc = GeoLoc;


}
