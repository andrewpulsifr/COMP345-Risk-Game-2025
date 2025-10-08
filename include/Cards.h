#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cstdlib>

// Forward declarations
class Card;
class Deck;
class Hand;
class Player;

// Deck class holds a collection of cards.
class Deck {
    public:
        Deck();
        Deck(Deck const &deck); // Copy constructor for Deck.
        Deck& operator=(const Deck& other); // Assignment operator for Deck.
        void addCard(Card* card);
        void removeCard(Card* card);
        std::vector<Card*> getCardsOnDeck() const;
        std::string draw(Hand &hand);
        void showDeck();
        ~Deck(); // Destructor for Card*.
    private:
        std::vector<Card*> cardsOnDeck;
    
};


class Card {
    public:
        enum typeOfCard { // The different types a card can be.
            Bomb,
            Reinforcement,
            Blockade,
            Airlift,
            Diplomacy
        };

        Card(typeOfCard cardType); // Constructor.
        Card(Card const & card); // Copy constructor for Card.
        Card& operator=(const Card& other); // Assignment operator for Card.
        typeOfCard getCard() const;
        void play(Player& player, Deck &deck, Hand &hand); // Cards can be played, creates an order and adds it to the player's list of orders, then returns the card to the deck.

    private:
        typeOfCard card;
};


// Hand class holds cards that have been drawn from the deck.
class Hand {
    public:
        Hand();
        Hand(Hand const &hand); // Copy constructor for Hand.
        Hand& operator=(const Hand& other); // Assignment operator for Hand.
        std::vector<Card*> getCardsOnHand() const;
        void addCard(Card* card);
        void removeCard(Card* card);
        void showHand();
        ~Hand(); // Destructor.

    private:
        std::vector<Card*> cardsOnHand;
};


// Stream overloaders for each class: Cards, Hand, and Deck.
// Free helper to convert enum to string
std::string cardToString(Card::typeOfCard cardType);

std::ostream & operator << (std::ostream &os, const Card &card); // stream overloading for Cards.
std::ostream & operator << (std::ostream &os, const Hand &hand); // stream overloading for Hand.
std::ostream & operator << (std::ostream &os, const Deck &deck); // stream overloading for Deck.
