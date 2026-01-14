#include "Game.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Game* game;
    game = new Game();

    game->Run();

    delete game;

    return 0;
}
