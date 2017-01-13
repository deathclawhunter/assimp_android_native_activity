#define LOG_TAG "HELLO_WORLD"

#include <math.h>
#include <GLES/gl.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_camera.h"
#include "gl3stub.h"
#include "helloworld.h"

using namespace std;

#include "GLError.h"

/**
 * Simple example about how to use shader
 */

bool HelloWorldPlugin::addShader(GLuint prog, GLenum ShaderType, const char *pFilename) {
    string s;

    if (!ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        LOGE("Error creating shader type %d\n", ShaderType);
        return false;
    }

    const GLchar *p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = {(GLint) s.size()};

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        LOGI("Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }

    glAttachShader(prog, ShaderObj);

    return true;
}

void HelloWorldPlugin::CreateVertexBuffer() {

    /**
     * Test data for helloworld
     *
     * The indices is clockwise for now. Check display.cpp for
     * implementation.
     *
     * Example for how to change face culling settings:
     *
     * // Enable culling front faces or back faces
     * glEnable(GL_CULL_FACE);
     * // Specify clockwise indexed are front face, clockwise is determined by the sequence in index buffer
     * glFrontFace(GL_CW);
     * // Cull the back face
     * glCullFace(GL_BACK);
     *
     */
    Vector3f Vertices[] = {
            Vector3f(-1.0f, -1.0f, 0.0f),
            Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f),
            Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f),
            Vertices[3] = Vector3f(-1.0f, 1.0f, 0.0f)
    };
    GLubyte Indices[] = { 0, 2, 1 };

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    numOfElements = sizeof(Indices) / sizeof(Indices[0]);
}

bool HelloWorldPlugin::initShaders() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    gProgram = glCreateProgram();

    if (!addShader(gProgram, GL_VERTEX_SHADER, "basicVertex.vs")) {
        return false;
    }

    if (!addShader(gProgram, GL_FRAGMENT_SHADER, "basicFragment.fs")) {
        return false;
    }

    glLinkProgram(gProgram);

    glGetProgramiv(gProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(gProgram, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(gProgram);
    glGetProgramiv(gProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(gProgram, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    glUseProgram(gProgram);

    gvPositionHandle = glGetAttribLocation(gProgram, "Position");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
          gvPositionHandle);

    CreateVertexBuffer();

    return true;
}

bool HelloWorldPlugin::Init(int32_t width, int32_t height) {

    LOGI("in helloInit\n");

    if (initShaders()) {

        static float grey;
        grey += 0.01f;
        if (grey > 1.0f) {
            grey = 0.0f;
        }
        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        return true;
    }
    LOGE("fail to initialize shader\n");

    return false;
}

bool HelloWorldPlugin::Draw() {

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(gvPositionHandle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, numOfElements, GL_UNSIGNED_BYTE, 0);

    return true;
}

int32_t HelloWorldPlugin::KeyHandler(AInputEvent *event) {
    return 1;
}
