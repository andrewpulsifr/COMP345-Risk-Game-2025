/**
 * @file Map.cpp
 * @brief Implementation of Map, Territory, Continent, and MapLoader classes for the Warzone game
 * @author Andrew Pulsifer
 * @date October 2025
 * @version 1.0
 * 
 * This file contains the core map functionality including:
 * - Map loading and parsing from .map files
 * - Territory and continent management
 * - Map validation (connectivity, territory-continent relationships)
 * - Deep copy semantics for safe object copying
 */

#include "../include/Map.h"
#include "../include/Player.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <filesystem>
#include <utility> 

using namespace std;
namespace fs = std::filesystem;


/** @brief Anonymous namespace containing map parsing utilities and constants */
namespace {
    /// Directory path containing game map files
    const string MAP_PATH = "assets/maps";

    /**
     * @brief Context structure for map file parsing state
     * 
     * This structure maintains parsing state during map file processing,
     * handling forward references and deferred adjacency resolution.
     */
    struct ParseContext {
        /// Fast O(1) lookup from continent name to continent object
        unordered_map<string, Continent*> continentMap;
        
        /// Fast O(1) lookup from territory name to territory object  
        unordered_map<string, Territory*> territoryMap;
        
        /** 
         * Territories awaiting adjacency resolution due to forward references
         * Key: territory name that doesn't exist yet
         * Value: territories that want to connect to that territory
         */
        unordered_map<string, vector<Territory*>> waitingTerritories;
        
        int nextContinentId = 0;  ///< Auto-incrementing ID for new continents
        int nextTerritoryId = 0;  ///< Auto-incrementing ID for new territories
    };

    /**
     * @brief Removes leading and trailing whitespace from a string
     * @param rawLine The input string to trim
     * @return A new string with whitespace removed from both ends
     * @note Returns empty string if input contains only whitespace
     * @complexity O(n) where n is the length of the string
     */
    inline string trim(string_view rawLine) {
        constexpr char whitespace[] = " \t\n\r\f\v";

        // Find first non-whitespace character from the beginning
        const string::size_type first = rawLine.find_first_not_of(whitespace);
        if (first == string_view::npos) { return string(); }

        // Find last non-whitespace character from the end
        const string::size_type last  = rawLine.find_last_not_of(whitespace);

        // Extract substring containing only non-whitespace content
        return string(rawLine.substr(first, last - first + 1));
    }

    /**
     * @brief Parses a comma-separated values string into individual tokens
     * @param str Input string containing comma-separated values
     * @return Vector of trimmed string tokens
     * 
     * @details Splits the input string on comma delimiters and trims whitespace
     * from each resulting token. Handles empty tokens and trailing commas gracefully.
     * 
     * @complexity O(n) where n is the length of the input string
     */
    inline vector<string> csvParse(string_view str) {
        vector<string> result;
        size_t start = 0;
        size_t end = str.find(',');

        // Split the string by commas and trim each token of whitespace
        while (end != string_view::npos) {
            result.push_back(string(trim(str.substr(start, end - start))));
            start = end + 1;
            end = str.find(',', start);
        }
        // Add the final token after the last comma (or entire string if no commas)
        result.push_back(string(trim(str.substr(start))));
        return result;
    }

    /**
     * @brief Lists all .map files in a directory sorted alphabetically by filename
     * @param dir Path to directory containing map files
     * @return Vector of file paths sorted by filename
     * 
     * @throws std::runtime_error if directory doesn't exist or isn't accessible
     * 
     * @details Scans the specified directory for files with .map extension,
     * sorts them alphabetically by filename for consistent ordering.
     * Only includes regular files, ignoring subdirectories and special files.
     * 
     * @complexity O(n log n) where n is the number of .map files (due to sorting)
     */
    static vector<fs::path> listMapFiles(const fs::path& dir) {
        vector<fs::path> mapFiles;

        // Validate directory existence and accessibility
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            throw runtime_error("Map directory does not exist or is not a directory: " + dir.string());
        }

