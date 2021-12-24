#version 330 core
in vec2 TexCoords;

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


void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    frag_colour = inColor;
}
