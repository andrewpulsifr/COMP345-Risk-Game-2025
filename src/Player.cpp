#include "../include/Player.h"
#include "../include/Map.h"
#include <algorithm>
#include <iostream>


//Player

Player::Player() : playerName("defaultName"), /*playerHand(),*/ ownedTerritories(), playerOrders() {}

Player::Player(const Player& copyPlayer)
	: playerName(copyPlayer.playerName),
	// playerHand(copyPlayer.playerHand),
	ownedTerritories(copyPlayer.ownedTerritories),
	playerOrders(copyPlayer.playerOrders) {
}

Player& Player::operator=(const Player& copyPlayer) {
    if (this != &copyPlayer) {
        playerName = copyPlayer.playerName;
        // playerHand = copyPlayer.playerHand;
        ownedTerritories = copyPlayer.ownedTerritories;
        playerOrders = copyPlayer.playerOrders;
    }
    return *this;
}

Player::Player(std::string name) : playerName(name), /*playerHand(),*/ ownedTerritories(), playerOrders() {}

Player::~Player() {
    // playerHand.clear();
    ownedTerritories.clear();
    playerOrders.clear();
}

std::string Player::getPlayerName() const {
    return playerName;
}

//Territory Management

void Player::addPlayerTerritory(Territory* territory) {
    ownedTerritories.push_back(territory);
    territory->setOwner(this);
}

void Player::removePlayerTerritory(Territory* territory) {
    auto it = std::find(ownedTerritories.begin(), ownedTerritories.end(), territory);
    if (it != ownedTerritories.end()) {
        ownedTerritories.erase(it);
        territory->setOwner(nullptr);
    }
}

std::vector<Territory*> Player::getOwnedTerritories() const {
    return ownedTerritories;
}

//Attack / Defend Lists

std::vector<Territory*> Player::toDefend() {
        //TODO Implement toDefend() fully
    return ownedTerritories;
}

//Checks adjacent territories to owned ones and returns a vector with the attackable territories
std::vector<Territory*> Player::toAttack() {
    std::vector<Territory*> attackList;

    //TODO Implement toAttack() fully
    return attackList;
}

//TODO Implement Card Functionality, here's an Idea as to what itll probably look like once Person D does it
//Cards
// void Player::addCard(Card* card) {
//     playerHand.push_back(card);
// }

// void Player::removeCard(Card* card) {
//     auto it = std::find(playerHand.begin(), playerHand.end(), card);
//     if (it != playerHand.end())
//         playerHand.erase(it);
// }

//Orders

void Player::issueOrder(Order* orderIssued) {
    playerOrders.push_back(orderIssued);
}

//Debug / Print

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player: " << player.getPlayerName() << "\nOwned Territories: ";
    for (auto t : player.getOwnedTerritories())
        os << t->getName() << " ";
    os << "\n";
    return os;
}