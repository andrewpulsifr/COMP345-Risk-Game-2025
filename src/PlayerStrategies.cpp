#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include <iostream>
#include <algorithm>

// ====================== PlayerStrategy Base Class =======================

/** Currently all functions are placeholders.
 * @TODO: Determine if these must be implemented or if they can all be
 * defaulted. */

PlayerStrategy::PlayerStrategy() : player_(nullptr) {}

PlayerStrategy::~PlayerStrategy() = default;

PlayerStrategy::PlayerStrategy(Player* player) : player_(player) {}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) : player_(other.player_) {
    // Base class copy: shallow copy of the player pointer is sufficient here.
}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        player_ = other.player_;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const PlayerStrategy& strategy) {
    (void)strategy;
    os << "PlayerStrategy Unknown Type";
    return os;
}

Player* PlayerStrategy::getPlayer() const {
    return player_;
}

void PlayerStrategy::setPlayer(Player* player) {
    player_ = player;
}

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
    std::vector<Territory*> defendList = player_->getOwnedTerritories();
    std::sort(defendList.begin(), defendList.end(), [](Territory* a, Territory* b) {
        return a->getArmies() > b->getArmies();
    });
    return defendList;
}

/** TODO: Return all adjacent enemy territories
 Strategy: Attack any/all reachable enemies */
std::vector<Territory*> AggressivePlayerStrategy::toAttack() {
    Territory* strongest = toDefend().front();
    std::vector<Territory*> attackList;
    for (Territory* adj : strongest->getAdjacents()) {
        if (adj->getOwner() != player_) {
            attackList.push_back(adj);
        }
    }
    return attackList;
}

/**
 * @brief Deploy all available reinforcements to the strongest owned territory
 * 
 * @details This is the first priority action for aggressive players during the issuing orders phase.
 * Concentrates all reinforcement armies on the territory with the most armies to maximize
 * offensive power. Creates a DeployOrder and adds it to the player's orders list.
 * 
 * @return true if reinforcements were available and a deploy order was issued
 * @return false if no reinforcements are available in the reinforcement pool
 * 
 * @pre player_ must have at least one owned territory
 * @post Reinforcement pool is reduced to 0 if successful
 * @post A DeployOrder is added to the player's orders list if successful
 */
bool AggressivePlayerStrategy::deployToStrongest() {
    int numReinforcements = player_->getReinforcementPool();
    if (numReinforcements <= 0) {
        return false;
    }
    
    Territory* strongest = toDefend().front();
    DeployOrder* deployOrder = new DeployOrder(player_, strongest, numReinforcements);
    player_->getOrdersList()->add(deployOrder);
    player_->subtractFromReinforcementPool(numReinforcements);
    
    std::cout << "[AggressivePlayerStrategy] issueOrder() - DEPLOY logic executed.\n";
    return true;
}

/**
 * @brief Attack adjacent enemy territories from any owned territory with sufficient armies
 * 
 * @details This is the second priority action for aggressive players, executed after deployment.
 * Iterates through owned territories (prioritizing strongest first) and creates an AdvanceOrder
 * to attack the weakest adjacent enemy territory. This implements the aggressive strategy's
 * requirement to "always advance to enemy territories until it cannot do so anymore".
 * 
 * Strategy:
 * - Iterates through owned territories sorted by army count (strongest first)
 * - For each territory with at least 2 armies (must leave 1 behind)
 * - Finds the weakest adjacent enemy territory
 * - Creates an AdvanceOrder to attack with all available armies (armies - 1)
 * - Returns immediately after issuing one attack order
 * 
 * @return true if an attack order was issued
 * @return false if no owned territories have adjacent enemies or sufficient armies to attack
 * 
 * @pre player_ must own at least one territory
 * @post An AdvanceOrder to an enemy territory is added to the player's orders list if successful
 */
