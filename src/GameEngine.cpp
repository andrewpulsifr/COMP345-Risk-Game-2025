#include "COMP345-Risk-Game-2025/include/GameEngine.h"
#include <iostream>

// Constructor: define valid transitions
GameEngine::GameEngine() {
    currentState = GameState::Start;

    // Startup transitions
    transitions[{GameState::Start, "loadmap"}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, "loadmap"}] = GameState::MapLoaded;
    transitions[{GameState::MapLoaded, "validatemap"}] = GameState::MapValidated;
    transitions[{GameState::MapValidated, "addplayer"}] = GameState::PlayersAdded;
    transitions[{GameState::PlayersAdded, "addplayer"}] = GameState::PlayersAdded;
    transitions[{GameState::PlayersAdded, "assigncountries"}] = GameState::AssignReinforcement;

    // Play transitions
    transitions[{GameState::AssignReinforcement, "issueorder"}] = GameState::IssueOrders;
    transitions[{GameState::IssueOrders, "issueorder"}] = GameState::IssueOrders;
    transitions[{GameState::IssueOrders, "endissueorders"}] = GameState::ExecuteOrders;
    transitions[{GameState::ExecuteOrders, "execorder"}] = GameState::ExecuteOrders;
    transitions[{GameState::ExecuteOrders, "endexecorders"}] = GameState::AssignReinforcement;
    transitions[{GameState::ExecuteOrders, "win"}] = GameState::Win;

    // From win, game can restart
    transitions[{GameState::Win, "play"}] = GameState::AssignReinforcement;
    transitions[{GameState::Win, "end"}] = GameState::Start;
}

void GameEngine::handleCommand(const std::string& command) {
    auto key = std::make_pair(currentState, command);

    if (transitions.find(key) != transitions.end()) {
        changeState(transitions[key]);
    } else {
        std::cout << "[ERROR] Invalid command \"" << command 
                  << "\" in current state.\n";
    }
}

GameState GameEngine::getCurrentState() const {
    return currentState;
}

void GameEngine::changeState(GameState newState) {
    currentState = newState;

    switch (newState) {
        case GameState::Start: std::cout << "State: Start\n"; break;
        case GameState::MapLoaded: std::cout << "State: Map Loaded\n"; break;
        case GameState::MapValidated: std::cout << "State: Map Validated\n"; break;
        case GameState::PlayersAdded: std::cout << "State: Players Added\n"; break;
        case GameState::AssignReinforcement: std::cout << "State: Assign Reinforcement\n"; break;
        case GameState::IssueOrders: std::cout << "State: Issue Orders\n"; break;
        case GameState::ExecuteOrders: std::cout << "State: Execute Orders\n"; break;
        case GameState::Win: std::cout << "State: Win\n"; break;
    }
}
