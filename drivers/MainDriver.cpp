#include <iostream>
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include "../include/GameEngine.h"

// Forward declarations of driver test functions
void testLoadMaps();
void testLoadMaps();
void testPlayers();
void testOrdersLists();
void testCards();
void testGameStates();

int main() {
    std::cout << "=== Starting Warzone Test Drivers ===\n\n";
    testLoadMaps();
    testPlayers();
    testOrdersLists();
    testCards();
    testGameStates();
    std::cout << "\n";
    std::cout << "\n=== Program finished successfully ===\n";
    return 0;
}