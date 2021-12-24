#version 400

in vec3 texcoords;

layout (location = 0) out vec4 frag_colour;
layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

uniform highp vec3 CameraPosition;
uniform highp mat4 CMatrix;
uniform highp float SResolutionX;
uniform highp float SResolutionY;
uniform highp float iTime;

uniform highp vec3 iSunDir;

uniform highp float iCloudCover;
uniform highp float iCloudHeight;
uniform highp float iCloudThickness;
uniform highp int iCloudIter;
uniform int isclouds;

uniform highp vec2 cloudSize;
uniform highp float cloudRoughness;
uniform highp float cloudOpacity;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

uniform sampler2D iPosX;
uniform sampler2D iPosY;
uniform sampler2D iPosZ;

//SETTINGS//
const float timeScale = 10.0;
const float cloudScale = 0.5;
const float skyCover = 0.6; //overwritten by mouse x drag
const float softness = 0.2;
const float brightness = 1.0;
const int noiseOctaves = 8;
const float curlStrain = 3.0;
//SETTINGS//

#define COVERAGE		.5
#define THICKNESS		10.
#define ABSORPTION		1.1
#define WIND			vec3(0, 0, -iTime * 0.002)
#define FBM_FREQ		2.76434
#define SUN_DIR			iSunDir
#define STEPS			25

#define _in(T) const in T
#define _inout(T) inout T
#define _out(T) out T
#define _begin(type) type (
#define _end )
#define _mutable(T) T
#define _constant(T) const T
#define mul(a, b) (a) * (b)

float hash(
        _in(float) n
){
        return fract(sin(n)*753.5453123);
}


float noise(
        _in(vec3) x
){
        /*vec3 p = floor(x);
        vec3 f = fract(x);
        f = f*f*(3.0 - 2.0*f);

        vec2 uv = (p.xy + vec2(37.0, 17.0)*p.z) + f.xy;
        vec2 rg = textureLod( iChannel0, (uv+.5)/256., 0.).yx;
        return mix(rg.x, rg.y, f.z);*/
    vec3 p = floor(x);
            vec3 f = fract(x);
            f = f*f*(3.0 - 2.0*f);


        float n = p.x + p.y*157.0 + 113.0*p.z;
        return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                       mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
                   mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                       mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}


float fbm(
        _in(vec3) pos,
        _in(float) lacunarity
){
        vec3 p = pos;
        float
        t  = 0.51749673 * noise(p); p *= lacunarity;
        t += 0.25584929 * noise(p); p *= lacunarity;
        t += 0.12527603 * noise(p); p *= lacunarity;
        t += 0.06255931 * noise(p);

        return t;
}

float get_noise(_in(vec3) x)
{
        return fbm(x, FBM_FREQ);

}


float densityf(
        _in(vec3) pos,
        _in(vec3) offset,
        _in(float) t
){
        // signal
        vec3 p = pos * .0212242 + offset ;
        float dens = get_noise(p);

        float cov = 1. - COVERAGE;
        //dens = band (.1, .3, .6, dens);
        //dens *= step(cov, dens);
        //dens -= cov;
        dens *= smoothstep (cov, cov + .05, dens);

        return clamp(dens, 0., 1.);
}


