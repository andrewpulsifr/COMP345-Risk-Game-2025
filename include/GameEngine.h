/**
 * @file GameEngine.h
 * @brief Assignment 1 – Part 5 (Warzone): Game Engine with state management
 *
 * @details
 *  Implements a game engine that controls the flow of the game using states, transitions, and commands.
 *  The state represents a certain phase of the game and dictates valid actions or user commands.
 *  Commands may trigger transitions to other states, controlling the game flow.
 *
 * @note All Part-5 classes/functions live in this duo (GameEngine.h/GameEngine.cpp). 
 *       The driver `testGameStates()` is implemented in GameEngineDriver.cpp.
 */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <utility>

// Forward declarations
class Map;
class Player;
class MapLoader;
class Order;
class Territory;

/**
 * @brief Simple command object representing user input commands
 * 
 * @details Commands are strings that correspond to edges in the state transition diagram.
 * They trigger state transitions when valid for the current state.
 */
class Command {
public:
    Command(); // Default constructor
    Command(const Command& other); // Copy constructor
    Command(const std::string& name); // Parameterized constructor
    ~Command(); // Destructor
    
    Command& operator=(const Command& other); // Assignment operator
    friend std::ostream& operator<<(std::ostream& os, const Command& command); // Stream insertion operator
    
    std::string getName() const;
    void setName(const std::string& name);

private:
    std::string* name; // Command name as pointer (requirement: all data members must be pointer type)
};

/**
 * @brief Enumeration of all possible game states
 * 
 * @details States correspond to nodes in the state transition diagram:
 * - Start: Initial startup state  
 * - MapLoaded: Map file has been loaded successfully  
 * - MapValidated: Loaded map has been validated
 * - PlayersAdded: Players have been added to the game
 * - AssignReinforcement: Countries assigned, ready for reinforcement phase
 * - IssueOrders: Players are issuing orders
 * - ExecuteOrders: Orders are being executed
 * - Win: Game has ended with a winner
 * - End: Final end state
 */
enum class GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders, 
    ExecuteOrders,
    Win,
    End
};

/**
 * @brief Main game engine class that manages game state and transitions
 * 
 * @details Controls the flow of the game using a finite state machine.
 * Validates commands against current state and transitions to new states accordingly.
 * Rejects invalid commands with error messages.
 */
class GameEngine {
public:
    GameEngine(); // Default constructor
    GameEngine(const GameEngine& other); // Copy constructor
    ~GameEngine(); // Destructor
    
    GameEngine& operator=(const GameEngine& other); // Assignment operator
    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine); // Stream insertion operator

    // Core game engine methods
    bool processCommand(const std::string& commandStr);
    bool processCommand(const Command& cmd);
    
    // State accessors
    GameState getCurrentState() const;
    std::string getStateName() const;
    std::string getStateName(GameState state) const;
    
    // Game state queries
    bool isValidCommand(const std::string& commandStr) const;
    std::vector<std::string> getValidCommands() const;
    
    // Game progression methods
    void startGame();
    void endGame();
    bool isGameOver() const;
    
    // Utility methods for console interface
    void printCurrentState() const;
    void printValidCommands() const;
    void printErrorMessage(const std::string& invalidCommand) const;
    void displayWelcomeMessage() const;
    void displayGameStatus() const;

private:
    GameState* currentState; // Current game state (pointer as required)
    std::map<std::pair<GameState, std::string>, GameState>* stateTransitions; // Valid transitions map (pointer as required)
    
    // Game components (will be used in future assignments)
    Map* gameMap; // The current game map (pointer as required)
    std::vector<Player*>* players; // List of players in the game (pointer as required)
    MapLoader* mapLoader; // Map loader instance (pointer as required)
    
    // Private helper methods
    void initializeTransitions();
    void setState(GameState newState);
    bool isValidTransition(GameState from, const std::string& command, GameState& to) const;
    void executeStateTransition(GameState newState, const std::string& command);
    
    // State-specific action methods (stubs for future implementation)
    void handleLoadMap(const std::string& command);
    void handleValidateMap(const std::string& command);
    void handleAddPlayer(const std::string& command);
    void handleAssignCountries(const std::string& command);
    void handleIssueOrder(const std::string& command);
    void handleExecuteOrders(const std::string& command);
    void handleEndGame(const std::string& command);
};

/**
 * @brief Free function for testing game states
 * 
 * @details Creates a console-driven interface that allows the user to navigate
 * through all states by typing commands as specified in the state transition diagram.
 * Invalid commands result in error messages without state transitions.
 */
void testGameStates();
