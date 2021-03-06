#define LOG_TAG "AppCamera"

#include "AppLog.h"

#include "AppCamera.h"


static AppCamera *s_AppCamera = NULL;

AppCamera::AppCamera(int width, int height) {
    Vector3f Pos(0.0f, 3.0f, -1.0f);
    // Vector3f Pos(0.0f, 1.0f, -20.0f);
    Vector3f Target(0.0f, 0.0f, 1.0f);
    Vector3f Up(0.0, 1.0f, 0.0f);

    m_Camera = new Camera(width, height, Pos, Target, Up);
    m_Camera->SetStep(GAME_STEP_SCALE);

    m_PersProjInfo.FOV = 60.0f;
    m_PersProjInfo.Height = height;
    m_PersProjInfo.Width = width;
    m_PersProjInfo.zNear = 1.0f;
    m_PersProjInfo.zFar = 100.0f;
}

AppCamera::~AppCamera() {
    SAFE_DELETE(m_Camera);
}

AppCamera *AppCamera::GetInstance(int width, int height) {
    if (s_AppCamera == NULL) {
        s_AppCamera = new AppCamera(width, height);
    }

    return s_AppCamera;
}

AppCamera *AppCamera::GetInstance() {
    return s_AppCamera;
}