        // Scan directory for .map files, filtering out non-regular files
        for (const fs::directory_entry& fileEntry : fs::directory_iterator(dir)) {
            if (fileEntry.is_regular_file() && fileEntry.path().extension() == ".map") {
                mapFiles.push_back(fileEntry.path());
            }
        }

        // Sort alphabetically by filename for consistent user experience
        sort(mapFiles.begin(), mapFiles.end(), [](const fs::path& a, const fs::path& b){
            return a.filename().string() < b.filename().string();
        });

        return mapFiles;
    }

    /**
     * @brief Parses a continent definition line from a map file
     * @param line Raw line from map file containing continent definition
     * @param context Parsing context for state management
     * @param mapOutput Target map to add the parsed continent to
     * 
     * @throws std::runtime_error if line format is invalid (missing '=' separator or invalid bonus value)
     * 
     * @details Expected format: "ContinentName=BonusValue"
     * - Creates new continent with auto-generated ID and parsed bonus value
     * - Parses and validates bonus value (integer after '=')
     * - Uses RAII with unique_ptr for exception safety during construction
     * 
     * @pre line must be a valid continent definition line with valid integer bonus
     * @post New continent with bonus value added to map and indexed in parsing context
     */
    static void parseContinents(string_view line, ParseContext& context, Map& mapOutput){
        if(line.find('=') == string_view::npos) {
            throw runtime_error("Invalid continent line: " + string(line));
        }
        
        size_t equalPos = line.find('=');
        string continentName = trim(line.substr(0, equalPos));
        string bonusStr = trim(line.substr(equalPos + 1));
        
        // Parse bonus value with error handling
        int bonus = 0;
        try {
            bonus = stoi(bonusStr);
        } catch (const invalid_argument& e) {
            throw runtime_error("Invalid bonus value '" + bonusStr + "' for continent '" + continentName + "'");
        } catch (const out_of_range& e) {
            throw runtime_error("Bonus value '" + bonusStr + "' out of range for continent '" + continentName + "'");
        }

        // Create continent with bonus value using RAII for exception safety during construction
        auto newContinent = make_unique<Continent>(context.nextContinentId++, continentName, bonus);
        Continent* rawPointer = newContinent.get(); // Keep raw pointer for map ownership transfer

        mapOutput.addContinent(rawPointer); // Transfer ownership to map
        newContinent.release(); // Release unique_ptr ownership
        context.continentMap[continentName] = rawPointer; // Index for fast lookup during parsing
    }

    /**
     * @brief Parses a territory definition line from a map file
     * @param line Raw line from map file containing territory definition
     * @param context Parsing context for state management and deferred resolution
     * @param mapOutput Target map to add the parsed territory to
     * 
     * @throws std::runtime_error if line format is invalid (< 4 tokens required)
     * 
     * @details Expected format: "TerritoryName, X, Y, Continent, Adjacent1, Adjacent2, ..."
     * - Creates territory with auto-generated ID
     * - Handles forward references for adjacencies via waiting list
     * - Coordinates (X, Y) are parsed but currently unused
     * - Uses RAII for exception safety during construction
     * 
     * @pre line must contain at least territory name, coordinates, and continent
     * @post New territory added to map with continent membership and adjacencies
     */
    static void parseTerritories(const string& line, ParseContext& context, Map& mapOutput){
        // Parse comma-separated tokens from territory definition line
        vector<string> tokens = csvParse(line);
        
        if (tokens.size() < 4) throw runtime_error("Invalid territory line: " + line);

        string territoryName = tokens[0];
    
        // Create territory using RAII for exception safety during construction
        auto newTerritory = std::make_unique<Territory>(context.nextTerritoryId++, territoryName);
        Territory* rawPointer = newTerritory.get(); // Keep raw pointer for map ownership transfer

        mapOutput.addTerritory(rawPointer); // Transfer ownership to map
        newTerritory.release(); // Release unique_ptr ownership
        context.territoryMap[territoryName] = rawPointer; // Index for fast lookup during parsing
        // Check if any territories were waiting to connect to this one
        if(context.waitingTerritories.find(territoryName) != context.waitingTerritories.end()) {
            for(Territory* waitingTerritory : context.waitingTerritories[territoryName]) {
                waitingTerritory->addAdjacent(rawPointer);
            }
            context.waitingTerritories.erase(territoryName); // Clear the waiting list for this territory
        }
    
        // ignore X, Y for now
    
        // Get the continent name
        string continentName = tokens[3];
        if (context.continentMap.find(continentName) != context.continentMap.end()) {
            rawPointer->addContinent(context.continentMap[continentName]);
            context.continentMap[continentName]->addTerritory(rawPointer);
        }
    
        // Add connections to adjacent territories
        for (size_t i = 4; i < tokens.size(); ++i) {
            string adjacentName = tokens[i];
            if (context.territoryMap.find(adjacentName) != context.territoryMap.end()) {
                rawPointer->addAdjacent(context.territoryMap[adjacentName]);
            } else {
                // Adjacent territory not yet created, add to waiting list
                context.waitingTerritories[adjacentName].push_back(rawPointer);
            }
        }
    }

    /**
     * @brief Performs depth-first search to verify connectivity of territory subgraph
     * @param start Starting territory for the traversal
     * @param includedTerritories Set of territories that must be reachable from start
     * @return true if all territories in includedTerritories are reachable from start
     * 
     * @details This function implements iterative DFS using a stack to avoid recursion
     * stack overflow for large maps. It verifies that the given territory subgraph
     * forms a connected component.
     * 
     * @complexity Time: O(V + E) where V is territories, E is adjacencies
     *            Space: O(V) for visited set and DFS stack
     * 
     * @pre start must be non-null and in includedTerritories set
     * @post No side effects - function is read-only
     */
    static bool isConnectedDFS( const Territory* start,
                                const unordered_set<const Territory*>& includedTerritories){
        
        // Validate input parameters and handle edge cases
        if (!start || includedTerritories.empty() ) return false;
        if (includedTerritories.find(start) == includedTerritories.end()) return false;
    
        // Track visited territories to avoid cycles and count reachable nodes
        unordered_set<const Territory*> visited;
        visited.reserve(includedTerritories.size());

        // Iterative DFS stack to avoid recursion depth limits
        vector<const Territory*> stack;
        stack.reserve(includedTerritories.size());
        stack.push_back(start);

        // Execute iterative depth-first search traversal
        while (!stack.empty()) {
            const Territory* currentTerritory = stack.back();
            stack.pop_back();

           // Skip territories outside the subgraph of interest
           if (includedTerritories.find(currentTerritory) == includedTerritories.end()) continue;

            // Skip already processed territories to avoid infinite loops
            if (visited.find(currentTerritory) != visited.end()) continue;
            visited.insert(currentTerritory); // Mark current territory as visited

            // Add all unvisited neighbors within subgraph to exploration stack
            for (const Territory* adjacent : currentTerritory->getAdjacents()) {
                // Only explore valid, included, and unvisited adjacent territories
                if (adjacent 
                    && includedTerritories.find(adjacent) != includedTerritories.end() 
                    && visited.find(adjacent) == visited.end()) 
                {
                    stack.push_back(adjacent); // Queue for future exploration
                }
            }
        }

        // Success: all territories in subgraph are reachable from start
        return visited.size() == includedTerritories.size(); 
    }

    static bool validateContinent(const Continent* continent){
        if (!continent) return false;

        // Check if all territories in the continent are connected
        const vector<Territory*>& territories = continent->getTerritories();
        if (territories.empty()) return false;

        // Build a set of territory pointers for quick lookup
        unordered_set<const Territory*> territorySet;
        territorySet.reserve(territories.size());
        for (const Territory* t : territories) {
            if (!t) return false; // Null pointer check
            territorySet.insert(t);
        }

        // Perform DFS from the first territory in the continent
        return isConnectedDFS(territories[0], territorySet);
    }

    static bool validateAllTerritories(const Map& map) {
        const vector<Territory*>& allTerritories = map.getTerritories();
        if (allTerritories.empty()) return false;

        // Build a set of all territory pointers for quick lookup
        unordered_set<const Territory*> territorySet;
        territorySet.reserve(allTerritories.size());
        for (const Territory* t : allTerritories) {
            if (!t) return false; // Null pointer check
            territorySet.insert(t);
        }

        // Perform DFS from the first territory in the map
        return isConnectedDFS(allTerritories[0], territorySet);
    }
}

