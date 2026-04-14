# Space Shooter Opera

An arcade-style 2D space shooter built with **C++17** and **raylib**.

The project currently includes:
- Player movement and shooting
- Shooter and speedster enemy behaviors
- Bullet collision handling (player/enemy)
- Randomized space backgrounds
- Basic entity management using polymorphism

## Tech Stack

- C++17
- [raylib 5.5](https://www.raylib.com/)
- Visual Studio 2022 solution/project files (`.sln` / `.vcxproj`)

## Controls

- `W` / `A` / `S` / `D` — Move player
- `Space` — Shoot
- `Esc` / window close — Exit game

## Project Structure

```text
Space-Shooter-Opera/
├── Space-Shooter-Opera.sln
├── README.md
└── Space-Shooter-Opera/
    ├── main.cpp
    ├── Game.h / Game.cpp
    ├── Entity.h / EntityManager.h
    ├── Player.h / Player.cpp
    ├── Enemy.h / Enemy.cpp
    ├── ShooterEnemy.h / ShooterEnemy.cpp
    ├── SpeedsterEnemy.h / SpeedsterEnemy.cpp
    ├── Bullet.h / Bullet.cpp
    └── assets/
        ├── background/
        └── player/
```

## Build and Run (Windows + Visual Studio)

1. Install **Visual Studio 2022** with C++ desktop workload.
2. Install **raylib 5.5** for MSVC.
3. Open `Space-Shooter-Opera.sln` in Visual Studio.
4. Ensure the raylib include/lib directories in  
   `Space-Shooter-Opera/Space-Shooter-Opera.vcxproj` match your local install:
   - `AdditionalIncludeDirectories`
   - `AdditionalLibraryDirectories`
5. Build and run the `Space-Shooter-Opera` project.

> Note: The current `.vcxproj` references raylib with a local Windows path (`C:\raylib-5.5_win64_msvc16\...`). Update it if your installation path differs.

## Current State

This is an in-progress prototype focused on gameplay systems and entity behavior. Additional polish (UI, progression, balancing, and content) can be built on top of the current foundation.
