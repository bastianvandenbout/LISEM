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

uniform sampler2D TextureH;
uniform sampler2D TextureN;

uniform highp int is_flow;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform highp float iTextureScale;
layout( triangles, fractional_even_spacing, ccw ) in;
in terrainVertex {
    highp vec3 position;
    flat highp float upscale;
    flat highp vec3 referencepos;
        float tess_level;
} In[];

out worldVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    vec4 color;
    vec3 patchdist;
    flat float upscale;
    float alpha;
    float tess_level;
    flat highp vec3 referencepos;
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
    highp vec3 position = gl_TessCoord.x*In[0].position + gl_TessCoord.y*In[1].position + gl_TessCoord.z*In[2].position;

    position.x = position.x*TerrainSize.x*0.5;
    position.z = position.z*TerrainSize.y*0.5;
    position.y = GetElevation(vec4(position,1.0));


    /*position0.x = position0.x*TerrainSize.x*0.5;// + CameraPosition.x;
    position0.z = position0.z*TerrainSize.y*0.5;// + CameraPosition.z;
    position0.y = GetElevation(vec4(position0));

    position1.x = position1.x*TerrainSize.x*0.5;// + CameraPosition.x;
    position1.z = position1.z*TerrainSize.y*0.5;// + CameraPosition.z;
    position1.y = GetElevation(vec4(position1));

    position2.x = position2.x*TerrainSize.x*0.5;// + CameraPosition.x;
    position2.z = position2.z*TerrainSize.y*0.5;// + CameraPosition.z;
    position2.y = GetElevation(vec4(position2));*/


    gl_Position = vec4(position, 1.0);
   Out.worldPosition = vec4(position, 1.0);
   Out.position = vec4(position, 1.0);
   Out.patchdist = gl_TessCoord.xyz;
   Out.upscale = In[0].upscale;
   Out.tess_level = In[0].tess_level;//max(max(In[0].tess_level,In[1].tess_level), In[2].tess_level);
   Out.alpha = 1.0;
   Out.referencepos = In[0].referencepos;
   Out.color = vec4(0.0,1.0,1.0,1.0);
}
