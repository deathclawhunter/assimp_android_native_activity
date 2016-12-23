#include <string>
#include "ogldev_camera.h"

using namespace std;

// cap for meshes in a scene
#define MAX_NUM_SKINNED_MESHES 5
// TODO: #define MAX_NUM_STATIC_MESHES 20

// TODO: move to engine app plugin context
#define KEY_COUNT 10
int KeyCounter = 0;
int KeyDirectionFlag = 0;
#define MAX_STRIKE_TIME 1000 // sec
long long KeyTimeStamp = 0;

/**
 * 0: left, 1: right, otherwise n/a
 */
int scroll(float touchX, float lastTouchX) {

    // invalidate keys if time passed MAX_STRIKE_TIME
    if (KeyTimeStamp == 0) {
        KeyTimeStamp = GetCurrentTimeMillis();
    } else {
        long long current = GetCurrentTimeMillis();
        if (current - KeyTimeStamp >= MAX_STRIKE_TIME) {
            // reset key buffer
            KeyCounter = 0;
            KeyDirectionFlag = 0;
            KeyTimeStamp = current;
            return 2;
        }
    }

    if (touchX < lastTouchX) {
        if (KeyDirectionFlag == 0) {
            KeyCounter++;
        } else {
            KeyCounter = 0;
            KeyDirectionFlag = 0;
        }
    } else if (touchX > lastTouchX) {

        if (KeyDirectionFlag == 0) {
            KeyCounter = 0;
            KeyDirectionFlag = 1;
        } else {
            KeyCounter++;
        }

        // LOGI("touchX > engine->touchX: KeyCounter = %d, KeyDirectionFlag = %d\n", KeyCounter, KeyDirectionFlag);
    }

    if (KeyCounter >= KEY_COUNT) {
        KeyCounter = 0;

        // LOGI("KeyCounter >= KEY_COUNT: KeyDirectionFlag = %d\n", KeyDirectionFlag);
        if (KeyDirectionFlag == 0) {
            KeyTimeStamp = GetCurrentTimeMillis();
            return 0;
        } else {
            KeyTimeStamp = GetCurrentTimeMillis();
            return 1;
        }
    }

    return -1;
}



