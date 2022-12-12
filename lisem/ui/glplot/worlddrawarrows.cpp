#include "worldwindow.h"
#include "gl/openglcldatamanager.h"
#include "lisemmath.h"
LSMVector2 WorldWindow::WorldToScreenPix(LSMMatrix4x4 m, LSMVector3 DrawCenter)
{

         LSMVector4 pixlx = m * LSMVector4(DrawCenter.x,DrawCenter.y,DrawCenter.z,1.0);
         pixlx = pixlx/pixlx.w;
         pixlx.x = ((pixlx.x+1.0)/2.0) *((float)( m_OpenGLCLManager->m_width));
         pixlx.y = m_OpenGLCLManager->m_height - ((pixlx.y+1.0)/2.0) *((float)( m_OpenGLCLManager->m_height));

         return LSMVector2(pixlx.x,pixlx.y);
}

void WorldWindow::Draw3DArrows(GeoWindowState s, bool external)
{
    m_DragElements.clear();

    //if not external and in moving state

    if(m_DrawArrows && !external)
    {

        //for all movable objects

        //draw 3d arrows that show up in camera-space with identical sizes (rescale based on distance and fov)

        for(int i = 0; i < m_UILayerList.length() ; i++)
        {

            UILayer * l = m_UILayerList.at(i);

            if((l->IsMovable() || l->IsRotateAble() || l->IsScaleAble()) && l->Is3D())
            {

                if(l->IsMovable() && m_GizmoMode == GIZMO_MOVE)
                {


                    LSMVector3 FLoc = l->GetPosition();

                    //draw 3 arrows aligned with axes
                    //use view-axis aligned line segments (as done in some other 3d viewers/editors)

                    LSMVector3 camerapos = s.Camera3D->GetPosition();

                    LSMVector3 DrawCenter = FLoc  - LSMVector3(camerapos.x,0.0,camerapos.z);

                    LSMVector3 NormalX = LSMVector3(1.0,0.0,0.0) * 0.15 * DrawCenter.length();
                    LSMVector3 NormalY = LSMVector3(0.0,1.0,0.0) * 0.15 * DrawCenter.length();
                    LSMVector3 NormalZ = LSMVector3(0.0,0.0,1.0) * 0.15 * DrawCenter.length();




                    //view-plane translation


                    //view-plane rotation

                    for(int j = 0; j < 90; j++)
                    {
                        //get position along great circle made by intersection of plane

                        //m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalX,LSMVector3(0.0,camerapos.y,0.0),0.02* 0.5 * DrawCenter.length(),LSMVector4(1.0,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);


                    }



                    //get pixel locations of lines

                    LSMVector4 pix = s.Camera3D->GetProjectionMatrixNoTranslationXZ() * LSMVector4(DrawCenter.x,DrawCenter.y,DrawCenter.z,1.0);
                    pix = pix/pix.w;
                    pix.x = ((pix.x+1.0)/2.0) *((float)( m_OpenGLCLManager->m_width));
                    pix.y = m_OpenGLCLManager->m_height - ((pix.y+1.0)/2.0) *((float)( m_OpenGLCLManager->m_height));

                    LSMVector4 pixlx = s.Camera3D->GetProjectionMatrixNoTranslationXZ() * LSMVector4(DrawCenter.x + NormalX.x,DrawCenter.y + NormalX.y,DrawCenter.z + NormalX.z,1.0);
                    pixlx = pixlx/pixlx.w;
                    pixlx.x = ((pixlx.x+1.0)/2.0) *((float)( m_OpenGLCLManager->m_width));
                    pixlx.y = m_OpenGLCLManager->m_height - ((pixlx.y+1.0)/2.0) *((float)( m_OpenGLCLManager->m_height));

                    LSMVector4 pixly = s.Camera3D->GetProjectionMatrixNoTranslationXZ() * LSMVector4(DrawCenter.x + NormalY.x,DrawCenter.y + NormalY.y,DrawCenter.z + NormalY.z,1.0);
                    pixly = pixly/pixly.w;
                    pixly.x = ((pixly.x+1.0)/2.0) *((float)( m_OpenGLCLManager->m_width));
                    pixly.y = m_OpenGLCLManager->m_height - ((pixly.y+1.0)/2.0) *((float)( m_OpenGLCLManager->m_height));

                    LSMVector4 pixlz = s.Camera3D->GetProjectionMatrixNoTranslationXZ() * LSMVector4(DrawCenter.x + NormalZ.x,DrawCenter.y + NormalZ.y,DrawCenter.z + NormalZ.z,1.0);
                    pixlz = pixlz/pixlz.w;
                    pixlz.x = ((pixlz.x+1.0)/2.0) *((float)( m_OpenGLCLManager->m_width));
                    pixlz.y = m_OpenGLCLManager->m_height - ((pixlz.y+1.0)/2.0) *((float)( m_OpenGLCLManager->m_height));




                    DragElement lx;
                    lx.xc.push_back(pix.x);
                    lx.yc.push_back(pix.y);
                    lx.xc.push_back(pixlx.x);
                    lx.yc.push_back(pixlx.y);

                    DragElement ly;
                    ly.xc.push_back(pix.x);
                    ly.yc.push_back(pix.y);
                    ly.xc.push_back(pixly.x);
                    ly.yc.push_back(pixly.y);

                    DragElement lz;
                    lz.xc.push_back(pix.x);
                    lz.yc.push_back(pix.y);
                    lz.xc.push_back(pixlz.x);
                    lz.yc.push_back(pixlz.y);

                    DragElement sx;
                    sx.xc.push_back(pix.x * 0.6 + 0.4 * pixlx.x);
                    sx.yc.push_back(pix.y * 0.6 + 0.4 * pixlx.y);
                    sx.xc.push_back(pix.x * 0.4 + 0.6 * pixlx.x);
                    sx.yc.push_back(pix.y * 0.4 + 0.6 * pixlx.y);

                    DragElement sy;
                    sy.xc.push_back(pix.x * 0.6 + 0.4 * pixly.x);
                    sy.yc.push_back(pix.y * 0.6 + 0.4 * pixly.y);
                    sy.xc.push_back(pix.x * 0.4 + 0.6 * pixly.x);
                    sy.yc.push_back(pix.y * 0.4 + 0.6 * pixly.y);

                    DragElement sz;
                    sz.xc.push_back(pix.x * 0.6 + 0.4 * pixlz.x);
                    sz.yc.push_back(pix.y * 0.6 + 0.4 * pixlz.y);
                    sz.xc.push_back(pix.x * 0.4 + 0.6 * pixlz.x);
                    sz.yc.push_back(pix.y * 0.4 + 0.6 * pixlz.y);


                    lx.Layer = l;
                    lx.GizmoDirection = 0;
                    lx.GizmoType = 0;
                    lx.GizmoAction = 0;
                    lx.CollisionType = 0;
                    LSMVector2 lxpixv(pixlx.x - pix.x,pixlx.y-pix.y);
                    //lxpixv = lxpixv.Normalize();

                    ly.Layer = l;
                    ly.GizmoDirection = 1;
                    ly.GizmoType = 0;
                    ly.GizmoAction = 0;
                    ly.CollisionType = 0;
                    LSMVector2 lypixv(pixly.x - pix.x,pixly.y-pix.y);
                    //lypixv = lypixv.Normalize();

                    lz.Layer = l;
                    lz.GizmoDirection = 2;
                    lz.GizmoType = 0;
                    lz.GizmoAction = 0;
                    lz.CollisionType = 0;
                    LSMVector2 lzpixv(pixlz.x - pix.x,pixlz.y-pix.y);
                    //lzpixv = lzpixv.Normalize();

                    if( lxpixv.x < 1.0 && lxpixv.x > -1.0)
                    {
                        lxpixv.x =1.0;
                    }
                    if( lxpixv.y < 1.0 && lxpixv.y > -1.0)
                    {
                        lxpixv.y =1.0;
                    }
                    if( lypixv.x < 1.0 && lypixv.x > -1.0)
                    {
                        lypixv.x =1.0;
                    }
                    if( lypixv.y < 1.0 && lypixv.y > -1.0)
                    {
                        lypixv.y =1.0;
                    }
                    if( lzpixv.x < 1.0 && lzpixv.x > -1.0)
                    {
                        lzpixv.x =1.0;
                    }
                    if( lzpixv.y < 1.0 && lzpixv.y > -1.0)
                    {
                        lzpixv.y =1.0;
                    }

                    //std::cout << "arrow stuff " << 0.3 * DrawCenter.length() << " " << lxpixv.x << " " << lxpixv.y << " " << lypixv.x << " " << lypixv.y << " " << lzpixv.x << " " << lzpixv.y << std::endl;

                    lx.fac_x =  -0.01 * DrawCenter.length()*absMax(0.1f,lxpixv.x)/std::max(0.001f,std::sqrt(lxpixv.x * lxpixv.x + lxpixv.y * lxpixv.y));
                    lx.fac_y = -0.01 * DrawCenter.length()*absMax(0.1f,lxpixv.y)/std::max(0.001f,std::sqrt(lxpixv.x * lxpixv.x + lxpixv.y * lxpixv.y));
                    ly.fac_x = -0.01 * DrawCenter.length()*absMax(0.1f,lypixv.x)/std::max(0.001f,std::sqrt(lypixv.x * lypixv.x + lypixv.y * lypixv.y));
                    ly.fac_y =  -0.01 * DrawCenter.length()*absMax(0.1f,lypixv.y)/std::max(0.001f,std::sqrt(lypixv.x * lypixv.x + lypixv.y * lypixv.y));
                    lz.fac_x = -0.01 * DrawCenter.length()*absMax(0.1f,lzpixv.x)/std::max(0.001f,std::sqrt(lzpixv.x * lzpixv.x + lzpixv.y * lzpixv.y));
                    lz.fac_y =  -0.01 * DrawCenter.length()*absMax(0.1f,lzpixv.y)/std::max(0.001f,std::sqrt(lzpixv.x * lzpixv.x + lzpixv.y * lzpixv.y));




                    sx.Layer = l;
                    sx.GizmoDirection = 0;
                    sx.GizmoType = 2;
                    sx.GizmoAction = 0;
                    sx.CollisionType = 0;
                    sx.fac_x = 0.1 * 1.0/lxpixv.x ;
                    sx.fac_y = 0.1 * 1.0/lxpixv.y;

                    sy.Layer = l;
                    sy.GizmoDirection = 1;
                    sy.GizmoType = 2;
                    sy.GizmoAction = 0;
                    sy.CollisionType = 0;
                    sy.fac_x = 0.1 * 1.0/lypixv.y;
                    sy.fac_y = 0.1 * 1.0/lypixv.x;


                    sz.Layer = l;
                    sz.GizmoDirection = 2;
                    sz.GizmoType = 2;
                    sz.GizmoAction = 0;
                    sz.CollisionType = 0;
                    sz.fac_x = 0.1 * 1.0/lzpixv.y;
                    sz.fac_y = 0.1 * 1.0/lzpixv.x;



                    DragElement px;
                    px.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.67 *NormalZ).x);
                    px.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.67 *NormalZ).y);
                    px.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.67 *NormalZ).x);
                    px.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.67 *NormalZ).y);
                    px.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.33 *NormalZ).x);
                    px.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.33 *NormalZ).y);
                    px.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.33 *NormalZ).x);
                    px.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.33 *NormalZ).y);

                    DragElement py;
                    py.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.67 *NormalY).x);
                    py.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.67 *NormalY).y);
                    py.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.67 *NormalY).x);
                    py.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.67 *NormalY).y);
                    py.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.33 *NormalY).x);
                    py.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalX + 0.33 *NormalY).y);
                    py.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.33 *NormalY).x);
                    py.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalX + 0.33 *NormalY).y);

                    DragElement pz;
                    pz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalY + 0.67 *NormalZ).x);
                    pz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalY + 0.67 *NormalZ).y);
                    pz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalY + 0.67 *NormalZ).x);
                    pz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalY + 0.67 *NormalZ).y);
                    pz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalY + 0.33 *NormalZ).x);
                    pz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.33 * NormalY + 0.33 *NormalZ).y);
                    pz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalY + 0.33 *NormalZ).x);
                    pz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + 0.67 * NormalY + 0.33 *NormalZ).y);



                    px.Layer = l;
                    px.GizmoDirection = 0;
                    px.GizmoType = 0;
                    px.GizmoAction = 1;
                    px.CollisionType = 1;

                    py.Layer = l;
                    py.GizmoDirection = 1;
                    py.GizmoType = 0;
                    py.GizmoAction = 1;
                    py.CollisionType = 1;

                    pz.Layer = l;
                    pz.GizmoDirection = 2;
                    pz.GizmoType = 0;
                    pz.GizmoAction = 1;
                    pz.CollisionType = 1;

                    px.fac_x =  -0.01 * DrawCenter.length()*absMax(0.1f,lxpixv.x)/std::max(0.001f,std::sqrt(lxpixv.x * lxpixv.x + lxpixv.y * lxpixv.y));
                    px.fac_y = -0.01 * DrawCenter.length()*absMax(0.1f,lxpixv.y)/std::max(0.001f,std::sqrt(lxpixv.x * lxpixv.x + lxpixv.y * lxpixv.y));
                    py.fac_x = -0.01 * DrawCenter.length()*absMax(0.1f,lypixv.x)/std::max(0.001f,std::sqrt(lypixv.x * lypixv.x + lypixv.y * lypixv.y));
                    py.fac_y =  -0.01 * DrawCenter.length()*absMax(0.1f,lypixv.y)/std::max(0.001f,std::sqrt(lypixv.x * lypixv.x + lypixv.y * lypixv.y));
                    pz.fac_x = -0.01 * DrawCenter.length()*absMax(0.1f,lzpixv.x)/std::max(0.001f,std::sqrt(lzpixv.x * lzpixv.x + lzpixv.y * lzpixv.y));
                    pz.fac_y =  -0.01 * DrawCenter.length()*absMax(0.1f,lzpixv.y)/std::max(0.001f,std::sqrt(lzpixv.x * lzpixv.x + lzpixv.y * lzpixv.y));



                    DragElement rx;
                    DragElement ry;
                    DragElement rz;



                    bool is_prev_vis_x = false;
                    bool is_prev_vis_y = false;
                    bool is_prev_vis_z = false;

                    for(int j = 0; j < 90; j++)
                    {
                        //get position along great circle made by intersection of plane
                        float radians = (((float)(j))/90.0)* 2.0 * 3.14159;
                        float radiansn = (((float)(j+1))/90.0)* 2.0 * 3.14159;

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal = (sin(radians) * NormalX + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalX + cos(radians) * NormalY - LSMVector3(0.0,camerapos.y,0.0)).dot(normal) <0.0)
                        {
                            rx.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalX + cos(radians) * NormalY).x);
                            rx.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalX + cos(radians) * NormalY).y);
                            rx.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalX + cos(radiansn) * NormalY).x);
                            rx.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalX + cos(radiansn) * NormalY).y);
                        }

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal2 = (sin(radians) * NormalX + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalX + cos(radians) * NormalZ - LSMVector3(0.0,camerapos.y,0.0)).dot(normal2) <0.0)
                        {
                            ry.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalX + cos(radians) * NormalZ).x);
                            ry.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalX + cos(radians) * NormalZ).y);
                            ry.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalX + cos(radiansn) * NormalZ).x);
                            ry.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalX + cos(radiansn) * NormalZ).y);
                        }

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal3 = (sin(radians) * NormalZ + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalZ + cos(radians) * NormalY - LSMVector3(0.0,camerapos.y,0.0)).dot(normal3) <0.0)
                        {
                            rz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalZ + cos(radians) * NormalY).x);
                            rz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radians) * NormalZ + cos(radians) * NormalY).y);
                            rz.xc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalZ + cos(radiansn) * NormalY).x);
                            rz.yc.push_back(WorldToScreenPix(s.Camera3D->GetProjectionMatrixNoTranslationXZ(),DrawCenter + sin(radiansn) * NormalZ + cos(radiansn) * NormalY).y);
                        }
                    }


                    rx.Layer = l;
                    rx.GizmoDirection = 0;
                    rx.GizmoType = 1;
                    rx.GizmoAction = 0;
                    rx.CollisionType = 0;
                    rx.lines_seperated = true;
                    rx.fac_x = 0.01 * DrawCenter.length()/absMax(0.01,lxpixv.x);
                    rx.fac_y = 0.01 * DrawCenter.length()/absMax(0.01,lxpixv.y);

                    ry.Layer = l;
                    ry.GizmoDirection = 1;
                    ry.GizmoType = 1;
                    ry.GizmoAction = 0;
                    ry.CollisionType = 0;
                    ry.lines_seperated = true;
                    ry.fac_x = 0.01 * DrawCenter.length()/absMax(0.01,lypixv.x);
                    ry.fac_y = 0.01 * DrawCenter.length()/absMax(0.01,lypixv.y);

                    rz.Layer = l;
                    rz.GizmoDirection = 2;
                    rz.GizmoType = 1;
                    rz.GizmoAction = 0;
                    rz.CollisionType = 0;
                    rz.lines_seperated = true;
                    rz.fac_x = 0.01 * DrawCenter.length()/absMax(0.01,lzpixv.x);
                    rz.fac_y = 0.01 * DrawCenter.length()/absMax(0.01,lzpixv.y);


                    //translation arrows
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalX,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),lx.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(1.0,0.5,0.5,1.0) : LSMVector4(1.0,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalY,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),ly.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,1.0,0.5,1.0) : LSMVector4(0.0,1.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + NormalZ,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),lz.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,0.5,1.0,1.0) : LSMVector4(0.0,0.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);

                    //scale cubes
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + 0.40 * NormalX,DrawCenter + 0.60*NormalX,LSMVector3(0.0,camerapos.y,0.0),0.04* 0.5 * DrawCenter.length(),sx.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(1.0,0.5,0.5,1.0) : LSMVector4(0.8,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + 0.40 * NormalY,DrawCenter + 0.60*NormalY,LSMVector3(0.0,camerapos.y,0.0),0.04* 0.5 * DrawCenter.length(),sy.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,1.0,0.0,1.0) : LSMVector4(0.0,0.8,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + 0.40 * NormalZ,DrawCenter + 0.60*NormalZ,LSMVector3(0.0,camerapos.y,0.0),0.04* 0.5 * DrawCenter.length(),sz.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,0.5,1.0,1.0) : LSMVector4(0.0,0.0,0.8,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);

                    //translate planes
                    m_OpenGLCLManager->m_ShapePainter->DrawSquare3D(DrawCenter + 0.67 * NormalX + 0.67 *NormalZ,DrawCenter + 0.33 * NormalX + 0.67 *NormalZ,DrawCenter + 0.33 * NormalX + 0.33 *NormalZ,DrawCenter + 0.67 * NormalX + 0.33 *NormalZ,LSMVector3(0.0,camerapos.y,0.0),0.02* 0.5 * DrawCenter.length(),px.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,1.0,0.5,1.0) : LSMVector4(0.0,1.0,.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawSquare3D(DrawCenter + 0.67 * NormalX + 0.67 *NormalY,DrawCenter + 0.33 * NormalX + 0.67 *NormalY,DrawCenter + 0.33 * NormalX + 0.33 *NormalY,DrawCenter + 0.67 * NormalX + 0.33 *NormalY,LSMVector3(0.0,camerapos.y,0.0),0.02* 0.5 * DrawCenter.length(),py.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(0.5,0.5,1.0,1.0) : LSMVector4(0.0,0.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                    m_OpenGLCLManager->m_ShapePainter->DrawSquare3D(DrawCenter + 0.67 * NormalY + 0.67 *NormalZ,DrawCenter + 0.33 * NormalY + 0.67 *NormalZ,DrawCenter + 0.33 * NormalY + 0.33 *NormalZ,DrawCenter + 0.67 * NormalY + 0.33 *NormalZ,LSMVector3(0.0,camerapos.y,0.0),0.02* 0.5 * DrawCenter.length(),pz.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y) ? LSMVector4(1.0,0.5,0.5,1.0) : LSMVector4(1.0,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);

                    //rotational arcs

                    LSMVector4 color_rx = LSMVector4(0.0,0.0,1.0,1.0);
                    LSMVector4 color_ry = LSMVector4(1.0,0.0,0.0,1.0);
                    LSMVector4 color_rz = LSMVector4(0.0,1.0,0.0,1.0);

                    if(rx.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y))
                    {
                        color_rx =LSMVector4(0.5,0.5,1.0,1.0);
                    }
                    if(ry.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y))
                    {
                        color_ry =LSMVector4(1.0,0.5,0.5,1.0);
                    }
                    if(rz.Collides(m_MouseState.Pos_x,m_MouseState.Pos_y))
                    {
                        color_rz =LSMVector4(0.5,1.0,0.5,1.0);
                    }


                    //add rototaional line arc to the collision detection elements


                    //check collision to determine colors


                    //draw the actual line arcs
                    for(int j = 0; j < 90; j++)
                    {
                        //get position along great circle made by intersection of plane
                        float radians = (((float)(j))/90.0)* 2.0 * 3.14159;
                        float radiansn = (((float)(j+1))/90.0)* 2.0 * 3.14159;

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal = (sin(radians) * NormalX + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalX + cos(radians) * NormalY - LSMVector3(0.0,camerapos.y,0.0)).dot(normal) <0.0)
                        {
                            m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + sin(radians) * NormalX + cos(radians) * NormalY,DrawCenter+ sin(radiansn) * NormalX + cos(radiansn) * NormalY,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),color_rx,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                        }

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal2 = (sin(radians) * NormalX + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalX + cos(radians) * NormalZ - LSMVector3(0.0,camerapos.y,0.0)).dot(normal2) <0.0)
                        {
                            m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + sin(radians) * NormalX + cos(radians) * NormalZ,DrawCenter+ sin(radiansn) * NormalX + cos(radiansn) * NormalZ,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),color_ry,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                        }

                        //only draw this segment of the great circle if it is visible to camera
                        LSMVector3 normal3 = (sin(radians) * NormalZ + cos(radians) * NormalY).Normalize();
                        if((DrawCenter + sin(radians) * NormalZ + cos(radians) * NormalY - LSMVector3(0.0,camerapos.y,0.0)).dot(normal3) <0.0)
                        {
                            m_OpenGLCLManager->m_ShapePainter->DrawLine3D(DrawCenter + sin(radians) * NormalZ + cos(radians) * NormalY,DrawCenter+ sin(radiansn) * NormalZ + cos(radiansn) * NormalY,LSMVector3(0.0,camerapos.y,0.0),0.01* 0.5 * DrawCenter.length(),color_rz,s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false,false);
                        }
                    }

                    m_DragElements.push_back(lx);
                    m_DragElements.push_back(ly);
                    m_DragElements.push_back(lz);

                    m_DragElements.push_back(sx);
                    m_DragElements.push_back(sy);
                    m_DragElements.push_back(sz);

                    m_DragElements.push_back(px);
                    m_DragElements.push_back(py);
                    m_DragElements.push_back(pz);

                    m_DragElements.push_back(rx);
                    m_DragElements.push_back(ry);
                    m_DragElements.push_back(rz);


                    //std::cout << "pix " << pix.x << " " << pix.y << " " << pix.z << " " << pix.w << std::endl;

                }
                if(l->IsRotateAble() && m_GizmoMode == GIZMO_ROTATE)
                {

                }
                if(l->IsScaleAble() && m_GizmoMode == GIZMO_SCALE)
                {

                }



            }


        }

        //keep track of the arrows just so we can determine later if the user clicked one of them



    }


}

