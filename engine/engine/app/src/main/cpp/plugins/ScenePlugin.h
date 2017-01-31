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

class ScenePlugin : public InteractivePlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    IPlugin::PLUGIN_STATUS status();

    ScenePlugin();

    ~ScenePlugin();

#if ENABLE_IN_SCENE_HUD
    bool Init(string mesh[], int numMesh, string hudMesh[], int numHudMesh, int w, int h);
#else

    bool Init(string mesh[], int numMesh, int w, int h);

#endif

    void renderScene();

private:
    AppTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    AppMesh *m_Meshes[MAX_NUM_MESHES] = {0};
    int m_NumMesh;
    int m_width, m_height;

private:
    IPlugin::PLUGIN_STATUS sceneStatus;
    Octree *m_Oct = NULL;

#if DEBUG_POSITION
    Matrix4f m_OrthogonalMatrix;
    bool m_OrthoMatrixInitialized = false;
    void GetBound(vector<Vector3f> ary, Vector3f* ret);
#endif
};
