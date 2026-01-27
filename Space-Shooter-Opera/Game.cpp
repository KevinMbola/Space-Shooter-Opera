#include "Game.h"



void Game::Init()  
{  
   // Select random background  
   //m_selectBackground = std::uniform_int_distribution<int>(0, MAX_BACKGROUND - 1)(m_rng);  

   InitStarryBackground();

   // Initialise the Meteors and shooting stars
   for (int i = 0; i < MAX_METEORS; i++) {
	   float x = std::uniform_real_distribution<float>(0, SCREEN_WIDTH)(m_rng);
	   float y = std::uniform_real_distribution<float>(0, SCREEN_HEIGHT)(m_rng);
	   float speed = std::uniform_real_distribution<float>(100, 200)(m_rng);
	   float size = std::uniform_real_distribution<float>(8, 20)(m_rng);
	   m_meteors.push_back({ {x, y}, {0, speed}, size, 0.7f });
   }

   m_shootingStars.clear();

   // Reserve space for entities to reduce reallocations  
   m_entManager.Reserve(128);  

   m_entManager.AddEntity(std::make_unique<Player>(  
       Vector2{ SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100 },  
       400.f,  
	   playertexPath
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
		if (m_state == GameState::GAMEOVER) {
			UpdateGameOver();
			DrawGameOver();
			if (IsKeyPressed(KEY_SPACE)) {
				m_state = GameState::MENU;
			}
		}
		else {
			
			UpdateStarfield(dt);
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
		DrawStarfield();
		DrawGame();
		
		if (m_state == GameState::PAUSED) {
			DrawPauseMenu();
		}
	}

	EndDrawing();
}

void Game::FreeResources() const
{
}

bool Game::ShouldClose() const
{
	return WindowShouldClose();
}

void Game::InitStarryBackground()
{
	auto makeStars = [&](std::vector<Star>& stars, int count, float minSpeed, float maxSpeed) {
		std::uniform_real_distribution<float> distX(0, SCREEN_WIDTH);
		std::uniform_real_distribution<float> distY(0, SCREEN_HEIGHT);
		std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);
		stars.clear();
		for (int i = 0; i < count; i++) {
			stars.push_back({ distX(m_rng), distY(m_rng), speedDist(m_rng) });
		}
		};

	makeStars(m_starsFar, 50, 10.f, 20.f);
	makeStars(m_starsMid, 30, 30.f, 50.f);
	makeStars(m_starsNear, 20, 60.f, 100.f);
}

void Game::DrawStarfield() const
{

	for (const auto& star : m_starsFar)
		DrawPixel((int)star.x, (int)star.y, Fade(WHITE, 0.3f));
	for (const auto& star : m_starsMid)
		DrawPixel((int)star.x, (int)star.y, Fade(WHITE, 0.6f));
	for (const auto& star : m_starsNear)
		DrawPixel((int)star.x, (int)star.y, WHITE);

	// Draw the twinkle
	for (const auto& star : m_starsNear)
		DrawPixel((int)star.x, (int)star.y, Fade(WHITE, star.alpha));

	if (m_state == GameState::PLAYING) {
		// Draw Earth (bottom center, behind everything)
		Vector2 earthCenter = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 80 };
		float earthRadius = 100.0f;
		DrawCircleV(earthCenter, earthRadius, Fade(BLUE, 0.7f));
		DrawCircleV(earthCenter, earthRadius * 0.7f, Fade(GREEN, 0.5f));
		// Simple "clouds" using white circles
		for (int i = 0; i < 5; ++i) {
			float angle = m_earthAngle + i * 1.2f;
			Vector2 cloudPos = {
				earthCenter.x + std::cos(angle) * (earthRadius * 0.6f),
				earthCenter.y + std::sin(angle) * (earthRadius * 0.4f)
			};
			DrawCircleV(cloudPos, 18, Fade(WHITE, 0.3f));
		}

		// Draw meteors
		for (const auto& m : m_meteors) {
			DrawCircleV(m.pos, m.size, Fade(GRAY, m.alpha));
			DrawCircleV({ m.pos.x + m.size * 0.3f, m.pos.y + m.size * 0.2f }, m.size * 0.5f, Fade(DARKGRAY, m.alpha * 0.7f));
		}

		// Draw shooting stars
		for (const auto& s : m_shootingStars) {
			Vector2 tail = { s.pos.x - s.vel.x * 0.05f, s.pos.y - s.vel.y * 0.05f };
			DrawLineEx(s.pos, tail, 2.5f, Fade(WHITE, s.alpha));
		}
	}
}

