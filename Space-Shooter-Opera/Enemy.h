#pragma once
#include "Entity.h"

#include <raymath.h>


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

class EntityManager;

class Enemy :
    public Entity
{
public:
    enum class Type { NONE, SHOOTER, SPEED, BRUISER, BOSS };

protected:
    float m_speed;
    Vector2 m_target;
    int m_maxHealth;
    int m_health;
    Color m_healthColour;
    Type m_type;
    int m_scoreValue;

private:
    void DrawHealthBar();

public:
    Enemy(Vector2 pos, float speed, Type t, const char* texturePath = nullptr);
    ~Enemy();

    void Update(float dt) override;
    void Draw() override;

    void SetTarget(Vector2 t);
    void TakeDamage(int damage);
    int GetHealth() const;
    int GetMaxHealth() const;
    int GetScoreValue() const;
    Type GetType() const;
};

