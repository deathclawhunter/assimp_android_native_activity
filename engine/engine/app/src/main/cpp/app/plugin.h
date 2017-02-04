#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <cstdint>
#include <android/input.h>

class IPlugin {
public:
    enum ACTION_TYPE {
        ACTION_TYPE_MOVE,
        ACTION_TYPE_UP,
        ACTION_TYPE_ROTATE,
        ACTION_TYPE_RESET,
        ACTION_TYPE_TIMER
    };
    typedef struct InputData_struct {
        float m_X0;
        float m_Y0;
        float m_X1;
        float m_Y1;
        int m_ButtonCount;
        ACTION_TYPE m_ButtonType;
    } InputData;

    virtual bool Init(int32_t width, int32_t height) = 0;
    virtual bool Draw() = 0;
    virtual int32_t KeyHandler(InputData *event) = 0;
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
