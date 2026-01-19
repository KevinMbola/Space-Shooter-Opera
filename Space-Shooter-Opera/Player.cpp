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
	manager.AddEntity(std::make_unique<Bullet>(
		position, 200.f, Bullet::Type::NORMAL, "assets/player/playerBullet.png"
	));
}

Player::Player(Vector2 pos, float speed, const char* texturePath) :
	Entity(pos, 1.0f, -90.f), m_speed(speed), m_health(100)
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
}

void Player::Draw()
{
	// Draw the player
	if (texture.id != 0) {
		DrawTextureCentered();
	}
	else {
		// Draw a placement circle
		DrawCircleV(position, 20, BLUE);
	}

#ifdef  DEBUG
	Rectangle bounds = GetBounds();
	DrawRectangleLinesEx(bounds, 2, GREEN);
#endif //  DEBUG

}

Vector2& Player::GetPosition()
{
	return position;
}

