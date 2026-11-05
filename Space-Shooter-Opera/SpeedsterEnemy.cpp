#include "SpeedsterEnemy.h"
#include "EntityManager.h"
#include "Bullet.h"

void SpeedsterEnemy::StraightPhaseUpdate(float dt)
{
	//TraceLog(LOG_INFO, "Doing Phase: STRAIGHT");

	// Make sure the rotation of the enemy is straight
	rotation = 90.f;

	m_target = { position.x, SCREEN_HEIGHT + 50 };

	Vector2 dir = Vector2Normalize(Vector2Subtract(m_target, position));
	float dist = Vector2Distance(position, m_target);
	float step = m_speed * dt;

	if (dist > step)
	{


		position = Vector2Add(position, Vector2Scale(dir, step));

	}
	else
	{
		position = m_target;
		m_phase = Phase::WAIT;
		m_waitTimer = 0.0f;
	}
	
}

void SpeedsterEnemy::uShapePhaseUpdate(float dt, Vector2 playerPos)
{
	//TraceLog(LOG_INFO, "Doing Phase: USHAPE");

	// To point to the player 
	Vector2 pointToPlayer = Vector2Subtract(playerPos, position);
	float angleRad = atan2f(pointToPlayer.y, pointToPlayer.x);
	float step = m_speed * dt;
	switch (m_uShapeStep)
	{
	case SpeedsterEnemy::UShapeStep::DOWN:
		m_target = { position.x, SCREEN_HEIGHT / 2 };
		break;
	case SpeedsterEnemy::UShapeStep::RIGHT:
		m_target = leftSpawnPoint 
			? Vector2 { SCREEN_WIDTH / 2 + 150, SCREEN_HEIGHT / 2 } 
			: Vector2 { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 };
		break;
	case SpeedsterEnemy::UShapeStep::UP:
		m_target = leftSpawnPoint 
			? Vector2{ SCREEN_WIDTH / 2 + 150, -50 } 
			: Vector2 { SCREEN_WIDTH / 2 - 150, -50 };
		break;
	default:
		break;
	}

	Vector2 dir = Vector2Normalize(Vector2Subtract(m_target, position));
	float dist = Vector2Distance(position, m_target);

	if (dist > step) {
		position = Vector2Add(position, Vector2Scale(dir, step));
		rotation = angleRad * (180.0f / PI);
	}
	else {
		position = m_target;
		
		// Advance the next step
		if (m_uShapeStep == UShapeStep::DOWN)
			m_uShapeStep = UShapeStep::RIGHT;
		else if (m_uShapeStep == UShapeStep::RIGHT)
			m_uShapeStep = UShapeStep::UP;
		else if (m_uShapeStep == UShapeStep::UP)
			m_phase = Phase::WAIT;
	}
}

void SpeedsterEnemy::ShootAtPlayer(EntityManager& manager)
{
	float angleRad = rotation * (PI / 180.0f);
	float bulletSpeed = 300.f;
	Vector2 bulletVel = { cosf(angleRad) * bulletSpeed, sinf(angleRad) * bulletSpeed };

	// Create bullet at the enemy position
	auto bullet = std::make_unique<Bullet>(position, bulletSpeed, Bullet::Type::NORMAL, eBullettexPath);
	bullet->SetVelocity(bulletVel);
	bullet->SetRotation(rotation);
	bullet->SetOwner(this);
	bullet->SetFaction(Bullet::Faction::ENEMY);
	manager.AddEntity(std::move(bullet));
}

SpeedsterEnemy::Phase SpeedsterEnemy::selectNextPhase()
{
	const int phaseIndex = std::uniform_int_distribution<int>(0, 1)(m_rng);
	
	return phaseIndex == 0 ? Phase::STRAIGHT : Phase::USHAPE;;
}

SpeedsterEnemy::SpeedsterEnemy(Vector2 pos, float speed, const char* texturePath)
	: Enemy(pos, speed, Type::SPEED, texturePath), m_phase(Phase::IDLE), m_uShapeStep(UShapeStep::DOWN)
{
	
}

void SpeedsterEnemy::Update(float dt)
{
}

void SpeedsterEnemy::Update(float dt, EntityManager& manager, Vector2 playerPos)
{
	// The ship will either move in a U bend or straight across the screen
	srand(time(0));

	int health = GetHealth();

	if (health <= 0) {
		SetActive(false);
	}

	if (IsActive())
	{
		switch (m_phase)
		{
		case SpeedsterEnemy::Phase::IDLE:
			leftSpawnPoint = std::uniform_int_distribution<int>(0, 1)(m_rng) == 0;

			if (leftSpawnPoint)
				position = { SCREEN_WIDTH / 2 - 150, -50 };
			else
				position = { SCREEN_WIDTH / 2 + 150, -50 };
			
			m_uShapeStep = UShapeStep::DOWN;
			m_phase = selectNextPhase();
			break;
		case SpeedsterEnemy::Phase::STRAIGHT:
			StraightPhaseUpdate(dt);
			break;
		case SpeedsterEnemy::Phase::USHAPE:
			uShapePhaseUpdate(dt, playerPos);

			// Shoot only in this phase
			m_shootTimer += dt;
			if (m_shootTimer >= m_shootInterval) {
				ShootAtPlayer(manager);
				m_shootTimer = 0.0f;
			}
			break;
		case SpeedsterEnemy::Phase::WAIT:
			m_waitTimer += dt;
			if (m_waitTimer >= m_waitDuration)
			{
				m_phase = Phase::IDLE; // Go to next position
			}
			break;
		default:
			break;
		}
	}

	
}
