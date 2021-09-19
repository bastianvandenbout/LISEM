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


void main()
{
    vec2 uv = texcoord;

    int index_x = int(uv.x * 3.0);
    int index_y = int(uv.y * 2.0);
    int index = index_x + 3 * index_y;

    vec2 uv_rel = vec2(fract(uv.x * 3.0),fract(uv.y * 2.0));

    vec3 color;

    if(index == 0)
    {
        color = texture(tex,uv_rel).rgb;

    }else if(index == 1)
    {
        color.r = (texture(PosX,uv_rel).r + 1000.0)/2000.0;
        color.g = (texture(PosY,uv_rel).r + 1000.0)/2000.0;
        color.b = (texture(PosZ,uv_rel).r + 1000.0)/2000.0;
    }else if(index == 2)
    {
        color.r = (texture(Normal,uv_rel).r + 1.0)/2.0;
        color.g = (texture(Normal,uv_rel).g + 1.0)/2.0;
        color.b = (texture(Normal,uv_rel).b + 1.0)/2.0;
    }else if(index == 3)
    {
        color = vec3(texture(Props,uv_rel).b/4.0,0.0,0.0);

    }else if(index == 4)
    {

        color.r = (texture(Props,uv_rel).r);
        color.g = (texture(Props,uv_rel).g);
        color.b = 0.0;
    }else if(index == 5)
    {
        color.r = (texture(Light,uv_rel).r)/1.0;
        color.g = (texture(Light,uv_rel).g)/1.0;
        color.b = (texture(Light,uv_rel).b)/1.0;
    }

        //vec3 wpos = vec3(texture(PosX,uv).r,texture(PosX,uv).g,texture(PosX,uv).b);
    fragColor.rgb =vec3(color.r,color.g,color.b);
    fragColor.a = 1.0f;

}
