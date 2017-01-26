#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "gl3stub.h"
#include "plugin.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class TextPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);
    bool Draw();
    int32_t KeyHandler(AInputEvent *event);
    IPlugin::PLUGIN_STATUS status();

private:
    GLuint m_Program;
    GLuint m_AttrCoord;
    GLuint m_UnifTex;
    GLuint m_UnifColor;
    PLUGIN_STATUS my_status = PLUGIN_STATUS_INIT_RIGHT_NOW;
    int m_width, m_height;

    FT_Library m_FT;
    FT_Face m_Face;
    FT_GlyphSlot m_G;
    GLuint m_Tex;

private:
    bool addShader(GLuint prog, GLenum ShaderType, const char *pFilename);
    bool initShaders();
    void RenderText(const char *text, float x, float y, float sx, float sy);

};


