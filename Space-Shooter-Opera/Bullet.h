#pragma once
#include "Entity.h"



class Bullet :
    public Entity
{

public:
    enum class Type { NORMAL, GIANT, SPREAD, FAST };
private:
    float m_speed;
    Type m_type;

public:
    

    Bullet(Vector2 pos, float speed, Type type = Type::NORMAL, const char* texturePath = nullptr);
    ~Bullet();

    void Update(float dt) override;
    void Draw() override;

};

