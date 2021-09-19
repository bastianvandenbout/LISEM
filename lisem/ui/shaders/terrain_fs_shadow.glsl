#version 400

uniform highp mat4 CMatrix;
uniform highp vec2 viewportSize;
uniform highp vec3 CameraPosition;
uniform highp vec2 TerrainSize;
uniform highp vec2 TerrainSizeL;
uniform highp float pixelsPerTriangleEdge = 10.0;
uniform highp float ZScale = 1.0;

uniform sampler2D TextureC;
uniform sampler2D TextureD;

layout (location = 0) out vec4 frag_depth;

in wireFrameVertex {
    vec4 worldPosition;
    vec4 position;
    vec4 color;
    vec3 patchdist;
    vec3 tridist;
    float tess_level;
    float alpha;
} VIN;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

highp vec2 GetTexCoords(highp vec4 wpos)
{
    highp vec2 texcoord = (vec2(wpos.x,wpos.z));
    texcoord.x = 0.5 + texcoord.x/TerrainSize.x;
    texcoord.y = 0.5 + texcoord.y/TerrainSize.y;

    return texcoord;
}

vec4 GetColor(vec4 wpos)
{
    vec2 texcoord = GetTexCoords(wpos);
    return vec4(texture(TextureC,texcoord).rgb,1.0);
}
highp float GetElevation(highp vec4 wpos)
{

    highp vec2 texcoord = GetTexCoords(wpos);
    return ZScale * texture(TextureD,texcoord).r;
}


void main() {

    frag_depth = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1.0) ;

}
