#include "Game.h"



void Game::Init()  
{  
   m_stars.clear();  
   std::uniform_real_distribution<float> xDist(0, SCREEN_WIDTH);  
   std::uniform_real_distribution<float> yDist(0, SCREEN_HEIGHT);  
   std::uniform_real_distribution<float> speedDist(20.f, 80.f);  

   for (int i = 0; i < STAR_COUNT; i++) {  
       Star star;  
       star.x = xDist(m_rng);  
       star.y = yDist(m_rng);  
       star.speed = speedDist(m_rng);  
       m_stars.push_back(star);  
   }  

   for (int i = 0; i < MAX_BACKGROUND; i++) {  
       backgound[i] = LoadImage(TextFormat("assets/background/space_bcg_%d.png", i + 1));  
       ImageResize(&backgound[i], SCREEN_WIDTH, SCREEN_HEIGHT);  
       texBackgrounds[i] = LoadTextureFromImage(backgound[i]);  
       UnloadImage(backgound[i]);  
   }  

   // Select random background  
   m_selectBackground = std::uniform_int_distribution<int>(0, MAX_BACKGROUND - 1)(m_rng);  

   // Reserve space for entities to reduce reallocations  
   m_entManager.Reserve(128);  

   m_entManager.AddEntity(std::make_unique<Player>(  
       Vector2{ SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100 },  
       400.f,  
       "assets/player/player.png"  
   ));  
}

void Game::Update()
{
	dt = GetFrameTime();
	
	if (m_state == GameState::MENU) {
		UpdateStarfield(dt);
		UpdateMenu();
	}
	else {
		if (m_state == GameState::PAUSED) {
			if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
				m_state = GameState::PLAYING;
			}
			return;
		}
		else {
			UpdateGame();
		}
		
	}
}

void Game::Draw() const
{
	BeginDrawing();

	if (m_state == GameState::MENU) {
		DrawStarfield();
		DrawMenu();
	}
	else {
		DrawGame();
		
		if (m_state == GameState::PAUSED) {
			DrawPauseMenu();
		}
	}

	EndDrawing();
}

void Game::FreeResources() const
{
	// Free the assets in game
	for (int i = 0; i < MAX_BACKGROUND; i++) {
		UnloadTexture(texBackgrounds[i]);
	}
}

bool Game::ShouldClose() const
{
	return WindowShouldClose();
}

void Game::DrawStarfield() const
{
	for (const auto& star : m_stars) {
		DrawPixel((int)star.x, (int)star.y, WHITE);
	}
}

void Game::UpdateStarfield(float dt)
{
	for (auto& star : m_stars) {
		star.y += star.speed * dt;
		if (star.y > SCREEN_HEIGHT) {
			star.y = 0;
			star.x = std::uniform_real_distribution<float>(0, SCREEN_WIDTH)(m_rng);
		}
	}
}

void Game::SpawnWave()
{
	m_waveActive = true;
	m_waveTimer = 0.0f;
	int n = 2 + m_waveNumber * 2; // Increase with every wave

	switch (m_currentWave)
	{
	case Game::WaveType::SHOOTERS:
		for (int i = 0; i < n; i++) {
			m_entManager.AddEntity(std::make_unique<ShooterEnemy>(
				Vector2{ 50.0f + i * 80.0f, 0 }, 200.f,
				"assets/player/player.png"
			));
		}
		break;
	case Game::WaveType::SPEEDSTERS:
		for (int i = 0; i < n; i++) {
			m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
				Vector2{ 100.0f + (i % 5) * 60.f, -50}, 450.f,
				"assets/player/player.png"
			));
		}
		break;
	case Game::WaveType::MIXED:
		for (int i = 0; i < n; i++) {
			if (std::uniform_int_distribution<int>(0, 1)(m_rng) == 0) {
				m_entManager.AddEntity(std::make_unique<ShooterEnemy>(
					Vector2{ 50.0f + (i % 5) * 80.0f, -50.0f - (i / 5) * 60.0f }, 200.f,
					"assets/player/player.png"
				));
			}
			else {
				m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
					Vector2{ 100.0f + (i % 5) * 60.0f, -50.0f - (i / 5) * 60.0f }, 450.f,
					"assets/player/player.png"
				));
			}
		}
		break;
	default:
		break;
	}
}

void Game::DrawMenu() const
{
	ClearBackground(BLACK);

	// Title
	const char* title = "SPACE OPERA";
	int titleFontSize = SCREEN_HEIGHT / 12;
	int titleWidth = MeasureText(title, titleFontSize);
	DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, SCREEN_HEIGHT / 8, titleFontSize, DARKBLUE);

	// Menu items
	int menuFontSize = SCREEN_HEIGHT / 20;
	int spacing = menuFontSize * 2;
	int startY = SCREEN_HEIGHT / 2 - (int(m_menuItems.size()) * spacing) / 2;

	Vector2 mouse = GetMousePosition();

	for (size_t i = 0; i < m_menuItems.size(); i++) {
		Color c = (i == m_selectedMenuItem) ? RED : RAYWHITE;
		int textWidth = MeasureText(m_menuItems[i].c_str(), menuFontSize);
		int x = (SCREEN_WIDTH - textWidth) / 2;
		int y = startY + int(i) * spacing;

		// Mouse hover effect
		Rectangle itemRect = { (float)x, (float)y, (float)textWidth, (float)menuFontSize };
		if (CheckCollisionPointRec(mouse, itemRect)) {
			c = MAROON;
		}

		DrawText(m_menuItems[i].c_str(), x, y, menuFontSize, c);
	}
}

