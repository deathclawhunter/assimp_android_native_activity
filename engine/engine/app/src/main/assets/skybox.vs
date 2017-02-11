attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Normal;

varying vec2 TexCoord0;
varying vec3 Normal0;

uniform mat4 gWorld;
uniform mat4 gWVP;

void main()
{
    vec4 PosL    = vec4(Position, 1.0);
    vec4 tmp = gWVP * PosL;
    gl_Position = tmp.xyww;
    TexCoord0    = TexCoord;
    vec4 NormalL = vec4(Normal, 0.0);
    Normal0      = (gWorld * NormalL).xyz;
}
