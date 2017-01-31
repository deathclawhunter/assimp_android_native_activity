#include "ogldev_math_3d.h"

#ifndef _PLAYER_H_
#define _PLAYER_H_

class Player {
public:
    static Player *GetInstance();
    void SetPosition(const Vector3f &Position);
    Vector3f &GetPosition();
    void SetRotation(const Vector3f &Rotation);
    Vector3f &GetRotation();
    ~Player();

private:
    Player();
    Vector3f m_Position;
    Vector3f m_Rotation;
};

#endif /* _PLAYER_H_ */