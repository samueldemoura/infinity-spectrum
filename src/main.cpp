#include <Engine.h>

int main(int argc, char *argv[])
{
    Engine gameEngine;
    if (gameEngine.Initialize(argc, argv))
    	gameEngine.GameLoop();

    gameEngine.Shutdown();
    return 0;
}
