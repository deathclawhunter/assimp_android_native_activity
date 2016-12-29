#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "ogldev_glut_backend.h"
#include "app_technique.h"

using namespace std;

// cap for meshes in a scene
#define MAX_NUM_MESHES 10

class SceneEngine : public ICallbacks, public OgldevApp {
public:

    SceneEngine();
    ~SceneEngine();
    bool Init(string mesh[], int numMesh, int w, int h);
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


    void PassiveMouseCB(int x, int y) {
        m_pGameCamera->OnMouse(x, y);
    }

    OGLDEV_KEY ConvertKey(float x, float y);

    void PassiveKeyCB(float x, float y) {
        m_pGameCamera->OnKeyboard(ConvertKey(x, y));
    }

    void ResetMouse();

    float DistToCenter(float x, float y);

private:
    AppTechnique m_Renderer;
    Camera *m_pGameCamera;
    DirectionalLight m_DirectionalLight;
    AppMesh m_Meshes[MAX_NUM_MESHES];
    int m_NumMesh;
    Vector3f m_Position;
    PersProjInfo m_PersProjInfo;
    float m_RCenterX, m_RCenterY; // Center point of right half of the screen

private:
    const float MINIMAL_MOVE_DIFF = 0.1f;
    const bool ENABLE_UP_N_DOWN = true; // In mobile game, we disable up and down
    void CalculateCenterOfRightHalf();
    const float GAME_STEP_SCALE = 0.5f;
};
