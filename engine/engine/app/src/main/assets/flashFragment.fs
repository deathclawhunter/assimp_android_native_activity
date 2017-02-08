precision highp float;

varying vec2 texcoord;
uniform sampler2D tex;
// uniform vec4 color;

void main(void) {
//  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
    gl_FragColor = texture2D(tex, texcoord);
//    gl_FragColor = vec4(0, 1, 0, 1);
}
