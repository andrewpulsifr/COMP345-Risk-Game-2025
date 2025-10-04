#include "../include/Map.h"
#include "../include/Player.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


/** Directory containing map files and helper to trim whitespace */
namespace {
    const string MAP_PATH = "assets/maps"; // Directory containing map files

    struct ParseContext {
        unordered_map<string, Continent*> continentMap; // Fast lookup of name to Continent*
        unordered_map<string, Territory*> territoryMap; // Fast lookup of name to Territory*
        // Territories waiting for adjacency resolution
        unordered_map<string, vector<Territory*>> waitingTerritories;
        int currentContinentId = 0; 
        int currentTerritoryId = 0;
    };

    // inline helper to trim whitespace
    inline string trim(string_view rawLine) {
        constexpr char whitespace[] = " \t\n\r\f\v";

        // Find first non-whitespace character
        const string::size_type first = rawLine.find_first_not_of(whitespace);
        if (first == string_view::npos) { return string(); }

        // Find last non-whitespace character
        const string::size_type last  = rawLine.find_last_not_of(whitespace);

        // return trimmed string
        return string(rawLine.substr(first, last - first + 1));
    }

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
        result.push_back(string(str.substr(start))); // Add the last token
        return result;
    }

    // List all .map files in a directory sorted by filename
    static vector<fs::path> listMapFiles(const fs::path& dir) {
        vector<fs::path> mapFiles;

        // Check if the directory exists and is a directory
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            throw runtime_error("Map directory does not exist or is not a directory: " + dir.string());
        }

        // Iterate through the directory and find all .map files
        for (const fs::directory_entry& fileEntry : fs::directory_iterator(dir)) {
            if (fileEntry.is_regular_file() && fileEntry.path().extension() == ".map") {
                mapFiles.push_back(fileEntry.path());
            }
        }

        // Sort the map files by filename
        sort(mapFiles.begin(), mapFiles.end(), [](const fs::path& a, const fs::path& b){
            return a.filename().string() < b.filename().string();
        });

        return mapFiles; // Return the sorted list of map files
    }

    /** Helper function to parse continents from the map file */
    static void parseContinents(string& line, ParseContext& context, Map& mapOutput){
        if(line.find('=') == string::npos) {
            throw runtime_error("Invalid continent line: " + line);
        }
        string continent = trim(line.substr(0, line.find('=')));
        Continent* newContinent = new Continent(context.currentContinentId++, continent);
        context.continentMap[continent] = newContinent; // Map continent name to Continent*
        mapOutput.addContinent(newContinent); // Add continent with unique ID
        // ignore the value after '=' for now
    }

    static void parseTerritories(string& line, ParseContext& context, Map& mapOutput){
        // Expected format: TerritoryName, X, Y, Continent, Adjacent1, Adjacent2, ...
        vector<string> tokens = csvParse(line);
        
        if (tokens.size() < 4) {
            throw runtime_error("Invalid territory line: " + line);
        }

        // Create the territory and add to map
        string territoryName = trim(tokens[0]);
        Territory* newTerritory = new Territory(context.currentTerritoryId++, territoryName);
        mapOutput.addTerritory(newTerritory);
        context.territoryMap[territoryName] = newTerritory;

        // Check if any territories were waiting to connect to this one
        if(context.waitingTerritories.find(territoryName) != context.waitingTerritories.end()) {
            for(Territory* waitingTerritory : context.waitingTerritories[territoryName]) {
                waitingTerritory->addAdjacent(newTerritory);
            }
            context.waitingTerritories.erase(territoryName); // Clear the waiting list for this territory
        }

        // ignore X, Y for now

        // Get the continent name
        string continentName = trim(tokens[3]);
        if (context.continentMap.find(continentName) != context.continentMap.end()) {
            newTerritory->addContinent(context.continentMap[continentName]);
            context.continentMap[continentName]->addTerritory(newTerritory);
        }

        // Add connections to adjacent territories
        for (size_t i = 4; i < tokens.size(); ++i) {
            string adjacentName = trim(tokens[i]);
            if (context.territoryMap.find(adjacentName) != context.territoryMap.end()) {
                newTerritory->addAdjacent(context.territoryMap[adjacentName]);
            } else {
                // Adjacent territory not yet created, add to waiting list
                context.waitingTerritories[adjacentName].push_back(newTerritory);
            }
        }
    }

    static bool isConnectedDFS( const Territory* start,
                                const unordered_set<const Territory*>& includedTerritories){
        
        // Check if graph is empty or start is null or start not in included set
        if (!start || includedTerritories.empty() ) return false;
        if (includedTerritories.find(start) == includedTerritories.end()) return false;
    

        // Track visited territories
        unordered_set<const Territory*> visited;
        visited.reserve(includedTerritories.size());

        // Stack for DFS
        vector<const Territory*> stack;
        stack.reserve(includedTerritories.size());
        stack.push_back(start);

        // Perform DFS
        while (!stack.empty()) {
            const Territory* currentTerritory = stack.back();
            stack.pop_back();

           // Only process nodes inside the allowed included subgraph
           if (includedTerritories.find(currentTerritory) == includedTerritories.end()) continue;

            // Check for already visited
            if (visited.find(currentTerritory) != visited.end()) continue;
            visited.insert(currentTerritory); // Mark as visited

            // Insert neighbors onto stack
            for (const Territory* adjacent : currentTerritory->getAdjacents()) {
                // Check if adjacent is valid and not visited
                if (adjacent 
                    && includedTerritories.find(adjacent) != includedTerritories.end() 
                    && visited.find(adjacent) == visited.end()) 
                {
                    stack.push_back(adjacent); // Add to stack for further exploration
                }
            }
        }

        // All included territories must be visited
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
Territory::Territory() : id(0), name(""), continents(), owner(nullptr), armies(0) {}

Territory::Territory(const Territory& other)
    : id(other.id), name(other.name), continents(other.continents), 
      owner(other.owner), armies(other.armies),
      adjacentTerritories(other.adjacentTerritories) {}

Territory::Territory(int id, const string& name, Player* owner, int armies)
    : id(id), name(name), continents(), owner(owner), armies(armies) {}

Territory::Territory(int id, const string& name)
    : id(id), name(name), continents(), owner(nullptr), armies(0) {}

Territory::~Territory() {}


Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        continents = other.continents;
        owner = other.owner;
        armies = other.armies;
        adjacentTerritories = other.adjacentTerritories;
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
bool Territory::isAdjacentTo(const Territory* t) const {
    if (t == nullptr) {
        return false;
    }
    const int idToFind = t->getId();
    return any_of(adjacentTerritories.begin(), adjacentTerritories.end(),
                       [idToFind](const Territory* adj) {
                           return adj != nullptr && adj->getId() == idToFind;
                       });
}
const vector<Territory*>& Territory::getAdjacents() const { return adjacentTerritories; }

// ======================= Continent =======================
Continent::Continent() : id(0), name(""), territories() {}

Continent::Continent(const Continent& other)
    : id(other.id), name(other.name), territories(other.territories) {}

Continent::Continent(int id, const string& name)
    : id(id), name(name), territories() {}

Continent::~Continent() {}

Continent& Continent::operator=(const Continent& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        territories = other.territories;
    }
    return *this;
}

