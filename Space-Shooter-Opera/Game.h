#pragma 

#include <raylib.h>
#include "EntityManager.h"

#include "Player.h"
#include "Bullet.h"
#include "ShooterEnemy.h"


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

#define MAX_BACKGROUND 5

class Game
{
private:
	Image backgound[MAX_BACKGROUND];
	Texture2D texBackgrounds[MAX_BACKGROUND];

	int m_selectBackground = 0;

	EntityManager m_entManager;

	float dt;

private:
	void Init();
	void Update();
	void Draw() const;
	void FreeResources() const;
	bool ShouldClose() const;

public:
	Game();
	~Game() {}

	void Run();

};
