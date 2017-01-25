attribute vec4 Position;

varying vec2 TexCoord0;

void main() {
    gl_Position = Position;
    TexCoord0 = vec2(Position.x, Position.y);
}
