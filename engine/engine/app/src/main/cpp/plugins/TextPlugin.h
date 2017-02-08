#ifndef _TEXT_PLUGIN_H_
#define _TEXT_PLUGIN_H_

#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"
#include "plugin.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Technique.h"
#include "GLError.h"

#define LOG_TAG "Text"

#include "AppLog.h"
#include "ogldev_util.h"

class TextTechnique : public Technique {

private:
    GLint m_AttrCoord;
    GLint m_UnifTex;
    GLint m_UnifColor;

public:
    bool Init() {
        if (!Technique::Init()) {
            return false;
        }

        if (!AddShader(GL_VERTEX_SHADER, "textVertex.vs")) {
            return false;
        }

        if (!AddShader(GL_FRAGMENT_SHADER, "textFragment.fs")) {
            return false;
        }

        if (!Finalize()) {
            return false;
        }

        Enable();

        m_AttrCoord = GetAttributeLocation("coord");
        m_UnifTex = GetUniformLocation("tex");
        m_UnifColor = GetUniformLocation("color");

        if (m_AttrCoord == -1 || m_UnifColor == INVALID_UNIFORM_LOCATION ||
            m_UnifTex == INVALID_UNIFORM_LOCATION) {
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

    void SetUniformColor(GLsizei count, const GLfloat *v) {
        glUniform4fv(m_UnifColor, count, v);
    }
};


class TextPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS status();

    TextPlugin();

    ~TextPlugin();

private:

    PLUGIN_STATUS my_status = PLUGIN_STATUS_INIT_RIGHT_NOW;
    int m_width, m_height;

    FT_Library m_FT;
    FT_Face m_Face;
    FT_GlyphSlot m_G;
    GLuint m_Tex;
    GLuint m_VBO;

    TextTechnique *m_Shaders;

    char *m_Text = NULL;
    float m_X = 0.0f;
    float m_Y = 0.0f;
    float m_ScaleX = 0.0f;
    float m_ScaleY = 0.0f;

    FT_UInt m_FontSize = 48;

    GLfloat m_TextColor[4] = {0, 1, 0, 0.5};

public:
    void DisplayText(const char *text, float x, float y, float sx, float sy);
    void SetFontSize(FT_UInt fontSize);
    void SetFontColor(GLfloat color[4]);

private:
    void RenderText(const char *text, float x, float y, float sx, float sy);
};

#endif /* _TEXT_PLUGIN_H_ */


