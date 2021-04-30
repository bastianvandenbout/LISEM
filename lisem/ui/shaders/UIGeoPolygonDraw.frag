#version 330

in Vertex {
    vec4 Position;
    vec2 texcoord;
    flat int v_index;
    float v_attr;
} In;


out vec4 fragColor;

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

uniform float h_max;
uniform float h_min;

uniform float alpha = 1.0;


uniform vec4 Color1;
uniform vec4 Color2;
uniform float linewidth;
uniform float lineangle;
uniform float lineseperation;
uniform int type;
uniform int is_relative;
uniform float relative_size;


uniform int is_AttributeColor;
uniform float AttributeColorMin;
uniform float AttributeColorMax;


uniform float colorstop_0 = 0.0;
uniform float colorstop_1 = 0.001;
uniform float colorstop_2 = 0.01;
uniform float colorstop_3 = 0.025;
uniform float colorstop_4 = 0.05;
uniform float colorstop_5 = 0.1;
uniform float colorstop_6 = 0.25;
uniform float colorstop_7 = 0.5;
uniform float colorstop_8 = 0.75;
uniform float colorstop_9 = 1.0;
uniform vec4 colorstop_c0 = vec4(0.9019,0.9764,1.0,0.0);
uniform vec4 colorstop_c1 = vec4(0.9019,0.9764,1.0,0.25);
uniform vec4 colorstop_c2 = vec4(0.0,1.0,1.0,0.5);
uniform vec4 colorstop_c3 = vec4(0.0,0.8,1.0,0.75);
uniform vec4 colorstop_c4 = vec4(0.0,0.6,1.0,1.0);
uniform vec4 colorstop_c5 = vec4(0.0,0.2,0.8,1.0);
uniform vec4 colorstop_c6 = vec4(0.0,0.0,0.5,1.0);
uniform vec4 colorstop_c7 = vec4(1.0,0.0,0.0,1.0);
uniform vec4 colorstop_c8 = vec4(0.5,0.0,0.5,1.0);
uniform vec4 colorstop_c9 = vec4(0.0,0.0,0.0,1.0);

uniform float colorstop2_0 = 0.0;
uniform float colorstop2_1 = 0.001;
uniform float colorstop2_2 = 0.01;
uniform float colorstop2_3 = 0.025;
uniform float colorstop2_4 = 0.05;
uniform float colorstop2_5 = 0.1;
uniform float colorstop2_6 = 0.25;
uniform float colorstop2_7 = 0.5;
uniform float colorstop2_8 = 0.75;
uniform float colorstop2_9 = 1.0;
uniform vec4 colorstop2_c0 = vec4(0.9019,0.9764,1.0,0.0);
uniform vec4 colorstop2_c1 = vec4(0.9019,0.9764,1.0,0.25);
uniform vec4 colorstop2_c2 = vec4(0.0,1.0,1.0,0.5);
uniform vec4 colorstop2_c3 = vec4(0.0,0.8,1.0,0.75);
uniform vec4 colorstop2_c4 = vec4(0.0,0.6,1.0,1.0);
uniform vec4 colorstop2_c5 = vec4(0.0,0.2,0.8,1.0);
uniform vec4 colorstop2_c6 = vec4(0.0,0.0,0.5,1.0);
uniform vec4 colorstop2_c7 = vec4(1.0,0.0,0.0,1.0);
uniform vec4 colorstop2_c8 = vec4(0.5,0.0,0.5,1.0);
uniform vec4 colorstop2_c9 = vec4(0.0,0.0,0.0,1.0);


