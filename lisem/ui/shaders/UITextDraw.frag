#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{
	vec2 texcoord = TexCoords;
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	float val = texture(text, TexCoords).r;
    color = textColor *pow(smoothstep(-0.5*fwidth(val),0.5*fwidth(val),val-0.68),2);
}
