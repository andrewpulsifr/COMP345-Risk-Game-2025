#include <iostream>
#include "../include/Map.h"    
#include "../include/Player.h"
#include "../include/Orders.h"

void testPlayers() {
    std::cout << "\n=== Testing Player Class Functionality ===\n";

    // Create players with names
    Player alice("Alice");
    Player bob("Bob");

    // Create a map to manage territory ownership
    Map gameMap;

    // Create territories with meaningful names and IDs
    Territory* canada = new Territory(1, "Canada");
    Territory* usa = new Territory(2, "USA");
    Territory* mexico = new Territory(3, "Mexico");
    Territory* brazil = new Territory(4, "Brazil");

    // Add territories to map (map takes ownership)
    gameMap.addTerritory(canada);
    gameMap.addTerritory(usa);
    gameMap.addTerritory(mexico);
    gameMap.addTerritory(brazil);

    // Set up territory adjacencies (borders between countries)
    // Creates a chain: Canada <-> USA <-> Mexico <-> Brazil
    canada->addAdjacent(usa);      // Canada borders USA
    usa->addAdjacent(canada);      // USA borders Canada
    usa->addAdjacent(mexico);      // USA borders Mexico
    mexico->addAdjacent(usa);      // Mexico borders USA
    mexico->addAdjacent(brazil);   // Mexico borders Brazil
    brazil->addAdjacent(mexico);   // Brazil borders Mexico

    // Set territory ownership and initial armies
    canada->setOwner(&alice);      // Alice owns Canada
    canada->setArmies(5);          // Canada has 5 armies
    usa->setOwner(&alice);         // Alice owns USA
    usa->setArmies(3);            // USA has 3 armies
    mexico->setOwner(&bob);        // Bob owns Mexico
    mexico->setArmies(4);         // Mexico has 4 armies
    brazil->setOwner(&bob);        // Bob owns Brazil
    brazil->setArmies(2);         // Brazil has 2 armies

    // Register territories with their owning players
    alice.addPlayerTerritory(canada);
    alice.addPlayerTerritory(usa);
    bob.addPlayerTerritory(mexico);
    bob.addPlayerTerritory(brazil);

    // Get Alice's defend and attack lists
    std::vector<Territory*> territoriesToDefend = alice.toDefend();
    std::vector<Territory*> territoriesToAttack = alice.toAttack();

    // Display Alice's territory information
    std::cout << "Alice's Territories:\n";
    std::cout << "Currently owned territories: ";
    const std::vector<Territory*>& ownedTerritories = alice.getOwnedTerritories();
    for (Territory* territory : ownedTerritories) {
        std::cout << territory->getName() << " ";
    }
    
    std::cout << "\nTerritories to defend: ";
    for (Territory* territory : territoriesToDefend) {
        std::cout << territory->getName() << " ";
    }
    
    std::cout << "\nTerritories to attack: ";
    for (Territory* territory : territoriesToAttack) {
        std::cout << territory->getName() << " ";
    }
    std::cout << "\n\n";

    // Create and issue orders for Alice
    std::cout << "Creating orders for Alice:\n";
    
    // Deploy order - reinforce Canada with 3 armies
    alice.issueOrder(new DeployOrder(&alice, canada, 3));
    std::cout << "- Deploy 3 armies to " << canada->getName() << "\n";
    
    // Advance order - attack Mexico from Canada with 2 armies
    alice.issueOrder(new AdvanceOrder(&alice, canada, mexico, 2));
    std::cout << "- Advance 2 armies from " << canada->getName() 
              << " to " << mexico->getName() << "\n";

    std::cout << "\n=== Player Class Testing Complete ===\n";
}