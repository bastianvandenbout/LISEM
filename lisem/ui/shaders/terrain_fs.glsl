#version 400

uniform highp mat4 CMatrix;
uniform highp vec2 viewportSize;
uniform highp vec3 CameraPosition;
uniform highp vec2 TerrainSize;
uniform highp vec2 TerrainSizeL;
uniform highp float pixelsPerTriangleEdge = 10.0;
uniform highp float ZScale = 1.0;

uniform sampler2D TextureC;
uniform sampler2D TextureD;

uniform int DrawLights = 0;
uniform int DrawShadows = 0;

layout (location = 0) out vec4 frag_colour;

layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

layout (location = 4) out vec4 frag_normal;
layout (location = 5) out vec4 frag_props;
layout (location = 6) out vec4 frag_Light;
layout (location = 7) out vec4 frag_Velocity;


in wireFrameVertex {
    vec4 worldPosition;
    vec4 position;
    vec3 normal;
    vec4 color;
    vec3 patchdist;
    vec3 tridist;
    float tess_level;
    float alpha;
    flat float is_edge;
    flat float edge_elev;
} VIN;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

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
    return ZScale * texture(TextureD,texcoord).r;
}

highp vec3 GetNormal(highp vec4 wpos)
{

   highp vec2 texcoord = GetTexCoords(wpos) + vec2(0.0005,0.0005);
   float ratio = TerrainSize.x/max(0.0001f,TerrainSize.y*(TerrainSize.y < 0.0?-1.0:1.0));
   highp vec2 texcoord_x = texcoord + vec2(-0.001,0.0);
   highp vec2 texcoord_y = texcoord + vec2(0.0,-0.001 * ((ratio < 0.0)? -ratio:ratio));
   float elev = ZScale * texture(TextureD,texcoord).r;
   float elev_x = ZScale * texture(TextureD,texcoord_x).r;
   float elev_y = ZScale * texture(TextureD,texcoord_y).r;

   if(!(elev > -1e20f && elev < 1e20f))
   {
           return vec3(0.0,1.0,0.0);
   }
   if(!(elev_x > -1e20f && elev_x < 1e20f))
   {
           return vec3(0.0,1.0,0.0);
   }
   if(!(elev_y > -1e20f && elev_y < 1e20f))
   {
           return vec3(0.0,1.0,0.0);
   }
   float dzdx = (elev-elev_x)/(0.001f*TerrainSize.x);
   float dzdy = (elev-elev_y)/(0.001f*TerrainSize.x);

   return normalize(vec3(dzdx,-1.0,dzdy));
}



void main() {

    if((VIN.worldPosition.x > -TerrainSizeL.x*0.485 ) && (VIN.worldPosition.x < TerrainSizeL.x*0.485 ) && (VIN.worldPosition.z > -TerrainSizeL.y*0.485) && (VIN.worldPosition.z < TerrainSizeL.y * 0.485 ))
    {
        discard;
    }

    float d1 = min(min(VIN.patchdist.x, VIN.patchdist.y), VIN.patchdist.z);
    float d2 = min(min(VIN.tridist.x, VIN.tridist.y), VIN.tridist.z);

    vec2 texcoord = GetTexCoords(VIN.worldPosition);

    float elevation = GetElevation(VIN.worldPosition);

    if(elevation < -1e20 && VIN.is_edge < 0.5f)
    {
           discard;
    }
    float grayscale = VIN.worldPosition.y/1000.0;
    //frag_colour = vec4(grayscale,grayscale,grayscale,1.0);
    vec3 color = VIN.is_edge > 0.5f? vec3(0.45,0.45,0.45) +(VIN.worldPosition.y/VIN.edge_elev)*vec3(0.45,0.45,0.45):texture(TextureC,texcoord).rgb;
    frag_colour = vec4(color,1.0) ;
    //frag_colour = vec4(texcoord.x,texcoord.y,0.0,1.0);//vec4(amplify(d1, 40, -0.5) * amplify(d2, 20, -0.5) *vec3(1.0,0.0,0.0),1.0);

    frag_posx = vec4(VIN.worldPosition.z,VIN.worldPosition.z,VIN.worldPosition.z,1.0);
    frag_posy = vec4(elevation,elevation,elevation,1.0);
    frag_posz = vec4(VIN.worldPosition.x,VIN.worldPosition.x,VIN.worldPosition.x,1.0) ;

    frag_normal = vec4(VIN.is_edge > 0.5f? VIN.normal : GetNormal(VIN.worldPosition),1.0);
    frag_props = vec4(0.6,0.6,VIN.is_edge > 0.5f? 2.0 : 1.0,1.0);
    frag_Light = vec4(1.0,1.0,1.0,1.0);
    frag_Velocity = vec4(0.0,0.0,0.0,1.0);



}
