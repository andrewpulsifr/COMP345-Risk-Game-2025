#include <iostream>
#include "../include/Map.h"    
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"

using namespace std;

void testPlayers() {
    cout << "\n=== Testing Player Class Functionality ===\n";

    // Create players and territories
    Player alice("Alice");
    Player bob("Bob");
    Map gameMap;

    Territory* canada = new Territory(1, "Canada");
    Territory* usa = new Territory(2, "USA");
    Territory* mexico = new Territory(3, "Mexico");

    // Add territories to the map
    gameMap.addTerritory(canada);
    gameMap.addTerritory(usa);
    gameMap.addTerritory(mexico);

    // Set up adjacencies and ownership
    canada->addAdjacent(usa);
    usa->addAdjacent(canada);
    usa->addAdjacent(mexico);
    mexico->addAdjacent(usa);

    // Assign owners and armies for demonstration
    canada->setOwner(&alice);
    canada->setArmies(5);
    usa->setOwner(&alice);
    usa->setArmies(3);
    mexico->setOwner(&bob);
    mexico->setArmies(4);

    // Demonstrate Player owns Territories
    alice.addPlayerTerritory(canada);
    alice.addPlayerTerritory(usa);
    bob.addPlayerTerritory(mexico);

    // Demonstrate Player owns a Hand of Cards
    cout << "\n=== Player Hand Ownership ===\n";
    Hand* aliceHand = alice.getPlayerHand();
    Deck gameDeck;
    gameDeck.addCard(new Card(Card::Bomb));
    gameDeck.addCard(new Card(Card::Reinforcement));
    
    gameDeck.draw(*aliceHand);
    gameDeck.draw(*aliceHand);
    cout << "Alice's hand after drawing 2 cards:\n";
    aliceHand->showHand(*aliceHand);

    // Demonstrate toDefend() and toAttack() methods
    cout << "\n=== toDefend() and toAttack() Methods ===\n";
    cout << "Alice's Territories:\n";
    cout << "Currently owned territories: ";
    for (Territory* territory : alice.getOwnedTerritories()) {
        cout << territory->getName() << " ";
    }
    
    cout << "\nTerritories to defend: ";
    for (Territory* territory : alice.toDefend()) {
        cout << territory->getName() << " ";
    }
    
    cout << "\nTerritories to attack: ";
    for (Territory* territory : alice.toAttack()) {
        cout << territory->getName() << " ";
    }
    cout << "\n";

    // Demonstrate issueOrder() method
    cout << "\n=== issueOrder() Method Demonstration ===\n";
    cout << "Creating orders for Alice:\n";
    
    alice.issueOrder(new DeployOrder(&alice, canada, 3));
    cout << "Deploy 3 armies to Canada\n";
    
    alice.issueOrder(new AdvanceOrder(&alice, canada, mexico, 2));
    cout << "Advance 2 armies from Canada to Mexico\n";

    cout << "\n=== Player Class Testing Complete ===\n";
}