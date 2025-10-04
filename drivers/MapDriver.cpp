#include "../include/Map.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>
#include <algorithm>

using namespace std;

#include <unordered_map>
#include <string>

namespace {

    /* -- Data for testing territory adjacency and continent membership -- */

    // Set expected continent names and territory counts 
    const unordered_map<string,int> expContCount = {
        {"North America", 9}, {"South America", 4}, {"Africa", 6},
        {"Europe", 7}, {"Asia", 12}, {"Australia", 4}
    };

    // sample of the expected continent a territory should be a member of
    const pair<const string, const string> expContMembership[] = {
        {"Alaska", "North America"},
        {"Brazil", "South America"},
        {"Egypt", "Africa"},
        {"Ukraine", "Europe"},
        {"Japan", "Asia"},
        {"New Guinea", "Australia"},
    };
    // Pairs that should be adjacent
    const unordered_map<string, unordered_set<string>> adjTerritoriesMap = {
        // North America
        {"Alaska", {"Northwest Territory", "Alberta", "Kamchatka"}},
        {"Central America", {"Western United States", "Eastern United States", "Venezuala"}},
        {"Brazil", {"Venezuala", "Peru", "Argentina", "North Africa"}},

        // Africa
        {"Egypt", {"North Africa", "East Africa", "Middle East", "Southern Europe"}},

        // Europe
        {"Iceland", {"Greenland", "Great Britain", "Scandinavia"}},
        {"Ukraine", {"Ural", "Afghanistan", "Middle East", "Southern Europe", "Northern Europe", "Scandinavia"}},

        // Asia
        {"Kamchatka", {"Yatusk", "Irkutsk", "Japan", "Alaska", "Mongolia"}},
        {"Siam", {"China", "India", "Indonesia"}},

        // Australia
        {"Western Australia", {"Indonesia", "New Guinea", "Eastern Australia"}},
    };

    // Pairs that should not be adjacent.
    const vector<pair<string, string>> shouldNotBeAdjacentMap = {
        {"Alaska", "Quebec"},
        {"Japan", "China"},
        {"Brazil", "Greenland"},
        {"Iceland", "Western Europe"},
        {"Western Australia", "Japan"},
    };

    /* Helper Functions */

    // Find a territory by name in the map
    static Territory* findTerritoryByName(const Map& map, const string& name){
        for (Territory* t: map.getTerritories() ){
            if(t->getName() == name){
                return t;
            }
        }
        return nullptr; // Not found
    }

    // Assert that a sample of territories belong to their expected continents
    static void assertContinentMembershipSample(const Map& smallMap){
        for (const auto& [name, expectedCont] : expContMembership) {
            // Find the territory by name and assert it exists
            Territory* t = findTerritoryByName(smallMap, name);
            Continent* c = (t && !t->getContinents().empty()) ? t->getContinents()[0] : nullptr;

            // Assert the territory and its continent exist
            assert(t && "Territory not found.");
            assert(c && "Territory continent not found.");

            // Check if the territory's continent name matches the expected continent name
            string contName = c->getName();
            assert( contName == expectedCont && "Incorrect Continent.");
        }
    }

    // Assert that the adjacency relationships between territories are correct
    static void assertAdjOfTerritories(const Map& smallMap) {
        for(const auto& [tName, expAdjTerritories]: adjTerritoriesMap){
            Territory* t = findTerritoryByName(smallMap, tName);
            assert(t && "Territory not found.");

            for (const std::string& adjName : expAdjTerritories) {
                Territory* adjTerritory = findTerritoryByName(smallMap, adjName);
                assert(adjTerritory && "Adjacent territory not found.");
                assert(t->isAdjacentTo(adjTerritory) && "Missing expected neighbor.");
                assert(adjTerritory->isAdjacentTo(t) && "Missing reverse adjacency.");
            }

            // Create a set to compare for extras or duplicates in adjacency
            const vector<Territory*>& adj = t->getAdjacents();
            std::unordered_set<std::string> actualAdjNames;
            actualAdjNames.reserve(adj.size()); // Reserve space to avoid rehashing
            for (const Territory* a : adj) {
                assert(a && "Null adjacent pointer.");
                actualAdjNames.insert(a->getName());
            }

            // check that there are no extras and no missing
            assert(actualAdjNames.size() == expAdjTerritories.size() && "Extra or duplicate neighbors present.");
        }
    }

