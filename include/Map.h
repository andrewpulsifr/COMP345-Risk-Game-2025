/**
 * @file Map.h
 * @brief Assignment 1 – Part 1 (Warzone): Map, Territory, Continent, MapLoader declarations.
 *
 * @details
 *  - The map is modeled as a connected, undirected graph: Territory = node; adjacency = edge.
 *  - Continents must form connected subgraphs; every Territory belongs to exactly one Continent.
 *  - Each Territory has a Player owner and an armies count.
 *  - MapLoader reads Conquest `.map` files and builds a Map; it must also handle arbitrary text files,
 *    rejecting invalid maps gracefully.
 *  - Validation (`Map::validate`) must demonstrate:
 *      (1) the whole map is connected,
 *      (2) each continent is a connected subgraph,
 *      (3) each territory belongs to exactly one continent.
 *
 * @note All Part-1 classes/functions live in this duo (Map.h/Map.cpp). The driver `testLoadMaps()` is
 *       implemented in MapDriver.cpp and loads multiple files, accepting valid maps and rejecting invalid ones.
 */

#pragma once
#include <vector>
#include <string>
#include <iosfwd>

using namespace std;

class Player;
class Continent;
enum class MapFileSections { None, Map, Continents, Territories };


/**
 * @class Territory
 * @brief Node in the map graph.
 *
 * @details
 *  Represents a country/region on the Warzone map. Each Territory:
 *   - belongs to exactly one Continent,
 *   - may have any number of adjacent Territories (undirected edges),
 *   - is owned by a Player and tracks an armies count.
 *
 *  The adjacency list should be treated as **undirected**; when wiring edges, add both directions.
 *  `Map::validate()` relies on adjacency to traverse the graph and check the assignment requirements.
 *
 * @invariant
 *  - `continent` is non-null for a valid, validated map.
 *  - Adjacency is symmetric (if A lists B, B lists A).
 *
 * @ownership
 *  - `continent` and adjacent territories are **non-owning pointers** (owned by Map/Continent).
 *  - `owner` is a non-owning pointer to a Player (owned elsewhere).
 *
 * @note Implementations are in Map.cpp (no inline bodies in headers, per A1 rules).
 */
class Territory{

public:
    Territory(); // default constructor
    Territory(const Territory& other); // copy constructor
    Territory(int id, const string& name, Player* owner, int armies); // parameterized constructor
    ~Territory(); // destructor

    Territory& operator=(const Territory& other); // copy assignment operator
    friend ostream& operator<<(ostream& os, const Territory& territory);

    // note some setter might not be needed but added for completeness
    int getId() const;
    string getName() const;
    Player* getOwner() const;
    Continent* getContinent() const;
    void setContinent(Continent* c);
    int getArmies() const;
    void setOwner(Player* newOwner);
    void setArmies(int newArmies);
    void addArmies(int additionalArmies);
    void removeArmies(int removedArmies);
    void addAdjacent(Territory* t);
    bool isAdjacentTo(const Territory* t) const;
    const vector<Territory*>& getAdjacents() const;

private:
    int id;
    string name;
    Continent* continent; // pointer to the continent the territory belongs to
    Player* owner; // pointer to the player who owns the territory
    int armies; // number of armies in the territory
    vector<Territory*> adjacentTerritories; // list of pointers to adjacent territories
};

/**
 * @class Continent
 * @brief Connected subgraph grouping related Territories.
 *
 * @details
 *  A Continent is a set of Territories that must form a **connected subgraph** when validated.
 *  It does not enforce connectivity on mutation; connectivity is checked by `Map::validate()`.
 *
 * @ownership
 *  The `territories` vector stores **non-owning** pointers; `Map` is the owner of Territory objects.
 *
 * @invariant
 *  Under a successful `Map::validate()`:
 *   - All territories listed here are unique and belong to this continent.
 *   - The induced subgraph on these territories is connected.
 */
class Continent {
public:
    Continent(); // default constructor
    Continent(const Continent& other); // copy constructor
    Continent(int id, const string& name); // parameterized constructor
    ~Continent();

    Continent& operator=(const Continent& other); // copy assignment operator

    int getId() const;
    string getName() const;
    void addTerritory(Territory* territory);
    const vector<Territory*>& getTerritories() const;

    friend ostream& operator<<(ostream& os, const Continent& continent);

private:
    int id;
    string name;
    vector<Territory*> territories; // list of pointers to territories in the continent
};

/**
 * @class Map
 * @brief Graph container for Territories and Continents, plus validation logic.
 *
 * @details
 *  Owns the Territory and Continent objects that make up a Warzone map and exposes helpers
 *  to add nodes/continents and wire undirected adjacency. The `validate()` routine demonstrates
 *  the three required checks:
 *   1) The overall map graph is connected.
 *   2) Each continent induces a connected subgraph.
 *   3) Every territory belongs to exactly one continent.
 *
 * @ownership
 *  - `Map` **owns** the `Territory*` and `Continent*` it stores (define copy/assign/destructor accordingly).
 *  - External pointers returned by getters are non-owning observations.
 *
 * @notes
 *  - No inline bodies in the header; implement in Map.cpp per A1 rules.
 */
class Map {
public:
    Map(); // default constructor
    Map(const Map& other); // copy constructor
    ~Map();

    Map& operator=(const Map& other); // copy assignment operator

    void addTerritory(Territory* territory);
    void addContinent(Continent* continent);
    const vector<Territory*>& getTerritories() const;
    const vector<Continent*>& getContinents() const;

    // 1) map connected
    // 2) each continent is connected subgraph
    // 3) each territory in exactly one continent
    bool validate() const;

    friend ostream& operator<<(ostream& os, const Map& map);

private:
    vector<Territory*> territories; // list of pointers to all territories in the map
    vector<Continent*> continents; // list of pointers to all continents in the map

};

/**
 * @class MapLoader
 * @brief Parser/loader for Conquest `.map` files that builds a Map graph.
 *
 * @details
 *  Reads text files in the Conquest map format (and arbitrary text files), constructing
 *  a `Map` instance with continents, territories, and undirected adjacencies. Invalid
 *  inputs are rejected gracefully (e.g., return nullptr / false, log diagnostics).
 *
 * @contracts
 *  - On success, returns a heap-allocated `Map*` (caller takes ownership).
 *  - On failure, returns `nullptr`.
 *
 * @note Robust loaders should verify section order, unique ids/names, symmetric edges,
 *       and consistent continent membership during or after parsing.
 */
class MapLoader {
public:
    MapLoader(); // default constructor
    MapLoader(const MapLoader&);              // copy constructor
    ~MapLoader();

    MapLoader& operator=(const MapLoader&);   // copy assignment operator
    friend ostream& operator<<(ostream& os, const MapLoader& ml);

    bool loadMap(const string& filename, Map& mapOutput); // load a map from a .map file
    vector<string> getMapFiles(); // get list of map files
    string printMapFiles(const vector<string>& mapFiles); // print a list of map files
private:
    bool parseMapFile(istream& inputMap, Map& mapOutput); // helper function to parse the file
    // TO DO: add any additional helper functions or data members as needed
};

/**
 * @file MapDriver.cpp
 * @brief Driver for Assignment 1 – Part 1.
 *
 * @details
 *  Implements `void testLoadMaps()`:
 *   - Iterates over a list of `.map` files (valid and invalid).
 *   - Uses `MapLoader` to construct a `Map`.
 *   - Accepts valid maps, rejects invalid ones.
 *   - For valid maps, calls `Map::validate()` and prints the result of the three checks.
 */
void testLoadMaps(); // driver function to test loading maps