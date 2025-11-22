#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <iostream>
#include <algorithm>


// ====================== AggressivePlayerStrategy =======================

/** Currently all functions are placeholders.
 * @TODO:  Actual implementation of all functions per the spec */


AggressivePlayerStrategy::AggressivePlayerStrategy() : PlayerStrategy() {}

AggressivePlayerStrategy::~AggressivePlayerStrategy() = default;

AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy any AggressivePlayerStrategy-specific members here if added in future
}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        // Copy any AggressivePlayerStrategy-specific members here if added in future
    }
    return *this;
}

PlayerStrategy* AggressivePlayerStrategy::clone() const {
    return new AggressivePlayerStrategy(*this);
}

std::ostream& operator<<(std::ostream& os, const AggressivePlayerStrategy& ps) {
    (void)ps;
    os << "AggressivePlayerStrategy";
    return os;
}

/** TODO: Return territories sorted by army count (descending) - strongest first
 Strategy: Focus on strongest territory for defense */
std::vector<Territory*> AggressivePlayerStrategy::toDefend() {
    // Dummy implementation - return all owned territories unsorted
    return player_->getOwnedTerritories();
}

/** TODO: Return all adjacent enemy territories
 Strategy: Attack any/all reachable enemies */
std::vector<Territory*> AggressivePlayerStrategy::toAttack() {
    // Dummy implementation - return empty list
    std::vector<Territory*> attackList;
    return attackList;
}

/** TODO: Deploy all armies to strongest territory, then advance to enemy territories
 Strategy: Maximize attack power by concentrating forces
 1. While reinforcementPool > 0: Deploy all to strongest territory
 2. When pool == 0: Advance from strongest to weakest adjacent enemy
 3. Use aggressive cards (Bomb, Airlift for attack) 
 Double check that the logic above matches spec */
bool AggressivePlayerStrategy::issueOrder() {
    // Dummy implementation - do nothing
    std::cout << "[AggressivePlayerStrategy] issueOrder() - NOT IMPLEMENTED\n";
    return false;
}

// Overload that accepts an Order pointer. Default behavior: ignore the provided
// order and delegate to the parameterless issueOrder(). This satisfies the
// pure-virtual signature in the header so concrete classes are not abstract.
bool AggressivePlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}

// ====================== BenevolentPlayerStrategy =======================

