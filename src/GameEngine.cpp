/**
 * @file GameEngine.cpp
 * @brief Implementation of Assignment 1 – Part 5 (Warzone): Game Engine with state management, and Assignment 2 - Part 2: Game Startup Phase.
 * @author Andrew Pulsifer and Matteo Bianchini (A1, P5), Chhay (A2, P2)
 * @date November 2025
 * @version 2.0
 * 
 * This file contains the implementation of the game engine that controls the flow of the game
 * using states, transitions, and commands. The engine validates commands against current state
 * and transitions to new states accordingly, rejecting invalid commands with error messages.
 * This file also implements the gamestart phase of the game, using input from the console or file to read the commands
 * and process them in the CommandProcessing class.
 */

#include "../include/GameEngine.h"
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Cards.h"
#include "../include/CommandProcessing.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>
#include <fstream>

// Importing only the neccessary std functions.
using std::cout;
using std::string;
using std::ostream;
using std::vector;
using std::endl;


// ======================= Command Class =======================

/** @brief Default constructor creates empty command */
Command::Command() : name(new string("")), effect (new string("")) {}

/** 
 * @brief Copy constructor performs deep copy of command name
 * @param other Command to copy from
 */
Command::Command(const Command& other) : name(new string(*other.name)), effect(new string(*other.effect)) {}

/**
 * @brief Parameterized constructor creates command with given name
 * @param cmdName The command name string
 */
Command::Command(const string& cmdName) : name(new string(cmdName)), effect(new string("")) {}

/**
 * @brief Parameterized constructor creates command with given name and an effect
 * @param cmdName The command name string
 * @param cmdEffect: The effect string.
 */
Command::Command(const std::string& cmdName, const std::string& cmdEffect) : name(new string(cmdName)), effect(new string(cmdEffect)) {}// Parameterized constructor with effect as a param.


/** @brief Destructor cleans up dynamically allocated name */
Command::~Command() {
    delete name;
    delete effect;
}

/**
 * @brief Assignment operator with deep copy semantics
 * @param other Command to assign from
 * @return Reference to this command for chaining
 */
Command& Command::operator=(const Command& other) {
    if (this != &other) {
        delete name;
        name = new string(*other.name);

        delete effect;
        effect = new string(*other.effect);
    }
    return *this;
}

/**
 * @brief Stream insertion operator for printing command details
 * @param os Output stream
 * @param command Command to print
 * @return Reference to output stream for chaining
 */
ostream& operator<<(ostream& os, const Command& command) {
    os << "Command: " << *command.name;
    return os;
}

/** @brief Get the name of this command */
string Command::getName() const { return *name; }

/** @brief Set the name of this command */
void Command::setName(const string& newName) { *name = newName; }

/** @brief Get the effect of the command. */
string Command::getEffect() const { return *effect; }

/** @brief Save the effect of the command. */
void Command::saveEffect(const string& newEffect) {
    *effect = newEffect;
    notify();  // Notify observers when effect is saved
}

/** @brief Generate log string for Command */
string Command::stringToLog() const {
    return "Command: " + *name + " | Effect: " + *effect;
}

// ======================= GameEngine Class =======================

/**
 * @brief Default constructor initializes game engine in Start state
 */
GameEngine::GameEngine() 
    : currentState(new GameState(GameState::Start)),
      stateTransitions(nullptr),
      gameMap(new Map()),
      players(new vector<Player*>()),
      mapLoader(new MapLoader()),
      deck(new Deck()) {
    initializeTransitions();
    cout << "GameEngine initialized in Start state." << endl;
}

/**
 * @brief Copy constructor performs deep copy of all components
 * @param other GameEngine to copy from
 */
GameEngine::GameEngine(const GameEngine& other) 
    : currentState(new GameState(*other.currentState)),
      stateTransitions(new TransitionMap(*other.stateTransitions)),
      gameMap(nullptr), // Map copying would require more complex logic
      players(new vector<Player*>()),
      mapLoader(nullptr), 
      deck(nullptr) {
    // Deep copy gameMap if it exists
    if(other.gameMap){
        gameMap = new Map(*other.gameMap);
    }
    
    // Deep copy mapLoader if it exists
    if(other.mapLoader){
        mapLoader = new MapLoader(*other.mapLoader);
    }
    
    // Deep copy players vector (shallow copy of Player pointers as they're managed elsewhere)
    for (Player* player : *other.players) {
        players->push_back(player);
    }

    // Deep copy deck if it exists
    if(other.deck) {
        deck = new Deck(*other.deck);
    }
}

