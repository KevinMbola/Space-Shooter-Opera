#pragma once
#include <raylib.h>

class Entity
{
protected:
	Vector2 position;
	Vector2 velocity;
	float scale;
	Vector2 origin;
	float rotation;
	bool active;
	Texture2D texture;
	Rectangle collisionBox;

public:
    Entity(Vector2 pos, float scl = 1.0f, float rot = 0.0f) :  
       position(pos), velocity({ 0, 0 }), scale(scl), origin({ 0.5, 0.5 }), rotation(rot), active(true), texture({ 0 }), collisionBox({0, 0, 1, 1}) {}

	virtual ~Entity() = default;

	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;

	bool IsActive() const { return active; }
	void SetActive(bool state) { active = state; }
	Vector2 GetPosition() const { return position; }

	float GetX() const { return position.x; }
	float GetY() const { return position.y; }

	// This will set our bounding box for each entity
	void SetCollisionBox(float offsetX, float offsetY, float width, float height) {
		collisionBox = { offsetX, offsetY, width, height };
	}

	Rectangle GetBounds() const {
		// If no texture is loaded, return the bounding box
		if (texture.id == 0) {
			return { position.x - 20, position.y - 20, 40, 40 };
		}

		float texWidth = texture.width * scale;
		float texHeight = texture.height * scale;

		// Apply the bounding box and origin
		return {
			position.x + (collisionBox.x * texWidth) - (texWidth * origin.x),
			position.y + (collisionBox.y * texHeight) - (texHeight * origin.y),
			collisionBox.width * texWidth,
			collisionBox.height * texHeight
		};
	}

	void DrawTextureCentered() {

		// If there is no texture just return
		if (texture.id == 0)
			return;

		Rectangle destRect = {
			position.x,
			position.y,
			texture.width * scale,
			texture.height * scale
		};

		Vector2 originPixels = {
			texture.width * origin.x,
			texture.height * origin.y
		};

		DrawTexturePro(
			texture,
			{ 0, 0, (float)texture.width, (float)texture.height },
			destRect,
			originPixels,
			rotation,
			WHITE
		);
	}
};

