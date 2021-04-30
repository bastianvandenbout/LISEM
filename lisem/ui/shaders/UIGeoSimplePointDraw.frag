#version 330

in vec2 texcoord;
in vec2 Position;
out vec4 fragColor;


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

uniform vec4 Color1;
uniform vec4 Color2;
uniform float linewidth;
uniform float lineangle;
uniform float lineseperation;
uniform int type;
uniform int is_relative;
uniform float relative_size;


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

void main()
{
    vec2 loc_pix = ((Position/2.0)+vec2(0.5,0.5))*vec2(ug_windowpixsizex,ug_windowpixsizey);
    vec2 dir_line_perp = vec2(-cos(lineangle*3.14159/180),sin(lineangle*3.14159/180));
    float line_dot = dot(loc_pix,dir_line_perp);

    vec4 sampled;
        sampled = Color1;
    fragColor = vec4(sampled.xyz,sampled.w * alpha);
}