/** @brief Destructor cleans up all dynamically allocated resources */
GameEngine::~GameEngine() {
    delete currentState;
    delete stateTransitions;
    delete players;
    delete gameMap;      // GameEngine owns the map
    delete mapLoader;    // GameEngine owns the map loader
    delete deck;
}

/**
 * @brief Assignment operator using copy-and-swap idiom
 * @param other GameEngine to assign from
 * @return Reference to this GameEngine for chaining
 */
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this != &other) {
        // Clean up existing resources
        delete currentState;
        delete stateTransitions;
        delete players;
        delete gameMap;
        delete mapLoader;
        delete deck;
        
        // Deep copy from other
        currentState = new GameState(*other.currentState);
        stateTransitions = new TransitionMap(*other.stateTransitions);
        players = new vector<Player*>();
        
        // Deep copy players vector
        for (Player* player : *other.players) {
            players->push_back(player);
        }
        
        // Deep copy gameMap if it exists
        if(other.gameMap){
            gameMap = new Map(*other.gameMap);
        } else {
            gameMap = nullptr;
        }
        
        // Deep copy mapLoader if it exists
        if(other.mapLoader){
            mapLoader = new MapLoader(*other.mapLoader);
        } else {
            mapLoader = nullptr;
        }

        // Deep copy deck if it exists
        if(other.deck) {
            deck = new Deck(*other.deck);
        } else {
            deck = nullptr;
        }
    }
    return *this;
}

/**
 * @brief Stream insertion operator for printing game engine state
 * @param os Output stream
 * @param engine GameEngine to print
 * @return Reference to output stream for chaining
 */
ostream& operator<<(ostream& os, const GameEngine& engine) {
    os << "GameEngine [State: " << engine.getStateName() 
       << ", Players: " << engine.players->size() << "]";
    return os;
}

/** 
 * @brief Generates a log string for the game engine
 * @return std::string String representation for logging
 */
string GameEngine::stringToLog() const {
    return "GameEngine: Current State = " + getStateName();
}

/** 
 * @brief Initialize the state transition table with valid transitions
 * 
 * @details Sets up all valid state transitions based on the assignment's state diagram.
 * All commands are defined in the GameCommands namespace (LOAD_MAP, VALIDATE_MAP, ADD_PLAYER, 
 * GAME_START, ASSIGN_COUNTRIES, ISSUE_ORDER, etc.) to ensure consistency and prevent typos.
 * Only transitions defined in this table are allowed for each state.
 */
void GameEngine::initializeTransitions() {
    using namespace GameCommands;
    
    stateTransitions = new TransitionMap{
        // Startup phase transitions
        {{GameState::Start,                string{LOAD_MAP}},           GameState::MapLoaded},
        {{GameState::MapLoaded,            string{LOAD_MAP}},           GameState::MapLoaded},
        {{GameState::MapLoaded,            string{VALIDATE_MAP}},       GameState::MapValidated},
        {{GameState::MapValidated,         string{ADD_PLAYER}},         GameState::PlayersAdded},
        {{GameState::PlayersAdded,         string{ADD_PLAYER}},         GameState::PlayersAdded},
        {{GameState::PlayersAdded,         string{GAME_START}},         GameState::AssignReinforcement},
        
        // Main game loop transitions
        {{GameState::AssignReinforcement,  string{ISSUE_ORDER}},        GameState::IssueOrders},
        {{GameState::IssueOrders,          string{ISSUE_ORDER}},        GameState::IssueOrders},
        {{GameState::IssueOrders,          string{END_ISSUE_ORDERS}},   GameState::ExecuteOrders},
        {{GameState::ExecuteOrders,        string{EXEC_ORDER}},         GameState::ExecuteOrders},
        {{GameState::ExecuteOrders,        string{END_EXEC_ORDERS}},    GameState::AssignReinforcement},
        {{GameState::ExecuteOrders,        string{WIN}},                GameState::Win},
        
        // End game transitions
        {{GameState::Win,                  string{REPLAY}},             GameState::Start},
        {{GameState::Win,                  string{QUIT}},               GameState::End},
    };
}

