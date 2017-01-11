#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <cstdint>
#include <android/input.h>

class IPlugin {
public:
    virtual bool Init(int32_t width, int32_t height) = 0;
    virtual bool Draw() = 0;
    virtual int32_t KeyHandler(AInputEvent *event) = 0;

public:
    IPlugin *next = NULL;
};

#endif // _PLUGIN_H_
