#include "MainState.hpp"

#include <string>

int main()
{
    TGE::Game::create("Noise Runner", true, 1280, 720);

    TGE::Game::getInstance()->getWindow()->setVerticalSyncEnabled(true);

    // Create the state you made, pass the games state manager as an argument
    MainState::create(TGE::Game::getInstance()->getStateManager());

    // Start the game with the passed state (as a string)
    TGE::Game::getInstance()->init("MainState");
    return 0;
}
