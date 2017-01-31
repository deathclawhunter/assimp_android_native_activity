precision highp float;

varying vec2 TexCoord0;
varying vec3 Normal0;
varying vec3 WorldPos0;

struct VSOutput
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 WorldPos;
};


struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation
{
    float Constant;
    float Linear;
    float Exp;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gColorMap;
uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;


vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, VSOutput In)
{
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
    float DiffuseFactor = dot(In.Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);

        vec3 VertexToEye = normalize(gEyeWorldPos - In.WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, In.Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0.0) {
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0);
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 CalcDirectionalLight(VSOutput In)
{
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, In);
}

void main()
{
    VSOutput In;
    In.TexCoord = TexCoord0;
    In.Normal   = normalize(Normal0);
    In.WorldPos = WorldPos0;

    vec4 TotalLight = CalcDirectionalLight(In);

    gl_FragColor = texture2D(gColorMap, In.TexCoord.xy) * TotalLight;
}
