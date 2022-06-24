#pragma once

#include "styles/sphstyle.h"
#include "openglclmanager.h"
#include "glplot/worldgltransforms.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_matrix4x4.h"
#include "glplot/gl3dcamera.h"
#include "QList"
#include "openglmsaarendertarget.h"
#include "layer/uilayer.h"

class UILayer;

////
//this struct contains all the parameters related to what/where should be draw
//layers are expected to follow the parameters contained within this struct
//example: 2d/3d
//it also contains user defined control of the
//map window so layers can know what scale/location is being drawn to this window
//
typedef struct GeoWindowState
{

    UILayer * FocusLayer;

    ////
    ///
    /// THE PROJECTION
    ///
    ////


    //all units are to be interpreted by the chosen CRS as defined in the world2dwindow class
    GeoProjection projection;

    ////
    ///
    /// CURRENT MOUSE STATE, AND GEOLOCATION OF MOUSE DIRECTION
    ///
    ////

    bool MouseHit = false;

    float MouseGeoPosX = 0;
    float MouseGeoPosY = 0;
    float MouseGeoPosZ = 0;

    float MousePosX = 0;
    float MousePosY = 0;
    bool MouseLeftButton = false;
    bool MouseRightButton = false;



    ////
    ///
    /// ALL THE 2D VIEW PARAMETERS
    ///
    ////

    //scale (the total WIDTH of the display in CRS units is (1.0f/scale) * 1.0
    float scale = 1.0f;

    //total width and height in CRS units
    float width = 1.0f;
    float height = 1.0f;

    //translation of center point of the window
    float translation_x = 0.0f;
    float translation_y = 0.0f;

    //coordinates of top left and bottom right corners of the display
    float tlx = -0.5f;
    float tly = -0.5f;
    float brx = 0.5f;
    float bry = 0.5f;

    //some screen properties
    float scr_pixwidth = 1.0f;
    float scr_pixheight = 1.0f;
    float scr_ratio = 1.0f;
    float scr_width = 1.0;
    float scr_height = 1.0;

    bool flip = false;


    bool ChannelFilter = false;
    int Channel = 0;


    ////
    ///
    /// ALL THE 3D VIEW PARAMETERS
    ///
    ////

    GL3DCamera * Camera3D;
    cTMap * ScreenPosX = nullptr;
    cTMap * ScreenPosY = nullptr;
    cTMap * ScreenPosZ = nullptr;

    GLuint ScreenPosXTex = -1;
    GLuint ScreenPosYTex = -1;
    GLuint ScreenPosZTex = -1;

    GLuint ScreenNormalXTex = -1;
    GLuint ScreenNormalYTex = -1;
    GLuint ScreenNormalZTex = -1;

    /*LSMVector3 CameraPosition;
    LSMVector3 CameraDir;
    LSMVector3 CameraUpDir;

    float fov;

    LSMMatrix4x4 MatrixProjection;
    LSMMatrix4x4 MatrixProjectionNoTranslation;*/

    ////
    ///
    /// ALL THE UI PARAMETERS
    ///
    ////

    //legend (if legendindex >= legendtotal, a legend should not be drawn (it is out of the display bounds anyway))
    int legendindex = 0;
    int legendtotal = 1;

    //ui scaling
    float ui_scale = 1.0;
    float ui_scalem = 1.0;
    float ui_scale2d3d = 1.0;

    //
    float ui_framewidth = 0.0;
    float ui_textscale = 0.0;
    float ui_legendwidth = 0.0;
    float ui_legendspacing = 0.0;

    //is ui present?
    bool draw_lines = false;
    bool draw_ui = true;
    bool draw_legends = true;
    bool draw_cursor = true;

    //are we drawing in 3d?
    bool is_3d = false;
    bool is_globe = false;
    int draw_lighting = 0;
    bool draw_shadows = false;

    LSMVector3 SunDir = LSMVector3(0.0,0.4,1.0);
    float m_time = 0.0;

    QList<LSMVector2> m_FocusLocations;
    QList<BoundingBox> m_FocusSquare;

    ////
    ///
    /// OPENGL RELATED STUFF
    ///
    ////

    //the frame buffer for opengl (this could be a temporary framebuffer, or the actual window, could be multi-sampling anti-alising or regular)
    OpenGLCLMSAARenderTarget* GL_3DFrameBuffer = 0;
    OpenGLCLMSAARenderTarget* GL_FrameBuffer = 0;
    OpenGLCLMSAARenderTarget* GL_PostProcessBuffer1 = 0;
    OpenGLCLMSAARenderTarget* GL_PostProcessBuffer2 = 0;
    OpenGLCLMSAARenderTarget* GL_PrimaryFrameBuffer = 0;


    ///these framebuffers are used for 3d rendering
    QList<BoundingBox> GL_FrameBuffer3DWindow;
    QList<OpenGLCLMSAARenderTarget*> GL_FrameBuffer3DColor;
    QList<OpenGLCLMSAARenderTarget*> GL_FrameBuffer3DElevation;

    OpenGLCLMSAARenderTarget* GL_FrameBuffer3DLayerColor;
    OpenGLCLMSAARenderTarget* GL_FrameBuffer3DLayerElevation;

    QList<float> m_2D3DRenderTargetScales;
    QList<BoundingBox> m_2D3DRenderTargetBoundingBox;


    ////
    ///
    /// GENERAL
    ///
    ////

    //this indicates wether no asynchronous operations may take place
    //examples are network requests
    //if this is true, the object must wait for the downloads to finish and draw before returning
    bool no_async = false;


    inline bool IsEqualForRendering(GeoWindowState s)
    {
        if(scr_ratio != s.scr_ratio)
        {
            return false;
        }
        if(scr_pixwidth != s.scr_pixwidth)
        {
            return false;
        }
        if(scr_pixheight != s.scr_pixheight)
        {
            return false;
        }
        if(scr_width != s.scr_width)
        {
            return false;
        }
        if(scr_height != s.scr_height)
        {
            return false;
        }
        if(scale != s.scale)
        {
            return false;
        }
        if(width != s.width)
        {
            return false;
        }
        if(height != height)
        {
            return false;
        }
        if(translation_x != s.translation_x)
        {
            return false;
        }
        if(translation_y != s.translation_y)
        {
            return false;
        }
        if(tlx  != s.tlx )
        {
            return false;
        }
        if(tly  != s.tly )
        {
            return false;
        }
        if(brx  != s.brx )
        {
            return false;
        }
        if(bry  != s.bry )
        {
            return false;
        }
        return true;
    }
} GeoWindowState;


