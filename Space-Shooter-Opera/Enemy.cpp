#include "Enemy.h"
#include "EntityManager.h"
#include "Bullet.h"

void Enemy::DrawHealthBar()
{
	float healthBarWidth = texture.width;
	int maxHealth = GetMaxHealth();
	int health = GetHealth();

	
	switch (m_type)
	{
	case Enemy::Type::SHOOTER:
		m_healthColour = (health <= 3) ? RED
			: (health <= 5) ? YELLOW
			: GREEN;
		break;
	case Enemy::Type::SPEED:
		m_healthColour = GREEN;
		break;
	case Enemy::Type::BOSS:
		m_healthColour = (health <= 5) ? RED
			: (health <= 10) ? YELLOW
			: GREEN;
		break;
	default:
		break;
	}

	// Filled portion is proportuinal to health
	float fillWidth = healthBarWidth * (float(health) / maxHealth);

	Rectangle healthRect = {
		position.x - healthBarWidth / 2,
		position.y - 35,
		fillWidth,
		10
	};

	Rectangle healthBorderRect = {
		healthRect.x,
		healthRect.y,
		healthBarWidth,
		12.5f
	};

	DrawRectangleRec(healthRect, m_healthColour);
	DrawRectangleLinesEx(healthBorderRect, 2, BLACK);
}

Enemy::Enemy(Vector2 pos, float speed, Type t, const char* texturePath)
	: Entity(pos, 1.0f, 90.f), m_speed(speed), m_target(pos), m_maxHealth(0), m_health(0), m_type(t)
{
	if (texturePath && texturePath[0] != '\0') {
		texture = LoadTexture(texturePath);
	}

	SetCollisionBox(0.15f, 0.15f, 0.7f, 0.7f);

	switch (m_type)
	{
	case Enemy::Type::SHOOTER:
		m_maxHealth = 7;
		m_scoreValue = 5;
		break;
	case Enemy::Type::SPEED:
		m_maxHealth = 1;
		m_scoreValue = 10;
		break;
	case Enemy::Type::BRUISER:
		m_maxHealth = 5;
		break;
	case Enemy::Type::BOSS:
		m_maxHealth = 20;
		break;
	default:
		break;
	}

	m_health = m_maxHealth;
}

Enemy::~Enemy()
{
	if (texture.id != 0)
		UnloadTexture(texture);
}

void Enemy::Update(float dt)
{

}


void Enemy::Draw()
{
	if (texture.id != 0) {
		DrawTextureCentered();
	}
	else {
		DrawCircleV(position, 10, RED);
	}

	DrawHealthBar();

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
	m_health -= damage;
}

int Enemy::GetHealth() const
{
	return m_health;
}

int Enemy::GetMaxHealth() const
{
	return m_maxHealth;
}

int Enemy::GetScoreValue() const
{
	return m_scoreValue;
}

Enemy::Type Enemy::GetType() const
{
	return m_type;
}
