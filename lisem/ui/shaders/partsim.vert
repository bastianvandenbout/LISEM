#version 330

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 index;

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

out vec2 screenpos;
out vec2 worldpos;

void main() {
	
        vec4 posn = vec4(pos.x, -pos.y,0.0,1.0);// * matrix;

        float tlx = ug_windowtransx - 0.5f * ug_windowsizex;
        float tly = ug_windowtransy - 0.5f * ug_windowsizey;
        float brx = ug_windowtransx + 0.5f * ug_windowsizex;
        float bry = ug_windowtransy + 0.5f * ug_windowsizey;

        posn.x = (2.0 * (posn.x - tlx)/(ug_windowsizex) - 1.0);
        posn.y = (2.0 * (posn.y - tly)/(ug_windowsizey) - 1.0);

        gl_Position = posn;

        screenpos = vec2(posn.x,posn.y);
        worldpos = vec2(index.x,index.y);
	
}
