#pragma once
#include "Entity.h"

#include <raymath.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

class Enemy :
    public Entity
{
public:
    enum class Type { NONE, SHOOTER, SPEEDER, BRUISER, BOSS };

protected:
    float m_speed;
    Vector2 m_target;
    int m_health;
    Type m_type;

private:
    void UpdateShooter(float dt);
    void UpdateSpeedster(float dt);
    void UpdateBruiser(float dt);
    void UpdateBoss(float dt);

public:
    Enemy(Vector2 pos, float speed, const char* texturePath = nullptr);
    ~Enemy();

    void Update(float dt) override;
    void Draw() override;

    void SetTarget(Vector2 t);
    void TakeDamage(int damage);
    int GetHealth() const;
};

