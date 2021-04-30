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

uniform highp float SResolutionX;
uniform highp float SResolutionY;

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


float waterlevel = 0.0;        // height of the water
float wavegain   = 1.0;       // change to adjust the general water wave level
float large_waveheight = 1.0; // change to adjust the "heavy" waves (set to 0.0 to have a very still ocean :)
float small_waveheight = 1.0; // change to adjust the small waves


const vec3 sundir =  vec3(0.0,0.4,1.0);
const vec3 watercolor  = vec3(0.2, 0.25, 0.3);

const float cloudscale = 1.1;
const float speed = 0.015;
const float clouddark = 0.5;
const float cloudlight = 0.3;
const float cloudcover = 0.2;
const float cloudalpha = 8.0;
const float skytint = 0.5;
const vec3 skycolour1 = vec3(0.2, 0.4, 0.6);
const vec3 skycolour2 = vec3(0.4, 0.7, 1.0);




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


//SETTINGS//
const float timeScale = 10.0;
const float cloudScale = 0.5;
const float skyCover = 0.6; //overwritten by mouse x drag
const float softness = 0.2;
const float brightness = 1.0;
const int noiseOctaves = 8;
const float curlStrain = 3.0;
//SETTINGS//

float saturate(float num)
{
    return clamp(num,0.0,1.0);
}

float noisefuv(vec2 uv)
{
    return texture(iChannel1,uv).r;
}

vec2 rotate(vec2 uv)
{
    uv = uv + noisefuv(uv*0.2)*0.005;
    float rot = curlStrain;
    float sinRot=sin(rot);
    float cosRot=cos(rot);
    mat2 rotMat = mat2(cosRot,-sinRot,sinRot,cosRot);
    return uv * rotMat;
}

float fbm2 (vec2 uv)
{
    float rot = 1.57;
    float sinRot=sin(rot);
    float cosRot=cos(rot);
    float f = 0.0;
    float total = 0.0;
    float mul = 0.5;
    mat2 rotMat = mat2(cosRot,-sinRot,sinRot,cosRot);

    for(int i = 0;i < noiseOctaves;i++)
    {
        f += noisefuv(uv+iTime*0.00015*timeScale*(1.0-mul))*mul;
        total += mul;
        uv *= 3.0;
        uv=rotate(uv);
        mul *= 0.5;
    }
    return f/total;
}

vec2 GetCloudCoverAndColor(vec3 uvray, float cover)
{
    vec2 uv = normalize(uvray.xz)*tan(acos(dot(normalize(uvray),vec3(0.0,1.0,0.0))))/100.0;

    if(uvray.y < 0)
    {
      return vec2(0.0,0.0);
    }
    float bright = brightness*(1.8-cover);

    float color1 = fbm2(uv-0.5+iTime*0.00004*timeScale);
    float color2 = fbm2(uv-10.5+iTime*0.00002*timeScale);

    float clouds1 = smoothstep(1.0-cover,min((1.0-cover)+softness*2.0,1.0),color1);
    float clouds2 = smoothstep(1.0-cover,min((1.0-cover)+softness,1.0),color2);

    float cloudsFormComb = saturate(clouds1+clouds2);

    vec4 skyCol = vec4(0.0,0.8,1.0,1.0);
    float cloudCol = saturate(saturate(1.0-pow(color1,1.0)*0.2)*bright);
    vec4 clouds1Color = vec4(cloudCol,cloudCol,cloudCol,1.0);
    vec4 clouds2Color = mix(clouds1Color,skyCol,0.25);
    vec4 cloudColComb = mix(clouds1Color,clouds2Color,saturate(clouds2-clouds1));

    float cloudfac =  mix(skyCol,cloudColComb,cloudsFormComb).r;

    return vec2(cloudfac,cloudCol);
}