float light(
        _in(vec3) origin
){
        const int steps = 8;
        float march_step = 0.5;

        vec3 pos = origin;
        vec3 dir_step = SUN_DIR * march_step;
        float T = 1.; // transmitance

        for (int i = 0; i < steps; i++) {
                float dens = densityf(pos, WIND, 0.);

                float T_i = exp(-ABSORPTION * dens * march_step);
                T *= T_i;
                //if (T < .01) break;

                pos += dir_step;
                if(pos.y > 40)
                {
                    break;
                }
        }

        return T;//;
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


vec4 cloud3d_raymarch( in vec3 ro, in vec3 rd, in vec3 bgcol, in ivec2 px, in vec3 suncol, in vec3 wpos)
{
     vec3 sundir = iSunDir;

        float mirror = 0.0;



    //ro.x = mod(ro.x,1e4); //for number precision stuff
    //ro.z = mod(ro.z,1e4);

    vec3 ro_org = ro;

        float lay_min = 10.0;
        float lay_max = 40.0;

    //we start far above the cloud layer, get started at the intersect
    if(ro.y < 10 && rd.y > 0.0)
    {
        float dist_y = ro.y -10;
        float l_vec = abs(dist_y)/abs(rd.y);
        ro = ro + rd * l_vec;
    }else if(ro.y > lay_max && rd.y < 0.0)
        {
                float dist_y = ro.y - lay_max;
        float l_vec = abs(dist_y)/abs(rd.y);
        ro = ro + rd * l_vec;
        }else if(!(ro.y >= 10 && ro.y <= lay_max))
        {
                return vec4(0.0,0.0,0.0,0.0);
        }else{
                ro = ro;
        }

    vec3 roc = ro;
    vec3 ro_orgnm = ro;
        int steps =  STEPS;

        float T = 1.; // transmitance
        vec3 C = vec3(0, 0, 0); // color
        float alpha = 0.;

        vec3 pos = ro;

    float ro_dif =abs(ro.y+15.0);

    float angle = min(acos(dot(rd,vec3(0.0,1.0,0.0))),acos(dot(rd,vec3(0.0,-1.0,0.0))));
    //float fac_tadd = max(0.0f,min(1.0f,(ro_dif)/15.0f)); // base the step progression on distance and angle
    //float fac_tadd2 = max(0.0f,min(1.0f,(ro_dif-15.0f)/100.0f)); // base the step progression on distance and angle
    //float tadd = max(0.05f,min (5.0,fac_tadd2/atan(angle)*0.05 + (1.0 - fac_tadd2)* (fac_tadd * 2.5f + (1.0f-fac_tadd) * 0.1f)));
        float tadd = max(0.25,1.5f*atan(angle));// * 30.0f/steps;
    float march_step = tadd;

    if(ro.y >= 5.0f && ro.y <= 45.0f)
    {
        float tadd_add = min(min(0.5f,max(0.0f,0.5 - 0.1f*(10.0f-ro.y))),max(0.0f,0.5 - 0.1f*(ro.y-40.0f)));
        tadd = tadd + tadd_add;
        march_step = tadd;
    }

    vec3 C_highlight = vec3(0.0,0.0,0.0);
    //further distance of ray marching step indicates we might want to not do this pixel (aliasing makes far-away clouds ugly anyway)
    float fac_fog = min(1.0f,max(0.0f,sqrt(length(ro.xz- ro_org.xz)/2500.0)));
    fac_fog = fac_fog;
    fac_fog = 1.0 - fac_fog;

    {
        for (int i = 0; i < steps; i++) {
                float h = float(i) / float(steps);
                float dens = fac_fog  * densityf(pos, WIND, h);


                float T_i = exp(-ABSORPTION * max(0.3,pow(abs(1.0-0.93*dot(rd,iSunDir)),0.25)) * dens * march_step);
                T *= T_i;
                if (T < .01) break;

        C_highlight +=  T *

                        (exp(h) / 1.75) *
                        dens * march_step *0.4;

                C +=  T *
                        light(pos) *
                        dens * march_step * suncol * (1.0/max(0.2,pow(abs(1.0-0.93*dot(rd,iSunDir)),0.1))) * 3.0 * suncol.b;
                alpha += (1. - T_i) * (1. - alpha);

                pos += tadd * rd;

                //
                if(length(pos - ro_org) > length(wpos - ro_org))
                {
                    break;};
                                if(pos.y >lay_max + 0.01 || pos.y < 9.990)
                                {
                                break;}
        }
    }
        return vec4(C + C_highlight, alpha);



}




vec4 GetFinalSkyColor(vec3 uv, vec2 screencoord, ivec2 pixcoord)
{
    float cloudiness = 0.4;

    vec3 color = getAtmosphericScattering(uv,normalize(iSunDir)) * pi;
    color = jodieReinhardTonemap(color);
    color = pow(color, vec3(2.2)); //Back to linear

    if(isclouds < 0.5)
    {
        return vec4(color,1.0);
    }

    vec3 colors = getAtmosphericScattering(iSunDir, iSunDir) * pi;
    colors = jodieReinhardTonemap(colors);
    colors = pow(colors, vec3(2.2)); //Back to linear

    //get worldposition

    float posx = texture(iPosX,screencoord).r;
    float posy = texture(iPosY,screencoord).r;
    float posz = texture(iPosZ,screencoord).r;


    //raymarch

    vec3 CameraPosition_mod = CameraPosition;//vec3(mod(CameraPosition.x,1e4),CameraPosition.y,mod(CameraPosition.z,1e4));

    vec4 cloudres = cloud3d_raymarch(
                vec3(CameraPosition_mod.x/iCloudThickness , (CameraPosition.y)/iCloudThickness - iCloudHeight/iCloudThickness, CameraPosition_mod.z/iCloudThickness),
                uv,
                vec3(0.0,0.0,0.0),
                pixcoord ,
                colors,
                vec3((posx + CameraPosition_mod.x)/iCloudThickness,-2 + posy/iCloudThickness - iCloudHeight/iCloudThickness,(posz + CameraPosition_mod.z/iCloudThickness)));

    return cloudres;
}


void main() {

    vec3 uv = normalize(texcoords);

    vec2 viewportCoord = vec2(gl_FragCoord.x/SResolutionX,gl_FragCoord.y/SResolutionY); //ndc is -1 to 1 in GL. scale for 0 to 1

    frag_colour = GetFinalSkyColor(uv,viewportCoord,ivec2(gl_FragCoord.x-0.5,gl_FragCoord.y-0.5));

    frag_posx = vec4(-1e30,-1e30,-1e30,0.0);
    frag_posy = vec4(-1e30,-1e30,-1e30,0.0);
    frag_posz = vec4(-1e30,-1e30,-1e30,0.0);

}
