#version 330

uniform sampler2D tex;
in vec2 texcoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(tex,texcoord);
}
