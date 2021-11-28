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
    vec3 normal;
} In[];

out wireFrameVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    highp vec3 normal;
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
	float elev = ZScale * texture(TextureD,texcoord).r;
	//if(elev > -1e20f && elev < 1e20f)
	//{
		return elev;
	//}else{
	//return 0.0f;
	//}
}

out vec3 texcoords;

void main() {

    vec3 referencepos = In[0].referencepos;
    vec4 position0 = In[0].worldPosition;
    vec4 position1 = In[1].worldPosition;
    vec4 position2 = In[2].worldPosition;
	position0.y = GetElevation(position0);
	position1.y = GetElevation(position1);
	position2.y = GetElevation(position2);

	if(position0.y > -1e20f || position1.y >  -1e20f || position2.y >  -1e20f )
    {
		float elev = 0.0f;
		float nelev = 0.0f;
		if(position0.y > -1e20f)
		{
			elev += position0.y;
			nelev += 1.0f;
		}
		if(position1.y > -1e20f)
		{
			elev += position1.y;
			nelev += 1.0f;
		}
		if(position2.y > -1e20f)
		{
			elev += position2.y;
			nelev += 1.0f;
		}
		
		if(nelev > 0.5f)
		{
			if(position0.y < -1e20f)
			{
				position0.y = elev/nelev;
			}
			if(position1.y < -1e20f)
			{
				position1.y = elev/nelev;
			}
			if(position2.y < -1e20f)
			{
				position2.y = elev/nelev;
			}
		}
		
	}
	
    if(position0.y > -1e20f && position1.y >  -1e20f && position2.y >  -1e20f )
    {
        vec3 normal = normalize(cross(position1.rgb-position0.rgb,position2.rgb-position0.rgb));

        Out.worldPosition = position0;
        Out.position = CMatrix *position0;
        Out.color = In[0].color;
        Out.normal = In[0].normal;
        Out.patchdist = In[0].patchdist;
        Out.tridist = vec3(1.0,0.0,0.0);
        Out.alpha = In[0].alpha;
        Out.tess_level = In[0].tess_level;
        gl_Position = CMatrix *position0; EmitVertex();

        Out.worldPosition = position1;
        Out.position = CMatrix *position1;
        Out.color = In[1].color;
        Out.normal = In[1].normal;
        Out.patchdist = In[1].patchdist;
        Out.tridist = vec3(0.0,1.0,0.0);
        Out.alpha = In[1].alpha;
        Out.tess_level = In[1].tess_level;
        gl_Position = CMatrix *position1; EmitVertex();

        Out.worldPosition = position2;
        Out.position = CMatrix *position2;
        Out.color = In[2].color;
        Out.normal = In[2].normal;
        Out.patchdist = In[2].patchdist;
        Out.tridist = vec3(0.0,0.0,1.0);
        Out.alpha = In[2].alpha;
        Out.tess_level = In[2].tess_level;
        gl_Position = CMatrix *position2; EmitVertex();


        EndPrimitive();
    }

    if(position0.y < -1e20f && position1.y >  -1e20f && position2.y >  -1e20f )
    {

        {
            vec4 temp0 = position1;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position2;
            temp1.y =0.0f;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position1;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position2;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position2;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position1;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }

    if(position0.y > -1e20f && position1.y <  -1e20f && position2.y >  -1e20f )
    {

        {
            vec4 temp0 = position0;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position2;
            temp1.y =0.0f;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position0;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position2;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position2;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = In[2].normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position0;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }

    if(position0.y > -1e20f && position1.y >  -1e20f && position2.y <  -1e20f )
    {

        {
            vec4 temp0 = position0;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position1;
            temp1.y =0.0f;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position0;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position1;
            temp0.y =0.0f;
            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp0; EmitVertex();

            vec4 temp1 = position1;
            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[1].color;
            Out.normal = In[1].normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            gl_Position = CMatrix *temp1; EmitVertex();

            vec4 temp2 = position0;
            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = In[0].normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }
    //detect edges, extrapolate triangel to direct neighbors of same size (so same tesselation is assumed)




}
