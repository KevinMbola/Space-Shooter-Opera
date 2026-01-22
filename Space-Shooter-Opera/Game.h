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

class Game
{
private:
	Image backgound[MAX_BACKGROUND];
	Texture2D texBackgrounds[MAX_BACKGROUND];

	int m_selectBackground = 0;

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

	void SpawnWave();

public:
	Game();
	~Game() {}

	void Run();

};
