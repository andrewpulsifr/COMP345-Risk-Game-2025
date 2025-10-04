#include <iostream>
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"

// Forward declarations of driver test functions
void testPlayers();
void testOrdersLists();

int main() {
    std::cout << "=== Starting Warzone Test Drivers ===\n\n";
    testPlayers();
    testOrdersLists();
    std::cout << "\n=== Program finished successfully ===\n";
    return 0;
}