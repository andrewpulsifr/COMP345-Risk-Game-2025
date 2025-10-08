/**
 * @file GameEngineDriver.cpp
 * @brief Test driver for the GameEngine state machine functionality
 * @details This driver tests the GameEngine class to demonstrate:
 *          - Game state transitions and command processing
 *          - Interactive console interface for state navigation
 *          - Command validation and error handling
 *          - State machine pattern implementation
 *          - User input processing and game flow control
 * @author Andrew Pulsifer and Matteo 
 * @date October 2025
 * @version 1.0
 */

#include "../include/GameEngine.h"
#include <iostream>
#include <string>

// Importing only the neccessary std functions.
using std::cout;
using std::cin;
using std::endl;
using std::string;

/**
 * @brief Test function that creates a console-driven interface for game states
 * 
 * @details Creates an interactive console that allows users to navigate through
 * all game states by typing commands. Invalid commands result in error messages
 * without state transitions, as required by the assignment.
 */
void testGameStates() {
    cout << "\n=== Testing Game States ===" << endl;
    
    // Create GameEngine instance and display initial welcome message
    GameEngine engine;
    engine.displayWelcomeMessage();
    engine.displayGameStatus();
    
    // Initialize input processing for interactive state transitions
    string input;
    cout << "\nEnter command: ";
    
    // Main loop for processing user commands and managing state transitions
    while (std::getline(cin, input)) {
        // Clean up input string by removing leading/trailing whitespace
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        // Skip empty input and prompt again
        if (input.empty()) {
            cout << "Enter command: ";
            continue;
        }
        
        // Handle quit/exit commands to terminate the test
        if (input == "quit" || input == "exit") {
            cout << "Exiting game engine test." << endl;
            break;
        }
        
        // Handle informational commands that don't change game state
        if (input == "help") {
            engine.displayGameStatus();
        } else if (input == "status") {
            engine.displayGameStatus();
        } else {
            // Process actual game commands that trigger state transitions
            engine.processCommand(input);
            
            // Display current state after command processing
            cout << "  Current state: " << engine.getStateName() << endl;
            
            // Check if game has reached terminal state
            if (engine.isGameOver()) {
                cout << "Game has ended. Type 'quit' to exit or continue testing." << endl;
            }
        }
        
        cout << "\nEnter command: ";
    }
    
    cout << "=== Game States Test Complete ===" << endl;
}