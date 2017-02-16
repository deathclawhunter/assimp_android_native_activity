#define LOG_TAG "SpritePlugin"

#include <math.h>
#include <string>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "AppTechnique.h"
#include "ogldev_glut_backend.h"
#include "AppMesh.h"
#include "SpritePlugin.h"

using namespace std;

#include "GLError.h"
#include "Player.h"

SpritePlugin::SpritePlugin() {
    m_DirectionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
    m_DirectionalLight.AmbientIntensity = 0.55f;
    m_DirectionalLight.DiffuseIntensity = 0.9f;
    m_DirectionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

    m_Status = PLUGIN_STATUS_INIT_LATER;

    m_Oct = new Octree(256);
}

SpritePlugin::~SpritePlugin() {
    SAFE_DELETE(m_Oct);

    for (int i = 0; i < m_NumMesh; i++) {
        SAFE_DELETE(m_Meshes[i]);
    }
}

bool SpritePlugin::Init(string mesh[], int numMesh, int w, int h) {

    InteractivePlugin::Init(w, h);

    m_Width = w;
    m_Height = h;

    AppCamera::GetInstance(w, h); // initialize camera
    if (!m_Renderer.Init()) {
        LOGE("Error initializing the lighting technique\n");
        m_Status = PLUGIN_STATUS_INIT_FAIL;
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
            m_Status = PLUGIN_STATUS_INIT_FAIL;
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

    m_Status = PLUGIN_STATUS_NEXT;

    return true;
}

void SpritePlugin::ResetAnimation() {

    for (int i = 0; i < m_NumMesh; i++) {
        float start = 0;
        float end = m_Meshes[i]->AnimationInSeconds();

        /**
         * A simple spec used here for sprite animation creating
         * the first 1/3 animation is for idle
         * the second 1/3 animation is for running
         * the third 1/3 animation is for attack
         */
        if (m_SpriteStatus == SPRITE_STATUS_IDLE) {
            m_Meshes[i]->SetAnimationStartInSeconds(start);
            m_Meshes[i]->SetAnimationEndInSeconds(end / 3.0f);
        } else if (m_SpriteStatus == SPRITE_STATUS_RUN) {
            m_Meshes[i]->SetAnimationStartInSeconds(start / 3.0f);
            m_Meshes[i]->SetAnimationEndInSeconds(end * 2.0f / 3.0f);
        } else { // Attack
            m_Meshes[i]->SetAnimationStartInSeconds(start * 2.0f / 3.0f);
            m_Meshes[i]->SetAnimationEndInSeconds(end);
        }
    }

    ResetRunningTime();
}

void SpritePlugin::renderScene() {

    m_Renderer.Enable();

    AppCamera::GetInstance()->OnRender();

    m_Renderer.SetEyeWorldPos(AppCamera::GetInstance()->GetPos());

    Pipeline p;
    p.SetCamera(AppCamera::GetInstance()->GetPos(),
                AppCamera::GetInstance()->GetTarget(),
                AppCamera::GetInstance()->GetUp());
    p.SetPerspectiveProj(AppCamera::GetInstance()->GetPersProjInfo());
    p.Scale(1.0f, 1.0f, 1.0f);

    p.WorldPos(Player::GetInstance()->GetPosition());
    p.Rotate(Player::GetInstance()->GetRotation());

    Matrix4f wvp = p.GetWVPTrans();
    m_Renderer.SetWVP(wvp);
    m_Renderer.SetWorldMatrix(p.GetWorldTrans());

    m_Oct->SetTransform(wvp);
    m_Oct->Purge();
    for (int j = 0; j < m_NumMesh; j++) {

        vector<Matrix4f> Transforms;
        if (m_Meshes[j]->NumBones() > 0) {

            m_Meshes[j]->BoneTransform(GetRunningTime(), Transforms);
        } else {
            // use identity bone for static mesh
            Transforms.resize(1);
            Transforms[0].InitIdentity();
        }
        for (uint i = 0; i < Transforms.size(); i++) {
            m_Renderer.SetBoneTransform(i, Transforms[i]);
        }
        m_Renderer.SetWVP(wvp);

#if DEBUG_POSITION
        m_Meshes[j]->Simulate(Transforms, wvp);
        vector<Vector3f> result = m_Meshes[j]->GetEndPositions();
        Vector3f bound[2];
        GetBound(result, bound);

        LOGI(">>>>>>>>>>>>>>>>>>>>>>>> bound in shader");
        bound[0].Print();
        bound[1].Print();
        LOGI(">>>>>>>>>>>>>>>>>>>>>>>> end of bound in shader");

        m_Oct->AddMesh(m_Meshes[j]);

        m_Meshes[j]->Render();
#else
        m_Meshes[j]->Render();
#endif
    }
}

bool SpritePlugin::Draw() {
    renderScene();
    return true;
}

IPlugin::PLUGIN_STATUS SpritePlugin::Status() {
    return m_Status; // this is mainloop scene, so return loop me always
}

void SpritePlugin::SetSpriteStatus(SpriteStatus status) {
    if (m_SpriteStatus != status) {
        m_SpriteStatus = status;
        ResetAnimation();
    }
}

SpritePlugin::SpriteStatus SpritePlugin::GetSpriteStatus() {
    return m_SpriteStatus;
}

#if DEBUG_POSITION
void ScenePlugin::GetBound(vector<Vector3f> ary, Vector3f* ret) {
    ret[0] = ary[0];
    ret[1] = ary[0];

    for (int i = 1; i < ary.size(); i++) {
        if (ary[i].x < ret[0].x) {
            ret[0].x = ary[i].x;
        }

        if (ary[i].y < ret[0].y) {
            ret[0].y = ary[i].y;
        }

        if (ary[i].z < ret[0].z) {
            ret[0].z = ary[i].z;
        }

        if (ary[i].x > ret[1].x) {
            ret[1].x = ary[i].x;
        }

        if (ary[i].y > ret[1].y) {
            ret[1].y = ary[i].y;
        }

        if (ary[i].z > ret[1].z) {
            ret[1].z = ary[i].z;
        }
    }
}

#endif

