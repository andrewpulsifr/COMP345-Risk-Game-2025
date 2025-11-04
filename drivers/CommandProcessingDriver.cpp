#include <iostream>
#include "../include/CommandProcessing.h"
#include "../include/GameEngine.h"

void testCommandProcessor(int argc, char* argv[]) {
    std::cout << "In testCommandProcessor.cpp file..." << std::endl;
    
    GameEngine engine;
    CommandProcessor *commandPro;

    if(argc == 2 && std::string(argv[1]) == "-console") {
        std::cout << "Mode Selected: Console" << std::endl;
        //commandPro = new CommandProcessor();
        //commandPro->getCommand(engine);
    } else if (argc == 3 && std::string(argv[1]) == "-file") {
        std::cout << "Mode Selected: File" << std::endl;
        //commandPro = new FileCommandProcessorAdapter();
    } else {
        std::cout << "\nInvalid command line. Please enter a command line in one of the two formats:\n\n"
                    "   1. Console Mode:    <./executable-file-name> -console\n"
                    "   2. File Mode:       <./executable-file-name> -file <file-name>\n\n"
                    "   Example: ./command -file input.txt" << std::endl;
    }
}