#pragma once
#include "Enemy.h"

#include <random>


class SpeedsterEnemy :
    public Enemy
{
public:
    enum class Phase {IDLE, STRAIGHT, USHAPE, WAIT};
    enum class UShapeStep { DOWN, RIGHT, UP };

private:
    Phase m_phase;
    UShapeStep m_uShapeStep;
    bool leftSpawnPoint = true;
    float m_waitTimer = 0.0f;
    float m_waitDuration = 1.0f; // Wait a second at each position
    float m_shootTimer = 0.0f;
    float m_shootInterval = 1.0f; // seconds between shots


    float m_minPosX, m_minPosY;
    float m_maxPosX, m_maxPosY;

    std::mt19937 m_rng{ std::random_device{}() };

    void StraightPhaseUpdate(float dt, float angleRad);
    void uShapePhaseUpdate(float dt);
    Phase selectNextPhase();

public:
    SpeedsterEnemy(Vector2 pos, float speed, const char* texturePath = nullptr);

    void Update(float dt) override;
    void Update(float dt, EntityManager& manager, Vector2 playerPos);
};

