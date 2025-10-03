#include "GameEngine.h"
#include <iostream>
#include <string>

void testGameStates() {
    GameEngine engine;
    std::string command;

    std::cout << "Game Engine State Machine\n";
    std::cout << "Type commands (as per the diagram). Type \"quit\" to exit.\n";

    while (true) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "quit") {
            std::cout << "Exiting game...\n";
            break;
        }

        engine.handleCommand(command);
    }
}

int main() {
    testGameStates();
    return 0;
}
