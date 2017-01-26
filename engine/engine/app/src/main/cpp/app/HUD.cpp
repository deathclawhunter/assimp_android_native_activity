#include "HUD.h"
#include "GLError.h"
#include "ogldev_math_3d.h"

HUDPlugin::HUDPlugin() {
    m_Shaders = new HUDTechnique();
}

HUDPlugin::~HUDPlugin() {
    delete m_Shaders;
}

void HUDPlugin::CreateVertexBuffer() {

    /**
     * The indices is clockwise for now. Check display.cpp for
     * implementation.
     *
     * Example for how to change face culling settings:
     *
     * // Enable culling front faces or back faces
     * glEnable(GL_CULL_FACE);
     * // Specify clockwise indexed are front face, clockwise is determined by the sequence in index buffer
     * glFrontFace(GL_CW);
     * // Cull the back face
     * glCullFace(GL_BACK);
     *
     */
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
    Vector3f Vertices[] = {
            Vector3f(-1.0f, -1.0f, 0.0f),
            Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f),
            Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f),
            Vertices[3] = Vector3f(-1.0f, 1.0f, 0.0f),
            Vertices[4] = Vector3f(0.0f, -1.0f, 0.0f),
            Vertices[5] = Vector3f(1.0f, 1.0f, 0.0f)
    };
    GLubyte Indices[] = { 0, 5, 1, 0, 3, 5 };

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    m_NumOfElements = sizeof(Indices) / sizeof(Indices[0]);
}

bool HUDPlugin::initShaders() {

    if (!m_Shaders->Init()) {
        return false;
    }

    CreateVertexBuffer();

    return true;
}

bool HUDPlugin::Init(int32_t width, int32_t height) {

    LOGI("in HUD init\n");

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

        m_Frame = 1;

#define FPS 30.0
        if(FPS > 0.0001f) {
            m_FrameDelay = (1.0f/FPS) * 1000ull;
            m_FrameTimeout = rx_hrtime() + m_FrameDelay;
        }

        return true;
    }
    LOGE("fail to initialize HUD shader\n");

    return false;
}

// TODO: put to time.cpp
uint64_t HUDPlugin::rx_hrtime() {
    timeval t;
    gettimeofday(&t, NULL);
    uint64_t ret = t.tv_sec * 1000 + t.tv_usec / 1000; // to nano
    return ret;
}

bool HUDPlugin::Draw() {

    m_Shaders->Enable();

    uint64_t now = rx_hrtime();
    if(now < m_FrameTimeout) {
        // LOGW("now = %llu vs frame_timeout = %llu\n", now, frame_timeout);
        // return false;

    } else {
        m_Frame++;
        if(m_FrameDelay > 0) {
            m_FrameTimeout = rx_hrtime() + m_FrameDelay;
        }
    }

    m_Shaders->SetUniformGlobalTime((GLfloat) m_Frame);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glVertexAttribPointer(m_Shaders->GetAttrPosition(), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(m_Shaders->GetAttrPosition());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, m_NumOfElements, GL_UNSIGNED_BYTE, 0);

    return true;
}

int32_t HUDPlugin::KeyHandler(AInputEvent *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS HUDPlugin::status() {
    return my_status; // example of never finish a plugin
}