// Benevolent helpers (local to benevolent strategy)
static bool playDefensiveCardIfAvailable(Player* player) {
    if (!player) return false;
    Hand* hand = player->getPlayerHand();
    if (!hand) return false;

    auto cards = hand->getCardsOnHand();
    if (cards.empty()) return false;

    // Look for Blockade or Airlift or Diplomacy (in that priority)
    for (Card* card : cards) {
        if (!card) continue;
        switch (card->getCard()) {
            case Card::Blockade: {
                // target: weakest owned territory
                auto defendList = player->getOwnedTerritories();
                if (defendList.empty()) break;
                Territory* weakest = *std::min_element(defendList.begin(), defendList.end(),
                    [](Territory* a, Territory* b){ return a->getArmies() < b->getArmies(); });
                if (!weakest) break;
                Order* order = new BlockadeOrder(player, weakest);
                if (order->validate()) {
                    player->getOrdersList()->add(order);
                    hand->removeCard(card);
                    delete card; // return to deck is not available here; free to avoid leak
                    std::cout << "Benevolent plays Blockade on " << weakest->getName() << "\n";
                    return true;
                }
                delete order;
                break;
            }
            case Card::Airlift: {
                // choose a strong source and weak owned target
                auto owned = player->getOwnedTerritories();
                Territory* source = nullptr;
                Territory* target = nullptr;
                for (Territory* t : owned) {
                    if (!t) continue;
                    if (!source || t->getArmies() > source->getArmies()) source = t;
                    if (!target || t->getArmies() < target->getArmies()) target = t;
                }
                if (!source || !target || source == target) break;
                int amount = source->getArmies() - 1;
                if (amount <= 0) break;
                Order* order = new AirliftOrder(player, source, target, amount);
                if (order->validate()) {
                    player->getOrdersList()->add(order);
                    hand->removeCard(card);
                    delete card;
                    std::cout << "Benevolent plays Airlift from " << source->getName() << " to " << target->getName() << "\n";
                    return true;
                }
                delete order;
                break;
            }
            case Card::Diplomacy: {
                // Negotiate with first adjacent enemy player found
                for (Territory* mine : player->getOwnedTerritories()) {
                    for (Territory* adj : mine->getAdjacents()) {
                        if (!adj) continue;
                        Player* other = adj->getOwner();
                        if (other && other != player) {
                            Order* order = new NegotiateOrder(player, other);
                            if (order->validate()) {
                                player->getOrdersList()->add(order);
                                hand->removeCard(card);
                                delete card;
                                std::cout << "Benevolent plays Diplomacy with " << other->getPlayerName() << "\n";
                                return true;
                            }
                            delete order;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    return false;
}

static bool benevolentDeployPhase(Player* player) {
    if (!player) return false;
    if (player->getReinforcementPool() <= 0) return false;
    std::vector<Territory*> defendList = player->toDefend();
    if (defendList.empty()) return false;
    Territory* weakest = defendList.front();
    int deployAmount = player->getReinforcementPool();
    if (deployAmount <= 0) return false;
    Order* deployOrder = new DeployOrder(player, weakest, deployAmount);
    if (deployOrder->validate()) {
        player->getOrdersList()->add(deployOrder);
        player->subtractFromReinforcementPool(deployAmount);
        std::cout << "Player " << player->getPlayerName() << " issues Deploy(" << deployAmount
                  << " on " << weakest->getName() << ")\n";
        return true;
    }
    delete deployOrder;
    return false;
}

static bool benevolentRedistributePhase(Player* player) {
    if (!player) return false;
    std::vector<Territory*> owned = player->getOwnedTerritories();
    if (owned.size() <= 1) return false;
    Territory* source = nullptr;
    for (Territory* t : owned) {
        if (!t) continue;
        if (t->getArmies() > 1) {
            if (!source || t->getArmies() > source->getArmies()) source = t;
        }
    }
    if (!source) return false;
    Territory* target = nullptr;
    for (Territory* adj : source->getAdjacents()) {
        if (!adj) continue;
        if (adj->getOwner() == player) {
            if (!target || adj->getArmies() < target->getArmies()) target = adj;
        }
    }
    if (!target) return false;
    if (target->getArmies() >= source->getArmies()) return false;
    int advanceAmount = source->getArmies() - 1;
    if (advanceAmount <= 0) return false;
    Order* adv = new AdvanceOrder(player, source, target, advanceAmount);
    if (adv->validate()) {
        player->getOrdersList()->add(adv);
        std::cout << "Player " << player->getPlayerName() << " issues Advance(" << advanceAmount
                  << " from " << source->getName() << " to " << target->getName() << ")\n";
        return true;
    }
    delete adv;
    return false;
}

/** Currently all functions are placeholders.
 * @TODO:  Actual implementation of all functions per the spec */

BenevolentPlayerStrategy::BenevolentPlayerStrategy() : PlayerStrategy() {}

BenevolentPlayerStrategy::~BenevolentPlayerStrategy() = default;

BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy any BenevolentPlayerStrategy-specific members here if added in future
}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        // Copy any BenevolentPlayerStrategy-specific members here if added in future
    }
    return *this;
}

PlayerStrategy* BenevolentPlayerStrategy::clone() const {
    return new BenevolentPlayerStrategy(*this);
}

std::ostream& operator<<(std::ostream& os, const BenevolentPlayerStrategy& ps) {
    (void)ps;
    os << "BenevolentPlayerStrategy";
    return os;
}

/**
 * @brief Returns territories sorted by army count (ascending) - weakest first
 * Strategy: Focus on weakest territory for defense
 */
std::vector<Territory*> BenevolentPlayerStrategy::toDefend() {
    std::vector<Territory*> territories = player_->getOwnedTerritories();
    // Sort by army count (ascending) - weakest first
    std::sort(territories.begin(), territories.end(),
        [](Territory* a, Territory* b) {
            return a->getArmies() < b->getArmies();
        });
    return territories;
}

/** TODO: Return empty list (never attacks)
 Strategy: Benevolent never attacks enemy territories */
std::vector<Territory*> BenevolentPlayerStrategy::toAttack() {
    // Benevolent never attacks
    return std::vector<Territory*>();
}

/** 
 @brief Issue orders to reinforce weakest territories and avoid attacks
 Strategy: Reinforce weakest positions defensively
 1. While reinforcementPool > 0: Deploy all to weakest territory
 2. When pool == 0: Advance from strong territories to own weak territories
 3. May use cards defensively (Blockade, Diplomacy) but never to harm others
*/
bool BenevolentPlayerStrategy::issueOrder() {
    if (!player_) return false;

    // (A) Deploy phase
    if (player_->getReinforcementPool() > 0) {
        return benevolentDeployPhase(player_);
    }

    // (B) Try to play a defensive card first (if any)
    if (playDefensiveCardIfAvailable(player_)) return true;

    // (C) Defensive redistribution
    return benevolentRedistributePhase(player_);
}

bool BenevolentPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}

// ====================== NeutralPlayerStrategy =======================

NeutralPlayerStrategy::NeutralPlayerStrategy() : PlayerStrategy() {}

NeutralPlayerStrategy::~NeutralPlayerStrategy() = default;

NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy any NeutralPlayerStrategy-specific members here if added in future
}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        // Copy any NeutralPlayerStrategy-specific members here if added in future
    }
    return *this;
}

