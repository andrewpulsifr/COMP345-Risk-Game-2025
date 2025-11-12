/**
 * @file GameEngine.cpp
 * @brief Implementation of Assignment 1 – Part 5 (Warzone): Game Engine with state management
 * @author Andrew Pulsifer and Matteo Bianchini
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
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <iostream>
#include <algorithm>
#include <sstream>

// Importing only the neccessary std functions.
using std::cout;
using std::string;
using std::ostream;
using std::vector;
using std::endl;


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

/** @brief Get the name of this command */
string Command::getName() const { return *name; }

/** @brief Set the name of this command */
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
      stateTransitions(new TransitionMap(*other.stateTransitions)),
      gameMap(nullptr), // Map copying would require more complex logic
      players(new vector<Player*>()),
      mapLoader(nullptr) {
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
}

/** @brief Destructor cleans up all dynamically allocated resources */
GameEngine::~GameEngine() {
    delete currentState;
    delete stateTransitions;
    delete players;
    delete gameMap;      // GameEngine owns the map
    delete mapLoader;    // GameEngine owns the map loader
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
    stateTransitions = new TransitionMap{
        {{GameState::Start,            "loadmap"},       GameState::MapLoaded},
        {{GameState::MapLoaded,        "loadmap"},       GameState::MapLoaded},
        {{GameState::MapLoaded,        "validatemap"},   GameState::MapValidated},
        {{GameState::MapValidated,     "addplayer"},     GameState::PlayersAdded},
        {{GameState::PlayersAdded,     "addplayer"},     GameState::PlayersAdded},
        {{GameState::PlayersAdded,     "assigncountries"}, GameState::AssignReinforcement},
        {{GameState::AssignReinforcement,"issueorder"},  GameState::IssueOrders},
        {{GameState::IssueOrders,      "issueorder"},    GameState::IssueOrders},
        {{GameState::IssueOrders,      "endissueorders"},GameState::ExecuteOrders},
        {{GameState::ExecuteOrders,    "execorder"},     GameState::ExecuteOrders},
        {{GameState::ExecuteOrders,    "endexecorders"}, GameState::AssignReinforcement},
        {{GameState::ExecuteOrders,    "win"},           GameState::Win},
        {{GameState::Win,              "play"},          GameState::Start},
        {{GameState::Win,              "end"},           GameState::End},
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
bool GameEngine::processCommand(const Command& cmd) {
    string commandStr = cmd.getName();
    
    if (!isValidCommand(commandStr)) {
        printErrorMessage(commandStr);
        return false;
    }
    
    GameStateCmdPair key = make_pair(*currentState, commandStr);
    GameState newState = (*stateTransitions)[key];
    
    cout << "Transitioning from " << getStateName(*currentState) 
         << " to " << getStateName(newState) 
         << " via command '" << commandStr << "'" << endl;
    
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
    GameStateCmdPair key = make_pair(*currentState, commandStr);
    return stateTransitions->find(key) != stateTransitions->end();
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
 * @brief Print error message for invalid command
 * @param invalidCommand The command that was invalid
 */
void GameEngine::printErrorMessage(const string& invalidCommand) const {
    cout << "ERROR: Invalid command '" << invalidCommand 
         << "' for current state " << getStateName() << endl;
}

/**
 * @brief Display welcome message with instructions for using the game engine
 */
void GameEngine::displayWelcomeMessage() const {
    cout << "\n=== Welcome to Warzone Game Engine ===" << endl;
    cout << "Type commands to navigate through game states." << endl;
    cout << "Type 'help' to see valid commands for current state." << endl;
    cout << "Type 'quit' to exit the game." << endl;
    cout << "=======================================" << endl;
}

/**
 * @brief Display current game status including state, valid commands, and player count
 */
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

// State-specific action handlers --> stub implementations for now

/**
 * @brief Handle map loading command
 * @param command The command that triggered this action
 */
void GameEngine::handleLoadMap(const string& command) {
    cout << "  -> Loading map... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning 
}

/**
 * @brief Handle map validation command
 * @param command The command that triggered this action
 */
void GameEngine::handleValidateMap(const string& command) {
    cout << "  -> Validating map... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
}

/**
 * @brief Handle add player command
 * @param command The command that triggered this action
 */
void GameEngine::handleAddPlayer(const string& command) {
    cout << "  -> Adding player... (stub implementation)" << endl;
    (void)command; // Stub suppress unused parameter warning
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

/**
 * @brief Main game loop managing the phases of the game
 */
void GameEngine::reinforcementPhase() {
    if(!gameMap || !players || players->empty()) {
        cout << "Reinforcement phase skipped (no map or players).\n";
        return;
    }

    std::cout << "\n--- Reinforcement Phase ---\n";

    const auto& continents = gameMap->getContinents();

    for(Player* p : *players) {
        if (!p) continue;

        vector<Territory*> ownedTerritories = p->getOwnedTerritories();
        int territoryCount = static_cast<int>(ownedTerritories.size());

        if(territoryCount == 0) {
            cout << "Player " << p->getPlayerName() << " controls no territories (no reinforcements).\n";
            continue;
        }

        //Base: floor(#territories / 3), minimum 3
          int base = territoryCount / 3;
        if (base < 3) base = 3;

        // Continent bonuses
        int bonus = 0;
        for (Continent* c : continents) {
            if (!c) continue;
            bool ownsAll = true;
            for (Territory* t : c->getTerritories()) {
                if (!t || t->getOwner() != p) {
                    ownsAll = false;
                    break;
                }
            }
            if (ownsAll) {
                bonus += c->getBonus();
            }
        }

        int total = base + bonus;
        p->addReinforcements(total);

        std::cout << "Player " << p->getPlayerName()
                  << " owns " << territoryCount
                  << " territories: +" << base << " base, +"
                  << bonus << " continent bonus = "
                  << total << " armies. Pool: "
                  << p->getReinforcementPool() << "\n";
    }

}


/**
 * @brief Issue orders phase where players issue their orders in round-robin fashion
 */
void GameEngine::issueOrdersPhase() {
    std::cout << "\n--- Issue Orders Phase ---\n";
    if (!players || players->empty()) return;

    const std::size_t n = players->size();
    // Track whether each player already issued a non-deploy in THIS phase
    std::vector<bool> nonDeployIssued(n, false);

    bool issuedInPass = false;
    std::size_t safetyCounter = 0;
    const std::size_t safetyLimit = 1000; // hard cap to avoid livelock from bad logic

    do {
        issuedInPass = false;

        for (std::size_t i = 0; i < n; ++i) {
            Player* p = (*players)[i];
            if (!p) continue;

            // If no reinforcements left and this player already issued one non-deploy this phase,
            // skip further non-deploys until next phase (after execution changes state).
            if (p->getReinforcementPool() == 0 && nonDeployIssued[i]) {
                continue;
            }

            OrdersList* ol = p->getOrdersList();
            const std::size_t before = (ol ? ol->size() : 0);

            const bool created = p->issueOrder();
            if (!created) continue;

            issuedInPass = true;

            // Detect what was just added to mark non-deploy once per phase.
            if (ol && ol->size() > before) {
                const auto& vec = ol->getOrders();      // const std::vector<Order*>&
                if (!vec.empty()) {
                    Order* justAdded = vec.back();       // last enqueued order
                    if (justAdded && justAdded->name() != "Deploy") {
                        nonDeployIssued[i] = true;
                    }
                }
            }
        }

        // Safety: if something is still producing orders endlessly without progress,
        // break out to avoid infinite phase.
        if (++safetyCounter > safetyLimit) {
            std::cout << "[Warn] Issue Orders safety limit reached; breaking out.\n";
            break;
        }

        // Repeat another pass only if at least one player created an order this pass.
    } while (issuedInPass);
}


/**
 * @brief Execute orders phase where players' orders are executed in round-robin fashion
 */
void GameEngine::executeOrdersPhase() {
    if (!players || players->empty()) {
        std::cout << "\n--- Execute Orders Phase skipped (no players) ---\n";
        return;
    }

    std::cout << "\n--- Execute Orders Phase ---\n";

    // ========= 1) Execute all DEPLOY orders first =========
    while (true) {
        bool executedAnyDeploy = false;

        for (Player* p : *players) {
            if (!p) continue;

            OrdersList* ol = p->getOrdersList();
            if (!ol) continue;

            // Try to remove one "Deploy" order from this player's list
            Order* deploy = ol->popFirstByName("Deploy");
            if (deploy) {
                std::cout << "[Deploy] " << *deploy << "\n";
                deploy->execute();
                delete deploy;
                executedAnyDeploy = true;
            }
        }

        // When no Deploy was found anywhere in this pass, we're done
        if (!executedAnyDeploy) {
            break;
        }
    }

    // ========= 2) Execute all remaining (non-deploy) orders round-robin =========
    bool executedAny = true;
    while (executedAny) {
        executedAny = false;

        for (Player* p : *players) {
            if (!p) continue;

            OrdersList* ol = p->getOrdersList();
            if (!ol || ol->empty()) continue;

            Order* o = ol->popfront();
            if (!o) continue;

            std::cout << "[Order] " << *o << "\n";
            o->execute();
            delete o;
            executedAny = true;
        }
    }

    // (Your existing removeDefeatedPlayers / checkWinCondition are called in mainGameLoop)
}






/**
 * @brief Remove defeated players who have no territories left
 */
void GameEngine::removeDefeatedPlayers() {
    if (!players) return;

    auto& vec = *players;
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
            [](Player* p) {
                if (!p) return true; // remove nulls
                if (p->getOwnedTerritories().empty()) {
                    std::cout << "Player " << p->getPlayerName()
                              << " has been eliminated (no territories).\n";
                    delete p;
                    return true;
                }
                return false;
            }),
        vec.end()
    );
}


/**
 * @brief Check for win condition: if a player has won the game
 * @param winner Reference to store pointer to winning player if found
 * @return true if there is a winner, false otherwise
 */
bool GameEngine::checkWinCondition(Player*& winner) const {
    winner = nullptr;

    if (!players || players->empty()) return false;

    // Simple rule: if only one player left, they win.
    if (players->size() == 1) {
        winner = (*players)[0];
        return winner != nullptr;
    }

    if (!gameMap) return false;

    // Alternative: all territories owned by the same player
    for (Territory* t : gameMap->getTerritories()) {
        if (!t) continue;
        Player* owner = t->getOwner();
        if (!owner) return false;

        if (!winner) {
            winner = owner;
        } else if (winner != owner) {
            return false; // more than one owner
        }
    }

    return winner != nullptr;
}

/**
 * @brief Main game loop managing the phases of the game
 */
void GameEngine::mainGameLoop() {
    if (!gameMap || !players || players->empty()) {
        std::cout << "Cannot start main game loop: map or players not initialized.\n";
        return;
    }

    std::cout << "\n===== MAIN GAME LOOP START =====\n";

    bool gameOver = false;
    int turn = 1;

    while (!gameOver) {
    std::cout << "\n===== TURN " << turn << " =====\n";

    reinforcementPhase();
    issueOrdersPhase();
    executeOrdersPhase();
    removeDefeatedPlayers();

    Player* winner = nullptr;
    if (checkWinCondition(winner)) {
        if (winner) {
            std::cout << "\n*** Player " << winner->getPlayerName()
                      << " wins the game! ***\n";
        } else {
            std::cout << "\n*** Game over (no winner). ***\n";
        }
        gameOver = true;
    }

    ++turn;
}


    std::cout << "===== MAIN GAME LOOP END =====\n";
}

/**
 * @brief Test hook to set internal map and players for demo/testing purposes
 * @param demoMap Pointer to the map to use
 * @param demoPlayers Pointer to the vector of players to use
 */
void GameEngine::setMapAndPlayersForDemo(Map* map, std::vector<Player*>* ps) {
    gameMap = map;
    players = ps;
}

// Helper: give ownership + armies and sync with player's territory list
static void ownTerritory(Player* p, Territory* t, int armies) {
    if (!p || !t) return;
    t->setOwner(p);
    t->setArmies(armies);
    p->addPlayerTerritory(t);
}

// === Required free function ===
void testMainGameLoop() {
    cout << "=============================================\n";
    cout << "          testMainGameLoop() - Part 3\n";
    cout << "=============================================\n\n";

    // ---------------------------------------------------------------------
    // Build a tiny demo map:
    //  - 1 continent (bonus 5)
    //  - 3 territories: Alaska, Northwest Territory, Alberta
    //  - fully adjacent so toAttack()/toDefend() have options
    // ---------------------------------------------------------------------
    Map* demoMap = new Map();

    Continent* cont = new Continent(1, "DemoContinent", 5);
    demoMap->addContinent(cont);

    Territory* alaska   = new Territory(1, "Alaska");
    Territory* nw       = new Territory(2, "Northwest Territory");
    Territory* alberta  = new Territory(3, "Alberta");

    cont->addTerritory(alaska);
    cont->addTerritory(nw);
    cont->addTerritory(alberta);

    demoMap->addTerritory(alaska);
    demoMap->addTerritory(nw);
    demoMap->addTerritory(alberta);

    // Make them all mutually adjacent
    alaska->addAdjacent(nw);
    alaska->addAdjacent(alberta);
    nw->addAdjacent(alaska);
    nw->addAdjacent(alberta);
    alberta->addAdjacent(alaska);
    alberta->addAdjacent(nw);

    // ---------------------------------------------------------------------
    // Create 3 players
    // ---------------------------------------------------------------------
    Player* alpha   = new Player("Alpha");
    Player* bravo   = new Player("Bravo");
    Player* charlie = new Player("Charlie");

    std::vector<Player*>* players = new std::vector<Player*>();
    players->push_back(alpha);
    players->push_back(bravo);
    players->push_back(charlie);

    // One territory each for the first reinforcement demo
    ownTerritory(alpha, alaska, 5);
    ownTerritory(bravo, nw, 5);
    ownTerritory(charlie, alberta, 5);

    cout << "[Setup] Demo map with 3 territories created.\n";
    cout << "[Setup] Alpha: Alaska | Bravo: Northwest Territory | Charlie: Alberta\n\n";

    // Hook them into the engine
    GameEngine engine;
    engine.setMapAndPlayersForDemo(demoMap, players);

    cout << "GameEngine initialized in Start state.\n";

    // ---------------------------------------------------------------------
    // (1) Reinforcement: case A – everyone has 1 territory
    // ---------------------------------------------------------------------
    cout << "=== (1a) Reinforcement: 1 territory each ===\n";
    cout << "Expected: each player gets 3 armies (minimum rule).\n\n";
    engine.reinforcementPhase(); // prints pools via GameEngine implementation

    // ---------------------------------------------------------------------
    // (1) Reinforcement: case B – Alpha owns whole continent
    // ---------------------------------------------------------------------
    // Reset pools so math is obvious
    alpha->setReinforcementPool(0);
    bravo->setReinforcementPool(0);
    charlie->setReinforcementPool(0);

    // Move territories to Alpha
    bravo->removePlayerTerritory(nw);
    charlie->removePlayerTerritory(alberta);
    ownTerritory(alpha, nw, 5);
    ownTerritory(alpha, alberta, 5);

    cout << "\n=== (1b) Reinforcement: Alpha owns all territories ===\n";
    cout << "Expected: Alpha gets max(3, 3/3) + 5 = 8 armies.\n\n";
    engine.reinforcementPhase();

    // ---------------------------------------------------------------------
    // (4) Card play: Alpha plays Bomb to create an order
    // ---------------------------------------------------------------------
    cout << "\n[Cards] Alpha receives a Bomb card.\n";
    Deck tempDeck;
    Card* bomb = new Card(Card::typeOfCard::Bomb);
    alpha->getPlayerHand()->addCard(bomb);
    cout << "[Cards] Alpha plays Bomb to create an order via Card::play().\n";
    bomb->play(*alpha, tempDeck, *alpha->getPlayerHand());

    // NOTE:
    //  - issueOrder() (GameEngine::issueOrdersPhase) respects:
    //      (2) Only Deploy while reinforcementPool > 0
    //      (3) Advance using toAttack()/toDefend()
    //    so we just run the loop and let your Player logic demonstrate it.

    // Reset pools so main loop behavior is clean
    alpha->setReinforcementPool(0);
    bravo->setReinforcementPool(0);
    charlie->setReinforcementPool(0);

    cout << "\n=== Running mainGameLoop() to demonstrate (2)-(6) ===\n\n";

    // mainGameLoop will:
    //  - (1) call reinforcementPhase() each turn
    //  - (2) call issueOrdersPhase() (round-robin, deploy-while-pool rule)
    //  - (3) call executeOrdersPhase() (deploys first, then others)
    //  - (5) remove players with 0 territories
    //  - (6) detect when one player owns all territories (Alpha) and end
    engine.mainGameLoop();

    cout << "\n=============================================\n";
    cout << "   End of testMainGameLoop() demonstration\n";
    cout << "=============================================\n\n";
}
