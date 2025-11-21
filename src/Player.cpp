#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <algorithm>
#include <iostream>
#include <set>


//Player

// Default Constructor for Player.
Player::Player()
    : playerName("defaultName"),
      playerHand(new Hand()),
      ownedTerritories(),
      orders_(new OrdersList()), 
      cardAwardedThisTurn(false),
      negotiatedPlayers(),
      reinforcementPool(0)
      {}


// Deep Copy Constructor for Player.
Player::Player(const Player& copyPlayer)
    : playerName(copyPlayer.playerName),
      playerHand(new Hand(*copyPlayer.playerHand)),
      ownedTerritories(copyPlayer.ownedTerritories),
      orders_(new OrdersList(*copyPlayer.orders_)),
      cardAwardedThisTurn(copyPlayer.cardAwardedThisTurn),
      negotiatedPlayers(copyPlayer.negotiatedPlayers),
      reinforcementPool(copyPlayer.reinforcementPool)
{}


// Constructor with name parameter for Player.
Player::Player(std::string name)
    : playerName(std::move(name)),
      playerHand(new Hand()),
      ownedTerritories(),
      orders_(new OrdersList()),
      cardAwardedThisTurn(false),
      negotiatedPlayers(),
      reinforcementPool(0)
{}


/**
 * @brief Assignment operator
 * @param copyPlayer The player to copy from
 * @return Player& Reference to this player after assignment
 */
Player& Player::operator=(const Player& copyPlayer) {
    if (this != &copyPlayer) {
        playerName = copyPlayer.playerName;
        
        // Delete existing playerHand to prevent memory leak
        delete playerHand;
        playerHand = new Hand(*copyPlayer.playerHand);
        
        ownedTerritories = copyPlayer.ownedTerritories;
        // deep copy into existing list
        *orders_ = *copyPlayer.orders_;
        cardAwardedThisTurn = copyPlayer.cardAwardedThisTurn;
        negotiatedPlayers = copyPlayer.negotiatedPlayers;  
        reinforcementPool = copyPlayer.reinforcementPool;  }
    return *this;
}


// Destructor for Player.
Player::~Player() {
    delete playerHand;
    delete orders_;
}
// Neutral player instance
Player* neutralPlayer = nullptr;

// Getter for Player's Name.
std::string Player::getPlayerName() const {
    return playerName;
}

// Getter for Player's Hand.
Hand* Player::getPlayerHand() const {
    return playerHand;
}

// Card Awarded This Turn Setter 
void Player::setCardAwardedThisTurn(bool awarded) {
    cardAwardedThisTurn = awarded;
}