PlayerStrategy* NeutralPlayerStrategy::clone() const {
    return new NeutralPlayerStrategy(*this);
}

std::ostream& operator<<(std::ostream& os, const NeutralPlayerStrategy& ps) {
    (void)ps;
    os << "NeutralPlayerStrategy";
    return os;
}

/** TODO: Return all owned territories (no specific priority)
 Strategy: Neutral doesn't actively defend */
std::vector<Territory*> NeutralPlayerStrategy::toDefend() {
    // Dummy implementation - return all owned territories
    return player_->getOwnedTerritories();
}

/** TODO: Return empty list (never attacks)
 Strategy: Neutral never attacks anyone */
std::vector<Territory*> NeutralPlayerStrategy::toAttack() {
    // Neutral never attacks
    return std::vector<Territory*>();
}

/** TODO: Never issue any orders
 Strategy: Neutral does nothing (unless attacked, then becomes Aggressive)
 Always return false - neutral takes no actions */
bool NeutralPlayerStrategy::issueOrder() {
    // Neutral never issues orders
    return false;
}

bool NeutralPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}

// ====================== HumanPlayerStrategy =======================

/** Currently all functions are placeholders.
 * @TODO:  Actual implementation of all functions per the spec */

HumanPlayerStrategy::HumanPlayerStrategy() : PlayerStrategy() {}

HumanPlayerStrategy::~HumanPlayerStrategy() = default;

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy any HumanPlayerStrategy-specific members here if added in future
}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        // Copy any HumanPlayerStrategy-specific members here if added in future
    }
    return *this;
}

PlayerStrategy* HumanPlayerStrategy::clone() const {
    return new HumanPlayerStrategy(*this);
}

std::ostream& operator<<(std::ostream& os, const HumanPlayerStrategy& ps) {
    (void)ps;
    os << "HumanPlayerStrategy";
    return os;
}

/** TODO: Return all owned territories (human decides priority)
 Strategy: Present all options to user */
std::vector<Territory*> HumanPlayerStrategy::toDefend() {
    // Dummy implementation - return all owned territories
    return player_->getOwnedTerritories();
}

