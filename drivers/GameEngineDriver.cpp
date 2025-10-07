#include "../include/GameEngine.h"
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Test function that creates a console-driven interface for game states
 * 
 * @details Creates an interactive console that allows users to navigate through
 * all game states by typing commands. Invalid commands result in error messages
 * without state transitions, as required by the assignment.
 */
void testGameStates() {
    cout << "\n=== Testing Game States ===" << endl;
    
    GameEngine engine;
    engine.displayWelcomeMessage();
    engine.displayGameStatus();
    
    string input;
    cout << "\nEnter command: ";
    
    while (std::getline(cin, input)) {
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        if (input.empty()) {
            cout << "Enter command: ";
            continue;
        }
        
        if (input == "quit" || input == "exit") {
            cout << "Exiting game engine test." << endl;
            break;
        }
        
        if (input == "help") {
            engine.displayGameStatus();
        } else if (input == "status") {
            engine.displayGameStatus();
        } else {
            // Process the actual game command
            engine.processCommand(input);
            
            // Show current state after processing
            cout << "  Current state: " << engine.getStateName() << endl;
            
            // Check if game is over
            if (engine.isGameOver()) {
                cout << "Game has ended. Type 'quit' to exit or continue testing." << endl;
            }
        }
        
        cout << "\nEnter command: ";
    }
    
    cout << "=== Game States Test Complete ===" << endl;
}