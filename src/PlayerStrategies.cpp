#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <iostream>
#include <algorithm>


// ====================== AggressivePlayerStrategy =======================

// ========== PlayerStrategy base implementations ==========
PlayerStrategy::PlayerStrategy() : player_(nullptr) {}
PlayerStrategy::PlayerStrategy(Player* player) : player_(player) {}
PlayerStrategy::~PlayerStrategy() = default;

PlayerStrategy::PlayerStrategy(const PlayerStrategy& other) {
    // Do not copy the player pointer; the owning Player will set this when cloning
    player_ = nullptr;
}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& other) {
    if (this != &other) {
        player_ = nullptr; // avoid sharing player pointers between strategy instances
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const PlayerStrategy& ps) {
    (void)ps;
    os << "PlayerStrategy";
    return os;
}

Player* PlayerStrategy::getPlayer() const { return player_; }
void PlayerStrategy::setPlayer(Player* player) { player_ = player; }


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

/** Return territories sorted by army count (descending) - strongest first
 Strategy: Focus on strongest territory for defense */
std::vector<Territory*> AggressivePlayerStrategy::toDefend() {
    std::vector<Territory*> defendList = player_->getOwnedTerritories();
    std::sort(defendList.begin(), defendList.end(), [](Territory* a, Territory* b) {
        return a->getArmies() > b->getArmies();
    });
    return defendList;
}

/** Returns all adjacent enemy territories.
 * Strategy: Attack any/all reachable enemies. */
std::vector<Territory*> AggressivePlayerStrategy::toAttack() {
    std::vector<Territory*> defendList = toDefend();
    std::vector<Territory*> attackList;
    if (defendList.empty()) {
        return attackList;
    }
    Territory* strongest = defendList.front();
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
    
    std::vector<Territory*> defendList = toDefend();
    if (defendList.empty()) {
        return false;
    }
    Territory* strongest = defendList.front();
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

void BenevolentPlayerStrategy::resetForNewRound() {
    handShownThisRound_ = false;
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
    if (!player_) {
        if (orderIssued) delete orderIssued;
        return false;
    }

    if (!orderIssued) return issueOrder();

    // Accept only defensive-type orders created by cards: Deploy, Blockade, Airlift, Negotiate
    std::string oname = orderIssued->name();
    if (oname == "Deploy") {
        // Card-created Deploy orders may not respect the reinforcementPool check in
        // DeployOrder::validate() (cards can generate deploys independently). For
        // the purposes of card-created orders, accept Deploy orders targeted at
        // this player and add them to the OrdersList without requiring validate()
        // to succeed on reinforcement pool.
        player_->getOrdersList()->add(orderIssued);
        return true;
    }
    if (oname == "Blockade" || oname == "Airlift" || oname == "Negotiate") {
        if (orderIssued->validate()) {
            player_->getOrdersList()->add(orderIssued);
            return true;
        } else {
            delete orderIssued;
            return false;
        }
    }

    // Otherwise reject offensive orders (e.g., Bomb, Advance) for Benevolent
    delete orderIssued;
    return false;
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
    if (!player_) return std::vector<Territory*>();
    return player_->getOwnedTerritories();
}

/** TODO: Return all adjacent enemy territories (human decides which to attack)
 Strategy: Present all attackable options to user */
std::vector<Territory*> HumanPlayerStrategy::toAttack() {
    std::vector<Territory*> attackList;
    if (!player_) return attackList;
    for (Territory* mine : player_->getOwnedTerritories()) {
        if (!mine) continue;
        for (Territory* adj : mine->getAdjacents()) {
            if (!adj) continue;
            Player* owner = adj->getOwner();
            if (owner != player_ && owner != nullptr &&
                std::find(attackList.begin(), attackList.end(), adj) == attackList.end()) {
                attackList.push_back(adj);
            }
        }
    }
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
    if (!player_) return false;

    // Helper to read an integer from stdin with validation
    auto readInt = [](int minVal, int maxVal) -> int {
        int choice;
        while (true) {
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::string dummy;
                std::getline(std::cin, dummy);
                std::cout << "Invalid input. Enter a number: ";
                continue;
            }
            if (choice < minVal || choice > maxVal) {
                std::cout << "Please enter a number in range [" << minVal << ", " << maxVal << "]: ";
                continue;
            }
            return choice;
        }
    };

    // Show basic menu
    std::cout << "\n[Human] " << player_->getPlayerName() << " - Available actions:\n";
    int pool = player_->getReinforcementPool();
    if (pool > 0) {
        std::cout << " 1) Deploy (reinforcements available: " << pool << ")\n";
        std::cout << " 2) View hand\n";
        std::cout << " 3) End turn (skip)\n";
        std::cout << "Choose an action (1-3): ";
        int action = readInt(1,3);

        if (action == 3) return false; // end this issuing pass

        if (action == 2) {
            Hand* h = player_->getPlayerHand();
            if (h) h->showHand();
            return true;
        }

        // Deploy
        auto owned = player_->getOwnedTerritories();
        if (owned.empty()) return false;
        std::cout << "Select territory to deploy to:\n";
        for (size_t i = 0; i < owned.size(); ++i) {
            Territory* t = owned[i];
            std::cout << " " << (i+1) << ") " << (t ? t->getName() : std::string("<null>"))
                      << " (armies=" << (t ? t->getArmies() : 0) << ")\n";
        }
        std::cout << "Choice: ";
        int idx = readInt(1, (int)owned.size()) - 1;
        Territory* target = owned[idx];
        std::cout << "Enter number of armies to deploy (1-" << pool << "): ";
        int amt = readInt(1, pool);
        Order* deploy = new DeployOrder(player_, target, amt);
        if (deploy->validate()) {
            player_->getOrdersList()->add(deploy);
            player_->subtractFromReinforcementPool(amt);
            std::cout << "Issued Deploy(" << amt << " on " << target->getName() << ")\n";
            return true;
        }
        delete deploy;
        std::cout << "Invalid Deploy order.\n";
        return false;
    }

    // When no reinforcements left, present more actions
    std::cout << " 1) Advance\n";
    std::cout << " 2) Play Card\n";
    std::cout << " 3) View hand\n";
    std::cout << " 4) End turn (skip)\n";
    std::cout << "Choose an action (1-4): ";
    int action = readInt(1,4);
    if (action == 4) return false;

    if (action == 3) {
        Hand* h = player_->getPlayerHand();
        if (h) h->showHand();
        return true;
    }

    if (action == 1) {
        // Advance: choose source (owned with armies>1)
        std::vector<Territory*> sources;
        for (Territory* t : player_->getOwnedTerritories()) if (t && t->getArmies() > 1) sources.push_back(t);
        if (sources.empty()) {
            std::cout << "No territories with movable armies.\n";
            return false;
        }
        std::cout << "Select source territory:\n";
        for (size_t i = 0; i < sources.size(); ++i) {
            std::cout << " " << (i+1) << ") " << sources[i]->getName() << " (armies=" << sources[i]->getArmies() << ")\n";
        }
        std::cout << "Choice: ";
        int sidx = readInt(1, (int)sources.size()) - 1;
        Territory* source = sources[sidx];

        // List adjacents
        auto adjs = source->getAdjacents();
        std::vector<Territory*> validTargets;
        for (Territory* a : adjs) if (a) validTargets.push_back(a);
        if (validTargets.empty()) {
            std::cout << "No adjacent targets.\n";
            return false;
        }
        std::cout << "Select target territory:\n";
        for (size_t i = 0; i < validTargets.size(); ++i) {
            Territory* t = validTargets[i];
            std::string ownerName = t->getOwner() ? t->getOwner()->getPlayerName() : std::string("<none>");
            std::cout << " " << (i+1) << ") " << t->getName() << " (owner=" << ownerName << ", armies=" << t->getArmies() << ")\n";
        }
        std::cout << "Choice: ";
        int tidx = readInt(1, (int)validTargets.size()) - 1;
        Territory* target = validTargets[tidx];
        int maxMove = source->getArmies() - 1;
        std::cout << "Enter number of armies to advance (1-" << maxMove << "): ";
        int amt = readInt(1, maxMove);
        Order* adv = new AdvanceOrder(player_, source, target, amt);
        if (adv->validate()) {
            player_->getOrdersList()->add(adv);
            std::cout << "Issued Advance(" << amt << " from " << source->getName() << " to " << target->getName() << ")\n";
            return true;
        }
        delete adv;
        std::cout << "Invalid Advance order.\n";
        return false;
    }

    if (action == 2) {
        Hand* hand = player_->getPlayerHand();
        if (!hand) return false;
        auto cards = hand->getCardsOnHand();
        if (cards.empty()) {
            std::cout << "No cards in hand.\n";
            return false;
        }
        std::cout << "Select a card to play:\n";
        for (size_t i = 0; i < cards.size(); ++i) {
            std::cout << " " << (i+1) << ") " << cardToString(cards[i]->getCard()) << "\n";
        }
        std::cout << "Choice: ";
        int cidx = readInt(1, (int)cards.size()) - 1;
        Card* chosen = cards[cidx];
        if (!chosen) return false;

        switch (chosen->getCard()) {
            case Card::Bomb: {
                // choose target among attackable territories
                auto attackable = toAttack();
                if (attackable.empty()) {
                    std::cout << "No valid enemy targets for Bomb.\n";
                    return false;
                }
                std::cout << "Select target to bomb:\n";
                for (size_t i = 0; i < attackable.size(); ++i)
                    std::cout << " " << (i+1) << ") " << attackable[i]->getName() << "\n";
                std::cout << "Choice: ";
                int targ = readInt(1, (int)attackable.size()) - 1;
                Order* bomb = new BombOrder(player_, attackable[targ]);
                if (bomb->validate()) {
                    player_->getOrdersList()->add(bomb);
                    hand->removeCard(chosen);
                    delete chosen;
                    std::cout << "Played Bomb on " << attackable[targ]->getName() << "\n";
                    return true;
                }
                delete bomb;
                std::cout << "Bomb order invalid.\n";
                return false;
            }
            case Card::Blockade: {
                auto owned = player_->getOwnedTerritories();
                if (owned.empty()) return false;
                std::cout << "Select owned territory for Blockade:\n";
                for (size_t i = 0; i < owned.size(); ++i)
                    std::cout << " " << (i+1) << ") " << owned[i]->getName() << "\n";
                std::cout << "Choice: ";
                int tid = readInt(1, (int)owned.size()) - 1;
                Order* block = new BlockadeOrder(player_, owned[tid]);
                if (block->validate()) {
                    player_->getOrdersList()->add(block);
                    hand->removeCard(chosen);
                    delete chosen;
                    std::cout << "Played Blockade on " << owned[tid]->getName() << "\n";
                    return true;
                }
                delete block;
                std::cout << "Blockade invalid.\n";
                return false;
            }
            case Card::Airlift: {
                auto owned = player_->getOwnedTerritories();
                std::vector<Territory*> sources;
                for (Territory* t : owned) if (t && t->getArmies() > 1) sources.push_back(t);
                if (sources.empty()) { std::cout << "No valid source for Airlift.\n"; return false; }
                std::cout << "Select source for Airlift:\n";
                for (size_t i = 0; i < sources.size(); ++i)
                    std::cout << " " << (i+1) << ") " << sources[i]->getName() << " (armies=" << sources[i]->getArmies() << ")\n";
                std::cout << "Choice: ";
                int s = readInt(1, (int)sources.size()) - 1;
                Territory* src = sources[s];
                std::cout << "Select destination (owned territory):\n";
                for (size_t i = 0; i < owned.size(); ++i)
                    std::cout << " " << (i+1) << ") " << owned[i]->getName() << "\n";
                std::cout << "Choice: ";
                int d = readInt(1, (int)owned.size()) - 1;
                Territory* dst = owned[d];
                int maxMove = src->getArmies() - 1;
                std::cout << "Enter number of armies to airlift (1-" << maxMove << "): ";
                int amt = readInt(1, maxMove);
                Order* air = new AirliftOrder(player_, src, dst, amt);
                if (air->validate()) {
                    player_->getOrdersList()->add(air);
                    hand->removeCard(chosen);
                    delete chosen;
                    std::cout << "Played Airlift from " << src->getName() << " to " << dst->getName() << "\n";
                    return true;
                }
                delete air;
                std::cout << "Airlift invalid.\n";
                return false;
            }
            case Card::Diplomacy: {
                // Choose an adjacent enemy player to negotiate with
                std::vector<Player*> candidates;
                for (Territory* mine : player_->getOwnedTerritories()) {
                    for (Territory* adj : mine->getAdjacents()) {
                        if (!adj) continue;
                        Player* other = adj->getOwner();
                        if (other && other != player_ && std::find(candidates.begin(), candidates.end(), other) == candidates.end())
                            candidates.push_back(other);
                    }
                }
                if (candidates.empty()) { std::cout << "No adjacent players to negotiate with.\n"; return false; }
                std::cout << "Select player to negotiate with:\n";
                for (size_t i = 0; i < candidates.size(); ++i)
                    std::cout << " " << (i+1) << ") " << candidates[i]->getPlayerName() << "\n";
                std::cout << "Choice: ";
                int pidx = readInt(1, (int)candidates.size()) - 1;
                Order* neg = new NegotiateOrder(player_, candidates[pidx]);
                if (neg->validate()) {
                    player_->getOrdersList()->add(neg);
                    hand->removeCard(chosen);
                    delete chosen;
                    std::cout << "Played Diplomacy with " << candidates[pidx]->getPlayerName() << "\n";
                    return true;
                }
                delete neg;
                std::cout << "Diplomacy invalid.\n";
                return false;
            }
            case Card::Reinforcement: {
                // Treat as a deploy card: choose an owned territory and amount
                auto owned = player_->getOwnedTerritories();
                if (owned.empty()) return false;
                std::cout << "Select territory to reinforce:\n";
                for (size_t i = 0; i < owned.size(); ++i)
                    std::cout << " " << (i+1) << ") " << owned[i]->getName() << "\n";
                std::cout << "Choice: ";
                int tid = readInt(1, (int)owned.size()) - 1;
                std::cout << "Enter number of armies to deploy with card (>=1): ";
                int amt = readInt(1, 1000000);
                Order* d = new DeployOrder(player_, owned[tid], amt);
                if (d->validate()) {
                    player_->getOrdersList()->add(d);
                    hand->removeCard(chosen);
                    delete chosen;
                    std::cout << "Played Reinforcement deploying " << amt << " to " << owned[tid]->getName() << "\n";
                    return true;
                }
                delete d;
                std::cout << "Reinforcement invalid.\n";
                return false;
            }
            default:
                break;
        }
    }

    return false;
}

bool HumanPlayerStrategy::issueOrder(Order* orderIssued) {
    if (!player_) {
        if (orderIssued) delete orderIssued;
        return false;
    }
    if (!orderIssued) return issueOrder();

    // Validate and accept the externally-created order when possible
    if (orderIssued->validate()) {
        player_->getOrdersList()->add(orderIssued);
        return true;
    }
    delete orderIssued;
    return false;
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

CheaterPlayerStrategy::CheaterPlayerStrategy() : PlayerStrategy(), actedThisRound_(false) {}
CheaterPlayerStrategy::~CheaterPlayerStrategy() = default;

CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other)
    : PlayerStrategy(other) {
    // Copy Cheater-specific flag
    this->actedThisRound_ = other.actedThisRound_;
}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    if (this != &other) {
        PlayerStrategy::operator=(other);
        this->actedThisRound_ = other.actedThisRound_;
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

void CheaterPlayerStrategy::resetForNewRound() {
    actedThisRound_ = false;
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
    // Only allow one automatic conquest per issuing-phase
    if (actedThisRound_) return false;
    auto targets = cheaterCollectTargets(player_);
    bool res = cheaterConquerTargets(player_, targets);
    if (res) actedThisRound_ = true;
    return res;
}

bool CheaterPlayerStrategy::issueOrder(Order* orderIssued) {
    (void)orderIssued;
    return issueOrder();
}
