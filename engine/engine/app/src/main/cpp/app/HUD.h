#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "gl3stub.h"
#include "plugin.h"

class HUDPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);
    bool Draw();
    int32_t KeyHandler(AInputEvent *event);
    IPlugin::PLUGIN_STATUS status();

private:
    GLuint m_Program;
    GLuint m_AttrPosition;
    GLuint m_Buffers[2];
    GLsizei m_NumOfElements = 0;
    PLUGIN_STATUS my_status = PLUGIN_STATUS_INIT_RIGHT_NOW;

private:
    bool addShader(GLuint prog, GLenum ShaderType, const char *pFilename);
    void CreateVertexBuffer();
    bool initShaders();

};

