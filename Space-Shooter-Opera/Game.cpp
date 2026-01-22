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
		400.f,
		"assets/player/player.png"
	));

	// Add an enemy
	m_entManager.AddEntity(std::make_unique<ShooterEnemy>(
		Vector2{100, 0}, 200.f, 
		"assets/player/player.png"
	));

	// Add an enemy
	m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
		Vector2{ 100, 0 }, 450.f,
		"assets/player/player.png"
	));
}

void Game::Update()
{

	dt = GetFrameTime();
	

	// Get the player type and use the other update function
	auto player = m_entManager.GetEntityOfType<Player>();
	Vector2 pPos = { 0, 0 };
	if (!player.empty()) {
		for (auto* player : player) {
			pPos = player->GetPosition();
			player->Update(dt, m_entManager);
		}
	}

	//// Get the shooter enemy and update
	auto shooters = m_entManager.GetEntityOfType<ShooterEnemy>();
	if (!shooters.empty()) {
		for (auto* shooter : shooters) {
			shooter->Update(dt, m_entManager, pPos);
		}
	}

	auto speedsters = m_entManager.GetEntityOfType<SpeedsterEnemy>();
	//if (!speedsters.empty()) {
	//	for (auto* speed : speedsters) {
	//		speed->Update(dt, m_entManager, pPos);
	//	}
	//}

	m_entManager.UpdateAll(dt);

	// Player colliding with an Enemy
	m_entManager.CheckCollision<Enemy, Player>([](Enemy* e, Player* p) {
		p->TakeDamageOnImpact(1);
		TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
	});

	// Bullet colliding with an Enemy Type
	m_entManager.CheckCollision<Bullet, Enemy>([](Bullet* b, Enemy* e) {
		if (b->GetOwner() == e)
			return;
		b->SetActive(false);
		e->TakeDamage(1);


	});

	// Bullet Colliding with Player
	m_entManager.CheckCollision<Bullet, Player>([](Bullet* b, Player* p) {
		if (b->GetOwner() == p)
			return;
		b->SetActive(false);
		p->TakeDamage(1);
		TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
	});


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