void Game::UpdateMenu()
{

	// Keyboard navigation
	if (IsKeyPressed(KEY_DOWN)) {
		m_selectedMenuItem = (m_selectedMenuItem + 1) % m_menuItems.size();
	}
	if (IsKeyPressed(KEY_UP)) {
		m_selectedMenuItem = (m_selectedMenuItem - 1 + m_menuItems.size()) % m_menuItems.size();
	}

	// Mouse hover
	Vector2 mouse = GetMousePosition();
	int menuFontSize = SCREEN_HEIGHT / 20;
	int spacing = menuFontSize * 2;
	int startY = SCREEN_HEIGHT / 2 - (int(m_menuItems.size()) * spacing) / 2;

	for (size_t i = 0; i < m_menuItems.size(); ++i) {
		int textWidth = MeasureText(m_menuItems[i].c_str(), menuFontSize);
		int x = (SCREEN_WIDTH - textWidth) / 2;
		int y = startY + int(i) * spacing;
		Rectangle itemRect = { (float)x, (float)y, (float)textWidth, (float)menuFontSize };
		if (CheckCollisionPointRec(mouse, itemRect)) {
			m_selectedMenuItem = (int)i;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				// Mouse click = select
				if (m_selectedMenuItem == 0) {
					m_state = GameState::PLAYING;
					Init(); // Reset game state if needed
				}
				else if (m_selectedMenuItem == 1) {
					CloseWindow();
				}
			}
		}
	}

	// Keyboard select
	if (IsKeyPressed(KEY_ENTER)) {
		if (m_selectedMenuItem == 0) {
			m_state = GameState::PLAYING;
			Init(); // Reset game state if needed
		}
		else if (m_selectedMenuItem == 1) {
			CloseWindow();
		}
	}
}

void Game::DrawGame() const
{
	ClearBackground(RAYWHITE);

	DrawTexture(texBackgrounds[m_selectBackground], 0, 0, WHITE);

	m_entManager.DrawAll();
}

void Game::UpdateGame()
{
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
	if (!speedsters.empty()) {
		for (auto* speed : speedsters) {
			speed->Update(dt, m_entManager, pPos);
		}
	}

	m_entManager.UpdateAll(dt);


	// Manage the waves

	// Check if all enemies are gone to start the next wave
	auto enemies = m_entManager.GetEntityOfType<Enemy>();
	if (m_waveActive && enemies.empty()) {
		m_waveActive = false;
		m_waveTimer = 0.0f;
		m_waveNumber++;
	}

	if (!m_waveActive) {
		m_waveTimer += dt;
		if (m_waveTimer >= m_timeBetweenWaves) {
			m_waveNumber = std::uniform_int_distribution<int>(0, 2)(m_rng);
			if (m_waveNumber % 3 == 0)
				m_currentWave = WaveType::SHOOTERS;
			else if (m_waveNumber % 3 == 1)
				m_currentWave = WaveType::SPEEDSTERS;
			else
				m_currentWave = WaveType::MIXED;
			SpawnWave();
		}
	}

	// Player colliding with an Enemy
	m_entManager.CheckCollision<Enemy, Player>([](Enemy* e, Player* p) {
		p->TakeDamageOnImpact(1);
		TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
		});

	// Bullet colliding with an Enemy Type
	m_entManager.CheckCollision<Bullet, Enemy>([](Bullet* b, Enemy* e) {
		if (b->GetFaction() != Bullet::Faction::PLAYER) // Only player bullets hurt enemies
			return;
		if (b->GetOwner() == e)
			return;
		b->SetActive(false);
		e->TakeDamage(1);
	});

	// Bullet Colliding with Player
	m_entManager.CheckCollision<Bullet, Player>([](Bullet* b, Player* p) {
		if (b->GetFaction() != Bullet::Faction::ENEMY) // Only enemy bullets hurt player
			return;
		if (b->GetOwner() == p)
			return;
		b->SetActive(false);
		p->TakeDamage(1);
		TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
	});

	if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
		m_state = (m_state == GameState::PLAYING) ? GameState::PAUSED : GameState::PLAYING;
	}

}

void Game::DrawPauseMenu() const
{
	if (m_state == GameState::PAUSED) {
		int fontSize = SCREEN_HEIGHT / 12;
		const char* msg = "PAUSED";
		int textWidth = MeasureText(msg, fontSize);
		DrawRectangle(0, SCREEN_HEIGHT / 2 - fontSize, SCREEN_WIDTH, fontSize * 2, Fade(BLACK, 0.5f));
		DrawText(msg, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, YELLOW);

	}
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
