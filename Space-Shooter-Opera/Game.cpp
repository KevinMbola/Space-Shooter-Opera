#include "Game.h"



void Game::Init()  
{  
   // Select random background  
   //m_selectBackground = std::uniform_int_distribution<int>(0, GameConfig::MAX_BACKGROUND - 1)(m_rng);  

   InitStarryBackground();

   // We always need to clear the meteors and shooting stars when we restart the game
   m_meteors.clear();
   m_meteors.reserve(GameConfig::MAX_METEORS);

   // Initialise the Meteors and shooting stars
   for (int i = 0; i < GameConfig::MAX_METEORS; i++) {
	   float x = 
		   std::uniform_real_distribution<float>(0, GameConfig::ScreenWidth)(m_rng);
	   float y = 
		   std::uniform_real_distribution<float>(0, GameConfig::ScreenHeight)(m_rng);
	   float speed = 
		   std::uniform_real_distribution<float>(100, 200)(m_rng);
	   float size = 
		   std::uniform_real_distribution<float>(8, 20)(m_rng);

	   m_meteors.push_back({ {x, y}, {0, speed}, size, 0.7f });
   }

   m_shootingStars.clear();
   m_earthAngle = 0.0f;


   m_score = 0;
   m_waveActive = false;
   m_waveTimer = 0.0f;
   m_waveNumber = 0;
   m_currentWave = WaveType::SHOOTERS;

   // Clear entities before we reserver
   m_entManager.Clear();

   // Reserve space for entities to reduce reallocations  
   m_entManager.Reserve(128);  

   m_entManager.AddEntity(std::make_unique<Player>(  
       Vector2{ GameConfig::ScreenWidth / 2, GameConfig::ScreenHeight - 100 },  
       400.f,  
	   playertexPath
   )); 

}

void Game::Update()
{
	dt = GetFrameTime();

	switch (m_state)
	{
	case GameState::MENU:
		UpdateStarfield(dt);
		UpdateMenu();
		break;
	case GameState::PLAYING:
		UpdateStarfield(dt);
		UpdateGame();
		break;
	case GameState::PAUSED:
		if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
			m_state = GameState::PLAYING;
		}
		break;
	case GameState::GAMEOVER:
		UpdateGameOver();
		if (IsKeyPressed(KEY_SPACE)) {
			m_state = GameState::MENU;
		}
		break;
	}
}

void Game::Draw() const
{
	BeginDrawing();
	ClearBackground(BLACK);

	DrawStarfield();

	if (m_state == GameState::MENU) {
		
		DrawMenu();
	}
	else {
		DrawGame();
		
		if (m_state == GameState::PAUSED) {
			DrawPauseMenu();
		}
		else if (m_state == GameState::GAMEOVER){
			DrawGameOver();
		}
	}

	EndDrawing();
}

void Game::FreeResources()
{
	// Clear the entity manager and all entities
	m_entManager.Clear();

	m_shootingStars.clear();
	m_starsFar.clear();
	m_starsMid.clear();
	m_starsNear.clear();
	m_meteors.clear();
}

bool Game::ShouldClose() const
{
	return m_isShuttingDown || WindowShouldClose();
}

void Game::InitStarryBackground()
{
	auto makeStars = [this](
		std::vector<Star>& stars, 
		int count, 
		float minSpeed, 
		float maxSpeed) 
	{
		std::uniform_real_distribution<float> distX(
			0, static_cast<float>(GameConfig::ScreenWidth));
		std::uniform_real_distribution<float> distY(
			0, static_cast<float>(GameConfig::ScreenHeight));
		std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);
		
		stars.clear();
		stars.reserve(static_cast<std::size_t>(count));

		for (int i = 0; i < count; ++i) {
			stars.push_back({ distX(m_rng), distY(m_rng), speedDist(m_rng), 1.0f });
		}
	};

	makeStars(m_starsFar, 50, 10.f, 20.f);
	makeStars(m_starsMid, 30, 30.f, 50.f);
	makeStars(m_starsNear, 20, 60.f, 100.f);
}

