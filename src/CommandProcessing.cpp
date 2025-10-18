#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"

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

// readCommand().
void CommandProcessor::readCommand() {
    std::string commandEntered;

    std::cin >> commandEntered;
    std::cout << "The command entered is: " << commandEntered << std::endl;
}

// saveCommand()
void CommandProcessor::saveCommand(std::string commandRead) {
    Command* newCommandObj = new Command(commandRead);
    
    this->commandObjects.push_back(newCommandObj);
}

// getCommmand()
// Command* CommandProcessor::getCommand() {
// }

// Getter for the Command object vector.
std::vector<Command*> CommandProcessor::getCommandObjects() {
    return commandObjects;
}

