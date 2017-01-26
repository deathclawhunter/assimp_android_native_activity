#define LOG_TAG "Text"

#include "text.h"
#include "AppLog.h"
#include "GLError.h"
#include "ogldev_math_3d.h"

using namespace std;

/**
 * Text drawing plugin
 */

bool TextPlugin::addShader(GLuint prog, GLenum ShaderType, const char *pFilename) {
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

bool TextPlugin::initShaders() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    m_Program = glCreateProgram();

    if (!addShader(m_Program, GL_VERTEX_SHADER, "textVertex.vs")) {
        return false;
    }

    if (!addShader(m_Program, GL_FRAGMENT_SHADER, "textFragment.fs")) {
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

    m_AttrCoord = glGetAttribLocation(m_Program, "coord");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
         m_AttrCoord);

    m_UnifTex = glGetUniformLocation(m_Program, "tex");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"tex\") = %d\n",
         m_UnifTex);

    m_UnifColor = glGetUniformLocation(m_Program, "color");
    checkGlError("glGetUniformLocation");
    LOGI("glGetUniformLocation(\"color\") = %d\n",
         m_UnifColor);

    return true;
}

bool TextPlugin::Init(int32_t width, int32_t height) {

    LOGI("in text init\n");

    m_width = width;
    m_height = height;
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

        if(FT_Init_FreeType(&m_FT)) {
            LOGE("Could not init freetype library\n");
            return false;
        }

        if(FT_New_Face(m_FT, "arial.ttf", 0, &m_Face)) {
            LOGE("Could not open font\n");
            return false;
        }

        m_G = m_Face->glyph;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_Tex);
        glBindTexture(GL_TEXTURE_2D, m_Tex);
        glUniform1i(m_UnifTex, 0);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glEnableVertexAttribArray(m_AttrCoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(m_AttrCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);

        return true;
    }
    LOGE("fail to initialize shader\n");

    return false;
}

void TextPlugin::RenderText(const char *text, float x, float y, float sx, float sy) {
    const char *p;

    for(p = text; *p; p++) {
        if(FT_Load_Char(m_Face, *p, FT_LOAD_RENDER))
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
                {x2,     -y2    , 0, 0},
                {x2 + w, -y2    , 1, 0},
                {x2,     -y2 - h, 0, 1},
                {x2 + w, -y2 - h, 1, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += (m_G->advance.x/64) * sx;
        y += (m_G->advance.y/64) * sy;
    }
}

bool TextPlugin::Draw() {

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




    GLfloat black[4] = {0, 0, 0, 1};
    glUniform4fv(m_UnifColor, 1, black);

    float sx = 2.0 / m_width;
    float sy = 2.0 / m_height;

    GLfloat transparent_green[4] = {0, 1, 0, 0.5};
    glUniform4fv(m_UnifColor, 1, transparent_green);

    RenderText("The Quick Brown Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
    RenderText("The Misaligned Fox Jumps Over The Lazy Dog",
                -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);

    FT_Set_Pixel_Sizes(m_Face, 0, 48);
    RenderText("The Small Texture Scaled Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 175 * sy,   sx * 0.5, sy * 0.5);
    FT_Set_Pixel_Sizes(m_Face, 0, 24);
    RenderText("The Small Font Sized Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 200 * sy,   sx, sy);
    FT_Set_Pixel_Sizes(m_Face, 0, 48);
    RenderText("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 235 * sy,   sx * 0.25, sy * 0.25);
    FT_Set_Pixel_Sizes(m_Face, 0, 12);
    RenderText("The Tiny Font Sized Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 250 * sy,   sx, sy);

    RenderText("The Transparent Green Fox Jumps Over The Lazy Dog",
                -1 + 8 * sx,   1 - 380 * sy,   sx, sy);
    RenderText("The Transparent Green Fox Jumps Over The Lazy Dog",
                -1 + 18 * sx,  1 - 440 * sy,   sx, sy);


    /* Restore texture settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, origStride);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

int32_t TextPlugin::KeyHandler(AInputEvent *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS TextPlugin::status() {
    return my_status; // example of never finish a plugin
}
