attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Normal;
attribute vec4 BoneIDs;
attribute vec4 Weights;

varying vec2 TexCoord0;
varying vec3 Normal0;
varying vec3 WorldPos0;

const int MAX_BONES = 100;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gBones[MAX_BONES];

void main()
{       
    mat4 BoneTransform = gBones[int(BoneIDs[0])] * Weights[0];
    BoneTransform     += gBones[int(BoneIDs[1])] * Weights[1];
    BoneTransform     += gBones[int(BoneIDs[2])] * Weights[2];
    BoneTransform     += gBones[int(BoneIDs[3])] * Weights[3];

    vec4 PosL    = BoneTransform * vec4(Position, 1.0);
    gl_Position  = gWVP * PosL;
    gl_Position  = vec4(Position, 1.0);
    TexCoord0    = TexCoord;
    vec4 NormalL = BoneTransform * vec4(Normal, 0.0);
    Normal0      = (gWorld * NormalL).xyz;
    WorldPos0    = (gWorld * PosL).xyz;
}
