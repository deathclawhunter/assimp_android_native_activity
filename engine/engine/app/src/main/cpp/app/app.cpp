#define LOG_TAG "ENGINE_APP"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "skinning_technique.h"
#include "ogldev_glut_backend.h"
#include "ogldev_skinned_mesh.h"

using namespace std;

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

#include "GLError.h"
#include "app.h"

extern int scroll(float touchX, float lastTouchX);

SceneEngine::SceneEngine() {
    m_pGameCamera = NULL;
    m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_directionalLight.AmbientIntensity = 0.55f;
    m_directionalLight.DiffuseIntensity = 0.9f;
    m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    m_persProjInfo.FOV = 60.0f;
    m_persProjInfo.Height = WINDOW_WIDTH;
    m_persProjInfo.Width = WINDOW_HEIGHT;
    m_persProjInfo.zNear = 1.0f;
    m_persProjInfo.zFar = 100.0f;

    m_position = Vector3f(0.0f, 0.0f, 6.0f);
}

SceneEngine::~SceneEngine() {
    SAFE_DELETE(m_pGameCamera);
}

bool SceneEngine::Init(string staticMesh[], int numStaticMesh,
          string skinnedMesh[], int numSkinnedMesh,
          int w, int h) {
    Vector3f Pos(0.0f, 3.0f, -1.0f);
    Vector3f Target(0.0f, 0.0f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

    m_persProjInfo.Width = w;
    m_persProjInfo.Height = h;

    m_pGameCamera = new Camera(w, h, Pos, Target, Up);

    m_numSkinnedMesh = min(MAX_NUM_SKINNED_MESHES, numSkinnedMesh);
    for (int i = 0; i < m_numSkinnedMesh; i++) {

        if (!m_pSkinnedEffect[i].Init()) {
            LOGE("Error initializing the lighting technique\n");
            return false;
        }

        m_pSkinnedEffect[i].Enable();
        m_pSkinnedEffect[i].SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pSkinnedEffect[i].SetDirectionalLight(m_directionalLight);
        m_pSkinnedEffect[i].SetMatSpecularIntensity(0.0f);
        m_pSkinnedEffect[i].SetMatSpecularPower(0);

        if (!m_skinnedMesh[i].LoadMesh(skinnedMesh[i])) {
            LOGE("fail to load mesh %s\n", skinnedMesh[i].c_str());
            return false;
        }
    }

    m_numStaticMesh = min(MAX_NUM_STATIC_MESHES, numStaticMesh);
    for (int i = 0; i < m_numStaticMesh; i++) {

        if (!m_pStaticEffect[i].Init()) {
            LOGE("Error initializing the lighting technique\n");
            return false;
        }

        m_pStaticEffect[i].Enable();
        m_pStaticEffect[i].SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pStaticEffect[i].SetDirectionalLight(m_directionalLight);
        m_pStaticEffect[i].SetMatSpecularIntensity(0.0f);
        m_pStaticEffect[i].SetMatSpecularPower(0);

        if (!m_staticMesh[i].LoadMesh(staticMesh[i])) {
            LOGE("fail to load mesh %s\n", staticMesh[i].c_str());
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

void SceneEngine::drawSkinnedMeshes() {
    for (int j = 0; j < m_numSkinnedMesh; j++) {

        m_pSkinnedEffect[j].Enable();

        vector<Matrix4f> Transforms;

        float RunningTime = GetRunningTime();

        m_skinnedMesh[j].BoneTransform(RunningTime, Transforms);

        for (uint i = 0; i < Transforms.size(); i++) {
            m_pSkinnedEffect[j].SetBoneTransform(i, Transforms[i]);
        }

        m_pSkinnedEffect[j].SetEyeWorldPos(m_pGameCamera->GetPos());

        Pipeline p;
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
        p.Scale(0.1f, 0.1f, 0.1f);

        Vector3f Pos(m_position);
        p.WorldPos(Pos);
        p.Rotate(270.0f, 180.0f, 0.0f);

        m_pSkinnedEffect[j].SetWVP(p.GetWVPTrans());
        m_pSkinnedEffect[j].SetWorldMatrix(p.GetWorldTrans());

        /* glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(false);
        glDepthFunc(GL_EQUAL); */

        m_skinnedMesh[j].Render();
    }
}

void SceneEngine::drawStaticMeshes() {
    for (int j = 0; j < m_numStaticMesh; j++) {
        m_pStaticEffect[j].Enable();

        m_pStaticEffect[j].SetEyeWorldPos(m_pGameCamera->GetPos());

        Pipeline p;
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
        p.Scale(0.1f, 0.1f, 0.1f);

        Vector3f Pos(m_position);
        p.WorldPos(Pos);
        p.Rotate(270.0f, 180.0f, 0.0f);

        m_pStaticEffect[j].SetWVP(p.GetWVPTrans());
        m_pStaticEffect[j].SetWorldMatrix(p.GetWorldTrans());

        m_staticMesh[j].Render();
    }
}

void SceneEngine::renderScene() {
    CalcFPS();

    m_pGameCamera->OnRender();

    drawStaticMeshes();
    drawSkinnedMeshes();

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

    std::string str[2];
    str[0].append("boblampclean.md5mesh");
    // str[1].append("marcus.dae");
    // str[0].append("ArmyPilot.dae");
    // str.append("sf2arms.dae");
    // str[0].append("monkey.dae");
    // str[0].append("untitled.dae");
    str[1].append("untitled2.dae");

    if (pApp->Init(str, 2, NULL, 0, w, h)) {
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