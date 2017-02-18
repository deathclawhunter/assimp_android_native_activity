#ifndef _MECH_PLUGIN_H_
#define _MECH_PLUGIN_H_

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

class MechPlugin : public SpritePlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height) {
        LOGI("in MeshPlugin init:\n");

        std::string str[2];
        str[0].append("mech0_animated.dae");

        if (SpritePlugin::Init(str, 1, width, height)) {
            return true;
        }

        return false;
    }
};

#endif /* _MECH_PLUGIN_H_ */
