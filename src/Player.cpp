#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <algorithm>
#include <iostream>


//Player

// Default Constructor for Player.
Player::Player()
    : playerName("defaultName"),
      playerHand(new Hand()),
      ownedTerritories(),
      orders_(new OrdersList()) {}


// Deep Copy Constructor for Player.
Player::Player(const Player& copyPlayer)
    : playerName(copyPlayer.playerName),
      playerHand(new Hand(*copyPlayer.playerHand)),
      ownedTerritories(copyPlayer.ownedTerritories),
      orders_(new OrdersList(*copyPlayer.orders_))
{}


// Constructor with name parameter for Player.
Player::Player(std::string name)
    : playerName(std::move(name)),
      playerHand(new Hand()),
      ownedTerritories(),
      orders_(new OrdersList()) {}


// @brief Assignment operator
// @param copyPlayer The player to copy from
// @return Player& Reference to this player after assignment
Player& Player::operator=(const Player& copyPlayer) {
    if (this != &copyPlayer) {
        playerName = copyPlayer.playerName;
        
        // Delete existing playerHand to prevent memory leak
        delete playerHand;
        playerHand = new Hand(*copyPlayer.playerHand);
        
        ownedTerritories = copyPlayer.ownedTerritories;
        // deep copy into existing list
        *orders_ = *copyPlayer.orders_;
    }
    return *this;
}


// Destructor for Player.
Player::~Player() {
    delete playerHand;
    delete orders_;
}


// Getter for Player's Name.
std::string Player::getPlayerName() const {
    return playerName;
}

// Getter for Player's Hand.
Hand* Player::getPlayerHand() const {
    return playerHand;
}


//Territory Management

// Add to a Player's owned territories.
void Player::addPlayerTerritory(Territory* territory) {
    ownedTerritories.push_back(territory);
    territory->setOwner(this);
}

// Remove a Player's territory.
void Player::removePlayerTerritory(Territory* territory) {
    std::vector<Territory*>::iterator it = std::find(ownedTerritories.begin(), ownedTerritories.end(), territory);
    if (it != ownedTerritories.end()) {
        ownedTerritories.erase(it);
        territory->setOwner(nullptr);
    }
}

// Getter for Player's Owned Territories.
std::vector<Territory*> Player::getOwnedTerritories() const {
    return ownedTerritories;
}

//Attack / Defend Lists

// Returns a player's attackable territory.
std::vector<Territory*> Player::toDefend() {
    return ownedTerritories;
}

// @brief Returns list of territories that can be attacked (adjacent enemy territories)
// @return std::vector<Territory*> List of all adjacent enemy territories that can be attacked
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


//Orders

// @brief Issues a new order and adds it to the player's order list
// @param orderIssued Pointer to the order being issued
void Player::issueOrder(Order* orderIssued) {
    if (!orders_ || !orderIssued) return;
    orders_->add(orderIssued);
}


//Debug / Print

// Stream overloading for Player.
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player: " << player.getPlayerName() << "\nOwned Territories: ";
    for (Territory* territory : player.getOwnedTerritories()) {
        os << territory->getName() << " ";
    }
    os << "\n";
    return os;
}