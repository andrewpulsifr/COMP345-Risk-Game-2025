#include "../include/GameEngine.h"
#include <iostream>

// --- helpers ---
static const char* toStr(GameState s) {
    switch (s) {
        case GameState::Start:               return "start";
        case GameState::MapLoaded:           return "map loaded";
        case GameState::MapValidated:        return "map validated";
        case GameState::PlayersAdded:        return "players added";
        case GameState::AssignReinforcement: return "assign reinforcement";
        case GameState::IssueOrders:         return "issue orders";
        case GameState::ExecuteOrders:       return "execute orders";
        case GameState::Win:                 return "win";
    }
    return "?";
}

std::string GameEngine::stateName() const { return toStr(current_); }

// --- ctor builds the transition table exactly like the diagram ---
GameEngine::GameEngine() : current_(GameState::Start) {
    // startup
    transitions_[{GameState::Start,        "loadmap"}]      = GameState::MapLoaded;
    transitions_[{GameState::MapLoaded,    "loadmap"}]      = GameState::MapLoaded;   // loop
    transitions_[{GameState::MapLoaded,    "validatemap"}]  = GameState::MapValidated;
    transitions_[{GameState::MapValidated, "addplayer"}]    = GameState::PlayersAdded;
    transitions_[{GameState::PlayersAdded, "addplayer"}]    = GameState::PlayersAdded; // loop
    transitions_[{GameState::PlayersAdded, "assigncountries"}] = GameState::AssignReinforcement;

    // play
    transitions_[{GameState::AssignReinforcement, "issueorder"}]   = GameState::IssueOrders;
    transitions_[{GameState::IssueOrders,         "issueorder"}]   = GameState::IssueOrders;     // loop while issuing
    transitions_[{GameState::IssueOrders,         "endissueorders"}] = GameState::ExecuteOrders;
    transitions_[{GameState::ExecuteOrders,       "execorder"}]    = GameState::ExecuteOrders;   // loop while executing
    transitions_[{GameState::ExecuteOrders,       "endexecorders"}]= GameState::AssignReinforcement;
    transitions_[{GameState::ExecuteOrders,       "win"}]          = GameState::Win;

    // after win
    transitions_[{GameState::Win, "play"}] = GameState::AssignReinforcement;
    transitions_[{GameState::Win, "end"}]  = GameState::Start;
}

void GameEngine::setState(GameState s) {
    if (s == current_) return;
    current_ = s;
    std::cout << ">> state -> " << toStr(current_) << "\n";
}

bool GameEngine::handleCommand(const Command& cmd) {
    auto key = std::make_pair(current_, cmd.name);
    auto it = transitions_.find(key);
    if (it == transitions_.end()) {
        std::cout << "[error] command \"" << cmd.name
                  << "\" not valid in state \"" << toStr(current_) << "\"\n";
        return false;
    }
    setState(it->second);
    return true;
}
