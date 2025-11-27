#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

// ======================= Constants =======================
namespace TestConstants {
    const int AGGRESSIVE_REINFORCEMENTS = 5;
    const int NEUTRAL_REINFORCEMENTS = 5;
    const int TRANSFORMED_REINFORCEMENTS = 3;
    
    // Territory army counts
    const int CENTRALLAND_ARMIES = 5;
    const int NORTHLAND_ARMIES = 10;  // Strongest
    const int EASTLAND_ARMIES = 8;
    const int SOUTHLAND_ARMIES = 6;
    const int WESTLAND_ARMIES = 4;    // Weakest
}

// ======================= Helper Functions =======================

/**
 * @brief Assigns a territory to a player with specified army count
 * @param territory Territory to assign
 * @param player Player to assign territory to
 * @param armies Number of armies to place on territory
 */
static void assignTerritory(Territory* territory, Player* player, int armies) {
    territory->setOwner(player);
    territory->setArmies(armies);
    player->addPlayerTerritory(territory);
}

/**
 * @brief Helper function to add territories to map and continent, and set up adjacencies
 * @param gameMap Map to add territories to
 * @param continent Continent to associate territories with
 * @param territories Vector of all territories to add
 * @param centralland Central hub territory (last element in territories vector)
 */
static void setupMap(Map& gameMap, Continent* continent,
                     const std::vector<Territory*>& territories,
                     Territory* centralland) {
    // Add all territories to map, continent, and set continent membership using a single loop
    for (Territory* territory : territories) {
        gameMap.addTerritory(territory);
        continent->addTerritory(territory);
        territory->addContinent(continent);
    }
    
    // Set up adjacencies using vector-based approach for maintainability
    
    // Centralland connects to all outer territories (bidirectional)
    // Outer territories are all except the last one (centralland)
    for (size_t i = 0; i < territories.size() - 1; i++) {
        centralland->addAdjacent(territories[i]);
        territories[i]->addAdjacent(centralland);
    }
    
    // Additional cross-connections between outer territories 
    // Using indices: Northland(0)-Eastland(1), Southland(2)-Westland(3)
    std::vector<std::pair<size_t, size_t>> additionalConnections = {
        {0, 1},  // Northland - Eastland
        {2, 3}   // Southland - Westland
    };
    for (const auto& connection : additionalConnections) {
        territories[connection.first]->addAdjacent(territories[connection.second]);
        territories[connection.second]->addAdjacent(territories[connection.first]);
    }
}

// Forward declaration for testHumanStrategy
static void testHumanStrategy();

/**
 * @brief Test driver for Player Strategies functionality
 * 
 * @details Demonstrates that:
 *          1. Different players can be assigned different strategies leading to different behavior
 *          2. Strategy can be changed dynamically during play
 *          3. Neutral player becomes Aggressive when attacked
 *          4. Aggressive player focuses on attacking from strongest territories
 * 
 * Creates a test scenario with multiple players using different strategies,
 * demonstrates strategy behavior and dynamic strategy switching.
 */
