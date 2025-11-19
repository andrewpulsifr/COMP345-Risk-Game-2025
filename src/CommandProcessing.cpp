/**
 * @file CommandProcessing.cpp
 * @brief Implementation of Assignment 2 - Part 1: Command Processor and Command Adapter.
 * @author Chhay (A2, P1)
 * @date November 2025
 * @version 1.0
 * 
 * This file contains the implementation of the command processor and command adapter that processes
 * the command that is entered either through -console or read through -file. It works with the GameEngine to
 * process the states of the game. The command adapter follows the Adapter Pattern.
 */
 
#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream> // To import istringstream to continuously from a string.



// ===== CommandProcessor class =====

/**
 * @brief Default constructor for CommandProcessor.
 */
CommandProcessor::CommandProcessor() : commandObjects() {}

/**
 * @brief Deep copy constructor for CommmandProcessor.
 * @param obj, CommandProcessor object that is being copied.
 */
CommandProcessor::CommandProcessor(const CommandProcessor &obj) {
    for(std::size_t i = 0; i < obj.commandObjects.size(); i++) {
        Command* commandptr = obj.commandObjects.at(i);

        if(commandptr != nullptr) {
            Command* newCopy = new Command(*commandptr);
            commandObjects.push_back(newCopy);

        }
    }
}

/**
 * @brief Destructor for CommandProcessor.
 */
CommandProcessor::~CommandProcessor() {
    for(std::size_t i = 0; i < commandObjects.size(); i++) {
        delete commandObjects.at(i);
    }

    commandObjects.clear();
    
    // std::cout << "** The CommandProcessor object is destroyed." << std::endl;
}

/**
 * @brief Validate and check if the command entered is a valid command.
 * @param engine, passed to check if command is valid and get the current state.
 * @param cmd, the Command that is being validated.
 * @return bool if the command is valid.
 */
bool CommandProcessor::validate(GameEngine& engine, Command& cmd) {
    std::string commandName = cmd.getName();

    //Extract only the command, if a mapname or playername is entered.
    std::string commandOnly = commandName.substr(0, commandName.find(" "));
    
    // If command not valid in current state, save the error.
    // NOTE: For valid commands, the effect will be set by GameEngine::processCommand()
    // with a descriptive message about what actually happened.
    if(!engine.isValidCommand(commandOnly)) {
        cmd.saveEffect("ERROR: Invalid command '" + commandOnly + "' for current state " + engine.getStateName() + ".");
        std::cout << cmd.getEffect() << std::endl;
        return false;
    } else {
        cmd.saveEffect("The command '" + commandOnly + "' is valid for the current state " + engine.getStateName() + ".");
        std::cout << cmd.getEffect() << std::endl;
        return true;
    }
}

/**
 * @brief Assignment Operator for CommandProcessor.
 * @param other, CommandProcessor object that is being assigned to current object.
 * @return a reference to the new CommandProcessor object.
 */
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

 /**
 * @brief Output Operator for CommandProcessor.
 */
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

 /**
 * @brief readCommand(): Prompts the user to enter a command.
 * @return a string value that is a command.
 */
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

/**
 * @brief saveCommand(): save Command as a Command object in a (private) vector
 * of Command objects.
 * @return a Command pointer to the object added to the vector.
 */
Command* CommandProcessor::saveCommand(std::string& commandRead) {
    Command* newCommandObj = new Command(commandRead);
    
    // Propagate observers to child Command so its saveEffect() notifications are observed
    this->propagateObserversTo(newCommandObj);
    
    this->commandObjects.push_back(newCommandObj);
    
    notify();  // Notify observers when command is saved

    return newCommandObj;
}

/**
 * @brief Generate log string for CommandProcessor
 */
std::string CommandProcessor::stringToLog() const {
    if (commandObjects.empty()) {
        return "CommandProcessor: No commands saved";
    }
    
    // Log the most recently saved command
    Command* lastCommand = commandObjects.back();
    return "CommandProcessor: Saved command - " + lastCommand->getName();
}


// ===== A3, Part 2: Tournament Mode =====

/** @brief A Helper Function, that clean the whitespace of commands to extract only the string entered.
 * @param the command entered.
 * @return the cleaned string.
 */
