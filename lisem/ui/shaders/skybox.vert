#version 400

layout(location = 0) in highp vec3 posAttr;
layout(location = 1) in highp vec2 UVAttr;
layout(location = 2) in highp vec3 normalAttr;
layout(location = 3) in highp vec3 tangentAttr;
layout(location = 4) in highp vec3 bitangentAttr;
layout(location = 5) in highp vec3 colorAttr;

uniform highp mat4 CMatrix;

out vec3 texcoords;

void main() {
   gl_Position = CMatrix *vec4(posAttr, 1.0);
   texcoords = posAttr;
}
