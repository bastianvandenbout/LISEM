#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 posorg;
layout(location = 3) in vec3 posorg2;

layout(location = 4) in int index;
layout(location = 5) in float attr;

out Vertex {
    vec4 Position;
    vec2 texcoord;
    flat int v_index;
    float v_attr;
} Out;

uniform int is_legend = 0;
uniform float legend_ulx;
uniform float legend_uly;
uniform float legend_sx;
uniform float legend_sy;
uniform float legend_attr;

uniform mat4 matrix;

uniform float ug_windowpixsizex;
uniform float ug_windowpixsizey;
uniform float ug_windowsizex;
uniform float ug_windowsizey;
uniform float ug_windowhwration;
uniform float ug_windowtransx;
uniform float ug_windowtransy;

uniform int is_transformed;


uniform vec4 Color1;
uniform vec4 Color2;
uniform mat4 pattern;
uniform float patterntotal;
uniform float linewidth;
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
vec2 transform_tolegendscreen(vec2 v)
{
    float tlx = legend_ulx/ug_windowpixsizex;
    float tly = legend_uly/ug_windowpixsizey;
    float brx = (legend_ulx + legend_sx)/ug_windowpixsizex;
    float bry = (legend_uly + legend_sy)/ug_windowpixsizey;

    vec2 ret;
    ret.x = (2.0 * (tlx + v.x * (brx - tlx)) - 1.0);
    ret.y = (2.0 * (tly + v.y * (bry - tly)) - 1.0);

    return ret;
}

void main() {
    Out.texcoord = tex;

    vec4 posn = vec4(pos.xy,0.0,1.0);// * matrix;
    vec4 posorgn = vec4(posorg.xy,0.0,1.0);// * matrix;
    vec4 posorgn2 = vec4(posorg2.xy,0.0,1.0);// * matrix;
    vec4 posorgnn = vec4(posorg.xy,0.0,1.0);// * matrix;

    if(is_legend > 0.5)
    {
        posn.xy = transform_tolegendscreen(posn.xy);
        posorgn.xy = transform_tolegendscreen(posorgn.xy);
        posorgn2.xy = transform_tolegendscreen(posorgn2.xy);
        posorgnn.xy = transform_tolegendscreen(posorgn.xy);

    }else
    {
        posn.xy = transform_toscreen(posn.xy);
        posorgn.xy = transform_toscreen(posorgn.xy);
        posorgn2.xy = transform_toscreen(posorgn2.xy);
        posorgnn.xy = transform_toscreen(posorgn.xy);

    }

    if(is_relative > 0)
    {


        vec2 dif =((posn.xy-posorgn.xy));
        float difnorm = distance(posn.xy,posorgn.xy);

        vec2 dif2 =((posn.xy-posorgn2.xy));
        float difnorm2 = distance(posn.xy,posorgn2.xy);
        vec2 dif2or = (posorgn.xy- posorgn2.xy);
        float difnormor = distance(posorgn.xy,posorgn2.xy);

        vec2 pos_scaled2;
        if(difnorm2 > 1e-20)
        {
            if(difnorm > 1-20)
            {
                pos_scaled2 = posorgn2.xy + ((dif2)/ difnormor) *relative_size*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));
                posorgnn.xy = posorgn2.xy + ((dif2or)/ difnormor) *relative_size*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));;
            }else
            {
                pos_scaled2 = posorgn2.xy + ((dif2)/ difnormor) *relative_size*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));
                posorgnn.xy = posorgn2.xy + ((dif2or)/ difnormor) *relative_size*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));;
            }
        }else
        {
            pos_scaled2 = posorgn2.xy;
            posorgnn.xy = posorgn2.xy;
        }
        posn.xy = pos_scaled2.xy;


        dif =((posn.xy-posorgnn.xy));
        difnorm = distance(posn.xy,posorgnn.xy);


        vec2 pos_scaled;
        if(difnorm > 1e-20)
        {
            pos_scaled = posorgnn.xy + (dif/difnorm)*linewidth*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));
        }else
        {
            pos_scaled = posorgnn.xy;
        }
        posn.xy = pos_scaled.xy;


    }else
    {
        vec2 dif =((posn.xy-posorgn.xy));
        float difnorm = distance(posn.xy,posorgn.xy);

        vec2 pos_scaled;
        if(difnorm > 0.000002)
        {
            pos_scaled = posorgn.xy + (dif/difnorm)*linewidth*(vec2(2.0/ug_windowpixsizex,2.0/ug_windowpixsizey));
        }else
        {
            pos_scaled = posorgn.xy;
        }

        posn.xy = pos_scaled.xy;

    }


    Out.v_index = index;
    if(is_legend > 0.5)
    {
        Out.v_attr = legend_attr;
    }else
    {
        Out.v_attr = attr;
    }


    gl_Position = vec4(posn.x, posn.y,0.0,1.0);
    Out.Position = vec4(posn.x, posn.y,pos.z * ug_windowpixsizex/ug_windowsizex,1.0);

}
