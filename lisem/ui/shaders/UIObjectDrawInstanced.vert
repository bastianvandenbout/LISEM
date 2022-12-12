#version 400

layout(location = 0) in highp vec3 posAttr;
layout(location = 1) in highp vec2 UVAttr;
layout(location = 2) in highp vec3 normalAttr;
layout(location = 3) in highp vec3 tangentAttr;
layout(location = 4) in highp vec3 bitangentAttr;
layout(location = 5) in highp vec3 colorAttr;
layout(location = 6) in highp mat4 matrixAttr;
layout(location = 10) in highp mat4 matrixPropAttr;
layout(location = 14) in highp mat4 matrixNAttr;

uniform highp mat4 CMatrix;
uniform highp vec3 ObjTranslate;

out vec3 frag_position;
out vec2 frag_texcoords;
out vec3 frag_in_normal;
out vec4 frag_highlight;

void main() {
   gl_Position = CMatrix *(vec4((matrixAttr*vec4(posAttr , 1.0)).xyz ,1.0));
   frag_position = (matrixAttr*vec4(posAttr , 1.0)).xyz;
   frag_texcoords = UVAttr;
   frag_in_normal = (matrixAttr*vec4(normalAttr,0.0)).xyz;
   frag_highlight = vec4(matrixPropAttr[0][0],matrixPropAttr[1][0],matrixPropAttr[2][0],matrixPropAttr[3][0]);
}
