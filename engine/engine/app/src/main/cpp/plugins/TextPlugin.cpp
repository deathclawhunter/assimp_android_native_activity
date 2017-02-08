#include <Magick++/Image.h>
#include "TextPlugin.h"

/**
 * Text drawing plugin
 */
TextPlugin::TextPlugin() {
    m_Shaders = new TextTechnique();
}

TextPlugin::~TextPlugin() {
    delete m_Shaders;
}

bool TextPlugin::Init(int32_t width, int32_t height) {

    LOGI("in text init\n");

    m_width = width;
    m_height = height;
    if (m_Shaders->Init()) {

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

        if (FT_Init_FreeType(&m_FT)) {
            LOGE("Could not init freetype library\n");
            return false;
        }

        if (FT_New_Face(m_FT, "arial.ttf", 0, &m_Face)) {
            LOGE("Could not open font\n");
            return false;
        }

        m_G = m_Face->glyph;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &m_Tex);
        m_Shaders->SetUniformTexture(0);

        glGenBuffers(1, &m_VBO);
        glEnableVertexAttribArray(m_Shaders->GetAttrCoord());
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glVertexAttribPointer(m_Shaders->GetAttrCoord(), 4, GL_FLOAT, GL_FALSE, 0, 0);

        return true;
    }
    LOGE("fail to initialize text shader\n");

    return false;
}

void TextPlugin::DisplayText(const char *text, float x, float y, float sx, float sy) {
    if (m_Text != NULL) {
        free(m_Text);
    }
    m_Text = strdup(text);
    m_X = x;
    m_Y = y;
    m_ScaleX = sx;
    m_ScaleY = sy;
}

void TextPlugin::SetFontSize(FT_UInt fontSize) {
    m_FontSize = fontSize;
}

void TextPlugin::RenderText(const char *text, float x, float y, float sx, float sy) {
    const char *p;

    for (p = text; *p; p++) {
        if (FT_Load_Char(m_Face, *p, FT_LOAD_RENDER))
            continue;

        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                m_G->bitmap.width,
                m_G->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                m_G->bitmap.buffer
        );

        float x2 = x + m_G->bitmap_left * sx;
        float y2 = -y - m_G->bitmap_top * sy;
        float w = m_G->bitmap.width * sx;
        float h = m_G->bitmap.rows * sy;

        GLfloat box[4][4] = {
                {x2,     -y2,     0, 0},
                {x2 + w, -y2,     1, 0},
                {x2,     -y2 - h, 0, 1},
                {x2 + w, -y2 - h, 1, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += (m_G->advance.x / 64) * sx;
        y += (m_G->advance.y / 64) * sy;
    }
}

bool TextPlugin::Draw() {

    if (m_Text == NULL || strlen(m_Text) <= 0) {
        return false;
    }

    m_Shaders->Enable();

    glEnableVertexAttribArray(m_Shaders->GetAttrCoord());
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(m_Shaders->GetAttrCoord(), 4, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Tex);

    /* Backup texture settings */
    GLint origStride, wrapS, wrapT, minFilter, magFilter;
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &origStride);

    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);

    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

    /* Change texture settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    float sx = 2.0 / m_width;
    float sy = 2.0 / m_height;

    m_Shaders->SetUniformColor(1, m_TextColor);

    FT_Set_Pixel_Sizes(m_Face, 0, m_FontSize);

    // Origin of the text
    // RenderText(m_Text, -1 + 8 * sx, 1 - 50 * sy, sx, sy);
    // RenderText(m_Text, -1 + 8 * m_X * sx, 1 - 50 * m_Y * sy, sx, sy);
    RenderText(m_Text, -1 + m_X * sx, 1 - (m_Y + 50) * sy, sx, sy);

    LOGI("Display text : %s", m_Text);

    /* Restore texture settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, origStride);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

int32_t TextPlugin::KeyHandler(InputData *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS TextPlugin::status() {
    return my_status; // example of never finish a plugin
}
