#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"

#include <iostream>
#include <vector>
#include <stdexcept>



// ===== CommandProcessor class =====

// Default constructor for CommandProcessor.
CommandProcessor::CommandProcessor() : commandObjects() {}

// Deep copy constructor for CommmandProcessor.
CommandProcessor::CommandProcessor(const CommandProcessor &obj) {
    for(std::size_t i = 0; i < obj.commandObjects.size(); i++) {
        Command* commandptr = obj.commandObjects.at(i);

        if(commandptr != nullptr) {
            Command* newCopy = new Command(*commandptr);
            commandObjects.push_back(newCopy);

        }
    }
}

// Destructor for CommandProcessor.
CommandProcessor::~CommandProcessor() {
    for(std::size_t i = 0; i < commandObjects.size(); i++) {
        delete commandObjects.at(i);
    }

    commandObjects.clear();
    
    std::cout << "** The CommandProcessor object is destroyed." << std::endl;
}

// validate() if command entered is a valid command.
bool CommandProcessor::validate(GameEngine& engine, Command* cmdptr) {
    std::string commandName = cmdptr->getName();

    //Extract only the command, if a mapname or playername is entered.
    std::string commandOnly = commandName.substr(0, commandName.find(" "));
    
    // If command not valid in current state, save the error. Else, save it as valid.
    if(!engine.isValidCommand(commandOnly)) {
        cmdptr->saveEffect("ERROR: Invalid command '" + commandOnly + "' for current state " + engine.getStateName() + ".");
        return false;
    } else {
        cmdptr->saveEffect("The command '" + commandOnly + "' is valid for the current state " + engine.getStateName() + ".");
        return true;
    }
}

// check to see if there are any typos in the commandEntered. If yes, command will not be saved.
bool CommandProcessor::validCommandSpelling(std::string& commandEntered) {

    if(commandEntered == "loadmap" || commandEntered == "validatemap" || commandEntered == "addplayer" ||
        commandEntered == "gamestart" || commandEntered == "replay" || commandEntered == "quit") {
        return true;
    } else {
        std::cout << "The command you entered, '" << commandEntered << "' is an invalid command." << std::endl;
        return false;
    }
}


// Assignment Operator for CommandProcessor.
 CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if(this != &other) {

        // Delete existing commandObjects.
        for(std::size_t i = 0; i < commandObjects.size(); i++) {
            delete commandObjects.at(i);
        }
        commandObjects.clear();

        // Deep Copy commandObjects from other.
        for(std::size_t i = 0; i < other.commandObjects.size(); i++) {
            if(other.commandObjects.at(i) != nullptr) {
                commandObjects.push_back(new Command(*other.commandObjects.at(i)));
            }
        }

    }
    return *this;
 }

 // Output Operator for CommandProcessor.
std::ostream& operator<<(std::ostream& os, const CommandProcessor& commandPro) {
    std::size_t size = commandPro.commandObjects.size();
    os << "\nCommandProcessor has " << size << " command(s). ";
    
    if(size == 0) {
        os << "\n";
        return os;
    } else {
        os << "They include: " << std::endl;
        for(std::size_t i = 0; i < size; i++) {
            os << "  Index " << i << " - (Command Name): " << commandPro.commandObjects[i]->getName() << ", (Effect): " << commandPro.commandObjects[i]->getEffect() << "\n";
        }
        os << "\n";
        return os;
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
    
    return lineEntered;
}

// saveCommand() and return a pointer to the object.
Command* CommandProcessor::saveCommand(std::string& commandRead) {
    Command* newCommandObj = new Command(commandRead);
    
    this->commandObjects.push_back(newCommandObj);
    
    notify();  // Notify observers when command is saved

    return newCommandObj;
}

/** @brief Generate log string for CommandProcessor */
std::string CommandProcessor::stringToLog() const {
    if (commandObjects.empty()) {
        return "CommandProcessor: No commands saved";
    }
    
    // Log the most recently saved command
    Command* lastCommand = commandObjects.back();
    return "CommandProcessor: Saved command - " + lastCommand->getName();
}

// getCommand() for GameEngine or Player objects to read from command line.
void CommandProcessor::getCommand(GameEngine& engine) {
    std::string lineEntered;

    // Adapted from the GameEngineDriver.cpp.
    while(true) {
        // Prompts user to enter commands, returns the command if it is valid.
        lineEntered = readCommand();

        //Extract only the command, if a mapname or playername is entered.
        std::string commandEntered = lineEntered.substr(0, lineEntered.find(" "));

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
        
        bool isValidCommandSpelling = validCommandSpelling(commandEntered);

        if(isValidCommandSpelling) {
            // Save the full lineEntered(command + mapname/playername if present) and get its pointer.
            Command* cmdptr = saveCommand(lineEntered);

            bool validCommand = validate(engine, cmdptr);

            // If Command is valid, process the command to trigger state transition.
            if(validCommand) {
                engine.processCommand(*cmdptr);
            }
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
    return commandObjects.at(0);
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

         //Extract only the command, if a mapname or playername is entered.
        std::string commandEntered = lineReadFromFile.substr(0, lineReadFromFile.find(" "));

        // Skip empty input and prompt again
        if (lineReadFromFile.empty()) {
            std::cout << "\nThe End of the File is Reached.\n" << std::endl;
            break;
        }

        bool isValidCommandSpelling = validCommandSpelling(commandEntered);

        if(isValidCommandSpelling) {
            // Save the full lineReadFromFile (command + mapname/playername if present) and get its pointer.
            Command* cmdptr = saveCommand(lineReadFromFile);

            bool validCommand = validate(engine, cmdptr);

            // If Command is valid, process the command to trigger state transition.
            if(validCommand) {
                engine.processCommand(*cmdptr);
            }
        }

        // Display current state after command processing
        std::cout << "  Current state: " << engine.getStateName() << std::endl;
        
        // Check if game has reached terminal state
        if (engine.isGameOver()) {
            std::cout << "Game has ended. Type 'quit' to exit or continue testing." << std::endl;
        }

    }
}
