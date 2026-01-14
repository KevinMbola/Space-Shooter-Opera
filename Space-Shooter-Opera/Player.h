#pragma once
#include "Entity.h"
#include <vector>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

class Bullet;

class Player :
    public Entity
{
private:
    float m_speed;
    int m_health;

    std::vector<Bullet> m_bullets;

    void StayInBounds();

public:
    Player(Vector2 pos, float speed, const char* texturePath = nullptr);
    ~Player();

    void Update(float dt) override;
    void Draw() override;

};

