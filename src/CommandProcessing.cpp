#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"

#include <iostream>
#include <vector>



// ===== CommandProcessor class =====

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
    
    std::cout << "** The CommandProcessor object is destroyed." << std::endl;
}

// validate() if command entered is a valid command.
bool CommandProcessor::validate(std::string& lineEntered) {

    //Extract only the command (first word), if a mapname or playername is entered.
    std::string commandEntered = lineEntered.substr(0, lineEntered.find(" "));

    if(commandEntered == "loadmap" || commandEntered == "validatemap" || commandEntered == "addplayer" ||
        commandEntered == "gamestart" || commandEntered == "replay" || commandEntered == "quit") {
        return true;
    } else {
        std::cout << "The command you entered, '" << commandEntered << "' is an invalid command." << std::endl;
        return false;
    }
}

// readCommand().
std::string CommandProcessor::readCommand() {
    std::string lineEntered;
    
    std::cout << "\nEnter command: ";
    
    // Prompt user to input command.
    std::getline(std::cin, lineEntered);

    // Remove whitespace that may be present at the start and end of the string entered.
    if(!lineEntered.empty()) {
        lineEntered.erase(0, lineEntered.find_first_not_of(" \t"));
        lineEntered.erase(lineEntered.find_last_not_of(" \t") + 1);
    }

    //Extract only the command, if a mapname or playername is entered.
    std::string commandEntered = lineEntered.substr(0, lineEntered.find(" "));
    
    return commandEntered;
}

// saveCommand() and return a pointer to the object.
Command* CommandProcessor::saveCommand(std::string& commandRead) {
    Command* newCommandObj = new Command(commandRead);
    
    this->commandObjects.push_back(newCommandObj);

    return newCommandObj;
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
        }
        
    
        if(validate(commandEntered)) {
            // Save Command and get its pointer.
            Command* cmdptr = saveCommand(commandEntered);

            // Process actual game commands that trigger state transitions.
            engine.processCommand(*cmdptr);
        }

        // Display current state after command processing
        std::cout << "  Current state: " << engine.getStateName() << std::endl;
        
        // Check if game has reached terminal state
        if (engine.isGameOver()) {
            std::cout << "Game has ended. Type 'quit' to exit or continue testing." << std::endl;
        }

    }
}

// Get pointer to the first element of the vector of Command Objects.
Command* CommandProcessor::getCommandObjects() {
    return commandObjects[0];
}

void CommandProcessor::printCommandObjects() {
    std::cout << "List of Command Names and Effects: " << std::endl;
    for(int i = 0; i < commandObjects.size(); i++) {
        std::cout << "  Index " << i << " - Name: " << commandObjects[i]->getName() << ", Effect: " << commandObjects[i]->getEffect() << std::endl;
    }
    std::cout << "\n";
}



// ===== FileCommandProcessorAdapter class =====

//Default Constructor.
FileCommandProcessorAdapter::FileCommandProcessorAdapter(std::string fileName) {
    file.open(fileName);
    
    // if file doesn't open, throw an error.
    if(!file.is_open()) {
        throw std::runtime_error("Error: The file name you entered (" + fileName + ") cannot be opened.\n"); 
    }

    std::cout << "The file '" + fileName + "' is opened successfully!\n" << std::endl;
}

//Destructor for the CommandProcessorAdapter.
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    
    // If file stream not closed, close it.
    if(file.is_open()) {
        file.close();
    }

    std::cout << "** The FileCommandProcessorAdapter object is destroyed." << std::endl;
}


//Implementation of the virtual function from CommandProcessing in the Adapter class.
std::string FileCommandProcessorAdapter::readCommand() {
    std::string lineReadFromFile;
    

    if(std::getline(file, lineReadFromFile)) {
        
        // Remove whitespace that may be present at the start and end of the string read.
        lineReadFromFile.erase(0, lineReadFromFile.find_first_not_of(" \t"));
        lineReadFromFile.erase(lineReadFromFile.find_last_not_of(" \t") + 1);
        
        return lineReadFromFile;

    } else {
        return "";
    }
    
}

void FileCommandProcessorAdapter::getCommand(GameEngine& engine) {
    std::string lineReadFromFile;

    while(true) {
        // Read a line from the file.
        lineReadFromFile = readCommand();

        // Skip empty input and prompt again
        if (lineReadFromFile.empty()) {
            std::cout << "\nThe End of the File is Reached.\n" << std::endl;
            break;
        }

        // If the command entered is valid, save command and trigger state transition.
        if(validate(lineReadFromFile)) {
            // Save Command and get its pointer.
            Command* cmdptr = saveCommand(lineReadFromFile);

            // Process actual game commands that trigger state transitions.
            engine.processCommand(*cmdptr);
        }

        // Display current state after command processing
        std::cout << "  Current state: " << engine.getStateName() << std::endl;
        
        // Check if game has reached terminal state
        if (engine.isGameOver()) {
            std::cout << "Game has ended. Type 'quit' to exit or continue testing." << std::endl;
        }

    }
}
