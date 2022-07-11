#version 330

layout(location = 0) in highp vec3 pos;
layout(location = 1) in highp vec2 tex;

out highp vec2 texcoord;


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
uniform int is_legend;

uniform vec2 texcoord_ulc = vec2(0.0,0.0);
uniform vec2 texcoord_brc = vec2(1.0,1.0);


void main() {
    texcoord = texcoord_ulc + (texcoord_brc - texcoord_ulc)*tex;

    vec4 posn = vec4(pos,1.0) * matrix;

    if(is_transformed == 0)
    {
        posn.x = posn.x * (ul_sizex/ug_windowsizex) + (2.0*ul_transx-2.0*ug_windowtransx)/ug_windowsizex;
        posn.y = posn.y * (ul_sizey/ug_windowsizey) + (2.0*ul_transy-2.0*ug_windowtransy)/ug_windowsizey;
    }
    gl_Position = posn;


}
