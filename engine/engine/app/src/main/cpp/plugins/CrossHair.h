#include <GLES/gl.h>
#include <android_native_app_glue.h>
#include <ogldev/ogldev_texture.h>

#include "GL3Stub.h"
#include "plugin.h"

#include "Technique.h"
#include "GLError.h"

#define LOG_TAG "CrossHair"

#include "AppLog.h"
#include "ogldev_util.h"
#include "FlashEffect.h"

class CrossHairPlugin : public FlashEffectPlugin {

public:

    CrossHairPlugin() {
        const char *Frames[1];

        Frames[0] = "CrossHair.png";

        Init(1, Frames);
    }

};


