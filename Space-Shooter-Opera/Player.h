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

    void StayInBounds();
    void Shoot(float dt, EntityManager& manager);

public:
    Player(Vector2 pos, float speed, const char* texturePath = nullptr);
    ~Player();

    void Update(float dt) override;
    void Update(float dt, EntityManager& manager);
    void Draw() override;

    void TakeDamage(int damage);

    Vector2& GetPosition();
    int GetHealth();
    

};

