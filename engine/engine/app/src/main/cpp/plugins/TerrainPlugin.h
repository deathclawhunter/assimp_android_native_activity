#ifndef _TERRAIN_PLUGIN_H_
#define _TERRAIN_PLUGIN_H_

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

class TerrainPlugin : public InteractivePlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    IPlugin::PLUGIN_STATUS status();

    TerrainPlugin();

    ~TerrainPlugin();

    bool Init(string mesh[], int numMesh, int w, int h);

    void renderScene();

private:
    AppTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    AppMesh *m_Meshes[MAX_NUM_MESHES] = {0};
    int m_NumMesh;
    int m_width, m_height;
    const float m_Near = 1.0f;
    const float m_Far = 2000.0f;

private:
    IPlugin::PLUGIN_STATUS sceneStatus;
};

#endif /* _TERRAIN_PLUGIN_H_ */
