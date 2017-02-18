attribute vec4 coord;
varying vec2 texcoord;
uniform vec2 modifier;

void main(void) {
  vec4 tmp = coord;
  tmp.x += modifier.x;
  tmp.y += modifier.y;
  gl_Position = vec4(tmp.xy, 0, 1);
  texcoord = tmp.zw;
}

