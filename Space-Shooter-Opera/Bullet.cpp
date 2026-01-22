#include "Bullet.h"

Bullet::Bullet(Vector2 pos, float speed, Type type, const char* texturePath) : Entity(pos, 1.0f, 0.0f), m_speed(speed), m_type(type)
{

	if (texturePath && texturePath[0] != '\0') {
		texture = LoadTexture(texturePath);
	}

}

Bullet::~Bullet()
{

}

void Bullet::SetOwner(Entity* owner)
{
	m_owner = owner;
}

Entity* Bullet::GetOwner() const
{
	return m_owner;
}

void Bullet::SetFaction(Faction f)
{
	m_faction = f;
}

Bullet::Faction Bullet::GetFaction() const
{
	return m_faction;
}

void Bullet::Update(float dt)
{
	if (active)
	{
		switch (m_type)
		{
		case Bullet::Type::NORMAL:
			position.x += velocity.x * dt;
			position.y += velocity.y * dt;
			break;
		case Bullet::Type::GIANT:
			break;
		case Bullet::Type::SPREAD:
			break;
		case Bullet::Type::FAST:
			break;
		default:
			break;
		}

		// If bullet is not on screen make it inactive
		if (position.x < 0 || position.x > SCREEN_WIDTH
			|| position.y < 0 || position.y > SCREEN_HEIGHT)
			SetActive(false);

	}
}

void Bullet::Draw()
{
	if (active)
	{
		if (texture.id != 0) {
			DrawTextureCentered();
		}
		else {
			DrawCircleV(position, 3, ORANGE);
		}
	}
}