// ======================= Territory =======================
/** @brief Default constructor creates empty territory with zero values */
Territory::Territory() : id(0), name(""), continents(), owner(nullptr), armies(0) {}

/**
 * @brief Copy constructor with intentional shallow copy of relationships
 * @param other Territory to copy from
 * 
 * @details This constructor performs a shallow copy by design:
 * - Copies: id, name, owner, armies (basic data)
 * - Clears: continents and adjacentTerritories vectors
 * 
 * @rationale The Map class copy constructor rebuilds all territory-continent
 * and territory-territory relationships to maintain consistency across the
 * entire map structure. Individual territory copying with deep relationship
 * copying would create dangling pointers.
 * 
 * @see Map::Map(const Map& other) for relationship reconstruction
 */
Territory::Territory(const Territory& other)
    : id(other.id),
      name(other.name),
      continents(),              // Intentionally empty - Map copy will rebuild continent links
      owner(other.owner),        // Non-owning pointer - safe to shallow copy
      armies(other.armies),
      adjacentTerritories()      // Intentionally empty - Map copy will rebuild adjacencies
{}

Territory::Territory(int id, const string& name, Player* owner, int armies)
    : id(id), name(name), continents(), owner(owner), armies(armies) {}

Territory::Territory(int id, const string& name)
    : id(id), name(name), continents(), owner(nullptr), armies(0) {}

