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

void Bullet::Update(float dt)
{
	if (active)
	{
		switch (m_type)
		{
		case Bullet::Type::NORMAL:
			position.y -= m_speed * dt;
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