    // Assert that certain pairs of territories are not adjacent
    static void assertNonAdjacencySample(const Map& m) {
        for (const auto& [t1Name, t2Name] : shouldNotBeAdjacentMap) {
            Territory* t1 = findTerritoryByName(m, t1Name);
            Territory* t2 = findTerritoryByName(m, t2Name);
            assert(t1 && t2 && "Territory not found for non-adjacency check.");
            assert(!t1->isAdjacentTo(t2) && "Territories should not be adjacent.");
            assert(!t2->isAdjacentTo(t1) && "Territories should not be adjacent.");
        }
    }


    // Assert that the map has the expected continents and territories
    static void assertSmallWorld(const Map& smallMap) {

        // assert the number of continents matches expected
        assert(smallMap.getContinents().size() == expContCount.size());

        // assert each continent has the expected number of territories and valid names
        for( Continent* cont : smallMap.getContinents()){
            assert(cont); // check for null pointer

            const string& name = cont -> getName();
            const auto& match = expContCount.find(name);

            assert(match != expContCount.end());
            assert(static_cast<size_t>(match->second) == cont -> getTerritories().size());
        }

        // assert total territories on the map 
        assert(static_cast<int>(smallMap.getTerritories().size()) == 42);

        // Adjacency and continent membership checks
        assertContinentMembershipSample(smallMap);
        assertAdjOfTerritories(smallMap);
        assertNonAdjacencySample(smallMap);
    }
}


void testLoadMaps(){

    cout << "=== Test Expected Map Loading ===" << endl;
    cout << "Run expected map loading tests for: World (small).map\n" << endl;

    MapLoader mapLoader;
    Map smallWorldMap;
    // Use the exact filename; ensure it’s in assets/maps/
    mapLoader.loadMap("assets/maps/World (small).map", smallWorldMap);
    cout << "Loaded World (small).map\n";

    assertSmallWorld(smallWorldMap);
    cout << "Assertions passed.\n";
    smallWorldMap.validate();
    cout << "Validation passed.\n";
    cout << "--------------------------------\n";
    cout << "Test of World (small).map OK\n";
    cout << "--------------------------------\n";

    cout << "\n=== Test Validation for all maps ===" << endl;

    vector<string> mapFiles = mapLoader.getMapFiles();
    for (const string& mapFile : mapFiles) {
            Map tempMap;
            mapLoader.loadMap(mapFile, tempMap);
            cout << "Loaded " << mapFile << " OK\n";
            tempMap.validate();
            cout << "Validation passed.\n";
            cout << "--------------------------------\n";
    }

    cout << "=== Interactive Map Loading ===" << endl;
    try{
        // Initialize MapLoader and get available map files
        mapLoader.printMapFiles(mapFiles);

        // Prompt user to select a map
        cout << "Select a map by index: ";
        int index = 0;
        if (!(cin >> index)){
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            throw runtime_error("Invalid input. Please enter a number.");
        }

        if(index < 1 || index > static_cast<int>(mapFiles.size()) + 1) {
            throw runtime_error("Invalid map selection.");
        }
        if(index == static_cast<int>(mapFiles.size()) + 1) {
            cout << "Exiting map loading." << endl;
            return; // Exit if the user selected the exit option
        }

        // Load the selected map
        Map mapOutput;
        string selectedMap = mapFiles[index - 1];
        mapLoader.loadMap(selectedMap, mapOutput);
        cout << mapOutput; // Print map details

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    cout << "=== Map Loading Test Complete ===" << endl;
}

