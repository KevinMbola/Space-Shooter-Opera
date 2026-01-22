#include "SpeedsterEnemy.h"

void SpeedsterEnemy::StraightPhaseUpdate(float dt, float angleRad)
{
	TraceLog(LOG_INFO, "Doing Phase: STRAIGHT");


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

void SpeedsterEnemy::uShapePhaseUpdate(float dt)
{
	TraceLog(LOG_INFO, "Doing Phase: USHAPE");
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

SpeedsterEnemy::Phase SpeedsterEnemy::selectNextPhase()
{

	int phaseIndex = std::uniform_int_distribution<int>(0, 1)(m_rng);
	m_phase = (phaseIndex == 0) ? Phase::STRAIGHT : Phase::USHAPE;
	return m_phase;
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
	Vector2 pointToPlayer = Vector2Subtract(playerPos, position);
	float angleRad = atan2f(pointToPlayer.y, pointToPlayer.x);
	srand(time(0));


	switch (m_phase)
	{
	case SpeedsterEnemy::Phase::IDLE:
		leftSpawnPoint = std::uniform_int_distribution<int>(0, 1)(m_rng) == 0;
		leftSpawnPoint ? position = { SCREEN_WIDTH / 2 - 150, -50 } : position = { SCREEN_WIDTH / 2 + 150, -50 }; // Keep the enmy off screen
		m_uShapeStep = UShapeStep::DOWN;
		m_phase = selectNextPhase();
		break;
	case SpeedsterEnemy::Phase::STRAIGHT:
		StraightPhaseUpdate(dt, angleRad);
		break;
	case SpeedsterEnemy::Phase::USHAPE:
		uShapePhaseUpdate(dt);
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
