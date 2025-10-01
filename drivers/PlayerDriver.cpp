#include "../include/Player.h"
#include "../include/Map.h"
#include <iostream>
#include <vector>

//testPlayers

void testPlayers() {
    std::cout << "=== Player Test Driver ===\n";

    //Creating Players
    Player* alice = new Player("Alice");
    Player* bob = new Player("Bob");

    //Creating dummy Territories
    Territory* t1 = new Territory(1, "Territory1", nullptr, 5);
    Territory* t2 = new Territory(2, "Territory2", nullptr, 3);
    Territory* t3 = new Territory(3, "Territory3", nullptr, 2);
    Territory* t4 = new Territory(4, "Territory4", nullptr, 4);

    //Set adjacency (simple graph)
    t1->addAdjacent(t2);
    t2->addAdjacent(t1);

    t2->addAdjacent(t3);
    t3->addAdjacent(t2);

    t3->addAdjacent(t4);
    t4->addAdjacent(t3);

    t1->addAdjacent(t4); // connect back
    t4->addAdjacent(t1);

    // 4. Assign Territories to Players
    alice->addPlayerTerritory(t1);
    alice->addPlayerTerritory(t3);

    bob->addPlayerTerritory(t2);
    bob->addPlayerTerritory(t4);

    // 5. Display players and their territories
    std::cout << *alice;
    std::cout << *bob;

    // 6. Test toDefend()
    std::cout << "\nAlice's toDefend() territories: ";
    for (auto t : alice->toDefend())
        std::cout << t->getName() << " ";
    std::cout << "\n";

    // 7. Test toAttack()
    std::cout << "Alice's toAttack() territories: ";
    for (auto t : alice->toAttack())
        std::cout << t->getName() << " ";
    std::cout << "\n";

    // 8. Clean up
    delete alice;
    delete bob;
    delete t1;
    delete t2;
    delete t3;
    delete t4;

    std::cout << "=== End of Player Test ===\n";
}