/**
 * @brief Process a command string and attempt state transition
 * @param commandStr The command string to process
 * @return true if command was valid and state transition occurred, false otherwise
 */
bool GameEngine::processCommand(const string& commandStr) {
    Command cmd(commandStr);
    return processCommand(cmd);
}

/**
 * @brief Process a command object and attempt state transition
 * @param cmd The command object to process
 * @return true if command was valid and state transition occurred, false otherwise
 */
bool GameEngine::processCommand(Command& cmd) {
    const string& commandStr = cmd.getName();
    
    // Validate command spelling first (check if command exists)
    if (!validCommandSpelling(commandStr)) {
        cmd.saveEffect(printTypoErrorMessage(commandStr));
        return false;
    }
    
    // Validate command is appropriate for current state
    if (!isValidCommand(commandStr)) {
        cmd.saveEffect(printStateErrorMessage(commandStr));
        return false;
    }
    
    // Extract command portion for transition lookup (before any space/arguments)
    std::string commandOnly = commandStr.substr(0, commandStr.find(" "));
    
    // Command is valid - perform state transition
    GameStateCmdPair key = make_pair(*currentState, commandOnly);
    GameState newState = (*stateTransitions)[key];
    
    cout << "Transitioning from " << getStateName(*currentState) 
         << " to " << getStateName(newState) 
         << " via command '" << commandStr << "'." << endl;
    
    cmd.saveEffect("The command '" + commandStr + "' is valid for the current state " + getStateName() + ".");
    executeStateTransition(newState, commandStr);
    return true;
}

/**
 * @brief Get the current state of the game engine
 * @return GameState enum value representing current state
 */
GameState GameEngine::getCurrentState() const { return *currentState; }

/**
 * @brief Get string representation of current state
 * @return String name of current state
 */
string GameEngine::getStateName() const {
    return getStateName(*currentState);
}

/**
 * @brief Get string representation of given state
 * @param state The state to get name for
 * @return String name of the state
 */
string GameEngine::getStateName(GameState state) const {
    switch(state) {
        case GameState::Start: return "Start";
        case GameState::MapLoaded: return "MapLoaded";
        case GameState::MapValidated: return "MapValidated";
        case GameState::PlayersAdded: return "PlayersAdded";
        case GameState::Gamestart: return "GameStart";
        case GameState::AssignReinforcement: return "AssignReinforcement";
        case GameState::IssueOrders: return "IssueOrders";
        case GameState::ExecuteOrders: return "ExecuteOrders";
        case GameState::Win: return "Win";
        case GameState::End: return "End";
        default: return "Unknown";
    }
}

// Getter for the deck variable.
Deck* GameEngine::getDeck() {
    return deck;
}

/**
 * @brief Check if a command is valid for the current state
 * @param commandStr Command string to validate
 * @return true if command is valid for current state, false otherwise
 */
bool GameEngine::isValidCommand(const string& commandStr) const {
    // Extract only the command portion (before any space/arguments)
    std::string commandOnly = commandStr.substr(0, commandStr.find(" "));
    
    GameStateCmdPair key = make_pair(*currentState, commandOnly);
    return stateTransitions->find(key) != stateTransitions->end();
}

/**
 * @brief Check if command spelling is valid (not a typo)
 * @param commandEntered The command string to validate
 * @return true if command spelling is valid, false otherwise
 */