/** @brief Destructor - no cleanup needed as Territory doesn't own its relationships */
Territory::~Territory() {}

/**
 * @brief Copy assignment operator with intentional shallow copy of relationships
 * @param other Territory to assign from
 * @return Reference to this territory for chaining
 * 
 * @details This assignment operator performs shallow copy by design:
 * - Assigns: id, name, owner, armies (basic data)
 * - Clears: continents and adjacentTerritories vectors
 * 
 * @rationale Maintains consistency with copy constructor behavior.
 * Deep copying relationships would create inconsistent state where territories
 * reference objects from different map instances.
 * 
 * @see Territory::Territory(const Territory& other) for similar rationale
 */
Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        owner = other.owner;
        armies = other.armies;

        // Clear existing relationships to maintain consistency
        // Map-level operations will rebuild these relationships appropriately
        continents.clear(); // Remove old continent memberships
        adjacentTerritories.clear(); // Remove old territory adjacencies
    }
    return *this;
}

ostream& operator<<(ostream& os, const Territory& territory) {
    os << "Territory: " << territory.name << " (ID: " << territory.id << ")\n";
    os << "  Continents: ";
    if (territory.continents.empty()) {
        os << "None";
    } else {
        bool first = true;
        for (const Continent* c : territory.continents) {
            if (!first) os << ", ";
            os << (c ? c->getName() : "Null");
            first = false;
        }
    }
    os << "\n";
    os << "  Owner: " << (territory.owner ? territory.owner->getPlayerName() : "None") << "\n";
    os << "  Armies: " << territory.armies << "\n";
    os << "  Adjacents: ";
    bool first = true;
    for (const Territory* t : territory.getAdjacents()) {
        if (!t) continue;
        if (!first) os << ", ";
        os << t->getName();
        first = false;
    }
    if (first) os << "(none)";
    os << "\n";
    return os;
}

int Territory::getId() const { return id; }
string Territory::getName() const { return name; }
Player* Territory::getOwner() const { return owner; }
const vector<Continent*>& Territory::getContinents() const { return continents; }
void Territory::addContinent(Continent* c) { 
    if (c) continents.push_back(c); 
}
void Territory::clearContinents() { continents.clear(); }
int Territory::getArmies() const { return armies; }
void Territory::setOwner(Player* newOwner) { owner = newOwner; }
void Territory::setArmies(int newArmies) { armies = newArmies; }
void Territory::addArmies(int additionalArmies) { armies += additionalArmies; }
void Territory::removeArmies(int removedArmies) { armies -= removedArmies; }
void Territory::addAdjacent(Territory* t) { adjacentTerritories.push_back(t); }
void Territory::clearAdjacents() { adjacentTerritories.clear(); }

