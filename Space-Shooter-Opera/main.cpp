#include "Game.h"

// Include library for the web
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif // __EMSCRIPTEN__

Game* game = nullptr;

#ifdef __EMSCRIPTEN__
void main_loop() {
    if (game) {
        game->Update();
        game->Draw();
        if (game->ShouldClose()) {
            emscripten_cancel_main_loop();
            game->FreeResources();
        }
    }
}
#endif // __EMSCRIPTEN__


int main(void)
{
    game = new Game();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    game->Run();
    delete game;
#endif // __EMSCRIPTEN__

    return 0;
}
