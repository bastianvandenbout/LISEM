#version 330 core

in LSMVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    highp vec3 normal;
    highp vec2 uv;
} VIN;



layout (location = 0) out vec4 frag_colour;

layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

layout (location = 4) out vec4 frag_normal;
layout (location = 5) out vec4 frag_props;
layout (location = 6) out vec4 frag_Light;
layout (location = 7) out vec4 frag_Velocity;

uniform sampler2D text;
uniform vec4 inColor;
uniform int SampleTexture;
uniform int TextureIsText;
uniform float TextureWeight;



void main()
{
    vec4 sampled;
    float type = 3.0;
    if(TextureIsText > 0.5)
    {
        type = 2.0;
        float val = texture(text, VIN.uv).r;
        sampled = inColor *pow(smoothstep(-0.5*fwidth(val),0.5*fwidth(val),val-0.68),2);
        frag_colour = sampled;//sampled;

    }else if(SampleTexture > 0.5)
    {
            sampled = texture(text, VIN.uv);
            frag_colour = sampled;
            frag_colour = vec4(1.0,0.0,0.0,1.0);//sampled;

    }else
    {
        frag_colour = inColor;
    }
    frag_posx = vec4(VIN.worldPosition.z,VIN.worldPosition.z,VIN.worldPosition.z,1.0);
    frag_posy = vec4(VIN.worldPosition.y,VIN.worldPosition.y,VIN.worldPosition.y,1.0);
    frag_posz = vec4(VIN.worldPosition.x,VIN.worldPosition.x,VIN.worldPosition.x,1.0) ;

    frag_normal = vec4(VIN.normal.xyz,1.0);
    frag_props = vec4(0.6,0.6,type,1.0);
    frag_Light = vec4(1.0,1.0,1.0,1.0);
    frag_Velocity = vec4(0.0,0.0,0.0,1.0);

}
