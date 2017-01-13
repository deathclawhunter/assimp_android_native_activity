#ifndef _HELLOWORLD_H_
#define _HELLOWORLD_H_

#include <math.h>
#include <GLES/gl.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_camera.h"
#include "gl3stub.h"
#include "plugin.h"

using namespace std;


class HelloWorldPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);
    bool Draw();
    int32_t KeyHandler(AInputEvent *event);

private:
    GLuint gProgram;
    GLuint gvPositionHandle;
    GLuint m_Buffers[2];
    GLsizei numOfElements = 0;

private:
    bool addShader(GLuint prog, GLenum ShaderType, const char *pFilename);
    void CreateVertexBuffer();
    bool initShaders();

};

#endif // _HELLOWORLD_H_