std::string CommandProcessor::cleanWhiteSpace(const std::string& command) {
    size_t start = command.find_first_not_of(" \t\r\n");
    size_t end = command.find_last_not_of(" \t\r\n");

    std::string cleanedCommand = command.substr(start, end - start + 1);

    return cleanedCommand;
}

/** 
 * @brief To validate the parameters entered in the line of tournament command.
 * @return A vector of int values, that corresponds to the value of -M, -P, -G, -D.
 */
std::vector<int> CommandProcessor::validateTournament(const std::string& command) {
    std::cout << "  -> Validating Tournament...\n" << std::endl;
    
    // Temporary vector to store and return the validate values.
    std::vector<int> tournamentValues;

    // Temp variables and vector for extracting and storing map and player names in the while loop below.
    std::string nameExtracted; // temp string used in while loop below to store each map/player name in the vector.
    std::vector<std::string> mapNames;
    std::vector<std::string> playerNames;


    // Get the position index of each parameter, used to extract their respective values.
    std::size_t listOfMapsIndex = command.find("-M");
    std::size_t listOfPlayersIndex = command.find("-P");
    std::size_t numOfGamesIndex = command.find("-G");
    std::size_t maxNumOfTurnsIndex = command.find("-D");

    // Throw an exception if the parameters or their values are not found.
    if(listOfMapsIndex == std::string::npos || listOfPlayersIndex == std::string::npos || numOfGamesIndex == std::string::npos || maxNumOfTurnsIndex == std::string::npos) {
        throw std::invalid_argument("");
    }




    // Extract the list of Maps, entered after -M and before -P, excluding the letters themselves.
    std::string listOfMapsStr = command.substr(listOfMapsIndex + 2, listOfPlayersIndex - listOfMapsIndex - 2);
    listOfMapsStr = cleanWhiteSpace(listOfMapsStr);

    // Process the Maps by validating and counting the number of maps entered.
    
        // Use istringstream to continuously read the list of maps entered and store it in a temp vector (called mapNames);
        std::istringstream iss(listOfMapsStr);

        while(iss >> nameExtracted) {
            mapNames.push_back(nameExtracted);
        }

        // reset istringstream to be reused in the player validation below.
        iss.clear();

    std::size_t numOfMaps = mapNames.size(); // get size of the vector of map names.

    // Check if it is a valid value. If the value is invalid, an exception is thrown and should be caught in the main command loop that asks user to re-enter a command.
    if(numOfMaps < 1 || numOfMaps > 5)
        throw std::out_of_range("The number of Map(s) entered is invalid. Please re-enter the tournament command, with a -M value between 1 - 5.");

    std::cout << "    The Number of Maps is valid, with a value of: " << numOfMaps << std::endl;
    tournamentValues.push_back(numOfMaps);

    
    

    // Extract the number of Players.
    std::string listOfPlayersStr = command.substr(listOfPlayersIndex + 2, numOfGamesIndex - listOfPlayersIndex - 2);
    listOfPlayersStr = cleanWhiteSpace(listOfPlayersStr);

    // Process the Maps by validating and counting the number of maps entered.
    
        // Reuse istringstream declared in map above to read the list of players entered and store it in a temp vector (called playerNames);
        iss.str(listOfPlayersStr);

        while(iss >> nameExtracted) {
            playerNames.push_back(nameExtracted);
        }

    std::size_t numOfPlayers = playerNames.size(); // get size of the vector of player names.

    if(numOfPlayers < 2 || numOfPlayers > 4)
        throw std::out_of_range("The number of Player(s) entered is invalid. Please re-enter the tournament command, with a -P value between 2 - 4.");
    
    std::cout << "    The Number of Players is valid, with a value of: " << numOfPlayers << std::endl;
    tournamentValues.push_back(numOfPlayers);





    // Extract the number of Games.
    std::string numOfGamesStr = command.substr(numOfGamesIndex + 2, maxNumOfTurnsIndex - numOfGamesIndex - 2);
    numOfGamesStr = cleanWhiteSpace(numOfGamesStr);

    int numOfGames = std::stoi(numOfGamesStr); // convert from string to int value.    

    if(numOfGames < 1 || numOfGames > 5)
            throw std::out_of_range("The number of Game(s) entered is invalid. Please re-enter the tournament command, with a -G value between 1 and 5.");

    std::cout << "    The Number of Games is valid, with a value of: " << numOfGames << std::endl;
    tournamentValues.push_back(numOfGames);


    // Extract the max number of Turns.
    std::string maxNumOfTurnsStr = command.substr(maxNumOfTurnsIndex + 2);
    maxNumOfTurnsStr = cleanWhiteSpace(maxNumOfTurnsStr);

    int maxNumOfTurns = std::stoi(maxNumOfTurnsStr); // convert from string to int value.

    if(maxNumOfTurns < 10 || maxNumOfTurns > 50)
        throw std::out_of_range("The number of maximum turn(s) entered is invalid. Please re-enter the tournament command, with a -D value between 10 and 50.");

    std::cout << "    The Max Number of Turns is valid, with a value of: " << maxNumOfTurns << std::endl;
    tournamentValues.push_back(maxNumOfTurns);

        
    std::cout << "\n    The Tournament Command is valid!\n" << std::endl;

    return tournamentValues;
}


