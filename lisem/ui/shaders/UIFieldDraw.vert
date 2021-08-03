#version 400

layout(location = 0) in highp vec3 posAttr;
layout(location = 1) in highp vec2 UVAttr;
layout(location = 2) in highp vec3 normalAttr;
layout(location = 3) in highp vec3 tangentAttr;
layout(location = 4) in highp vec3 bitangentAttr;
layout(location = 5) in highp vec3 colorAttr;

uniform highp mat4 OMatrix;
uniform highp mat4 CMatrix;

uniform highp vec3 ObjTranslate;
uniform highp vec3 ObjScale;

out vec3 frag_position;
out vec2 frag_texcoords;
out vec3 frag_normal;

void main() {
   gl_Position = CMatrix *(vec4((OMatrix*vec4(posAttr , 1.0)).xyz,1.0));
   frag_position = (OMatrix*vec4(posAttr , 1.0)).xyz;
   frag_texcoords = UVAttr;
   frag_normal = (OMatrix*vec4(normalAttr,0.0)).xyz;
}
