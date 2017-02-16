#define LOG_TAG "SkyBoxPlugin"

#include "AppLog.h"

#include "AppCamera.h"

#include "ogldev_engine_common.h"
#include "ogldev_pipeline.h"

#include "GLError.h"
#include "SkyBoxPlugin.h"
#include "Player.h"

SkyBox::SkyBox() {
    m_DirectionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_DirectionalLight.AmbientIntensity = 0.55f;
    m_DirectionalLight.DiffuseIntensity = 0.9f;
    m_DirectionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    m_Status = PLUGIN_STATUS_INIT_LATER;
}

SkyBox::~SkyBox() {
    SAFE_DELETE(m_pMesh);
}

bool SkyBox::Init(int w, int h) {

    InteractivePlugin::Init(w, h);

    if (!m_Renderer.Init("skybox.vs", "skybox.fs")) {
        LOGE("Error initializing the skybox shaders\n");
        m_Status = PLUGIN_STATUS_INIT_FAIL;
        return false;
    }
    m_Renderer.Enable();
    m_Renderer.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
    m_Renderer.SetDirectionalLight(m_DirectionalLight);

    m_pMesh = new BasicMesh(&m_Renderer);
    if (!m_pMesh->LoadMesh("box.dae")) {
        LOGE("fail to load skybox mesh %s\n");
        m_Status = PLUGIN_STATUS_INIT_FAIL;
        return false;
    }

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");

    m_Status = PLUGIN_STATUS_NEXT;

    return true;
}

bool SkyBox::Draw() {

    m_Renderer.Enable();
    // m_Renderer.SetEyeWorldPos(AppCamera::GetInstance()->GetPos());

    Pipeline p;
    p.SetCamera(AppCamera::GetInstance()->GetPos(),
                AppCamera::GetInstance()->GetTarget(),
                AppCamera::GetInstance()->GetUp());
    PersProjInfo tmp = AppCamera::GetInstance()->GetPersProjInfo();
    tmp.zNear = m_Near; // SkyBox has bigger near/far value
    tmp.zFar = m_Far;
    p.SetPerspectiveProj(tmp);
    p.Scale(1000.0f, 1000.0f, 1000.0f); // specialized for skybox, 1000 times bigger
    p.WorldPos(Player::GetInstance()->GetPosition());
    p.Rotate(Player::GetInstance()->GetRotation());

    Matrix4f wvp = p.GetWVPTrans();
    m_Renderer.SetWVP(wvp);
    m_Renderer.SetWorldMatrix(p.GetWorldTrans());

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    m_pMesh->Render();
    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);

    return true;
}

IPlugin::PLUGIN_STATUS SkyBox::Status() {
    return m_Status; // this is mainloop scene, so return loop me always
}


