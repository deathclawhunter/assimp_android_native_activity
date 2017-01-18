#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <cstdint>
#include <android/input.h>

class IPlugin {
public:
    virtual bool Init(int32_t width, int32_t height) = 0;
    virtual bool Draw() = 0;
    virtual int32_t KeyHandler(AInputEvent *event) = 0;
    enum PLUGIN_STATUS {
        PLUGIN_STATUS_FINISHED, // I am done with this plugin, unplug me from the plugin list
        PLUGIN_STATUS_LOOP_ME, // Do not process next plugin until I am done
        PLUGIN_STATUS_NEXT, // Go to next plugin after I am done
        PLUGIN_STATUS_INIT_LATER,
        PLUGIN_STATUS_INIT_RIGHT_NOW,
        PLUGIN_STATUS_INIT_FAIL
    };
    virtual IPlugin::PLUGIN_STATUS status() = 0;

public:
    IPlugin *next = NULL;
};

#endif // _PLUGIN_H_