bool GameEngine::validCommandSpelling(const string& commandEntered) const {
    using namespace GameCommands;
    
    // Extract only the command portion (before any space/arguments)
    std::string commandOnly = commandEntered.substr(0, commandEntered.find(" "));
    if(commandOnly == LOAD_MAP || commandOnly == VALIDATE_MAP || commandOnly == ADD_PLAYER ||
        commandOnly == ASSIGN_COUNTRIES || commandOnly == ISSUE_ORDER || commandOnly == END_ISSUE_ORDERS ||
        commandOnly == EXEC_ORDER || commandOnly == END_EXEC_ORDERS || commandOnly == WIN ||
        commandOnly == PLAY || commandOnly == END || commandOnly == GAME_START || 
        commandOnly == REPLAY || commandOnly == START || commandOnly == QUIT) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Get list of valid commands for current state
 * @return Vector of valid command strings
 */
vector<string> GameEngine::getValidCommands() const {
    vector<string> validCmds;
    for (const TransitionMap::value_type& transition : *stateTransitions) {
        if (transition.first.first == *currentState) {
            validCmds.push_back(transition.first.second);
        }
    }
    return validCmds;
}


/**
 * @brief Start a new game by setting state to Start
 */
void GameEngine::startGame() {
    *currentState = GameState::Start;
    cout << "Game started!" << endl;
}

/**
 * @brief End the current game by setting state to End
 */
void GameEngine::endGame() {
    *currentState = GameState::End;
    cout << "Game ended!" << endl;
}

/**
 * @brief Check if the game has ended
 * @return true if game is in End state, false otherwise
 */
bool GameEngine::isGameOver() const {
    return *currentState == GameState::End;
}

/**
 * @brief Print the current state to console
 */
void GameEngine::printCurrentState() const {
    cout << "Current State: " << getStateName() << endl;
}

/**
 * @brief Print all valid commands for current state to console
 */
void GameEngine::printValidCommands() const {
    cout << "Valid commands: ";
    vector<string> validCmds = getValidCommands();
    for (size_t i = 0; i < validCmds.size(); ++i) {
        cout << validCmds[i];
        if (i < validCmds.size() - 1) cout << ", ";
    }
    cout << endl;
}

/**
 * @brief Print error message for invalid command for current state
 * @param invalidCommand The command that was invalid
 */
std::string GameEngine::printStateErrorMessage(const string& invalidCommand) const {
    std::string errorMessage = std::string("ERROR: Invalid command '") + invalidCommand + "' for current state " + getStateName() + ".";
    cout << errorMessage << endl;
    
    // Show valid commands for current state
    vector<string> validCmds = getValidCommands();
    if (!validCmds.empty()) {
        cout << "Valid commands in " << getStateName() << " state: ";
        for (size_t i = 0; i < validCmds.size(); ++i) {
            cout << validCmds[i];
            if (i < validCmds.size() - 1) cout << ", ";
        }
        cout << endl;
    }

    return errorMessage;
}

/**
 * @brief Print error message for non-existent command (typo)
 * @param invalidCommand The command that doesn't exist
 * @return The error message string
 */
std::string GameEngine::printTypoErrorMessage(const string& invalidCommand) const {
    using namespace GameCommands;
    
    std::string errorMessage = std::string("ERROR: Unknown command '") + invalidCommand + "'. This command does not exist.";
    cout << errorMessage << endl;
    cout << "\nAll valid game commands:" << endl;
    cout << "Startup Phase:" << endl;
    cout << "  " << LOAD_MAP << " <mapfile>     - Load a map from assets/maps/" << endl;
    cout << "  " << VALIDATE_MAP << "            - Validate the loaded map" << endl;
    cout << "  " << ADD_PLAYER << " <playername>  - Add a player" << endl;
    cout << "  " << GAME_START << "              - Start the game" << endl;
    cout << "\nPlay Phase:" << endl;
    cout << "  " << ISSUE_ORDER << "             - Issue an order" << endl;
    cout << "  " << END_ISSUE_ORDERS << "        - End issuing orders" << endl;
    cout << "  " << EXEC_ORDER << "              - Execute an order" << endl;
    cout << "  " << END_EXEC_ORDERS << "         - End executing orders" << endl;
    cout << "\nEnd Game:" << endl;
    cout << "  " << WIN << "                    - Declare winner" << endl;
    cout << "  " << REPLAY << "                  - Replay the game (from win state)" << endl;
    cout << "  " << QUIT << "                    - Quit the game" << endl;
    
    return errorMessage;
}

/**
 * @brief Display welcome message with instructions for using the game engine
 */
void GameEngine::displayWelcomeMessage() const {
    using namespace GameCommands;
    
    cout << "\n=== Welcome to Warzone Game Engine ===" << endl;
    cout << "Type commands to navigate through game states." << endl;
    cout << "Type 'quit' to exit the game." << endl;
    cout << "=======================================" << endl;
}

/**
 * @brief Display current game status including state, valid commands, and player count
 */
void GameEngine::displayGameStatus() const {
    cout << "\n--- Game Status ---" << endl;
    
    // Show game phase context
    GameState state = *currentState;
    if (state == GameState::Start || state == GameState::MapLoaded || 
        state == GameState::MapValidated || state == GameState::PlayersAdded || 
        state == GameState::Gamestart) {
        cout << "Phase: Startup" << endl;
    } else if (state == GameState::AssignReinforcement || state == GameState::IssueOrders || 
               state == GameState::ExecuteOrders) {
        cout << "Phase: Main Game Loop" << endl;
    } else if (state == GameState::Win || state == GameState::Replay || state == GameState::End) {
        cout << "Phase: End Game" << endl;
    }
    
    printCurrentState();
    printValidCommands();
    cout << "Players in game: " << players->size() << endl;
    cout << "-------------------" << endl;
}

// === A2, PART 2: Game Startup Phase ===
void GameEngine::startupPhase(GameEngine& engine, CommandProcessor& commandPro) {
    // Calls the getCommand() method of the CommandProcessor to process the commands.
    commandPro.getCommand(engine);

    // After all commands are executed, all Command objects and their effects will be printed in the Command Processor.
    std::cout << commandPro << std::endl;
}


/**
 * @brief Transition to a new state
 * @param newState The new state to transition to
 */
void GameEngine::transition(GameState newState) {
    *currentState = newState;
    notify();
}

/**
 * @brief Check if transition from one state to another via command is valid
 * @param from Source state
 * @param command Command string
 * @param to Reference to store target state if valid
 * @return true if transition is valid, false otherwise
 */
bool GameEngine::isValidTransition(GameState from, const string& command, GameState& to) const {
    GameStateCmdPair key = make_pair(from, command);
    if (stateTransitions->find(key) != stateTransitions->end()) {
        to = (*stateTransitions)[key];
        return true;
    }
    return false;
}

/**
 * @brief Execute state transition and perform associated actions
 * @param newState Target state to transition to
 * @param command Command that triggered the transition
 */
void GameEngine::executeStateTransition(GameState newState, const string& command) {
    using namespace GameCommands;
    
    transition(newState);
    
    //Extract only the command, if a mapname or playername is entered.
    std::string commandOnly = command.substr(0, command.find(" "));

    // Execute state-specific actions based on command
    if (commandOnly == LOAD_MAP) {
        handleLoadMap(command);
    } else if (commandOnly == VALIDATE_MAP) {
        handleValidateMap();
    } else if (commandOnly == ADD_PLAYER) {
        handleAddPlayer(command);
    } else if (commandOnly == ASSIGN_COUNTRIES) {
        handleAssignCountries(command);
    } else if (commandOnly == ISSUE_ORDER) {
        handleIssueOrder(command);
    } else if (commandOnly == GAME_START) {
        handleGamestart();
        printGamestartLog();
    } else if (commandOnly == END_ISSUE_ORDERS || commandOnly == EXEC_ORDER || commandOnly == END_EXEC_ORDERS) {
        handleExecuteOrders(command);
    } else if (commandOnly == WIN || commandOnly == PLAY || commandOnly == END) {
        handleEndGame(command);
    }
}

// State-specific action handlers --> stub implementations for now

/**
 * @brief Extract and validate the map filename from a loadmap command
 * @param command The full command string (e.g., "loadmap World.map")
 * @param mapName Output parameter - the extracted and trimmed map filename
 * @return true if a valid filename was extracted, false otherwise
 */
bool GameEngine::extractMapFilename(const std::string& command, std::string& mapName) const {
    // Extract the filename from command (after the space)
    std::size_t nameIndex = command.find(' ');
    
    // Check if a map filename was provided
    if (nameIndex == std::string::npos || nameIndex + 1 >= command.length()) {
        std::cerr << "    ERROR: No map filename provided." << std::endl;
        std::cerr << "    Usage: loadmap <filename>" << std::endl;
        std::cerr << "    Example: loadmap World.map" << std::endl;
        std::cerr << "    Available maps are in assets/maps/ directory" << std::endl;
        return false;
    }
    
    mapName = command.substr(nameIndex + 1);
    
    // Trim any leading/trailing whitespace from map name
    size_t start = mapName.find_first_not_of(" \t\r\n");
    size_t end = mapName.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) {
        std::cerr << "    ERROR: Map filename is empty or contains only whitespace." << std::endl;
        std::cerr << "    Usage: loadmap <filename>" << std::endl;
        return false;
    }
    
    mapName = mapName.substr(start, end - start + 1);
    return true;
}

