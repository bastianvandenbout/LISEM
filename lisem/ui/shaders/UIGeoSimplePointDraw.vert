#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 posorg2;

out vec2 texcoord;
out vec2 Position;

uniform mat4 matrix;

uniform float ug_windowpixsizex;
uniform float ug_windowpixsizey;
uniform float ug_windowsizex;
uniform float ug_windowsizey;
uniform float ug_windowhwration;
uniform float ug_windowtransx;
uniform float ug_windowtransy;

uniform int is_transformed;
uniform int is_legend;

uniform vec4 Color1;
uniform vec4 Color2;
uniform float linewidth;
uniform float lineangle;
uniform float lineseperation;
uniform int is_relative;
uniform float relative_size;

vec2 transform_toscreen(vec2 v)
{
    float tlx = ug_windowtransx - 0.5f * ug_windowsizex;
    float tly = ug_windowtransy - 0.5f * ug_windowsizey;
    float brx = ug_windowtransx + 0.5f * ug_windowsizex;
    float bry = ug_windowtransy + 0.5f * ug_windowsizey;

    vec2 ret;
    ret.x = (2.0 * (v.x - tlx)/(ug_windowsizex) - 1.0);
    ret.y = (2.0 * (v.y - tly)/(ug_windowsizey) - 1.0);

    return ret;
}

void main() {
    texcoord = tex;

    vec4 posn = vec4(pos,1.0);// * matrix;
    vec4 posorgn = vec4(posorg2.xy,0.0,1.0);// * matrix;

    posn.xy = transform_toscreen(posn.xy);

    gl_Position = posn;
    Position = posn.xy;


}