// Card Awarded This Turn Getter
bool Player::getCardAwardedThisTurn() const {
    return cardAwardedThisTurn;
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
// Negotiation Management
void Player::addNegotiatedPlayer(Player* p) { 
    negotiatedPlayers.insert(p); 
}

void Player::clearNegotiatedPlayers() {
    negotiatedPlayers.clear();
 }

bool Player::isNegotiatedWith(Player* p) const {
    return negotiatedPlayers.find(p) != negotiatedPlayers.end();
}

void Player::subtractFromReinforcementPool(int amt) { reinforcementPool -= amt; }

//Attack / Defend Lists

// Returns a player's attackable territory.
std::vector<Territory*> Player::toDefend() {
    return ownedTerritories;
}

/**
 * @brief Returns list of territories that can be attacked (adjacent enemy territories)
 * @return std::vector<Territory*> List of all adjacent enemy territories that can be attacked
 */
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

/**
 * @brief Issues a new order and adds it to the player's order list
 * @param orderIssued Pointer to the order being issued
 */
void Player::issueOrder(Order* orderIssued) {
    if (!orders_ || !orderIssued) return;
    orders_->add(orderIssued);
}


/**
 * Attempts to create exactly one order for this player in the current pass.
 *
 * Semantics:
 *  - Returns true  => An order WAS created and appended to this player's OrdersList
 *                     during THIS CALL.
 *  - Returns false => No order was created during THIS CALL (either no valid action
 *                     exists right now, or constraints prevent issuing).
 *
 * Part 3 Rules:
 *  - While reinforcementPool > 0, only Deploy orders may be created.
 *  - When reinforcementPool == 0, non-deploy orders (Advance, card-based) may be created.
 *
 * The round-robin loop in GameEngine will call issueOrder() once per player per pass,
 * and repeat passes until every player returns false in a pass.
 */
// Returns true iff THIS CALL created exactly one order and appended it to orders_.
// Returns false if no valid order could be issued on this pass.
// Part 3 semantics enforced:
//  - While reinforcementPool > 0: only Deploy orders may be created.
//  - When reinforcementPool == 0: may issue non-deploy orders (Advance, etc.).
bool Player::issueOrder() {
    // If I literally own nothing, I can't issue anything.
    if (ownedTerritories.empty()) {
        return false;
    }

    // ---------------------------
    // (A) DEPLOY-ONLY while pool > 0
    // ---------------------------
    if (reinforcementPool > 0) {
        // Prefer a defend target; otherwise any owned territory.
        std::vector<Territory*> defendList = toDefend();
        Territory* target = nullptr;

        if (!defendList.empty()) {
            target = defendList.front();
        } else {
            // Fallback: first owned territory
            target = ownedTerritories.front();
        }

        if (!target) {
            // No valid place to deploy this pass.
            return false;
        }

        // Simple heuristic: dump the whole pool this pass.
        const int deployAmount = reinforcementPool;
        if (deployAmount <= 0) {
            return false;
        }

        reinforcementPool = 0;

        Order* deployOrder = new DeployOrder(this, target, deployAmount);
        orders_->add(deployOrder);

        std::cout << "Player " << playerName << " issues Deploy("
                  << deployAmount << " on " << target->getName() << ")\n";
        return true;
    }

    // ---------------------------
    // (B) NON-DEPLOY orders when pool == 0
    // Try an offensive Advance first (attack a neighboring enemy).
    // ---------------------------
    for (Territory* src : ownedTerritories) {
        if (!src || src->getArmies() <= 1) continue;

        for (Territory* adj : src->getAdjacents()) {
            if (!adj) continue;
            if (adj->getOwner() != this) {
                int advanceAmount = src->getArmies() / 2; // simple heuristic
                if (advanceAmount <= 0) continue;

                Order* advanceOrder = new AdvanceOrder(this, src, adj, advanceAmount);
                orders_->add(advanceOrder);

                std::cout << "Player " << playerName << " issues Advance("
                          << advanceAmount << " from " << src->getName()
                          << " to " << adj->getName() << ")\n";
                return true;
            }
        }
    }

    // ---------------------------
    // (C) If no enemy neighbors, try a defensive Advance (redistribute).
    // ---------------------------
    for (Territory* src : ownedTerritories) {
        if (!src || src->getArmies() <= 1) continue;

        for (Territory* adj : src->getAdjacents()) {
            if (adj && adj->getOwner() == this) {
                int advanceAmount = src->getArmies() / 2;
                if (advanceAmount <= 0) continue;

                Order* advanceOrder = new AdvanceOrder(this, src, adj, advanceAmount);
                orders_->add(advanceOrder);

                std::cout << "Player " << playerName << " issues Advance("
                          << advanceAmount << " from " << src->getName()
                          << " to " << adj->getName() << ")\n";
                return true;
            }
        }
    }

    // Nothing to issue this pass.
    return false;
}


/**
 * @brief Issues the next order in the player's order list
 * @return bool True if an order was issued, false if no orders are available
 */
bool Player::hasOrders() const {
    return orders_ && !orders_->empty();
}

/**
 * @brief Gets the next order to be executed and removes it from the list
 * @return Order* Pointer to the next order, or nullptr if no orders are available
 */
Order* Player::popNextOrder() const {
    if (!orders_) return nullptr;
    return orders_->popfront();
}

/**
 * @brief Checks the next order to be executed without removing it
 * @return Order* Pointer to the next order, or nullptr if no orders are available
 */
Order* Player::checkNextOrder() const {
    if (!orders_) return nullptr;
    return orders_->front();
}

/**
 * @brief Getter for the player's OrdersList
 * @return OrdersList* Pointer to the player's OrdersList
 */
OrdersList* Player::getOrdersList() const {
    return orders_;
}

//Reinforcements

/**
* @brief Getter for reinforcementPool
* @return int The number of reinforcements in the player's pool
*/
int Player::getReinforcementPool() const {
    return reinforcementPool;
}

/**
* @brief Setter for reinforcementPool
* @param newPool The new number of reinforcements to set
*/
void Player::setReinforcementPool(int newPool) {
    reinforcementPool = newPool;
}

/**
 * @brief Adds reinforcements to the player's reinforcement pool
 * @param amount The number of reinforcements to add
 */
void Player::addReinforcements(int amount) {
    if (amount > 0){
        reinforcementPool += amount;
    }
}

/**
 * @brief Checks if the player has any territories
 * @return bool True if the player owns at least one territory
 */
bool Player::hasTerritories() const {
    return !ownedTerritories.empty();
}





//Debug / Print

// Stream overloading for Player.
std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Player: " << player.getPlayerName() << "\n  " << player.getOwnedTerritories().size() << " Owned Territories: ";
    for (Territory* territory : player.getOwnedTerritories()) {
        os << territory->getName() << " ";
    }
    os << "\n";
    return os;
}