/**
 * @brief Validate that a map file exists at the specified path
 * @param mapPath The full path to the map file
 * @return true if the file exists and is accessible, false otherwise
 */
bool GameEngine::validateMapFileExists(const std::string& mapPath) const {
    std::ifstream fileCheck(mapPath);
    bool exists = fileCheck.good();
    fileCheck.close();
    
    if (!exists) {
        std::cerr << "    ERROR: Map file not found: " << mapPath << std::endl;
        std::cerr << "    Please check that:" << std::endl;
        std::cerr << "      1. The filename is correct (including .map extension)" << std::endl;
        std::cerr << "      2. The file exists in assets/maps/ directory" << std::endl;
        std::cerr << "    Hint: Check available maps in assets/maps/" << std::endl;
    }
    
    return exists;
}

/**
 * @brief Handle map loading command
 * @param command The command that triggered this action (e.g., "loadmap World.map")
 */
void GameEngine::handleLoadMap(const string& command) {
    cout << "  -> Loading map..." << endl;

    // Extract and validate the map filename
    std::string mapName;
    if (!extractMapFilename(command, mapName)) {
        return;
    }
    
    // Prepend the maps directory path (following the pattern from MapDriver)
    std::string mapPath = "assets/maps/" + mapName;
    
    // Validate that the file exists
    if (!validateMapFileExists(mapPath)) {
        return;
    }
    
    // Load map using the full path
    try {
        mapLoader->loadMap(mapPath, *gameMap);
        std::cout << "    SUCCESS: Map '" << mapName << "' loaded from " << mapPath << "." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    ERROR: Failed to load map '" << mapName << "': " << e.what() << std::endl;
        std::cerr << "    The map file may be corrupted or have invalid format." << std::endl;
    }
}