void Game::DrawStarfield() const
{
	if (m_isShuttingDown) return;

	for (const auto& star : m_starsFar) {
		DrawPixel(
			(int)star.x,
			(int)star.y,
			Fade(WHITE, 0.3f));
	}
	for (const auto& star : m_starsMid) {
		DrawPixel(
			(int)star.x,
			(int)star.y,
			Fade(WHITE, 0.6f));
	}
	for (const auto& star : m_starsNear) {
		DrawPixel(
			(int)star.x,
			(int)star.y,
			Fade(WHITE, star.alpha));
	}

	// Don't draw meteors or shooting stars in the menu
	if (m_state != GameState::PLAYING) {
		return;
	}

	// Create the earths center
	const Vector2 earthCenter = {
		GameConfig::ScreenWidth / 2.0f,
		GameConfig::ScreenHeight - 80.f
	};

	constexpr float earthRadius = 100.0f;

	// Draw the earth with a simple gradient effect (blue outer, green inner and behind everything)
	DrawCircleV(earthCenter, earthRadius, Fade(BLUE, 0.7f));
	DrawCircleV(earthCenter, earthRadius * 0.7f, Fade(GREEN, 0.5f));

	// Simple "clouds" using white circles
	for (int i = 0; i < 5; ++i) {
		const float angle = m_earthAngle + i * 1.2f;
		const Vector2 cloudPos = {
			earthCenter.x + std::cos(angle) * (earthRadius * 0.6f),
			earthCenter.y + std::sin(angle) * (earthRadius * 0.4f)
		};
		DrawCircleV(cloudPos, 18.f, Fade(WHITE, 0.3f));
	}


	// Draw meteors
	for (const auto& m : m_meteors) {
		DrawCircleV(m.pos, m.size, Fade(GRAY, m.alpha));

		DrawCircleV(
			{
				m.pos.x + m.size * 0.3f,
				m.pos.y + m.size * 0.2f
			},
			m.size * 0.5f,
			Fade(DARKGRAY, m.alpha * 0.7f));
	}

	// Draw shooting stars
	for (const auto& s : m_shootingStars) {
		Vector2 tail = {
			s.pos.x - s.vel.x * 0.05f,
			s.pos.y - s.vel.y * 0.05f
		};
		DrawLineEx(s.pos, tail, 2.5f, Fade(WHITE, s.alpha));
	}
}

