#version 400

uniform highp mat4 CMatrix;
uniform highp vec2 viewportSize;
uniform highp vec3 CameraPosition;
uniform highp vec2 TerrainSize;
uniform highp vec2 TerrainSizeL;
uniform highp float pixelsPerTriangleEdge = 10.0;
uniform highp float ZScale = 1.0;
uniform highp float iTextureScale;

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

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in worldVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    vec4 color;
    vec3 patchdist;
    flat float upscale;
    float alpha;
        float tess_level;
    flat highp vec3 referencepos;
} In[];

out wireFrameVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    vec4 color;
    vec3 patchdist;
    vec3 tridist;
    float tess_level;
    float alpha;
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


out vec3 texcoords;

void main() {

    vec3 referencepos = In[0].referencepos;
    vec4 position0 = In[0].worldPosition;
    vec4 position1 = In[1].worldPosition;
    vec4 position2 = In[2].worldPosition;

    if(position0.y > -1e20f && position1.y >  -1e20f && position2.y >  -1e20f )
    {
        Out.worldPosition = position0;
        Out.position = CMatrix *position0;
        Out.color = In[0].color;
        Out.patchdist = In[0].patchdist;
        Out.tridist = vec3(1.0,0.0,0.0);
        Out.alpha = In[0].alpha;
        Out.tess_level = In[0].tess_level;
        gl_Position = CMatrix *position0; EmitVertex();

        Out.worldPosition = position1;
        Out.position = CMatrix *position1;
        Out.color = In[1].color;
        Out.patchdist = In[1].patchdist;
        Out.tridist = vec3(0.0,1.0,0.0);
        Out.alpha = In[1].alpha;
        Out.tess_level = In[1].tess_level;
        gl_Position = CMatrix *position1; EmitVertex();

        Out.worldPosition = position2;
        Out.position = CMatrix *position2;
        Out.color = In[2].color;
        Out.patchdist = In[2].patchdist;
        Out.tridist = vec3(0.0,0.0,1.0);
        Out.alpha = In[2].alpha;
        Out.tess_level = In[2].tess_level;
        gl_Position = CMatrix *position2; EmitVertex();


        EndPrimitive();
    }



}