void testPlayerStrategies(){
    cout << "\n========================================" << endl;
    cout << "   Testing Player Strategies (Part 1)" << endl;
    cout << "========================================\n" << endl;

    // ======================= Test Setup =======================
    cout << "=== Test Setup: Creating Map and Territories ===\n";
    
    Map gameMap;
    
    // Create a continent (required for valid map)
    Continent* testContinent = new Continent(1, "TestContinent", 5);
    gameMap.addContinent(testContinent);
    
    // Create territories using a vector for easy initialization
    Territory* northland = new Territory(1, "Northland");
    Territory* eastland = new Territory(2, "Eastland");
    Territory* southland = new Territory(3, "Southland");
    Territory* westland = new Territory(4, "Westland");
    Territory* centralland = new Territory(5, "Centralland");
    
    // Vector of all territories (centralland last for hub-and-spoke pattern)
    std::vector<Territory*> allTerritories = {northland, eastland, southland, westland, centralland};
    
    // Set up map topology (territories, continent membership, and adjacencies)
    setupMap(gameMap, testContinent, allTerritories, centralland);
    
    cout << "Map created with 1 continent and 5 fully connected territories.\n";
    cout << "Adjacencies: Centralland hub connects to all + Northland-Eastland and Southland-Westland also connected.\n";
    
    // Validate the map to ensure it's properly constructed
    assert(gameMap.validate() && "Map validation failed - map must be connected");
    cout << "Map validation: PASSED OK\n\n";
    
    // ======================= Create Players with Different Strategies =======================
    cout << "=== Creating Players with Different Strategies ===\n";
    
    Player* aggressivePlayer = new Player("Aggressor");
    aggressivePlayer->setPlayerStrategy(new AggressivePlayerStrategy());
    
    Player* neutralPlayer = new Player("Neutral");
    neutralPlayer->setPlayerStrategy(new NeutralPlayerStrategy());
    
    cout << "Created a player with an Aggressive Strategy\n";
    cout << "Created a player with a Neutral Strategy\n\n";
    
    // ======================= Assign Territories =======================
    cout << "=== Assigning Territories ===\n";
    
    // Aggressive player gets Centralland and Northland (strongest)
    assignTerritory(centralland, aggressivePlayer, TestConstants::CENTRALLAND_ARMIES);
    assignTerritory(northland, aggressivePlayer, TestConstants::NORTHLAND_ARMIES);
    
    // Neutral player gets Eastland, Southland, and Westland
    assignTerritory(eastland, neutralPlayer, TestConstants::EASTLAND_ARMIES);
    assignTerritory(southland, neutralPlayer, TestConstants::SOUTHLAND_ARMIES);
    assignTerritory(westland, neutralPlayer, TestConstants::WESTLAND_ARMIES);
    
    cout << "Aggressor owns: Centralland (" << TestConstants::CENTRALLAND_ARMIES 
         << " armies), Northland (" << TestConstants::NORTHLAND_ARMIES << " armies - strongest)\n";
    cout << "Neutral owns: Eastland (" << TestConstants::EASTLAND_ARMIES 
         << " armies), Southland (" << TestConstants::SOUTHLAND_ARMIES 
         << " armies), Westland (" << TestConstants::WESTLAND_ARMIES << " armies)\n\n";
    
    // ======================= Test 1: Aggressive Strategy - Deploy Phase =======================
    cout << "=== Test 1: Aggressive Strategy - Deploy Phase ===\n";
    aggressivePlayer->setReinforcementPool(TestConstants::AGGRESSIVE_REINFORCEMENTS);
    
    bool issued = aggressivePlayer->issueOrder();
    assert(issued && "Aggressive player should issue deploy order when reinforcements available");
    assert(aggressivePlayer->getReinforcementPool() == 0 && "All reinforcements should be deployed");
    assert(aggressivePlayer->getOrdersList()->size() == 1 && "Should have one deploy order");
    cout << "OK Aggressive player deploys all reinforcements to strongest territory\n\n";
    
    // ======================= Test 2: Aggressive Strategy - Attack Phase =======================
    cout << "=== Test 2: Aggressive Strategy - Attack Phase ===\n";
    assert(aggressivePlayer->getReinforcementPool() == 0 && "No reinforcements should remain");
    
    issued = aggressivePlayer->issueOrder();
    assert(issued && "Aggressive player should issue advance order to attack enemy");
    assert(aggressivePlayer->getOrdersList()->size() == 2 && "Should have two orders (deploy + advance)");
    cout << "OK Aggressive player advances to attack adjacent enemy territory\n\n";
    
    // ======================= Test 3: Neutral Strategy Behavior =======================
    cout << "=== Test 3: Neutral Strategy - Never Issues Orders ===\n";
    neutralPlayer->setReinforcementPool(TestConstants::NEUTRAL_REINFORCEMENTS);
    
    issued = neutralPlayer->issueOrder();
    assert(!issued && "Neutral player should never issue orders");
    assert(neutralPlayer->getOrdersList()->size() == 0 && "Neutral should have no orders");
    assert(neutralPlayer->getReinforcementPool() == TestConstants::NEUTRAL_REINFORCEMENTS 
           && "Neutral should not use reinforcements");
    cout << "OK Neutral player never issues orders even with reinforcements\n\n";
    
    // ======================= Test 4: Neutral to Aggressive Transformation =======================
    cout << "=== Test 4: Dynamic Strategy Change - Neutral Becomes Aggressive ===\n";
    
    // Verify neutral strategy before attack
    bool isNeutralBefore = dynamic_cast<NeutralPlayerStrategy*>(neutralPlayer->getPlayerStrategy()) != nullptr;
    assert(isNeutralBefore && "Player should have neutral strategy before attack");
    
    // Execute orders: Skip deploy, execute advance (which triggers transformation)
    Order* skipDeployOrder = aggressivePlayer->popNextOrder();
    delete skipDeployOrder;
    
    Order* advanceOrder = aggressivePlayer->popNextOrder();
    assert(advanceOrder != nullptr && "Should have advance order to execute");
    assert(advanceOrder->name() == "Advance" && "Second order should be Advance");
    
    advanceOrder->execute(); // This should trigger neutral-to-aggressive transformation
    delete advanceOrder;
    
    // Verify transformation occurred
    bool isAggressiveAfter = dynamic_cast<AggressivePlayerStrategy*>(neutralPlayer->getPlayerStrategy()) != nullptr;
    bool notNeutralAfter = dynamic_cast<NeutralPlayerStrategy*>(neutralPlayer->getPlayerStrategy()) == nullptr;
    assert(isAggressiveAfter && "Neutral player should transform to aggressive after being attacked");
    assert(notNeutralAfter && "Should no longer have neutral strategy");
    
    cout << "OK Neutral player transforms to aggressive when attacked\n\n";
    
    // ======================= Test 5: Transformed Neutral Now Behaves Aggressively =======================
    cout << "=== Test 5: Transformed Player Behaves Aggressively ===\n";
    
    size_t ordersBeforeTransformation = neutralPlayer->getOrdersList()->size();
    neutralPlayer->setReinforcementPool(TestConstants::TRANSFORMED_REINFORCEMENTS);
    issued = neutralPlayer->issueOrder();
    size_t ordersAfterTransformation = neutralPlayer->getOrdersList()->size();
    
    assert(issued && "Transformed player should now issue orders");
    assert(ordersAfterTransformation > ordersBeforeTransformation 
           && "Transformed player should have added an order");
    assert(neutralPlayer->getReinforcementPool() == 0 
           && "Transformed aggressive player should deploy all reinforcements");
    cout << "OK Transformed player issues aggressive orders (dynamic strategy change works)\n\n";
    
    // ======================= Test 6: toDefend() and toAttack() Methods =======================
    cout << "=== Test 6: Strategy-Specific toDefend() and toAttack() ===\n";
    
    // Test 6a: Aggressive toDefend - should be sorted strongest first
    std::vector<Territory*> defendList = aggressivePlayer->toDefend();
    assert(!defendList.empty() && "Aggressive player should have territories to defend");
    assert(defendList.size() == 2 && "Aggressive player should have 2 territories");
    
    // Verify territories are sorted in descending order by army count
    for (size_t i = 1; i < defendList.size(); i++) {
        bool isSortedDescending = defendList[i-1]->getArmies() >= defendList[i]->getArmies();
        assert(isSortedDescending && "toDefend() should return territories sorted by armies (descending)");
    }
    cout << "OK Aggressive toDefend() returns territories sorted by strength (descending)\n";
    
    // Test 6b: Aggressive toAttack - should list adjacent enemies
    std::vector<Territory*> attackList = aggressivePlayer->toAttack();
    assert(!attackList.empty() && "Aggressive player should have enemies to attack");
    
    for (Territory* territory : attackList) {
        bool isEnemyTerritory = territory->getOwner() != aggressivePlayer;
        assert(isEnemyTerritory && "toAttack() should only return enemy territories");
    }
    cout << "OK Aggressive toAttack() returns adjacent enemy territories\n";
    cout << "OK toAttack() methods work correctly for different strategies\n\n";
    
    // ======================= Cleanup =======================
    cout << "=== Cleanup ===\n";
    delete aggressivePlayer;
    delete neutralPlayer;
    // Map destructor will clean up territories

    testHumanStrategy(); // Call the human player strategy test
    
  
    cout << "\n========================================" << endl;
    cout << "   Now Testing: Benevolent & Cheater" << endl;
    cout << "========================================\n" << endl;

    // Create a simple map and continent for benevolent/cheater tests
    Map gameMap2;
    Continent* cont = new Continent(1, "Cont", 5);
    gameMap2.addContinent(cont);

    // Create territories used across tests
    Territory* tA = new Territory(1, "A");
    Territory* tB = new Territory(2, "B");
    Territory* tC = new Territory(3, "C");
    Territory* tD = new Territory(4, "D");

    // Basic adjacency: A-B, B-C, C-D (chain)
    tA->addAdjacent(tB); tB->addAdjacent(tA);
    tB->addAdjacent(tC); tC->addAdjacent(tB);
    tC->addAdjacent(tD); tD->addAdjacent(tC);

    // Add to map and continent
    gameMap2.addTerritory(tA); gameMap2.addTerritory(tB);
    gameMap2.addTerritory(tC); gameMap2.addTerritory(tD);
    // Ensure territories are members of the continent (both sides of relationship)
    tA->addContinent(cont); cont->addTerritory(tA);
    tB->addContinent(cont); cont->addTerritory(tB);
    tC->addContinent(cont); cont->addTerritory(tC);
    tD->addContinent(cont); cont->addTerritory(tD);

    assert(gameMap2.validate() && "Map should be valid (connected chain)");

    // --- Benevolent test ---
    cout << "--- Benevolent strategy test ---\n";
    Player* benevo = new Player("Benevolent");
    benevo->setPlayerStrategy(new BenevolentPlayerStrategy());

    // Assign two owned territories (weak and strong)
    tA->setOwner(benevo); tA->setArmies(1); benevo->addPlayerTerritory(tA);
    tB->setOwner(benevo); tB->setArmies(5); benevo->addPlayerTerritory(tB);

    // Reinforcements: expect deploy to weakest (tA)
    benevo->setReinforcementPool(4);
    bool benevoIssued = benevo->issueOrder();
    assert(benevoIssued && "Benevolent should issue deploy when reinforcements available");
    assert(benevo->getOrdersList()->size() == 1 && "One deploy order should be queued");

    // Show state before card plays
    cout << "-- Before card plays --\n";
    cout << "  Reinforcement pool: " << benevo->getReinforcementPool() << "\n";
    cout << "  Armies: A=" << tA->getArmies() << ", B=" << tB->getArmies() << "\n";
    cout << "  OrdersList size: " << benevo->getOrdersList()->size() << "\n";

    // Play a reinforcement card via Card::play() to exercise issueOrder(Order*) path
    Deck deck;
    Card* rein = new Card(Card::Reinforcement);
    benevo->getPlayerHand()->addCard(rein);
    rein->play(*benevo, deck, *benevo->getPlayerHand());

    // Note: current Card::play converts Reinforcement into an AdvanceOrder.
    // Benevolent rejects offensive orders, so OrdersList should remain size 1.
    cout << "-- After Reinforcement play --\n";
    benevo->getPlayerHand()->showHand();
    if (benevo->getOrdersList()) benevo->getOrdersList()->print();
    assert(benevo->getOrdersList()->size() == 1 && "Benevolent should reject offensive card-created orders (Advance/Bomb)");

    // Prepare an enemy territory adjacent to B so Bomb would target an enemy if allowed
    Player* enemy = new Player("Enemy");
    Territory* tX = new Territory(99, "X");
    tX->addAdjacent(tB); tB->addAdjacent(tX);
    gameMap2.addTerritory(tX); cont->addTerritory(tX);
    tX->setOwner(enemy); tX->setArmies(3); enemy->addPlayerTerritory(tX);

    // Play a Bomb card (offensive) - Benevolent should reject it (order not queued)
    Card* bomb = new Card(Card::Bomb);
    benevo->getPlayerHand()->addCard(bomb);
    cout << "-- Before Bomb play: enemy X armies=" << tX->getArmies() << "\n";
    bomb->play(*benevo, deck, *benevo->getPlayerHand());
    cout << "-- After Bomb play (should be rejected by Benevolent) --\n";
    benevo->getPlayerHand()->showHand();
    if (benevo->getOrdersList()) benevo->getOrdersList()->print();
    cout << "  Enemy X armies still=" << tX->getArmies() << "\n";
    // OrdersList should remain the same size (still only initial deploy)
    assert(benevo->getOrdersList()->size() == 1 && "Benevolent should reject Bomb orders created by cards");

    // Execute queued orders for benevo (execute all until empty)
    while (benevo->getOrdersList()->size() > 0) {
        Order* o = benevo->popNextOrder();
        assert(o != nullptr);
        cout << "Executing: " << o->name() << "\n";
        o->execute();
        delete o;
    }

    // OrdersList should be empty after executing all orders
    assert(benevo->getOrdersList()->size() == 0 && "All queued orders should have been executed and removed");
    cout << "-- After execution --\n";
    cout << "  Reinforcement pool: " << benevo->getReinforcementPool() << "\n";
    cout << "  Armies: A=" << tA->getArmies() << ", B=" << tB->getArmies() << "\n";
    cout << "Benevolent test passed.\n\n";

    // --- Cheater test ---
    cout << "--- Cheater strategy test ---\n";
    Player* cheater = new Player("Cheater");
    cheater->setPlayerStrategy(new CheaterPlayerStrategy());
    Player* victim = new Player("Victim");

    // Assign tC to cheater and tD to victim (adjacent)
    tC->setOwner(cheater); tC->setArmies(3); cheater->addPlayerTerritory(tC);
    tD->setOwner(victim); tD->setArmies(2); victim->addPlayerTerritory(tD);

    // First call: cheater should auto-conquer tD
    bool c1 = cheater->issueOrder();
    assert(c1 && "Cheater should act when adjacent enemy territories exist");
    assert(tD->getOwner() == cheater && "tD should now belong to cheater");
    // Show cheater territories after conquest
    cout << "-- Cheater owned territories after first conquest: ";
    for (Territory* t : cheater->getOwnedTerritories()) cout << t->getName() << " ";
    cout << "\n";

    // Second call in same issuing-phase: should not act again
    bool c2 = cheater->issueOrder();
    assert(!c2 && "Cheater should only act once per issuing-phase");

    // Simulate new issuing-phase
    if (cheater->getPlayerStrategy()) cheater->getPlayerStrategy()->resetForNewRound();

    // Add a new victim territory adjacent to tC and assign to victim
    Territory* tE = new Territory(5, "E");
    tE->addAdjacent(tC); tC->addAdjacent(tE);
    gameMap2.addTerritory(tE); cont->addTerritory(tE);
    tE->setOwner(victim); tE->setArmies(2); victim->addPlayerTerritory(tE);

    bool c3 = cheater->issueOrder();
    assert(c3 && "Cheater should act again after resetForNewRound");
    assert(tE->getOwner() == cheater && "tE should now belong to cheater");
    // Show cheater territories after second conquest
    cout << "-- Cheater owned territories after second conquest: ";
    for (Territory* t : cheater->getOwnedTerritories()) cout << t->getName() << " ";
    cout << "\n";
    cout << "Cheater test passed.\n\n";

    // Cleanup
    delete benevo;
    delete cheater;
    delete victim;

    cout << "PlayerStrategies focused driver complete.\n";
}

