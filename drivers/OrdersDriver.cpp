#include <iostream>
#include "../include/Orders.h"
#include "../include/Player.h"
#include "../include/Map.h"

void testOrdersLists() {
    std::cout << "=== testOrdersLists ===\n";

    Player alice("Alice"), bob("Bob");
    OrdersList ol;

    // Keep the pointers you create
    Order* o0 = new DeployOrder(&alice, /*target*/nullptr, /*amount*/5);
    Order* o1 = new AdvanceOrder(&alice, /*src*/nullptr, /*dst*/nullptr, /*amount*/3);
    Order* o2 = new BombOrder(&alice, /*target*/nullptr);
    Order* o3 = new BlockadeOrder(&alice, /*target*/nullptr);
    Order* o4 = new AirliftOrder(&alice, /*src*/nullptr, /*dst*/nullptr, /*amount*/7);
    Order* o5 = new NegotiateOrder(&alice, &bob);

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

    // Execute each order (effects will be visible in operator<< if you print them)
    // It's fine if validate() fails with nullptrs; you'll see "Invalid ..." effects.
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
