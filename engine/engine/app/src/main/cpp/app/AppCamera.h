#ifndef _APP_CAMERA_H_
#define _APP_CAMERA_H_

#include "ogldev_camera.h"

class AppCamera {
public:
    static AppCamera *GetInstance(int width, int height);
    static AppCamera *GetInstance();

    ~AppCamera();

    void ResetMouse() {
        return m_Camera->ResetMouse();
    }

    bool OnKeyboard(OGLDEV_KEY Key) {
        return m_Camera->OnKeyboard(Key);
    }

    void OnMouse(int x, int y) {
        m_Camera->OnMouse(x, y);
    }

    void OnRender() {
        m_Camera->OnRender();
    }

    const Vector3f &GetPos() const {
        return m_Camera->GetPos();
    }

    const Vector3f &GetTarget() const {
        return m_Camera->GetTarget();
    }

    const Vector3f &GetUp() const {
        return m_Camera->GetUp();
    }

    void SetStep(float step) {
        return m_Camera->SetStep(step);
    }

    PersProjInfo &GetPersProjInfo() {
        return m_PersProjInfo;
    }

private:
    AppCamera();

    AppCamera(int width, int height);

    const float GAME_STEP_SCALE = 5.0f;

    Camera *m_Camera;

    PersProjInfo m_PersProjInfo;
};

#endif /* _APP_CAMERA_H_ */
