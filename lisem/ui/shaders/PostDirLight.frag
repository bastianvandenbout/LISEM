//from Shadertoy -FXAA example

#version 330

uniform sampler2D PosX; //position (in world space, relative to camera)
uniform sampler2D PosY;
uniform sampler2D PosZ;
uniform sampler2D Normal; //normal (in world space)
uniform sampler2D Props; //Metalness, Rougness, Object Type
uniform sampler2D Velocity;


uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;


in vec2 texcoord;

uniform vec2 iResolution;
uniform vec2 iViewPortSize;
uniform float iTime;
uniform vec3 iSolarDir;
uniform vec2 iSolarPos;
uniform vec3 iCameraPos; //Actual camera position
uniform vec3 iCameraDir; //View Direction in world space
uniform mat4x4 CMatrix;
uniform mat4x4 CMatrixShadowMap0;
uniform mat4x4 CMatrixShadowMap1;
uniform mat4x4 CMatrixShadowMap2;
uniform mat4x4 CMatrixNoTranslation;
uniform mat4x4 CMatrixWorldToView;

//pre-computed skybox color info
uniform vec3 iSolarColor;
uniform vec3 iSkyColor;
uniform vec3 iBaseColor;


out vec4 fragColor;



#define FXAA_SPAN_MAX	8.0
#define FXAA_REDUCE_MUL 1.0/8.0
#define FXAA_REDUCE_MIN 1.0/128.0


//variance cascaded shadowmapping for directional light
void main()
{
    vec2 uv = texcoord;

    //get current position

    float x = texture(PosZ,uv).r;
    float y = texture(PosY,uv).r;
    float z = texture(PosX,uv).r;

    //project to position in shadowmap coords

    vec4 pos_sh = CMatrixShadowMap0 * vec4(x,y,z,1.0);
    vec3 pos_cop = pos_sh.xyz/pos_sh.w;
    pos_sh.x = pos_cop.x * 0.5 + 0.5;
    pos_sh.y = pos_cop.y * 0.5 + 0.5;
    pos_sh.z = pos_cop.z * 0.5 + 0.5;
    //use uv in shadowmap coords to get shadowmap depth

    vec4 pos_sh1 = CMatrixShadowMap1 * vec4(x,y,z,1.0);
    vec3 pos_cop1 = pos_sh1.xyz/pos_sh1.w;
    pos_sh1.x = pos_cop1.x * 0.5 + 0.5;
    pos_sh1.y = pos_cop1.y * 0.5 + 0.5;
    pos_sh1.z = pos_cop1.z * 0.5 + 0.5;

    bool do_shadow = false;
    bool do_shadow1 = false;
    float shadow_depth = 0.0;
    float shadow_depth1 = 0.0;
    if(pos_sh.x > 0 && pos_sh.x < 1.0 && pos_sh.y < 1.0 && pos_sh.y > 0.0)
    {
        do_shadow= true;
        shadow_depth = 1.0 * texture(ShadowMap0,pos_sh.xy).r;

    }


    float shadow0 = 0.0;
    vec2 texelSize0 = 1.0 / textureSize(ShadowMap0, 0);
    for(int xi = -1; xi <= 1; ++xi)
    {
        for(int yi = -1; yi <= 1; ++yi)
        {
            float pcfDepth = texture(ShadowMap0, pos_sh.xy + vec2(xi, yi) * texelSize0).r;
            float s = do_shadow? (pcfDepth < pos_sh.z - 0.005 ? 0.0 : 1.0):1.0;
            if(pcfDepth > 0.99995 || pcfDepth < 0.0005)
            {
                s = 1.0;
            }
            shadow0 += s;
        }
    }


    if(pos_sh1.x > 0 && pos_sh1.x < 1.0 && pos_sh1.y < 1.0 && pos_sh1.y > 0.0)
    {
        do_shadow1= true;
        shadow_depth1 = 1.0 * texture(ShadowMap1,pos_sh1.xy).r;
    }


    float shadow1 = 0.0;
    vec2 texelSize1 = 1.0 / textureSize(ShadowMap1, 0);
    for(int xi = -1; xi <= 1; ++xi)
    {
        for(int yi = -1; yi <= 1; ++yi)
        {
            float pcfDepth = texture(ShadowMap1, pos_sh1.xy + vec2(xi, yi) * texelSize1).r;
            float s1 = do_shadow1? (pcfDepth < pos_sh1.z - 0.005 ? 0.0 : 1.0):1.0;
            if(pcfDepth > 0.99995 || pcfDepth < 0.0005)
            {
                s1 = 1.0;
            }
            shadow1 += s1;
        }
    }


    //compare depths to detect shadows

    float light0 = do_shadow? (shadow_depth < pos_sh.z - 0.005 ? 0.0 : 1.0):1.0;
    if(shadow_depth > 0.99995 || shadow_depth < 0.0005)
    {
        light0 = 1.0;
    }
    float light1 = do_shadow1? (shadow_depth1 < pos_sh1.z - 0.005 ? 0.0 : 1.0):1.0;
    if(shadow_depth1 > 0.99995 || shadow_depth1 < 0.0005)
    {
        light1 = 1.0;
    }
    shadow0 += light0;
    shadow1 += light1;

    shadow0 /= 10.0;

    shadow1 /= 10.0;



    float light = shadow0 * shadow1;

    fragColor = vec4(iSolarColor * light,1.0);//texture(ShadowMap0,uv);

}
