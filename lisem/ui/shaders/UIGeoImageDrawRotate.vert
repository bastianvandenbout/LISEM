#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec2 texcoord;


uniform mat4 matrix;

uniform float ul_sizex;
uniform float ul_sizey;
uniform float ul_pixx;
uniform float ul_pixy;
uniform float ul_pixdx;
uniform float ul_pixdy;
uniform float ul_transx;
uniform float ul_transy;
uniform float ul_angle = 0.0f;

uniform float ug_windowpixsizex;
uniform float ug_windowpixsizey;
uniform float ug_windowsizex;
uniform float ug_windowsizey;
uniform float ug_windowhwration;
uniform float ug_windowtransx;
uniform float ug_windowtransy;

uniform int is_transformed;



void main() {
    texcoord = tex;

    vec4 posn = vec4(pos,1.0) * matrix;




    //if we do not have a simple transform, we do an expensive full-screen ivnerse transform including rotation.

    if(is_transformed == 0)
    {
        //we can rorate these positions by ul_angle, before we transform them to geo-coordinates
        //we transform them relative to the mid-point of the region, so relative to (0.5,0.5)



        posn.x = posn.x * (ul_sizex/ug_windowsizex) + (2.0*ul_transx-2.0*ug_windowtransx)/ug_windowsizex;
        posn.y = posn.y * (ul_sizey/ug_windowsizey) + (2.0*ul_transy-2.0*ug_windowtransy)/ug_windowsizey;

        float posx_ref = -0.0 * (ul_sizex/ug_windowsizex) + (2.0*ul_transx-2.0*ug_windowtransx)/ug_windowsizex;
        float posy_ref = -0.0 * (ul_sizey/ug_windowsizey) + (2.0*ul_transy-2.0*ug_windowtransy)/ug_windowsizey;

        posn.x = posn.x * ug_windowsizex - posx_ref * ug_windowsizex;
        posn.y = posn.y * ug_windowsizey - posy_ref * ug_windowsizey;

        float posxn = cos(ul_angle) * posn.x - sin(ul_angle) * posn.y;
        float posyn = sin(ul_angle) * posn.x + cos(ul_angle) * posn.y;

        posn.x = (posxn +posx_ref * ug_windowsizex)/ug_windowsizex;
        posn.y = (posyn +posy_ref * ug_windowsizey)/ug_windowsizey;
    }
    gl_Position = posn;


}
