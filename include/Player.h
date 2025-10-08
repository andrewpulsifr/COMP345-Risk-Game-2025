/**
 * @file Player.h
 * @brief Assignment 1 – Part 2 (Warzone): Player, attack/defending
 *
 * @details
 *
 * @note All Part-2 classes/functions live in this duo (Player.h/Player.cpp). The driver `testPlayers()` is
 *       implemented in PlayerDriver.cpp and creates player object, printing them and creating dummy lists
 */

#pragma once
#include <vector>
#include <string>
#include <ostream>

class Territory;
class Hand;
class Order;
class OrdersList;

class Player {
public:
	Player(); //Default Constructor
	Player(const Player& copyPlayer); //Copy Constructor
	Player(std::string name); //Parametrized Constructor
	~Player(); //destructor

    std::string getPlayerName() const; //Getter for playerName
	Hand* getPlayerHand() const;

    Player& operator=(const Player& other);
	void addPlayerTerritory(Territory* territory); //Adds to Player's Owned Territories
	void removePlayerTerritory(Territory* territory); //Removes from Player's Owned Territories
    std::vector<Territory*> getOwnedTerritories() const; //Returns a vector containing every owned territory

	std::vector<Territory*> toDefend(); //Returns a vector containing every attackable territory of player's
	std::vector<Territory*> toAttack(); //Returns a vector containing every territory player can attack

	void issueOrder(Order* orderIssued); //Adds an Order to be issued
	
private:
	std::string playerName; //Player's Name
	Hand* playerHand; //Player's Hand
	std::vector<Territory*> ownedTerritories; //List of Territories currently owned by Player
	OrdersList* orders_; //List of orders issued by Player

friend std::ostream& operator<<(std::ostream& os, const Player& player);
};



void testPlayers();