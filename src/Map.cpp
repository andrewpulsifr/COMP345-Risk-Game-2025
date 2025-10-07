#include "../include/Map.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


/** Directory containing map files and helper to trim whitespace */
namespace {
    const string MAP_PATH = "assets/maps"; // Directory containing map files

    // inline helper to trim whitespace
    inline void trim(string& rawLine) {
        constexpr char whitespace[] = " \t\n\r\f\v";
        const auto first = rawLine.find_first_not_of(whitespace);
        if (first == string::npos) { rawLine.clear(); return; }
        const auto last  = rawLine.find_last_not_of(whitespace);
        rawLine.erase(last + 1);
        rawLine.erase(0, first);
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
}

// ======================= Territory =======================
Territory::Territory() : id(0), name(""), continent(nullptr), owner(nullptr), armies(0) {}

Territory::Territory(int id, const string& name, Player* owner, int armies)
    : id(id), name(name), continent(nullptr), owner(owner), armies(armies) {}

Territory::Territory(const Territory& other)
    : id(other.id), name(other.name), continent(other.continent), 
      owner(other.owner), armies(other.armies),
      adjacentTerritories(other.adjacentTerritories) {}

Territory::~Territory() {}

Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        owner = other.owner;
        continent = other.continent;
        armies = other.armies;
        adjacentTerritories = other.adjacentTerritories;
    }
    return *this;
}

ostream& operator<<(ostream& os, const Territory& territory) {
    os << "Territory: " << territory.name << " (ID: " << territory.id << ")";
    return os;
}

int Territory::getId() const { return id; }
string Territory::getName() const { return name; }
Player* Territory::getOwner() const { return owner; }
Continent* Territory::getContinent() const { return continent; }
void Territory::setContinent(Continent* c) { continent = c; }
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
                       [idToFind](const Territory* adj) { return adj != nullptr && adj->getId() == idToFind; });
}
const vector<Territory*>& Territory::getAdjacents() const { return adjacentTerritories; }

// ======================= Continent =======================
Continent::Continent() : id(0), name(""), territories() {}

Continent::Continent(int id, const string& name)
    : id(id), name(name), territories() {}

Continent::Continent(const Continent& other)
    : id(other.id), name(other.name), territories(other.territories) {}

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
    // TO DO: Implement detailed output
    (void)continent; // Suppress unused parameter warning
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
    // TO DO: Implement validation logic
    return true; // Placeholder
}
ostream& operator<<(ostream& os, const Map& map) {
    // TO DO: Implement detailed output
    (void)map; // Suppress unused parameter warning
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
    for (size_t i = 0; i < mapFiles.size(); ++i) {
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

    MapLoader::parseMapFile(mapInput, mapOutput);
    return true;
}

/** Helper function to parse the map file */
bool MapLoader::parseMapFile(istream& mapInput, Map& mapOutput) {
    (void)mapOutput; // Suppress unused parameter warning - TODO: implement parsing
    MapFileSections currentSection = MapFileSections::None;

    unordered_map<int, Continent*> continentMap; // Map of continent ID to Continent*
    unordered_map<int, Territory*> territoryMap; // Map of territory ID to Territory*

    // Read the file line by line
    string currentLine;
    while(getline(mapInput, currentLine)){
        trim(currentLine); // Trim whitespace from the line

        // Skip empty lines and comments
        if(currentLine.empty() ||
           currentLine[0] == ';' ||
           currentLine[0] == '#') continue; 

        // Detect section headers
        if(currentLine.front() == '[' && currentLine.back() == ']'){
            string section = currentLine.substr(1,currentLine.size()-2);

            if (section == "Map")               currentSection = MapFileSections::Map;
            else if (section == "Continents")   currentSection = MapFileSections::Continents;
            else if(section == "Territories")   currentSection = MapFileSections::Territories;
            else                                currentSection = MapFileSections::None;
            continue;
        }

        // Process section content here based on currentSection
        switch(currentSection) {
            case MapFileSections::Map:
                // TODO: Parse map metadata
                break;
            case MapFileSections::Continents:
                // TODO: Parse continent data
                break;
            case MapFileSections::Territories:
                // TODO: Parse territory data
                break;
            case MapFileSections::None:
                // Unknown section, skip
                break;
        }
    }

    return true;
}
