/**
 * @file GameEngineDriver.cpp
 * @brief Test driver for the GameEngine state machine functionality and for the Game Startup Phase.
 * @details This driver tests the GameEngine class to demonstrate:
 *          - Game state transitions and command processing
 *          - Interactive console interface for state navigation
 *          - Command validation and error handling
 *          - State machine pattern implementation
 *          - User input processing and game flow control
 * @author Andrew Pulsifer and Matteo  (A1, P5), Chhay (A2, P2)
 * @date November 2025
 * @version 2.0
 */

#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"
#include "../include/Cards.h"
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
        if (input == GameCommands::QUIT || input == "exit") {
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


// === A2, PART 2: Game Startup Phase ===
void testStartupPhase(int argc, char* argv[]) {
    cout << "\n=== Testing Startup Phase ===\n" << endl;
    cout << "Start the Game.." << endl;
    
    GameEngine engine;
    CommandProcessor *commandPro = nullptr;

    // Add cards to the deck so that cards can be drawn when 'gamestart' command is used.
    engine.getDeck()->addCard(new Card(Card::Reinforcement));
    engine.getDeck()->addCard(new Card(Card::Reinforcement));
    engine.getDeck()->addCard(new Card(Card::Bomb));
    engine.getDeck()->addCard(new Card(Card::Bomb));
    engine.getDeck()->addCard(new Card(Card::Blockade));
    engine.getDeck()->addCard(new Card(Card::Blockade));
    engine.getDeck()->addCard(new Card(Card::Diplomacy));
    engine.getDeck()->addCard(new Card(Card::Diplomacy));
    engine.getDeck()->addCard(new Card(Card::Airlift));
    engine.getDeck()->addCard(new Card(Card::Airlift));



    // Implement the CommandProcessor or its Adapter, depending on whether the user chooses to use the console or read from a file.
    if(argc == 2 && std::string(argv[1]) == "-console") {
        std::cout << "\nMode Selected: Console..." << std::endl;
        commandPro = new CommandProcessor();

        // After initializing the commandProcessor, the startupPhase() method will handle the commands entered/read.
        engine.startupPhase(engine, *commandPro);
    
    } else if (argc == 3 && std::string(argv[1]) == "-file") {
        std::string fileName = argv[2];
        std::cout << "\nMode Selected: File...." << std::endl;
        
        commandPro = new FileCommandProcessorAdapter(fileName);
        engine.startupPhase(engine, *commandPro);
    } else {
        // Invalid arguments: commandPro remains nullptr (no initialization needed)
        std::cout << "\nInvalid command line. Please enter a command line in one of the two formats:\n\n"
                    "   1. Console Mode:    <./executable-file-name> -console\n"
                    "   2. File Mode:       <./executable-file-name> -file <file-name>\n\n"
                    "   Example: ./gamestart -file input.txt" << std::endl;
        // Early return to make control flow explicit --> no cleanup needed when commandPro is nullptr
        cout << "\n=== Testing Startup Phase Completed ===" << endl;
        return;
    }

    // Delete and free up memory (engine will be automatically destroyed as its statically allocated).
    // Note: At this point, commandPro is guaranteed to be initialized (either from console or file mode)
    delete commandPro;
    commandPro = nullptr;

    cout << "\n=== Testing Startup Phase Completed ===" << endl;
}