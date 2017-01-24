#define LOG_TAG "HUD"

#include "HUD.h"
#include "AppLog.h"
#include "GLError.h"
#include "ogldev_math_3d.h"
#include <string>

using namespace std;

/**
 * Simple example about how to use shader
 */

bool HUDPlugin::addShader(GLuint prog, GLenum ShaderType, const char *pFilename) {
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

void HUDPlugin::CreateVertexBuffer() {

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
    Vector3f Vertices[] = {
            Vector3f(-1.0f, -1.0f, 0.0f),
            Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f),
            Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f),
            Vertices[3] = Vector3f(-1.0f, 1.0f, 0.0f),
            Vertices[4] = Vector3f(0.0f, -1.0f, 0.0f)
    };
    GLubyte Indices[] = { 0, 2, 4 };

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    m_NumOfElements = sizeof(Indices) / sizeof(Indices[0]);
}

bool HUDPlugin::initShaders() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    m_Program = glCreateProgram();

    if (!addShader(m_Program, GL_VERTEX_SHADER, "hudVertex.vs")) {
        return false;
    }

    if (!addShader(m_Program, GL_FRAGMENT_SHADER, "hudFragment.fs")) {
        return false;
    }

    glLinkProgram(m_Program);

    glGetProgramiv(m_Program, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(m_Program, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_Program);
    glGetProgramiv(m_Program, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(m_Program, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    glUseProgram(m_Program);

    m_AttrPosition = glGetAttribLocation(m_Program, "Position");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
          m_AttrPosition);

    CreateVertexBuffer();

    return true;
}

bool HUDPlugin::Init(int32_t width, int32_t height) {

    LOGI("in helloInit\n");

    if (initShaders()) {

        my_status = PLUGIN_STATUS_NEXT;

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

bool HUDPlugin::Draw() {

    glUseProgram(m_Program);
    checkGlError("HelloWorldPlugin::glUseProgram");

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glVertexAttribPointer(m_AttrPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_AttrPosition);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, m_NumOfElements, GL_UNSIGNED_BYTE, 0);

    return true;
}

int32_t HUDPlugin::KeyHandler(AInputEvent *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS HUDPlugin::status() {
    return my_status; // example of never finish a plugin
}