#include <GLES/gl.h>
#include <android_native_app_glue.h>
#include <ogldev/ogldev_texture.h>

#include "GL3Stub.h"
#include "plugin.h"

#include "Technique.h"
#include "GLError.h"

#define LOG_TAG "MuzzleEffect"

#include "AppLog.h"
#include "ogldev_util.h"
#include "FlashEffect.h"

class MuzzleEffectPlugin : public FlashEffectPlugin {

public:

    MuzzleEffectPlugin() {
        const char *Frames[5];

        Frames[0] = "image002.png";
        Frames[1] = "image003.png";
        Frames[2] = "image004.png";
        Frames[3] = "image006.png";
        Frames[4] = "image007.png";

        Init(5, Frames);
    }

};