/** Check if this territory is adjacent to another territory */
bool Territory::isAdjacentTo(const Territory* t) const {
    if (t == nullptr) {
        return false;
    }

    // Check if the territory is in the adjacent list
    const int idToFind = t->getId(); // Compare by ID to avoid pointer issues
    return any_of(adjacentTerritories.begin(), adjacentTerritories.end(),
                       [idToFind](const Territory* adj) {
                           return adj != nullptr && adj->getId() == idToFind;
                       });
}
const vector<Territory*>& Territory::getAdjacents() const { return adjacentTerritories; }

// ======================= Continent =======================
Continent::Continent() : id(0), name(""), bonus(0), territories() {}

/** Copy constructor does not deep copy territories
 This is intentional as Map copy constructor will rebuild these links */
Continent::Continent(const Continent& other)
    : id(other.id), name(other.name), bonus(other.bonus), territories() {
}

Continent::Continent(int id, const string& name)
    : id(id), name(name), bonus(0), territories() {}

Continent::Continent(int id, const string& name, int bonus)
    : id(id), name(name), bonus(bonus), territories() {}

Continent::~Continent() {}

/** Copy assignment operator does not deep copy territories
 This is intentional as Map copy assignment will rebuild these links */
Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        bonus = other.bonus;
        territories.clear(); // Clear existing territories
        // No deep copy of territories as Map copy will rebuild these links
    }
    return *this;
}

int Continent::getId() const { return id; }
string Continent::getName() const { return name; }
int Continent::getBonus() const { return bonus; }
void Continent::setBonus(int bonus) { this->bonus = bonus; }
void Continent::addTerritory(Territory* territory) { territories.push_back(territory); }
void Continent::clearTerritories() { territories.clear(); } // Clear existing territories
const vector<Territory*>& Continent::getTerritories() const { return territories; }

/** ostream overload for easy printing of continent details */
ostream& operator<<(ostream& os, const Continent& continent) {
    os << "Continent: " << continent.name << " (ID: " << continent.id << ", Bonus: " << continent.bonus << ")\n";
    os << "  Territories: ";
    if(continent.territories.empty()) {
        os << "None";
    }
    else {
        bool first = true;
        for (const Territory* territory : continent.territories) {
            if (!first) os << ", ";
            os << territory->getName();
            first = false;
        }
    }
    os << "\n";
    return os;
}

// ======================= Map =======================
Map::Map() : territories(), continents() {}

/** Copy constructor deep copy of territories and continents */
Map::Map(const Map& other) : territories(), continents() {

    // Build lookup of continent pointers in map for easy reference
    unordered_map<const Continent*, Continent*> continentMap; // Maps original continent pointers to new cloned pointers
    continents.reserve(other.continents.size()); // Reserve space to avoid multiple allocations
    
    // Clone continents name and id first
    for (const Continent* c : other.continents) {
        if (c) {
            Continent* newContinent = new Continent(c->getId(), c->getName());
            continents.push_back(newContinent);
            continentMap[c] = newContinent; // record in map for looking up later
        }
    }

    //build lookup of territory pointers in map
    unordered_map<const Territory*, Territory*> territoryMap; // Maps original territory pointers to new cloned pointers
    territoryMap.reserve(other.territories.size()); // Reserve space to avoid multiple allocations
    territories.reserve(other.territories.size()); // Reserve space to avoid multiple allocations

    // Clone basic territory info next
    for (const Territory* t : other.territories) {
        if (t) {
            Territory* newTerritory = new Territory(t->getId(), t->getName(), t->getOwner(), t->getArmies());
            territories.push_back(newTerritory);
            territoryMap[t] = newTerritory; // record in map for looking up later
        }
    }
    
    // Rebuild continent membership of territories and continents and adjacency lists
    for (Territory* oldTerritory : other.territories) {
        if (!oldTerritory) continue;
        Territory* newTerritory = territoryMap[oldTerritory]; // Get the corresponding new territory
        if (!newTerritory) continue;
        // Rebuild continent membership
        for (const Continent* oldContinent : oldTerritory->getContinents()) {
            if (continentMap.find(oldContinent) != continentMap.end()) {
                Continent* newContinent = continentMap[oldContinent]; // Get the corresponding new continent
                if (!newContinent) continue;
                newTerritory->addContinent(newContinent); // Add continent to territory
                newContinent->addTerritory(newTerritory); // Add territory to continent
            }
        }
        // Rebuild adjacency list
        for (const Territory* oldAdjacent : oldTerritory->getAdjacents()) {
            if (territoryMap.find(oldAdjacent) != territoryMap.end()) {
                Territory* newAdjacent = territoryMap[oldAdjacent];
                newTerritory->addAdjacent(newAdjacent);
            }
        }
    }
}

