#include <iostream>
#include <vector>

#include "../include/GameEngine.h"
#include "../include/Map.h"
#include "../include/Player.h"
#include "../include/Orders.h"
#include "../include/Cards.h"

using std::cout;
using std::endl;

// Helper: give ownership + armies and sync with player's territory list
static void ownTerritory(Player* p, Territory* t, int armies) {
    if (!p || !t) return;
    t->setOwner(p);
    t->setArmies(armies);
    p->addPlayerTerritory(t);
}

// === Required free function ===
void testMainGameLoop() {
    cout << "=============================================\n";
    cout << "          testMainGameLoop() - Part 3\n";
    cout << "=============================================\n\n";

    // ---------------------------------------------------------------------
    // Build a tiny demo map:
    //  - 1 continent (bonus 5)
    //  - 3 territories: Alaska, Northwest Territory, Alberta
    //  - fully adjacent so toAttack()/toDefend() have options
    // ---------------------------------------------------------------------
    Map* demoMap = new Map();

    Continent* cont = new Continent(1, "DemoContinent", 5);
    demoMap->addContinent(cont);

    Territory* alaska   = new Territory(1, "Alaska");
    Territory* nw       = new Territory(2, "Northwest Territory");
    Territory* alberta  = new Territory(3, "Alberta");

    cont->addTerritory(alaska);
    cont->addTerritory(nw);
    cont->addTerritory(alberta);

    demoMap->addTerritory(alaska);
    demoMap->addTerritory(nw);
    demoMap->addTerritory(alberta);

    // Make them all mutually adjacent
    alaska->addAdjacent(nw);
    alaska->addAdjacent(alberta);
    nw->addAdjacent(alaska);
    nw->addAdjacent(alberta);
    alberta->addAdjacent(alaska);
    alberta->addAdjacent(nw);

    // ---------------------------------------------------------------------
    // Create 3 players
    // ---------------------------------------------------------------------
    Player* alpha   = new Player("Alpha");
    Player* bravo   = new Player("Bravo");
    Player* charlie = new Player("Charlie");

    std::vector<Player*>* players = new std::vector<Player*>();
    players->push_back(alpha);
    players->push_back(bravo);
    players->push_back(charlie);

    // One territory each for the first reinforcement demo
    ownTerritory(alpha, alaska, 5);
    ownTerritory(bravo, nw, 5);
    ownTerritory(charlie, alberta, 5);

    cout << "[Setup] Demo map with 3 territories created.\n";
    cout << "[Setup] Alpha: Alaska | Bravo: Northwest Territory | Charlie: Alberta\n\n";

    // Hook them into the engine
    GameEngine engine;
    engine.setMapAndPlayersForDemo(demoMap, players);

    cout << "GameEngine initialized in Start state.\n";

    // ---------------------------------------------------------------------
    // (1) Reinforcement: case A – everyone has 1 territory
    // ---------------------------------------------------------------------
    cout << "=== (1a) Reinforcement: 1 territory each ===\n";
    cout << "Expected: each player gets 3 armies (minimum rule).\n\n";
    engine.reinforcementPhase(); // prints pools via GameEngine implementation

    // ---------------------------------------------------------------------
    // (1) Reinforcement: case B – Alpha owns whole continent
    // ---------------------------------------------------------------------
    // Reset pools so math is obvious
    alpha->setReinforcementPool(0);
    bravo->setReinforcementPool(0);
    charlie->setReinforcementPool(0);

    // Move territories to Alpha
    bravo->removePlayerTerritory(nw);
    charlie->removePlayerTerritory(alberta);
    ownTerritory(alpha, nw, 5);
    ownTerritory(alpha, alberta, 5);

    cout << "\n=== (1b) Reinforcement: Alpha owns all territories ===\n";
    cout << "Expected: Alpha gets max(3, 3/3) + 5 = 8 armies.\n\n";
    engine.reinforcementPhase();

    // ---------------------------------------------------------------------
    // (4) Card play: Alpha plays Bomb to create an order
    // ---------------------------------------------------------------------
    cout << "\n[Cards] Alpha receives a Bomb card.\n";
    Deck tempDeck;
    Card* bomb = new Card(Card::typeOfCard::Bomb);
    alpha->getPlayerHand()->addCard(bomb);
    cout << "[Cards] Alpha plays Bomb to create an order via Card::play().\n";
    bomb->play(*alpha, tempDeck, *alpha->getPlayerHand());

    // NOTE:
    //  - issueOrder() (GameEngine::issueOrdersPhase) respects:
    //      (2) Only Deploy while reinforcementPool > 0
    //      (3) Advance using toAttack()/toDefend()
    //    so we just run the loop and let your Player logic demonstrate it.

    // Reset pools so main loop behavior is clean
    alpha->setReinforcementPool(0);
    bravo->setReinforcementPool(0);
    charlie->setReinforcementPool(0);

    cout << "\n=== Running mainGameLoop() to demonstrate (2)-(6) ===\n\n";

    // mainGameLoop will:
    //  - (1) call reinforcementPhase() each turn
    //  - (2) call issueOrdersPhase() (round-robin, deploy-while-pool rule)
    //  - (3) call executeOrdersPhase() (deploys first, then others)
    //  - (5) remove players with 0 territories
    //  - (6) detect when one player owns all territories (Alpha) and end
    engine.mainGameLoop();

    cout << "\n=============================================\n";
    cout << "   End of testMainGameLoop() demonstration\n";
    cout << "=============================================\n\n";
}
