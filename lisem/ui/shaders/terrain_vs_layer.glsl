#version 400


uniform highp mat4 CMatrix;
uniform highp vec2 viewportSize;
uniform highp vec3 CameraPosition;
uniform highp vec2 TerrainSize;
uniform highp vec2 TerrainSizeL;
uniform highp float pixelsPerTriangleEdge = 10.0;
uniform highp float ZScale = 1.0;

uniform sampler2D TextureC;
uniform highp sampler2D TextureD;
uniform highp sampler2D TextureDL;

uniform highp float iTime;
uniform highp vec3 iSunDir;

uniform highp sampler2D ScreenColor;
uniform highp sampler2D ScreenPosX;
uniform highp sampler2D ScreenPosY;
uniform highp sampler2D ScreenPosZ;

uniform highp int is_flow;

uniform sampler2D TextureH;
uniform sampler2D TextureN;


uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

uniform highp float iTextureScale;
layout(location = 0) in highp vec3 posAttr;
layout(location = 1) in highp vec2 UVAttr;
layout(location = 2) in highp vec3 normalAttr;
layout(location = 3) in highp vec3 tangentAttr;
layout(location = 4) in highp vec3 bitangentAttr;
layout(location = 5) in highp vec3 colorAttr;

out terrainVertex {
    vec3 position;
} Out;


highp vec2 GetTexCoords(highp vec4 wpos)
{
    highp vec2 texcoord = (vec2(wpos.x,wpos.z));
    texcoord.x = 0.5 + texcoord.x/TerrainSize.x;
    texcoord.y = 0.5 + texcoord.y/TerrainSize.y;

    return texcoord;
}

vec4 GetColor(vec4 wpos)
{
    vec2 texcoord = GetTexCoords(wpos);
    return vec4(texture(TextureC,texcoord).rgb,1.0);
}
highp float GetElevation(highp vec4 wpos)
{
    highp vec2 texcoord = GetTexCoords(wpos);
    highp vec2 texcoordw = ( wpos.xz+CameraPosition.xz) /iTextureScale;
    float height_flow = ZScale * texture(TextureDL,texcoord).r;
    float height_wave = is_flow > 0? min(1.0,(height_flow/50.0)) * (ZScale* 10.0 * texture(TextureH,texcoordw+ vec2(1.3*iTime,-0.2*iTime)).r + ZScale*15.0 * texture(TextureH,vec2(texcoordw.y,texcoordw.x)/2.0 + vec2(iTime,-iTime)).r-5.0):0.0;

    return height_flow + height_wave +texture(TextureD,texcoord).r;

 }


void main() {

   highp vec3 pos = posAttr;
   pos.x = pos.x;//*TerrainSize.x*0.5 + CameraPosition.x;
   pos.z = pos.z;//*TerrainSize.y*0.5 + CameraPosition.z;

   Out.position = pos;
}
