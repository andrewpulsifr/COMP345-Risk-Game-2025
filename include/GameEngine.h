#pragma once
#include <string>
#include <map>
#include <utility>

// Simple command object (you can extend later with args, etc.)
struct Command {
    std::string name;
    explicit Command(std::string n) : name(std::move(n)) {}
};

enum class GameState {
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders,
    ExecuteOrders,
    Win
};

class GameEngine {
public:
    GameEngine();

    // Returns true if the command was valid (and possibly changed state).
    bool handleCommand(const Command& cmd);

    GameState state() const { return current_; }
    std::string stateName() const;

private:
    GameState current_;
    std::map<std::pair<GameState, std::string>, GameState> transitions_;

    void setState(GameState s);
};
