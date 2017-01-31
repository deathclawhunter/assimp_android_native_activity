#define LOG_TAG "InteractivePlugin"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_app.h"
#include "AppTechnique.h"
#include "ogldev_glut_backend.h"
#include "AppMesh.h"

using namespace std;

#include "InteractivePlugin.h"

void InteractivePlugin::CalculateCenterOfRightHalf(int width, int height) {
    m_RCenterX = width * 3.0f / 4.0f;
    m_RCenterY = height / 2.0f;
}

OGLDEV_KEY InteractivePlugin::ConvertKey(float x, float y) {

    float diffX, diffY, ratioX, ratioY;

    diffX = m_RCenterX - x;
    diffY = m_RCenterY - y;
    ratioX = diffX / m_Width;
    ratioY = diffY / m_Height;
    LOGI("m_RCenterX = %f, m_RCenterY = %f\n", m_RCenterX, m_RCenterY);

    if (abs(diffX) > MINIMAL_MOVE_DIFF && abs(ratioX) > abs(ratioY)) {
        if (diffX > 0) {
            LOGI("LEFT");
            return OGLDEV_KEY_LEFT;
        } else {
            LOGI("RIGHT");
            return OGLDEV_KEY_RIGHT;
        }
    } else if (abs(diffY) > MINIMAL_MOVE_DIFF && abs(ratioY) > abs(ratioX)) {

        if (ENABLE_UP_N_DOWN) {
            if (diffY > 0) {
                LOGI("UP");
                return OGLDEV_KEY_UP;
            } else {
                LOGI("DOWN");
                return OGLDEV_KEY_DOWN;
            }
        }
    }

    return OGLDEV_KEY_UNDEFINED;
}

void InteractivePlugin::ResetMouse() {
    AppCamera::GetInstance()->ResetMouse();
}

float InteractivePlugin::DistToCenter(float x, float y) {
    // simple algorithm to calculate right half or left half screen was pressed
    return abs(x - m_RCenterX) + abs(y - m_RCenterY);
}

bool InteractivePlugin::Init(int32_t width, int32_t height) {
    m_Width = width;
    m_Height = height;
    CalculateCenterOfRightHalf(width, height);
    return true;
}

int32_t InteractivePlugin::KeyHandler(AInputEvent *event) {
    int32_t action = AMotionEvent_getAction(event);
    size_t count = AMotionEvent_getPointerCount(event);
    if (count == 1) { // single finger touch

        if (action == AMOTION_EVENT_ACTION_MOVE) {

            float touchX = AMotionEvent_getX(event, 0);
            float touchY = AMotionEvent_getY(event, 0);

            PassiveMouseCB(touchX, touchY);
        } else if (action == AMOTION_EVENT_ACTION_UP) {

            LOGI("Reset mouse");
            ResetMouse();
        }
    } else if (count == 2) {

        if (action == AMOTION_EVENT_ACTION_MOVE) {

            float touch2X = AMotionEvent_getX(event, 1);
            float touch2Y = AMotionEvent_getY(event, 1);

            float touch1X = AMotionEvent_getX(event, 0);
            float touch1Y = AMotionEvent_getY(event, 0);

            if (DistToCenter(touch1X, touch1Y) >
                DistToCenter(touch2X, touch2Y)) {

                PassiveKeyCB(touch2X, touch2Y);
            } else {
                PassiveKeyCB(touch1X, touch1Y);
            }
        }
    }

    return 1;
}