/** Destructor to clean up dynamically allocated territories and continents */
Map::~Map() {
    // Clean up all dynamically allocated territories
    for (Territory* territory : territories) {
        delete territory;  // Free each Territory object
    }
    territories.clear();  // Clear the vector
    
    // Clean up all dynamically allocated continents  
    for (Continent* continent : continents) {
        delete continent;  // Free each Continent object
    }
    continents.clear();  // Clear the vector
}


/* Implement swap as a non-member function */
void swap(Map& a, Map& b) noexcept {
    using std::swap;
    swap(a.territories, b.territories);
    swap(a.continents,  b.continents);
}

/** Copy assignment operator using copy-and-swap idiom for strong exception safety */
Map& Map::operator=(Map other) { // note: pass by value
    swap(*this, other);          // now *this has the new graph; 'other' holds the old
    return *this;                // old graph is destroyed when 'other' goes out of scope
}

/**
 * @brief Adds a territory to the map's territory collection
 * @param t Raw pointer to territory (map takes ownership)
 * 
 * @warning Caller must not delete the territory pointer after calling this
 * @see Map::~Map() for cleanup logic
 */
void Map::addTerritory(Territory* t) { 
    territories.push_back(t); // Add territory to collection, taking ownership
}

/**
 * @brief Adds a continent to the map's continent collection
 * @param c Raw pointer to continent (map takes ownership)
 * 
 * @todo Refactor to use unique_ptr<Continent> for better memory safety
 *       This would eliminate manual delete calls in destructor and
 *       provide automatic cleanup on exceptions
 * 
 * @warning Caller must not delete the continent pointer after calling this
 * @see Map::~Map() for cleanup logic
 */
void Map::addContinent(Continent* c) { 
    continents.push_back(c); // Add continent to collection, taking ownership
}
const vector<Territory*>& Map::getTerritories() const { return territories; }
const vector<Continent*>& Map::getContinents() const { return continents; }

void Map::clear() {
    // Helper method to clean up all objects
    for (Territory* territory : territories) {
        delete territory;
    }
    territories.clear();
    
    for (Continent* continent : continents) {
        delete continent;
    }
    continents.clear();
}

/**
 * @brief Validates that the map satisfies all game rules and constraints
 * @return true if map is valid for gameplay, false otherwise
 * 
 * @details Performs comprehensive validation checking:
 * 1. Map connectivity: All territories must be reachable from any territory
 * 2. Continent connectivity: Each continent must form a connected subgraph
 * 3. Territory-continent membership: Each territory belongs to exactly one continent
 * 
 * @complexity O(V + E) for connectivity checks where V=territories, E=adjacencies
 * 
 * @note This method is const and performs no modifications to the map
 * @see validateAllTerritories() and validateContinent() for specific checks
 */
