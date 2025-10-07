#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include <algorithm>
#include <iostream>


// @brief Default constructor, creates a player with name "defaultName"
Player::Player()
    : playerName("defaultName"),
      playerHand(),
      ownedTerritories(),
      orders_(new OrdersList()) {}

// @brief Copy constructor
// @param copyPlayer The player to copy from
Player::Player(const Player& copyPlayer)
    : playerName(copyPlayer.playerName),
      playerHand(copyPlayer.playerHand),
      ownedTerritories(copyPlayer.ownedTerritories),
      orders_(new OrdersList(*copyPlayer.orders_))
{}

// @brief Constructor with player name
// @param name The name of the player
Player::Player(std::string name)
    : playerName(std::move(name)),
      playerHand(),
      ownedTerritories(),
      orders_(new OrdersList()) {}


// @brief Assignment operator
// @param copyPlayer The player to copy from
// @return Player& Reference to this player after assignment
Player& Player::operator=(const Player& copyPlayer) {
    if (this != &copyPlayer) {
        playerName = copyPlayer.playerName;
        playerHand = copyPlayer.playerHand;
        ownedTerritories = copyPlayer.ownedTerritories;
        // deep copy into existing list
        *orders_ = *copyPlayer.orders_;
    }
    return *this;
}

// @brief Destructor that cleans up player's resources
Player::~Player() {
    playerHand.clear();
    ownedTerritories.clear();
    delete orders_;
    orders_ = nullptr;
}

// @brief Gets the name of the player
// @return std::string The player's name
std::string Player::getPlayerName() const {
    return playerName;
}

//Territory Management

// @brief Adds a territory to player's ownership and sets the territory's owner
// @param territory The territory to add to player's ownership
void Player::addPlayerTerritory(Territory* territory) {
    ownedTerritories.push_back(territory);
    territory->setOwner(this);
}

// @brief Removes a territory from player's ownership and clears its owner
// @param territory The territory to remove from player's ownership
void Player::removePlayerTerritory(Territory* territory) {
    std::vector<Territory*>::iterator it = std::find(ownedTerritories.begin(), ownedTerritories.end(), territory);
    if (it != ownedTerritories.end()) {
        ownedTerritories.erase(it);
        territory->setOwner(nullptr);
    }
}

// @brief Gets the list of territories owned by the player
// @return std::vector<Territory*> List of territories currently owned by this player
std::vector<Territory*> Player::getOwnedTerritories() const {
    return ownedTerritories;
}

//Attack / Defend Lists

// @brief Returns list of territories to defend (currently owned territories)
// @return std::vector<Territory*> List of all territories that need to be defended
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

//Cards

// @brief Adds a card to player's hand
// @param card Pointer to the card to add
void Player::addCard(Card* card) {
    playerHand.push_back(card);
}

// @brief Removes a card from player's hand
// @param card Pointer to the card to remove
void Player::removeCard(Card* card) {
    std::vector<Card*>::iterator it = std::find(playerHand.begin(), playerHand.end(), card);
    if (it != playerHand.end())
        playerHand.erase(it);
}

//Orders

// @brief Issues a new order and adds it to the player's order list
// @param orderIssued Pointer to the order being issued
void Player::issueOrder(Order* orderIssued) {
    if (!orders_ || !orderIssued) return;
    orders_->add(orderIssued);
}


//Debug / Print

// @brief Stream output operator for Player class
// @param os The output stream
// @param player The player to output
// @return std::ostream& Reference to the output stream
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player: " << player.getPlayerName() << "\nOwned Territories: ";
    for (Territory* territory : player.getOwnedTerritories()) {
        os << territory->getName() << " ";
    }
    os << "\n";
    return os;
}