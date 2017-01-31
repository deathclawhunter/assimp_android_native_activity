#ifndef _INTERACTIVE_PLUGIN_H_
#define _INTERACTIVE_PLUGIN_H_

#include <android_native_app_glue.h>
#include <GL/freeglut.h>

#include "ogldev_app.h"
#include "plugin.h"
#include "AppCamera.h"
#include "ogldev_glut_backend.h"

class InteractivePlugin : public ICallbacks, public OgldevApp, public IPlugin {
public:

    // Plugin API
    bool Init(int32_t width, int32_t height);

    int32_t KeyHandler(InputData *event);

    virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State) {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                // GLUTBackendLeaveMainLoop();
                glutLeaveMainLoop();
                break;
            default:
                AppCamera::GetInstance()->OnKeyboard(OgldevKey);
        }
    }

    void PassiveMouseCB(int x, int y) {
        AppCamera::GetInstance()->OnMouse(x, y);
    }

    OGLDEV_KEY ConvertKey(float x, float y);

    void PassiveKeyCB(float x, float y) {
        AppCamera::GetInstance()->OnKeyboard(ConvertKey(x, y));
    }

    void ResetMouse();

    float DistToCenter(float x, float y);

private:
    float m_RCenterX, m_RCenterY; // Center point of right half of the screen
    int m_Width, m_Height;

private:
    const float MINIMAL_MOVE_DIFF = 0.1f;
    const bool ENABLE_UP_N_DOWN = true; // In mobile game, we disable up and down
    void CalculateCenterOfRightHalf(int width, int height);
};

#endif /* _INTERACTIVE_PLUGIN_H_ */
