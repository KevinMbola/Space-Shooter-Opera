#include "Player.h"
#include "EntityManager.h"
#include "Bullet.h"
#include "GameConfig.h"

#include <memory>

void Player::StayInBounds()
{
	const Rectangle bounds = GetBounds();

	const float screenWidth = static_cast<float>(GameConfig::ScreenWidth);
	const float screenHeight = static_cast<float>(GameConfig::ScreenHeight);

	if (bounds.width >= screenWidth)
		position.x += screenWidth / 2.f - (bounds.x + bounds.width / 2.f);
	else if (bounds.x < 0.0f)
		position.x -= bounds.x;
	else if (bounds.x + bounds.width > screenWidth)
		position.x -= bounds.x + bounds.width - screenWidth;
	
	if (bounds.height >= screenHeight)
		position.y += screenHeight / 2.f - (bounds.y + bounds.height / 2.f);
	else if (bounds.y < 0.0f)
		position.y -= bounds.y;
	else if (bounds.y + bounds.height > screenHeight)
		position.y -= bounds.y + bounds.height - screenHeight;
}

void Player::Shoot(float dt, EntityManager& manager)
{
	const float bulletSpeed = 400.f;

	auto bullet = std::make_unique<Bullet>(
		position, 
		bulletSpeed, 
		Bullet::Type::NORMAL, 
		pBullettexPath);
	
	bullet->SetVelocity({ 0.0f, -bulletSpeed });
	bullet->SetRotation(-90.f); // For upwards
	bullet->SetFaction(Bullet::Faction::PLAYER);

	manager.AddEntity(std::move(bullet));
}

void Player::TriggerInvincibility(float dt)
{
	if (!m_invincible)
		return;

	m_invincibleTimer += dt;
	m_flashTimer += dt;
	
	if (m_flashTimer >= m_flashInterval) {
		m_flashVisible = !m_flashVisible;
		m_flashTimer = 0.0f;
	}
	
	if (m_invincibleTimer >= m_invincibleDuration) {
		m_invincible = false;
		m_flashVisible = true;
	}
}

void Player::DrawHealthBar()
{
	const float width = texture.id != 0
		? static_cast<float>(texture.width)
		: 40.0f; // Default width if no texture

	int health = GetHealth();
	
	Color color = health < 10 
		? RED
		: health < 25
			? YELLOW 
			: GREEN;

	const float healthRatio = std::clamp(health / 50.f, 0.f, 1.f);

	Rectangle healthRect = {
		position.x - width / 2,
		position.y + 30,
		width * healthRatio,
		10.f
	};

	Rectangle BorderRect = {
		healthRect.x,
		healthRect.y,
		width + 3.f,
		12.5f
	};

	DrawRectangleRec(healthRect, color);
	DrawRectangleLinesEx(BorderRect, 2.f, BLACK);
}

Player::Player(Vector2 pos, float speed, const char* texturePath) 
	: Entity(pos, 1.0f, -90.f), 
	m_speed(speed), 
	m_health(50)
{
	if (texturePath && texturePath[0] != '\0') {
		texture = LoadTexture(texturePath);

		if (texture.id == 0) {
			TraceLog(LOG_WARNING, "Failed to load player texture from path: %s", texturePath);
		}
	}

	// Bounding box is 80% of the texture
	SetCollisionBox(0.1f, 0.1f, 0.8f, 0.8f);
}

Player::~Player()
{
	if (texture.id != 0)
		UnloadTexture(texture);
}

void Player::Update(float dt)
{

}

void Player::Update(float dt, EntityManager& manager)
{
	if (IsKeyDown(KEY_W))
		position.y -= m_speed * dt;
	if (IsKeyDown(KEY_S))
		position.y += m_speed * dt;
	if (IsKeyDown(KEY_A))
		position.x -= m_speed * dt;
	if (IsKeyDown(KEY_D))
		position.x += m_speed * dt;

	StayInBounds();

	// Player shoot
	m_shotTimer += dt;

	if (m_shotTimer > m_coolDown && IsKeyDown(KEY_SPACE)) {
		Shoot(dt, manager);
		m_shotTimer = 0;
	}

	TriggerInvincibility(dt);
}

void Player::Draw()
{
	if (m_invincible && !m_flashVisible) {
		return;
	}

	// Draw the player
	if (texture.id != 0) {
		DrawTextureCentered();
	}
	else {
		// Draw a placement circle
		DrawCircleV(position, 20, BLUE);
	}

	DrawHealthBar();

#ifdef  DEBUG
	const Rectangle bounds = GetBounds();
	DrawRectangleLinesEx(bounds, 2.f, GREEN);
#endif //  DEBUG

}

void Player::TakeDamage(int damage)
{
	m_health -= damage;
}

void Player::TakeDamageOnImpact(int damage)
{
	if (!m_invincible) {
		m_health -= damage;
		m_invincible = true;
		m_invincibleTimer = 0.0f;
		m_flashTimer = 0.0f;
		m_flashVisible = true;
	}
}

Vector2& Player::GetPosition()
{
	return position;
}

int Player::GetHealth()
{
	return m_health;
}

