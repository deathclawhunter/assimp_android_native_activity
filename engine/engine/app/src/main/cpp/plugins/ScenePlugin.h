#include <math.h>
#include <string>
#include <android_native_app_glue.h>
#include <GL/freeglut.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "AppTechnique.h"
#include "plugin.h"
#include "AppMesh.h"
#include "OCTree.h"
#include "AppCamera.h"
#include "ogldev_glut_backend.h"

using namespace std;

// cap for meshes in a scene
#define MAX_NUM_MESHES 10

class ScenePlugin : public ICallbacks, public OgldevApp, public IPlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(AInputEvent *event);

    IPlugin::PLUGIN_STATUS status();

    ScenePlugin();

    ~ScenePlugin();

#if ENABLE_IN_SCENE_HUD
    bool Init(string mesh[], int numMesh, string hudMesh[], int numHudMesh, int w, int h);
#else

    bool Init(string mesh[], int numMesh, int w, int h);

#endif

    void renderScene();

    virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State) {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                // GLUTBackendLeaveMainLoop();
                glutLeaveMainLoop();
                break;
            default:
                AppCamera::GetInstance()->OnKeyboard(OgldevKey);
        }
    }


    void PassiveMouseCB(int x, int y) {
        AppCamera::GetInstance()->OnMouse(x, y);
    }

    OGLDEV_KEY ConvertKey(float x, float y);

    void PassiveKeyCB(float x, float y) {
        AppCamera::GetInstance()->OnKeyboard(ConvertKey(x, y));
    }

    void ResetMouse();

    float DistToCenter(float x, float y);

private:
    AppTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    AppMesh *m_Meshes[MAX_NUM_MESHES] = {0};
    int m_NumMesh;
    float m_RCenterX, m_RCenterY; // Center point of right half of the screen
    int m_width, m_height;

private:
    const float MINIMAL_MOVE_DIFF = 0.1f;
    const bool ENABLE_UP_N_DOWN = true; // In mobile game, we disable up and down
    void CalculateCenterOfRightHalf(int width, int height);
    IPlugin::PLUGIN_STATUS sceneStatus;
    Octree *m_Oct = NULL;

#if DEBUG_POSITION
    Matrix4f m_OrthogonalMatrix;
    bool m_OrthoMatrixInitialized = false;
    void GetBound(vector<Vector3f> ary, Vector3f* ret);
#endif
};
