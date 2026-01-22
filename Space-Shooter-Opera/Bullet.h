#pragma once
#include "Entity.h"



class Bullet :
    public Entity
{

public:
    enum class Type { NORMAL, GIANT, SPREAD, FAST };
    enum class Faction { PLAYER, ENEMY };
private:
    float m_speed;
    Type m_type;
    Entity* m_owner = nullptr;
    Faction m_faction = Faction::PLAYER;
public:
    
    Bullet() : Entity({ 0,0 }), m_speed(0.f), m_type(Type::NORMAL) {}
    Bullet(Vector2 pos, float speed, Type type = Type::NORMAL, const char* texturePath = nullptr);
    ~Bullet();

    void SetOwner(Entity* owner);
    Entity* GetOwner() const;
    void SetFaction(Faction f);
    Faction GetFaction() const;

    void Update(float dt) override;
    void Draw() override;

};