/**
 * @brief Handle map validation command
 */
void GameEngine::handleValidateMap() {
    cout << "  -> Validating map..." << endl;
    
    // validate the map.
    bool validMap = gameMap->validate();

    if(validMap) {
        std::cout << "    The map is valid." << std::endl;
    } else {
        std::cout << "    The map is NOT valid." << std::endl;
    }
}

/**
 * @brief Handle add player command
 * @param command The command that triggered this action
 */
void GameEngine::handleAddPlayer(const string& command) {
    cout << "  -> Adding player..." << endl;

    // Extract the mapname from command.
    std::size_t nameIndex = command.find(' ');
    std::string playerName = command.substr(nameIndex + 1);

    // Add player into GameEngine's vectors of players.
    players->push_back(new Player(playerName));

    std::cout << "    Player '" << playerName << "' successfully added." << std::endl;
}

/**
 * @brief Handle assign countries command
 * @param command The command that triggered this action
 */
void GameEngine::handleAssignCountries(const string& command) {
    cout << "  -> Assigning countries... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
}

/**
 * @brief Handle issue order command
 * @param command The command that triggered this action
 */
void GameEngine::handleIssueOrder(const string& command) {
    cout << "  -> Issuing order... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
}

/**
 * @brief Handle execute orders command
 * @param command The command that triggered this action
 */
void GameEngine::handleExecuteOrders(const string& command) {
    cout << "  -> Executing orders... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
}

/**
 * @brief Handle end game command
 * @param command The command that triggered this action
 */