vec2 GetCloudCoverAndColor2D(vec2 uv, float cover)
{

    float bright = brightness*(1.8-cover);

    float color1 = fbm(uv-0.5+iTime*0.00004*timeScale);
    float color2 = fbm(uv-10.5+iTime*0.00002*timeScale);

    float clouds1 = smoothstep(1.0-cover,min((1.0-cover)+softness*2.0,1.0),color1);
    float clouds2 = smoothstep(1.0-cover,min((1.0-cover)+softness,1.0),color2);

    float cloudsFormComb = saturate(clouds1+clouds2);

    vec4 skyCol = vec4(0.0,0.8,1.0,1.0);
    float cloudCol = saturate(saturate(1.0-pow(color1,1.0)*0.2)*bright);
    vec4 clouds1Color = vec4(cloudCol,cloudCol,cloudCol,1.0);
    vec4 clouds2Color = mix(clouds1Color,skyCol,0.25);
    vec4 cloudColComb = mix(clouds1Color,clouds2Color,saturate(clouds2-clouds1));

    float cloudfac =  mix(skyCol,cloudColComb,cloudsFormComb).r;

    return vec2(cloudfac,cloudCol);
}



const float pi = 3.14159265359;
const float invPi = 1.0 / pi;

const float multiScatterPhase = 0.1;
const float density = 0.7;

const float anisotropicIntensity = 0.0; //Higher numbers result in more anisotropic scattering

const vec3 skyColor = vec3(0.39, 0.57, 1.0) * (1.0 + anisotropicIntensity); //Make sure one of the conponents is never 0.0

#define smooth(x) x*x*(3.0-2.0*x)
#define zenithDensity(x) density / pow(max(x - zenithOffset, 0.35e-2), 0.75)

vec3 getSkyAbsorption(vec3 x, float y){

        vec3 absorption = x * -y;
             absorption = exp2(absorption) * 2.0;

        return absorption;
}

float getSunPoint(vec3 p, vec3 lp){
        return smoothstep(0.03, 0.026, distance(p, lp)) * 50.0;
}

float getRayleigMultiplier(vec3 p, vec3 lp){
        return 1.0 + pow(1.0 - clamp(distance(p, lp), 0.0, 1.0), 2.0) * pi * 0.5;
}

float getMie(vec3 p, vec3 lp){
        float disk = clamp(1.0 - pow(distance(p, lp), 0.1), 0.0, 1.0);

        return disk*disk*(3.0 - 2.0 * disk) * 2.0 * pi;
}

vec3 getAtmosphericScattering(vec3 p, vec3 lp){


        const float zenithOffset = 0.0;

        float zenith = zenithDensity(p.y);
        float sunPointDistMult =  clamp(length(max(lp.y + multiScatterPhase - zenithOffset, 0.0)), 0.0, 1.0);

        float rayleighMult = getRayleigMultiplier(p, lp);

        vec3 absorption = getSkyAbsorption(skyColor, zenith);
         vec3 sunAbsorption = getSkyAbsorption(skyColor, zenithDensity(lp.y + multiScatterPhase));
        vec3 sky = skyColor * zenith * rayleighMult;
        vec3 sun = getSunPoint(p, lp) * absorption;
        vec3 mie = getMie(p, lp) * sunAbsorption;

        vec3 totalSky = mix(sky * absorption, sky / (sky + 0.5), sunPointDistMult);
         totalSky += sun + mie;
             totalSky *= sunAbsorption * 0.5 + 0.5 * length(sunAbsorption);

        return totalSky;
}

vec3 jodieReinhardTonemap(vec3 c){
    float l = dot(c, vec3(0.2126, 0.7152, 0.0722));
    vec3 tc = c / (c + 1.0);

    return mix(c / (l + 1.0), tc, tc);
}



#define pi 3.14
#define lightDirection iSunDir

#define cloudSize vec2(1.,1.)
#define cloudRoughness 0.4
#define cloudOpacity 2.0
#define planetBend .0