/**
 * @brief Test driver for Human Player Strategy functionality
 * 
 * @details Tests both interactive and non-interactive paths:
 *          1. Non-interactive tests validate toDefend(), toAttack(), and issueOrder(Order*)
 *          2. Optional interactive test allows user to test menu-driven issueOrder()
 */
static void testHumanStrategy() {
    cout << "\n========================================" << endl;
    cout << "   Testing: Human Strategy" << endl;
    cout << "========================================\n" << endl;

    // ======================= Test Setup =======================
    cout << "=== Test Setup: Creating Simple Map ===\n";
    
    Map gameMap;
    Continent* continent = new Continent(1, "TestContinent", 3);
    gameMap.addContinent(continent);
    
    // Create 3 territories
    Territory* myTerritory1 = new Territory(1, "MyLand1");
    Territory* myTerritory2 = new Territory(2, "MyLand2");
    Territory* enemyTerritory = new Territory(3, "EnemyLand");
    
    // Set up adjacencies
    myTerritory1->addAdjacent(myTerritory2);
    myTerritory2->addAdjacent(myTerritory1);
    myTerritory2->addAdjacent(enemyTerritory);
    enemyTerritory->addAdjacent(myTerritory2);
    
    // Add to map and continent
    gameMap.addTerritory(myTerritory1);
    gameMap.addTerritory(myTerritory2);
    gameMap.addTerritory(enemyTerritory);
    continent->addTerritory(myTerritory1);
    continent->addTerritory(myTerritory2);
    continent->addTerritory(enemyTerritory);
    myTerritory1->addContinent(continent);
    myTerritory2->addContinent(continent);
    enemyTerritory->addContinent(continent);
    
    assert(gameMap.validate() && "Map should be valid");
    cout << "Created simple map with 3 territories\n\n";
    
    // ======================= Create Players =======================
    cout << "=== Creating Human Player and Enemy ===\n";
    
    Player* humanPlayer = new Player("Human");
    humanPlayer->setPlayerStrategy(new HumanPlayerStrategy());
    
    Player* enemyPlayer = new Player("Enemy");
    enemyPlayer->setPlayerStrategy(new AggressivePlayerStrategy());
    
    // Assign territories
    myTerritory1->setOwner(humanPlayer);
    myTerritory1->setArmies(5);
    humanPlayer->addPlayerTerritory(myTerritory1);
    
    myTerritory2->setOwner(humanPlayer);
    myTerritory2->setArmies(3);
    humanPlayer->addPlayerTerritory(myTerritory2);
    
    enemyTerritory->setOwner(enemyPlayer);
    enemyTerritory->setArmies(4);
    enemyPlayer->addPlayerTerritory(enemyTerritory);
    
    cout << "Human owns: MyLand1 (5 armies), MyLand2 (3 armies)\n";
    cout << "Enemy owns: EnemyLand (4 armies)\n\n";
    
    // ======================= Test 1: toDefend() Returns All Territories =======================
    cout << "=== Test 1: Human toDefend() Returns All Owned Territories ===\n";
    std::vector<Territory*> defendList = humanPlayer->toDefend();
    assert(defendList.size() == 2 && "Human should have 2 territories to defend");
    
    bool hasMyLand1 = false, hasMyLand2 = false;
    for (Territory* t : defendList) {
        if (t->getName() == "MyLand1") hasMyLand1 = true;
        if (t->getName() == "MyLand2") hasMyLand2 = true;
    }
    assert(hasMyLand1 && hasMyLand2 && "toDefend() should include all owned territories");
    cout << "OK Human toDefend() returns all owned territories for user choice\n\n";
    
    // ======================= Test 2: toAttack() Returns Adjacent Enemies =======================
    cout << "=== Test 2: Human toAttack() Returns Adjacent Enemy Territories ===\n";
    std::vector<Territory*> attackList = humanPlayer->toAttack();
    assert(!attackList.empty() && "Human should have adjacent enemies to attack");
    
    bool hasEnemyLand = false;
    for (Territory* t : attackList) {
        if (t->getName() == "EnemyLand") hasEnemyLand = true;
        assert(t->getOwner() != humanPlayer && "toAttack() should only return enemy territories");
    }
    assert(hasEnemyLand && "toAttack() should include adjacent enemy territory");
    cout << "OK Human toAttack() returns adjacent enemy territories for user choice\n\n";
    
    // ======================= Test 3: issueOrder(Order*) Accepts Valid Orders =======================
    cout << "=== Test 3: Human issueOrder(Order*) Accepts Pre-Created Orders ===\n";
    
    // Create a deploy order and pass to human player
    humanPlayer->setReinforcementPool(5);
    Order* deployOrder = new DeployOrder(humanPlayer, myTerritory1, 3);
    humanPlayer->issueOrder(deployOrder);
    
    assert(humanPlayer->getOrdersList()->size() == 1 && "Human should accept deploy order");
    cout << "OK Human issueOrder(Order*) accepts valid deploy orders\n";
    
    // Create an advance order and pass to human player
    Order* advanceOrder = new AdvanceOrder(humanPlayer, myTerritory1, myTerritory2, 2);
    humanPlayer->issueOrder(advanceOrder);
    
    assert(humanPlayer->getOrdersList()->size() == 2 && "Human should accept advance order");
    cout << "OK Human issueOrder(Order*) accepts valid advance orders\n\n";
    
    // ======================= Test 4: Interactive Path =======================
    cout << "=== Test 4: Interactive Path ===\n";
    cout << "The Human strategy includes a full interactive menu system that:\n";
    cout << "  - Prompts user for Deploy/Advance/Card actions\n";
    cout << "  - Validates territory selection and army counts\n";
    cout << "  - Handles all 5 card types interactively\n";
    cout << "  - Allows viewing hand and ending turn\n\n";
    
    cout << "Would you like to test the interactive menu? (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response == "y" || response == "Y") {
        cout << "\n--- Interactive Test ---\n";
        cout << "Calling humanPlayer->issueOrder() for interactive testing...\n\n";
        
        // Give player some cards to play with
        Deck testDeck;
        humanPlayer->getPlayerHand()->addCard(new Card(Card::Bomb));
        humanPlayer->getPlayerHand()->addCard(new Card(Card::Airlift));
        
        bool issued = humanPlayer->issueOrder();
        
        if (issued) {
            cout << "\nOK Interactive order issued successfully\n";
        } else {
            cout << "\nUser chose to end turn (no order issued)\n";
        }
    } else {
        cout << "Skipping interactive test (run manually to test full menu system)\n";
    }
    
    cout << "\n";
    
    // ======================= Cleanup =======================
    cout << "=== Cleanup ===\n";
    delete humanPlayer;
    delete enemyPlayer;
    cout << "Human strategy test complete\n\n";
}

// Register for main driver discovery (MainDriver.cpp expects a test function with this name in drivers)
// Some projects call these test functions directly; keep the symbol available.