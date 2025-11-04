#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"

#include <iostream>
//#include <string>
#include <vector>

// Default constructor for CommandProcessor.
CommandProcessor::CommandProcessor() : commandObjects() {}

// Deep copy constructor for CommmandProcessor.
CommandProcessor::CommandProcessor(const CommandProcessor &obj) {
    for(int i = 0; i < obj.commandObjects.size(); i++) {
        Command* commandptr = obj.commandObjects.at(i);

        if(commandptr != nullptr) {
            Command* newCopy = new Command(commandptr->getName());
            this->commandObjects.push_back(newCopy);
        }
    }
}

// Destructor for CommandProcessor.
CommandProcessor::~CommandProcessor() {
    for(int i = 0; i < commandObjects.size(); i++) {
        delete commandObjects.at(i);
    }

    commandObjects.clear();
}

// validate()
bool CommandProcessor::validate(GameEngine& engine, std::string& commandEntered) {
    return engine.processCommand(commandEntered);
}

// readCommand().
std::string CommandProcessor::readCommand() {
    std::string lineEntered;
    
    std::cout << "\nEnter command: ";
    
    // Prompt user to input command.
    std::getline(std::cin, lineEntered);

    // Remove whitespace that may be present at the start and end of the string entered.
    lineEntered.erase(0, lineEntered.find_first_not_of(" \t"));
    lineEntered.erase(lineEntered.find_last_not_of(" \t") + 1);

    //Extract only the command, if a mapname or playername is entered.
    std::string commandEntered = lineEntered.substr(0, lineEntered.find(" "));
    
    return commandEntered;
}



// saveEffect()
void CommandProcessor::saveEffect(Command* cmdObject, std::string& effect) {
    cmdObject->setEffect(effect);
    std::cout << "The effect (" << effect << ") of Command: " << cmdObject->getName() << " is saved." << std::endl;
}

// saveCommand()
void CommandProcessor::saveCommand(std::string& commandRead) {
    Command* newCommandObj = new Command(commandRead);
    
    this->commandObjects.push_back(newCommandObj);
}

// getCommand() for GameEngine or Player objects to read from command line.
 void CommandProcessor::getCommand(GameEngine& engine) {
    std::string commandEntered;

    // Adapted from the GameEngineDriver.cpp.
    while(true) {
        // Prompts user to enter commands, returns the command if it is valid.
        commandEntered = readCommand();

        // Skip empty input and prompt again
        if (commandEntered.empty()) {
            std::cout << "Enter command: ";
            continue;
        }

        // Handle quit/exit commands to terminate the test
        if (commandEntered == "quit" || commandEntered == "exit") {
            std::cout << "Exiting game engine test." << std::endl;
            break;
        }

        // Handle informational commands that don't change game state
        if (commandEntered == "help") {
            engine.displayGameStatus();
        } else if (commandEntered == "status") {
            engine.displayGameStatus();
        } else {
            // Process actual game commands that trigger state transitions, store validity of command in 'validated'.
            bool validated = engine.processCommand(commandEntered);

            // Display current state after command processing
            std::cout << "  Current state: " << engine.getStateName() << std::endl;
            
            // Save the command in a collection of Command objects, if the command is valid.
            // if(validated) {
                saveCommand(commandEntered);
            // }
            
            // Check if game has reached terminal state
            if (engine.isGameOver()) {
                std::cout << "Game has ended. Type 'quit' to exit or continue testing." << std::endl;
            }
        }
    

    }

    // print out and see what commandObjects are stored.
    for(int i = 0; i < commandObjects.size(); i++) {
        std::cout << "commandObject[" << i << "]: " << commandObjects[i]->getName() << ", get effect: " << commandObjects[i]->getEffect() << std::endl;
    }
}
