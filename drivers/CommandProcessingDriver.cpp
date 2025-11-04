#include <iostream>
#include "../include/CommandProcessing.h"
#include "../include/GameEngine.h"

void testCommandProcessor() {
    std::cout << "In testCommandProcessor.cpp file..." << std::endl;

    // Create Objects.
    GameEngine engine;
    CommandProcessor commandPro;

    commandPro.getCommand(engine);


    // Destructors called.
    // engine.~GameEngine();
    // commandPro.~CommandProcessor();

}