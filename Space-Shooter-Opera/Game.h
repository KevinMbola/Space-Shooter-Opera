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
#define MAX_METEORS 8


enum class GameState { MENU, PLAYING, PAUSED, GAMEOVER };

struct Star {
	float x, y, speed, alpha;
};

struct Meteor {
	Vector2 pos, vel;
	float size, alpha;
};

struct ShootingStar {
	Vector2 pos, vel;
	float length, alpha, timer;
};

class Game
{
public:
	
private:
	const char* enemytexPath = "assets/enemy/enemy.png";
	const char* playertexPath = "assets/player/player.png";
	const char* bgMusicPath = "assets/sfx/fun-bgMusic.mp3";

	// Music and sfx
	Music m_bgMusic;

	// Background
	Image backgound[MAX_BACKGROUND];
	Texture2D texBackgrounds[MAX_BACKGROUND];
	//int m_selectBackground = 0;
	//float m_backgroundOffset[MAX_BACKGROUND] = { 0 };	// This is to have a scrolling effect
	
	std::vector<Star> m_starsNear, m_starsMid, m_starsFar;
	std::vector<Meteor> m_meteors;
	std::vector<ShootingStar> m_shootingStars;
	float m_earthAngle = 0.0f; // This will have a slow rotation

	// Game State manager
	GameState m_state = GameState::MENU;

#ifdef __EMSCRIPTEN__
	std::vector<std::string> m_menuItems = { "Start Game" };
#else
	std::vector<std::string> m_menuItems = { "Start Game", "Quit" };
#endif // __EMSCRIPTEN__
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

	// To keep track of score
	int m_score = 0;

private:


	void InitStarryBackground();
	void DrawStarfield() const;
	void UpdateStarfield(float dt);
	
	// Handle the waves 
	void SpawnWave();

	// Menu logic
	void DrawMenu() const;
	void UpdateMenu();

	// Game logic
	void DrawGame() const;
	void UpdateGame();

	// Pause menu
	void DrawPauseMenu() const;

	// Game Over Screen
	void DrawGameOver() const;
	void UpdateGameOver();

public:
	Game();
	~Game() {}

	void Init();
	void Update();
	void Draw() const;
	void FreeResources() const;
	bool ShouldClose() const;

	void Run();

};
