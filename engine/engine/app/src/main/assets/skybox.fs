precision highp float;

varying vec2 TexCoord0;

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

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gColorMap;

varying vec3 Normal0;

void main()
{
    vec4 AmbientColor = vec4(gDirectionalLight.Base.Color * gDirectionalLight.Base.AmbientIntensity, 1.0);
    float DiffuseFactor = dot(Normal0, -gDirectionalLight.Direction);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(gDirectionalLight.Base.Color * gDirectionalLight.Base.DiffuseIntensity * 1.0, 1.0);
    }

    vec4 color = AmbientColor + DiffuseColor;
    gl_FragColor = texture2D(gColorMap, TexCoord0.xy) * color;
}
