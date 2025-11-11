/**
 * @file GameEngine.h
 * @brief Assignment 1 – Part 5 (Warzone): Game Engine with state management, and Assignment 2 - Part 2: Game Startup Phase.
 *
 * @details
 *  Implements a game engine that controls the flow of the game using states, transitions, and commands.
 *  The state represents a certain phase of the game and dictates valid actions or user commands.
 *  Commands may trigger transitions to other states, controlling the game flow.
 *
 * @note All A1, Part-5 and A2, Part 2 classes/functions live in this duo (GameEngine.h/GameEngine.cpp). 
 *       The driver `testGameStates()` is implemented in GameEngineDriver.cpp.
 *       The driver 'testStartupPhase()' is implemented in GameEngineDriver.cpp.
 */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <utility>
#include "LoggingObserver.h"

// Forward declarations
class Map;
class Player;
class MapLoader;
class Order;
class Territory;
class Deck;
class CommandProcessor;

/**
 * @brief Simple command object representing user input commands
 * 
 * @details Commands are strings that correspond to edges in the state transition diagram.
 * They trigger state transitions when valid for the current state.
 */
class Command : public ILoggable , public Subject {
public:
    Command(); // Default constructor
    Command(const Command& other); // Copy constructor
    Command(const std::string& name); // Parameterized constructor
    Command(const std::string& name, const std::string& effect); // Parameterized constructor with effect as a param.
    ~Command(); // Destructor
    
    Command& operator=(const Command& other); // Assignment operator
    friend std::ostream& operator<<(std::ostream& os, const Command& command); // Stream insertion operator
    
    std::string getName() const;
    void setName(const std::string& name);

    std::string getEffect() const;
    void saveEffect(const std::string& effect);
    
    // ILoggable interface
    std::string stringToLog() const override;

private:
    std::string* name; // Command name as pointer (requirement: all data members must be pointer type)
    std::string* effect; // Save the effect of the command from the CommandProcessing.
};

/**
 * @brief Command string constants for game state transitions
 * 
 * @details Provides a single source of truth for all valid game commands.
 * These constants should be used throughout the codebase instead of hardcoded strings.
 */
namespace GameCommands {
    const std::string LOAD_MAP = "loadmap";
    const std::string VALIDATE_MAP = "validatemap";
    const std::string ADD_PLAYER = "addplayer";
    const std::string ASSIGN_COUNTRIES = "assigncountries";
    const std::string ISSUE_ORDER = "issueorder";
    const std::string END_ISSUE_ORDERS = "endissueorders";
    const std::string EXEC_ORDER = "execorder";
    const std::string END_EXEC_ORDERS = "endexecorders";
    const std::string WIN = "win";
    const std::string PLAY = "play";
    const std::string END = "end";
    const std::string GAME_START = "gamestart";
    const std::string REPLAY = "replay";
    const std::string START = "start";
    const std::string QUIT = "quit";
}

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
    End,
    Gamestart,
    Replay
};

/**
 * @brief Main game engine class that manages game state and transitions
 * 
 * @details Controls the flow of the game using a finite state machine.
 * Validates commands against current state and transitions to new states accordingly.
 * Rejects invalid commands with error messages.
 */
class GameEngine : public ILoggable, public Subject {
public:
    GameEngine(); // Default constructor
    GameEngine(const GameEngine& other); // Copy constructor
    ~GameEngine(); // Destructor
    
    GameEngine& operator=(const GameEngine& other); // Assignment operator
    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine); // Stream insertion operator

    // Core game engine methods
    bool processCommand(const std::string& commandStr);
    bool processCommand(Command& cmd);
    
    // State accessors
    GameState getCurrentState() const;
    std::string getStateName() const;
    std::string getStateName(GameState state) const;

    // Getter for deck.
    Deck* getDeck();
    
    // Game state queries
    bool isValidCommand(const std::string& commandStr) const;
    std::vector<std::string> getValidCommands() const;
    
    // Command validation
    bool validCommandSpelling(const std::string& commandEntered) const;
    
    // Game progression methods
    void startGame();
    void endGame();
    bool isGameOver() const;
    
    // Utility methods for console interface
    void printCurrentState() const;
    void printValidCommands() const;
    std::string printStateErrorMessage(const std::string& invalidCommand) const;
    std::string printTypoErrorMessage(const std::string& invalidCommand) const;
    void displayWelcomeMessage() const;
    void displayGameStatus() const;

    // ILoggable interface implementation
    std::string stringToLog() const override;

    // === A2, PART 2: Game Startup Phase ===
    void startupPhase(GameEngine& engine, CommandProcessor& commandPro);

private:
    // Type aliases for readability
    using GameStateCmdPair = std::pair<GameState, std::string>;
    using TransitionMap = std::map<GameStateCmdPair, GameState>;

    GameState* currentState; // Current game state using pointer as required
    TransitionMap* stateTransitions; // Valid transitions map using pointer as required

    // Game data members
    Map* gameMap; // The current game map using pointer as required
    std::vector<Player*>* players; // List of players in the game using pointer as required
    MapLoader* mapLoader; // Map loader instance (pointer as required)
    Deck* deck; // One deck of cards for each game.
    
    // Private helper methods
    void initializeTransitions();
    void transition(GameState newState);
    bool isValidTransition(GameState from, const std::string& command, GameState& to) const;
    void executeStateTransition(GameState newState, const std::string& command);
    
    // State-specific action methods
    void handleLoadMap(const std::string& command);
    void handleValidateMap();
    void handleAddPlayer(const std::string& command);
    void handleAssignCountries(const std::string& command);
    void handleIssueOrder(const std::string& command);
    void handleExecuteOrders(const std::string& command);
    void handleEndGame(const std::string& command);


    // Handling Gamestart command in Game startup phase (Part 2 of A2).
    void handleGamestart();
    void printGamestartLog() const;
};

/**
 * @brief Free function for testing game states
 * 
 * @details Creates a console-driven interface that allows the user to navigate
 * through all states by typing commands as specified in the state transition diagram.
 * Invalid commands result in error messages without state transitions.
 */
void testGameStates();
