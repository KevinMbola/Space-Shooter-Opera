#pragma once
#include "Enemy.h"

#include <random>

class EntityManager;

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
    float m_shootInterval = .6f; // seconds between shots

    std::mt19937 m_rng{ std::random_device{}() };

    void StraightPhaseUpdate(float dt);
    void uShapePhaseUpdate(float dt, Vector2 playerPos);

    void ShootAtPlayer(EntityManager& manager); // Will shoot at player 
    Phase selectNextPhase();

public:
    SpeedsterEnemy(Vector2 pos, float speed, const char* texturePath = nullptr);

    void Update(float dt) override;
    void Update(float dt, EntityManager& manager, Vector2 playerPos);
};