bool AggressivePlayerStrategy::attackAdjacentEnemies() {
    std::vector<Territory*> defendList = toDefend(); // Sorted strongest first
    
    if (defendList.empty()) {
        return false;
    }
    
    // Try to attack from any owned territory (prioritizing strongest)
    // "always advances to enemy territories until it cannot do so anymore"
    for (Territory* source : defendList) {
        if (source->getArmies() <= 1) {
            continue; // Need at least 2 armies to advance (must leave 1 behind)
        }
        
        // Find weakest adjacent enemy from this territory
        Territory* weakestEnemy = nullptr;
        int minEnemyArmies = INT_MAX;
        
        for (Territory* adj : source->getAdjacents()) {
            if (adj->getOwner() != player_ && adj->getOwner() != nullptr) {
                if (adj->getArmies() < minEnemyArmies) {
                    minEnemyArmies = adj->getArmies();
                    weakestEnemy = adj;
                }
            }
        }
        
        // If this territory has an adjacent enemy, attack it
        if (weakestEnemy) {
            AdvanceOrder* advanceOrder = new AdvanceOrder(
                player_, source, weakestEnemy, source->getArmies() - 1
            );
            player_->getOrdersList()->add(advanceOrder);
            std::cout << "[AggressivePlayerStrategy] Advancing from " << source->getName()
                      << " to attack " << weakestEnemy->getName() << "\n";
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Consolidate armies from weaker owned territories to the strongest territory
 * 
 * @details This is the third priority action for aggressive players, executed only when no
 * enemy territories are adjacent to any owned territories. Moves armies from weaker territories
 * to the strongest territory to concentrate forces for future attacks. Only moves armies from
 * territories that are directly adjacent to the strongest territory.
 * 
 * Strategy:
 * - Identifies the strongest owned territory (most armies)
 * - Iterates through other owned territories (weakest first after strongest)
 * - For each territory adjacent to the strongest with at least 2 armies
 * - Creates an AdvanceOrder to move all available armies (armies - 1) to strongest
 * - Returns immediately after issuing one consolidation order
 * 
 * @return true if a consolidation order was issued
 * @return false if no territories are adjacent to the strongest or have armies to move
 * 
 * @pre player_ must own at least two territories
 * @post An AdvanceOrder from a weaker territory to the strongest is added if successful
 */
bool AggressivePlayerStrategy::consolidateToStrongest() {
    std::vector<Territory*> defendList = toDefend(); // Sorted strongest first
    
    if (defendList.empty()) {
        return false;
    }
    
    Territory* strongest = defendList.front();
    for (size_t i = 1; i < defendList.size(); ++i) {
        Territory* source = defendList[i];
        
        // Check if source is adjacent to strongest and has armies to move
        if (source->isAdjacentTo(strongest) && source->getArmies() > 1) {
            AdvanceOrder* advanceOrder = new AdvanceOrder(
                player_, source, strongest, source->getArmies() - 1
            );
            player_->getOrdersList()->add(advanceOrder);
            std::cout << "[AggressivePlayerStrategy] Consolidating armies from " 
                      << source->getName() << " to strongest territory " 
                      << strongest->getName() << "\n";
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Issue orders following the aggressive player strategy
 * 
 * @details Implements the aggressive strategy as specified: "deploys or advances armies on its
 * strongest country, then always advances to enemy territories until it cannot do so anymore".
 * 
 * The strategy follows a three-priority approach:
 * 1. Deploy Phase: If reinforcements are available, deploy all to the strongest territory
 * 2. Attack Phase: If no reinforcements, attack adjacent enemy territories from any owned territory
 * 3. Consolidate Phase: If no attacks possible, consolidate armies to the strongest territory
 * 
 * This method is called repeatedly during the issuing orders phase until it returns false,
 * indicating no more orders can be issued.
 * 
 * @return true if an order was successfully issued (Deploy or Advance)
 * @return false if no valid orders can be issued this turn
 * 
 * @pre player_ must be set to a valid Player instance
 * @post An order (Deploy or Advance) is added to the player's orders list if return is true
 * 
 * @see deployToStrongest() for deployment logic
 * @see attackAdjacentEnemies() for attack logic
 * @see consolidateToStrongest() for consolidation logic
 */
bool AggressivePlayerStrategy::issueOrder() {
    // Priority 1: Deploy all reinforcements to strongest territory
    if (deployToStrongest()) {
        return true;
    }
    
    // Priority 2: Attack adjacent enemies from any territory (prioritize strongest)
    if (attackAdjacentEnemies()) {
        return true;
    }
    
    // Priority 3: Consolidate armies to strongest territory if no attacks possible
    // @note This code will likely never be hit because this would only happen if the player owns
    // all territories and thus has no adjacent enemies to attack. 
    // TODO: Confirm if this is desired behavior or if it should be moved before the attack.
    if (consolidateToStrongest()) {
        return true;
    }
    
    // No valid moves available
    return false;
}

/**
 * @brief Handle card-based orders for aggressive player strategy
 * 
 * @details Aggressive players only use cards with aggressive purposes:
 * - Bomb: Attack enemy territories
 * - Airlift: Move armies for offensive purposes
 * 
 * Rejects Reinforcement, Blockade, and Diplomacy cards as they are not aggressive.
 * For non-card orders, delegates to the parameterless issueOrder().
 * 
 * @param orderIssued The order created by playing a card
 * @return true if the order was accepted and added to the player's orders list
 * @return false if the order was rejected (non-aggressive card) or delegated
 */
bool AggressivePlayerStrategy::issueOrder(Order* orderIssued) {
    if (!orderIssued) {
        return issueOrder();
    }
    
    // Check if this is an aggressive card order
    BombOrder* bombOrder = dynamic_cast<BombOrder*>(orderIssued);
    AirliftOrder* airliftOrder = dynamic_cast<AirliftOrder*>(orderIssued);
    
    // Accept only Bomb and Airlift (aggressive cards)
    if (bombOrder || airliftOrder) {
        player_->getOrdersList()->add(orderIssued);
        return true;
    }
    
    // Reject non-aggressive cards (Blockade, Negotiate, Reinforcement)
    delete orderIssued; // Clean up rejected order
    return false;
}

// ====================== BenevolentPlayerStrategy =======================

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

/** TODO: Return territories sorted by army count (ascending) - weakest first
 Strategy: Protect weak territories */
std::vector<Territory*> BenevolentPlayerStrategy::toDefend() {
    // Dummy implementation - return all owned territories unsorted
    return player_->getOwnedTerritories();
}

/** TODO: Return empty list (never attacks)
 Strategy: Benevolent never attacks enemy territories */
std::vector<Territory*> BenevolentPlayerStrategy::toAttack() {
    // Benevolent never attacks
    return std::vector<Territory*>();
}

/** TODO: Deploy all armies to weakest territory, advance only to own weak territories
 Strategy: Reinforce weakest positions defensively
 1. While reinforcementPool > 0: Deploy all to weakest territory
 2. When pool == 0: Advance from strong territories to own weak territories
 3. May use cards defensively (Blockade, Diplomacy) but never to harm others
 Double check that the logic above matches spec */
bool BenevolentPlayerStrategy::issueOrder() {
    // Dummy implementation - do nothing
    std::cout << "[BenevolentPlayerStrategy] issueOrder() - NOT IMPLEMENTED\n";
    return false;
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

/**
 * @brief Handle card-based orders for neutral player strategy
 * 
 * @details Neutral players never use cards. All card-based orders are rejected.
 * According to spec: "never issues any order, nor uses any cards, though it may
 * have or receive cards".
 * 
 * @param orderIssued The order created by playing a card
 * @return false always - neutral players never issue orders or use cards
 */
bool NeutralPlayerStrategy::issueOrder(Order* orderIssued) {
    if (orderIssued) {
        std::cout << "[NeutralPlayerStrategy] Rejected card - neutral players never use cards.\n";
        delete orderIssued; // Clean up rejected order
    }
    return false; // Neutral never issues orders
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

/** TODO: Automatically conquer all adjacent enemy territories (once per turn)
 Strategy: Iterate through owned territories, transfer ownership of all adjacent enemies
 1. For each owned territory, find all adjacent territories
 2. For each adjacent enemy territory, set owner to this player
 3. Add conquered territories to player's territory list
 4. Print conquest messages
 5. Return true if any territory was conquered, false otherwise */
bool CheaterPlayerStrategy::issueOrder() {
    // Dummy implementation - do nothing
    std::cout << "[CheaterPlayerStrategy] issueOrder() - NOT IMPLEMENTED\n";
    return false;
}

bool CheaterPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}
