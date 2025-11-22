#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
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
    cout << "Map validation: PASSED ✓\n\n";
    
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
    cout << "✓ Aggressive player deploys all reinforcements to strongest territory\n\n";
    
    // ======================= Test 2: Aggressive Strategy - Attack Phase =======================
    cout << "=== Test 2: Aggressive Strategy - Attack Phase ===\n";
    assert(aggressivePlayer->getReinforcementPool() == 0 && "No reinforcements should remain");
    
    issued = aggressivePlayer->issueOrder();
    assert(issued && "Aggressive player should issue advance order to attack enemy");
    assert(aggressivePlayer->getOrdersList()->size() == 2 && "Should have two orders (deploy + advance)");
    cout << "✓ Aggressive player advances to attack adjacent enemy territory\n\n";
    
    // ======================= Test 3: Neutral Strategy Behavior =======================
    cout << "=== Test 3: Neutral Strategy - Never Issues Orders ===\n";
    neutralPlayer->setReinforcementPool(TestConstants::NEUTRAL_REINFORCEMENTS);
    
    issued = neutralPlayer->issueOrder();
    assert(!issued && "Neutral player should never issue orders");
    assert(neutralPlayer->getOrdersList()->size() == 0 && "Neutral should have no orders");
    assert(neutralPlayer->getReinforcementPool() == TestConstants::NEUTRAL_REINFORCEMENTS 
           && "Neutral should not use reinforcements");
    cout << "✓ Neutral player never issues orders even with reinforcements\n\n";
    
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
    
    cout << "✓ Neutral player transforms to aggressive when attacked\n\n";
    
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
    cout << "✓ Transformed player issues aggressive orders (dynamic strategy change works)\n\n";
    
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
    cout << "✓ Aggressive toDefend() returns territories sorted by strength (descending)\n";
    
    // Test 6b: Aggressive toAttack - should list adjacent enemies
    std::vector<Territory*> attackList = aggressivePlayer->toAttack();
    assert(!attackList.empty() && "Aggressive player should have enemies to attack");
    
    for (Territory* territory : attackList) {
        bool isEnemyTerritory = territory->getOwner() != aggressivePlayer;
        assert(isEnemyTerritory && "toAttack() should only return enemy territories");
    }
    cout << "✓ Aggressive toAttack() returns adjacent enemy territories\n";
    cout << "✓ toAttack() methods work correctly for different strategies\n\n";
    
    // ======================= Cleanup =======================
    cout << "=== Cleanup ===\n";
    delete aggressivePlayer;
    delete neutralPlayer;
    // Map destructor will clean up territories
    
    cout << "\n=== Player Strategies Test Complete ===\n";
}
