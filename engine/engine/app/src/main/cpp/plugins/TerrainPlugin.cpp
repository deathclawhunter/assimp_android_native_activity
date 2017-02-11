#define LOG_TAG "TerrainPlugin"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "AppTechnique.h"
#include "ogldev_glut_backend.h"
#include "AppMesh.h"

using namespace std;

#include "GLError.h"
#include "TerrainPlugin.h"
#include "Player.h"

TerrainPlugin::TerrainPlugin() {
    m_DirectionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_DirectionalLight.AmbientIntensity = 0.55f;
    m_DirectionalLight.DiffuseIntensity = 0.9f;
    m_DirectionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    sceneStatus = PLUGIN_STATUS_INIT_LATER;
}

TerrainPlugin::~TerrainPlugin() {

    for (int i = 0; i < m_NumMesh; i++) {
        SAFE_DELETE(m_Meshes[i]);
    }
}

bool TerrainPlugin::Init(string mesh[], int numMesh, int w, int h) {

    m_width = w;
    m_height = h;

    AppCamera::GetInstance(w, h); // initialize camera
    if (!m_Renderer.Init()) {
        LOGE("Error initializing the lighting technique\n");
        sceneStatus = PLUGIN_STATUS_INIT_FAIL;
        return false;
    }
    m_Renderer.Enable();
    m_Renderer.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
    m_Renderer.SetDirectionalLight(m_DirectionalLight);
    m_Renderer.SetMatSpecularIntensity(0.0f);
    m_Renderer.SetMatSpecularPower(0);

    m_NumMesh = min(MAX_NUM_MESHES, numMesh);
    for (int i = 0; i < m_NumMesh; i++) {
        m_Meshes[i] = new AppMesh(&m_Renderer);
        if (!m_Meshes[i]->LoadMesh(mesh[i])) {
            LOGE("fail to load mesh %s\n", mesh[i].c_str());
            sceneStatus = PLUGIN_STATUS_INIT_FAIL;
            return false;
        }
        // float start = m_Meshes[i]->AnimationInSeconds() / 2.0f;
        // float end = m_Meshes[i]->AnimationInSeconds();

        /* float start = 0;
        float end = m_Meshes[i]->AnimationInSeconds() / 2.0f;

        m_Meshes[i]->SetAnimationStartInSeconds(start);
        m_Meshes[i]->SetAnimationEndInSeconds(end); */
    }

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");

    sceneStatus = PLUGIN_STATUS_NEXT;

    return true;
}

void TerrainPlugin::renderScene() {

    m_Renderer.Enable();

    AppCamera::GetInstance()->OnRender();

    m_Renderer.SetEyeWorldPos(AppCamera::GetInstance()->GetPos());

    Pipeline p;
    p.SetCamera(AppCamera::GetInstance()->GetPos(),
                AppCamera::GetInstance()->GetTarget(),
                AppCamera::GetInstance()->GetUp());
    PersProjInfo tmp = AppCamera::GetInstance()->GetPersProjInfo();
    tmp.zNear = m_Near; // SkyBox has bigger near/far value
    tmp.zFar = m_Far;
    p.SetPerspectiveProj(tmp);
    p.Scale(250.0f, 250.0f, 250.0f); // specialized for terrain bigger
    p.WorldPos(Player::GetInstance()->GetPosition());
    p.Rotate(Player::GetInstance()->GetRotation());

    Matrix4f wvp = p.GetWVPTrans();
    m_Renderer.SetWVP(wvp);
    m_Renderer.SetWorldMatrix(p.GetWorldTrans());

    glEnable(GL_DEPTH_TEST);
    glDepthRangef(m_Near, m_Far);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS );
    glEnable(GL_DEPTH_TEST);

    for (int j = 0; j < m_NumMesh; j++) {

        vector<Matrix4f> Transforms;
        if (m_Meshes[j]->NumBones() > 0) {

            float RunningTime = GetRunningTime();
            m_Meshes[j]->BoneTransform(RunningTime, Transforms);
        } else {
            // use identity bone for static mesh
            Transforms.resize(1);
            Transforms[0].InitIdentity();
        }
        for (uint i = 0; i < Transforms.size(); i++) {
            m_Renderer.SetBoneTransform(i, Transforms[i]);
        }
        m_Renderer.SetWVP(wvp);
        m_Meshes[j]->Render();
    }
}

bool TerrainPlugin::Init(int32_t width, int32_t height) {
    LOGI("in Scene init:\n");

    InteractivePlugin::Init(width, height);

    std::string str[2];
    // str[0].append("box.dae");
    // str[0].append("boblampclean.md5mesh");
    // str[0].append("mech0_animated.dae");
    // str[0].append("mech1_animated.fbx");
    // str[0].append("marcus.dae");
    // str[0].append("ArmyPilot.dae");
    // str.append("sf2arms.dae");
    // str[0].append("monkey.dae");
    // str[0].append("untitled.dae");
    // str[0].append("mech0.dae");
    str[0].append("scene0_001.dae");
    // str[1].append("scene0_002.dae");

    // std::string str2[1];
    // str2[0].append("menu.dae");
    // str[1].append("untitled2.dae");

    if (Init(str, 1, width, height)) {
        return true;
    }

    return false;
}

bool TerrainPlugin::Draw() {
    renderScene();
    return true;
}

IPlugin::PLUGIN_STATUS TerrainPlugin::status() {
    return sceneStatus; // this is mainloop scene, so return loop me always
}


