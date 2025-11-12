/**
 * @file OrdersDriver.cpp
 * @brief Assignment 1: Orders and OrdersList class driver and testing
 * @author Matteo
 * @date October 2025
 * @version 1.0
 * 
 * This file contains the test driver for the Orders system functionality, demonstrating
 * order creation, execution, OrdersList manipulation (add, remove, move), and
 * proper polymorphic behavior of different order types.
 */

#include <iostream>
#include "../include/Orders.h"
#include "../include/Player.h"
#include "../include/Map.h"

// Importing only the neccessary std functions.
using std::cout;

/**
 * @brief Comprehensive test function for Orders and OrdersList functionality
 * @details Tests all requirements:
 *          - Order creation and polymorphic behavior
 *          - OrdersList add, remove, and move operations
 *          - Order execution and validate methods
 *          - Stream insertion operators for Orders and OrdersList
 * 
 * Creates a game scenario with players and territories to test all order types
 * (Deploy, Advance, Bomb, Blockade, Airlift, Negotiate) and OrdersList management.
 */
void testOrdersLists() {
    cout << "=== testOrdersLists ===\n";

    // ======================= Test Setup =======================
    // Create test players for order ownership and execution
    Player alice("Alice"), bob("Bob");
    OrdersList ol;
    
    // Build minimal map with territories for order execution testing
    Map m;
    Territory* t1 = new Territory(1, "Territory-1");
    Territory* t2 = new Territory(2, "Territory-2");
    Territory* t3 = new Territory(3, "Territory-3");

    // Set up territory adjacencies for Advance order testing
    t1->addAdjacent(t2);
    t2->addAdjacent(t1);

    // Add territories to map for proper management
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

    // ======================= Order Creation and OrdersList Testing =======================
    // Create different order types to test polymorphic behavior
    Order* o0 = new DeployOrder(&alice, t1, 5);               // Deploy to own territory
    Order* o1 = new AdvanceOrder(&alice, t1, t2, 3);          // Advance from own to adjacent territory
    Order* o2 = new BombOrder(&alice, t2);                    // Bomb enemy territory
    Order* o3 = new BlockadeOrder(&alice, t3);                // Blockade own territory
    Order* o4 = new AirliftOrder(&alice, t1, t3, 7);          // Airlift between own territories
    Order* o5 = new NegotiateOrder(&alice, &bob);             // Negotiate with enemy player

    // Add all orders to OrdersList to test add functionality
    ol.add(o0);
    ol.add(o1);
    ol.add(o2);
    ol.add(o3);
    ol.add(o4);
    ol.add(o5);

    // Display initial state to show proper order addition
    cout << "Initial list:\n" << ol;

    // ======================= OrdersList Move Operation Testing =======================
    // Test move functionality by moving last order to first position
    ol.move(5, 0);
    cout << "After move(5 -> 0):\n" << ol;

    // ======================= Order Execution Testing =======================
    // Execute all orders to test polymorphic execute() method
    o0->execute();
    o1->execute();
    o2->execute();
    o3->execute();
    o4->execute();
    o5->execute();

    // Display results after execution to show order effects
    cout << "After execute() calls:\n" << ol;

    // ======================= OrdersList Remove Operation Testing =======================
    // Test remove functionality by removing order at index 2
    ol.remove(2);
    cout << "After remove(2):\n" << ol;

    cout << "=== end testOrdersLists ===\n";
}

void testOrderExecution() {
    std::cout << "\n=== testOrderExecution ===\n";

    //Setup players and territories
    Player alice("Alice"), bob("Bob");
    neutralPlayer = new Player("Neutral"); 

    Map m;
    Territory* t1 = new Territory(1, "Territory-1");
    Territory* t2 = new Territory(2, "Territory-2");
    Territory* t3 = new Territory(3, "Territory-3");

    t1->addAdjacent(t2);
    t2->addAdjacent(t1);

    m.addTerritory(t1);
    m.addTerritory(t2);
    m.addTerritory(t3); 

    t1->setOwner(&alice); t1->setArmies(10);
    t2->setOwner(&bob);   t2->setArmies(8);
    t3->setOwner(&alice); t3->setArmies(6);

    //deploy
    DeployOrder deploy(&alice, t1, 5);
    deploy.validate();
    deploy.execute();
    std::cout << "Deploy: " << deploy.effect() << ", t1 armies: " << t1->getArmies() << "\n";

    //advance
    AdvanceOrder advance(&alice, t1, t2, 3);
    advance.validate();
    advance.execute();
    std::cout << "Advance: " << advance.effect() 
        << ", t1 armies: " << t1->getArmies()
        << ", t2 armies: " << t2->getArmies() << "\n";
    AdvanceOrder advance2(&alice, t1, t2, 2);
    advance2.execute();
    std::cout << "Advance after Negotiate: " << advance2.effect() << "\n";

    //bomb
    BombOrder bomb(&alice, t2);
    bomb.validate();
    bomb.execute();
    std::cout << "Bomb: " << bomb.effect() 
        << ", t2 armies: " << t2->getArmies() << "\n";
    
    //blockade
    BlockadeOrder blockade(&alice, t3);
    blockade.validate();
    blockade.execute();
    std::cout << "Blockade: " << blockade.effect() 
        << ", t3 armies: " << t3->getArmies()
        << ", t3 new owner: " << (t3->getOwner() ? t3->getOwner()->getPlayerName() : "none") << "\n";

    //airlift
    AirliftOrder airlift(&alice, t1, t3, 2);
    airlift.validate();
    airlift.execute();
    std::cout << "Airlift: " << airlift.effect() 
        << ", t1 armies: " << t1->getArmies()
        << ", t3 armies: " << t3->getArmies() << "\n";

    //negotiate
    NegotiateOrder negotiate(&alice, &bob);
    negotiate.validate();
    negotiate.execute();
    std::cout << "Negotiate: " << negotiate.effect() << "\n";

    //Cleanup
    delete t1; delete t2; delete t3; delete neutralPlayer;

    std::cout << "=== end testOrderExecution ===\n";
}

