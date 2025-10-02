#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include <algorithm>
#include <iostream>


//Player

Player::Player() : playerName("defaultName"), playerHand(), ownedTerritories(), orders_() {}

Player::Player(const Player& copyPlayer)
	: playerName(copyPlayer.playerName),
	playerHand(copyPlayer.playerHand),
	ownedTerritories(copyPlayer.ownedTerritories),
	orders_(copyPlayer.orders_) {
}

Player& Player::operator=(const Player& copyPlayer) {
    if (this != &copyPlayer) {
        playerName = copyPlayer.playerName;
        playerHand = copyPlayer.playerHand;
        ownedTerritories = copyPlayer.ownedTerritories;
        delete orders_;
        orders_ = new OrdersList(*copyPlayer.orders_);
    }
    return *this;
}

Player::Player(std::string name) : playerName(name), playerHand(), ownedTerritories(), orders_() {}

Player::~Player() {
    playerHand.clear();
    ownedTerritories.clear();
    delete orders_;
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
    return ownedTerritories;
}

//Checks adjacent territories to owned ones and returns a vector with the attackable territories
std::vector<Territory*> Player::toAttack() {
    std::vector<Territory*> attackList;
    for (Territory* mine : ownedTerritories) {
        for (Territory* adj : mine->getAdjacents()) {
            if (adj->getOwner() != this &&
                std::find(attackList.begin(), attackList.end(), adj) == attackList.end()) {
                attackList.push_back(adj);
            }
        }
    }
    return attackList;
}

//Cards
void Player::addCard(Card* card) {
    playerHand.push_back(card);
}

void Player::removeCard(Card* card) {
    auto it = std::find(playerHand.begin(), playerHand.end(), card);
    if (it != playerHand.end())
        playerHand.erase(it);
}

//Orders

void Player::issueOrder(Order* orderIssued) {
    orders_->add(orderIssued);
}

//Debug / Print

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player: " << player.getPlayerName() << "\nOwned Territories: ";
    for (auto t : player.getOwnedTerritories())
        os << t->getName() << " ";
    os << "\n";
    return os;
}