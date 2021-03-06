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
    vec3 normal;
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

highp vec3 GetNormal(highp vec4 wpos)
{

    highp vec2 texcoord = GetTexCoords(wpos) + vec2(0.00005,0.00005);
   highp vec2 texcoord_x = texcoord + vec2(-0.0001,0.0);
   highp vec2 texcoord_y = texcoord + vec2(0.0,-0.0001);
   float elev = ZScale * texture(TextureD,texcoord).r;
   float elev_x = ZScale * texture(TextureD,texcoord_x).r;
   float elev_y = ZScale * texture(TextureD,texcoord_y).r;

    if(!(elev > -1e20f && elev < 1e20f))
    {
            return vec3(0.0,-1.0,0.0);
    }
    if(!(elev_x > -1e20f && elev_x < 1e20f))
    {
            return vec3(0.0,-1.0,0.0);
    }
    if(!(elev_y > -1e20f && elev_y < 1e20f))
    {
            return vec3(0.0,-1.0,0.0);
    }

    float dzdx = (elev-elev_x)/0.0001;
    float dzdy = (elev-elev_y)/0.0001;

    return normalize(vec3(dzdx,-1.0,dzdy));



}


void main() {
    highp vec3 position = gl_TessCoord.x*In[0].position + gl_TessCoord.y*In[1].position + gl_TessCoord.z*In[2].position;

    position.x = position.x*TerrainSize.x*0.5;
    position.z = position.z*TerrainSize.y*0.5;
    position.y = GetElevation(vec4(position,1.0));

    Out.normal = GetNormal(vec4(position,1.0));

    //y_y = GetElevation(vec4(position,1.0));
    //y_x = GetElevation(vec4(position,1.0));


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
