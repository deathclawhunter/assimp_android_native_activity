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
#include "SpritePlugin.h"

using namespace std;

// cap for meshes in a scene
#define MAX_NUM_MESHES 10

class MechPlugin : public SpritePlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height) {
        LOGI("in Target init:\n");

        std::string str[2];
        str[0].append("mech0_animated.dae");

        if (SpritePlugin::Init(str, 1, width, height)) {
            return true;
        }

        return false;
    }
};
