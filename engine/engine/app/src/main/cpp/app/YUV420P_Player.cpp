//
// Created by Davis Z on 1/5/17.
//

#define LOG_TAG "YUV420P_PLAYER"

#include <assert.h>
#include <stdio.h>
#include <utils/GLError.h>
#include "AppLog.h"
#include "YUV420P_Player.h"


#if 1
static vec3 Vertices[] = {
        vec3(-1.0f, -1.0f, 0.0f),
        Vertices[1] = vec3(1.0f, -1.0f, 0.0f),
        Vertices[2] = vec3(0.0f, 1.0f, 0.0f),
        Vertices[3] = vec3(-1.0f, 1.0f, 0.0f),
        Vertices[4] = vec3(1.0f, 1.0f, 0.0f),
};
//static GLubyte Indices[] = { 0, 2, 1, 1, 2, 4, 0, 3, 2};

/**
 * How phone screen (portrait) layout looks like in opengl es:
 *
 * 0 (-1, -1)                3 (-1, 1)
 *     =========================
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     |                       |
 *     =========================
 * 1 (1, -1)                4 (1, 1)
 *
 */
static GLubyte Indices[] = { 0, 4, 1, 0, 3, 4};
#else // Only display in the center
static vec3 Vertices[] = {
        vec3(-0.5f, -0.5f, 0.0f),
        Vertices[1] = vec3(-0.5f, 0.5f, 0.0f),
        Vertices[2] = vec3(0.5f, -0.5f, 0.0f),
        Vertices[3] = vec3(0.5f, 0.5f, 0.0f)
};
static GLubyte Indices[] = { 0, 3, 2, 0, 1, 3};
#endif

YUV420P_Player::YUV420P_Player()
        :vid_w(0)
        ,vid_h(0)
        ,win_w(0)
        ,win_h(0)
        // ,vao(0)
        ,y_tex(0)
        ,u_tex(0)
        ,v_tex(0)
        // ,u_pos(-1)
        ,textures_created(false)
        ,shader_created(false)
        ,y_pixels(NULL)
        ,u_pixels(NULL)
        ,v_pixels(NULL)
{
}

bool YUV420P_Player::setup(int vidW, int vidH, int winW, int winH) {

    vid_w = vidW;
    vid_h = vidH;
    win_w = winW;
    win_h = winH;

    if(!vid_w || !vid_h) {
        LOGE("Invalid texture size.\n");
        return false;
    }

    if(y_pixels || u_pixels || v_pixels) {
        LOGI("Already setup the YUV420P_Player.\n");
        return false;
    }

    y_pixels = new uint8_t[vid_w * vid_h];
    u_pixels = new uint8_t[int((vid_w * 0.5) * (vid_h * 0.5))];
    v_pixels = new uint8_t[int((vid_w * 0.5) * (vid_h * 0.5))];

    if(!setupTextures()) {
        return false;
    }

    if(!setupShader()) {
        return false;
    }

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    return true;
}

bool YUV420P_Player::setupShader() {

    if(shader_created) {
        LOGW("Already creatd the shader.\n");
        return false;
    }

    if (!Technique::Init()) {
        return false;
    }

    if (!Technique::AddShaderFromString(GL_VERTEX_SHADER, YUV420P_VS)) {
        return false;
    }

    if (!Technique::AddShaderFromString(GL_FRAGMENT_SHADER, YUV420P_FS)) {
        return false;
    }

    if (!Technique::Finalize()) {
        return false;
    }

    Technique::Enable();

    glUniform1i(GetUniformLocation("y_tex"), 0);
    glUniform1i(GetUniformLocation("u_tex"), 1);
    glUniform1i(GetUniformLocation("v_tex"), 2);

    gvPositionHandle = GetAttributeLocation("YUVPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"YUVPosition\") = %d\n",
         gvPositionHandle);


    GLuint ScaleFactorX = GetUniformLocation("ScaleFactorX");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"ScaleFactorX\") = %d\n",
         ScaleFactorX);

    GLuint ScaleFactorY = GetUniformLocation("ScaleFactorY");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"ScaleFactorY\") = %d\n",
         ScaleFactorY);

    GLuint OffsetX = GetUniformLocation("OffsetX");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"OffsetX\") = %d\n",
         ScaleFactorX);

    GLuint OffsetY = GetUniformLocation("OffsetY");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"OffsetY\") = %d\n",
         ScaleFactorY);

    // by now window width and window height should already know
    // also video width and video height also know
    // step 1: fit video into UV map
    // UV is 0,0 to 1,1, uv_f is the scaling factor
    // based on video size when it comes to UV map

    // vertex position mapping to UV map should be
    float scaleX = 0.5;
    float scaleY = 0.5;
    glUniform1f(ScaleFactorX, scaleX);
    glUniform1f(ScaleFactorY, scaleY);

    float offsetX = 1.0;
    float offsetY = 1.0;

    glUniform1f(OffsetX, offsetX);
    glUniform1f(OffsetY, offsetY);

    return true;
}

bool YUV420P_Player::setupTextures() {

    if(textures_created) {
        LOGW("Textures already created.\n");
        return false;
    }

    glGenTextures(1, &y_tex);
    glBindTexture(GL_TEXTURE_2D, y_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w, vid_h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &u_tex);
    glBindTexture(GL_TEXTURE_2D, u_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w/2, vid_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glGenTextures(1, &v_tex);
    glBindTexture(GL_TEXTURE_2D, v_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w/2, vid_h/2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Backup texture parameters
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &origStride);
    LOGI("Original stride is %d\n", origStride);

    textures_created = true;
    return true;
}

void YUV420P_Player::draw(int x, int y, int w, int h) {
    assert(textures_created == true);

    if(w == 0) {
        w = vid_w;
    }

    if(h == 0) {
        h = vid_h;
    }

    Technique::Enable();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, y_tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, u_tex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, v_tex);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(gvPositionHandle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, sizeof(Indices) / sizeof(Indices[0]), GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void YUV420P_Player::setYPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, y_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w, vid_h, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void YUV420P_Player::setUPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, u_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w/2, vid_h/2, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void YUV420P_Player::setVPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, v_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w/2, vid_h/2, GL_RED, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void YUV420P_Player::tearDown() {
    glPixelStorei(GL_UNPACK_ROW_LENGTH, origStride);
}