//2d point rotation
vec2 rot(in vec2 p, in float a) {
    return p*mat2(sin(a),cos(a),-cos(a),sin(a));
}
//3d point rotation on xy then xz axes
vec3 rot(in vec3 ip, in vec2 a) {
    vec3 p = ip;
    p.yz = rot(p.yz,a.y);
    p.xz = rot(p.xz,a.x);
    return p;
}

//random float 0-1 from seed a
float hash2(float a) {
    return fract(fract(a*24384.2973)*512.34593+a*128.739623);
}
//random float 0-1 from seed p
float hash3(in vec3 p) {
    return fract(fract(p.x)*128.234+fract(p.y)*124.234+fract(fract(p.z)*128.234)+
                 fract(p.x*128.234)*18.234+fract(p.y*128.234)*18.234+fract(fract(p.z*128.234)*18.234));
}

//cloud distance function, rough and not an accurate distance
float df(vec3 ip, float i, float cn) {
    vec3 sip = ip;
    ip.y += cos(iTime*.02)*512.;//slide through y axis to simulate evaporation, thanks to Max M on Twitter for the suggestion
        ip.xz /= cloudSize;
    float ldst = abs(sip.y)-(6.+sin(cos(ip.x/24.234)+ip.z/131.5435+
                                   cos(ip.y/33.3958)+ip.x/191.9257+
                                   cos(ip.z/36.3834)+ip.y/135.3245)*6.*(1.-cn))+
                                                max(0.,.5-cn)*32.;

    ldst += (.5-pow(texture(iChannel0,ip.xz/512.).x,2.2))*6.;



    return max(.2, ldst);

    /*


    ldst += (cos((ip.x/i)/i+cos((ip.x+ldst*4.)/i*cloudRoughness+(ip.y*.39)/i)*4.0192854)*
             cos((ip.y/i)/i+cos(.8+(ip.y+ldst*4.)/i*cloudRoughness+(ip.z*.37)/i)*4.0817395)*
             cos((ip.z/i)/i+cos(1.6+(ip.z+ldst*4.)/i*cloudRoughness+(ip.x*.41)/i)*4.0243927))*i+min(3.,length(p.xz)*.05);

    return max(.2, ldst);*/
}

vec4 GetCloudRM(vec3 rd, float cloudiness)
{
    vec3 rp = vec3(CameraPosition.x/50.0,CameraPosition.y/50.0-20.0,CameraPosition.z/50.0);//vec3(0.,cos(iTime*.12945)*100.-80.,iTime*20.);
    //rough transparent object ray marching with 4 lod's for lighting
        //only 1 lod is rendered each step and lod's are switched each step for speed

    vec2 c = vec2(0.);
    float s = max(0.,abs(rp.y)-3.)/abs(rd.y),//ray-traced to start at cloud-plane
          im = 2.;
    vec3 pstart = rp+rd*s;
    for (int i = 0; i < 6; i++) {
        vec3 pd = rp+rd*s;


        float ie = pow(im = mod(im,4.)+1., 2.),
              od = df(pd, ie, i == 0? cloudiness * 0.5 : cloudiness);
        c += vec2( (df(pd+lightDirection*(ie*.33), ie,cloudiness)-od)/(ie*.33),
                    .02)*pow(max(0.01,(ie*.4-od))/(ie*.4),1.);

        s += od;
    }



    vec2 cloud2 = GetCloudCoverAndColor2D(pstart.xz/4092.0,0.9);

    return vec4(c.x,c.y,cloud2.x,cloud2.y);
}

vec4 GetFinalSkyColor(vec3 uv)
{
    float cloudiness = 0.4;

    vec3 color = getAtmosphericScattering(uv,normalize(iSunDir)) * pi;
    color = jodieReinhardTonemap(color);
    color = pow(color, vec3(2.2)); //Back to linear

    vec4 c = GetCloudRM(uv,cloudiness);

    vec3 colors = getAtmosphericScattering(iSunDir, iSunDir) * pi;
    colors = jodieReinhardTonemap(colors);
    colors = pow(colors, vec3(2.2)); //Back to linear


    vec4 diffuseColor = vec4(c.w,c.w,c.w,1.0) * c.z;
    vec4 ambientColor = vec4(colors,1.0);

    return mix(vec4(color,1.0),ambientColor*(c.z+ clamp((c.z-0.5)*0.5,0.0,0.5)) +diffuseColor*c.x * c.z,c.y* (1.5 + c.y * 20.0) *cloudOpacity);

}