void GameEngine::handleEndGame(const string& command) {
    cout << "  -> Handling game end... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
}

// Handle the 'gamestart command.'
void GameEngine::handleGamestart() {
    
    cout << "  -> Handling Gamestart...\n" << endl;
    // (a) Fairly distribute all the territories to the player.
        // get vectors of Territories in map.
        const std::vector<Territory*>& territories = gameMap->getTerritories();
        
        // get the number of Territories in map, and the number of players.
        size_t numOfTerritories = gameMap->getTerritories().size();
        size_t numOfPlayers = players->size();

        // Fairly Distribute territories by alternating the territory given to player. 
        for(size_t i = 0; i < numOfTerritories; i++) {
            Territory* currentTerritory = territories.at(i);
            size_t currentPlayerIndex = i % numOfPlayers;
            
            players->at(currentPlayerIndex)->addPlayerTerritory(currentTerritory);
        }
        
        std::cout << "  ...Territories are distributed to each player.\n\n";
    

    // (b) Determine randomly the order of play of players (Shuffling the actual vector).

        // Generate random number.
        std::random_device random;
        std::mt19937 g(random());

        // Shuffle
        std::shuffle(players->begin(), players->end(), g);

        std::cout << "  ...Order of players are shuffled.\n\n";


    // (c) Give 50 army units to each player.
        // ** TODO: IMPLEMENT (essentially just uncomment the code below) AFTER THE REINFORCEMENT POOL IS MERGED INTO MAIN. **
        // for(Player* p : *players) {
        //     p->setReinforcementPool(50);
        // }
    
        std::cout << "  ...50 army units are assigned to each player.\n\n";


    // (d) Let each player draw 2 initial cards from Deck.
        std::cout << "  ...Each player draws 2 cards from Deck.\n\n";
        for(Player* p : *players) {
            Hand* playerHand = p->getPlayerHand();
            std::cout << "    ------Player " << p->getPlayerName() << ":\n    ";
            deck->draw(*playerHand);
            std::cout << "    ";
            deck->draw(*playerHand);
        }


    // (e) Switch game to play phase (the assignreinforcement state).
        transition(GameState::AssignReinforcement);
        std::cout << "\n  ...The state is switched to play.\n\n";

    
}


// Prints out the status of each step after executing the 'gamestart' command.
void GameEngine::printGamestartLog() const {
    std::cout << "=======================================" << std::endl;
    std::cout << "=== PRINTING OUT THE GAMESTART LOG: ===" << std::endl;
    std::cout << "=======================================\n" << std::endl;

    // (a) Fairly distribute all the territories to the player.
        std::cout << "=== (a) Distributing territories: ===" << std::endl;

        size_t numOfPlayers = players->size();

        // See the territories that each player own.
        for(size_t i = 0; i < numOfPlayers; i++) {
            std::cout << "  " << *(players->at(i)) << std::endl;
        }
        std::cout << "\n";
    

    // (b) Determine randomly the order of play of players (Shuffling the actual vector).
        std::cout << "=== (b) Determine the order of players randomly, by shuffling the vector: ===" << std::endl;
        std::cout << "  After shuffling - Players: ";
        for(Player* p : *players) {
            std::cout << p->getPlayerName() << " ";
        }
        std::cout << "\n\n\n";
    

    // (c) Give 50 army units to each player.
        std::cout << "=== (c) Give 50 army units to each player: ===" << std::endl;
        // TO DO: Uncomment after setReinforcementPool is integrated.
        // for(Player* p : *players) {
        //     std::cout << "Player " << p->getName() << " - Reinforcement Pool: " << p->getReinforcementPool();
        // }
        std::cout << "\n\n";


    // (d) Let each player draw 2 initial cards from Deck.
        std::cout << "=== (d) Let each player draw 2 cards from the Deck: ===" << std::endl;
        for(Player* p : *players) {
            std::cout << "  Player " << p->getPlayerName() << " - ";
            p->getPlayerHand()->showHand();
        }
        std::cout << "\n\n";


    // (e) Switch game to play phase (the assignreinforcement state).
        std::cout << "=== (e) Switch game to play phase: ===" << std::endl;
}
