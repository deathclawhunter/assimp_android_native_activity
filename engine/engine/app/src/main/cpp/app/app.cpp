#define LOG_TAG "ENGINE_APP"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "app_technique.h"
#include "ogldev_glut_backend.h"
#include "app_mesh.h"

using namespace std;

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

#include "GLError.h"
#include "app.h"

extern int scroll(float touchX, float lastTouchX);

SceneEngine::SceneEngine() {
    m_pGameCamera = NULL;
    m_DirectionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_DirectionalLight.AmbientIntensity = 0.55f;
    m_DirectionalLight.DiffuseIntensity = 0.9f;
    m_DirectionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    m_PersProjInfo.FOV = 60.0f;
    m_PersProjInfo.Height = WINDOW_WIDTH;
    m_PersProjInfo.Width = WINDOW_HEIGHT;
    m_PersProjInfo.zNear = 1.0f;
    m_PersProjInfo.zFar = 100.0f;

    m_Position = Vector3f(0.0f, 0.0f, 6.0f);
}

SceneEngine::~SceneEngine() {
    SAFE_DELETE(m_pGameCamera);
}

bool SceneEngine::Init(string mesh[], int numMesh, int w, int h) {
    Vector3f Pos(0.0f, 3.0f, -1.0f);
    Vector3f Target(0.0f, 0.0f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

    m_PersProjInfo.Width = w;
    m_PersProjInfo.Height = h;

    m_pGameCamera = new Camera(w, h, Pos, Target, Up);
    if (!m_Renderer.Init()) {
        LOGE("Error initializing the lighting technique\n");
        return false;
    }
    m_Renderer.Enable();
    m_Renderer.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
    m_Renderer.SetDirectionalLight(m_DirectionalLight);
    m_Renderer.SetMatSpecularIntensity(0.0f);
    m_Renderer.SetMatSpecularPower(0);

    m_NumMesh = min(MAX_NUM_MESHES, numMesh);
    for (int i = 0; i < m_NumMesh; i++) {
        if (!m_Meshes[i].LoadMesh(mesh[i])) {
            LOGE("fail to load mesh %s\n", mesh[i].c_str());
            return false;
        }
    }

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");

    // TODO: implement font management and text
    /* if (!m_fontRenderer.InitFontRenderer()) {
        return false;
    } */
    return true;
}

void SceneEngine::renderScene() {
    CalcFPS();

    m_pGameCamera->OnRender();

    m_Renderer.SetEyeWorldPos(m_pGameCamera->GetPos());

    Pipeline p;
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_PersProjInfo);
    p.Scale(0.1f, 0.1f, 0.1f);

    Vector3f Pos(m_Position);
    p.WorldPos(Pos);
    p.Rotate(270.0f, 180.0f, 0.0f);

    m_Renderer.SetWVP(p.GetWVPTrans());
    m_Renderer.SetWorldMatrix(p.GetWorldTrans());

    /* glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(false);
    glDepthFunc(GL_EQUAL); */

    for (int j = 0; j < m_NumMesh; j++) {

        vector<Matrix4f> Transforms;
        if (m_Meshes[j].NumBones() > 0) {
            float RunningTime = GetRunningTime();
            m_Meshes[j].BoneTransform(RunningTime, Transforms);
        } else {
            // use identity bone for static mesh
            Transforms.resize(1);
            Transforms[0].InitIdentity();
        }
        for (uint i = 0; i < Transforms.size(); i++) {
            m_Renderer.SetBoneTransform(i, Transforms[i]);
        }
        m_Meshes[j].Render();
    }

    RenderFPS();
}

float SceneEngine::getTouchX() {
    return touchX;
}

void SceneEngine::setTouchX(float touchX) {
    SceneEngine::touchX = touchX;
}

float SceneEngine::getTouchY() {
    return touchY;
}

void SceneEngine::setTouchY(float touchY) {
    SceneEngine::touchY = touchY;
}

/**
 * Callbacks for main
 */
void appDrawFrame(void *pContext) {
    SceneEngine *pApp = (SceneEngine *) pContext;
    pApp->renderScene();
}

void* appInit(int32_t w, int32_t h) {

    LOGI("in App init:\n");

    SceneEngine *pApp = new SceneEngine();

    std::string str[3];
    str[0].append("boblampclean.md5mesh");
    str[1].append("marcus.dae");
    // str[0].append("ArmyPilot.dae");
    // str.append("sf2arms.dae");
    // str[0].append("monkey.dae");
    str[2].append("untitled.dae");
    // str[1].append("untitled2.dae");

    if (pApp->Init(str, 3, w, h)) {
        return pApp;
    }

    delete(pApp);
    return NULL;
}

int32_t appKeyHandler(void *pContext, AInputEvent *event) {
    SceneEngine *engine = (SceneEngine *) pContext;
    int32_t action = AMotionEvent_getAction(event);
    if (action == AMOTION_EVENT_ACTION_MOVE) {
        float touchX = AMotionEvent_getX(event, 0);
        float touchY = AMotionEvent_getY(event, 0);

        int scrollFlag = scroll(touchX, engine->getTouchX());
        if (scrollFlag == 0) {
            // LOGI("scroll left\n");
            engine->KeyboardCB(OGLDEV_KEY_LEFT, OGLDEV_KEY_STATE_PRESS);
        } else if (scrollFlag == 1) {
            // LOGI("scroll right\n");
            engine->KeyboardCB(OGLDEV_KEY_RIGHT, OGLDEV_KEY_STATE_PRESS);
        }

        engine->setTouchX(touchX);
        engine->setTouchY(touchY);
    }
    return 1;
}