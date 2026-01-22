#include "ShooterEnemy.h"
#include "EntityManager.h"
#include "Bullet.h"


void ShooterEnemy::StayInBounds()
{
	// Clamp X
	if (position.x < 20)
		position.x = 20;
	if (position.x > SCREEN_WIDTH - 20)
		position.x = SCREEN_WIDTH - 20;

	// Clamp Y 
	if (position.y < 0)
		position.y = 0;
	if (position.y > SCREEN_HEIGHT - 200)
		position.y = SCREEN_HEIGHT - 200;
}

void ShooterEnemy::MoveShooterEnemy(float dt, float angleRad)
{
	Vector2 dir = Vector2Normalize(Vector2Subtract(m_targetPos, position));
	float dist = Vector2Distance(position, m_targetPos);
	float step = m_speed * dt;
	if (dist > step)
	{


		position = Vector2Add(position, Vector2Scale(dir, step));
		rotation = angleRad * (180.0f / PI);

	}
	else
	{
		position = m_targetPos;
		m_phase = Phases::WAITING;
		m_waitTimer = 0.0f;
	}
}

void ShooterEnemy::ShootAtPlayer(EntityManager& manager)
{
	float angleRad = rotation * (PI / 180.0f);
	float bulletSpeed = 300.f;
	Vector2 bulletVel = { cosf(angleRad) * bulletSpeed, sinf(angleRad) * bulletSpeed };

	// Create bullet at the enemy position
	auto bullet = std::make_unique<Bullet>(position, bulletSpeed, Bullet::Type::NORMAL, "assets/player/playerBullet.png");
	bullet->SetVelocity(bulletVel);
	bullet->SetRotation(rotation);
	bullet->SetOwner(this);
	manager.AddEntity(std::move(bullet));
}

ShooterEnemy::ShooterEnemy(Vector2 pos, float speed, const char* texturePath)
	: Enemy(pos, speed, Type::SHOOTER, texturePath), m_phase(Phases::IDLE), m_minPosX(50.f), m_maxPosX(SCREEN_WIDTH - 50.f), m_minPosY(50.f), m_maxPosY(SCREEN_HEIGHT - 200)
{
}

void ShooterEnemy::Update(float dt)
{
}

void ShooterEnemy::Update(float dt, EntityManager& manager, Vector2 playerPos)
{
	// For the random generator on both X and Y axis
	std::mt19937 gen(m_rd());
	std::uniform_real_distribution<> distX(m_minPosX, m_maxPosX);
	std::uniform_real_distribution<> distY(m_minPosY, m_maxPosY);

	// To point to the player 
	Vector2 pointToPlayer = Vector2Subtract(playerPos, position);
	float angleRad = atan2f(pointToPlayer.y, pointToPlayer.x);

	// Generate the random position
	float posX = distX(gen);
	float posY = distY(gen);

	// What phase in the enemy in
	switch (m_phase)
	{
	case ShooterEnemy::Phases::IDLE:
		// Pick a new random target position
		m_targetPos = { (float)posX, (float)posY };
		m_phase = Phases::MOVE;
		break;
	case ShooterEnemy::Phases::MOVE:
		MoveShooterEnemy(dt, angleRad);
		break;
	case ShooterEnemy::Phases::WAITING:
		m_waitTimer += dt;
		rotation = angleRad * (180.0f / PI);
		if (m_waitTimer >= m_waitDuration)
		{
			m_phase = Phases::IDLE; // Go to next position
		}
		break;
	default:
		break;
	}

	m_shootTimer += dt;
	if (m_shootTimer >= m_shootInterval) {
		ShootAtPlayer(manager);
		m_shootTimer = 0.0f;
	}
}
