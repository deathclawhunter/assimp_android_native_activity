precision highp float;

varying vec3 SkyBoxTexCoord0;
uniform samplerCube gCubemapTexture;

void main()
{
    gl_FragColor = textureCube(gCubemapTexture, SkyBoxTexCoord0);
}
