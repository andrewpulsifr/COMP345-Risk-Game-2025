#include "../include/Orders.h"

void testOrdersLists() {
    OrdersList list;

    // Add one of each order
    list.add(new DeployOrder());
    list.add(new AdvanceOrder());
    list.add(new BombOrder());
    list.add(new BlockadeOrder());
    list.add(new AirliftOrder());
    list.add(new NegotiateOrder());

    std::cout << "=== Initial Orders ===\n";
    list.print();

    // Move first order to end
    std::cout << "\nMove first order to last...\n";
    list.move(0, 5);
    list.print();

    // Remove an order
    std::cout << "\nRemove order at index 2...\n";
    list.remove(2);
    list.print();

    std::cout << "=== End Orders Test ===\n";
}