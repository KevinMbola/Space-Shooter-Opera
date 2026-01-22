#pragma once
#include "Enemy.h"
#include <random>

class EntityManager;

class ShooterEnemy :
    public Enemy
{
public:
    enum class Phases {IDLE, MOVE, WAITING};

private:
    Phases m_phase;
    Vector2 m_targetPos = { 0, 0 };
    float m_waitTimer = 0.0f;
    float m_waitDuration = 1.0f; // Wait a second at each position
    float m_shootTimer = 0.0f;
    float m_shootInterval = 1.0f; // seconds between shots


    float m_minPosX, m_minPosY;
    float m_maxPosX, m_maxPosY;

    std::random_device m_rd;

    void StayInBounds();
    void MoveShooterEnemy(float dt, float angleRad);
    void ShootAtPlayer(float dt, Vector2 playerPos, EntityManager& manager);

public:
    ShooterEnemy(Vector2 pos, float speed, const char* texturePath = nullptr);

    void Update(float dt) override;
    void Update(float dt, EntityManager& manager, Vector2 playerPos);
};