void Game::UpdateStarfield(float dt)
{
	auto updateStars = [&](std::vector<Star>& stars, float speedScale) {
		for (auto& star : stars) {
			star.y += star.speed * dt * speedScale;
			if (star.y > SCREEN_HEIGHT) {
				star.y = 0;
				star.x = std::uniform_real_distribution<float>(0, SCREEN_WIDTH)(m_rng);
			}
		}
	};

	updateStars(m_starsFar, 0.5f);
	updateStars(m_starsMid, 1.0f);
	updateStars(m_starsNear, 1.5f);

	// For random star twinkle
	for (auto& star : m_starsNear) {
		star.alpha += (std::uniform_real_distribution<float>(-0.01f, 0.01f)(m_rng));
		star.alpha = std::clamp(star.alpha, 0.5f, 1.0f);
	}

	if (m_state == GameState::PLAYING) {
		// Meteors
		for (auto& m : m_meteors) {
			m.pos.x += m.vel.x * dt;
			m.pos.y += m.vel.y * dt;
			if (m.pos.y > SCREEN_HEIGHT || m.pos.x < -50 || m.pos.x > SCREEN_WIDTH + 50) {
				m.pos.x = std::uniform_real_distribution<float>(0, SCREEN_WIDTH)(m_rng);
				m.pos.y = std::uniform_real_distribution<float>(-100, -10)(m_rng);
				m.vel.x = std::uniform_real_distribution<float>(-30, 30)(m_rng);
				m.vel.y = std::uniform_real_distribution<float>(100, 200)(m_rng);
				m.size = std::uniform_real_distribution<float>(8, 20)(m_rng);
			}
		}

		// Shooting stars (spawn randomly)
		if (std::uniform_real_distribution<float>(0, 1)(m_rng) < 0.01f) {
			float x = std::uniform_real_distribution<float>(0, SCREEN_WIDTH)(m_rng);
			float y = std::uniform_real_distribution<float>(-50, 0)(m_rng);
			float angle = std::uniform_real_distribution<float>(-0.5f, 0.5f)(m_rng); // slight angle
			float speed = std::uniform_real_distribution<float>(600, 900)(m_rng);
			m_shootingStars.push_back({
				{x, y},
				{speed * std::cos(angle), speed * std::sin(angle) + 400},
				std::uniform_real_distribution<float>(60, 120)(m_rng),
				1.0f,
				0.5f // duration
				});
		}
		for (auto& s : m_shootingStars) {
			s.pos.x += s.vel.x * dt;
			s.pos.y += s.vel.y * dt;
			s.timer -= dt;
			s.alpha -= dt * 2.0f;
		}
		m_shootingStars.erase(
			std::remove_if(m_shootingStars.begin(), m_shootingStars.end(), [](const ShootingStar& s) {
				return s.timer <= 0 || s.alpha <= 0.0f;
				}),
			m_shootingStars.end()
		);

		// Earth rotation
		m_earthAngle += dt * 0.05f;
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
				enemytexPath
			));
		}
		break;
	case Game::WaveType::SPEEDSTERS:
		for (int i = 0; i < n; i++) {
			m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
				Vector2{ 100.0f + (i % 5) * 60.f, -50}, 450.f,
				enemytexPath
			));
		}
		break;
	case Game::WaveType::MIXED:
		for (int i = 0; i < n; i++) {
			if (std::uniform_int_distribution<int>(0, 1)(m_rng) == 0) {
				m_entManager.AddEntity(std::make_unique<ShooterEnemy>(
					Vector2{ 50.0f + (i % 5) * 80.0f, -50.0f - (i / 5) * 60.0f }, 200.f,
					enemytexPath
				));
			}
			else {
				m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
					Vector2{ 100.0f + (i % 5) * 60.0f, -50.0f - (i / 5) * 60.0f }, 450.f,
					enemytexPath
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
	ClearBackground(BLACK);

	//// Draw the parallax background
	//for (int i = 0; i < MAX_BACKGROUND; i++) {
	//	DrawTextureEx(texBackgrounds[i], { 0, m_backgroundOffset[i] - SCREEN_HEIGHT }, 0, 1.0f, WHITE);
	//	DrawTextureEx(texBackgrounds[i], { 0, m_backgroundOffset[i] }, 0, 1.0f, WHITE);
	//}
	
	m_entManager.DrawAll();

	DrawText(TextFormat("SCORE: %02i", m_score), 10, 10, 20, RAYWHITE);
}

void Game::UpdateGame()
{
	//for (int i = 0; i < MAX_BACKGROUND; i++) {
	//	m_backgroundOffset[i] += (i + 1) * 10.0f * dt; // Farther background move slower
	//	if (m_backgroundOffset[i] > SCREEN_HEIGHT)
	//		m_backgroundOffset[i] -= SCREEN_HEIGHT;
	//}

	
	
	// Get the player type and use the other update function
	auto player = m_entManager.GetEntityOfType<Player>();
	Vector2 pPos = { 0, 0 };
	if (!player.empty()) {
		for (auto* player : player) {
			pPos = player->GetPosition();
			player->Update(dt, m_entManager);
			if (player->GetHealth() <= 0) {
				player->SetActive(false);
				m_state = GameState::GAMEOVER;
			}
		}
	}

	//// Get the shooter enemy and update
	auto shooters = m_entManager.GetEntityOfType<ShooterEnemy>();
	if (!shooters.empty()) {
		for (auto* shooter : shooters) {
			shooter->Update(dt, m_entManager, pPos);
			if (shooter->GetHealth() <= 0 && !shooter->IsActive()) {
				m_score += shooter->GetScoreValue();
			}
		}
	}

	auto speedsters = m_entManager.GetEntityOfType<SpeedsterEnemy>();
	if (!speedsters.empty()) {
		for (auto* speed : speedsters) {
			speed->Update(dt, m_entManager, pPos);
			if (speed->GetHealth() <= 0 && !speed->IsActive()) {
				m_score += speed->GetScoreValue();
			}
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
		//TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
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
		//TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
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

void Game::DrawGameOver() const
{
	if (m_state == GameState::GAMEOVER) {
		int fontSize = SCREEN_HEIGHT / 12;
		int subfontSize = SCREEN_HEIGHT / 24;
		const char* msg = "GAME OVER";
		const char* subMsg = "The world is to be destoryed!\n Press Space to restart";
		int textWidth = MeasureText(msg, fontSize);
		int subtextWidth = MeasureText(subMsg, subfontSize);
		DrawRectangle(0, SCREEN_HEIGHT / 2 - fontSize + subfontSize, SCREEN_WIDTH, fontSize * 2, Fade(RAYWHITE, 0.5f));
		DrawText(msg, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, RED);
		DrawText(subMsg, (SCREEN_WIDTH - subtextWidth) / 2, SCREEN_HEIGHT / 1.7 - subfontSize / 2, subfontSize, RED);
	}
}

void Game::UpdateGameOver()
{
	
}

Game::Game()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Opera!");
	SetTargetFPS(60);

	Init();
}

void Game::Run()
{
#ifndef __EMSCRIPTEN__
	while (!ShouldClose()) {
		Update();
		Draw();
	}

	FreeResources();
	CloseWindow();
#endif // !__EMSCRIPTEN__
}
