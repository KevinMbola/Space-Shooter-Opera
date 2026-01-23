#pragma 

#include <raylib.h>
#include "EntityManager.h"

#include "Player.h"
#include "Bullet.h"
#include "ShooterEnemy.h"
#include "SpeedsterEnemy.h"


#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800

#define MAX_BACKGROUND 5
#define STAR_COUNT 100

struct Star {
	float x, y, speed;
};

class Game
{
private:
	// Background
	Image backgound[MAX_BACKGROUND];
	Texture2D texBackgrounds[MAX_BACKGROUND];
	int m_selectBackground = 0;

	// Menu Background
	std::vector<Star> m_stars;

	// Game State manager
	enum class GameState { MENU, PLAYING, PAUSED, GAMEOVER };
	GameState m_state = GameState::MENU;

	std::vector<std::string> m_menuItems = { "Start Game", "Quit" };
	int m_selectedMenuItem = 0;

	// Entity manager
	EntityManager m_entManager;
	float dt;
	float m_waitTimer = 0.0f;
	float m_waitDuration = 3.f;

	std::mt19937 m_rng{ std::random_device{}() };

	// Wave attribs
	enum class WaveType { SHOOTERS, SPEEDSTERS, MIXED };
	WaveType m_currentWave = WaveType::SHOOTERS;
	int m_waveNumber = 0;
	int m_enemiesToSpawn = 0;
	float m_waveTimer = 0.0f;
	float m_timeBetweenWaves = 3.0f;
	bool m_waveActive = false;

private:
	void Init();
	void Update();
	void Draw() const;
	void FreeResources() const;
	bool ShouldClose() const;

	void DrawStarfield() const;
	void UpdateStarfield(float dt);
	

	void SpawnWave();

	// Menu logic
	void DrawMenu() const;
	void UpdateMenu();

	// Game logic
	void DrawGame() const;
	void UpdateGame();

	// Pause menu
	void DrawPauseMenu() const;

public:
	Game();
	~Game() {}

	void Run();

};
