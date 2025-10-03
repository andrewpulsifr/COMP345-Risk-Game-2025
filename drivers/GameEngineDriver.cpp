#include <iostream>
#include <string>
#include "../include/GameEngine.h"

// Free function required by the assignment.
void testGameStates() {
    GameEngine engine;

    std::cout << "=== Game Engine State Machine ===\n";
    std::cout << "Current state: " << engine.stateName() << "\n";
    std::cout << "Type commands from the diagram (e.g., loadmap, validatemap, addplayer,\n"
                 "assigncountries, issueorder, endissueorders, execorder, endexecorders, win,\n"
                 "play, end). Type 'quit' to exit.\n";

    std::string cmd;
    while (true) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) break;
        if (cmd == "quit") break;
        engine.handleCommand(Command{cmd});
    }

    std::cout << "Exiting testGameStates().\n";
}

// If you want this file to run standalone, uncomment the main below.
// Otherwise, keep only testGameStates() and call it from your MainDriver.
/*
int main() {
    testGameStates();
    return 0;
}
*/
