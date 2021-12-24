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



#define FXAA_SPAN_MAX	8.0
#define FXAA_REDUCE_MUL 1.0/8.0
#define FXAA_REDUCE_MIN 1.0/128.0

void main()
{
    vec2 uv = texcoord;


        vec2 add = vec2(1.0) / iResolution.xy;

        vec3 rgbNW = texture(tex,uv+vec2(-add.x, -add.y)).rgb;
        vec3 rgbNE = texture(tex,uv+vec2( add.x, -add.y)).rgb;
        vec3 rgbSW = texture(tex,uv+vec2(-add.x,  add.y)).rgb;
        vec3 rgbSE = texture(tex,uv+vec2( add.x,  add.y)).rgb;
        vec3 rgbM  = texture(tex,uv).rgb;

        vec3 luma = vec3(0.299, 0.587, 0.114);
        float lumaNW = dot(rgbNW, luma);
        float lumaNE = dot(rgbNE, luma);
        float lumaSW = dot(rgbSW, luma);
        float lumaSE = dot(rgbSE, luma);
        float lumaM  = dot(rgbM,  luma);

        float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
        float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

        vec2 dir;
        dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
        dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));


        float dirReduce = max(
                (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

        float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

        dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
                  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
                  dir * rcpDirMin)) * add;


        vec3 rgbA = (1.0/2.0) * (texture(tex,uv + dir * (1.0/3.0 - 0.5)).rgb +
                                                         texture(tex,uv + dir * (2.0/2.0 - 0.5)).rgb);

        vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) *
                (texture(tex,uv.xy + dir * (0.0/3.0 - 0.5)).rgb +
                 texture(tex,uv.xy + dir * (3.0/3.0 - 0.5)).rgb);

        float lumaB = dot(rgbB, luma);
        if((lumaB < lumaMin) || (lumaB > lumaMax))
        {
                fragColor.xyz=rgbA;
                fragColor.a = 1.0;
        }else
        {
                fragColor.xyz=rgbB;
                fragColor.a = 1.0;
        }

        //light calculation

        vec3 props =  texture(Props,uv).rgb;


        //object type 1 is terrain, 2 is terrain edge
        if(props.b > 0.5)//&& !(props.b > 1.5 && props.b < 2.5))
        {
            fragColor.rgb = fragColor.rgb;

        }else
        {
            fragColor.rgb = fragColor.rgb;
        }

        fragColor.a = 1.0;

}
