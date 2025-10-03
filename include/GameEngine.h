#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#include <string>
#include <map>

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
class GameEngine{

public:
   GameEngine();
void handleCommand(const std::string& command)
GameState getCurrentState() const;
private:
GameState currentState;
void changeState(GameState newState);

std::map<std::pair<GameState, std::string>, GameState> transition;
};
#endif
