#include <iostream>
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Orders.h"

// Forward declarations of driver test functions
void testLoadMaps();
void testPlayers();
void testOrdersLists();

int main() {
    std::cout << "=== Starting Warzone Test Drivers ===\n\n";

    testLoadMaps();
    testPlayers();
    testOrdersLists();
    std::cout << "\n=== Program finished successfully ===\n";
    return 0;
}