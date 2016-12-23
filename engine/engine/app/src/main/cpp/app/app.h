#if 1

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

// cap for meshes in a scene
#define MAX_NUM_SKINNED_MESHES 5
// TODO: #define MAX_NUM_STATIC_MESHES 20

class SceneEngine : public ICallbacks, public OgldevApp {
public:

    SceneEngine();
    ~SceneEngine();
    bool Init(string staticMesh[], int numStaticMesh,
              string skinnedMesh[], int numSkinnedMesh,
              int w, int h);
    void renderScene();
    virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State) {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                GLUTBackendLeaveMainLoop();
                break;
            default:
                m_pGameCamera->OnKeyboard(OgldevKey);
        }
    }


    virtual void PassiveMouseCB(int x, int y) {
        m_pGameCamera->OnMouse(x, y);
    }

private:
    SkinningTechnique m_pSkinnedEffect[MAX_NUM_SKINNED_MESHES];
    // StaticTechnique m_pStaticEffect[MAX_NUM_STATIC_MESHES];
    Camera *m_pGameCamera;
    DirectionalLight m_directionalLight;
    SkinnedMesh m_skinnedMesh[MAX_NUM_SKINNED_MESHES];
    int m_numSkinnedMesh;
    // StaticMesh m_staticMesh[MAX_NUM_STATIC_MESHES]; // Most likely those static mesh will never change
    Vector3f m_position;
    PersProjInfo m_persProjInfo;
public:
    float getTouchX();
    void setTouchX(float touchX);
    float getTouchY();
    void setTouchY(float touchY);

private:
    float touchX, touchY;
};

#endif