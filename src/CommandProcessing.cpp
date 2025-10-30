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
bool CommandProcessor::validate(std::string& commandEntered) {

}

// readCommand().
void CommandProcessor::readCommand(std::string& lineEntered) {

    // Remove whitespace that may be present at the start and end of the string entered.
    lineEntered.erase(0, lineEntered.find_first_not_of(" \t"));
    lineEntered.erase(lineEntered.find_last_not_of(" \t") + 1);


    // Extract only the command, if a mapname or playername is entered.
    std::string commandEntered = lineEntered.substr(0, lineEntered.find(" "));

    std::cout << "The command entered is: " << commandEntered << std::endl;

    if(commandEntered == "loadmap") {
        std::cout << "Loadmap." << std::endl;
        // saveCommand("loadmap");
    } else if (commandEntered == "validatemap") {
        std::string mapfileEntered = lineEntered.substr(commandEntered.length()+1, lineEntered.length());
        std::cout << "The mapfile entered is: " << mapfileEntered << std::endl;
        
        // std::cout << "In validatemap." << std::endl;
    } else if (commandEntered == "addplayer") {
        std::string playernameEntered = lineEntered.substr(commandEntered.length()+1, lineEntered.length());
        std::cout << "The playername entered is: " << playernameEntered << std::endl;

        // saveCommand("addplayer");
    } else if (commandEntered == "gamestart") {
        std::cout << "Gamestart." << std::endl;
        // saveCommand("gamestart");
    } else if (commandEntered == "replay") {
        std::cout << "Replay." << std::endl;
        // saveCommand("replay");
    } else if (commandEntered == "quit") {
        std::cout << "Exiting Program..." << std::endl;
        exit(1);
    } else {
            std::cout << "The command you entered is not valid." << std::endl;
    }

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
 void CommandProcessor::getCommand() {
    std::string lineEntered;

    std::cout << "Please enter a command (in lowercase): ";
    std::getline(std::cin, lineEntered);
    
    readCommand(lineEntered);
    
    if(validate) {
        saveCommand(lineEntered);
    }
}
