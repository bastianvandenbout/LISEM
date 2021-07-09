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
	float elev = ZScale * texture(TextureD,texcoord).r;
	if(elev > -1e20f && elev < 1e20f)
	{
		return elev;
	}else{
	return 0.0f;
	}
}


void main() {

   highp vec3 pos = posAttr;
   pos.x = pos.x;//*TerrainSize.x*0.5 + CameraPosition.x;
   pos.z = pos.z;//*TerrainSize.y*0.5 + CameraPosition.z;

   Out.position = pos;
}
