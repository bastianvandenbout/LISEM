#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out LSMVertex {
    highp vec4 worldPosition;
    highp vec4 position;
    highp vec3 normal;
    highp vec2 uv;
} Out;



uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 1.0);
    Out.uv = uv;
    Out.normal = normal;
    Out.position = projection * vec4(position, 1.0);
    Out.worldPosition = vec4(position,1.0);
}
