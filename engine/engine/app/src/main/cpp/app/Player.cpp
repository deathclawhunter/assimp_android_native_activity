#include "Player.h"


static Player *sPlayer = NULL;

Player *Player::GetInstance() {
    if (sPlayer == NULL) {
        sPlayer = new Player();
    }

    return sPlayer;
}


void Player::SetPosition(const Vector3f &Position) {
    m_Position = Position;
}


Vector3f &Player::GetPosition() {
    return m_Position;
}

void Player::SetRotation(const Vector3f &Rotation) {
    m_Rotation = Rotation;
}

Vector3f &Player::GetRotation() {
    return m_Rotation;
}

Player::~Player() {
}

Player::Player() {
}

