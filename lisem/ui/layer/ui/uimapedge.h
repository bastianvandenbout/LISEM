#ifndef UIMAPEDGE_H
#define UIMAPEDGE_H

#include "layer/uilayer.h"

////
/// \brief The UIMapEdgeLayer class
///
/// This class draws all things related to the map ui edge
///
class UIMapEdgeLayer : public UILayer
{
private:


protected:

    bool m_shift_pressed = false;
    bool m_F_pressed = false;

public:

    inline UIMapEdgeLayer() : UILayer("map edge", false,"",false)
    {
        m_IsNative = true;
        m_IsUser = false;
        m_IsRequired =true;
    }

    inline ~UIMapEdgeLayer()
    {

    }

    inline QString layertypeName()
    {
        return "MapBorderLayer";
    }



    //virtual sub-draw function that is specifically meant for geo-objects
    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {
        //if(!s.is_3d)
        {

            DrawMapEdge(m,s,tm);
        }
    }

    inline void OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm) override
    {
        /*if(s.is_3d)
        {
            DrawMapEdge(m,s,tm,false);
        }*/
    }

    inline virtual void OnPostDraw3DUI(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
    {
        DrawMapEdge(m,s,tm, false,true);
    }

    inline void DrawMapEdge(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, bool drawcursor = true, bool do_if_3d = false)
    {

        if(s.GL_FrameBuffer->GetWidth() == 0 ||  s.GL_FrameBuffer->GetHeight() == 0)
        {
            return;
        }

        bool draw_ui = s.draw_ui;
        if(s.is_3d && !do_if_3d)
        {

            draw_ui = false;
        }else
        {
            if(s.is_3d)
            {
                if(s.ScreenPosX == nullptr || s.ScreenPosY == nullptr || s.ScreenPosZ == nullptr )
                {
                    return;

                }
            }
        }



        double unitsm = s.projection.GetUnitMultiplier();
        bool projected = s.projection.IsProjected();
        QString units = s.projection.GetUnitName();

        float width = s.scr_pixwidth;
        float height = s.scr_pixheight;

        float framewidth = s.ui_framewidth;//0.65*s.ui_scalem *std::max(25.0f,((float)std::max(0.0f,(width + height)/2.0f)) * 0.05f);
        float ui_ticktextscale = s.ui_textscale;//s.ui_scalem * std::max(0.3f,0.65f * framewidth/25.0f);

        if(!s.is_3d)
        {

        }
        if(draw_ui)
        {


            LSMVector4 BorderColor = LSMVector4(1.0,1.0,1.0,1.0);
            LSMVector4 LineColor = LSMVector4(0.0,0.0,0.0,1.0);
            float linethickness = std::max(1.0f,1.0f);
            LSMVector4 TickColor = LSMVector4(0.2,0.2,0.2,1.0);
            LSMVector4 TickSmallColor = LSMVector4(0.5,0.5,0.5,1.0);
            float tickwidth = std::max(1.f,1.5f*ui_ticktextscale);
            float ticksmallwidth = std::max(1.f,1.0f * ui_ticktextscale);
            float ticklength = 5.0f*ui_ticktextscale;
            float ticksmalllength = 3.0f* ui_ticktextscale;
            float tickdist = 2.0f * ui_ticktextscale;

            int nticks_legend = 11;

            bool dolegend = s.draw_legends && s.legendtotal > 0;
            float legendwidth = s.ui_scalem*0.085 * width ;
            float legendspacing = s.ui_scalem*0.015* width;
            float legendheight = s.ui_scalem*0.8 * height;
            float legendspacingy =s.ui_scalem*0.5 *(height-legendheight);
            float legendborderwidth = 2*ui_ticktextscale;
            float spacingright = framewidth + dolegend? legendwidth + 2.0 * legendspacing : 0.0;

            float xright = width-framewidth-spacingright;
            float xleft = framewidth;
            float ybottom = framewidth;
            float ytop = height - framewidth;

            m->m_ShapePainter->DrawSquare(0,0,width,framewidth,BorderColor);
            m->m_ShapePainter->DrawSquare(0,framewidth,framewidth,height - 2.0 * framewidth,BorderColor);
            m->m_ShapePainter->DrawSquare(0,height - framewidth,width,framewidth,BorderColor);
            m->m_ShapePainter->DrawSquare(width-framewidth-spacingright,framewidth,framewidth+spacingright,height - 2.0 *framewidth,BorderColor);

            m->m_ShapePainter->DrawSquareLine(framewidth,framewidth,width - 2.0 * framewidth-spacingright, height - 2.0 * framewidth,linethickness ,LineColor);

            float dx = s.brx - s.tlx;
            float dy = s.bry - s.tly;

            if(!((dx  > 0.00000001f) && (dy  > 0.00000001f)))
            {
                dx = 1;
                dy = 1;
            }

            if(dx > 0.00000001f)
            {
                m->m_TextPainter->DrawString(units,NULL,xleft - 30*ui_ticktextscale,ybottom - 20*ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);

                float dxpowcorr = dx/std::pow(10.0f,std::floor(log10f(dx)));
                float dxinterval = 0.0;

                int nticksx = 0;
                if(dxpowcorr < 2.5)
                {
                    dxinterval = 0.25*std::pow(10.0f,std::floor(log10f(dx)));
                    nticksx = std::floor(4.0*dxpowcorr)+ 3;
                }else if(dxpowcorr < 7.5)
                {
                    dxinterval = 0.5*std::pow(10.0f,std::floor(log10f(dx)));
                    nticksx = std::floor(2.0*dxpowcorr)+ 3;
                }else
                {
                    dxinterval = 1.0*std::pow(10.0f,std::floor(log10f(dx)));
                    nticksx = std::floor(dxpowcorr) + 3;
                }

                float xtickstart = (std::floor(s.tlx/dxinterval)-1.0f) * dxinterval;

                for(int i = 0; i < nticksx; i++)
                {
                    float xtick;
                    float xpix;

                    bool do_tick =true;
                    if(s.is_3d)
                    {
                        int temp_xpix = (float)(i) * (float)(width)/(float)(nticksx);

                        if(temp_xpix > -1 && temp_xpix < s.ScreenPosX->nrCols() && ybottom > -1 &&  ybottom < s.ScreenPosX->nrRows())
                        {


                            if(s.ScreenPosZ->data[(int)ybottom][(int)temp_xpix] < -1e25)
                            {
                                do_tick = false;
                            }
                            if(s.ScreenPosZ!= nullptr)
                            {
                                if(temp_xpix > -1 && temp_xpix < s.ScreenPosX->nrCols() && ybottom > -1 &&  ybottom < s.ScreenPosX->nrRows())
                                {
                                     xtick = s.Camera3D->GetPosition().z + s.ScreenPosZ->data[(int)ybottom][(int)temp_xpix];
                                }else {
                                    xtick = 0;
                                }

                            }else {
                                xtick = 0;
                            }
                        }
                        xpix =  temp_xpix;
                    }else {
                        xtick = xtickstart + (float)(i) * dxinterval;
                        xpix = width * (xtick - s.tlx)/std::max(0.00000000001f,s.brx-s.tlx);
                    }

                    if(do_tick)
                    {
                        if(xpix > xleft && xpix < xright)
                        {
                            m->m_ShapePainter->DrawLine(xpix,ybottom-tickdist,xpix,ybottom-tickdist-ticklength,tickwidth,TickColor);
                            m->m_TextPainter->DrawString(QString::number(xtick),NULL,xpix -20.0f,ybottom-tickdist-ticklength-1-1.15f*12 * ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);
                        }

                        if(!s.is_3d)
                        {
                            for(int j = 0; j < 4; j++)
                            {
                                xtick = xtick + (((float)(1.0))/5.0) *dxinterval;
                                xpix = width * (xtick - s.tlx)/std::max(0.00000000001f,s.brx-s.tlx);
                                if(xpix > xleft && xpix < xright)
                                {
                                    m->m_ShapePainter->DrawLine(xpix,ybottom-tickdist,xpix,ybottom-tickdist-ticksmalllength,ticksmallwidth,TickSmallColor);
                                }
                            }
                        }
                    }
                }
            }

            if(dy > 0.00000001f)
            {

                m->m_TextPainter->DrawString(units,NULL,xright - 25*ui_ticktextscale,ytop + 15*ui_ticktextscale,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);

                float dypowcorr = dy/std::pow(10.0f,std::floor(log10f(dy)));
                float dyinterval = 0.0;

                int nticksy = 0;
                if(dypowcorr < 2.5)
                {
                    dyinterval = 0.25*std::pow(10.0f,std::floor(log10f(dy)));
                    nticksy = std::floor(4.0*dypowcorr)+ 3;
                }else if(dypowcorr < 7.5)
                {
                    dyinterval = 0.5*std::pow(10.0f,std::floor(log10f(dy)));
                    nticksy = std::floor(2.0*dypowcorr)+ 3;
                }else
                {
                    dyinterval = 1.0*std::pow(10.0f,std::floor(log10f(dy)));
                    nticksy = std::floor(dypowcorr) + 3;
                }

                float ytickstart = (std::floor(s.tly/dyinterval)-1.0f) * dyinterval;

                for(int i = 0; i < nticksy; i++)
                {
                    float ytick;
                    float ypix;

                    bool do_tick = true;
                    if(s.is_3d)
                    {
                        int temp_ypix = (int) ((float)(i) * (float)(height)/(float)(nticksy));

                        if(s.ScreenPosX->data[ (int)temp_ypix][(int)xright] < -1e25)
                        {
                            do_tick = false;
                        }
                        if(s.ScreenPosX!= nullptr)
                        {
                            if(xright > -1 && xright < s.ScreenPosX->nrCols() && temp_ypix > -1 && temp_ypix < s.ScreenPosX->nrRows())
                            {
                                 ytick = s.Camera3D->GetPosition().x + s.ScreenPosX->data[ (int)temp_ypix][(int)xright];
                            }else {
                                ytick = 0;
                            }

                        }else {
                            ytick = 0;
                        }
                        ypix = temp_ypix;
                    }else {
                        ytick = ytickstart + (float)(i) * dyinterval;
                        ypix =  height * (ytick - s.tly)/std::max(0.00000000001f,s.bry-s.tly);
                    }

                    if(do_tick)
                    {
                        if(ypix > ybottom && ypix < ytop)
                        {
                            m->m_ShapePainter->DrawLine(xright,ypix,xright+ticklength + tickdist,ypix,tickwidth,TickColor);
                            m->m_TextPainter->DrawString(QString::number(ytick),NULL,xright + tickdist+ticklength+1,ypix -10.0f,LSMVector4(0.0,0.0,0.0,1.0),12 * ui_ticktextscale);
                        }

                        if(!s.is_3d)
                        {
                            for(int j = 0; j < 4; j++)
                            {
                                ytick = ytick + (((float)(1.0))/5.0) *dyinterval;
                                ypix = height * (ytick - s.tly)/std::max(0.00000000001f,s.bry-s.tly);
                                if(ypix > ybottom && ypix < ytop)
                                {
                                    m->m_ShapePainter->DrawLine(xright,ypix,xright+ticksmalllength + tickdist,ypix,ticksmallwidth,TickSmallColor);
                                }
                            }
                        }
                    }

                }
            }
        }

        if(drawcursor)
        {
            if(s.draw_cursor)
            {
                float CursorX = s.scr_width * (s.MouseGeoPosX - s.tlx)/s.width;
                float CursorY = (s.scr_height * (s.MouseGeoPosZ- s.tly)/s.height) ;

                float width_white = std::max(1.0,4.0 * s.ui_scale2d3d);
                float width_black = std::max(1.0,2.0 * s.ui_scale2d3d);
                float size = std::max(1.0,10.0 *s.ui_scale2d3d);

                //draw black dot on cursor location
                m->m_ShapePainter->DrawLine(CursorX-size,CursorY,CursorX+size,CursorY,width_white,LSMVector4(1.0,1.0,1.0,1.0));
                m->m_ShapePainter->DrawLine(CursorX,CursorY-size,CursorX,CursorY+size,width_white,LSMVector4(1.0,1.0,1.0,1.0));

                m->m_ShapePainter->DrawLine(CursorX-size,CursorY,CursorX+size,CursorY,width_black,LSMVector4(0.5,0.5,0.5,1.0));
                m->m_ShapePainter->DrawLine(CursorX,CursorY-size,CursorX,CursorY+size,width_black,LSMVector4(0.5,0.5,0.5,1.0));

                float midX = s.scr_width * (0.5*(s.brx+s.tlx) - s.tlx)/s.width;
                float midY = (s.scr_height - 1)- (s.scr_height * (0.5*(s.bry+s.tly)- s.tly)/s.height) ;


                for(int i = 0; i < s.m_FocusLocations.length(); i++)
                {
                    LSMVector2 loc = s.m_FocusLocations.at(i);

                    float squaresize = 6;
                    float FocusX = s.scr_width * (loc.x - s.tlx)/s.width;
                    float FocusY = (s.scr_height * (loc.y- s.tly)/s.height);

                    //square around point
                    m->m_ShapePainter->DrawSquareLine(FocusX - squaresize/2,FocusY - squaresize/2,squaresize,squaresize,2,LSMVector4(0,0,0,1));

                    //lines to edge of display to indicate coordinates
                    m->m_ShapePainter->DrawLine(0,FocusY,s.scr_width,FocusY,1,LSMVector4(0,0,0,0.3));
                    m->m_ShapePainter->DrawLine(FocusX,0,FocusX,s.scr_height,1,LSMVector4(0,0,0,0.3));
                }

                if(m_F_pressed)
                {
                    for(int i = 0; i < s.m_FocusLocations.length()-1; i++)
                    {
                        LSMVector2 loc = s.m_FocusLocations.at(i);
                        LSMVector2 loc2 = s.m_FocusLocations.at(i+1);

                        float squaresize = 6;
                        float FocusX = s.scr_width * (loc.x - s.tlx)/s.width;
                        float FocusY = (s.scr_height * (loc.y- s.tly)/s.height);

                        float FocusX2 = s.scr_width * (loc2.x - s.tlx)/s.width;
                        float FocusY2 = (s.scr_height * (loc2.y- s.tly)/s.height);

                        //square around point
                        m->m_ShapePainter->DrawSquareLine(FocusX - squaresize/2,FocusY - squaresize/2,squaresize,squaresize,2,LSMVector4(0,0,0,1));

                        //lines to edge of display to indicate coordinates
                        m->m_ShapePainter->DrawLine(FocusX,FocusY,FocusX2,FocusY2,2,LSMVector4(0,0,0,0.5));
                    }
                }


                for(int i = 0; i < s.m_FocusSquare.length(); i++)
                {
                    BoundingBox loc = s.m_FocusSquare.at(i);

                    float squaresize = 6;

                    float tlX = s.scr_width * (loc.GetMinX() - s.tlx)/s.width;
                    float tlY = (s.scr_height * (loc.GetMinY()- s.tly)/s.height);

                    float sX = s.scr_width * (loc.GetSizeX())/s.width;
                    float sY = (s.scr_height * (loc.GetSizeY())/s.height);

                    //square around point
                    m->m_ShapePainter->DrawSquareLine(tlX,tlY,sX,sY,2,LSMVector4(0,0,0,1));
                    m->m_ShapePainter->DrawSquare(tlX,tlY,sX,sY,LSMVector4(0,0,0,0.3));

                }


            }



        }

        if(s.FocusLayer != nullptr && s.MouseHit && m_shift_pressed)
        {
            double tolerence = 0.02 *std::min(s.width,s.height);
            LayerProbeResult pr = s.FocusLayer->Probe(LSMVector2(s.MouseGeoPosX,s.MouseGeoPosZ), s.projection,tolerence);
            QString text = "(x:" +  QString::number(s.MouseGeoPosX) +" ,y:"+ QString::number(s.MouseGeoPosZ) + ")   ";
            for(int i = 0; i < pr.AttrNames.length(); i++)
            {
                text += pr.AttrNames.at(i) + " : " + pr.AttrValues.at(i);
            }
            m->m_TextPainter->DrawString(text,NULL,0,5,LSMVector4(0.0,0.0,0.0,1.0),ui_ticktextscale*12);


        }else if(s.MouseHit && m_shift_pressed)
        {
            m->m_TextPainter->DrawString("(x:" +  QString::number(s.MouseGeoPosX) +" ,y:"+ QString::number(s.MouseGeoPosZ) + ")  ",NULL,0,5,LSMVector4(0.0,0.0,0.0,1.0),ui_ticktextscale*12);
        }


        //draw some camera properties for debugging
        //m->m_TextPainter->DrawString("Camera Direction: x" +  QString::number(s.Camera3D->GetViewDir().X()) +" y"+ QString::number(s.Camera3D->GetViewDir().Y())+ " z"+ QString::number(s.Camera3D->GetViewDir().Z()),NULL,0,0,LSMVector4(0.0,0.0,0.0,1.0),12);
        //m->m_TextPainter->DrawString("Camera Position: x" +  QString::number(s.Camera3D->GetPosition().X()) +" y"+ QString::number(s.Camera3D->GetPosition().Y())+ " z"+ QString::number(s.Camera3D->GetPosition().Z()),NULL,0,15,LSMVector4(0.0,0.0,0.0,1.0),12);
        //m->m_TextPainter->DrawString("Camera up: x" +  QString::number(s.Camera3D->GetUpDir().X()) +" y"+ QString::number(s.Camera3D->GetUpDir().Y())+ " z"+ QString::number(s.Camera3D->GetUpDir().Z()),NULL,0,30,LSMVector4(0.0,0.0,0.0,1.0),12);
        //m->m_TextPainter->DrawString("Camera right: x" +  QString::number(s.Camera3D->GetRightDir().X()) +" y"+ QString::number(s.Camera3D->GetRightDir().Y())+ " z"+ QString::number(s.Camera3D->GetRightDir().Z()),NULL,0,45,LSMVector4(0.0,0.0,0.0,1.0),12);
        //m->m_TextPainter->DrawString("Camera Info ",NULL,0,60,LSMVector4(0.0,0.0,0.0,1.0),12);



    }

    inline void OnKeyPressed(int key, int pressed, int mods)
    {
        if(key == GLFW_KEY_LEFT_SHIFT && pressed == GLFW_PRESS)
        {
            m_shift_pressed = true;
        }else if(key == GLFW_KEY_LEFT_SHIFT && pressed == GLFW_RELEASE)
        {
             m_shift_pressed = false;
        }

        if(key == GLFW_KEY_F && pressed == GLFW_PRESS)
        {
            m_F_pressed = true;
        }else if(key == GLFW_KEY_F && pressed == GLFW_RELEASE)
        {
             m_F_pressed = false;
        }

    }


    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {

        m_IsPrepared = true;
    }
    inline void OnDestroy(OpenGLCLManager * m) override
    {

        m_IsPrepared = false;
    }

    inline void OnDraw(OpenGLCLManager * m, GeoWindowState s) override
    {

    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s,WorldGLTransformManager * tm)
    {


        m_IsCRSChanged = false;

    }


    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {

    }



};


#endif // UIMAPEDGE_H
