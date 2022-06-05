//from Shadertoy -FXAA example

#version 330

uniform sampler2D tex; //color
uniform sampler2D PosX; //position (in world space, relative to camera)
uniform sampler2D PosY;
uniform sampler2D PosZ;
uniform sampler2D Normal; //normal (in world space)
uniform sampler2D Props; //Metalness, Rougness, Object Type
uniform sampler2D Light; // albedo,metalness,roughness
uniform sampler2D Velocity;

uniform int DrawLights = 0;
uniform int DrawShadows = 0;

in vec2 texcoord;

uniform vec2 iResolution;
uniform vec2 iViewPortSize;
uniform float iTime;
uniform vec3 iSolarDir;
uniform vec2 iSolarPos;
uniform vec3 iCameraPos; //Actual camera position
uniform vec3 iCameraDir; //View Direction in world space
uniform mat4x4 CMatrix;
uniform mat4x4 CMatrixNoTranslation;
uniform mat4x4 CMatrixWorldToView;


out vec4 fragColor;

#define saturate(x) clamp(x, 0.0, 1.0)
#define PI 3.14159265359


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


vec4 GetFinalSkyColor(vec3 uv)
{
    float cloudiness = 0.4;

    vec3 color = getAtmosphericScattering(uv,normalize(iSolarDir)) * pi;
    color = jodieReinhardTonemap(color);
    color = pow(color, vec3(2.2)); //Back to linear

    return vec4(color,1.0);

}

//------------------------------------------------------------------------------
// BRDF
//------------------------------------------------------------------------------
float somestep(float t) {
    return pow(t,4.0);
}

vec3 textureSky(vec3 dir)
{

    return GetFinalSkyColor(dir).rgb;
}

vec3 textureAVG(vec3 tc) {
    const float diff0 = 0.35;
    const float diff1 = 0.12;
        vec3 s0 = textureSky(tc).xyz;
    vec3 s1 = textureSky(tc+vec3(diff0)).xyz;
    vec3 s2 = textureSky(tc+vec3(-diff0)).xyz;
    vec3 s3 = textureSky(tc+vec3(-diff0,diff0,-diff0)).xyz;
    vec3 s4 = textureSky(tc+vec3(diff0,-diff0,diff0)).xyz;

    vec3 s5 = textureSky(tc+vec3(diff1)).xyz;
    vec3 s6 = textureSky(tc+vec3(-diff1)).xyz;
    vec3 s7 = textureSky(tc+vec3(-diff1,diff1,-diff1)).xyz;
    vec3 s8 = textureSky(tc+vec3(diff1,-diff1,diff1)).xyz;

    return (s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8) * 0.111111111;
}

vec3 textureBlured(vec3 tc) {
        vec3 r = textureAVG(vec3(1.0,0.0,0.0));
    vec3 t = textureAVG(vec3(0.0,1.0,0.0));
    vec3 f = textureAVG(vec3(0.0,0.0,1.0));
    vec3 l = textureAVG(vec3(-1.0,0.0,0.0));
    vec3 b = textureAVG(vec3(0.0,-1.0,0.0));
    vec3 a = textureAVG(vec3(0.0,0.0,-1.0));

    float kr = dot(tc,vec3(1.0,0.0,0.0)) * 0.5 + 0.5;
    float kt = dot(tc,vec3(0.0,1.0,0.0)) * 0.5 + 0.5;
    float kf = dot(tc,vec3(0.0,0.0,1.0)) * 0.5 + 0.5;
    float kl = 1.0 - kr;
    float kb = 1.0 - kt;
    float ka = 1.0 - kf;

    kr = somestep(kr);
    kt = somestep(kt);
    kf = somestep(kf);
    kl = somestep(kl);
    kb = somestep(kb);
    ka = somestep(ka);

    float d;
    vec3 ret;
    ret  = f * kf; d  = kf;
    ret += a * ka; d += ka;
    ret += l * kl; d += kl;
    ret += r * kr; d += kr;
    ret += t * kt; d += kt;
    ret += b * kb; d += kb;

    return ret / d;
}

float phong(vec3 l, vec3 e, vec3 n, float power) {
    float nrm = (power + 8.0) / (PI * 8.0);
    return pow(max(dot(l,reflect(e,n)),0.0), power) * nrm;
}

