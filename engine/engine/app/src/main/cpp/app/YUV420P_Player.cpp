//
// Created by Davis Z on 1/5/17.
//

#define LOG_TAG "YUV420P_PLAYER"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <utils/GLError.h>
#include "AppLog.h"
#include "gl3stub.h"
#include "YUV420P_Player.h"


static vec3 Vertices[] = {
        vec3(-1.0f, -1.0f, 0.0f),
        Vertices[1] = vec3(1.0f, -1.0f, 0.0f),
        Vertices[2] = vec3(0.0f, 1.0f, 0.0f),
        Vertices[3] = vec3(-1.0f, 1.0f, 0.0f),
        Vertices[4] = vec3(1.0f, 1.0f, 0.0f),
};
static GLubyte Indices[] = { 0, 2, 1, 0, 3, 2, 1, 2, 4 };


YUV420P_Player::YUV420P_Player()
        :vid_w(0)
        ,vid_h(0)
        ,win_w(0)
        ,win_h(0)
        // ,vao(0)
        ,y_tex(0)
        ,u_tex(0)
        ,v_tex(0)
        ,vert(0)
        ,frag(0)
        ,prog(0)
        // ,u_pos(-1)
        ,textures_created(false)
        ,shader_created(false)
        ,y_pixels(NULL)
        ,u_pixels(NULL)
        ,v_pixels(NULL)
{
}

bool YUV420P_Player::setup(int vidW, int vidH) {

    vid_w = vidW;
    vid_h = vidH;

    if(!vid_w || !vid_h) {
        printf("Invalid texture size.\n");
        return false;
    }

    if(y_pixels || u_pixels || v_pixels) {
        printf("Already setup the YUV420P_Player.\n");
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

    // TODO: Need to remove glGenVertexArrays()
    // glGenVertexArrays(1, &vao);

    vec3 Vertices[3];
    Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
    Vertices[1] = vec3(1.0f, -1.0f, 0.0f);
    Vertices[2] = vec3(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    return true;
}

GLuint YUV420P_Player::rx_create_shader(GLenum ShaderType, const char *s) {

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        LOGE("Error creating shader type %d\n", ShaderType);
        return ShaderObj;
    }

    const GLchar *p[1];
    p[0] = s;
    GLint Lengths[1] = {(GLint) strlen(s)};

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        LOGI("Error compiling '%s'\n", InfoLog);
        return 0;
    }

    return ShaderObj;
}



GLuint YUV420P_Player::rx_create_program(GLuint shader1, GLuint shader2) {

    GLuint prog = glCreateProgram();

    glAttachShader(prog, shader1);
    glAttachShader(prog, shader2);

    return prog;
}

bool YUV420P_Player::setupShader() {

    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    if(shader_created) {
        printf("Already creatd the shader.\n");
        return false;
    }

    vert = rx_create_shader(GL_VERTEX_SHADER, YUV420P_VS);
    frag = rx_create_shader(GL_FRAGMENT_SHADER, YUV420P_FS);
    prog = rx_create_program(vert, frag);

    glLinkProgram(prog);
    // rx_print_shader_link_info(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(prog);
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    glUseProgram(prog);
    glUniform1i(glGetUniformLocation(prog, "y_tex"), 0);
    glUniform1i(glGetUniformLocation(prog, "u_tex"), 1);
    glUniform1i(glGetUniformLocation(prog, "v_tex"), 2);

    // u_pos = glGetUniformLocation(prog, "draw_pos");

    gvPositionHandle = glGetAttribLocation(prog, "Position");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
         gvPositionHandle);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    resize(viewport[2], viewport[3]);

    return true;
}

bool YUV420P_Player::setupTextures() {

    if(textures_created) {
        printf("Textures already created.\n");
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

    // glBindVertexArray(vao);
    glUseProgram(prog);

    // glUniform4f(u_pos, x, y, w, h);

    glClear(GL_COLOR_BUFFER_BIT);

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
}

void YUV420P_Player::resize(int winW, int winH) {
    assert(winW > 0 && winH > 0);

    win_w = winW;
    win_h = winH;

    pm.identity();
    pm.ortho(0, win_w, win_h, 0, 0.0, 100.0f);

    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "u_pm"), 1, GL_FALSE, pm.ptr());
}

void YUV420P_Player::setYPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, y_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w, vid_h, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void YUV420P_Player::setUPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, u_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w/2, vid_h/2, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void YUV420P_Player::setVPixels(uint8_t* pixels, int stride) {
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, v_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w/2, vid_h/2, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

