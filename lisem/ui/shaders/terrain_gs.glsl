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
layout( triangle_strip, max_vertices = 6 ) out;

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
    flat float is_edge;
    flat float edge_elev;
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
        //if(elev > recognize && elev < 1e20f)
	//{
		return elev;
	//}else{
	//return 0.0f;
	//}
}

out vec3 texcoords;

float distSquared( vec4 A, vec4 B )
{

    vec2 C = A.xz - B.xz;
    return dot( C, C );

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

    vec3 referencepos = In[0].referencepos;
    vec4 position0 = In[0].worldPosition;
    vec4 position1 = In[1].worldPosition;
    vec4 position2 = In[2].worldPosition;
	position0.y = GetElevation(position0);
	position1.y = GetElevation(position1);
	position2.y = GetElevation(position2);


        float recognize = -1e20f;

        float elev = 0.0f;
        float nelev = 0.0f;

        float elev_max = -1e30;

        if(position0.y > recognize || position1.y >  recognize || position2.y >  recognize )
        {
                float elev = 0.0f;
                float nelev = 0.0f;
                if(position0.y > recognize)
                {
                        elev += position0.y;
                        nelev += 1.0f;
                        elev_max = max(elev_max,position0.y);
                }
                if(position1.y > recognize)
                {
                        elev += position1.y;
                        nelev += 1.0f;
                        elev_max = max(elev_max,position1.y);
                }
                if(position2.y > recognize)
                {
                        elev += position2.y;
                        nelev += 1.0f;
                        elev_max = max(elev_max,position2.y);
                }
        }

        /*if(position0.y > recognize && position1.y >  recognize && position2.y >  recognize )
        {

            float l1 = distSquared(position0, position1);
            float l2 = distSquared(position1, position2);
            float l3 = distSquared(position2, position0);

            //check for possible in-corner triangle
            if( l1 > l2 && l1 > l3)
            {
                if(GetElevation(0.5 * (position0 + position1)) < recognize)
                {
                    position2.y = -1e31f;
                }

            }else if( l2 > l1 && l2 > l3)
            {
                if(GetElevation(0.5 * (position1 + position2)) < recognize)
                {
                    position0.y = -1e31f;
                }

            }else if( l3 > l2 && l3 > l1)
            {
                if(GetElevation(0.5 * (position2 + position0)) < recognize)
                {
                    position1.y = -1e31f;
                }

            }
        }*/

        //get half-way position on longest side

        //sample GetElevation

        //if we have three good elevations, but not on the half-way point,
        //we are looking at a corner-triangle (very likely)
        //so we have to extrude it down for the boundary.


        bool one_point = false;


        /*if(position0.y > recognize || position1.y >  recognize || position2.y >  recognize )
    {
		float elev = 0.0f;
		float nelev = 0.0f;
                if(position0.y > recognize)
		{
			elev += position0.y;
			nelev += 1.0f;
		}
                if(position1.y > recognize)
		{
			elev += position1.y;
			nelev += 1.0f;
		}
                if(position2.y > recognize)
		{
			elev += position2.y;
			nelev += 1.0f;
		}
		
                //if only one point
                if(nelev > 0.5f && nelev < 1.5f)
		{
                    bool one_point = true;
                        if(position0.y < recognize)
			{
				position0.y = elev/nelev;
			}
                        if(position1.y < recognize)
			{
				position1.y = elev/nelev;
			}
                        if(position2.y < recognize)
			{
				position2.y = elev/nelev;
			}
		}
		
        }*/


    if(position0.y > recognize && position1.y >  recognize && position2.y >  recognize )
    {
        //in this case, get detailed normal from the texture, so the normal can be blended for the final fragments


        vec3 normal = normalize(cross(position1.rgb-position0.rgb,position2.rgb-position0.rgb));

        vec3 normal0 =GetNormal(position0);
        vec3 normal1 =GetNormal(position1);
        vec3 normal2 =GetNormal(position2);

        Out.worldPosition = position0;
        Out.position = CMatrix *position0;
        Out.color = In[0].color;
        Out.normal = normal0;
        Out.patchdist = In[0].patchdist;
        Out.tridist = vec3(1.0,0.0,0.0);
        Out.alpha = In[0].alpha;
        Out.tess_level = In[0].tess_level;
        Out.is_edge = one_point?1.0:0.0;
        Out.edge_elev = elev_max;
        gl_Position = CMatrix *position0; EmitVertex();

        Out.worldPosition = position1;
        Out.position = CMatrix *position1;
        Out.color = In[1].color;
        Out.normal = normal1;
        Out.patchdist = In[1].patchdist;
        Out.tridist = vec3(0.0,1.0,0.0);
        Out.alpha = In[1].alpha;
        Out.tess_level = In[1].tess_level;
        Out.is_edge = one_point?1.0:0.0;
        Out.edge_elev = elev_max;
        gl_Position = CMatrix *position1; EmitVertex();

        Out.worldPosition = position2;
        Out.position = CMatrix *position2;
        Out.color = In[2].color;
        Out.normal = normal2;
        Out.patchdist = In[2].patchdist;
        Out.tridist = vec3(0.0,0.0,1.0);
        Out.alpha = In[2].alpha;
        Out.tess_level = In[2].tess_level;
        Out.is_edge = one_point?1.0:0.0;
        Out.edge_elev = elev_max;
        gl_Position = CMatrix *position2; EmitVertex();


        EndPrimitive();
    }

    /*if(position0.y < recognize && position1.y <  recognize && position2.y <  recognize )
    {
        vec3 normal = normalize(cross(position1.rgb-position0.rgb,position2.rgb-position0.rgb));
position0.y =0.0f;
        Out.worldPosition = position0;

        Out.position = CMatrix *position0;
        Out.color = In[0].color;
        Out.normal = In[0].normal;
        Out.patchdist = In[0].patchdist;
        Out.tridist = vec3(1.0,0.0,0.0);
        Out.alpha = In[0].alpha;
        Out.tess_level = In[0].tess_level;
        Out.is_edge = 1.0;
        gl_Position = CMatrix *position0; EmitVertex();
        position1.y =0.0f;
        Out.worldPosition = position1;

        Out.position = CMatrix *position1;
        Out.color = In[1].color;
        Out.normal = In[1].normal;
        Out.patchdist = In[1].patchdist;
        Out.tridist = vec3(0.0,1.0,0.0);
        Out.alpha = In[1].alpha;
        Out.tess_level = In[1].tess_level;
        Out.is_edge = 1.0;
        gl_Position = CMatrix *position1; EmitVertex();
        position2.y =0.0f;
        Out.worldPosition = position2;

        Out.position = CMatrix *position2;
        Out.color = In[2].color;
        Out.normal = In[2].normal;
        Out.patchdist = In[2].patchdist;
        Out.tridist = vec3(0.0,0.0,1.0);
        Out.alpha = In[2].alpha;
        Out.tess_level = In[2].tess_level;
        Out.is_edge = 1.0;
        gl_Position = CMatrix *position2; EmitVertex();


        EndPrimitive();
    }*/

    if(position0.y < recognize && position1.y >  recognize && position2.y >  recognize )
    {

        {
            vec4 temp0 = position1;
            temp0.y =0.0f;
            vec4 temp1 = position2;
            temp1.y =0.0f;
            vec4 temp2 = position1;
            vec3 normal = -normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));


            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position2;
            vec4 temp1 = position2;
            temp1.y =0.0f;
            vec4 temp2 = position1;
            vec3 normal = normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));

            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }

    if(position0.y > recognize && position1.y <  recognize && position2.y >  recognize )
    {

        {
            vec4 temp0 = position0;
            temp0.y =0.0f;

            vec4 temp1 = position2;
            temp1.y =0.0f;

            vec4 temp2 = position0;

            vec3 normal = normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));


            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position2;

            vec4 temp1 = position2;
            temp1.y =0.0f;

            vec4 temp2 = position0;
            vec3 normal = -normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));


            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[2].color;
            Out.normal = normal;
            Out.patchdist = In[2].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[2].alpha;
            Out.tess_level = In[2].tess_level;
            Out.edge_elev = elev_max;
            Out.is_edge = 1.0;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }

    if(position0.y > recognize && position1.y >  recognize && position2.y <  recognize )
    {

        {
            vec4 temp0 = position0;
            temp0.y =0.0f;

            vec4 temp1 = position1;
            temp1.y =0.0f;
            vec4 temp2 = position0;

            vec3 normal = -normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));


            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }

        {

            vec4 temp0 = position1;

            vec4 temp1 = position1;

            temp1.y =0.0f;

            vec4 temp2 = position0;
            vec3 normal = normalize(cross(temp1.rgb-temp0.rgb,temp2.rgb-temp0.rgb));


            Out.worldPosition = temp0;
            Out.position = CMatrix * temp0;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(1.0,0.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp0; EmitVertex();

            Out.worldPosition = temp1;
            Out.position = CMatrix * temp1;
            Out.color = In[1].color;
            Out.normal = normal;
            Out.patchdist = In[1].patchdist;
            Out.tridist = vec3(0.0,1.0,0.0);
            Out.alpha = In[1].alpha;
            Out.tess_level = In[1].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp1; EmitVertex();

            Out.worldPosition = temp2;
            Out.position = CMatrix *temp2;
            Out.color = In[0].color;
            Out.normal = normal;
            Out.patchdist = In[0].patchdist;
            Out.tridist = vec3(0.0,0.0,1.0);
            Out.alpha = In[0].alpha;
            Out.tess_level = In[0].tess_level;
            Out.is_edge = 1.0;
            Out.edge_elev = elev_max;
            gl_Position = CMatrix *temp2; EmitVertex();

            EndPrimitive();
        }


    }
    //detect edges, extrapolate triangel to direct neighbors of same size (so same tesselation is assumed)




}
