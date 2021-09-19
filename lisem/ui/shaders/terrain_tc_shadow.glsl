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

layout(vertices = 3) out;
in terrainVertex {
   highp vec3 position;
} In[];

out terrainVertex {
   highp vec3 position;
   flat highp float upscale;
   flat highp vec3 referencepos;
   float tess_level;
} Out[];


bool isEdgeInViewFrustum( const in vec4 p1, const in vec4 p2 )
{
   return !( ( p1.x < -p1.w && p2.x < -p2.w )
           || ( p1.x >  p1.w && p2.x >  p2.w )
           || ( p1.y < -p1.w && p2.y < -p2.w )
           || ( p1.y >  p1.w && p2.y >  p2.w )
           || ( p1.z < -p1.w && p2.z < -p2.w )
           || ( p1.z >  p1.w && p2.z >  p2.w ) );
}


float calcTessellationLevelDiscrete( float in_ntess )
{
    if(in_ntess < 2.0){ return 1.0;}
    if(in_ntess < 4.0){ return 2.0;}
    if(in_ntess < 8.0){ return 4.0;}
    if(in_ntess < 16.0){ return 8.0;}
    if(in_ntess < 32.0){ return 16.0;}
        if(in_ntess < 64.0){ return 32.0;}
    return 64.0;
}

highp float calcTessellationLevel( highp vec4 v1, highp vec4 v2 )
{
    highp vec4 p1 = 0.5 * ( v1 + v2 );
    highp vec4 p2 = p1;
    p2.y += distance( v1, v2 );
    p1 = p1 / p1.w;
    p2 = p2 / p2.w;
    highp float l = max(length( 0.5 * viewportSize * ( p1.xy - p2.xy) ),length( 0.5 * viewportSize * (p2.xy - p1.xy) ) );
    return calcTessellationLevelDiscrete(clamp( l / pixelsPerTriangleEdge, 1.0, 64.0 ));
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
	float elev = ZScale * texture(TextureD,texcoord).r;
	if(elev > -1e20f && elev < 1e20f)
	{
		return elev;
	}else{
	return 0.0f;
	}
	
}


void main() {


    Out[gl_InvocationID].position = In[gl_InvocationID].position;
    Out[gl_InvocationID].upscale = 0;
    Out[gl_InvocationID].referencepos = In[0].position;


    //if(gl_InvocationID == 0)
    {
        if(false)
        {
            gl_TessLevelOuter[0] = 1;//;
            gl_TessLevelOuter[1] = 1;//;
            gl_TessLevelOuter[2] = 1;//;
            gl_TessLevelInner[0] =  5;

            Out[gl_InvocationID].tess_level =  5;

            Out[gl_InvocationID].upscale = 0;

        }else
        {
               Out[gl_InvocationID].tess_level = 0.0;

               vec2 patchCornersXZ[3];
               patchCornersXZ[0] = In[0].position.xz;
               patchCornersXZ[1] = In[1].position.xz;
               patchCornersXZ[2] = In[2].position.xz;

               bool all_out = true;
               vec4 clipSpacePatchCorners[3];
               for ( int i = 0; i < 3; i++ )
               {
                   vec3 position;
                   position.xz= patchCornersXZ[i].xy;
                   position.x = position.x*TerrainSize.x*0.5;
                   position.z = position.z*TerrainSize.y*0.5;


                   position.y = GetElevation(vec4(position.x,0.0,position.z,1.0));
                   clipSpacePatchCorners[i] = CMatrix *vec4(position, 1.0);


                   //if(!((position.x > -TerrainSizeL.x*0.485 ) && (position.x < TerrainSizeL.x*0.485 ) && (position.z > -TerrainSizeL.y*0.485) && (position.z < TerrainSizeL.y * 0.485 )))
                   {
                       all_out = false;
                   }

               }

               if(all_out)
               {
                   gl_TessLevelOuter[0] = 0.0;
                   gl_TessLevelOuter[1] = 0.0;
                   gl_TessLevelOuter[2] = 0.0;
                   gl_TessLevelInner[0] = 0.0;
               }else
               {
                   // Determine if we can cull this patch
                   if ( isEdgeInViewFrustum( clipSpacePatchCorners[0], clipSpacePatchCorners[2] )
                   || isEdgeInViewFrustum( clipSpacePatchCorners[0], clipSpacePatchCorners[1] )
                   || isEdgeInViewFrustum( clipSpacePatchCorners[1], clipSpacePatchCorners[2] ))
                    {
                       highp float tess_out_0 =calcTessellationLevel( clipSpacePatchCorners[2], clipSpacePatchCorners[1] );
                       highp float tess_out_1 = calcTessellationLevel( clipSpacePatchCorners[2], clipSpacePatchCorners[0] );
                       highp float tess_out_2 =  calcTessellationLevel( clipSpacePatchCorners[0], clipSpacePatchCorners[1] );
                       highp float tess_in_0 =  max(max(tess_out_0,tess_out_1),tess_out_2);

                       gl_TessLevelOuter[0] = tess_out_0;
                       gl_TessLevelOuter[1] = tess_out_1;
                       gl_TessLevelOuter[2] = tess_out_2;
                       gl_TessLevelInner[0] = tess_in_0;

                       Out[gl_InvocationID].tess_level = tess_out_0;

                  }else
                  {
                   gl_TessLevelOuter[0] = 0.0;
                   gl_TessLevelOuter[1] = 0.0;
                   gl_TessLevelOuter[2] = 0.0;
                   gl_TessLevelInner[0] = 0.0;


                  }

               }






        }
    }
}
