#include <iostream>
#include "../include/Map.h"     // still include, so Territory is complete
#include "../include/Player.h"
#include "../include/Orders.h"

void testPlayers() {
    std::cout << "\n=== testPlayers ===\n";

    Player alice("Alice");
    Player bob("Bob");

    // Use default ctor (declared in your Map.h) to avoid relying on a param ctor
    Territory* t1 = new Territory();
    Territory* t2 = new Territory();
    Territory* t3 = new Territory();

    // Minimal ownership so toDefend() works (toAttack will be empty without adjacency)
    alice.addPlayerTerritory(t1);
    bob.addPlayerTerritory(t2);
    bob.addPlayerTerritory(t3);

    // Show lists
    auto defendA = alice.toDefend();
    auto attackA = alice.toAttack();

    std::cout << "Alice owns: ";
    for (auto* t : alice.getOwnedTerritories()) {
        // getName() may be empty with default ctor; that's fine for now
        std::cout << t->getName() << " ";
    }
    std::cout << "\nAlice toDefend: ";
    for (auto* t : defendA) std::cout << t->getName() << " ";
    std::cout << "\nAlice toAttack (likely empty w/o adjacency): ";
    for (auto* t : attackA) std::cout << t->getName() << " ";
    std::cout << "\n";

    // Use the pointer overload (declared in your Player.h)
    alice.issueOrder(new DeployOrder(&alice, /*target*/nullptr, /*amount*/3));
    alice.issueOrder(new AdvanceOrder(&alice, /*src*/nullptr, /*dst*/nullptr, /*amount*/2));
    std::cout << "Issued two orders for Alice (pointer overload).\n";

    delete t1; delete t2; delete t3;
    std::cout << "=== end testPlayers ===\n";
}