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

    if(is_transformed == 0)
    {
        posn.x = posn.x * (ul_sizex/ug_windowsizex) + (2.0*ul_transx-2.0*ug_windowtransx)/ug_windowsizex;
        posn.y = posn.y * (ul_sizey/ug_windowsizey) + (2.0*ul_transy-2.0*ug_windowtransy)/ug_windowsizey;
    }
    gl_Position = posn;


}
