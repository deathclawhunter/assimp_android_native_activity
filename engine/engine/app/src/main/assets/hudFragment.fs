precision mediump float;

uniform float iGlobalTime;
uniform vec2 iResolution;

varying vec2 TexCoord0;

float inner (float a, float l){
    
    float j = l*2.3;
    //j = pow(j, 4.0);
    float f = 1.0- smoothstep(1., 1.04, j) - (1.0-smoothstep(0.5, .002, j));
    
    return f;
}

float rimlight(float a, float l){
    
    float j = l*1.5;
    //j = pow(j, 4.0);
    float f = 1.0- smoothstep(0.5, 1.4, j) - (1.0-smoothstep(0.5, .92, j));
    
    return f;
}

float rim(float a, float l){
    
    float j = l;
        j = l+sin(iGlobalTime);
    //j = pow(j, 4.0);
    float g = 1.0-smoothstep(0.7, .72, j);
    
    float f = smoothstep(0.8, 1., j);
    f = f-g;
    f = 1.0-smoothstep(f-0.009, f+0.009, j);
    
    return f;
}

float darkrim(float a, float l){
    
    float j = l*2.5;
    float f = pow(1.0- smoothstep(1., 1.09, j) - (1.0-smoothstep(.92, .9995, j)), 9.);
	f = f * abs(fract(a/5.)*2.0-1.)*2.;
    
    return f;
}

float threeline(float a, float l){
    
    float f = (smoothstep(-0., 0.03, cos((a)*5.))*.1+0.31) ;
    float g = (1.0 - smoothstep(0.0, 1.0, l*2.30391));
    f = f-g;
    f = 1.0-smoothstep(f,f+0.008, l) ;
    return f*100.0;
}

float spiral(float a, float l){
    float f = smoothstep(.0, -.2, cos(a*15.0 + iGlobalTime*10.0))*.341;

 	float g = 1.0-smoothstep(0.0, 1.0, l*3.0);

    f = f-g;
    f = 1.0-smoothstep(f, f+0.008, l);
    return f;
}

float innerspiral(float a, float l){
    float f = smoothstep(-.5, 1.0, cos(a*50.0+l*20.*sin(iGlobalTime)*10.0 ))*.21;

 	float g = 1.0-smoothstep(0.0, 1.0, l*5.);

    f = f-g;
    f = 1.0-smoothstep(f, f+0.03, l);
    return f;
}

void main() {

	vec2 uv = TexCoord0;
    uv = uv*2.0-1.0;
    float a = atan(uv.y,uv.x);
    float l = length(uv) - 0.2;
    l = l/1.05;
    vec3 color = vec3(.0, 0., 0.);
    
   	vec3 blue = vec3(0.0,0.0,1.0);
    vec3 green = vec3(0.0,1.0,0.0);
    vec3 grey = vec3(0.0, 0.0, 0.0);

    color = mix(color, grey, smoothstep(0., 1., 1.));
    
    color = mix(color, vec3(0.0,0.5,0.9),rim(a, l));
    color = mix(color, vec3(0.0,0.5,0.9),rim(a, l*1.5));
    color = mix(color, vec3(0.0,0.5,0.9),rim(a, l*1.2));
    
    color = mix(color, vec3(0.05,0.05,0.05),darkrim(a, l));
    color = mix(color, vec3(0.0,0.4,0.9), threeline(a, l));
    
    color = mix(color, vec3(1. ,1., 1.), spiral(a, l*1.1));
    color = mix(color, vec3(1. ,1., 1.), spiral(a, l/2.4));
    
    color = mix(color, vec3(0.0, 0.3, 0.6), innerspiral(a, l));
    color = mix(color, vec3(0.0, 0.3, 0.6), inner(a, l));
    
    color = mix(color, vec3(0.0, 0.3, 0.6), rimlight(a, l));
    color = mix(color, vec3(0.0, 0.3, 0.6), rimlight(a, l*1.3));
    
	gl_FragColor = vec4(color,0.05);
}