int Continent::getId() const { return id; }
string Continent::getName() const { return name; }
void Continent::addTerritory(Territory* territory) { territories.push_back(territory); }
const vector<Territory*>& Continent::getTerritories() const { return territories; }

ostream& operator<<(ostream& os, const Continent& continent) {
    os << "Continent: " << continent.name << " (ID: " << continent.id << ")\n";
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
Map::Map(const Map& other)
    : territories(other.territories), continents(other.continents) {}
Map::~Map() {}
Map& Map::operator=(const Map& other) {
    if (this != &other) {
        territories = other.territories;
        continents = other.continents;
    }
    return *this;
}
void Map::addTerritory(Territory* territory) { territories.push_back(territory); }
void Map::addContinent(Continent* continent) { continents.push_back(continent); }
const vector<Territory*>& Map::getTerritories() const { return territories; }
const vector<Continent*>& Map::getContinents() const { return continents; }  
bool Map::validate() const {
    // 1) map connected
    if(!validateAllTerritories(*this)) return false;

    // 2) each continent is connected subgraph
    for(const Continent* continent : continents) {
        if(!validateContinent(continent)) return false;
    }

    // 3) each territory in exactly one continent
    for(const Territory* territory : territories) {
        if(territory->getContinents().empty()) return false;
        if(territory->getContinents().size() != 1) return false; // Exactly one continent
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
    for (size_t i = 0; i <= mapFiles.size(); ++i) {
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