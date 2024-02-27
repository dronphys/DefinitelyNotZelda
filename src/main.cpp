#include <iostream>
#include "Game.h"
#include "GameEngine.h"
#include "Components.h"
#include "Profiler.h"

int main()
{
    PROFILE_SCOPE("main");
    GameEngine g( MY_PROJECT_PATH"/config/assets.txt");
    g.run();

    
    return 0;
}