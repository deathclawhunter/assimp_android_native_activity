attribute vec3 SkyBoxPosition;

uniform mat4 SkyBoxWVP;
                                                                                    
varying vec3 SkyBoxTexCoord0;
                                                                                    
void main()                                                                         
{
    vec4 WVP_Pos = SkyBoxWVP * vec4(SkyBoxPosition, 1.0);
    gl_Position = WVP_Pos.xyww;
    SkyBoxTexCoord0   = SkyBoxPosition;
}