// GGX code from https://www.shadertoy.com/view/MlB3DV
float G1V ( float dotNV, float k ) {
        return 1.0 / (dotNV*(1.0 - k) + k);
}
float GGX(vec3 N, vec3 V, vec3 L, float roughness, float F0) {
        float alpha = roughness*roughness;
        vec3 H = normalize (V + L);

        float dotNL = clamp (dot (N, L), 0.0, 1.0);
        float dotNV = clamp (dot (N, V), 0.0, 1.0);
        float dotNH = clamp (dot (N, H), 0.0, 1.0);
        float dotLH = clamp (dot (L, H), 0.0, 1.0);

        float D, vis;
        float F;

        // NDF : GGX
        float alphaSqr = alpha*alpha;
        float pi = 3.1415926535;
        float denom = dotNH * dotNH *(alphaSqr - 1.0) + 1.0;
        D = alphaSqr / (pi * denom * denom);

        // Fresnel (Schlick)
        float dotLH5 = pow (1.0 - dotLH, 5.0);
        F = F0 + (1.0 - F0)*(dotLH5);

        // Visibility term (G) : Smith with Schlick's approximation
        float k = alpha / 2.0;
        vis = G1V (dotNL, k) * G1V (dotNV, k);

        return /*dotNL */ D * F * vis;
}


//------------------------------------------------------------------------------
// Tone mapping and transfer functions
//------------------------------------------------------------------------------

vec3 Tonemap_ACES(const vec3 x) {
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}


vec3 PBR(vec3 base_color, float roughness, float metallic, vec3 normal, vec3 position, vec3 ray, vec3 shadowmult)
{
    vec3 point = position;
    normal = normalize(normal);

    // material
    float fresnel_pow = mix(10.0, 7.5,metallic);
    //const vec3 color_mod = vec3(1.000, 0.766, 0.336);
    vec3 color_mod =base_color;

    //get sun color


    vec3 light_color = pow(min(vec3(1.0),GetFinalSkyColor(iSolarDir).xyz * 1.5), vec3(2.2)) * shadowmult;//pow(texture(iChannel0,vec3(1.0,0.0,0.0)).xyz * 1.2, vec3(2.2));

    // IBL
    vec3 ibl_diffuse = pow(textureBlured(normal), vec3(2.2));
    vec3 ibl_reflection = pow(textureBlured(reflect(ray,normal)), vec3(2.2));

    // fresnel
    float fresnel = max(1.0 - dot(normal,-ray), 0.0);
    fresnel = pow(fresnel,fresnel_pow);

    // reflection
    vec3 refl = pow(textureSky(reflect(ray,vec3(normal.x,normal.y,normal.z))).xyz, vec3(2.2));
    refl = mix(refl,ibl_reflection,(1.0-fresnel)*roughness);
    refl = mix(refl,ibl_reflection,roughness);

    // specular
    vec3 light = normalize(vec3(iSolarDir.x,-iSolarDir.y,iSolarDir.z));
    float power = 1.0 / max(roughness * 0.4,0.01);
    //vec3 spec = light_color * phong(light,ray,normal,power);
    vec3 spec = light_color * GGX(normal,-ray,light,roughness*0.7, 0.2);
    refl -= spec;

    // diffuse
    vec3 diff = ibl_diffuse * base_color;
    //diff = mix(diff * color_mod,refl,fresnel);

    vec3 color = mix(diff,refl * color_mod,metallic) + spec;
    //custom shadow effect
    return pow(color, vec3(1.0/2.2)) *  mix(shadowmult,vec3(shadowmult) + 0.5 * (vec3(1.0) -vec3(shadowmult)),length(light_color));//

}
void main()
{
    vec2 uv = texcoord;


    //light calculation

    //pixel props (roughness, metallic, object type)
    vec3 props =  texture(Props,uv).rgb;


    //get main fragColor
    fragColor.rgb = texture(tex,uv).rgb;


    //if the object type is 2 (edge), dont do lighting
    if((props.b > 0.5 && !(props.b > 1.5 && props.b < 2.5) )&& DrawLights > 0)
    {
        //do the physically-based rendering
        float posx = texture(PosX,uv).r;
        float posy = texture(PosY,uv).r;
        float posz = texture(PosZ,uv).r;
        vec3 color = PBR(fragColor.rgb,1.0,0.0,texture(Normal,uv).rgb,vec3(posz,posy,posx),-normalize(vec3(posz,posy-iCameraPos.y,posx)),vec3(texture(Light,uv).rgb));
        fragColor.rgb = color.rgb;


    }else
    {
        fragColor.rgb = fragColor.rgb * 1.0;
    }


    fragColor.a = 1.0;

}
