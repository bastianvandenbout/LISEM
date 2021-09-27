#version 400

in vec3 frag_position;
in vec2 frag_texcoords;
in vec3 frag_in_normal;

layout (location = 0) out vec4 frag_colour;
layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

layout (location = 4) out vec4 frag_normal;
layout (location = 5) out vec4 frag_props;
layout (location = 6) out vec4 frag_Light;
layout (location = 7) out vec4 frag_Velocity;

uniform highp vec3 CameraPosition;
uniform highp mat4 CMatrix;
uniform highp float SResolutionX;
uniform highp float SResolutionY;
uniform highp float iTime;

uniform highp vec3 iSunDir;

uniform highp float iCloudCover;
uniform highp float iCloudHeight;
uniform highp float iCloudThickness;
uniform highp int iCloudIter;
uniform int isclouds;

uniform highp vec2 cloudSize;
uniform highp float cloudRoughness;
uniform highp float cloudOpacity;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

uniform sampler2D iPosX;
uniform sampler2D iPosY;
uniform sampler2D iPosZ;


void main() {

    vec2 viewportCoord = vec2(gl_FragCoord.x/SResolutionX,gl_FragCoord.y/SResolutionY); //ndc is -1 to 1 in GL. scale for 0 to 1



    frag_colour = vec4(vec3(1.0,0.702,0.278) * (0.4 +0.6 * max(0.0f,dot(normalize(frag_in_normal),normalize(iSunDir)))),1.0);

    frag_posx = vec4(frag_position.x,frag_position.x,frag_position.x,1.0);
    frag_posy = vec4(frag_position.y,frag_position.y,frag_position.y,1.0);
    frag_posz = vec4(frag_position.z,frag_position.z,frag_position.z,1.0) ;

    frag_normal = vec4(normalize(frag_in_normal),1.0);
    frag_props = vec4(0.6,0.6,2,1.0);
    frag_Light = vec4(1.0,1.0,1.0,1.0);
    frag_Velocity = vec4(0.0,0.0,0.0,1.0);

}
