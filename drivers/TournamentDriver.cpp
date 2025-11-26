#include <iostream>
#include <vector>
#include <string>
#include "../include/GameEngine.h"
#include "../include/CommandProcessing.h"
#include "../include/Cards.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

/**
 * @brief Driver for Assignment 3 – Part 2: Tournament Mode.
 *
 * Demonstrates that:
 *  (1) The 'tournament' command line is parsed and validated by CommandProcessor
 *      (including both valid and invalid examples).
 *  (2) GameEngine executes a tournament completely automatically (no user input),
 *      using GameEngine::handleTournament(), which in turn runs multiple games
 *      via runSingleTournamentGame() and runGameWithTurnLimit().
 */
void testTournament()
{
    cout << "=============================================\n";
    cout << "          testTournament() - Part 2\n";
    cout << "=============================================\n\n";

    CommandProcessor cp;
    GameEngine engine;


    engine.getDeck()->showDeck();

    // ---------------------------------------------------------------------
    // 1) INVALID TOURNAMENT COMMANDS: show  error messages
    // ---------------------------------------------------------------------
    cout << "[1] Testing CommandProcessor with INVALID tournament commands\n\n";

    // A couple of different error cases
    vector<string> invalidCmds = {
        // (a) Too many maps (> 5)
        "tournament -M A.map B.map C.map D.map E.map F.map -P Aggressive Benevolent -G 2 -D 10",

        // (b) Bad strategy name + G and D out of range
        "tournament -M World.map -P Crazy AI -G 0 -D 5",

        // (c) Same player strategy is entered twice.
        "tournament -M World.map -P Benevolent Benevolent -G 3 -D 10"
    };

    for (std::size_t i = 0; i < invalidCmds.size(); ++i) {
        cout << "  Invalid command #" << (i + 1) << ":\n";
        cout << "    " << invalidCmds.at(i) << "\n";

        try {
            cp.printTournamentCommandLog(invalidCmds[i]);
            //cout << "    -> ERROR: command should have been rejected but was accepted!\n\n";
        }
        catch (const std::exception& ex) {
            cout << "    -> Correctly rejected with message:\n";
            cout << "       " << ex.what() << "\n\n";
        }
    }

    // ---------------------------------------------------------------------
    // 2) VALID TOURNAMENT COMMAND: parsed & validated by CommandProcessor
    // ---------------------------------------------------------------------
    string validTournamentCmd = "tournament -M World.map Vernon.map -P Aggressive Benevolent Cheater Neutral -G 1 -D 10";

    cout << "[2] Testing CommandProcessor with a VALID tournament command\n";
    cout << "    Command: " << validTournamentCmd << "\n\n";

    try {
        // This internally calls validateTournament() and prints a detailed log
        cp.printTournamentCommandLog(validTournamentCmd);
        cout << "    -> validateTournament() accepted the command.\n\n";
    }
    catch (const std::exception& ex) {
        cout << "    -> UNEXPECTED ERROR for a supposedly valid command:\n";
        cout << "       " << ex.what() << "\n\n";
    }

    // ---------------------------------------------------------------------
    // 3) Run a full tournament via GameEngine::handleTournament()
    // ---------------------------------------------------------------------
    cout << "[3] Running full tournament via GameEngine::handleTournament()\n\n";

    bool ok = engine.handleTournament(validTournamentCmd);

    if (ok) {
        cout << "  -> Tournament finished successfully.\n";
    } else {
        cout << "  -> Tournament failed to complete.\n";
    }

    cout << "\n=============================================\n";
    cout << "      End of testTournament() demonstration\n";
    cout << "=============================================\n\n";
}