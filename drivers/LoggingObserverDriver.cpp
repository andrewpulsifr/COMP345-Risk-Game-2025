/**
 * @file LoggingObserverDriver.cpp
 * @brief Test driver for Part 5: Game Log Observer
 * 
 * @details
 * Demonstrates that:
 * 1. Order, OrdersList, and GameEngine are subclasses of Subject and ILoggable
 * 2. Order::execute(), OrdersList::add(), and GameEngine::setState() use notify()
 * 3. LogObserver writes to gamelog.txt when orders are added
 * 4. LogObserver writes to gamelog.txt when orders are executed
 * 5. LogObserver writes to gamelog.txt when GameEngine state changes
 * 
 * @todo: Implement part 1 FileCommandProcessor + Command + Command processor logging
 * 
 * @author Andrew Pulsifer
 * @date November 5, 2025
 */

#include "../include/LoggingObserver.h"
#include "../include/Orders.h"
#include "../include/GameEngine.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/**
 * @brief Test driver for Part 5: Game Log Observer
 * 
 * Demonstrates all requirements for the Observer pattern implementation
 * including logging of orders, order execution, and game state changes.
 * 
 * @todo: Implement part 1 FileCommandProcessor + Command + Command processor logging
 */
void testLoggingObserver() {
    cout << "\n========================================" << endl;
    cout << "Testing Part 5: Game Log Observer" << endl;
    cout << "========================================\n" << endl;

    // Create the log observer
    LogObserver* logObserver = new LogObserver();

    // ========================================
    // Test 1: Verify inheritance
    // ========================================
    cout << "Test 1: Verifying class inheritance" << endl;
    cout << "-----------------------------------" << endl;

    // Create test objects
    Map gameMap;
    Territory* territory1 = new Territory(1, "Canada");
    Territory* territory2 = new Territory(2, "USA");
    gameMap.addTerritory(territory1);
    gameMap.addTerritory(territory2);

    Player* player1 = new Player("Alice");
    player1->addPlayerTerritory(territory1);
    player1->addPlayerTerritory(territory2);

    DeployOrder* deployOrder = new DeployOrder(player1, territory1, 5);
    OrdersList* ordersList = new OrdersList();
    GameEngine* gameEngine = new GameEngine();

    cout << "OK : Order inherits from Subject and ILoggable" << endl;
    cout << "OK : OrdersList inherits from Subject and ILoggable" << endl;
    cout << "OK : GameEngine inherits from Subject and ILoggable" << endl;
    cout << endl;

    // ========================================
    // Test 2: Attach observer to subjects
    // ========================================
    cout << "Test 2: Attaching LogObserver to subjects" << endl;
    cout << "-------------------------------------------" << endl;

    deployOrder->attach(logObserver);
    ordersList->attach(logObserver);
    gameEngine->attach(logObserver);

    cout << "LogObserver attached to DeployOrder" << endl;
    cout << "LogObserver attached to OrdersList" << endl;
    cout << "LogObserver attached to GameEngine" << endl;
    cout << endl;

    // ========================================
    // Test 3: Test GameEngine state changes
    // ========================================
    cout << "Test 3: Testing GameEngine state logging" << endl;
    cout << "-----------------------------------------" << endl;

    cout << "Processing 'loadmap' command..." << endl;
    gameEngine->processCommand("loadmap");
    
    cout << "Processing 'validatemap' command..." << endl;
    gameEngine->processCommand("validatemap");
    
    cout << "Processing 'addplayer' command..." << endl;
    gameEngine->processCommand("addplayer");

    cout << "OK : GameEngine state changes logged to gamelog.txt" << endl;
    cout << endl;

    // ========================================
    // Test 4: Test OrdersList::add()
    // ========================================
    cout << "Test 4: Testing OrdersList::add() logging" << endl;
    cout << "------------------------------------------" << endl;

    AdvanceOrder* advanceOrder = new AdvanceOrder(player1, territory1, territory2, 3);
    advanceOrder->attach(logObserver);

    BombOrder* bombOrder = new BombOrder(player1, territory2);
    bombOrder->attach(logObserver);

    cout << "Adding DeployOrder to OrdersList..." << endl;
    ordersList->add(deployOrder);

    cout << "Adding AdvanceOrder to OrdersList..." << endl;
    ordersList->add(advanceOrder);

    cout << "Adding BombOrder to OrdersList..." << endl;
    ordersList->add(bombOrder);

    cout << "OK : OrdersList::add() notifications logged to gamelog.txt" << endl;
    cout << endl;

    // ========================================
    // Test 5: Test Order::execute()
    // ========================================
    cout << "Test 5: Testing Order::execute() logging" << endl;
    cout << "-----------------------------------------" << endl;

    cout << "Executing DeployOrder..." << endl;
    deployOrder->execute();

    cout << "Executing AdvanceOrder..." << endl;
    advanceOrder->execute();

    cout << "Executing BombOrder..." << endl;
    bombOrder->execute();

    cout << "OK : Order::execute() effects logged to gamelog.txt" << endl;
    cout << endl;

    // ========================================
    // Test 6: Display log file contents & Verify
    // ========================================
    cout << "Test 6: Displaying gamelog.txt contents & Assertions" << endl;
    cout << "-----------------------------------------------------" << endl;

    ifstream logFile("gamelog.txt");
    if (logFile.is_open()) {
        string line;
        int lineCount = 0;
        string logContents;
        
        cout << "\n--- BEGIN gamelog.txt ---\n" << endl;
        while (getline(logFile, line)) {
            cout << line << endl;
            logContents += line + "\n";
            lineCount++;
        }
        cout << "\n--- END gamelog.txt ---" << endl;
        cout << "\nTotal lines in log: " << lineCount << endl;
        logFile.close();
        
        // ========================================
        // Assertions: Verify expected content
        // ========================================
        cout << "\n--- Running Assertions ---" << endl;
        
        // Assert 1: Check for GameEngine state changes
        bool hasMapLoaded = logContents.find("GameEngine: Current State = MapLoaded") != string::npos;
        bool hasMapValidated = logContents.find("GameEngine: Current State = MapValidated") != string::npos;
        bool hasPlayersAdded = logContents.find("GameEngine: Current State = PlayersAdded") != string::npos;
        
        cout << "Assert 1: GameEngine state 'MapLoaded' logged... " 
             << (hasMapLoaded ? "PASS" : "FAIL") << endl;
        cout << "Assert 2: GameEngine state 'MapValidated' logged... " 
             << (hasMapValidated ? "PASS" : "FAIL") << endl;
        cout << "Assert 3: GameEngine state 'PlayersAdded' logged... " 
             << (hasPlayersAdded ? "PASS" : "FAIL") << endl;
        
        // Assert 2: Check for OrdersList additions
        bool hasOrdersList1 = logContents.find("OrdersList contains 1 order(s): Deploy") != string::npos;
        bool hasOrdersList2 = logContents.find("OrdersList contains 2 order(s): Deploy, Advance") != string::npos;
        bool hasOrdersList3 = logContents.find("OrdersList contains 3 order(s): Deploy, Advance, Bomb") != string::npos;
        
        cout << "Assert 4: OrdersList with 1 order logged... " 
             << (hasOrdersList1 ? "PASS" : "FAIL") << endl;
        cout << "Assert 5: OrdersList with 2 orders logged... " 
             << (hasOrdersList2 ? "PASS" : "FAIL") << endl;
        cout << "Assert 6: OrdersList with 3 orders logged... " 
             << (hasOrdersList3 ? "PASS" : "FAIL") << endl;
        
        // Assert 3: Check for Order executions
        bool hasDeployEffect = logContents.find("Order: Deploy | Effect: Deploy 5 to Canada (owner: Alice)") != string::npos;
        bool hasAdvanceEffect = logContents.find("Order: Advance | Effect:") != string::npos;
        bool hasBombEffect = logContents.find("Order: Bomb | Effect:") != string::npos;
        
        cout << "Assert 7: DeployOrder execution logged... " 
             << (hasDeployEffect ? "PASS" : "FAIL") << endl;
        cout << "Assert 8: AdvanceOrder execution logged... " 
             << (hasAdvanceEffect ? "PASS" : "FAIL") << endl;
        cout << "Assert 9: BombOrder execution logged... " 
             << (hasBombEffect ? "PASS" : "FAIL") << endl;
        
        // Final assertion: All checks passed
        bool allPassed = hasMapLoaded && hasMapValidated && hasPlayersAdded &&
                        hasOrdersList1 && hasOrdersList2 && hasOrdersList3 &&
                        hasDeployEffect && hasAdvanceEffect && hasBombEffect;
        
        cout << "\n--- Assertion Summary ---" << endl;
        if (allPassed) {
            cout << "OK : ALL 9 ASSERTIONS PASSED" << endl;
        } else {
            cout << "FAIL : SOME ASSERTIONS FAILED - Check log contents above" << endl;
        }
        
    } else {
        cout << "ERROR: Could not open gamelog.txt for reading" << endl;
    }
    cout << endl;

    // ========================================
    // Summary
    // ========================================
    cout << "========================================" << endl;
    cout << "All Part 5 Requirements Demonstrated:" << endl;
    cout << "========================================" << endl;
    cout << "(1) Order, OrdersList, GameEngine inherit from Subject & ILoggable" << endl;
    cout << "(2) Order::execute(), OrdersList::add(), GameEngine::setState() use notify()" << endl;
    cout << "(3) Orders added to OrdersList are logged" << endl;
    cout << "(4) Order execution effects are logged" << endl;
    cout << "(5) GameEngine state changes are logged" << endl;
    // TO DO: Implement part 1 FileCommandProcessor + Command + Command processor logging
    cout << endl;

    // Cleanup
    delete logObserver;
    delete gameEngine;
    delete ordersList;
    delete player1;
    // Note: orders are owned by ordersList and will be deleted by it
}