void WorldWindow::Arrow3DRayCast()
{




}



void WorldWindow::Draw2DArrows(GeoWindowState s, bool external)
{
    m_DragElements.clear();

    //if not external and in moving state

    if(m_DrawArrows && !external)
    {
        float pixelsizex = (s.tlx-s.brx)/std::max(1.0f,(float)(s.scr_pixwidth));
        float pixelsizey = (s.tly-s.bry)/std::max(1.0f,(float)(s.scr_pixheight));

        for(int i = 0; i < m_UILayerList.length() ; i++)
        {

            UILayer * l = m_UILayerList.at(i);


            if((l->IsMovable() || l->IsRotateAble() || l->IsScaleAble()) && !l->Is3D())
            {

                if(l->IsMovable() && m_GizmoMode == GIZMO_MOVE)
                {


                    float ui_ticktextscale = s.ui_textscale;
                    float width = s.scr_pixwidth;
                    float height = s.scr_pixheight;


                    LSMVector3 FLoc = l->GetPosition();

                    float tlX = s.scr_width * (FLoc.x- s.tlx)/s.width;
                    float tlY = (s.scr_height * (FLoc.z- s.tly)/s.height);

                    float grey = 0.5;
                    //movement gizmo, 4-directional arrow in a circle
                    DragElement elem_mov;

                    elem_mov.Layer = l;
                    elem_mov.GizmoDirection = 3;
                    elem_mov.GizmoType = 0;
                    elem_mov.GizmoAction = 0;
                    elem_mov.CollisionType = 1;

                    for(int j = 0; j < 25; j++)
                    {
                        //get position along great circle made by intersection of plane
                        float radians = (((float)(j))/25.0)* 2.0 * 3.14159;
                        float radiansn = (((float)(j+1))/25.0)* 2.0 * 3.14159;


                        elem_mov.xc.push_back(tlX + cos(radians) * 10.0);
                        elem_mov.yc.push_back(m_OpenGLCLManager->m_height - (tlY + sin(radians) * 10.0));

                        elem_mov.xc.push_back(tlX + cos(radiansn) * 10.0);
                        elem_mov.yc.push_back(m_OpenGLCLManager->m_height - (tlY + sin(radiansn) * 10.0));

                    }
                    elem_mov.fac_x = 1.0* pixelsizex;
                    elem_mov.fac_y = 1.0*pixelsizey;

                    if(elem_mov.Collides(s.MousePosX,m_OpenGLCLManager->m_height - s.MousePosY))
                    {
                        grey = 0.0;
                    }

                    m_OpenGLCLManager->m_ShapePainter->DrawRegularNGon(tlX,tlY,10,20,LSMVector4(1.0,1.0,1.0,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawRegularNGonOutline(tlX,tlY,10,2.0,20,LSMVector4(0.0,1.0,0.0,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX-7,tlY,tlX+7,tlY,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY-7,tlX,tlY+7,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY-7,tlX-3,tlY-4,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY-7,tlX+3,tlY-4,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY+7,tlX-3,tlY+4,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY+7,tlX+3,tlY+4,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX-7,tlY,tlX-4,tlY-3,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX-7,tlY,tlX-4,tlY+3,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX+7,tlY,tlX+4,tlY-3,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX+7,tlY,tlX+4,tlY+3,2,LSMVector4(grey,grey,grey,1.0));

                    //m->m_ShapePainter->DrawSquare(0,0,width,framewidth,BorderColor);

                    //m->m_ShapePainter->DrawLine(xpix,ybottom-tickdist,xpix,ybottom-tickdist-ticklength,tickwidth,TickColor);



                    m_DragElements.push_back(elem_mov);


                    //scale gizmo
                    //based on a current scale, draw two diagonal arrows in the bottom right corner

                    DragElement elem_size;

                    elem_size.Layer = l;
                    elem_size.GizmoDirection = 3;
                    elem_size.GizmoType = 2;
                    elem_size.GizmoAction = 1;
                    elem_size.CollisionType = 1;



                    LSMVector3 FScale = l->GetScale();

                    float tlXs = s.scr_width * (FLoc.x + 0.5*FScale.x - s.tlx)/s.width;
                    float tlYs = (s.scr_height * (FLoc.z - 0.5 * FScale.z - s.tly)/s.height);


                    for(int j = 0; j < 25; j++)
                    {
                        //get position along great circle made by intersection of plane
                        float radians = (((float)(j))/25.0)* 2.0 * 3.14159;
                        float radiansn = (((float)(j+1))/25.0)* 2.0 * 3.14159;


                        elem_size.xc.push_back(tlXs + cos(radians) * 10.0);
                        elem_size.yc.push_back(m_OpenGLCLManager->m_height - (tlYs + sin(radians) * 10.0));

                        elem_size.xc.push_back(tlXs + cos(radiansn) * 10.0);
                        elem_size.yc.push_back(m_OpenGLCLManager->m_height - (tlYs + sin(radiansn) * 10.0));

                    }
                    elem_size.fac_x = -1.0/std::max(5.0f,std::fabs(tlX - tlXs));
                    elem_size.fac_y = 1.0/std::max(5.0f,std::fabs(tlY - tlYs));


                    grey = 0.5;
                    if(elem_size.Collides(s.MousePosX,m_OpenGLCLManager->m_height - s.MousePosY))
                    {
                        grey = 0.0;
                    }
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlX,tlY,tlXs,tlYs,2,LSMVector4(0.5,0.5,0.5,0.4));

                    m_OpenGLCLManager->m_ShapePainter->DrawRegularNGon(tlXs,tlYs,7,20,LSMVector4(1.0,1.0,1.0,0.8));
                    m_OpenGLCLManager->m_ShapePainter->DrawRegularNGonOutline(tlXs,tlYs,7,2.0,20,LSMVector4(1.0,0.0,0.0,0.8));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlXs-5,tlYs-5,tlXs+5,tlYs+5,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlXs-5,tlYs-5,tlXs-5,tlYs-2,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlXs-5,tlYs-5,tlXs-2,tlYs-5,2,LSMVector4(grey,grey,grey,1.0));

                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlXs+5,tlYs+5,tlXs+5,tlYs+2,2,LSMVector4(grey,grey,grey,1.0));
                    m_OpenGLCLManager->m_ShapePainter->DrawLine(tlXs+5,tlYs+5,tlXs+2,tlYs+5,2,LSMVector4(grey,grey,grey,1.0));


                    m_DragElements.push_back(elem_size);




                }

            }
        }
    }




}

void WorldWindow::Arrow2DRayCast()
{

    //we need to draw the arrows in the middle of the visible domain of  a layer



}