layout (location = 0) out vec4 frag_colour;

layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

in wireFrameVertex {
    vec4 worldPosition;
    vec4 position;
    vec4 color;
    vec3 patchdist;
    vec3 tridist;
    float tess_level;
    float alpha;
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
    return ZScale * texture(TextureDL,texcoord).r +texture(TextureD,texcoord).r;
}

highp float GetElevationNormal(highp vec4 wpos)
{

    highp vec2 texcoord = GetTexCoords(wpos);
    return texture(TextureD,texcoord).r;
}
vec3 GetNormal(vec4 wpos)
{
    vec2 texcoord = ( wpos.xz+CameraPosition.xz) /iTextureScale;
    return normalize(ZScale*10.0 * normalize(texture(TextureN,texcoord+ vec2(1.3*iTime,-0.2*iTime)).xyz * 2.0 - 1.0) + ZScale*15.0 * normalize(texture(TextureN,vec2(texcoord.y,texcoord.x)/2.0 + vec2(iTime,-iTime)).yxz * 2.0 - 1.0));
}
highp float GetElevationLayer(highp vec4 wpos)
{
    highp vec2 texcoordw = ( wpos.xz+CameraPosition.xz) /iTextureScale;
    highp vec2 texcoord = GetTexCoords(wpos);
    float height_flow = ZScale * texture(TextureDL,texcoord).r;
    float height_wave = is_flow > 0? min(1.0,(height_flow/50.0)) * (ZScale* 10.0 * texture(TextureH,texcoordw+ vec2(1.3*iTime,-0.2*iTime)).r + ZScale*15.0 * texture(TextureH,vec2(texcoordw.y,texcoordw.x)/2.0 + vec2(iTime,-iTime)).r-5.0):0.0;

    return height_flow + height_wave+texture(TextureD,texcoord).r;
}

