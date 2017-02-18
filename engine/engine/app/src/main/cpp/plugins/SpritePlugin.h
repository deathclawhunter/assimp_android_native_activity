#ifndef _SPRITE_PLUGIN_H_
#define _SPRITE_PLUGIN_H_

#include <GL/freeglut.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "AppTechnique.h"
#include "AppMesh.h"
#include "OCTree.h"
#include "AppCamera.h"
#include "InteractivePlugin.h"

using namespace std;

// cap for meshes in a scene
#define MAX_NUM_MESHES 10

class Weapon {
public:
    float Damage;
    float CoolDown;
    int AmmoCapacity;
    int AmmoCount;
};

#define MAX_WEAPON_SLOTS 3

class SpritePlugin : public InteractivePlugin {
public:

    // Plugin API
    bool Draw();

    IPlugin::PLUGIN_STATUS Status();

    SpritePlugin();

    ~SpritePlugin();

    bool Init(string mesh[], int numMesh, int w, int h);

    void RenderScene();

    enum SpriteStatus {
        SPRITE_STATUS_IDLE,
        SPRITE_STATUS_ATTACK,
        SPRITE_STATUS_RUN
    };

    void SetSpriteStatus(SpriteStatus status);
    SpriteStatus GetSpriteStatus();

private:
    void ResetAnimation();
    void AutoAttackFromPlayer(AppMesh *Mesh, vector<Matrix4f> BoneTransforms, Matrix4f& WVP);
    void AutoAttackFromMech(AppMesh *Mesh, vector<Matrix4f> BoneTransforms, Matrix4f& WVP);
    void GetShot(float Damage);
    void Dead();
    float GetDamage();
    bool CanAttack();

private:
    AppTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    AppMesh *m_Meshes[MAX_NUM_MESHES] = {0};
    int m_NumMesh;
    int m_Width, m_Height;
    IPlugin::PLUGIN_STATUS m_Status;
    Octree *m_Oct = NULL;
    SpriteStatus m_SpriteStatus = SPRITE_STATUS_IDLE;

    float m_Life = 10.0f;
    float m_DR = 1.0f;
    float m_CurrentLife = m_Life;

#if DEBUG_POSITION
    Matrix4f m_OrthogonalMatrix;
    bool m_OrthoMatrixInitialized = false;
    void GetBound(vector<Vector3f> ary, Vector3f* ret);
#endif


    Weapon m_Weapons[MAX_WEAPON_SLOTS];
    int m_Equiped = 0;
    uint64_t m_LastFire = 0;
};

#endif /* _SPRITE_PLUGIN_H_ */
