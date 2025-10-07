/**
 * @file GameEngine.cpp
 * @brief Implementation of Assignment 1 – Part 5 (Warzone): Game Engine with state management
 * @author Andrew Pulsifer
 * @date October 2025
 * @version 1.0
 * 
 * This file contains the implementation of the game engine that controls the flow of the game
 * using states, transitions, and commands. The engine validates commands against current state
 * and transitions to new states accordingly, rejecting invalid commands with error messages.
 */

#include "../include/GameEngine.h"
#include "../include/Map.h"
#include "../include/Player.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

// ======================= Command Class =======================

/** @brief Default constructor creates empty command */
Command::Command() : name(new string("")) {}

/** 
 * @brief Copy constructor performs deep copy of command name
 * @param other Command to copy from
 */
Command::Command(const Command& other) : name(new string(*other.name)) {}

/**
 * @brief Parameterized constructor creates command with given name
 * @param cmdName The command name string
 */
Command::Command(const string& cmdName) : name(new string(cmdName)) {}

/** @brief Destructor cleans up dynamically allocated name */
Command::~Command() {
    delete name;
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

string Command::getName() const { return *name; }
void Command::setName(const string& newName) { *name = newName; }

// ======================= GameEngine Class =======================

/**
 * @brief Default constructor initializes game engine in Start state
 */
GameEngine::GameEngine() 
    : currentState(new GameState(GameState::Start)),
      stateTransitions(nullptr),
      gameMap(nullptr),
      players(new vector<Player*>()),
      mapLoader(nullptr) {
    initializeTransitions();
    cout << "GameEngine initialized in Start state." << endl;
}

/**
 * @brief Copy constructor performs deep copy of all components
 * @param other GameEngine to copy from
 */
GameEngine::GameEngine(const GameEngine& other) 
    : currentState(new GameState(*other.currentState)),
      stateTransitions(new map<pair<GameState, string>, GameState>(*other.stateTransitions)),
      gameMap(nullptr), // Map copying would require more complex logic
      players(new vector<Player*>()),
      mapLoader(nullptr) {
    // Deep copy players vector (shallow copy of Player pointers as they're managed elsewhere)
    for (Player* player : *other.players) {
        players->push_back(player);
    }
}

/** @brief Destructor cleans up all dynamically allocated resources */
GameEngine::~GameEngine() {
    delete currentState;
    delete stateTransitions;
    delete players;
    // Note: gameMap and mapLoader are not deleted here as they may be managed elsewhere
    // In a full implementation, you'd need to decide on ownership semantics
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
        
        // Deep copy from other
        currentState = new GameState(*other.currentState);
        stateTransitions = new map<pair<GameState, string>, GameState>(*other.stateTransitions);
        players = new vector<Player*>();
        
        for (Player* player : *other.players) {
            players->push_back(player);
        }
        
        gameMap = other.gameMap; // Shallow copy - assumes external ownership
        mapLoader = other.mapLoader; // Shallow copy - assumes external ownership
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
 * @brief Initialize the state transition table with valid transitions
 * 
 * @details Sets up all valid state transitions based on the assignment's state diagram:
 * - Commands like "loadmap", "validatemap", "addplayer", etc.
 * - Ensures only valid transitions are allowed for each state
 */
void GameEngine::initializeTransitions() {
    // HARDCODED state transitions as per assignment diagram !!!!!!!
    stateTransitions = new map<pair<GameState, string>, GameState>();
    
    // Initialize all valid transitions based on assignment state diagram
    (*stateTransitions)[{GameState::Start, "loadmap"}] = GameState::MapLoaded;
    
    (*stateTransitions)[{GameState::MapLoaded, "loadmap"}] = GameState::MapLoaded;
    (*stateTransitions)[{GameState::MapLoaded, "validatemap"}] = GameState::MapValidated;
    
    (*stateTransitions)[{GameState::MapValidated, "addplayer"}] = GameState::PlayersAdded;
    
    (*stateTransitions)[{GameState::PlayersAdded, "addplayer"}] = GameState::PlayersAdded;
    (*stateTransitions)[{GameState::PlayersAdded, "assigncountries"}] = GameState::AssignReinforcement;
    
    (*stateTransitions)[{GameState::AssignReinforcement, "issueorder"}] = GameState::IssueOrders;
    
    (*stateTransitions)[{GameState::IssueOrders, "issueorder"}] = GameState::IssueOrders;
    (*stateTransitions)[{GameState::IssueOrders, "endissueorders"}] = GameState::ExecuteOrders;
    
    (*stateTransitions)[{GameState::ExecuteOrders, "execorder"}] = GameState::ExecuteOrders;
    (*stateTransitions)[{GameState::ExecuteOrders, "endexecorders"}] = GameState::AssignReinforcement;
    (*stateTransitions)[{GameState::ExecuteOrders, "win"}] = GameState::Win;
    
    (*stateTransitions)[{GameState::Win, "play"}] = GameState::Start;
    (*stateTransitions)[{GameState::Win, "end"}] = GameState::End;
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
bool GameEngine::processCommand(const Command& cmd) {
    string commandStr = cmd.getName();
    
    if (!isValidCommand(commandStr)) {
        printErrorMessage(commandStr);
        return false;
    }
    
    auto key = make_pair(*currentState, commandStr);
    GameState newState = (*stateTransitions)[key];
    
    cout << "Transitioning from " << getStateName(*currentState) 
         << " to " << getStateName(newState) 
         << " via command '" << commandStr << "'" << endl;
    
    executeStateTransition(newState, commandStr);
    return true;
}

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
        case GameState::AssignReinforcement: return "AssignReinforcement";
        case GameState::IssueOrders: return "IssueOrders";
        case GameState::ExecuteOrders: return "ExecuteOrders";
        case GameState::Win: return "Win";
        case GameState::End: return "End";
        default: return "Unknown";
    }
}

/**
 * @brief Check if a command is valid for the current state
 * @param commandStr Command string to validate
 * @return true if command is valid for current state, false otherwise
 */
bool GameEngine::isValidCommand(const string& commandStr) const {
    auto key = make_pair(*currentState, commandStr);
    return stateTransitions->find(key) != stateTransitions->end();
}

/**
 * @brief Get list of valid commands for current state
 * @return Vector of valid command strings
 */
vector<string> GameEngine::getValidCommands() const {
    vector<string> validCmds;
    for (const auto& transition : *stateTransitions) {
        if (transition.first.first == *currentState) {
            validCmds.push_back(transition.first.second);
        }
    }
    return validCmds;
}

void GameEngine::startGame() {
    *currentState = GameState::Start;
    cout << "Game started!" << endl;
}

void GameEngine::endGame() {
    *currentState = GameState::End;
    cout << "Game ended!" << endl;
}

bool GameEngine::isGameOver() const {
    return *currentState == GameState::End;
}

void GameEngine::printCurrentState() const {
    cout << "Current State: " << getStateName() << endl;
}

void GameEngine::printValidCommands() const {
    cout << "Valid commands: ";
    auto validCmds = getValidCommands();
    for (size_t i = 0; i < validCmds.size(); ++i) {
        cout << validCmds[i];
        if (i < validCmds.size() - 1) cout << ", ";
    }
    cout << endl;
}

void GameEngine::printErrorMessage(const string& invalidCommand) const {
    cout << "ERROR: Invalid command '" << invalidCommand 
         << "' for current state " << getStateName() << endl;
}

void GameEngine::displayWelcomeMessage() const {
    cout << "\n=== Welcome to Warzone Game Engine ===" << endl;
    cout << "Type commands to navigate through game states." << endl;
    cout << "Type 'help' to see valid commands for current state." << endl;
    cout << "Type 'quit' to exit the game." << endl;
    cout << "=======================================" << endl;
}

void GameEngine::displayGameStatus() const {
    cout << "\n--- Game Status ---" << endl;
    printCurrentState();
    printValidCommands();
    cout << "Players in game: " << players->size() << endl;
    cout << "-------------------" << endl;
}

/**
 * @brief Set the current state (private helper)
 * @param newState The new state to transition to
 */
void GameEngine::setState(GameState newState) {
    *currentState = newState;
}

/**
 * @brief Check if transition from one state to another via command is valid
 * @param from Source state
 * @param command Command string
 * @param to Reference to store target state if valid
 * @return true if transition is valid, false otherwise
 */
bool GameEngine::isValidTransition(GameState from, const string& command, GameState& to) const {
    auto key = make_pair(from, command);
    auto it = stateTransitions->find(key);
    if (it != stateTransitions->end()) {
        to = it->second;
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
    setState(newState);
    
    // Execute state-specific actions based on command
    if (command == "loadmap") {
        handleLoadMap(command);
    } else if (command == "validatemap") {
        handleValidateMap(command);
    } else if (command == "addplayer") {
        handleAddPlayer(command);
    } else if (command == "assigncountries") {
        handleAssignCountries(command);
    } else if (command == "issueorder") {
        handleIssueOrder(command);
    } else if (command == "endissueorders" || command == "execorder" || command == "endexecorders") {
        handleExecuteOrders(command);
    } else if (command == "win" || command == "play" || command == "end") {
        handleEndGame(command);
    }
}

// State-specific action handlers (stubs for future implementation)
void GameEngine::handleLoadMap(const string& command) {
    cout << "  -> Loading map... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleValidateMap(const string& command) {
    cout << "  -> Validating map... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleAddPlayer(const string& command) {
    cout << "  -> Adding player... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleAssignCountries(const string& command) {
    cout << "  -> Assigning countries... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleIssueOrder(const string& command) {
    cout << "  -> Issuing order... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleExecuteOrders(const string& command) {
    cout << "  -> Executing orders... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

void GameEngine::handleEndGame(const string& command) {
    cout << "  -> Handling game end... (stub implementation)" << endl;
    (void)command; // Suppress unused parameter warning
}

// ======================= Free Function =======================

/**
 * @brief Test function that creates a console-driven interface for game states
 * 
 * @details Creates an interactive console that allows users to navigate through
 * all game states by typing commands. Invalid commands result in error messages
 * without state transitions, as required by the assignment.
 */
void testGameStates() {
    cout << "\n=== Testing Game States ===" << endl;
    
    GameEngine engine;
    engine.displayWelcomeMessage();
    engine.displayGameStatus();
    
    string input;
    cout << "\nEnter command: ";
    
    while (getline(cin, input)) {
        // Trim whitespace and convert to lowercase for consistency
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        if (input.empty()) {
            cout << "Enter command: ";
            continue;
        }
        
        if (input == "quit" || input == "exit") {
            cout << "Exiting game engine test." << endl;
            break;
        }
        
        if (input == "help") {
            engine.displayGameStatus();
        } else if (input == "status") {
            engine.displayGameStatus();
        } else {
            // Process the actual game command
            engine.processCommand(input);
            
            // Show current state after processing
            cout << "  Current state: " << engine.getStateName() << endl;
            
            // Check if game is over
            if (engine.isGameOver()) {
                cout << "Game has ended. Type 'quit' to exit or continue testing." << endl;
            }
        }
        
        cout << "\nEnter command: ";
    }
    
    cout << "=== Game States Test Complete ===" << endl;
}