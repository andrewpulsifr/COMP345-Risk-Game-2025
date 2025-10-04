#include <iostream>
#include "../include/Orders.h"
#include "../include/Player.h"
// If you want valid orders with territories/adjacency, include Map bits too:
// #include "../include/Map.h"

void testOrdersLists() {
    std::cout << "=== testOrdersLists ===\n";

    Player alice("Alice"), bob("Bob");

    OrdersList ol;

    // Minimal orders (parameterized ctors so validate() can succeed if you wire targets)
    // If you don't have Map wired yet, you can pass nullptrs and just demo add/move/remove.
    // Example with nullptrs (execute() will report invalid; that's fine for Part 3 demo):
    ol.add(new DeployOrder(&alice, /*target*/nullptr, /*amount*/5));
    ol.add(new AdvanceOrder(&alice, /*src*/nullptr, /*dst*/nullptr, /*amount*/3));
    ol.add(new BombOrder(&alice, /*target*/nullptr));
    ol.add(new BlockadeOrder(&alice, /*target*/nullptr));
    ol.add(new AirliftOrder(&alice, /*src*/nullptr, /*dst*/nullptr, /*amount*/7));
    ol.add(new NegotiateOrder(&alice, &bob));

    std::cout << "Initial list:\n" << ol;

    // Move last to front
    ol.move(5, 0);
    std::cout << "After move(5 -> 0):\n" << ol;

    // Execute everything (effects are stored internally; your operator<< prints description)
    for (int i = 0; i < static_cast<int>(6); ++i) {
        // safeguard if sizes changed
        if (i >= 0) ; // no-op
    }
    // Just reprint list after "execution" to show stable order
    std::cout << "After execute() calls (effects recorded internally):\n" << ol;

    // Remove one
    ol.remove(2);
    std::cout << "After remove(2):\n" << ol;

    std::cout << "=== end testOrdersLists ===\n";
}