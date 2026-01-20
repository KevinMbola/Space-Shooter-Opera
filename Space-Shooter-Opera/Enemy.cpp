#include "Enemy.h"
#include "EntityManager.h"
#include "Bullet.h"

void Enemy::UpdateShooter(float dt)
{
	
}

void Enemy::UpdateSpeedster(float dt)
{
	position.y += m_speed * dt;

	if (position.y > SCREEN_HEIGHT + 20) {
		SetActive(false);
	}
}

void Enemy::UpdateBruiser(float dt)
{
}

void Enemy::UpdateBoss(float dt)
{
}

Enemy::Enemy(Vector2 pos, float speed, Type t, const char* texturePath)
	: Entity(pos, 1.0f, 90.f), m_speed(speed), m_target(pos), m_health(0), m_type(t)
{
	if (texturePath && texturePath[0] != '\0') {
		texture = LoadTexture(texturePath);
	}

	SetCollisionBox(0.15f, 0.15f, 0.7f, 0.7f);

	switch (m_type)
	{
	case Enemy::Type::SHOOTER:
		m_health = 3;
		break;
	case Enemy::Type::SPEEDER:
		m_health = 1;
		break;
	case Enemy::Type::BRUISER:
		m_health = 5;
		break;
	case Enemy::Type::BOSS:
		m_health = 10;
		break;
	default:
		break;
	}


}

Enemy::~Enemy()
{
	if (texture.id != 0)
		UnloadTexture(texture);
}

void Enemy::Update(float dt)
{
	//Vector2 dir = Vector2Normalize(Vector2Subtract(m_target, position));
	//velocity = Vector2Scale(dir, m_speed);
	//position = Vector2Add(position, Vector2Scale(velocity, dt));

	switch (m_type)
	{
	case Enemy::Type::SHOOTER:
		UpdateShooter(dt);
		break;
	case Enemy::Type::SPEEDER:
		UpdateSpeedster(dt);
		break;
	case Enemy::Type::BRUISER:
		UpdateBruiser(dt);
		break;
	case Enemy::Type::BOSS:
		UpdateBoss(dt);
		break;
	default:
		break;
	}
}


void Enemy::Draw()
{
	if (texture.id != 0) {
		DrawTextureCentered();
	}
	else {
		DrawCircleV(position, 10, RED);
	}

#ifdef DEBUG
	Rectangle bounds = GetBounds();
	DrawRectangleLinesEx(bounds, 2, RED);
#endif // DEBUG

}

void Enemy::SetTarget(Vector2 t)
{
}

void Enemy::TakeDamage(int damage)
{
	damage -= m_health;
}

int Enemy::GetHealth() const
{
	return m_health;
}

Enemy::Type Enemy::GetType() const
{
	return m_type;
}
