/**
 * @file CommandProcessingDriver.cpp
 * @brief Test driver for the commands entered by client, for the game.
 * @details This driver tests the Command Processing class to demonstrate:
 *          - The processing of commands entered (loadmap, validatemap, addplayer, gamestart, replay, quit).
 *          - The reading and saving of commands to a vector of Command* objects in the CommandProcessor.
 *          - Validate that the commands are valid in current state of the game.
            - Commands can either be read from the command line (-console), or from file (-file).
 * @author Chhay (A2, P1)
 * @date November 2025
 * @version 1.0
 */

#include <iostream>
#include "../include/CommandProcessing.h"
#include "../include/GameEngine.h"

void testCommandProcessor(int argc, char* argv[]) {
    std::cout << "=== Starting CommandProcessing Test Drivers ===" << std::endl;
    
    GameEngine engine;
    CommandProcessor *commandPro = nullptr;

    if(argc == 2 && std::string(argv[1]) == "-console") {
        std::cout << "\nMode Selected: Console..." << std::endl;
        commandPro = new CommandProcessor();
        commandPro->getCommand(engine);

        std::cout << *commandPro << std::endl;
    } else if (argc == 3 && std::string(argv[1]) == "-file") {
        std::string fileName = argv[2];
        std::cout << "\nMode Selected: File...." << std::endl;

        commandPro = new FileCommandProcessorAdapter(fileName);
        commandPro->getCommand(engine);

        std::cout << *commandPro << std::endl;
    } else {
        std::cout << "\nInvalid command line. Please enter a command line in one of the two formats:\n\n"
                    "   1. Console Mode:    <./executable-file-name> -console\n"
                    "   2. File Mode:       <./executable-file-name> -file <file-name>\n\n"
                    "   Example: ./command -file input.txt" << std::endl;
    }

    // Delete and free up memory.
    delete commandPro;
    commandPro = nullptr;
}



// === A3, PART 2: TOURNAMENT MODE ===

void testTournament() {
    GameEngine engine;
    std::string effectMsg = "Placeholder effect";
    engine.handleTournament("tournament -M   1 -P  2 -G       3 -D  4", effectMsg); // TESTING 
}