bool Map::validate() const {
    // Rule 1: Entire map must form a connected graph
    if(!validateAllTerritories(*this)) return false;

    // Rule 2: Each continent must be a connected subgraph
    for(const Continent* continent : continents) {
        if(!validateContinent(continent)) return false;
    }

    // Rule 3: Each territory must belong to exactly one continent
    for(const Territory* territory : territories) {
        if(territory->getContinents().empty()) return false;
        if(territory->getContinents().size() != 1) return false; // Exactly one continent required
    }

    return true; // All validations passed
}
ostream& operator<<(ostream& os, const Map& map) {
    os << "Map Overview:\n";
    if (map.continents.empty() && map.territories.empty()) {
        os << "No continents or territories available.\n";
    }
    if(map.continents.empty()) {
        os << "No continents available.\n";
    } else {
        os << "----------------------\n";
        os << "Continent Details:\n";
        os << "----------------------\n";
        for (const Continent* continent : map.continents) {
            os << *continent;
        }
        if(map.territories.empty()) {
            os << "No territories available.\n";
        }
        else {
            os << "\n";
            os << "----------------------\n";
            os << "Territories Details:\n";
            os << "----------------------\n";
            for (const Territory* territory : map.territories) {
                os << *territory;
            }
        }
    }
    return os;
}

// ======================= MapLoader =======================

MapLoader::MapLoader() {}

MapLoader::MapLoader(const MapLoader& other) { (void)other; }

MapLoader::~MapLoader() {}

MapLoader& MapLoader::operator=(const MapLoader& other) {
    if (this != &other) {
        // TODO: Copy any member variables if added later
        (void)other; // Suppress unused parameter warning
    }
    return *this;
}

ostream& operator<<(ostream& os, const MapLoader& ml) {
    (void)ml; // Suppress unused parameter warning
    os << "MapLoader";
    return os;
}

/** Get list of map files in the directory */
vector<string> MapLoader::getMapFiles() {
    vector<string> mapFiles;

    // List and sort .map files
    for (const fs::path& p : listMapFiles(fs::path(MAP_PATH))) {
        mapFiles.push_back(p.string());
    }

    return mapFiles;
}

/** Display map files from the list */
void MapLoader::printMapFiles(const vector<string>& mapFiles) {
    if (mapFiles.empty()) {
        throw runtime_error("No map files found.");
    }
    
    cout << "Available map files:" << endl;
    for (size_t i = 0; i < mapFiles.size() + 1; ++i) {
        if(i == mapFiles.size()) {
            cout << i + 1 << ". Exit" << endl;
            break;
        }
        fs::path p{mapFiles[i]}; // Extract filename from full path
        cout << i + 1 << ". " << p.filename().string() << endl; // Display only the filename
    }
}

/** Load a map from a .map file */
bool MapLoader::loadMap(const string& filename, Map& mapOutput) {
    fs::path p = filename;

    ifstream mapInput(p);
    if (!mapInput) {
        throw runtime_error("Cannot Open: " + p.string());
    }

    MapLoader::parseMapFileSections(mapInput, mapOutput);
    mapInput.close();

    return true;
}

/** Helper function to parse the map file */
void MapLoader::parseMapFileSections(istream& mapInput, Map& mapOutput) {
    MapFileSections currentSection = MapFileSections::None;

    ParseContext context;  // Local variable on the stack

    // Read the file line by line
    string line;
    while(getline(mapInput, line)){
        string trimmedLine = trim(line); // Trim whitespace from the line

        // Skip empty lines and comments
        if(trimmedLine.empty() ||
           trimmedLine[0] == ';' ||
           trimmedLine[0] == '#') continue;

        // Detect section headers
        if(trimmedLine.front() == '[' && trimmedLine.back() == ']'){
            string section = trimmedLine.substr(1, trimmedLine.size()-2);

            if (section == "Map")               currentSection = MapFileSections::Map;
            else if (section == "Continents")   currentSection = MapFileSections::Continents;
            else if(section == "Territories")   currentSection = MapFileSections::Territories;
            else                                currentSection = MapFileSections::None;
            continue;
        }

        // Process section content here based on currentSection
        switch(currentSection) {
            case MapFileSections::Map:
                continue; // Ignore metadata parse in [Map] section   
                break;
            case MapFileSections::Continents:
                parseContinents(trimmedLine, context, mapOutput);
                break;
            case MapFileSections::Territories:
                parseTerritories(trimmedLine, context, mapOutput);
                break;
            case MapFileSections::None:
                // Unknown section, skip
                break;
        }
    }
}

