/**
 * @file TournamentDriver.cpp
 * @brief Test driver for the tournament command entered by user for the game.
 * @details This driver tests the 'tournament' command, a part of the CommandProcessor class to demonstrate:
 *              - 1. The 'tournament' command can be processed and validated in the CommandProcessor.
                - 2. The 'tournament' command can be executed in the GameEngine.
 * @author Chhay, and 
 * @date November 2025
 * @version 1.0
 */

#include <iostream>
#include "../include/CommandProcessing.h"
#include "../include/GameEngine.h"

// === A3, PART 2: TOURNAMENT MODE ===
void testTournament(int argc, char* argv[]) {
    std::cout << "=== Starting Tournament Mode Test Drivers ===" << std::endl;
    
    GameEngine engine;
    CommandProcessor *commandPro = nullptr;
    
    try {
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
            throw std::invalid_argument("");
        }
    } catch (const std::invalid_argument& invalidErr) {
        std::cerr << "\nERROR: Invalid command line. Please enter a command line in one of the two formats:\n\n"
                    "   1. Console Mode:    <./executable-file-name> -console\n"
                    "   2. File Mode:       <./executable-file-name> -file <file-name>\n\n"
                    "   Example: ./tournament -file test.txt" << std::endl;
    }

    // Delete and free up memory.
    delete commandPro;
    commandPro = nullptr;

    std::cout << "=== End of Tournament Mode Test Drivers ===" << std::endl;
}