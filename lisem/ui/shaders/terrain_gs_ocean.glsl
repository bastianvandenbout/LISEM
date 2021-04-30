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
uniform sampler2D TextureN;

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
