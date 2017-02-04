/*

	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ogldev_camera.h"

const static float STEP_SCALE = 100.0f;
const static float EDGE_STEP = 0.5f;
const static int MARGIN = 10;

Camera::Camera(int WindowWidth, int WindowHeight) {
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Vector3f(0.0f, 0.0f, 0.0f);
    m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_target.Normalize();
    m_up = Vector3f(0.0f, 1.0f, 0.0f);
    m_StepScale = STEP_SCALE;

    Init();
}


Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f &Pos, const Vector3f &Target,
               const Vector3f &Up) {
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize();

    m_up = Up;
    m_up.Normalize();

    Init();
}


void Camera::Init() {
    Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize();

    if (HTarget.z >= 0.0f) {
        if (HTarget.x >= 0.0f) {
            m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
        }
        else {
            m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
        }
    }
    else {
        if (HTarget.x >= 0.0f) {
            m_AngleH = ToDegree(asin(-HTarget.z));
        }
        else {
            m_AngleH = 180.0f - ToDegree(asin(-HTarget.z));
        }
    }

    m_AngleV = -ToDegree(asin(m_target.y));

    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge = false;
    m_OnRightEdge = false;
    m_mousePos.x = m_windowWidth / 2;
    m_mousePos.y = m_windowHeight / 2;
    m_ResetFlag = true;

    // glutWarpPointer(m_mousePos.x, m_mousePos.y);
}


bool Camera::OnKeyboard(OGLDEV_KEY Key) {
    bool Ret = false;

    switch (Key) {

        case OGLDEV_KEY_UP: {
            // m_pos += (m_target * m_StepScale);

            // Not moving in Y direction
            m_pos.x += m_target.x * m_StepScale;
            m_pos.z += m_target.z * m_StepScale;

            Ret = true;
        }
            break;

        case OGLDEV_KEY_DOWN: {
            // m_pos -= (m_target * m_StepScale);

            // Not moving in Y direction
            m_pos.x -= m_target.x * m_StepScale;
            m_pos.z -= m_target.z * m_StepScale;

            Ret = true;
        }
            break;

        case OGLDEV_KEY_LEFT: {
            Vector3f Left = m_target.Cross(m_up);
            Left.Normalize();
            Left *= m_StepScale;
            m_pos += Left;
            Ret = true;
        }
            break;

        case OGLDEV_KEY_RIGHT: {
            Vector3f Right = m_up.Cross(m_target);
            Right.Normalize();
            Right *= m_StepScale;
            m_pos += Right;
            Ret = true;
        }
            break;

        case OGLDEV_KEY_PAGE_UP:
            m_pos.y += m_StepScale;
            break;

        case OGLDEV_KEY_PAGE_DOWN:
            m_pos.y -= m_StepScale;
            break;

        default:
            break;
    }

    if (Ret) {
        Update();
    }

    return Ret;
}


void Camera::OnMouse(int x, int y) {

    LOGI("OnMouse: x = %d, y = %d", x, y);

    const int DeltaX = m_ResetFlag ? 0 : x - m_mousePos.x;
    const int DeltaY = m_ResetFlag ? 0 : y - m_mousePos.y;
    // const int DeltaY = 0;

    m_ResetFlag = false;
    m_mousePos.x = x;
    m_mousePos.y = y;

    m_AngleH += (float) DeltaX / 20.0f;
    m_AngleV += (float) DeltaY / 20.0f;

    if (DeltaX == 0) {
        if (x <= MARGIN) {
            //    m_AngleH -= 1.0f;
            m_OnLeftEdge = true;
        }
        else if (x >= (m_windowWidth - MARGIN)) {
            //    m_AngleH += 1.0f;
            m_OnRightEdge = true;
        }
    }
    else {
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
    }

    if (DeltaY == 0) {
        if (y <= MARGIN) {
            m_OnUpperEdge = true;
        }
        else if (y >= (m_windowHeight - MARGIN)) {
            m_OnLowerEdge = true;
        }
    }
    else {
        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
    }

    Update();
}


void Camera::OnRender() {
    bool ShouldUpdate = false;

    if (m_OnLeftEdge) {
        m_AngleH -= EDGE_STEP;
        ShouldUpdate = true;
    }
    else if (m_OnRightEdge) {
        m_AngleH += EDGE_STEP;
        ShouldUpdate = true;
    }

    if (m_OnUpperEdge) {
        if (m_AngleV > -90.0f) {
            m_AngleV -= EDGE_STEP;
            ShouldUpdate = true;
        }
    }
    else if (m_OnLowerEdge) {
        if (m_AngleV < 90.0f) {
            m_AngleV += EDGE_STEP;
            ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
}

void Camera::Update() {
    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_AngleH, Vaxis);
    View.Normalize();

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f Haxis = Vaxis.Cross(View);
    Haxis.Normalize();
    View.Rotate(m_AngleV, Haxis);

    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(Haxis);
    m_up.Normalize();
}

void Camera::ResetMouse() {
    m_ResetFlag = true;
}


/* void Camera::AddToATB(TwBar* bar)
{
    TwAddButton(bar, "Camera", NULL, NULL, "");                
    TwAddVarRW(bar, "Position", TW_TYPE_OGLDEV_VECTOR3F, (void*)&m_pos, NULL);
    TwAddVarRO(bar, "Direction", TW_TYPE_DIR3F, &m_target, " axisz=-z ");
} */

void Camera::SetStep(float step) {
    m_StepScale = step;
}


