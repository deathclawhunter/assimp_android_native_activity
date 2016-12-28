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


    virtual void PassiveMouseCB(int x, int y) {
        m_pGameCamera->OnMouse(x, y);
    }

private:
    AppTechnique m_Renderer;
    Camera *m_pGameCamera;
    DirectionalLight m_DirectionalLight;
    AppMesh m_Meshes[MAX_NUM_MESHES];
    int m_NumMesh;
    Vector3f m_Position;
    PersProjInfo m_PersProjInfo;
public:
    float getTouchX();
    void setTouchX(float touchX);
    float getTouchY();
    void setTouchY(float touchY);

private:
    float touchX, touchY;
};
