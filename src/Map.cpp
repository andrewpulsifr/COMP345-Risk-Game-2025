#include "../include/Map.h"
#include <ostream>

// ======================= Territory =======================
Territory::Territory() : id(0), name(""), owner(nullptr), continent(nullptr), armies(0) {}

Territory::Territory(int id, const std::string& name, Player* owner, int armies)
    : id(id), name(name), owner(owner), continent(nullptr), armies(armies) {}

Territory::Territory(const Territory& other)
    : id(other.id), name(other.name), owner(other.owner),
      continent(other.continent), armies(other.armies),
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

std::ostream& operator<<(std::ostream& os, const Territory& territory) {
    os << "Territory: " << territory.name << " (ID: " << territory.id << ")";
    return os;
}

int Territory::getId() const { return id; }
std::string Territory::getName() const { return name; }
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
    const int idToFind = t->getId();
    return std::any_of(adjacentTerritories.begin(), adjacentTerritories.end(),
                       [idToFind](const Territory* adj) { return adj->getId() == idToFind; });
}
const std::vector<Territory*>& Territory::getAdjacents() const { return adjacentTerritories; }

// ======================= Continent =======================
Continent::Continent() : id(0), name(""), territories() {}

Continent::Continent(int id, const std::string& name)
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
std::string Continent::getName() const { return name; }
void Continent::addTerritory(Territory* territory) { territories.push_back(territory); }
const std::vector<Territory*>& Continent::getTerritories() const { return territories; }

std::ostream& operator<<(std::ostream& os, const Continent& continent) {
    // TO DO: Implement detailed output
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
const std::vector<Territory*>& Map::getTerritories() const { return territories; }
const std::vector<Continent*>& Map::getContinents() const { return continents; }  
bool Map::validate() const {
    // TO DO: Implement validation logic
    return true; // Placeholder
}
std::ostream& operator<<(std::ostream& os, const Map& map) {
    // TO DO: Implement detailed output
    return os;
}

