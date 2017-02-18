#include "FlashEffect.h"

#include <string>
#include <utils/UtilTime.h>

using namespace std;

/**
 * Basic texture based 2D animation plugin
 */
bool FlashEffectPlugin::Init(int frameCount, const char **frames) {
    m_Shaders = new FlashEffectTechnique();

    m_Frames = (Magick::Blob **) malloc(sizeof(Magick::Blob *) * frameCount);
    m_Images = (Magick::Image **) malloc(sizeof(Magick::Image *) * frameCount);

    for (int i = 0; i < frameCount; i++) {

        string fn;
        fn.append(frames[i]);

        try {
            m_Images[i] = new Magick::Image();
            m_Frames[i] = new Magick::Blob();
            m_Images[i]->read(fn);
            m_Images[i]->write(m_Frames[i], "RGBA");
        } catch (Magick::Error &Error) {
            LOGE("Error loading texture");
        }
    }

    m_FrameCount = frameCount;

    return true;
}

FlashEffectPlugin::~FlashEffectPlugin() {
    delete m_Shaders;

    for (int i = 0; i < m_FrameCount; i++) {
        SAFE_DELETE(m_Frames[i]);
    }

    SAFE_DELETE(m_Frames);

    for (int i = 0; i < m_FrameCount; i++) {
        SAFE_DELETE(m_Images[i]);
    }

    SAFE_DELETE(m_Images);

    m_FrameCount = 0;
}

bool FlashEffectPlugin::Init(int32_t width, int32_t height) {

    LOGI("in flash effect init\n");

    m_Width = width;
    m_Height = height;
    if (m_Shaders->Init()) {

        m_Status = PLUGIN_STATUS_NEXT;

        static float grey;
        grey += 0.01f;
        if (grey > 1.0f) {
            grey = 0.0f;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_Shaders->SetUniformTexture(0);
        GLfloat DefModifier[2] = {0.0f, 0.0f};
        m_Shaders->SetUniformModifier(2, (GLfloat *) DefModifier);

        glGenBuffers(1, &m_VBO);
        glEnableVertexAttribArray(m_Shaders->GetAttrCoord());
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glVertexAttribPointer(m_Shaders->GetAttrCoord(), 4, GL_FLOAT, GL_FALSE, 0, 0);

        return true;
    }
    LOGE("fail to initialize flash effect shader\n");

    return false;
}

bool FlashEffectPlugin::RenderFlash(float x, float y, float sx, float sy) {

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
    GLfloat box[4][4] = {
            {-1, 1,  0, 0},
            {1,  1,  1, 0},
            {-1, -1, 0, 1},
            {1,  -1, 1, 1},
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Images[m_FrameIndex]->columns(),
                 m_Images[m_FrameIndex]->rows(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_Frames[m_FrameIndex]->data());

    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_FrameIndex++;
    if (m_FrameIndex >= m_FrameCount) {
        m_FrameIndex = 0;
    }

    return true;
}

bool FlashEffectPlugin::Draw() {

    if (!m_ShowFlag) {
        return false;
    }

    m_Shaders->Enable();

    glEnableVertexAttribArray(m_Shaders->GetAttrCoord());
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(m_Shaders->GetAttrCoord(), 4, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);

    /* Backup texture settings */
    GLint wrapS, wrapT, minFilter, magFilter;

    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);

    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);

    /* Change texture settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bool ret = RenderFlash(0, 0, 0, 0);

    /* Restore texture settings */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glBindTexture(GL_TEXTURE_2D, 0);

    return ret;
}

int32_t FlashEffectPlugin::KeyHandler(InputData *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS FlashEffectPlugin::Status() {
    return m_Status; // example of never finish a plugin
}

void FlashEffectPlugin::Play() {
    m_ShowFlag = true;
}

void FlashEffectPlugin::Pause() {
    m_ShowFlag = false;
}

void FlashEffectPlugin::Recoil() {
    m_DistY = 0.5f;
    m_Pulse = 5;
}

void FlashEffectPlugin::Moving() {
    m_DistY = 0.2f;
    m_Pulse = -1.0f; // negtive mean forever until pause or status change
}

void FlashEffectPlugin::Idle() {
    m_DistY = 0.0f;
    m_Pulse = 0.0f;
}
