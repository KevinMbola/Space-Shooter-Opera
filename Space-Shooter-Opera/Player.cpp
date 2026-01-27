#include "Player.h"
#include "EntityManager.h"
#include "Bullet.h"
#include <memory>

void Player::StayInBounds()
{
	float x = GetPosition().x;
	float y = GetPosition().y;

	if (x - 20 <= 0)
		x = 20;
	else if (x + texture.width > SCREEN_WIDTH)
		x = SCREEN_WIDTH - texture.width;
	else if (y - 20 <= 0)
		y = 20;
	else if (y + texture.height > SCREEN_HEIGHT)
		y = SCREEN_HEIGHT - texture.height;

	// Set x and y to the player posiiton
	position.x = x;
	position.y = y;
}

void Player::Shoot(float dt, EntityManager& manager)
{
	float bulletSpeed = 400.f;
	Vector2 bulletVel = { 0, -bulletSpeed }; // For upward momentum
	auto bullet = std::make_unique<Bullet>(position, bulletSpeed, Bullet::Type::NORMAL, pBullettexPath);
	bullet->SetVelocity(bulletVel);
	bullet->SetRotation(-90.f); // For upwards
	bullet->SetOwner(this);
	bullet->SetFaction(Bullet::Faction::PLAYER);
	manager.AddEntity(std::move(bullet));
}

void Player::TriggerInvincibility(float dt)
{
	if (m_invincible) {
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
}

void Player::DrawHealthBar()
{
	float healthBarWidth = texture.width;
	int health = GetHealth();
	Color color = (health < 10) ? RED
		: (health < 25) ? YELLOW 
		: GREEN;

	Rectangle healthRect = {
		position.x - healthBarWidth / 2,
		position.y + 30,
		healthBarWidth * (health / 50.f),
		10
	};

	Rectangle healthBorderRect = {
		healthRect.x,
		healthRect.y,
		healthBarWidth + 3,
		12.5f
	};

	DrawRectangleRec(healthRect, color);
	DrawRectangleLinesEx(healthBorderRect, 2, BLACK);
	//DrawRectangle(position.x - healthBarWidth / 2, position.y + 10, healthBarWidth * (health / 50.f), 10, GREEN);
}

Player::Player(Vector2 pos, float speed, const char* texturePath) :
	Entity(pos, 1.0f, -90.f), m_speed(speed), m_health(50)
{
	if (texturePath && texturePath[0] != '\0') {
		texture = LoadTexture(texturePath);
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
	if (m_shotTimer > m_coolDown) {
		if (IsKeyDown(KEY_SPACE)) {
			Shoot(dt, manager);
		}
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
	Rectangle bounds = GetBounds();
	DrawRectangleLinesEx(bounds, 2, GREEN);
#endif //  DEBUG

}

void Player::TakeDamage(int damage)
{
	m_health -= damage;
}

void Player::TakeDamageOnImpact(int damage)
{
	TraceLog(LOG_INFO, "Hit on Impact!");
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

