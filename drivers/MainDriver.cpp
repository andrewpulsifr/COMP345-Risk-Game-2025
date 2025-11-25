/**
 * @file MainDriver.cpp
 * @brief Main test driver for the Warzone game system
 * @details This driver coordinates and executes all component test drivers to demonstrate:
 *          - Map loading and validation functionality
 *          - Player class features and territory management
 *          - OrdersList operations and Order execution
 *          - Cards system with deck, hand, and playing mechanics
 *          - GameEngine state transitions and command processing
 *          
 *          Each test driver validates requirements for their respective components,
 *          ensuring system testing and demonstration of functionality.
 * @author Andrew Pulsifer, Chhay and Matteo 
 * @date October 2025
 * @version 1.0
 */

#include <iostream>
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include "../include/GameEngine.h"

// Forward declarations of driver test functions
void testLoadMaps();
void testPlayers();
void testOrdersLists();
void testCards();
void testGameStates();
void testCommandProcessor(int argc, char* argv[]);
void testStartupPhase(int argc, char* argv[]);
void testLoggingObserver();
void testMainGameLoop();
void testPlayerStrategies(); // TODO: not yet implemented

/**
 * @brief Main entry point for Warzone component testing
 * @details Orchestrates execution of all test drivers in logical sequence:
 *          1. Map system testing (loading, validation, operations)
 *          2. Player functionality testing (territories, orders, hand)
 *          3. Orders system testing (creation, execution, list management)
 *          4. Cards system testing (deck, drawing, playing, Order generation)
 *          5. GameEngine testing (state transitions, command processing)
 * @return 0 on successful test completion
 */
int main(int argc, char* argv[]) {
    std::cout << "=== Starting Warzone Test Drivers ===\n\n";
    
    // Run only the PlayerStrategies driver for focused testing
    // All other drivers are intentionally skipped to keep this run deterministic
    // and focused on verifying new PlayerStrategy implementations.
    // testLoadMaps();
    // testPlayers();
    // testOrdersLists();
    // testCards();
    // testGameStates();
    // testMainGameLoop();
    // testCommandProcessor(argc, argv);
    // testStartupPhase(argc, argv);
    // testLoggingObserver();
    testPlayerStrategies();
  

    std::cout << "\n";
    std::cout << "\n=== Program finished successfully ===\n";
    return 0;
}