#version 400


uniform highp mat4 CMatrix;
uniform highp vec2 viewportSize;
uniform highp vec3 CameraPosition;
uniform highp vec2 TerrainSize;
uniform highp float pixelsPerTriangleEdge = 10.0;
uniform highp float ZScale;
uniform highp float iTextureScale;


uniform highp float SResolutionX;
uniform highp float SResolutionY;
uniform highp float iTime;

uniform sampler2D TextureH;
uniform highp sampler2D TextureN;

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

vec3 GetNormal(vec4 wpos)
{
    vec2 texcoord = ( wpos.xz+CameraPosition.xz) /iTextureScale;

    return vec3(texture(TextureN,texcoord).rgb);
}
vec4 GetColor(vec4 wpos)
{
    vec2 texcoord = ( wpos.xz+CameraPosition.xz) /iTextureScale;
    return vec4(texture(TextureH,texcoord).rgb,1.0);
}
highp float GetElevation(highp vec4 wpos)
{
    vec2 texcoord = ( wpos.xz+CameraPosition.xz) /iTextureScale;
    return ZScale*10.0 * texture(TextureH,texcoord+ vec2(1.3*iTime,-0.2*iTime)).r + ZScale*15.0 * texture(TextureH,vec2(texcoord.y,texcoord.x)/2 + vec2(iTime,-iTime)).r;
}


void main() {

   highp vec3 pos = posAttr;
   pos.x = pos.x;//*TerrainSize.x*0.5 + CameraPosition.x;
   pos.z = pos.z;//*TerrainSize.y*0.5 + CameraPosition.z;

   Out.position = pos;
}
