#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "gl3stub.h"
#include "plugin.h"

#define LOG_TAG "HUD"
#include "AppLog.h"
#include "technique.h"
#include "ogldev_util.h"

class HUDTechnique : public Technique {

private:
    GLint m_AttrPosition;
    GLint m_UnifGlobalTime;

public:
    bool Init() {
        if (!Technique::Init()) {
            return false;
        }

        if (!AddShader(GL_VERTEX_SHADER, "hudVertex.vs")) {
            return false;
        }

        if (!AddShader(GL_FRAGMENT_SHADER, "hudFragment.fs")) {
            return false;
        }

        if (!Finalize()) {
            return false;
        }

        Enable();

        m_AttrPosition = GetAttributeLocation("Position");
        m_UnifGlobalTime = GetUniformLocation("iGlobalTime");

        if (m_AttrPosition == -1 || m_UnifGlobalTime == INVALID_UNIFORM_LOCATION) {
            return false;
        }

        return true;
    }

    GLint GetAttrPosition() {
        return m_AttrPosition;
    }

    void SetUniformGlobalTime(GLfloat value) {
        glUniform1f(m_UnifGlobalTime, value);
    }
};

class HUDPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(AInputEvent *event);

    IPlugin::PLUGIN_STATUS status();

    HUDPlugin();

    ~HUDPlugin();

private:
    GLuint m_Buffers[2];
    GLsizei m_NumOfElements = 0;
    PLUGIN_STATUS my_status = PLUGIN_STATUS_INIT_RIGHT_NOW;
    int m_width, m_height;

    int m_Frame;
    uint64_t m_FrameTimeout;
    uint64_t m_FrameDelay;

    HUDTechnique *m_Shaders;

private:
    void CreateVertexBuffer();

    bool initShaders();

    uint64_t rx_hrtime();

};


