//
// Created by Davis Z on 1/5/17.
//

/*

  YUV420P Player
  --------------

  This class implements a simple YUV420P renderer. This means that you
  need to feed planar YUV420 data to the `setYPixels()`, `setUPixels()`
  and `setVPixels()`.

  First make sure to call setup() with the video width and height. We use
  these dimensions to allocate the Y, U and V textures. After calling setup
  you call the zset{Y,U,V}Pixels()` everytime you have a new frame that
  you want to render. With the `draw()` function you draw the current
  frame to the screen.

  If you resize your viewport, make sure to  call `resize()` so we can
  adjust the projection matrix.

 */
#ifndef ROXLU_YUV420P_PLAYER_H
#define ROXLU_YUV420P_PLAYER_H

#define ROXLU_USE_MATH
#define ROXLU_USE_PNG
#define ROXLU_USE_OPENGL
#include <stdint.h>
#include <GLES/gl.h>
#include "math.h"
#include "tinylib.h"
#include "Technique.h"

static const char* YUV420P_VS = ""

        "attribute vec4 YUVPosition;"
        "uniform float ScaleFactorX;"
        "uniform float ScaleFactorY;"
        "uniform float OffsetX;"
        "uniform float OffsetY;"
        "varying vec2 v_coord; "
        "void main() {"
        "    gl_Position = YUVPosition;"
        /**
         * From -1,-1 -> 1,1 map to 0,0 -> 2,2
         * Then use scale factor to fit video
         */
        "    v_coord = vec2((YUVPosition.x + OffsetX) * ScaleFactorX, (YUVPosition.y + OffsetY) * ScaleFactorY);"
        "}";
        /* // "#version 330\n"
        ""
        // "uniform mat4 u_pm;"
        "uniform vec4 draw_pos;"
        ""
        "varying vec2 v_coord; "
        ""
        "void main() {"
        "   vec2 verts[4] = vec2[] ("
        "       vec2(-0.5,  0.5), "
        "       vec2(-0.5, -0.5), "
        "       vec2( 0.5,  0.5), "
        "       vec2( 0.5, -0.5)  "
        "   );"
        ""
        "   vec2 texcoords[4] = vec2[] ("
        "       vec2(0.0, 1.0), "
        "       vec2(0.0, 0.0), "
        "       vec2(1.0, 1.0), "
        "       vec2(1.0, 0.0)  "
        "   ); "
        // "   vec2 vert = verts[gl_VertexID];"
        "   vec2 vert = verts[0];"
        "   vec4 p = vec4((0.5 * draw_pos.z) + draw_pos.x + (vert.x * draw_pos.z), "
        "                 (0.5 * draw_pos.w) + draw_pos.y + (vert.y * draw_pos.w), "
        "                 0, 1);"
        // "   gl_Position = u_pm * p;"
        "   gl_Position = p;"
        // "   v_coord = texcoords[gl_VertexID];"
        "   v_coord = texcoords[0];"
        "}"
        ""; */

static const char* YUV420P_FS = ""
        // "#version 330\n"
        "precision mediump float;"
        "uniform sampler2D y_tex;"
        "uniform sampler2D u_tex;"
        "uniform sampler2D v_tex;"
        "varying vec2 v_coord;"
        // "layout( location = 0 ) out vec4 fragcolor;"
        ""
        "const vec3 R_cf = vec3(1.164383,  0.000000,  1.596027);"
        "const vec3 G_cf = vec3(1.164383, -0.391762, -0.812968);"
        "const vec3 B_cf = vec3(1.164383,  2.017232,  0.000000);"
        "const vec3 offset = vec3(-0.0625, -0.5, -0.5);"
        ""
        "void main() {"
        "  float y = texture2D(y_tex, v_coord).r;"
        "  float u = texture2D(u_tex, v_coord).r;"
        "  float v = texture2D(v_tex, v_coord).r;"
        "  vec3 yuv = vec3(y,u,v);"
        "  yuv += offset;"
        "  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);"
        "  gl_FragColor.r = dot(yuv, R_cf);"
        "  gl_FragColor.g = dot(yuv, G_cf);"
        "  gl_FragColor.b = dot(yuv, B_cf);"
        "}"
        "";

class YUV420PPlayer : Technique {

public:
    YUV420PPlayer();
    bool setup(int w, int h, int winW, int winH);
    void setYPixels(uint8_t* pixels, int stride);
    void setUPixels(uint8_t* pixels, int stride);
    void setVPixels(uint8_t* pixels, int stride);
    void draw(int x, int y, int w = 0, int h = 0);
    void resize(int winW, int winH);
    void tearDown();

private:
    bool setupTextures();
    bool setupShader();

public:
    GLint origStride;
    int vid_w;
    int vid_h;
    int win_w;
    int win_h;
    GLuint m_Buffers[2];
    GLuint y_tex;
    GLuint u_tex;
    GLuint v_tex;
    bool textures_created;
    bool shader_created;
    uint8_t* y_pixels;
    uint8_t* u_pixels;
    uint8_t* v_pixels;
    mat4 pm;

    GLuint gvPositionHandle;
};
#endif
