#define LOG_TAG "HELLO_WORLD"

#include <math.h>
#include <GLES/gl.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_camera.h"
#include "gl3stub.h"

using namespace std;

#include "GLError.h"

struct HelloContext {
    GLuint gProgram;
    GLuint gvPositionHandle;
    GLuint m_Buffers[2];
};

/**
 * Simple example about how to use shader
 */

static bool addShader(GLuint prog, GLenum ShaderType, const char *pFilename) {
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

static void CreateVertexBuffer(HelloContext *pContext) {

    glUseProgram(pContext->gProgram);

    Vector3f Vertices[4];
    Vertices[0] = Vector3f(-1.0f, -1.0f, -1.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, -1.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, -1.0f);
    Vertices[3] = Vector3f(-1.0f, 1.0f, -1.0f);

    GLubyte Indices[] = { 0, 1, 2 };

    glGenBuffers(1, &pContext->m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, pContext->m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &pContext->m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pContext->m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static bool initShaders(HelloContext *pContext) {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    GLuint prog = glCreateProgram();

    if (!addShader(prog, GL_VERTEX_SHADER, "basicVertex.vs")) {
        return false;
    }

    if (!addShader(prog, GL_FRAGMENT_SHADER, "basicFragment.fs")) {
        return false;
    }

    glLinkProgram(prog);

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

    pContext->gvPositionHandle = glGetAttribLocation(prog, "Position");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
          pContext->gvPositionHandle);

    pContext->gProgram = prog;

    return true;
}

void* helloInit(int32_t width, int32_t height) {

    LOGI("in helloInit\n");

    struct HelloContext* pContext = (struct HelloContext *) malloc(sizeof(struct HelloContext));

    if (initShaders(pContext)) {

        static float grey;
        grey += 0.01f;
        if (grey > 1.0f) {
            grey = 0.0f;
        }
        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        CreateVertexBuffer(pContext);

        return pContext;
    }
    LOGE("fail to initialize shader\n");
    free(pContext);

    return NULL;
}

void helloDrawFrame(void *pContext) {

    struct HelloContext *pHelloCtx = (struct HelloContext *) pContext;

    glUseProgram(pHelloCtx->gProgram);
    checkGlError("glUseProgram");

    glBindBuffer(GL_ARRAY_BUFFER, pHelloCtx->m_Buffers[0]);
    glVertexAttribPointer(pHelloCtx->gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pHelloCtx->gvPositionHandle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pHelloCtx->m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, 4, GL_UNSIGNED_BYTE, 0);
}

int32_t helloKeyHandler(void *pContext, AInputEvent *event) {
    return 1;
}
