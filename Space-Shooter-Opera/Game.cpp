#include "Game.h"

void Game::Init()
{

	backgound[0] = LoadImage("assets/background/space_bcg_1.png");
	backgound[1] = LoadImage("assets/background/space_bcg_2.png");
	backgound[2] = LoadImage("assets/background/space_bcg_3.png");
	backgound[3] = LoadImage("assets/background/space_bcg_4.png");
	backgound[4] = LoadImage("assets/background/space_bcg_5.png");

	for (int i = 0; i < MAX_BACKGROUND; i++) {
		ImageResize(&backgound[i], SCREEN_WIDTH, SCREEN_HEIGHT);

		texBackgrounds[i] = LoadTextureFromImage(backgound[i]);
		UnloadImage(backgound[i]);
	}

	// Select random background
	m_selectBackground = rand() % 4;

	m_entManager.AddEntity(std::make_unique<Player>(
		Vector2{ SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100 },
		200.f,
		"assets/player/player.png"
	));

	// Add an enemy
	m_entManager.AddEntity(std::make_unique<Enemy>(
		Vector2{100, 0}, 200.f, Enemy::Type::SPEEDER, 
		"assets/player/player.png"
	));
}

void Game::Update()
{

	dt = GetFrameTime();
	

	// Get the player type and use the other update function
	auto player = m_entManager.GetEntityOfType<Player>();
	if (!player.empty()) {
		for (auto* player : player) {
			player->Update(dt, m_entManager);
		}
	}

	m_entManager.UpdateAll(dt);
}

void Game::Draw() const
{
	BeginDrawing();

	ClearBackground(RAYWHITE);

	DrawTexture(texBackgrounds[m_selectBackground], 0, 0, WHITE);
	
	m_entManager.DrawAll();

	EndDrawing();
}

void Game::FreeResources() const
{
	// Free the assets in game
}

bool Game::ShouldClose() const
{
	return WindowShouldClose();
}

Game::Game()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Opera!");
	SetTargetFPS(60);

	Init();
}

void Game::Run()
{
	while (!ShouldClose()) {
		Update();
		Draw();
	}

	FreeResources();

	CloseWindow();
}
