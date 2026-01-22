#pragma once
#include "Entity.h"

#include <array>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

#define MAX_BULLETS 1000


class EntityManager;

class Player :
    public Entity
{
private:
    float m_speed;
    int m_health;
    float m_shotTimer = 0;
    float m_coolDown = .3f;
    
    // For invincibility frames
    bool m_invincible = false;
    float m_invincibleTimer = 0.0f;
    float m_invincibleDuration = 1.0f; // Seconds of invincibility
    float m_flashTimer = 0.0f;
    float m_flashInterval = 0.1f; // seconds between flashes
    bool m_flashVisible = true;

    void StayInBounds();
    void Shoot(float dt, EntityManager& manager);
    void TriggerInvincibility(float dt);

    void DrawHealthBar();

public:
    Player(Vector2 pos, float speed, const char* texturePath = nullptr);
    ~Player();

    void Update(float dt) override;
    void Update(float dt, EntityManager& manager);
    void Draw() override;

    void TakeDamage(int damage);
    void TakeDamageOnImpact(int damage);

    Vector2& GetPosition();
    int GetHealth();
    

};