/** TODO: Return all adjacent enemy territories (human decides which to attack)
 Strategy: Present all attackable options to user */
std::vector<Territory*> HumanPlayerStrategy::toAttack() {
    // Dummy implementation - return empty list
    std::vector<Territory*> attackList;
    return attackList;
}

/** TODO: Prompt user for input and create orders based on user choices
 Strategy: Interactive order creation via console
 1. Display available actions (Deploy, Advance, Play Card, End Turn)
 2. Get user input for order type
 3. Get user input for order parameters (territories, army counts, etc.)
 4. Create and add order to player's order list
 5. Return true if order created, false if user chose to end turn */
bool HumanPlayerStrategy::issueOrder() {
    // Dummy implementation - do nothing
    std::cout << "[HumanPlayerStrategy] issueOrder() - NOT IMPLEMENTED\n";
    std::cout << "Player " << player_->getPlayerName() << " needs user input.\n";
    return false;
}

bool HumanPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}

// ====================== CheaterPlayerStrategy =======================

// Cheater helpers (local to cheater strategy)
static std::vector<Territory*> cheaterCollectTargets(Player* player) {
    std::vector<Territory*> toConquer;
    if (!player) return toConquer;
    for (Territory* mine : player->getOwnedTerritories()) {
        if (!mine) continue;
        for (Territory* adj : mine->getAdjacents()) {
            if (!adj) continue;
            if (adj->getOwner() != player) {
                if (std::find(toConquer.begin(), toConquer.end(), adj) == toConquer.end()) {
                    toConquer.push_back(adj);
                }
            }
        }
    }
    return toConquer;
}

static bool cheaterConquerTargets(Player* player, const std::vector<Territory*>& targets) {
    if (!player) return false;
    bool conqueredAny = false;
    for (Territory* t : targets) {
        if (!t) continue;
        Player* prev = t->getOwner();
        if (prev == player) continue;
        if (prev) prev->removePlayerTerritory(t);
        player->addPlayerTerritory(t);
        // Set a minimal occupying force so the territory isn't left empty.
        t->setArmies(1);
        std::cout << "[Cheater] " << player->getPlayerName() << " automatically conquers " << t->getName() << "\n";
        conqueredAny = true;
    }
    return conqueredAny;
}

CheaterPlayerStrategy::CheaterPlayerStrategy() : PlayerStrategy() {}

CheaterPlayerStrategy::~CheaterPlayerStrategy() = default;

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy any CheaterPlayerStrategy-specific members here if added in future
}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        // Copy any CheaterPlayerStrategy-specific members here if added in future
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const CheaterPlayerStrategy& ps) {
    (void)ps;
    os << "CheaterPlayerStrategy";
    return os;
}

PlayerStrategy* CheaterPlayerStrategy::clone() const {
    return new CheaterPlayerStrategy(*this);
}

/**  TODO: Return empty list (doesn't need to defend)
 Strategy: Cheater conquers everything automatically, no defense needed */
std::vector<Territory*> CheaterPlayerStrategy::toDefend() {
    // Cheater doesn't need to defend
    return std::vector<Territory*>();
}

/** TODO: Return empty list (doesn't attack normally)
 Strategy: Cheater automatically conquers adjacent territories */
std::vector<Territory*> CheaterPlayerStrategy::toAttack() {
    // Cheater doesn't use normal attack mechanism
    return std::vector<Territory*>();
}

/** @brief Issue orders to automatically conquer adjacent enemy territories
 Strategy: Iterate through owned territories, transfer ownership of all adjacent enemies
 1. For each owned territory, find all adjacent territories
 2. For each adjacent enemy territory, set owner to this player
 3. Add conquered territories to player's territory list
 4. Print conquest messages
 5. Return true if any territory was conquered, false otherwise */
bool CheaterPlayerStrategy::issueOrder() {
    if (!player_) return false;
    auto targets = cheaterCollectTargets(player_);
    return cheaterConquerTargets(player_, targets);
}

bool CheaterPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}
