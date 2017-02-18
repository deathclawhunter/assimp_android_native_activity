#ifndef _FLASHEFFECT_H_
#define _FLASHEFFECT_H_


#include <GLES/gl.h>
#include <android_native_app_glue.h>
#include <ogldev/ogldev_texture.h>

#include "GL3Stub.h"
#include "plugin.h"

#include "Technique.h"
#include "GLError.h"

#define LOG_TAG "FlashEffect"

#include "AppLog.h"
#include "ogldev_util.h"

class FlashEffectTechnique : public Technique {

private:
    GLint m_AttrCoord;
    GLint m_UnifTex;
    GLint m_UnifModifier;

public:
    bool Init() {
        if (!Technique::Init()) {
            return false;
        }

        if (!AddShader(GL_VERTEX_SHADER, "flashVertex.vs")) {
            return false;
        }

        if (!AddShader(GL_FRAGMENT_SHADER, "flashFragment.fs")) {
            return false;
        }

        if (!Finalize()) {
            return false;
        }

        Enable();

        m_AttrCoord = GetAttributeLocation("coord");
        m_UnifTex = GetUniformLocation("tex");
        m_UnifModifier = GetUniformLocation("modifier");

        if (m_AttrCoord == -1 || m_UnifTex == INVALID_UNIFORM_LOCATION ||
            m_UnifModifier == INVALID_UNIFORM_LOCATION) {
            return false;
        }

        return true;
    }

    GLint GetAttrCoord() {
        return m_AttrCoord;
    }

    void SetUniformTexture(GLint value) {
        glUniform1i(m_UnifTex, value);
    }

    void SetUniformModifier(GLsizei count, const GLfloat *xy) {
        glUniform2fv(m_UnifModifier, count, xy);
    }
};


class FlashEffectPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    bool Init(int frameCount, const char **frames);

    ~FlashEffectPlugin();

    void Play();
    void Pause();

    void Recoil();
    void Moving();
    void Idle();

private:

    PLUGIN_STATUS m_Status = PLUGIN_STATUS_INIT_RIGHT_NOW;
    int m_Width, m_Height;

    GLuint m_VBO;

    FlashEffectTechnique *m_Shaders;

    Magick::Image **m_Images;
    Magick::Blob **m_Frames;
    int m_FrameCount;

    int m_FrameIndex = 0; // internal counter for frame index

    bool m_ShowFlag = false;

    float m_DistY = 0.0f; // Distance in Y axis for this effect
    float m_Pulse = 0.0f; // Frame count for this effect

private:
    bool RenderFlash(float x, float y, float sx, float sy);
};

#endif /* _FLASHEFFECT_H_ */