/**
 * @brief getCommand(): for GameEngine or Player objects to read and process commands from the command line.
 * @param engine, passed to process command and to display game status.
 */
void CommandProcessor::getCommand(GameEngine& engine) {
    using namespace GameCommands;
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
        if (commandEntered == QUIT || commandEntered == "exit") {
            std::cout << "Exiting game engine test." << std::endl;
            break;
        }
        
        // Handle informational commands that don't change game state
        if (commandEntered == "help") {
            engine.displayGameStatus();
        } else if (commandEntered == "status") {
            engine.displayGameStatus();
        }

        // Save the full lineEntered(command + mapname/playername if present) and get its pointer.
        Command* cmdptr = saveCommand(lineEntered);

        // Check to see if the comand is valid in the current state.
        bool validCommand = validate(engine, *cmdptr);

        // A3, P2 - ADDITIONAL LAYER OF CHECKING FOR TOURNAMENT COMMAND:
        // See if the command is a valid tournament command line. If a value of the parameter (-M, -P, -G, -D) is invalid, an exception will be thrown.
        // And the bool validCommand will be set to false, error message is printed, and command will not be processed.
        if(commandEntered == "tournament" && validCommand) {
            try {
                validateTournament(lineEntered);
            } catch(const std::out_of_range& valueErr) { // Catch out-of-range errors in the parameters of 'tournament'.
                std::cout << "ERROR 1: " << valueErr.what() << std::endl;
                validCommand = false;
            } catch(const std::invalid_argument& invalidErr) { // Catch invalid 'tournament' without enough parameters/ whitespace.
                std::cout << "ERROR 2: Please enter a valid 'tournament' command in the following format:\n" <<
                             "       tournament -M <listofmapfiles> -P <listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>" << std::endl;
                validCommand = false;
            }
        }

        // If Command is valid, process the command to trigger state transition.
        if(validCommand) {
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


// ===== FileCommandProcessorAdapter class =====

/**
 * @brief Default constructor of the FileCommandProcessorAdapter class.
 * @param fileName, a private variable of the FileCommandProcessorAdapter class.
 */
FileCommandProcessorAdapter::FileCommandProcessorAdapter(std::string fileName) {
    file.open(fileName);
    
    // if file doesn't open, throw an error.
    if(!file.is_open()) {
        throw std::runtime_error("Error: The file name you entered (" + fileName + ") cannot be opened.\n"); 
    }

    std::cout << "The file '" + fileName + "' is opened successfully!\n" << std::endl;
}

/**
 * @brief Destructor of the CommandProcessorAdapter.
 * @param fileName, a private variable of the FileCommandProcessorAdapter class.
 */
FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    
    // If file stream not closed, close it.
    if(file.is_open()) {
        file.close();
    }

    // std::cout << "** The FileCommandProcessorAdapter object is destroyed." << std::endl;
}

/**
 * @brief readCommand(): the implementation of the virtual function inherited from the CommandProcessing class.
 * this method reads a line from a text file.
 * @return a line read from the file.
 */
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

/**
 * @brief getCommand(): the implementation of another virtual function inherited from the CommandProcessing class.
 * this method continuously reads from the file until the end of the file is reached.
 */
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

        // Save the full lineReadFromFile (command + mapname/playername if present) and get its pointer.
        Command* cmdptr = saveCommand(lineReadFromFile);

        bool validCommand = validate(engine, *cmdptr);

        // If Command is valid, process the command to trigger state transition.
        if(validCommand) {
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