void Game::UpdateStarfield(float elapsed)
{
	auto updateStars = [this, elapsed](
		std::vector<Star>& stars, 
		float speedScale) 
	{
		for (auto& star : stars) 
		{
			star.y += star.speed * elapsed * speedScale;
			
			if (star.y > GameConfig::ScreenHeight) {
				star.y = 0.f;
				star.x = std::uniform_real_distribution<float>(
					0.f, 
					static_cast<float>(GameConfig::ScreenWidth))(m_rng);
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

	if (m_state != GameState::PLAYING)
		return; // Don't update meteors or shooting stars in the menu or anywhere else

		// Meteors
		for (auto& m : m_meteors) {
			m.pos.x += m.vel.x * dt;
			m.pos.y += m.vel.y * dt;

			if (m.pos.y > GameConfig::ScreenHeight || 
				m.pos.x < -50 || 
				m.pos.x > GameConfig::ScreenWidth + 50) 
			{
				m.pos.x = std::uniform_real_distribution<float>(
					0.f, static_cast<float>(GameConfig::ScreenWidth))(m_rng);
				m.pos.y = 
					std::uniform_real_distribution<float>(-100.f, -10.f)(m_rng);
				m.vel.x = 
					std::uniform_real_distribution<float>(-30.f, 30.f)(m_rng);
				m.vel.y = 
					std::uniform_real_distribution<float>(100.f, 200.f)(m_rng);
				m.size = 
					std::uniform_real_distribution<float>(8.f, 20.f)(m_rng);
			}
		}

		// Shooting stars (spawn randomly)
		if (std::uniform_real_distribution<float>(0, 1)(m_rng) < 0.01f) {
			float x = 
				std::uniform_real_distribution<float>(0, GameConfig::ScreenWidth)(m_rng);
			float y = 
				std::uniform_real_distribution<float>(-50, 0)(m_rng);
			float angle = 
				std::uniform_real_distribution<float>(-0.5f, 0.5f)(m_rng); // slight angle
			float speed = 
				std::uniform_real_distribution<float>(600, 900)(m_rng);
			m_shootingStars.push_back({
				{x, y},
				{
					speed * std::cos(angle), 
					speed * std::sin(angle) + 400
				},
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
			std::remove_if(
				m_shootingStars.begin(), 
				m_shootingStars.end(), 
				[](const ShootingStar& s) {
					return s.timer <= 0 || s.alpha <= 0.0f;
				}),
			m_shootingStars.end()
		);

		// Earth rotation
		m_earthAngle += dt * 0.05f;
}

void Game::SpawnWave()
{
	m_waveActive = true;
	m_waveTimer = 0.0f;

	// Limit the number of enemies based on the wave number
	const int cappedWaveAmount = std::clamp(m_waveNumber, 0, 49);
	const int count = 2 + cappedWaveAmount * 2; // Increase with every wave


	for (int i = 0; i < count; ++i) {
		WaveType waveType = m_currentWave;

		if (m_currentWave == WaveType::MIXED) {
			waveType = (std::uniform_int_distribution<int>(0, 1)(m_rng) == 0) ?
				WaveType::SHOOTERS : WaveType::SPEEDSTERS;
		}

		if (waveType == WaveType::SHOOTERS) {
			m_entManager.AddEntity(std::make_unique<ShooterEnemy>(
				Vector2{ 
					50.0f + (i % 5) * 80.0f,
					-50.0f - (i / 5) * 60.0f 
				}, 
				200.f,
				enemytexPath
			));
		}
		else if (waveType == WaveType::SPEEDSTERS) {
			m_entManager.AddEntity(std::make_unique<SpeedsterEnemy>(
				Vector2{ 
					100.0f + (i % 5) * 60.0f, 
					-50.0f - (i / 5) * 60.0f 
				}, 
				450.f,
				enemytexPath
			));
		}
	}
}

void Game::DrawMenu() const
{
	ClearBackground(BLACK);

	// Title
	const char* title = "SPACE OPERA";
	const int titleFontSize = GameConfig::ScreenHeight / 12;
	const int titleWidth = MeasureText(title, titleFontSize);

	DrawText(
		title, 
		(GameConfig::ScreenWidth - titleWidth) / 2, 
		GameConfig::ScreenHeight / 8, 
		titleFontSize, 
		DARKBLUE);

	// Menu items
	const int menuFontSize = GameConfig::ScreenHeight / 20;
	const int spacing = menuFontSize * 2;
	const int itemCount = static_cast<int>(m_menuItems.size());
	const int startY = GameConfig::ScreenHeight / 2 - (int(m_menuItems.size()) * spacing) / 2;
	const Vector2 mouse = GetMousePosition();

	for (int i = 0; i < itemCount; ++i) {
		const auto& item = m_menuItems[static_cast<std::size_t>(i)];
		const int textWidth = MeasureText(item.c_str(), menuFontSize);
		const int x = (GameConfig::ScreenWidth - textWidth) / 2;
		const int y = startY + i * spacing;

		// Mouse hover effect
		Rectangle itemRect = { 
			static_cast<float>(x), 
			static_cast<float>(y), 
			static_cast<float>(textWidth), 
			static_cast<float>(menuFontSize) 
		};
		
		Color colour = i == m_selectedMenuItem ? RED : RAYWHITE;

		if (CheckCollisionPointRec(mouse, itemRect)) {
			colour = MAROON;
		}

		DrawText(item.c_str(), x, y, menuFontSize, colour);
	}
}

void Game::UpdateMenu()
{
	const int itemCount = static_cast<int>(m_menuItems.size());

	// Keyboard navigation
	if (IsKeyPressed(KEY_DOWN)) {
		m_selectedMenuItem = (m_selectedMenuItem + 1) % itemCount;
	}
	if (IsKeyPressed(KEY_UP)) {
		m_selectedMenuItem = (m_selectedMenuItem - 1 + itemCount) % itemCount;
	}

	// Mouse hover
	const Vector2 mouse = GetMousePosition();
	const int menuFontSize = GameConfig::ScreenHeight / 20;
	const int spacing = menuFontSize * 2;
	const int startY = GameConfig::ScreenHeight / 2 - (int(m_menuItems.size()) * spacing) / 2;

	bool selected = IsKeyPressed(KEY_ENTER); // Default to keyboard selection

	for (int i = 0; i < itemCount; ++i) {
		const auto& item = m_menuItems[static_cast<std::size_t>(i)];
		const int textWidth = MeasureText(item.c_str(), menuFontSize);
		const int x = (GameConfig::ScreenWidth - textWidth) / 2;
		const int y = startY + int(i) * spacing;


		Rectangle itemRect = { 
			static_cast<float>(x), 
			static_cast<float>(y), 
			static_cast<float>(textWidth), 
			static_cast<float>(menuFontSize) 
		};
		
		if (CheckCollisionPointRec(mouse, itemRect)) {
			m_selectedMenuItem = static_cast<int>(i);

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				// Mouse click = select
				selected = true;
			}
		}
	}

	if (!selected) {
		return; // No selection made
	}

	if (m_selectedMenuItem == 0) {
		Init(); // Reset game state if needed
		m_state = GameState::PLAYING;
	}
	else if (m_selectedMenuItem == 1) {
		m_isShuttingDown = true;
	}
}

void Game::DrawGame() const
{

	//// Draw the parallax background
	//for (int i = 0; i < MAX_BACKGROUND; i++) {
	//	DrawTextureEx(texBackgrounds[i], { 0, m_backgroundOffset[i] - ScreenHeight }, 0, 1.0f, WHITE);
	//	DrawTextureEx(texBackgrounds[i], { 0, m_backgroundOffset[i] }, 0, 1.0f, WHITE);
	//}
	
	m_entManager.DrawAll();

	DrawText(
		TextFormat("SCORE: %02i", m_score), 
		10, 
		10, 
		20, 
		RAYWHITE);
}

void Game::UpdateGame()
{
	//for (int i = 0; i < MAX_BACKGROUND; i++) {
	//	m_backgroundOffset[i] += (i + 1) * 10.0f * dt; // Farther background move slower
	//	if (m_backgroundOffset[i] > ScreenHeight)
	//		m_backgroundOffset[i] -= ScreenHeight;
	//}

	
	
	// Get the player type and use the other update function
	auto player = m_entManager.GetEntityOfType<Player>();
	Vector2 playerPosition = { 0, 0 };

	for (auto* player : player) {
		if (!player->IsActive()) {
			continue;
		}

		player->Update(dt, m_entManager);
		playerPosition = player->GetPosition();

		if (player->GetHealth() <= 0) {
			player->SetActive(false);
			m_state = GameState::GAMEOVER;
		}
	}

	//// Get the shooter enemy and update
	auto shooters = m_entManager.GetEntityOfType<ShooterEnemy>();
	for (auto* shooter : shooters) {
		shooter->Update(dt, m_entManager, playerPosition);

		if (shooter->GetHealth() <= 0 && !shooter->IsActive()) {
			m_score += shooter->GetScoreValue();
		}
	}

	auto speedsters = m_entManager.GetEntityOfType<SpeedsterEnemy>();
	if (!speedsters.empty()) {
		for (auto* speed : speedsters) {
			speed->Update(dt, m_entManager, playerPosition);
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

	if (!m_waveActive && m_state == GameState::PLAYING) {
		m_waveTimer += dt;

		if (m_waveTimer >= m_timeBetweenWaves) {
			const int waveType = 
				std::uniform_int_distribution<int>(0, 2)(m_rng);
			
			if (m_waveNumber == 0)
				m_currentWave = WaveType::SHOOTERS;
			else if (m_waveNumber == 1)
				m_currentWave = WaveType::SPEEDSTERS;
			else
				m_currentWave = WaveType::MIXED;

			SpawnWave();
		}
	}

	// Player colliding with an Enemy
	m_entManager.CheckCollision<Enemy, Player>(
		[](Enemy* e, Player* p) {
			p->TakeDamageOnImpact(1);
		//TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
		});

	// Bullet colliding with an Enemy Type
	m_entManager.CheckCollision<Bullet, Enemy>(
		[](Bullet* b, Enemy* e) {
			if (b->GetFaction() != Bullet::Faction::PLAYER) // Only player bullets hurt enemies
				return;


			b->SetActive(false);
			e->TakeDamage(1);
	});

	// Bullet Colliding with Player
	m_entManager.CheckCollision<Bullet, Player>(
		[](Bullet* b, Player* p) {
			if (b->GetFaction() != Bullet::Faction::ENEMY) // Only enemy bullets hurt player
				return;

			b->SetActive(false);
			p->TakeDamage(1);
		//TraceLog(LOG_INFO, "Player Health: %d", p->GetHealth());
	});

	if (m_state == GameState::PLAYING && 
		(IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))) {
		m_state = GameState::PAUSED;
	}

}

void Game::DrawPauseMenu() const
{
	if (m_state == GameState::PAUSED) {
		const int fontSize = GameConfig::ScreenHeight / 12;
		const char* msg = "PAUSED";
		const int textWidth = MeasureText(msg, fontSize);
		
		DrawRectangle(
			0, 
			GameConfig::ScreenHeight / 2 - fontSize, 
			GameConfig::ScreenWidth, 
			fontSize * 2, 
			Fade(BLACK, 0.5f));
		
		DrawText(msg, 
			(GameConfig::ScreenWidth - textWidth) / 2, 
			GameConfig::ScreenHeight / 2 - fontSize / 2, 
			fontSize, 
			YELLOW);

	}
}

void Game::DrawGameOver() const
{
	const int fontSize = GameConfig::ScreenHeight / 12;
	const int subfontSize = GameConfig::ScreenHeight / 24;
	const char* msg = "GAME OVER";
	const char* subMsg = "The world is to be destoryed!\n Press Space to restart";
	
	const int textWidth = MeasureText(msg, fontSize);
	const int subtextWidth = MeasureText(subMsg, subfontSize);
	
	DrawRectangle(
		0, 
		GameConfig::ScreenHeight / 2 - fontSize + subfontSize, 
		GameConfig::ScreenWidth, 
		fontSize * 2, 
		Fade(RAYWHITE, 0.5f));
	
	DrawText(msg, 
		(GameConfig::ScreenWidth - textWidth) / 2, 
		GameConfig::ScreenHeight / 2 - fontSize / 2, 
		fontSize, 
		RED);

	const int subY = static_cast<int>(
		GameConfig::ScreenHeight / 1.7f - subfontSize / 2.f);
	
	DrawText(subMsg, 
		(GameConfig::ScreenWidth - subtextWidth) / 2, 
		subY, 
		subfontSize,
		RED);
}

void Game::UpdateGameOver()
{
	
}

Game::Game()
{
	InitWindow(GameConfig::ScreenWidth, GameConfig::ScreenHeight, "Space Opera!");
	

	SetExitKey(KEY_NULL); // Disable default exit key to handle it manually
	SetTargetFPS(60);

	Init();
}

void Game::Run()
{
#ifndef __EMSCRIPTEN__
	while (!ShouldClose()) {
		Update();

		if (!ShouldClose()) {
			Draw();
		}	
	}

	FreeResources();
	CloseWindow();
#endif // !__EMSCRIPTEN__
}
