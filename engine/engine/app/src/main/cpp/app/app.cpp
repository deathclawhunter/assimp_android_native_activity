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

ScenePlugin::ScenePlugin() {
    m_pGameCamera = NULL;
    m_DirectionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_DirectionalLight.AmbientIntensity = 0.55f;
    m_DirectionalLight.DiffuseIntensity = 0.9f;
    m_DirectionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    m_PersProjInfo.FOV = 60.0f;
    m_PersProjInfo.Height = WINDOW_HEIGHT;
    m_PersProjInfo.Width = WINDOW_WIDTH;
    CalculateCenterOfRightHalf();
    m_PersProjInfo.zNear = 1.0f;
    m_PersProjInfo.zFar = 100.0f;

    m_Position = Vector3f(0.0f, 0.0f, 6.0f);
}

ScenePlugin::~ScenePlugin() {
    SAFE_DELETE(m_pGameCamera);
}

void ScenePlugin::CalculateCenterOfRightHalf() {
    m_RCenterX = m_PersProjInfo.Width * 3.0f / 4.0f;
    m_RCenterY = m_PersProjInfo.Height / 2.0f;
}

bool ScenePlugin::Init(string mesh[], int numMesh, int w, int h) {
    Vector3f Pos(0.0f, 3.0f, -1.0f);
    Vector3f Target(0.0f, 0.0f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

    m_PersProjInfo.Width = w;
    m_PersProjInfo.Height = h;

    // calculate center point of right half of the screen
    CalculateCenterOfRightHalf();

    m_pGameCamera = new Camera(w, h, Pos, Target, Up);
    if (!m_Renderer.Init()) {
        LOGE("Error initializing the lighting technique\n");
        return false;
    }
    m_pGameCamera->SetStep(GAME_STEP_SCALE);
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

void ScenePlugin::renderScene() {
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

OGLDEV_KEY ScenePlugin::ConvertKey(float x, float y) {

    float diffX, diffY, ratioX, ratioY;

    diffX = m_RCenterX - x;
    diffY = m_RCenterY - y;
    ratioX = diffX / m_PersProjInfo.Width;
    ratioY = diffY / m_PersProjInfo.Height;
    LOGI("m_RCenterX = %f, m_RCenterY = %f\n", m_RCenterX, m_RCenterY);

    if (abs(diffX) > MINIMAL_MOVE_DIFF && abs(ratioX) > abs(ratioY)) {
        if (diffX > 0) {
            LOGI("LEFT");
            return OGLDEV_KEY_LEFT;
        } else {
            LOGI("RIGHT");
            return OGLDEV_KEY_RIGHT;
        }
    } else if (abs(diffY) > MINIMAL_MOVE_DIFF && abs(ratioY) > abs(ratioX)) {

        if (ENABLE_UP_N_DOWN) {
            if (diffY > 0) {
                LOGI("UP");
                return OGLDEV_KEY_UP;
            } else {
                LOGI("DOWN");
                return OGLDEV_KEY_DOWN;
            }
        }
    }

    return OGLDEV_KEY_UNDEFINED;
}

void ScenePlugin::ResetMouse() {
    m_pGameCamera->ResetMouse();
}

float ScenePlugin::DistToCenter(float x, float y) {
    // simple algorithm to calculate right half or left half screen was pressed
    return abs(x - m_RCenterX) + abs(y - m_RCenterY);
}

bool ScenePlugin::Init(int32_t width, int32_t height) {
    LOGI("in App init:\n");

    std::string str[2];
    str[1].append("boblampclean.md5mesh");
    // str[1].append("marcus.dae");
    // str[0].append("ArmyPilot.dae");
    // str.append("sf2arms.dae");
    // str[0].append("monkey.dae");
    str[0].append("untitled.dae");
    // str[1].append("untitled2.dae");

    if (Init(str, 2, width, height)) {
        return true;
    }

    return false;
}

bool ScenePlugin::Draw() {
    renderScene();
    return true;
}

int32_t ScenePlugin::KeyHandler(AInputEvent *event) {
    int32_t action = AMotionEvent_getAction(event);
    size_t count = AMotionEvent_getPointerCount(event);
    if (count == 1) { // single finger touch
        if (action == AMOTION_EVENT_ACTION_MOVE) {
            float touchX = AMotionEvent_getX(event, 0);
            float touchY = AMotionEvent_getY(event, 0);

            PassiveMouseCB(touchX, touchY);
        } else if (action == AMOTION_EVENT_ACTION_UP) {
            LOGI("Reset mouse");
            ResetMouse();
        }
    } else if (count == 2) {
        if (action == AMOTION_EVENT_ACTION_MOVE) {
            float touch2X = AMotionEvent_getX(event, 1);
            float touch2Y = AMotionEvent_getY(event, 1);

            float touch1X = AMotionEvent_getX(event, 0);
            float touch1Y = AMotionEvent_getY(event, 0);

            if (DistToCenter(touch1X, touch1Y) >
                DistToCenter(touch2X, touch2Y)) {
                PassiveKeyCB(touch2X, touch2Y);
            } else {
                PassiveKeyCB(touch1X, touch1Y);
            }
        }
    }

    return 1;
}
