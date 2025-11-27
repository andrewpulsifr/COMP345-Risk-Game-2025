/**
 * @file PlayerDriver.cpp
 * @brief Assignment 1 : Player class driver and testing
 * @author Matteo
 * @date October 2025
 * @version 1.0
 * 
 * This file contains the test driver for the Player class functionality, demonstrating
 * all requirements including hand ownership, territory management methods
 * (toDefend/toAttack), and order issuance capabilities.
 */

#include <iostream>
#include "../include/Map.h"    
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"

// Importing only the neccessary std functions.
using std::cout;

/**
 * @brief Comprehensive test function for Player class functionality
 * @details Tests all requirements:
 *          - Player owns a Hand of cards (pointer member)
 *          - toDefend() method returns owned territories
 *          - toAttack() method returns attackable adjacent enemy territories  
 *          - issueOrder() method adds orders to player's OrdersList
 * 
 * Creates a simple game scenario with two players (Alice and Bob) owning
 * territories with adjacency relationships, then demonstrates all required
 * Player class methods and functionality.
 */
void testPlayers() {
    cout << "\n=== Testing Player Class Functionality ===\n";

    // ======================= Test Setup =======================
    // Create test players with meaningful names
    Player alice("Alice");
    Player bob("Bob");
    Map gameMap;

    // Create territories for game scenario
    Territory* canada = new Territory(1, "Canada");
    Territory* usa = new Territory(2, "USA");
    Territory* mexico = new Territory(3, "Mexico");

    // Add territories to map for proper management
    gameMap.addTerritory(canada);
    gameMap.addTerritory(usa);
    gameMap.addTerritory(mexico);

    // Set up territory adjacencies to enable attack/defend logic 
    canada->addAdjacent(usa);      // Canada borders USA
    usa->addAdjacent(canada);      // USA borders Canada  
    usa->addAdjacent(mexico);      // USA borders Mexico
    mexico->addAdjacent(usa);      // Mexico borders USA

    // Assign territory ownership and initial army counts
    canada->setOwner(&alice);      // Alice owns Canada
    canada->setArmies(5);
    usa->setOwner(&alice);         // Alice owns USA
    usa->setArmies(3);
    mexico->setOwner(&bob);        // Bob owns Mexico
    mexico->setArmies(4);

    // Register territories with their owning players
    alice.addPlayerTerritory(canada);
    alice.addPlayerTerritory(usa);
    bob.addPlayerTerritory(mexico);

    // =======================Player Hand Ownership =======================
    
    // Demonstrate that Player owns a Hand of cards
    cout << "\n=== Player Hand Ownership ===\n";
    Hand* aliceHand = alice.getPlayerHand();
    Deck gameDeck;
    gameDeck.addCard(new Card(Card::Bomb));
    gameDeck.addCard(new Card(Card::Reinforcement));
    
    // Draw cards to populate Alice's hand
    gameDeck.draw(*aliceHand);
    gameDeck.draw(*aliceHand);
    cout << "Alice's hand after drawing 2 cards:\n";
    aliceHand->showHand();

    // ======================= toDefend() and toAttack() Methods =======================
    
    //Demonstrate toDefend() and toAttack() strategic methods
    cout << "\n=== toDefend() and toAttack() Methods ===\n";
    cout << "Alice's Territories:\n";
    cout << "Currently owned territories: ";
    for (Territory* territory : alice.getOwnedTerritories()) {
        cout << territory->getName() << " ";
    }
    
    // Test toDefend(): should return Alice's owned territories
    cout << "\nTerritories to defend: ";
    for (Territory* territory : alice.toDefend()) {
        cout << territory->getName() << " ";
    }
    
    /** Test toAttack(): should return adjacent enemy territories Alice can attack */
    cout << "\nTerritories to attack: ";
    for (Territory* territory : alice.toAttack()) {
        cout << territory->getName() << " ";
    }
    cout << "\n";

    // ======================= issueOrder() Method =======================
    
    // Demonstrate issueOrder() method for adding orders to player's list
    cout << "\n=== issueOrder() Method Demonstration ===\n";
    cout << "Creating orders for Alice:\n";

    //  Create and issue Deploy order to reinforce territory with armies
    alice.issueOrder(new DeployOrder(&alice, canada, 3));
    cout << "Deploy 3 armies to Canada\n";

    // Create and issue Advance order to move armies between territories
    alice.issueOrder(new AdvanceOrder(&alice, canada, mexico, 2));
    cout << "Advance 2 armies from Canada to Mexico\n";

    // ======================= Cleanup =======================
    canada->setOwner(nullptr);
    usa->setOwner(nullptr);
    mexico->setOwner(nullptr);

    cout << "\n=== Player Class Testing Complete ===\n";
}