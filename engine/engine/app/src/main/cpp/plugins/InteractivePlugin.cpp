#include <math.h>
#include <string>
#include <android_native_app_glue.h>
#include <app/Config.h>

#include "ogldev_app.h"
#include "AppTechnique.h"
#include "ogldev_glut_backend.h"
#include "AppMesh.h"

#define LOG_TAG "InteractivePlugin"
#include "AppLog.h"

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

int32_t InteractivePlugin::KeyHandler(InputData *event) {

    if (event->m_ButtonType ==  IPlugin::ACTION_TYPE_TIMER) {

        LOGI("Sensor movement");
        PassiveMoveKeyCB(); // movement

        return 1;
    }

    bool SensorEnabled = Config::GetInstance(NULL)->GetbCfg(Config::CFG_USE_SENSOR, Config::DEFAULT_CFG_USE_SENSOR);
    if (event->m_ButtonCount == 1) { // single finger touch

        // Sensor Controller
        if (SensorEnabled) {
            if (event->m_ButtonType == IPlugin::ACTION_TYPE_ROTATE) {
                LOGI("Rotation by sensor");
                PassiveMouseCB(event->m_X0, event->m_Y0); // rotation
            } else if (event->m_ButtonType == IPlugin::ACTION_TYPE_UP) {
                LOGI("reset m_Forward");
                if (m_Forward) {
                    m_Forward = false;
                } else {
                    m_Forward = true;
                }
            } else if (event->m_ButtonType == IPlugin::ACTION_TYPE_RESET) {
                LOGI("Reset mouse");
                ResetMouse();
            }
        // Screen controller
        } else {
            if (event->m_ButtonType == IPlugin::ACTION_TYPE_MOVE) {

                float touchX = event->m_X0;
                float touchY = event->m_Y0;

                LOGI("x = %f, y = %f", touchX, touchY);

                touchX *= 2.0f;
                touchY *= 2.0f;
                LOGI("Rotation");
                PassiveMouseCB(touchX, m_Height / 2.0f);
            } else if (event->m_ButtonType == IPlugin::ACTION_TYPE_UP) {

                LOGI("Reset mouse");
                ResetMouse();
            }
        }
    // Screen Controller
    } else if (!SensorEnabled && event->m_ButtonCount == 2) {

        LOGI(">>>>>>>>>>>>>>> TWO touch : event->m_ButtonType = %d", event->m_ButtonType);
        if (event->m_ButtonType == IPlugin::ACTION_TYPE_UP) {
            if (m_Forward) {
                m_Forward = false;
                LOGI("reset m_Forward to false");
            } else {
                m_Forward = true;
                LOGI("reset m_Forward to true");
            }
        }
    }

    return 1;
}