vec4 GetColorFromRamp(float val)
{
        if(val > colorstop_0)
        {
                if(val < colorstop_1)
                {
                        return mix(colorstop_c0,colorstop_c1,(val - colorstop_0)/(colorstop_1 - colorstop_0));
                }else if(val < colorstop_2)
                {
                        return mix(colorstop_c1,colorstop_c2,(val - colorstop_1)/(colorstop_2 - colorstop_1));
                }else if(val < colorstop_3)
                {
                        return mix(colorstop_c2,colorstop_c3,(val - colorstop_2)/(colorstop_3 - colorstop_2));
                }else if(val < colorstop_4)
                {
                        return mix(colorstop_c3,colorstop_c4,(val - colorstop_3)/(colorstop_4 - colorstop_3));
                }else if(val < colorstop_5)
                {
                        return mix(colorstop_c4,colorstop_c5,(val - colorstop_4)/(colorstop_5 - colorstop_4));
                }else if(val < colorstop_6)
                {
                        return mix(colorstop_c5,colorstop_c6,(val - colorstop_5)/(colorstop_6 - colorstop_5));
                }else if(val < colorstop_7)
                {
                        return mix(colorstop_c6,colorstop_c7,(val - colorstop_6)/(colorstop_7 - colorstop_6));
                }else if(val < colorstop_8)
                {
                        return mix(colorstop_c7,colorstop_c8,(val - colorstop_7)/(colorstop_8 - colorstop_7));
                }else if(val < colorstop_9)
                {
                        return mix(colorstop_c8,colorstop_c9,(val - colorstop_8)/(colorstop_9 - colorstop_8));
                }else
                {
                        return colorstop_c9;
                }
        }else
        {
        return colorstop_c0;
        }
}


vec4 GetColorFromRamp2(float val)
{
        if(val > colorstop2_0)
        {
                if(val < colorstop2_1)
                {
                        return mix(colorstop2_c0,colorstop2_c1,(val - colorstop2_0)/(colorstop2_1 - colorstop2_0));
                }else if(val < colorstop2_2)
                {
                        return mix(colorstop2_c1,colorstop2_c2,(val - colorstop2_1)/(colorstop2_2 - colorstop2_1));
                }else if(val < colorstop2_3)
                {
                        return mix(colorstop2_c2,colorstop2_c3,(val - colorstop2_2)/(colorstop2_3 - colorstop2_2));
                }else if(val < colorstop2_4)
                {
                        return mix(colorstop2_c3,colorstop2_c4,(val - colorstop2_3)/(colorstop2_4 - colorstop2_3));
                }else if(val < colorstop2_5)
                {
                        return mix(colorstop2_c4,colorstop2_c5,(val - colorstop2_4)/(colorstop2_5 - colorstop2_4));
                }else if(val < colorstop2_6)
                {
                        return mix(colorstop2_c5,colorstop2_c6,(val - colorstop2_5)/(colorstop2_6 - colorstop2_5));
                }else if(val < colorstop2_7)
                {
                        return mix(colorstop2_c6,colorstop2_c7,(val - colorstop2_6)/(colorstop2_7 - colorstop2_6));
                }else if(val < colorstop2_8)
                {
                        return mix(colorstop2_c7,colorstop2_c8,(val - colorstop2_7)/(colorstop2_8 - colorstop2_7));
                }else if(val < colorstop2_9)
                {
                        return mix(colorstop2_c8,colorstop2_c9,(val - colorstop2_8)/(colorstop2_9 - colorstop2_8));
                }else
                {
                        return colorstop2_c9;
                }
        }else
        {
        return colorstop2_c0;
        }
}



void main()
{
    vec2 loc_pix = ((In.Position.xy/2.0)+vec2(0.5,0.5))*vec2(ug_windowpixsizex,ug_windowpixsizey);
    vec2 dir_line_perp = vec2(-cos(lineangle*3.14159/180),sin(lineangle*3.14159/180));
    float line_dot = dot(loc_pix,dir_line_perp);


    float h_norm = (In.v_attr - AttributeColorMin)/(AttributeColorMax -AttributeColorMin);

    vec4 Color_s1 = Color1;
    vec4 Color_s2 = Color2;

    if(is_AttributeColor > 0.5)
    {
        Color_s1 = GetColorFromRamp(h_norm);
        Color_s2 = GetColorFromRamp2(h_norm);
    }

    //fragColor = vec4(h_norm,h_norm,h_norm,1.0);

    vec4 sampled;
    if(type == 1)
    {
        sampled = mod(line_dot,lineseperation) > lineseperation -linewidth? Color_s2:Color_s1;
    }else
    {
        sampled = Color_s1;
    }
    fragColor = vec4(sampled.xyz,sampled.w * alpha);
}
