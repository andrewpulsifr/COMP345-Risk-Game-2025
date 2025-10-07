#include <iostream>
#include "../include/Orders.h"
#include "../include/Player.h"
#include "../include/Map.h"

void testOrdersLists() {
    std::cout << "=== testOrdersLists ===\n";

    Player alice("Alice"), bob("Bob");
    OrdersList ol;
    // Builds a tiny map with 3 territories and assign owners/adjacencies so they can be adjacent to eachother/to let orders run
    Map m;
    Territory* t1 = new Territory(1, "Territory-1");
    Territory* t2 = new Territory(2, "Territory-2");
    Territory* t3 = new Territory(3, "Territory-3");

    // Adjacency for Advance
    t1->addAdjacent(t2);
    t2->addAdjacent(t1);

    m.addTerritory(t1);
    m.addTerritory(t2);
    m.addTerritory(t3);

    // Assign owners and armies to players
    t1->setOwner(&alice);
    t1->setArmies(10);

    t2->setOwner(&bob);
    t2->setArmies(8);

    t3->setOwner(&alice);
    t3->setArmies(6);

    // Create orders with real targets/sources
    Order* o0 = new DeployOrder(&alice, t1, 5);               // deploy to own territory
    Order* o1 = new AdvanceOrder(&alice, t1, t2, 3);          // move from t1 -> adjacent t2
    Order* o2 = new BombOrder(&alice, t2);                    // bomb enemy territory
    Order* o3 = new BlockadeOrder(&alice, t3);                // blockade own territory
    Order* o4 = new AirliftOrder(&alice, t1, t3, 7);          // airlift between own territories
    Order* o5 = new NegotiateOrder(&alice, &bob);             // negotiate with Bob

    ol.add(o0);
    ol.add(o1);
    ol.add(o2);
    ol.add(o3);
    ol.add(o4);
    ol.add(o5);

    std::cout << "Initial list:\n" << ol;

    // Reorder to show move works
    ol.move(5, 0);
    std::cout << "After move(5 -> 0):\n" << ol;

    // Execute each order
    o0->execute();
    o1->execute();
    o2->execute();
    o3->execute();
    o4->execute();
    o5->execute();

    std::cout << "After execute() calls:\n" << ol;

    // Remove one
    ol.remove(2);
    std::cout << "After remove(2):\n" << ol;

    std::cout << "=== end testOrdersLists ===\n";
}
