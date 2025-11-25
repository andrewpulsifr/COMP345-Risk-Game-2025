#include "../include/PlayerStrategies.h"
#include "../include/Player.h"
#include "../include/Map.h"
#include "../include/Orders.h"
#include "../include/Cards.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

// Partner-style focused driver for Benevolent and Cheater strategies
// - No external helpers; execute orders inline
// - Uses asserts to verify expected behavior

void testPlayerStrategies() {
    cout << "\n========================================" << endl;
    cout << "   PlayerStrategies Driver (Focused)" << endl;
    cout << "========================================\n" << endl;

    // Create a simple map and continent
    Map gameMap;
    Continent* cont = new Continent(1, "Cont", 5);
    gameMap.addContinent(cont);

    // Create territories used across tests
    Territory* tA = new Territory(1, "A");
    Territory* tB = new Territory(2, "B");
    Territory* tC = new Territory(3, "C");
    Territory* tD = new Territory(4, "D");

    // Basic adjacency: A-B, B-C, C-D (chain)
    tA->addAdjacent(tB); tB->addAdjacent(tA);
    tB->addAdjacent(tC); tC->addAdjacent(tB);
    tC->addAdjacent(tD); tD->addAdjacent(tC);

    // Add to map and continent
    gameMap.addTerritory(tA); gameMap.addTerritory(tB);
    gameMap.addTerritory(tC); gameMap.addTerritory(tD);
    // Ensure territories are members of the continent (both sides of relationship)
    tA->addContinent(cont); cont->addTerritory(tA);
    tB->addContinent(cont); cont->addTerritory(tB);
    tC->addContinent(cont); cont->addTerritory(tC);
    tD->addContinent(cont); cont->addTerritory(tD);

    assert(gameMap.validate() && "Map should be valid (connected chain)");

    // --- Benevolent test ---
    cout << "--- Benevolent strategy test ---\n";
    Player* benevo = new Player("Benevolent");
    benevo->setPlayerStrategy(new BenevolentPlayerStrategy());

    // Assign two owned territories (weak and strong)
    tA->setOwner(benevo); tA->setArmies(1); benevo->addPlayerTerritory(tA);
    tB->setOwner(benevo); tB->setArmies(5); benevo->addPlayerTerritory(tB);

    // Reinforcements: expect deploy to weakest (tA)
    benevo->setReinforcementPool(4);
    bool issued = benevo->issueOrder();
    assert(issued && "Benevolent should issue deploy when reinforcements available");
    assert(benevo->getOrdersList()->size() == 1 && "One deploy order should be queued");

    // Show state before card plays
    cout << "-- Before card plays --\n";
    cout << "  Reinforcement pool: " << benevo->getReinforcementPool() << "\n";
    cout << "  Armies: A=" << tA->getArmies() << ", B=" << tB->getArmies() << "\n";
    cout << "  OrdersList size: " << benevo->getOrdersList()->size() << "\n";

    // Play a reinforcement card via Card::play() to exercise issueOrder(Order*) path
    Deck deck;
    Card* rein = new Card(Card::Reinforcement);
    benevo->getPlayerHand()->addCard(rein);
    rein->play(*benevo, deck, *benevo->getPlayerHand());

    // Note: current Card::play converts Reinforcement into an AdvanceOrder.
    // Benevolent rejects offensive orders, so OrdersList should remain size 1.
    cout << "-- After Reinforcement play --\n";
    benevo->getPlayerHand()->showHand();
    if (benevo->getOrdersList()) benevo->getOrdersList()->print();
    assert(benevo->getOrdersList()->size() == 1 && "Benevolent should reject offensive card-created orders (Advance/Bomb)");

    // Prepare an enemy territory adjacent to B so Bomb would target an enemy if allowed
    Player* enemy = new Player("Enemy");
    Territory* tX = new Territory(99, "X");
    tX->addAdjacent(tB); tB->addAdjacent(tX);
    gameMap.addTerritory(tX); cont->addTerritory(tX);
    tX->setOwner(enemy); tX->setArmies(3); enemy->addPlayerTerritory(tX);

    // Play a Bomb card (offensive) - Benevolent should reject it (order not queued)
    Card* bomb = new Card(Card::Bomb);
    benevo->getPlayerHand()->addCard(bomb);
    cout << "-- Before Bomb play: enemy X armies=" << tX->getArmies() << "\n";
    bomb->play(*benevo, deck, *benevo->getPlayerHand());
    cout << "-- After Bomb play (should be rejected by Benevolent) --\n";
    benevo->getPlayerHand()->showHand();
    if (benevo->getOrdersList()) benevo->getOrdersList()->print();
    cout << "  Enemy X armies still=" << tX->getArmies() << "\n";
    // OrdersList should remain the same size (still only initial deploy)
    assert(benevo->getOrdersList()->size() == 1 && "Benevolent should reject Bomb orders created by cards");

    // Execute queued orders for benevo (execute all until empty)
    while (benevo->getOrdersList()->size() > 0) {
        Order* o = benevo->popNextOrder();
        assert(o != nullptr);
        cout << "Executing: " << o->name() << "\n";
        o->execute();
        delete o;
    }

    // OrdersList should be empty after executing all orders
    assert(benevo->getOrdersList()->size() == 0 && "All queued orders should have been executed and removed");
    cout << "-- After execution --\n";
    cout << "  Reinforcement pool: " << benevo->getReinforcementPool() << "\n";
    cout << "  Armies: A=" << tA->getArmies() << ", B=" << tB->getArmies() << "\n";
    cout << "Benevolent test passed.\n\n";

    // --- Cheater test ---
    cout << "--- Cheater strategy test ---\n";
    Player* cheater = new Player("Cheater");
    cheater->setPlayerStrategy(new CheaterPlayerStrategy());
    Player* victim = new Player("Victim");

    // Assign tC to cheater and tD to victim (adjacent)
    tC->setOwner(cheater); tC->setArmies(3); cheater->addPlayerTerritory(tC);
    tD->setOwner(victim); tD->setArmies(2); victim->addPlayerTerritory(tD);

    // First call: cheater should auto-conquer tD
    bool c1 = cheater->issueOrder();
    assert(c1 && "Cheater should act when adjacent enemy territories exist");
    assert(tD->getOwner() == cheater && "tD should now belong to cheater");
    // Show cheater territories after conquest
    cout << "-- Cheater owned territories after first conquest: ";
    for (Territory* t : cheater->getOwnedTerritories()) cout << t->getName() << " ";
    cout << "\n";

    // Second call in same issuing-phase: should not act again
    bool c2 = cheater->issueOrder();
    assert(!c2 && "Cheater should only act once per issuing-phase");

    // Simulate new issuing-phase
    if (cheater->getPlayerStrategy()) cheater->getPlayerStrategy()->resetForNewRound();

    // Add a new victim territory adjacent to tC and assign to victim
    Territory* tE = new Territory(5, "E");
    tE->addAdjacent(tC); tC->addAdjacent(tE);
    gameMap.addTerritory(tE); cont->addTerritory(tE);
    tE->setOwner(victim); tE->setArmies(2); victim->addPlayerTerritory(tE);

    bool c3 = cheater->issueOrder();
    assert(c3 && "Cheater should act again after resetForNewRound");
    assert(tE->getOwner() == cheater && "tE should now belong to cheater");
    // Show cheater territories after second conquest
    cout << "-- Cheater owned territories after second conquest: ";
    for (Territory* t : cheater->getOwnedTerritories()) cout << t->getName() << " ";
    cout << "\n";
    cout << "Cheater test passed.\n\n";

    // Cleanup
    delete benevo;
    delete cheater;
    delete victim;

    cout << "PlayerStrategies focused driver complete.\n";
}

// Register for main driver discovery (MainDriver.cpp expects a test function with this name in drivers)
// Some projects call these test functions directly; keep the symbol available.
