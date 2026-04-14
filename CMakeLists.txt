cmake_minimum_required(VERSION 3.16)

project(SpaceShooterOpera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FETCH_RAYLIB
    "Download and build raylib 5.5 automatically via FetchContent when not found on the system"
    ON)

set(GAME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Space-Shooter-Opera")
set(GAME_TARGET "Space-Shooter-Opera")

add_executable(${GAME_TARGET}
    "${GAME_SOURCE_DIR}/main.cpp"
    "${GAME_SOURCE_DIR}/Game.cpp"
    "${GAME_SOURCE_DIR}/Player.cpp"
    "${GAME_SOURCE_DIR}/Enemy.cpp"
    "${GAME_SOURCE_DIR}/ShooterEnemy.cpp"
    "${GAME_SOURCE_DIR}/SpeedsterEnemy.cpp"
    "${GAME_SOURCE_DIR}/Bullet.cpp"
)

target_include_directories(${GAME_TARGET} PRIVATE "${GAME_SOURCE_DIR}")

set(RAYLIB_TARGET "")
find_package(raylib QUIET)

if(TARGET raylib)
    set(RAYLIB_TARGET raylib)
elseif(TARGET raylib::raylib)
    set(RAYLIB_TARGET raylib::raylib)
endif()

if(NOT RAYLIB_TARGET)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(RAYLIB QUIET IMPORTED_TARGET raylib)
        if(TARGET PkgConfig::RAYLIB)
            set(RAYLIB_TARGET PkgConfig::RAYLIB)
        endif()
    endif()
endif()

if(NOT RAYLIB_TARGET AND WIN32 AND DEFINED ENV{RAYLIB_ROOT})
    add_library(raylib_manual STATIC IMPORTED)
    set_target_properties(raylib_manual PROPERTIES
        IMPORTED_LOCATION "$ENV{RAYLIB_ROOT}/lib/raylib.lib"
        INTERFACE_INCLUDE_DIRECTORIES "$ENV{RAYLIB_ROOT}/include"
    )
    set(RAYLIB_TARGET raylib_manual)
endif()

if(NOT RAYLIB_TARGET)
    if(FETCH_RAYLIB)
        message(STATUS "raylib not found on system — fetching raylib 5.5 via FetchContent")
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG        5.5
            GIT_SHALLOW    TRUE
        )
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(raylib)
        set(RAYLIB_TARGET raylib)
    else()
        message(FATAL_ERROR
            "raylib was not found. Options:\n"
            "  1. Install raylib and make sure CMake can find it.\n"
            "  2. On Windows, set the RAYLIB_ROOT environment variable.\n"
            "  3. Let CMake download it automatically: cmake -DFETCH_RAYLIB=ON .")
    endif()
endif()

target_link_libraries(${GAME_TARGET} PRIVATE ${RAYLIB_TARGET})

if(WIN32)
    target_link_libraries(${GAME_TARGET} PRIVATE winmm)
endif()

add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${GAME_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${GAME_TARGET}>/assets"
)
