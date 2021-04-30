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

layout(vertices = 3) out;
in terrainVertex {
   highp vec3 position;
} In[];

out terrainVertex {
   highp vec3 position;
   flat highp float upscale;
   flat highp vec3 referencepos;
   float tess_level;
   float flat_scale;
} Out[];



const mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );

vec2 hash( vec2 p ) {
        p = vec2(dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)));
        return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p ) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
        vec2 i = floor(p + (p.x+p.y)*K1);
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    vec2 b = a - o + K2;
        vec2 c = a - 1.0 + 2.0*K2;
    vec3 h = max(0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
        vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot(n, vec3(70.0));
}

float fbm(vec2 n) {
        float total = 0.0, amplitude = 0.1;
        for (int i = 0; i < 7; i++) {
                total += noise(n) * amplitude;
                n = m * n;
                amplitude *= 0.4;
        }
        return total;
}

mat2 m2 = mat2(1.6,-1.2,1.2,1.6);


float waterlevel = 0.0;        // height of the water
float wavegain   = 1.0;       // change to adjust the general water wave level
float large_waveheight = 1.0; // change to adjust the "heavy" waves (set to 0.0 to have a very still ocean :)
float small_waveheight = 1.0; // change to adjust the small waves


// this calculates the water as a height of a given position
float water( vec2 p )
{
  float height = waterlevel;

  vec2 shift1 = 0.001*vec2( iTime*160.0*2.0, iTime*120.0*2.0 );
  vec2 shift2 = 0.001*vec2( iTime*190.0*2.0, -iTime*130.0*2.0 );

  // coarse crossing 'ocean' waves...
  float wave = 0.0;
  wave += sin(p.x*0.021  + shift2.x)*4.5;
  wave += sin(p.x*0.0172+p.y*0.010 + shift2.x*1.121)*4.0;
  wave -= sin(p.x*0.00104+p.y*0.005 + shift2.x*0.121)*4.0;
  // ...added by some smaller faster waves...
  wave += sin(p.x*0.02221+p.y*0.01233+shift2.x*3.437)*5.0;
  wave += sin(p.x*0.03112+p.y*0.01122+shift2.x*4.269)*2.5 ;
  wave *= large_waveheight;
  wave -= fbm(p*0.004-shift2*.5)*small_waveheight*24.;
  // ...added by some distored random waves (which makes the water looks like water :)

  float amp = 6.*small_waveheight;
  shift1 *= .3;
  for (int i=0; i<7; i++)
  {
    wave -= abs(sin((noise(p*0.01+shift1)-.5)*3.14))*amp;
    amp *= .51;
    shift1 *= 1.841;
    p *= m2*0.9331;
  }

  height += wave;
  return height;
}



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
    return (clamp( l / pixelsPerTriangleEdge, 1.0, 64.0 ));
}


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
    //return water(wpos.xz);
    return ZScale*10.0 * texture(TextureH,texcoord+ vec2(1.3*iTime,-0.2*iTime)).r + ZScale*15.0 * texture(TextureH,vec2(texcoord.y,texcoord.x)/2 + vec2(iTime,-iTime)).r;
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

               float size_max = 0.0;

               vec4 clipSpacePatchCorners[3];
               for ( int i = 0; i < 3; i++ )
               {
                   vec3 position;
                   position.xz= patchCornersXZ[i].xy;
                   position.x = position.x*TerrainSize.x*0.5;
                   position.z = position.z*TerrainSize.y*0.5;


                   position.y = GetElevation(vec4(position.x,0.0,position.z,1.0));
                   clipSpacePatchCorners[i] = CMatrix *vec4(position, 1.0);
               }

               size_max = max(size_max,length(clipSpacePatchCorners[0]-clipSpacePatchCorners[2]));
               size_max = max(size_max,length(clipSpacePatchCorners[0]-clipSpacePatchCorners[1]));
               size_max = max(size_max,length(clipSpacePatchCorners[1]-clipSpacePatchCorners[2]));

               if(size_max > ZScale * iTextureScale * 100.0)
               {
                   Out[gl_InvocationID].flat_scale = 0.0;

               }else
               {
                    Out[gl_InvocationID].flat_scale = 1.0;
               }
               // Determine if we can cull this patch
              if ( isEdgeInViewFrustum( clipSpacePatchCorners[0], clipSpacePatchCorners[2] )
              || isEdgeInViewFrustum( clipSpacePatchCorners[0], clipSpacePatchCorners[1] )
              || isEdgeInViewFrustum( clipSpacePatchCorners[1], clipSpacePatchCorners[2] ))
              {
                 highp float tess_out_0 =calcTessellationLevel(clipSpacePatchCorners[2], clipSpacePatchCorners[1]);
                 highp float tess_out_1 = calcTessellationLevel(clipSpacePatchCorners[2], clipSpacePatchCorners[0]);
                 highp float tess_out_2 =  calcTessellationLevel(clipSpacePatchCorners[0], clipSpacePatchCorners[1]);
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