void main() {

    vec2 viewportCoord = vec2(gl_FragCoord.x/SResolutionX,gl_FragCoord.y/SResolutionY); //ndc is -1 to 1 in GL. scale for 0 to 1

    if((VIN.worldPosition.x > -TerrainSizeL.x*0.485 ) && (VIN.worldPosition.x < TerrainSizeL.x*0.485 ) && (VIN.worldPosition.z > -TerrainSizeL.y*0.485) && (VIN.worldPosition.z < TerrainSizeL.y * 0.485 ))
    {
        discard;
    }

    vec4 screencolor = texture(ScreenColor,viewportCoord).rgba;
    vec3 screenwpos = vec3(texture(ScreenPosX,viewportCoord).r,texture(ScreenPosY,viewportCoord).r,texture(ScreenPosZ,viewportCoord).r);

    float elevation = GetElevation(VIN.worldPosition);

    float dist_terrain = length(vec3(screenwpos.x,screenwpos.y-CameraPosition.y,screenwpos.z));
    float dist_flow = length(vec3(VIN.worldPosition.x,elevation-CameraPosition.y, VIN.worldPosition.z));

    //discard if we are too close to the normal elevation layer

    float d1 = min(min(VIN.patchdist.x, VIN.patchdist.y), VIN.patchdist.z);
    float d2 = min(min(VIN.tridist.x, VIN.tridist.y), VIN.tridist.z);

    vec2 texcoord = GetTexCoords(VIN.worldPosition);

    float grayscale = elevation/1000.0;

    vec4 color = texture(TextureC,texcoord).rgba;

    if(is_flow > 0 )
    {
        if((dist_terrain-dist_flow) < 1e-3)
        {
            //discard;
        }
        vec3 shallowwatercolor = vec3(0.1,0.9,1.0)*0.85;
        vec3 deepwatercolor = vec3(0.1, 0.4, 0.5);
        float factor_shallowwatercolor =min(0.9,(max(0.1,(dist_terrain-dist_flow)/5.0)));
        float factor_deepwatercolor =min(0.9,(max(0.0,((dist_terrain-dist_flow)-5)/35.0)));
        factor_shallowwatercolor = max(0.0,min(1.0-factor_deepwatercolor,factor_shallowwatercolor));

        vec3 normals = vec3(0.0,1.0,0.0);
        vec3 binormals = vec3(1.0,0.0,0.0);
        vec3 tanormals = vec3(0.0,0.0,1.0);

        // calculate water-mirror
        vec4 xdiff = vec4(1.0,0.0, 0.0,0.0)*0.01f;
        vec4 ydiff = vec4(0.0, 0.0,1.0,0.0)*0.01f;
        vec3 wpos = VIN.worldPosition.xyz + CameraPosition.xyz;
        vec3 normall = normalize(vec3((GetElevation(VIN.worldPosition-xdiff) - GetElevation(VIN.worldPosition+xdiff))*100.0f, 1.0, (GetElevation(VIN.worldPosition-ydiff) - GetElevation(VIN.worldPosition+ydiff))*100.0f));

        vec3 normaladd = GetNormal(VIN.worldPosition);
        vec3 normal = normalize(normall + normalize((normaladd.z) * normals + normaladd.x * binormals + normaladd.y * tanormals));
        vec3 eyevec = normalize(VIN.worldPosition.xyz-vec3(0.0,CameraPosition.y,0.0));
        vec3 reflect_coord = normalize(reflect(eyevec,normal));

        float reflec_coeff =0.9 *(1.0-clamp(dot(reflect_coord,normal),0.0,1.0));

        float fresnel = (0.04 + (1.0-0.04)*(pow(1.0 - max(0.0, dot(normal, reflect_coord)), 5.0)));
        vec4 screencolor = texture(ScreenColor,viewportCoord).rgba;

        vec3 skycolor = GetFinalSkyColor(reflect_coord).rgb;
        vec3 skycolors = GetFinalSkyColor(iSunDir).rgb;

        vec3 reflskycolor= vec3(0.9, 0.9, 1.0);

        vec4 wcolor = vec4(screencolor.rgb * (1.0 - factor_deepwatercolor - factor_shallowwatercolor) + factor_deepwatercolor * deepwatercolor + factor_shallowwatercolor * shallowwatercolor,factor_shallowwatercolor + factor_deepwatercolor) ;//color) ;

        vec3 finalcolor = vec3(reflskycolor * skycolor.rgb * reflec_coeff + (1.0-reflec_coeff) * wcolor.rgb );

        float sundot = clamp(dot(normalize(reflect_coord),normalize(iSunDir)),0.0,1.0);

        // Sun
        float wsunrefl = 1.0 * (0.5*pow( sundot, 40.0 )+0.25*pow( sundot, 13.5)+.75*pow( sundot, 300.0));

        float sun = pow(max(0.0, dot(reflect_coord, iSunDir)), 528.0) * 110.0;

        finalcolor += 1.75 *skycolors*wsunrefl; // sun reflection

        frag_colour = vec4( finalcolor,1.0);//vec4((dist_terrain-dist_flow)/20.0,(dist_terrain-dist_flow)/20.0,(dist_terrain-dist_flow)/20.0,1.0);//


    }else
    {
        frag_colour = color;
    }
    frag_posx = vec4(VIN.worldPosition.x,VIN.worldPosition.x,VIN.worldPosition.x,1.0);
    frag_posy = vec4(elevation,elevation,elevation,1.0);
    frag_posz = vec4(VIN.worldPosition.z,VIN.worldPosition.z,VIN.worldPosition.z,1.0) ;

}
